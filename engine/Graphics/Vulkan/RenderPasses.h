#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	struct SubpassParameters {
		VkPipelineBindPoint				pipelineType;
		Array<VkAttachmentReference>	inputAttachments;
		Array<VkAttachmentReference>	colorAttachments;
		Array<VkAttachmentReference>	resolveAttachments;
		VkAttachmentReference*			depthStencilAttachment;
		Array<uint32_t>					preserveAttachments;
	};

	bool CreateRenderPass( Array<VkAttachmentDescription>& attachmentDescriptions,
						   Array<SubpassParameters>& subpassParametersList,
						   Array<VkSubpassDependency>& subpassDependencies,
						   VkRenderPass& renderPass );

	bool CreateFramebuffer( VkRenderPass renderPass, Array<VkImageView>& attachments,
							uint32_t width, uint32_t height, uint32_t layers,
							VkFramebuffer& framebuffer );

	void BeginRenderPass( VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer,
						  VkRect2D renderArea, Array<VkClearValue>& clearValues, bool isInline );
	void GoToNextSubpass( VkCommandBuffer commandBuffer, bool isInline );
	void EndRenderPass( VkCommandBuffer commandBuffer );

	
	void DestroyFramebuffer( VkFramebuffer& framebuffer );
	void DestroyRenderPass( VkRenderPass& renderPass );
}