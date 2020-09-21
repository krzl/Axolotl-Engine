// ReSharper disable CppClangTidyMiscMisplacedConst
#include "stdafx.h"
#include "TextureBinding.h"
#include "Common.h"

namespace axlt::vk {
	
	Array<TextureBinding::StagingBufferCopyInfo> TextureBinding::awaitingStagingBuffers;
	SparseArray<TextureBinding> TextureBinding::bindings;

	TextureBinding& TextureBinding::CreateBinding( const ResourceHandle<TextureResource>& texture ) {
		TextureBinding& binding = *(TextureBinding*) bindings.Emplace().element;
		bindingMap.Add( texture->GetInstanceId(), &binding );

		return binding;
	}

	void TextureBinding::UpdateBinding( const ResourceHandle<TextureResource>& texture ) {
		VkImageCreateInfo imageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			0,
			GetImageType( texture->dimension ),
			GetNativeTextureFormat( texture->format ),
			{
				texture->width,
				texture->height,
				texture->depth
			},
			texture->mipmapCount,
			texture->arrayCount,
			GetSampleCount( texture->sampleCount ),
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr,
			VK_IMAGE_LAYOUT_UNDEFINED
		};

		VkResult result = vkCreateImage( device, &imageCreateInfo, nullptr, &image );
		if( result != VK_SUCCESS ) {
			printf( "Could not create image\n" );
			return;
		}

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements( device, image, &memoryRequirements );

