// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "ModelBufferBinding.h"

namespace axlt::vk {
	
	Array<MeshBufferBinding::StagingBufferCopyInfo> MeshBufferBinding::awaitingStagingBuffers;
	SparseArray<MeshBufferBinding> MeshBufferBinding::bindings;

	SparseArray<ModelBufferBinding> ModelBufferBinding::bindings;

	bool MeshBufferBinding::CreateBuffer( const uint32_t size, const VkBufferUsageFlags usage, VkBuffer& buffer ) {
		if( size == 0 ) return false;
		
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

		const VkResult result = vkCreateBuffer( device, &createInfo, nullptr, &buffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not create buffer\n" );
			return false;
		}

		return true;
	}

	bool MeshBufferBinding::BindMemory( VkBuffer& buffer, VkDeviceMemory& memory, const uint32_t offset ) {

		const VkResult result = vkBindBufferMemory( device, buffer, memory, offset );
		if( result != VK_SUCCESS ) {
			printf( "Could not bind buffer memory\n" );
			return false;
		}

		return true;
	}

	void MeshBufferBinding::CreateStagingBuffers( const Mesh& mesh, const VkMemoryRequirements& memoryRequirements, 
		const uint32_t vertexBufferMemoryOffset, VkBuffer indexBuffer, VkBuffer vertexBuffer ) {
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		if( !CreateBuffer( (uint32_t) memoryRequirements.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer ) ) {
			return;
		}

		if( !AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &stagingBufferMemory ) ) {
			return;
		}

		if( !BindMemory( stagingBuffer, stagingBufferMemory, 0 ) ) {
			return;
		}
	
		void* data;
		vkMapMemory( device, stagingBufferMemory, 0, memoryRequirements.size, 0, &data );

		memcpy( data, mesh.indices.GetData(), mesh.indices.GetByteSize() );
		memcpy( (void*) ( (uint64_t) data + vertexBufferMemoryOffset ), mesh.vertexData.GetData(), mesh.vertexData.GetByteSize() );

		vkUnmapMemory( device, stagingBufferMemory );
		
		VkMappedMemoryRange mappedMemoryRange = {
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			stagingBufferMemory,
			0,
			VK_WHOLE_SIZE
		};
		
		const VkResult result = vkFlushMappedMemoryRanges( device, 1, &mappedMemoryRange );
		if (result != VK_SUCCESS) {
			printf( "Could not flush mapped memory of staging buffer\n" );
			return;
		}

