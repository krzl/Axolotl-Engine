#pragma once

#include "VulkanBinding.h"

namespace axlt::vk {

	class MaterialBinding;
	
	class TextureBinding final : public VulkanBinding {

		friend class MaterialBinding;
		
	public:

		static TextureBinding& CreateBinding( const ResourceHandle<TextureResource>& texture );
		void UpdateBinding( const ResourceHandle<TextureResource>& texture );

		static bool HasAwaitingStagingBuffers();
		static void SubmitAwaitingStagingBuffers( VkCommandBuffer commandBuffer );
		static void CleanupAwaitingStagingBuffers();
		
	private:
		
		void Reset() override;
		void RemoveBinding() override;

		VkImage image = VK_NULL_HANDLE;
		VkImageView imageView = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
		
		VkDeviceMemory imageMemory = VK_NULL_HANDLE;

		struct StagingBufferCopyInfo {
			VkImage image;
			VkBuffer stagingBuffer;
			uint32_t width;
			uint32_t height;
			uint32_t depth;
			VkFormat format;
			VkDeviceMemory stagingMemory;
			uint8_t mipmapCount;
			uint8_t arrayCount;
		};
		
		static Array<StagingBufferCopyInfo> awaitingStagingBuffers;
		static SparseArray<TextureBinding> bindings;
	};
}
