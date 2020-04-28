// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Pipelines.h"

namespace axlt::vk {

	bool CreatePipelineLayout( Array<VkDescriptorSetLayout>& descriptorSetLayouts,
							   Array<VkPushConstantRange>& pushConstantRanges,
							   VkPipelineLayout* pipelineLayout ) {
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			descriptorSetLayouts.GetSize(),
			descriptorSetLayouts.GetData(),
			pushConstantRanges.GetSize(),
			pushConstantRanges.GetData()
		};

		const VkResult result = vkCreatePipelineLayout( device, &pipelineLayoutCreateInfo, nullptr, pipelineLayout );
		if( VK_SUCCESS != result ) {
			printf( "Could not create pipeline layout\n" );
			return false;
		}

		return true;
	}

	bool CreatePipelineCacheObject( Array<uint8_t>& cacheData, VkPipelineCache& pipelineCache ) {
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
			nullptr,
			0,
			cacheData.GetSize(),
			cacheData.GetData()
		};

		const VkResult result = vkCreatePipelineCache( device, &pipelineCacheCreateInfo, nullptr, &pipelineCache );
		if( VK_SUCCESS != result ) {
			printf( "Could not create pipeline cache object\n" );
			return false;
		}

		return true;
	}

	bool GetPipelineCacheData( VkPipelineCache pipelineCache, Array<uint8_t>& data ) {
		uint64_t dataSize;
		VkResult result = vkGetPipelineCacheData( device, pipelineCache, &dataSize, nullptr );
		if( VK_SUCCESS != result || dataSize == 0 ) {
			printf( "Could not get size of pipeline cache data\n" );
			return false;
		}

		data.Clear();
		data.AddEmpty( (uint32_t) dataSize );

		result = vkGetPipelineCacheData( device, pipelineCache, &dataSize, data.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not get pipeline cache data\n" );
			return false;
		}

		return true;
	}

	bool MergePipelineCaches( Array<VkPipelineCache>& cachesToMerge, VkPipelineCache& mergedCache ) {
		const VkResult result = vkMergePipelineCaches( device, mergedCache, cachesToMerge.GetSize(), cachesToMerge.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not merge pipeline cache objects\n" );
			return false;
		}

		return true;
	}

	bool CreateGraphicsPipeline( Array<ShaderStageParameters>& shaderStages,
								 Array<VkVertexInputBindingDescription>& bindingDescriptions,
								 Array<VkVertexInputAttributeDescription>& attributeDescriptions,
								 VkPipelineInputAssemblyStateCreateInfo& inputAssemblyStateCreateInfo,
								 VkPipelineTessellationStateCreateInfo* tessellationStateCreateInfo,
								 Array<VkViewport>& viewports,
								 Array<VkRect2D>& rects,
								 VkPipelineRasterizationStateCreateInfo& rasterizationStateCreateInfo,
								 VkPipelineMultisampleStateCreateInfo& multisampleStateCreateInfo,
								 VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo,
								 VkPipelineColorBlendStateCreateInfo& colorBlendStateCreateInfo,
								 Array<VkDynamicState>& dynamicStates,
								 VkPipelineLayout pipelineLayout,
								 VkRenderPass renderPass,
								 uint32_t subpass,
								 VkPipelineCreateFlags additionalOptions,
								 VkPipeline basePipelineHandle,
								 int32_t basePipelineIndex,
								 VkPipelineCache* cache,
								 VkPipeline& pipeline ) {

		Array<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos( shaderStages );

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			bindingDescriptions.GetSize(),
			bindingDescriptions.GetData(),
			attributeDescriptions.GetSize(),
			attributeDescriptions.GetData()
		};

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,
			viewports.GetSize(),
			viewports.GetData(),
			rects.GetSize(),
			rects.GetData()
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			nullptr,
			0,
			dynamicStates.GetSize(),
			dynamicStates.GetData()
		};

		const VkGraphicsPipelineCreateInfo createInfo = {
		   VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		   nullptr,
		   additionalOptions,
		   shaderStageCreateInfos.GetSize(),
		   shaderStageCreateInfos.GetData(),
		   &vertexInputStateCreateInfo,
		   &inputAssemblyStateCreateInfo,
		   tessellationStateCreateInfo,
		   &viewportStateCreateInfo,
		   &rasterizationStateCreateInfo,
		   &multisampleStateCreateInfo,
		   &depthStencilStateCreateInfo,
		   &colorBlendStateCreateInfo,
		   &dynamicStateCreateInfo,
		   pipelineLayout,
		   renderPass,
		   subpass,
		   basePipelineHandle,
		   basePipelineIndex
		};
		
		const VkResult result = vkCreateGraphicsPipelines( device, cache == nullptr ? VK_NULL_HANDLE : *cache,
														   1, &createInfo, nullptr, &pipeline );
		if( VK_SUCCESS != result ) {
			printf( "Could not create graphics pipeline\n" );
			return false;
		}

		return true;
	}

	bool CreateGraphicsPipeline( VkGraphicsPipelineCreateInfo& graphicsPipelineCreateInfo,
								 VkPipelineCache& cache, VkPipeline& pipeline ) {
		const VkResult result = vkCreateGraphicsPipelines( device, cache, 1, &graphicsPipelineCreateInfo,
														   nullptr, &pipeline );
		if( VK_SUCCESS != result ) {
			printf( "Could not create graphics pipeline\n" );
			return false;
		}

		return true;
	}

	bool CreateGraphicsPipelines( Array<VkGraphicsPipelineCreateInfo>& graphicsPipelineCreateInfos,
								  VkPipelineCache& cache, Array<VkPipeline>& pipelines ) {

		pipelines.Clear();
		pipelines.AddEmpty( graphicsPipelineCreateInfos.GetSize() );

		const VkResult result = vkCreateGraphicsPipelines( device, cache, graphicsPipelineCreateInfos.GetSize(),
														   graphicsPipelineCreateInfos.GetData(),
														   nullptr, pipelines.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not create graphics pipelines\n" );
			return false;
		}

		return true;
	}

	bool CreateComputePipeline( ShaderStageParameters shaderStages, VkPipelineLayout pipelineLayout,
								VkPipelineCache& cache, VkPipelineCreateFlags additionalOptions,
								VkPipeline basePipeline, int32_t basePipelineIndex, VkPipeline& pipeline ) {

		const VkPipelineShaderStageCreateInfo shaderStageCreateInfo = (VkPipelineShaderStageCreateInfo) shaderStages;

		VkComputePipelineCreateInfo computePipelineCreateInfo = {
			VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			nullptr,additionalOptions,
			shaderStageCreateInfo,
			pipelineLayout,
			basePipeline,
			basePipelineIndex
		};

		const VkResult result = vkCreateComputePipelines( device, cache, 1, &computePipelineCreateInfo, nullptr, &pipeline );
		if( VK_SUCCESS != result ) {
			printf( "Could not create compute pipeline\n" );
			return false;
		}

		return true;
	}

	void BindPipeline( VkCommandBuffer commandBuffer, bool isCompute, VkPipeline pipeline ) {
		vkCmdBindPipeline( commandBuffer, isCompute ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );
	}

	void DestroyPipeline( VkPipeline& pipeline ) {
		if( pipeline != VK_NULL_HANDLE ) {
			vkDestroyPipeline( device, pipeline, nullptr );
			pipeline = VK_NULL_HANDLE;
		}
	}

	void DestroyPipelineCache( VkPipelineCache& pipelineCache ) {
		if( pipelineCache != VK_NULL_HANDLE ) {
			vkDestroyPipelineCache( device, pipelineCache, nullptr );
			pipelineCache = VK_NULL_HANDLE;
		}
	}

	void DestroyPipelineLayout( VkPipelineLayout& pipelineLayout ) {
		if( pipelineLayout != VK_NULL_HANDLE ) {
			vkDestroyPipelineLayout( device, pipelineLayout, nullptr );
			pipelineLayout = VK_NULL_HANDLE;
		}
	}

	ShaderStageParameters::operator VkPipelineShaderStageCreateInfo() const {
		return VkPipelineShaderStageCreateInfo{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			shaderStage,
			shaderModule,
			"main",
			specializationInfo
		};
	}
}