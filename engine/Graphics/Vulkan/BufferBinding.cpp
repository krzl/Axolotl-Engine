#include "stdafx.h"
#include "Vulkan.h"

namespace axlt::vk {

	struct BufferCopyInfo {
		VkBuffer source;
		uint32_t sourceOffset;
		VkBuffer destination;
		uint32_t destinationOffset;
		uint32_t size;
	};

	struct ImageSetupInfo {
		VkImage image;
		VkBuffer stagingBuffer;
		uint32_t width;
		uint32_t height;
		VkFormat format;
	};

	VkFormat GetNativeTextureFormat( const TextureFormat format ) {
		switch (format) {
		case TextureFormat::DXT1:
			return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		case TextureFormat::DXT5:
			return VK_FORMAT_BC3_SRGB_BLOCK;
		case TextureFormat::RGBA32:
			return VK_FORMAT_R8G8B8A8_SRGB;
		default:
			return VK_FORMAT_R8G8B8A8_SRGB;
		}
	}

	Array<BufferCopyInfo> bufferCopyInfos;
	Array<ImageSetupInfo> imageSetupInfos;
	Array<VkBuffer> stagingBuffers;
	Array<VkDeviceMemory> stagingBuffersMemory;

	bool CreateBuffer( const uint32_t size, const VkBufferUsageFlags usage,
					   VkBuffer* buffer, uint32_t& allocationOffset,
					   uint32_t& allocationSize, VkMemoryRequirements* memReq ) {
		VkBufferCreateInfo createInfo = {
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			nullptr,
			0,
			size,
			usage,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr
		};

		const VkResult result = vkCreateBuffer( device, &createInfo, nullptr, buffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not create buffer\n" );
			return false;
		}

		VkMemoryRequirements tempMemReq;
		vkGetBufferMemoryRequirements( device, *buffer, &tempMemReq );

		if( memReq->size == 0 ) {
			allocationOffset = 0;
			*memReq = tempMemReq;
		} else {
			allocationOffset = (uint32_t) memReq->size;
			memReq->size += tempMemReq.size;
			memReq->alignment = max( memReq->alignment, tempMemReq.alignment );
			memReq->memoryTypeBits |= tempMemReq.memoryTypeBits;
		}

		allocationSize = (uint32_t) tempMemReq.size;

		return true;
	}

	const void* IndexToBufferData( const uint32_t index, const MeshResource& mesh ) {
		if( index == 0 ) {
			return mesh.vertices.GetData();
		}
		if( index == 1 ) {
			return mesh.normals.GetData();
		}
		if( index == 2 ) {
			return mesh.tangents.GetData();
		}
		if( index == 3 ) {
			return mesh.bitangents.GetData();
		}
		for( uint32_t i = 0; i < MAX_COLOR_CHANNELS; i++ ) {
			if( index == 4 + i ) {
				return mesh.colorChannels[i].GetData();
			}
		}
		for( uint32_t i = 0; i < MAX_UV_CHANNELS; i++ ) {
			if( index == 4 + MAX_COLOR_CHANNELS + i ) {
				return mesh.texCoordChannels[i].GetData();
			}
		}
		if( index == buffersPerMesh - 1 ) {
			return mesh.indices.GetData();
		}
		return nullptr;
	}

	uint32_t IndexToBufferSize( const uint32_t index, const MeshResource& mesh ) {
		if( index == 0 ) {
			return mesh.vertices.GetSize();
		}
		if( index == 1 ) {
			return mesh.normals.GetSize();
		}
		if( index == 2 ) {
			return mesh.tangents.GetSize();
		}
		if( index == 3 ) {
			return mesh.bitangents.GetSize();
		}
		for( uint32_t i = 0; i < MAX_COLOR_CHANNELS; i++ ) {
			if( index == 4 + i ) {
				return mesh.colorChannels[i].GetSize();
			}
		}
		for( uint32_t i = 0; i < MAX_UV_CHANNELS; i++ ) {
			if( index == 4 + MAX_COLOR_CHANNELS + i ) {
				return mesh.texCoordChannels[i].GetSize();
			}
		}
		if( index == buffersPerMesh - 1 ) {
			return mesh.indices.GetSize();
		}
		return 0;
	}

	uint32_t IndexToBufferByteSize( const uint32_t index, const MeshResource& mesh ) {
		if( index == 0 ) {
			return sizeof( Vector3 ) * mesh.vertices.GetSize();
		}
		if( index == 1 ) {
			return sizeof( Vector3 ) * mesh.normals.GetSize();
		}
		if( index == 2 ) {
			return sizeof( Vector3 ) * mesh.tangents.GetSize();
		}
		if( index == 3 ) {
			return sizeof( Vector3 ) * mesh.bitangents.GetSize();
		}
		for( uint32_t i = 0; i < MAX_COLOR_CHANNELS; i++ ) {
			if( index == 4 + i ) {
				return sizeof( Color ) * mesh.colorChannels[i].GetSize();
			}
		}
		for( uint32_t i = 0; i < MAX_UV_CHANNELS; i++ ) {
			if( index == 4 + MAX_COLOR_CHANNELS + i ) {
				return sizeof( float ) * mesh.texCoordChannels[i].GetSize();
			}
		}
		if( index == buffersPerMesh - 1 ) {
			return sizeof( uint32_t ) * mesh.indices.GetSize();
		}
		return 0;
	}

