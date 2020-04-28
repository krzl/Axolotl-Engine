// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Images.h"

namespace axlt::vk {

	bool CreateImage( VkImageType type, VkFormat format, VkExtent3D size, uint32_t mipLevels, uint32_t layerCount,
					  VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap, VkImage& image ) {

		VkImageCreateInfo imageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			cubemap ? (uint32_t) VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0,
			type,
			format,
			size,
			mipLevels,
			layerCount,
			samples,
			VK_IMAGE_TILING_OPTIMAL,
			usage,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr,
			VK_IMAGE_LAYOUT_UNDEFINED
		};

		const VkResult result = vkCreateImage( device, &imageCreateInfo, nullptr, &image );
		if( VK_SUCCESS != result ) {
			printf( "Could not create image\n" );
			return false;
		}

		return true;
	}

	bool BindMemoryToImage( VkImage image, VkMemoryPropertyFlagBits memoryProperties,
							VkDeviceMemory memoryObject, uint32_t memoryOffset ) {

		if( VK_NULL_HANDLE == memoryObject ) {
			VkMemoryRequirements memoryRequirements;
			vkGetImageMemoryRequirements( device, image, &memoryRequirements );
			if( !AllocateMemory( memoryRequirements, memoryProperties, memoryObject ) ) {
				return false;
			}
		}

		const VkResult result = vkBindImageMemory( device, image, memoryObject, memoryOffset );
		if( VK_SUCCESS != result ) {
			printf( "Could not bind memory to buffer\n" );
			return false;
		}

		return true;
	}

	// commandBuffer must be in recording state
	void SetImageBarriers( Array<ImageTransition>& imageTransitions, VkCommandBuffer commandBuffer,
						   VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages ) {

		Array<VkImageMemoryBarrier> barriers;
		barriers.AddEmpty( imageTransitions.GetSize() );

		for( uint32_t i = 0; i < imageTransitions.GetSize(); i++ ) {
			ImageTransition& transition = imageTransitions[i];

			barriers[i] = VkImageMemoryBarrier{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				transition.oldAccess,
				transition.newAccess,
				transition.oldLayout,
				transition.newLayout,
				transition.oldQueueFamily,
				transition.newQueueFamily,
				transition.image,
				VkImageSubresourceRange {
					transition.aspect,
					0,
					VK_REMAINING_MIP_LEVELS,
					0,
					VK_REMAINING_ARRAY_LAYERS
				}
			};
		}

		vkCmdPipelineBarrier( commandBuffer, generatingStages, consumingStages,
							  0, 0, nullptr, 0, nullptr,
							  barriers.GetSize(), barriers.GetData() );
	}

	bool CreateImageView( VkImage image, VkImageViewType viewType, VkFormat format,
						  VkImageAspectFlags aspectMask, VkImageView& imageView ) {

		VkImageViewCreateInfo imageViewCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			image,
			viewType,
			format,
			VkComponentMapping {
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			VkImageSubresourceRange {
				aspectMask,
				0,
				VK_REMAINING_MIP_LEVELS,
				0,
				VK_REMAINING_ARRAY_LAYERS
			}
		};

		const VkResult result = vkCreateImageView( device, &imageViewCreateInfo, nullptr, &imageView );
		if( VK_SUCCESS != result ) {
			printf( "Could not create image view\n" );
			return false;
		}

		return true;
	}

	void CopyDataFromBuffer( VkCommandBuffer commandBuffer, VkBuffer source,
							 VkImage destination, VkImageLayout layout,
							 Array<VkBufferImageCopy>& bufferCopyInfo ) {

		if( bufferCopyInfo.GetSize() > 0 ) {
			vkCmdCopyBufferToImage( commandBuffer, source, destination, layout,
									bufferCopyInfo.GetSize(), bufferCopyInfo.GetData() );
		}
	}

	bool CreateSampler( VkImage image, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipMapMode,
						VkSamplerAddressMode addressModeU, VkSamplerAddressMode addressModeV,
						VkSamplerAddressMode addressModeW, float mipLodBias, bool anisotropyEnabled,
						float maxAnisotropy, bool compareEnabled, VkCompareOp compareOp,
						float minLod, float maxLod, VkBorderColor borderColor, bool unnormalizedCoords,
						VkSampler& sampler ) {

		VkSamplerCreateInfo samplerCreateInfo{
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			nullptr,
			0,
			magFilter,
			minFilter,
			mipMapMode,
			addressModeU,
			addressModeV,
			addressModeW,
			mipLodBias,
			anisotropyEnabled,
			maxAnisotropy,
			compareEnabled,
			compareOp,
			minLod,
			maxLod,
			borderColor,
			unnormalizedCoords
		};

		const VkResult result = vkCreateSampler( device, &samplerCreateInfo, nullptr, &sampler );
		if( VK_SUCCESS != result ) {
			printf( "Could not create sampler\n" );
			return false;
		}

		return true;
	}

	void DestroySampler( VkSampler& sampler ) {
		if( VK_NULL_HANDLE != sampler ) {
			vkDestroySampler( device, sampler, nullptr );
			sampler = VK_NULL_HANDLE;
		}
	}

	void DestroyImageView( VkImageView& imageView ) {
		if( VK_NULL_HANDLE != imageView ) {
			vkDestroyImageView( device, imageView, nullptr );
			imageView = VK_NULL_HANDLE;
		}
	}

	void DestroyImage( VkImage& image ) {
		if( VK_NULL_HANDLE != image ) {
			vkDestroyImage( device, image, nullptr );
			image = VK_NULL_HANDLE;
		}
	}
}
