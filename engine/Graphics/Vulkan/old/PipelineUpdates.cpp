// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "PipelineUpdates.h"

namespace axlt::vk {

	void UpdatePushConstants( VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages,
							  uint32_t offset, uint32_t size, void* data ) {
		vkCmdPushConstants( commandBuffer, pipelineLayout, pipelineStages, offset, size, data );
	}

	void UpdateViewports( VkCommandBuffer commandBuffer, uint32_t firstViewport, Array<VkViewport>& viewports ) {
		vkCmdSetViewport( commandBuffer, firstViewport, viewports.GetSize(), viewports.GetData() );
	}

	void UpdateScissors( VkCommandBuffer commandBuffer, uint32_t firstScissor, Array<VkRect2D>& scissors ) {
		vkCmdSetScissor( commandBuffer, firstScissor, scissors.GetSize(), scissors.GetData() );
	}

	void UpdateLineWidth( VkCommandBuffer commandBuffer, float lineWidth ) {
		vkCmdSetLineWidth( commandBuffer, lineWidth );
	}

	void UpdateDepthBias( VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor ) {
		vkCmdSetDepthBias( commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
	}

	void UpdateBlendConstants( VkCommandBuffer commandBuffer, Vector4 blendConstants ) {
		vkCmdSetBlendConstants( commandBuffer, blendConstants.data );
	}
}