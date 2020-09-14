#pragma once

#include "VulkanBinding.h"
#include "Graphics/RenderTextureResource.h"

namespace axlt::vk {

	class MaterialBinding;

	struct RenderAttachmentData {
		VkImage image = VK_NULL_HANDLE;
		VkImageView imageView = VK_NULL_HANDLE;
		VkDeviceMemory imageMemory = VK_NULL_HANDLE;

		void Reset();
	};
	
	class RenderTextureBinding final : public VulkanBinding {

		friend class MaterialBinding;
		
	public:

		static RenderTextureBinding& CreateBinding( const ResourceHandle<RenderTextureResource>& texture );
		void UpdateBinding( const ResourceHandle<RenderTextureResource>& texture );
		
	private:
		
		void Reset() override;
		void RemoveBinding() override;

		RenderAttachmentData colorAttachment{};
		RenderAttachmentData depthAttachment{};

		VkSampler sampler = VK_NULL_HANDLE;

		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkFramebuffer framebuffer = VK_NULL_HANDLE;

		static SparseArray<RenderTextureBinding> bindings;
	};
}
