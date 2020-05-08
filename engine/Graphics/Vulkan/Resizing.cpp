#include "stdafx.h"
#include "Vulkan.h"

#include "Initialization/Initializer.h"

namespace axlt::vk {
	void ResizeFramebuffer( uint32_t width, uint32_t height ) {
		vkDeviceWaitIdle( device );
		vkFreeCommandBuffers( device, commandPool, swapchainImages.GetSize(), commandBuffers.GetData() );
		vkDestroyRenderPass( device, renderPass, nullptr );

		for( uint32_t i = 0; i < swapchainImages.GetSize(); i++ ) {
			vkDestroyFramebuffer( device, framebuffers[i], nullptr );
			vkDestroyImageView( device, swapchainImageViews[i], nullptr );
		}

		init::SetupSwapchain( width, height );
		
		//TODO: CREATE CONFIG
		Array<VkAttachmentDescription> attachments;
		attachments.Add(
			VkAttachmentDescription{
				0,
				surfaceFormat.format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			}
		);

		VkAttachmentReference subpassColorAttachment = {
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		Array<VkSubpassDescription> subpassDescriptions = {
			VkSubpassDescription {
				0,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				0,
				nullptr,
				1,
				&subpassColorAttachment,
				nullptr,
				nullptr,
				0,
				nullptr
			}
		};

		Array<VkSubpassDependency> subpassDependencies;

		init::CreateRenderPass( attachments, subpassDescriptions, subpassDependencies, renderPass );
		init::CreateFramebuffers();
		init::AllocateCommandBuffers( commandPool, true, swapchainImages.GetSize(), commandBuffers );
	}
}