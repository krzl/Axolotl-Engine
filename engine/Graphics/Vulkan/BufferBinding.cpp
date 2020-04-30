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

	Array<BufferCopyInfo> bufferCopyInfos;
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
			return sizeof( float ) * 3 * mesh.vertices.GetSize();
		}
		if( index == 1 ) {
			return sizeof( float ) * 3 * mesh.normals.GetSize();
		}
		if( index == 2 ) {
			return sizeof( float ) * 3 * mesh.tangents.GetSize();
		}
		if( index == 3 ) {
			return sizeof( float ) * 3 * mesh.bitangents.GetSize();
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

		stagingBuffers.Add( stagingBuffer );
	}

	void CopyAllDrawBuffers() {
		if( bufferCopyInfos.GetSize() == 0 ) return;
		
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
			//vkDestroyBuffer( device, buffer, nullptr );
		}

		for( VkDeviceMemory memory : stagingBuffersMemory ) {
			//vkFreeMemory( device, memory, nullptr );
		}

		bufferCopyInfos.Clear();
		stagingBuffers.Clear();
		stagingBuffersMemory.Clear();
	}
}