		awaitingStagingBuffers.Emplace( StagingBufferCopyInfo {
			stagingBuffer,
			indexBuffer,
			vertexBuffer,
			(uint32_t) memoryRequirements.size,
			vertexBufferMemoryOffset,
			stagingBufferMemory
		} );
	}

	MeshBufferBinding& MeshBufferBinding::CreateBinding( const Mesh& mesh, const ResourceHandle<ModelResource>& model ) {
		MeshBufferBinding& binding = *(MeshBufferBinding*) bindings.Emplace().element;
		
		return binding;
	}

	void MeshBufferBinding::UpdateBinding( const Mesh& mesh, const ResourceHandle<ModelResource>& model ) {
		const VkBufferUsageFlags usageFlags = model->isDynamic ? 0 : VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		const VkMemoryPropertyFlagBits bufferMemoryProperty = model->isDynamic ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkMemoryRequirements memoryRequirements;
		if( CreateBuffer( mesh.indices.GetByteSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | usageFlags, indexBuffer ) ) {
			indexCount = mesh.indices.GetSize();
			vkGetBufferMemoryRequirements( device, indexBuffer, &memoryRequirements );
		}

		uint32_t vertexBufferMemoryOffset = 0;
		if( CreateBuffer( mesh.vertexData.GetByteSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | usageFlags, vertexBuffer ) ) {
			VkMemoryRequirements vertexBufferMemoryRequirement;
			vkGetBufferMemoryRequirements( device, vertexBuffer, &vertexBufferMemoryRequirement );

			vertexBufferMemoryOffset = (uint32_t) memoryRequirements.size;
			
			memoryRequirements.alignment = max( memoryRequirements.alignment, vertexBufferMemoryRequirement.alignment );
			memoryRequirements.size += vertexBufferMemoryRequirement.size;
			memoryRequirements.memoryTypeBits |= vertexBufferMemoryRequirement.memoryTypeBits;
		}

		if( memoryRequirements.size == 0 ) {
			return;
		}
		
		if( !AllocateMemory( memoryRequirements, bufferMemoryProperty, &memory ) ) {
			return;
		}

		if( indexBuffer != VK_NULL_HANDLE ) {
			BindMemory( indexBuffer, memory, 0 );
		}

		if( vertexBuffer != VK_NULL_HANDLE ) {
			BindMemory( vertexBuffer, memory, vertexBufferMemoryOffset );
		}

		if( model->isDynamic ) {
			void* data;
			vkMapMemory( device, memory, 0, memoryRequirements.size, 0, &data );

			memcpy( data, mesh.indices.GetData(), mesh.indices.GetByteSize() );
			memcpy( (void*) ( (uint64_t) data + vertexBufferMemoryOffset ), mesh.vertexData.GetData(), mesh.vertexData.GetByteSize() );

			vkUnmapMemory( device, memory );
		} else {
			CreateStagingBuffers( mesh, memoryRequirements, vertexBufferMemoryOffset, indexBuffer, vertexBuffer );
		}

		if( !model->storeMeshData ) {
			const_cast<Mesh&>(mesh).vertexData.Clear();
		}

		meshLayout = mesh.GetMeshLayout();
	}

	bool MeshBufferBinding::HasAwaitingStagingBuffers() {
		return awaitingStagingBuffers.GetSize() != 0;
	}

	void MeshBufferBinding::SubmitAwaitingStagingBuffers( VkCommandBuffer commandBuffer ) {
		Array<VkBufferCopy> bufferCopyData( 2 );
		for( StagingBufferCopyInfo& copyInfo : awaitingStagingBuffers ) {
			bufferCopyData[0] = {
				0,
				0,
				copyInfo.vertexBufferOffset
			};
			bufferCopyData[ 1 ] = {
				copyInfo.vertexBufferOffset,
				0,
				copyInfo.size - copyInfo.vertexBufferOffset
			};
			vkCmdCopyBuffer( commandBuffer, copyInfo.stagingBuffer, copyInfo.indexBuffer, 1, &bufferCopyData[ 0 ] );
			vkCmdCopyBuffer( commandBuffer, copyInfo.stagingBuffer, copyInfo.vertexBuffer, 1, &bufferCopyData[ 1 ] );
		}
	}

	void MeshBufferBinding::CleanupAwaitingStagingBuffers() {
		for( StagingBufferCopyInfo& copyInfo : awaitingStagingBuffers ) {
			vkDestroyBuffer( device, copyInfo.indexBuffer, nullptr );
			vkDestroyBuffer( device, copyInfo.vertexBuffer, nullptr );
			vkFreeMemory( device, copyInfo.memory, nullptr );
		}

		awaitingStagingBuffers.Clear();
	}

	uint16_t MeshBufferBinding::GetMeshLayout() const {
		return meshLayout;
	}

	void MeshBufferBinding::Reset() {
		if( vertexBuffer != VK_NULL_HANDLE ) {
			vkDestroyBuffer( device, vertexBuffer, nullptr );
			vertexBuffer = VK_NULL_HANDLE;
		}
		if( indexBuffer != VK_NULL_HANDLE ) {
			vkDestroyBuffer( device, indexBuffer, nullptr );
			indexBuffer = VK_NULL_HANDLE;
		}
		if( memory != VK_NULL_HANDLE ) {
			vkFreeMemory( device, memory, nullptr );
			memory = VK_NULL_HANDLE;
		}
	}

	void MeshBufferBinding::RemoveBinding() {
		const uint64_t ptrDiff = (uint64_t) this - (uint64_t)bindings.GetData();
		bindings.Remove( (uint32_t) ( ptrDiff / sizeof( MeshBufferBinding ) ) );
	}

	ModelBufferBinding& ModelBufferBinding::CreateBinding( const ResourceHandle<ModelResource>& model ) {
		ModelBufferBinding& binding = *(ModelBufferBinding*) bindings.Emplace().element;
		binding.meshBindings.AddEmpty( model->meshes.GetSize() );
		bindingMap.Add( model->GetInstanceId(), &binding );

		for( uint32_t i = 0; i < model->meshes.GetSize(); i++ ) {
			const Mesh& mesh = model->meshes[ i ];
			MeshBufferBinding& meshBinding = MeshBufferBinding::CreateBinding( mesh, model );
			binding.meshBindings[ i ] = &meshBinding;
		}

		return binding;
	}

	void ModelBufferBinding::UpdateBinding( const ResourceHandle<ModelResource>& model ) {
		for( uint32_t i = 0; i < model->meshes.GetSize(); i++ ) {
			meshBindings[i]->UpdateBinding( model->meshes[i], model );
		}
	}

	void ModelBufferBinding::Reset() {
		for( MeshBufferBinding* meshBinding : meshBindings ) {
			meshBinding->Reset();
			meshBinding->RemoveBinding();
		}
		meshBindings.Clear();
	}

	void ModelBufferBinding::RemoveBinding() {
		const uint64_t ptrDiff = (uint64_t) this - (uint64_t)bindings.GetData();
		bindings.Remove( (uint32_t) ( ptrDiff / sizeof( ModelBufferBinding ) ) );
	}
}
