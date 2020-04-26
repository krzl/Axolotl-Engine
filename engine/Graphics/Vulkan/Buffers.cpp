// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Buffers.h"

namespace axlt::vk {

	bool CreateBuffer( const VkDeviceSize size, const VkBufferUsageFlags usage, VkBuffer& buffer ) {

		VkBufferCreateInfo bufferCreateInfo = {
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			nullptr,
			0,
			size,
			usage,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr
		};

		const VkResult result = vkCreateBuffer( device, &bufferCreateInfo, nullptr, &buffer );
		if( VK_SUCCESS != result ) {
			printf( "Could not create buffer\n" );
			return false;
		}

		return true;
	}

	bool BindMemoryToBuffer( const VkBuffer buffer, VkDeviceMemory& memoryObject,
	                         const VkMemoryPropertyFlagBits memoryProperties,
	                         uint32_t memoryOffset ) {

		if( VK_NULL_HANDLE == memoryObject ) {
			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements( device, buffer, &memoryRequirements );
			if( !AllocateMemory( memoryRequirements, memoryProperties, memoryObject ) ) {
				return false;
			}
		}

		const VkResult result = vkBindBufferMemory( device, buffer, memoryObject, memoryOffset );
		if( VK_SUCCESS != result ) {
			printf( "Could not bind memory to buffer\n" );
			return false;
		}

		return true;
	}

	bool BindMemoryToBuffers( const Array<VkBuffer>& buffers, VkMemoryPropertyFlagBits memoryProperties, VkDeviceMemory& memoryObject ) {
		Array<uint32_t> memoryOffsets;
		memoryOffsets.AddEmpty( buffers.GetSize() );

		VkMemoryRequirements memoryRequirements = {};

		for( uint32_t i = 0; i < buffers.GetSize(); i++ ) {
			VkMemoryRequirements temp;
			vkGetBufferMemoryRequirements( device, buffers[i], &temp );
			memoryRequirements.alignment = max( temp.alignment, memoryRequirements.alignment );
			memoryRequirements.memoryTypeBits |= temp.memoryTypeBits;
			memoryRequirements.size += memoryRequirements.size % temp.alignment;
			memoryOffsets.Add( memoryRequirements.size );
			memoryRequirements.size += temp.size;
		}
		if( !AllocateMemory( memoryRequirements, memoryProperties, memoryObject ) ) {
			return false;
		}

		for( uint32_t i = 0; i < buffers.GetSize(); i++ ) {
			if( !BindMemoryToBuffer( buffers[i], memoryObject, memoryProperties, memoryOffsets[i] ) ) {
				return false;
			}
		}

		return true;
	}

	// commandBuffer must be in recording state
	void SetBufferBarriers( Array<BufferTransition>& bufferTransitions, VkCommandBuffer commandBuffer,
							VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages ) {

		Array<VkBufferMemoryBarrier> barriers;
		barriers.AddEmpty( bufferTransitions.GetSize() );

		for( uint32_t i = 0; i < bufferTransitions.GetSize(); i++ ) {

			BufferTransition& transitionInfo = bufferTransitions[i];

			barriers[i] = VkBufferMemoryBarrier{
				VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
				nullptr,
				transitionInfo.newAccess,
				transitionInfo.oldAccess,
				transitionInfo.newQueueFamily,
				transitionInfo.oldQueueFamily,
				transitionInfo.buffer,
				0,
				VK_WHOLE_SIZE
			};
		}

		vkCmdPipelineBarrier( commandBuffer, generatingStages, consumingStages, 0, 0, nullptr,
							  barriers.GetSize(), barriers.GetData(), 0, nullptr );
	}

	bool CreateBufferView( VkBuffer buffer, VkFormat format,
						   VkDeviceSize memoryOffset, VkDeviceSize memoryRange,
						   VkBufferView& bufferView ) {

		VkBufferViewCreateInfo bufferViewCreateInfo = {
			VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
			nullptr,
			0,
			buffer,
			format,
			memoryOffset,
			memoryRange
		};

		const VkResult result = vkCreateBufferView( device, &bufferViewCreateInfo, nullptr, &bufferView );
		if( VK_SUCCESS != result ) {
			printf( "Could not create buffer view\n" );
			return false;
		}

		return true;
	}

	void CopyBuffersData( VkCommandBuffer commandBuffer, VkBuffer source,
						  VkBuffer destination, Array<VkBufferCopy>& bufferCopyInfo ) {

		if( bufferCopyInfo.GetSize() > 0 ) {
			vkCmdCopyBuffer( commandBuffer, source, destination,
							 bufferCopyInfo.GetSize(), bufferCopyInfo.GetData() );
		}
	}

	void CopyDataFromImage( VkCommandBuffer commandBuffer, VkImage source,
							VkImageLayout layout, VkBuffer destination,
							Array<VkBufferImageCopy>& bufferCopyInfo ) {
		if( bufferCopyInfo.GetSize() > 0 ) {
			vkCmdCopyImageToBuffer( commandBuffer, source, layout, destination,
									bufferCopyInfo.GetSize(), bufferCopyInfo.GetData() );
		}
	}

	void BindVertexBuffers( VkCommandBuffer commandBuffer, uint32_t firstBindingId, Array<VkBuffer>& buffers, Array<VkDeviceSize>& offsets ) {
		vkCmdBindVertexBuffers( commandBuffer, firstBindingId, buffers.GetSize(), buffers.GetData(), offsets.GetData() );
	}

	void BindIndexBuffers( VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType ) {
		vkCmdBindIndexBuffer( commandBuffer, buffer, offset, indexType );
	}

	void DestroyBufferView( VkBufferView& bufferView ) {
		if( VK_NULL_HANDLE != bufferView ) {
			vkDestroyBufferView( device, bufferView, nullptr );
			bufferView = VK_NULL_HANDLE;
		}
	}

	void DestroyBuffer( VkBuffer& buffer ) {
		if( VK_NULL_HANDLE != buffer ) {
			vkDestroyBuffer( device, buffer, nullptr );
			buffer = VK_NULL_HANDLE;
		}
	}
}