	void CreateDrawBuffers( const ResourceHandle<ModelResource>& model ) {
		DrawBuffers& drawBuffers = meshBuffers.Add( model.guid, DrawBuffers() );
		drawBuffers.buffers.AddEmpty( buffersPerMesh * model->meshes.GetSize() );
		memset( drawBuffers.buffers.GetData(), VK_NULL_HANDLE, drawBuffers.buffers.GetSize() );
		Array<uint32_t> allocationOffsets( buffersPerMesh * model->meshes.GetSize() );
		Array<uint32_t> allocationSizes( buffersPerMesh * model->meshes.GetSize() );

		VkMemoryRequirements memoryRequirements = {
			0,
			0,
			0
		};
		for( uint32_t i = 0; i < model->meshes.GetSize(); i++ ) {
			auto& mesh = model->meshes[i];
			for( uint32_t j = 0; j < buffersPerMesh; j++ ) {
				const uint32_t byteSize = IndexToBufferByteSize( j, mesh );
				if( byteSize != 0 ) {
					if( !CreateBuffer( byteSize, j == buffersPerMesh - 1 ?
									   VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT :
									   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
									   &drawBuffers.buffers[i * buffersPerMesh + j],
									   allocationOffsets[i * buffersPerMesh + j],
									   allocationSizes[i * buffersPerMesh + j],
									   &memoryRequirements ) ) {
						return;
					}
				}
			}
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

		VkResult result = vkCreateBuffer( device, &createInfo, nullptr, &stagingBuffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not create buffer\n" );
			return;
		}

		if( !AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
							 &stagingBufferMemory ) ) {
			return;
		}

		result = vkBindBufferMemory( device, stagingBuffer, stagingBufferMemory, 0 );
		if( result != VK_SUCCESS ) {
			printf( "Could not bind buffer memory\n" );
			return;
		}

		if( !AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
							 &drawBuffers.memory ) ) {
			return;
		}

		void* data;
		vkMapMemory( device, stagingBufferMemory, 0, memoryRequirements.size, 0, &data );

		for( uint32_t i = 0; i < drawBuffers.buffers.GetSize(); i++ ) {
			if( drawBuffers.buffers[i] != VK_NULL_HANDLE ) {
				const uint32_t meshIndex = i / buffersPerMesh;
				const uint32_t bufferIndex = i % buffersPerMesh;

				result = vkBindBufferMemory( device, drawBuffers.buffers[i],
											 drawBuffers.memory, allocationOffsets[i] );
				if( result != VK_SUCCESS ) {
					printf( "Could not bind buffer memory\n" );
					return;
				}
				memcpy( (void*) ( (uint64_t) data + allocationOffsets[i] ),
						IndexToBufferData( bufferIndex, model->meshes[meshIndex] ),
						IndexToBufferByteSize( bufferIndex, model->meshes[meshIndex] ) );

				BufferCopyInfo& copyInfo = bufferCopyInfos.Emplace();
				copyInfo = {
					stagingBuffer,
					allocationOffsets[i],
					drawBuffers.buffers[i],
					0,
					IndexToBufferByteSize( bufferIndex, model->meshes[meshIndex] )
				};
			}
		}

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

