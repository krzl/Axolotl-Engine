#include "stdafx.h"
#include "Vulkan.h"

#include "Initialization/Initializer.h"

namespace axlt::vk {
	void ResizeFramebuffer( const uint32_t width, const uint32_t height ) {
		vkDeviceWaitIdle( device );
		vkFreeCommandBuffers( device, commandPool, swapchainImages.GetSize(), commandBuffers.GetData() );
		vkDestroyRenderPass( device, renderPass, nullptr );

		for( uint32_t i = 0; i < swapchainImages.GetSize(); i++ ) {
			vkDestroyFramebuffer( device, framebuffers[i], nullptr );
			vkDestroyImageView( device, swapchainImageViews[i], nullptr );
		}

		vkDestroyImageView( device, depthImageView, nullptr );
		vkFreeMemory( device, depthImageMemory, nullptr );
		vkDestroyImage( device, depthImage, nullptr );

		init::SetupSwapchain( width, height );
		
		//TODO: CREATE CONFIG
		const Array<VkAttachmentDescription> attachments = {
			VkAttachmentDescription {
				0,
				surfaceFormat.format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			},
			VkAttachmentDescription{
				0,
				depthTextureFormat,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			}
		};

		VkAttachmentReference subpassColorAttachment = {
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkAttachmentReference subpassDepthAttachment = {
			1,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		const Array<VkSubpassDescription> subpassDescriptions = {
			VkSubpassDescription {
				0,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				0,
				nullptr,
				1,
				&subpassColorAttachment,
				nullptr,
				&subpassDepthAttachment,
				0,
				nullptr
			}
		};

		const Array<VkSubpassDependency> subpassDependencies;

		init::CreateRenderPass( attachments, subpassDescriptions, subpassDependencies, renderPass );
		init::CreateFramebuffers();
		init::AllocateCommandBuffers( commandPool, true, swapchainImages.GetSize(), commandBuffers );
	}
}