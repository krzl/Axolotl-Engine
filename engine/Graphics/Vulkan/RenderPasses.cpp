// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "RenderPasses.h"

namespace axlt::vk {

	bool CreateRenderPass( Array<VkAttachmentDescription>& attachmentDescriptions,
						   Array<SubpassParameters>& subpassParametersList,
						   Array<VkSubpassDependency>& subpassDependencies,
						   VkRenderPass& renderPass ) {

		Array<VkSubpassDescription> subpassDescriptions;
		subpassDescriptions.AddEmpty( subpassParametersList.GetSize() );

		for( uint32_t i = 0; i < subpassParametersList.GetSize(); i++ ) {
			SubpassParameters& subpassParams = subpassParametersList[i];
			subpassDescriptions[i] = VkSubpassDescription{
				0,
				subpassParams.pipelineType,
				subpassParams.inputAttachments.GetSize(),
				subpassParams.inputAttachments.GetData(),
				subpassParams.colorAttachments.GetSize(),
				subpassParams.colorAttachments.GetData(),
				subpassParams.resolveAttachments.GetData(),
				subpassParams.depthStencilAttachment,
				subpassParams.preserveAttachments.GetSize(),
				subpassParams.preserveAttachments.GetData()
			};
		}

		VkRenderPassCreateInfo renderPassCreateInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			0,
			attachmentDescriptions.GetSize(),
			attachmentDescriptions.GetData(),
			subpassDescriptions.GetSize(),
			subpassDescriptions.GetData(),
			subpassDependencies.GetSize(),
			subpassDependencies.GetData(),
		};

		const VkResult result = vkCreateRenderPass( device, &renderPassCreateInfo, nullptr, &renderPass );
		if( VK_SUCCESS != result ) {
			printf( "Could not create render pass\n" );
			return false;
		}

		return true;
	}

	bool CreateFramebuffer( VkRenderPass renderPass, Array<VkImageView>& attachments,
							uint32_t width, uint32_t height, uint32_t layers,
							VkFramebuffer& framebuffer ) {

		VkFramebufferCreateInfo framebufferCreateInfo = {
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			renderPass,
			attachments.GetSize(),
			attachments.GetData(),
			width,
			height,
			layers
		};

		const VkResult result = vkCreateFramebuffer( device, &framebufferCreateInfo, nullptr, &framebuffer );
		if( VK_SUCCESS != result ) {
			printf( "Could not create framebuffer\n" );
			return false;
		}

		return true;
	}

	void BeginRenderPass( VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer,
						  VkRect2D renderArea, Array<VkClearValue>& clearValues, bool isInline ) {

		VkRenderPassBeginInfo renderPassBeginInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			renderPass,
			framebuffer,
			renderArea,
			clearValues.GetSize(),
			clearValues.GetData(),
		};

		vkCmdBeginRenderPass( commandBuffer, &renderPassBeginInfo, isInline ? VK_SUBPASS_CONTENTS_INLINE :
							  VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
	}

	void GoToNextSubpass( VkCommandBuffer commandBuffer, bool isInline ) {
		vkCmdNextSubpass( commandBuffer, isInline ? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
	}

	void EndRenderPass( VkCommandBuffer commandBuffer ) {
		vkCmdEndRenderPass( commandBuffer );
	}

	void DestroyFramebuffer( VkFramebuffer& framebuffer ) {
		if( framebuffer != VK_NULL_HANDLE ) {
			vkDestroyFramebuffer( device, framebuffer, nullptr );
			framebuffer = VK_NULL_HANDLE;
		}
	}

	void DestroyRenderPass( VkRenderPass& renderPass ) {
		if( renderPass != VK_NULL_HANDLE ) {
			vkDestroyRenderPass( device, renderPass, nullptr );
			renderPass = VK_NULL_HANDLE;
		}
	}
}