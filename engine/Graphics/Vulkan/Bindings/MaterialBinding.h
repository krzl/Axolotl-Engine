#pragma once

#include "VulkanBinding.h"

namespace axlt::vk {
	class MaterialBinding final : public VulkanBinding {

	public:

		static MaterialBinding& CreateBinding( const ResourceHandle<MaterialResource>& material );
		void UpdateBinding( const ResourceHandle<MaterialResource>& material );

		static void CreateAwaitingDescriptorSets();

		struct PerCommandBuffer {
			Array<VkBuffer> uniformBuffers;
			Array<VkDeviceMemory> uniformBuffersMemory;
			Array<VkDescriptorSet> descriptorSets;
			BitArray<> dirtyUniformBuffers;
		};

		struct BufferDescriptorInfo {
			VkBuffer buffer = VK_NULL_HANDLE;
			uint32_t size = 0;
			uint8_t binding = 0;
			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		};

		struct BufferDescriptorCreateInfo {
			Array<BufferDescriptorInfo> bufferDescriptorInfos{};
		};

		struct TextureDescriptorCreateInfo {
			VkDescriptorImageInfo imageInfo{};
			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			uint8_t binding = 0;
		};

		uint32_t techniqueId = 0xFFFFFFFF;
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		Array<PerCommandBuffer> perFrameDataArray{};

	private:

		void Reset() override;
		void RemoveBinding() override;

		static Array<BufferDescriptorCreateInfo> awaitingBufferDescriptorSets;
		static Array<TextureDescriptorCreateInfo> awaitingTextureDescriptorSets;
		static SparseArray<MaterialBinding> bindings;
	};
}