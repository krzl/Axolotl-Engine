#pragma once

#include "VulkanBinding.h"

namespace axlt::vk {

	class ModelBufferBinding;
	
	class MeshBufferBinding final : public VulkanBinding {

		friend class ModelBufferBinding;
		
	public:

		static MeshBufferBinding& CreateBinding( const Mesh& mesh, const ResourceHandle<ModelResource>& model );
		void UpdateBinding( const Mesh& mesh, const ResourceHandle<ModelResource>& model );

		static bool HasAwaitingStagingBuffers();
		static void SubmitAwaitingStagingBuffers( VkCommandBuffer commandBuffer );
		static void CleanupAwaitingStagingBuffers();

		static bool CreateBuffer( uint32_t size, VkBufferUsageFlags usage, VkBuffer& buffer );
		static bool BindMemory( VkBuffer& buffer, VkDeviceMemory& memory, uint32_t offset );
		static void CreateStagingBuffers( const Mesh& mesh, const VkMemoryRequirements& memoryRequirements,
			uint32_t vertexBufferMemoryOffset, VkBuffer indexBuffer, VkBuffer vertexBuffer );

		uint16_t GetMeshLayout() const;

		struct StagingBufferCopyInfo {
			VkBuffer stagingBuffer;
			VkBuffer indexBuffer;
			VkBuffer vertexBuffer;
			uint32_t size;
			uint32_t vertexBufferOffset;
			VkDeviceMemory memory;
		};
		
		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkBuffer indexBuffer = VK_NULL_HANDLE;
		uint32_t indexCount = 0;

		VkDeviceMemory memory = VK_NULL_HANDLE;

		static Array<StagingBufferCopyInfo> awaitingStagingBuffers;

	private:

		void Reset() override;
		void RemoveBinding() override;

		uint16_t meshLayout = 0;

		static SparseArray<MeshBufferBinding> bindings;
	};

	class ModelBufferBinding final : public VulkanBinding {

	public:

		static ModelBufferBinding& CreateBinding( const ResourceHandle<ModelResource>& model );
		void UpdateBinding( const ResourceHandle<ModelResource>& model );

		Array<MeshBufferBinding*> meshBindings{};

		static SparseArray<ModelBufferBinding> bindings;
		
	private:
		
		void Reset() override;
		void RemoveBinding() override;
	};
}