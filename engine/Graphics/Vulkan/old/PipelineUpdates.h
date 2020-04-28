#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	void UpdatePushConstants( VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages,
							  uint32_t offset, uint32_t size, void* data );
	void UpdateViewports( VkCommandBuffer commandBuffer, uint32_t firstViewport, Array<VkViewport>& viewports );
	void UpdateScissors( VkCommandBuffer commandBuffer, uint32_t firstScissor, Array<VkRect2D>& scissors );
	void UpdateLineWidth( VkCommandBuffer commandBuffer, float lineWidth );
	void UpdateDepthBias( VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor );
	void UpdateBlendConstants( VkCommandBuffer commandBuffer, Vector4 blendConstants );
}