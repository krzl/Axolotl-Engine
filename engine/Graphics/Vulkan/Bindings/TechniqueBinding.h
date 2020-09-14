#pragma once

#include "VulkanBinding.h"

namespace axlt::vk {

	class MaterialBinding;

	struct ShaderModuleInfo {
		VkShaderStageFlagBits stage;
		VkShaderModule module;

		explicit operator VkPipelineShaderStageCreateInfo() const {
			return VkPipelineShaderStageCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				stage,
				module,
				"main",
				nullptr
			};
		}

		explicit operator VkShaderModule() const {
			return module;
		}
	};
	
	class TechniqueBinding final : public VulkanBinding {

		friend class MaterialBinding;
		
	public:

		static TechniqueBinding& CreateBinding( const ResourceHandle<TechniqueResource>& technique );
		void UpdateBinding( const ResourceHandle<TechniqueResource>& technique );
		void CreatePipeline( const ResourceHandle<TechniqueResource>& technique, uint16_t meshLayout );
		VkPipeline GetPipeline( uint16_t meshLayout ) const;
		bool CheckIfPipelineExists( uint16_t meshLayout ) const;

		static void CreateAwaitingPipelines();

		struct PipelineCreateInfo {

		public:

			Array<VkPipelineShaderStageCreateInfo> shaderStages{};
			Array<VkVertexInputBindingDescription> inputBindings{};
			Array<VkVertexInputAttributeDescription> inputAttributes{};
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
			//VkPipelineTessellationStateCreateInfo* tessellationInfo{};
			VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
			VkPipelineMultisampleStateCreateInfo multisampleInfo{};
			VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
			VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
			Array<VkPipelineColorBlendAttachmentState> blendAttachmentState{};

			TechniqueBinding* techniqueBinding;
			uint16_t meshLayout;

			explicit PipelineCreateInfo( TechniqueBinding* techniqueBinding, uint16_t meshLayout );

			explicit operator VkGraphicsPipelineCreateInfo();
			explicit operator VkGraphicsPipelineCreateInfo() const;

		private:

			VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
			Array<VkViewport> viewports{};
			Array<VkRect2D> scissorRects{};
			VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
			Array<VkDynamicState> dynamicStates{};
			VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};

		};

		friend struct PipelineCreateInfo;
		
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		Array<VkDescriptorSetLayout> layouts{};

	private:

		void Reset() override;
		void RemoveBinding() override;

		static Array<PipelineCreateInfo> awaitingPipelines;
		static SparseArray<TechniqueBinding> bindings;

		Array<ShaderModuleInfo> shaderModuleInfos{};
		Map<uint16_t, VkPipeline> meshLayoutToPipelineMap{};
	};
}
