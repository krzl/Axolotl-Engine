#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	struct ShaderStageParameters {
		VkShaderStageFlagBits	shaderStage;
		VkShaderModule			shaderModule;
		VkSpecializationInfo*	specializationInfo;

		explicit operator VkPipelineShaderStageCreateInfo() const;
	};

	bool CreatePipelineLayout( Array<VkDescriptorSetLayout>& descriptorSetLayouts,
							   Array<VkPushConstantRange>& pushConstantRanges,
							   VkPipelineLayout& pipelineLayout );
	bool CreatePipelineCacheObject( Array<uint8_t>& cacheData, VkPipelineCache& pipelineCache );

	bool GetPipelineCacheData( VkPipelineCache pipelineCache, Array<uint8_t>& data );
	bool MergePipelineCaches( Array<VkPipelineCache>& cachesToMerge, VkPipelineCache& mergedCache );

	VkGraphicsPipelineCreateInfo GetGraphicsPipelineCreateInfo( Array<ShaderStageParameters>& shaderStages,
																Array<VkVertexInputBindingDescription>& bindingDescriptions,
																Array<VkVertexInputAttributeDescription>& attributeDescriptions,
																VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo,
																VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo,
																Array<VkViewport>& viewports,
																Array<VkRect2D>& rects,
																VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo,
																VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo,
																VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo,
																VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo,
																Array<VkDynamicState>& dynamicStates,
																VkPipelineLayout pipelineLayout,
																VkRenderPass renderPass,
																uint32_t subpass,
																VkPipelineCreateFlags additionalOptions,
																VkPipeline basePipelineHandle,
																int32_t basePipelineIndex );

	bool CreateGraphicsPipelines( Array<VkGraphicsPipelineCreateInfo>& graphicsPipelineCreateInfos,
								  VkPipelineCache& cache, Array<VkPipeline>& pipelines );
	bool CreateComputePipeline( ShaderStageParameters shaderStages, VkPipelineLayout pipelineLayout,
								VkPipelineCache& cache, VkPipelineCreateFlags additionalOptions,
								VkPipeline basePipeline, int32_t basePipelineIndex, VkPipeline& pipeline );

	void BindPipeline( VkCommandBuffer commandBuffer, bool isCompute, VkPipeline pipeline );

	void DestroyPipeline( VkPipeline& pipeline );
	void DestroyPipelineCache( VkPipelineCache& pipelineCache );
	void DestroyPipelineLayout( VkPipelineLayout& pipelineLayout );
}