		if( !AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &imageMemory ) ) {
			return;
		}

		result = vkBindImageMemory( device, image, imageMemory, 0 );
		if( result != VK_SUCCESS ) {
			printf( "Could not bind image memory\n" );
			return;
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		
		VkBufferCreateInfo createInfo = {
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			nullptr,
			0,
			memoryRequirements.size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr
		};

		result = vkCreateBuffer( device, &createInfo, nullptr, &stagingBuffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not create staging buffer\n" );
			return;
		}

		vkGetBufferMemoryRequirements( device, stagingBuffer, &memoryRequirements );
		if( !AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &stagingBufferMemory ) ) {
			return;
		}

		result = vkBindBufferMemory( device, stagingBuffer, stagingBufferMemory, 0 );
		if( result != VK_SUCCESS ) {
			printf( "Could not bind staging buffer memory\n" );
			return;
		}
		
		void* data;
		result = vkMapMemory( device, stagingBufferMemory, 0, memoryRequirements.size, 0, &data );
		if( result != VK_SUCCESS ) {
			printf( "Could not map staging buffer memory\n" );
			return;
		}

		memcpy( data, texture->textureData.GetData(), texture->textureData.GetSize() );

		vkUnmapMemory( device, stagingBufferMemory );
		
		VkMappedMemoryRange mappedMemoryRange = {
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			stagingBufferMemory,
			0,
			VK_WHOLE_SIZE
		};

		result = vkFlushMappedMemoryRanges( device, 1, &mappedMemoryRange );
		if( result != VK_SUCCESS ) {
			printf( "Could not flush mapped memory of staging buffer\n" );
			return;
		}
		
		VkImageViewCreateInfo imageViewCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			image,
			GetImageViewType( texture->dimension ),
			imageCreateInfo.format,
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				texture->mipmapCount,
				0,
				texture->arrayCount
			}
		};
		
		result = vkCreateImageView( device, &imageViewCreateInfo, nullptr, &imageView );
		if( result != VK_SUCCESS ) {
			printf( "Could not create image view\n" );
			return;
		}
		
		VkSamplerCreateInfo samplerCreateInfo = {
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			nullptr,
			0,
			GetTextureFilter( texture->magFilter ),
			GetTextureFilter( texture->minFilter ),
			texture->mipmapFilterLinear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST,
			GetTextureAddressMode( texture->addressModeU ),
			GetTextureAddressMode( texture->addressModeV ),
			GetTextureAddressMode( texture->addressModeW ),
			texture->mipLodBias,
			texture->anisotropicFiltering > 1.0f,
			texture->anisotropicFiltering,
			VK_FALSE,
			VK_COMPARE_OP_ALWAYS,
			0.0f,
			0.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			VK_FALSE
		};

		result = vkCreateSampler( device, &samplerCreateInfo, nullptr, &sampler );
		if( result != VK_SUCCESS ) {
			printf( "Could not create sampler\n" );
			return;
		}

		awaitingStagingBuffers.Emplace( StagingBufferCopyInfo {
			image,
			stagingBuffer,
			texture->width,
			texture->height,
			texture->depth,
			imageCreateInfo.format,
			stagingBufferMemory,
			texture->mipmapCount,
			texture->arrayCount
		} );
	}

	void ImageTransition( const VkCommandBuffer commandBuffer, const VkImage image, const VkFormat format,
		const VkImageLayout srcLayout, const VkImageLayout destLayout, const uint8_t mipmapCount, const uint8_t arrayCount ) {

		VkImageMemoryBarrier barrier = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			0,
			0,
			srcLayout,
			destLayout,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			image,
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				mipmapCount,
				0,
				arrayCount
			}
		};

		VkPipelineStageFlags sourceStage = 0;
		VkPipelineStageFlags destinationStage = 0;

		if( srcLayout == VK_IMAGE_LAYOUT_UNDEFINED && destLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if( srcLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && destLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		vkCmdPipelineBarrier( commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier );
	}

	bool TextureBinding::HasAwaitingStagingBuffers() {
		return awaitingStagingBuffers.GetSize() != 0;
	}

	void TextureBinding::SubmitAwaitingStagingBuffers( const VkCommandBuffer commandBuffer ) {
		for( StagingBufferCopyInfo& copyInfo : awaitingStagingBuffers ) {
			VkBufferImageCopy imageCopyData = {
				0,
				0,
				0,
				{
					VK_IMAGE_ASPECT_COLOR_BIT,
					0, //TODO: Handle mipmaps
					0,
					copyInfo.arrayCount
				},
				{
					0,
					0,
					0
				},
				{
					copyInfo.width,
					copyInfo.height,
					copyInfo.depth
				}
			};

			ImageTransition( commandBuffer, copyInfo.image, copyInfo.format,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				copyInfo.mipmapCount, copyInfo.arrayCount );

			vkCmdCopyBufferToImage( commandBuffer, copyInfo.stagingBuffer, copyInfo.image,
									VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyData );

			ImageTransition( commandBuffer, copyInfo.image, copyInfo.format, 
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
				copyInfo.mipmapCount, copyInfo.arrayCount );
		}
	}

	void TextureBinding::CleanupAwaitingStagingBuffers() {
		for( StagingBufferCopyInfo& copyInfo : awaitingStagingBuffers ) {
			vkDestroyBuffer( device, copyInfo.stagingBuffer, nullptr );
			vkFreeMemory( device, copyInfo.stagingMemory, nullptr );
		}

		awaitingStagingBuffers.Clear();
	}

	void TextureBinding::Reset() {
		if( image != VK_NULL_HANDLE ) {
			vkDestroyImage( device, image, nullptr );
			image = VK_NULL_HANDLE;
		}
		if( imageView != VK_NULL_HANDLE ) {
			vkDestroyImageView( device, imageView, nullptr );
			imageView = VK_NULL_HANDLE;
		}
		if( sampler != VK_NULL_HANDLE ) {
			vkDestroySampler( device, sampler, nullptr );
			sampler = VK_NULL_HANDLE;
		}
		if( imageMemory != VK_NULL_HANDLE ) {
			vkFreeMemory( device, imageMemory, nullptr );
			imageMemory = VK_NULL_HANDLE;
		}
	}

	void TextureBinding::RemoveBinding() {
		const uint64_t ptrDiff = (uint64_t) this - (uint64_t)bindings.GetData();
		bindings.Remove( (uint32_t) ( ptrDiff / sizeof( TextureBinding ) ) );
	}
}