		stagingBuffersMemory.Add( stagingBufferMemory );
		stagingBuffers.Add( stagingBuffer );
	}

	void CreateTextureBuffer( const ResourceHandle<TextureResource>& texture ) {
		TextureData& textureData = textureDataArray.Add( texture.guid, TextureData() );

		VkImageCreateInfo imageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			0,
			VK_IMAGE_TYPE_2D,
			GetNativeTextureFormat( texture->format ),
			{
				texture->width,
				texture->height,
				1,
			},
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr,
			VK_IMAGE_LAYOUT_UNDEFINED
		};

		VkResult result = vkCreateImage( device, &imageCreateInfo, nullptr, &textureData.image );
		if( result != VK_SUCCESS ) {
			printf( "Could not create image\n" );
			return;
		}

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements( device, textureData.image, &memoryRequirements );

		if( !AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
							 &textureData.imageMemory ) ) {
			return;
		}

		result = vkBindImageMemory( device, textureData.image, textureData.imageMemory, 0 );
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
		if( !AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
							 &stagingBufferMemory ) ) {
			return;
		}

		stagingBuffersMemory.Add( stagingBufferMemory );
		stagingBuffers.Add( stagingBuffer );

		result = vkBindBufferMemory( device, stagingBuffer, stagingBufferMemory, 0 );
		if( result != VK_SUCCESS ) {
			printf( "Could not bind staging buffer memory\n" );
			return;
		}

		void* data;
		vkMapMemory( device, stagingBufferMemory, 0, memoryRequirements.size, 0, &data );

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
			textureData.image,
			VK_IMAGE_VIEW_TYPE_2D,
			GetNativeTextureFormat( texture->format ),
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				1,
				0,
				1
			}
		};

		result = vkCreateImageView( device, &imageViewCreateInfo, nullptr, &textureData.imageView );
		if( result != VK_SUCCESS ) {
			printf( "Could not create image view\n" );
			return;
		}

		VkSamplerCreateInfo samplerCreateInfo = {
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			nullptr,
			0,
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			0.0f,
			VK_FALSE,
			1.0f,
			VK_FALSE,
			VK_COMPARE_OP_ALWAYS,
			0.0f,
			0.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			VK_FALSE
		};

		result = vkCreateSampler( device, &samplerCreateInfo, nullptr, &textureData.sampler );
		if( result != VK_SUCCESS ) {
			printf( "Could not create sampler\n" );
			return;
		}

		ImageSetupInfo& copyInfo = imageSetupInfos.Emplace();
		copyInfo = {
			textureData.image,
			stagingBuffer,
			texture->width,
			texture->height,
			imageCreateInfo.format
		};
	}

	void ImageTransition( const VkCommandBuffer commandBuffer, const VkImage image, const VkFormat format,  // NOLINT(misc-misplaced-const)
						  const VkImageLayout srcLayout, const VkImageLayout destLayout ) {

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
				1,
				0,
				1
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

		vkCmdPipelineBarrier( commandBuffer, sourceStage, destinationStage,
							  0, 0, nullptr, 0, nullptr, 1, &barrier );
	}

	void CopyAllBuffers() {
		if( bufferCopyInfos.GetSize() == 0 && imageSetupInfos.GetSize() == 0 ) return;

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			commandPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1
		};

		VkCommandBufferBeginInfo beginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			nullptr
		};

		VkCommandBuffer copyCommandBuffer;

		VkResult result = vkAllocateCommandBuffers( device, &commandBufferAllocateInfo, &copyCommandBuffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not allocate copy command buffer\n" );
			return;
		}
		result = vkBeginCommandBuffer( copyCommandBuffer, &beginInfo );
		if( result != VK_SUCCESS ) {
			printf( "Could not begin copy command buffer\n" );
			return;
		}

		for( BufferCopyInfo& copyInfo : bufferCopyInfos ) {
			VkBufferCopy bufferCopyData = {
				copyInfo.sourceOffset,
				copyInfo.destinationOffset,
				copyInfo.size
			};

			vkCmdCopyBuffer( copyCommandBuffer, copyInfo.source, copyInfo.destination, 1, &bufferCopyData );
		}


		for( ImageSetupInfo& imageSetupInfo : imageSetupInfos ) {
			VkBufferImageCopy imageCopyData = {
				0,
				0,
				0,
				{
					VK_IMAGE_ASPECT_COLOR_BIT,
					0,
					0,
					1
				},
				{
					0,
					0,
					0
				},
				{
					imageSetupInfo.width,
					imageSetupInfo.height,
					1
				}
			};

			ImageTransition( copyCommandBuffer, imageSetupInfo.image, imageSetupInfo.format,
							 VK_IMAGE_LAYOUT_UNDEFINED,
							 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

			vkCmdCopyBufferToImage( copyCommandBuffer, imageSetupInfo.stagingBuffer, imageSetupInfo.image,
									VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyData );

			ImageTransition( copyCommandBuffer, imageSetupInfo.image, imageSetupInfo.format,
							 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		vkEndCommandBuffer( copyCommandBuffer );

		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			0,
			nullptr,
			nullptr,
			1,
			&copyCommandBuffer,
			0,
			nullptr
		};

		result = vkQueueSubmit( queues[0][0], 1, &submitInfo, VK_NULL_HANDLE );
		if( result != VK_SUCCESS ) {
			printf( "Could not submit queue\n" );
			return;
		}
		result = vkQueueWaitIdle( queues[0][0] );
		if( result != VK_SUCCESS ) {
			printf( "Could not wait idle for queue\n" );
			return;
		}

		vkFreeCommandBuffers( device, commandPool, 1, &copyCommandBuffer );

		for( VkBuffer& buffer : stagingBuffers ) {
			vkDestroyBuffer( device, buffer, nullptr );
		}

		for( VkDeviceMemory memory : stagingBuffersMemory ) {
			vkFreeMemory( device, memory, nullptr );
		}

		bufferCopyInfos.Clear();
		imageSetupInfos.Clear();
		stagingBuffers.Clear();
		stagingBuffersMemory.Clear();
	}
}