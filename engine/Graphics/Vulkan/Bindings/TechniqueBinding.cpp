#include "stdafx.h"
#include "TechniqueBinding.h"
#include "Common.h"

namespace axlt::vk {
	
	Array<TechniqueBinding::PipelineCreateInfo> TechniqueBinding::awaitingPipelines;
	SparseArray<TechniqueBinding> TechniqueBinding::bindings;

	static VkFormat GetInputFormat( const ShaderType type, const ShaderPrecision precision, const uint8_t vectorSize ) {
		if (vectorSize == 1) {
			switch (type) {
			case ShaderType::Float:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32_SFLOAT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16_SFLOAT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8_SSCALED;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			case ShaderType::Double:
				return VK_FORMAT_R64_SFLOAT;
			case ShaderType::Int:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32_SINT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16_SINT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8_SINT;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			case ShaderType::UInt:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32_UINT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16_UINT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8_UINT;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			default:
				return VK_FORMAT_UNDEFINED;
			}
		}
		else if (vectorSize == 2) {
			switch (type) {
			case ShaderType::Float:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32_SFLOAT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16_SFLOAT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8_SSCALED;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			case ShaderType::Double:
				return VK_FORMAT_R64G64_SFLOAT;
			case ShaderType::Int:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32_SINT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16_SINT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8_SINT;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			case ShaderType::UInt:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32_UINT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16_UINT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8_UINT;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			default:
				return VK_FORMAT_UNDEFINED;
			}
		}
		else if (vectorSize == 3) {
			switch (type) {
			case ShaderType::Float:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32B32_SFLOAT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16B16_SFLOAT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8B8_SSCALED;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			case ShaderType::Double:
				return VK_FORMAT_R64G64B64_SFLOAT;
			case ShaderType::Int:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32B32_SINT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16B16_SINT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8B8_SINT;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			case ShaderType::UInt:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32B32_UINT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16B16_UINT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8B8_UINT;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			default:
				return VK_FORMAT_UNDEFINED;
			}
		}
		else if (vectorSize == 4) {
			switch (type) {
			case ShaderType::Float:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32B32A32_SFLOAT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16B16A16_SFLOAT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8B8A8_SSCALED;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			case ShaderType::Double:
				return VK_FORMAT_R64G64B64A64_SFLOAT;
			case ShaderType::Int:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32B32A32_SINT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16B16A16_SINT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8B8A8_SINT;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			case ShaderType::UInt:
				switch (precision) {
				case ShaderPrecision::Full:
					return VK_FORMAT_R32G32B32A32_UINT;
				case ShaderPrecision::Half:
					return VK_FORMAT_R16G16B16A16_UINT;
				case ShaderPrecision::Low:
					return VK_FORMAT_R8G8B8A8_UINT;
				default:
					return VK_FORMAT_UNDEFINED;
				}
			default:
				return VK_FORMAT_UNDEFINED;
			}
		}

		return VK_FORMAT_UNDEFINED;
	}

	static VkCullModeFlags GetCullMode( const ShaderCullMode cullFace ) {
		switch( cullFace ) {
			case ShaderCullMode::NONE:
				return VK_CULL_MODE_NONE;
			case ShaderCullMode::BACK:
				return VK_CULL_MODE_BACK_BIT;
			case ShaderCullMode::FRONT:
				return VK_CULL_MODE_FRONT_BIT;
			case ShaderCullMode::FRONT_AND_BACK:
				return VK_CULL_MODE_FRONT_AND_BACK;
		}
		throw 0;
	}

	static VkStencilOp GetStencilOperation( const ShaderStencilOperation shaderStencilOperation ) {
		switch( shaderStencilOperation ) {
			case ShaderStencilOperation::KEEP:
				return VK_STENCIL_OP_KEEP;
			case ShaderStencilOperation::ZERO:
				return VK_STENCIL_OP_ZERO;
			case ShaderStencilOperation::REPLACE:
				return VK_STENCIL_OP_REPLACE;
			case ShaderStencilOperation::INCREMENT_CLAMP:
				return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case ShaderStencilOperation::DECREMENT_CLAMP:
				return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			case ShaderStencilOperation::INVERT:
				return VK_STENCIL_OP_INVERT;
			case ShaderStencilOperation::INCREMENT_WRAP:
				return VK_STENCIL_OP_INCREMENT_AND_WRAP;
			case ShaderStencilOperation::DECREMENT_WRAP:
				return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		}
		throw 0;
	}

	static VkStencilOpState GetStencilOpState( const ShaderStencilInfo& stencilInfo ) {
		return VkStencilOpState {
			GetStencilOperation( stencilInfo.failOp ),
			GetStencilOperation( stencilInfo.passOp ),
			GetStencilOperation( stencilInfo.depthFailOp ),
			GetCompareOperation( stencilInfo.compareOp ),
			stencilInfo.compareMask,
			stencilInfo.writeMask,
			stencilInfo.reference
		};
	}

	static VkBlendFactor GetBlendFactor( const ShaderBlendFactor blendFactor ) {
		switch( blendFactor ) {
			case ShaderBlendFactor::ZERO:
				return VK_BLEND_FACTOR_ZERO;
			case ShaderBlendFactor::ONE:
				return VK_BLEND_FACTOR_ONE;
			case ShaderBlendFactor::SRC_COLOR:
				return VK_BLEND_FACTOR_SRC_COLOR;
			case ShaderBlendFactor::ONE_MINUS_SRC_COLOR:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case ShaderBlendFactor::DST_COLOR:
				return VK_BLEND_FACTOR_DST_COLOR;
			case ShaderBlendFactor::ONE_MINUS_DST_COLOR:
				return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case ShaderBlendFactor::SRC_ALPHA:
				return VK_BLEND_FACTOR_SRC_ALPHA;
			case ShaderBlendFactor::ONE_MINUS_SRC_ALPHA:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case ShaderBlendFactor::DST_ALPHA:
				return VK_BLEND_FACTOR_DST_ALPHA;
			case ShaderBlendFactor::ONE_MINUS_DST_ALPHA:
				return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		}
		throw 0;
	}

	static VkBlendOp GetBlendOperation( const ShaderBlendOperation blendOperation ) {
		switch( blendOperation ) {
			case ShaderBlendOperation::ADD:
				return VK_BLEND_OP_ADD;
			case ShaderBlendOperation::SUBTRACT:
				return VK_BLEND_OP_SUBTRACT;
			case ShaderBlendOperation::REVERSE_SUBTRACT:
				return VK_BLEND_OP_REVERSE_SUBTRACT;
			case ShaderBlendOperation::MIN:
				return VK_BLEND_OP_MIN;
			case ShaderBlendOperation::MAX:
				return VK_BLEND_OP_MAX;
		}
		throw 0;
	}

	bool CreateShaderModule( const ResourceHandle<BinaryResource>& source, 
		const VkShaderStageFlagBits stage, Array<ShaderModuleInfo>& modules ) {

		ShaderModuleInfo& moduleInfo = modules.Emplace();
		moduleInfo.stage = stage;
		
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,
			source->binaryData.GetSize(),
			reinterpret_cast<const uint32_t*>( source->binaryData.GetData() )
		};

		const VkResult result = vkCreateShaderModule( device, &shaderModuleCreateInfo, nullptr, &moduleInfo.module );
		if( VK_SUCCESS != result ) {
			printf( "Could not create shader module\n" );
			return false;
		}

		return true;
	}
	
	TechniqueBinding& TechniqueBinding::CreateBinding( const ResourceHandle<TechniqueResource>& technique ) {
		TechniqueBinding& binding = *(TechniqueBinding*) bindings.Emplace().element;
		bindingMap.Add( technique->GetInstanceId(), &binding );

		return binding;
	}

	void TechniqueBinding::UpdateBinding( const ResourceHandle<TechniqueResource>& technique ) {
		
		if( !CreateShaderModule( technique->vertexShader, VK_SHADER_STAGE_VERTEX_BIT, shaderModuleInfos ) ) {
			return;
		}
		
		if( !CreateShaderModule( technique->fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, shaderModuleInfos ) ) {
			return;
		}

		int16_t lastLayoutSetIndex = -1;

		for( uint32_t i = 0; i < technique->uniformBlocks.GetSize(); i++ ) {
			if( technique->uniformBlocks[i].set > 7 ) {
				continue;
			}
			lastLayoutSetIndex = max( lastLayoutSetIndex, technique->uniformBlocks[ i ].set );
		}
		for( uint32_t i = 0; i < technique->samplers.GetSize(); i++ ) {
			if( technique->samplers[i].set > 7 ) {
				continue;
			}
			lastLayoutSetIndex = max( lastLayoutSetIndex, technique->samplers[ i ].set );
		}

		layouts.AddEmpty( lastLayoutSetIndex + 1 );

		for( uint32_t layoutId = 0; layoutId < layouts.GetSize(); layoutId++ ) {
			Array<VkDescriptorSetLayoutBinding> layoutBindings;

			for( uint32_t i = 0; i < technique->uniformBlocks.GetSize(); i++ ) {
				if( technique->uniformBlocks[i].set == layoutId ) {
					VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.Emplace();
					layoutBinding = {
						technique->uniformBlocks[i].binding,
						VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						1,
						(uint32_t)technique->uniformBlocks[i].shaderStages,
						nullptr
					};
				}
			}

			for( uint32_t i = 0; i< technique->samplers.GetSize(); i++ ) {
				if( technique->samplers[i].set == layoutId ) {
					VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.Emplace();
					layoutBinding = {
						technique->samplers[i].binding,
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						technique->samplers[i].count,
						(uint32_t) technique->samplers[i].shaderStages,
						nullptr
					};
				}
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				nullptr,
				0,
				layoutBindings.GetSize(),
				layoutBindings.GetData()
			};

			const VkResult result = vkCreateDescriptorSetLayout( device, &descriptorSetLayoutCreateInfo, nullptr, &layouts[layoutId] );
			if( VK_SUCCESS != result ) {
				printf( "Could not create descriptor set layout\n" );
				return;
			}
		}

		Array<VkPushConstantRange> pushConstants = {
			{
				VK_SHADER_STAGE_ALL,
				0,
				sizeof( PerDrawUniformBuffer )
			}
		};

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			layouts.GetSize(),
			layouts.GetData(),
			pushConstants.GetSize(),
			pushConstants.GetData()
		};

		const VkResult result = vkCreatePipelineLayout( device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout );
		if( VK_SUCCESS != result ) {
			printf( "Could not create pipeline layout\n" );
		}
	}

	static uint8_t GetPreviousUsedMeshAttributeLocation( uint8_t location, const uint16_t meshLayout ) {
		while( location != 0 ) {
			if( Mesh::GetAttributeVectorSize( location, meshLayout ) != 0 ) {
				return location;
			}
			location--;
		}
		return 0;
	}

	void TechniqueBinding::CreatePipeline( const ResourceHandle<TechniqueResource>& technique, const uint16_t meshLayout ) {

		PipelineCreateInfo& createInfo = awaitingPipelines.Emplace( this, meshLayout );

		createInfo.shaderStages = shaderModuleInfos;

		createInfo.inputBindings.Emplace( VkVertexInputBindingDescription{
			0,
			Mesh::GetMeshStride( meshLayout ),
			VK_VERTEX_INPUT_RATE_VERTEX
		} );

		createInfo.inputAttributes.AddEmpty( technique->inputs.GetSize() );

		for (uint32_t i = 0; i < technique->inputs.GetSize(); i++) {
			const ShaderInputElement& input = technique->inputs[ i ];
			const uint8_t location = GetPreviousUsedMeshAttributeLocation( input.location, meshLayout );
			const uint16_t offset = Mesh::GetAttributeOffset( location, meshLayout );
			VkVertexInputAttributeDescription& inputAttribute = createInfo.inputAttributes[ i ];
			inputAttribute = {
				input.location,
				0,
				GetInputFormat( input.type, input.precision, input.vectorSize ),
				offset
			};
		}

		createInfo.inputAssemblyInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			false
		};

		createInfo.rasterizationInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			technique->depthClampEnabled,
			VK_FALSE,
			VK_POLYGON_MODE_FILL,
			GetCullMode( technique->cullMode ),
			VK_FRONT_FACE_CLOCKWISE,
			technique->depthBiasEnabled,
			technique->depthBiasConstantFactor,
			technique->depthBiasClamp,
			technique->depthBiasSlopeFactor,
			1.0f
		};

		createInfo.multisampleInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			0,
			GetSampleCount( technique->rasterizationSamples ),
			VK_FALSE,
			0.0f,
			nullptr,
			technique->alphaToCoverage,
			VK_FALSE
		};

		createInfo.depthStencilInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			technique->depthTestEnabled,
			technique->depthWriteEnabled,
			GetCompareOperation( technique->depthTestOp ),
			VK_FALSE,
			technique->stencilTestEnabled,
			GetStencilOpState( technique->frontStencilOperation ),
			GetStencilOpState( technique->backStencilOperation ),
			0.0f,
			1.0f
		};

		createInfo.blendAttachmentState.AddEmpty( 1 );
		createInfo.blendAttachmentState[ 0 ] = {
			technique->blendEnable,
			GetBlendFactor( technique->blendInfo.colorSrcBlendFactor ),
			GetBlendFactor( technique->blendInfo.colorDstBlendFactor ),
			GetBlendOperation( technique->blendInfo.colorBlendOperation ),
			GetBlendFactor( technique->blendInfo.alphaSrcBlendFactor ),
			GetBlendFactor( technique->blendInfo.alphaDstBlendFactor ),
			GetBlendOperation( technique->blendInfo.alphaBlendOperation ),
			technique->colorMask
		};

		createInfo.colorBlendInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_LOGIC_OP_CLEAR,
			createInfo.blendAttachmentState.GetSize(),
			createInfo.blendAttachmentState.GetData(),
			{
				0.0f,
				0.0f,
				0.0f,
				0.0f
			}
		};

		meshLayoutToPipelineMap.Add( meshLayout, VK_NULL_HANDLE );
	}

	VkPipeline TechniqueBinding::GetPipeline( const uint16_t meshLayout ) const {
		const VkPipeline* pipelinePtr = meshLayoutToPipelineMap.Find( meshLayout );
		return pipelinePtr == nullptr ? VK_NULL_HANDLE : *pipelinePtr;
	}

	bool TechniqueBinding::CheckIfPipelineExists( const uint16_t meshLayout ) const {
		return meshLayoutToPipelineMap.Find( meshLayout ) != nullptr;
	}

	void TechniqueBinding::CreateAwaitingPipelines() {
		if( awaitingPipelines.GetSize() == 0 ) return;

		ExactArray<VkGraphicsPipelineCreateInfo> pipelineCreateInfos( awaitingPipelines );
		Array<VkPipeline> pipelines( awaitingPipelines.GetSize() );

		const VkResult result = vkCreateGraphicsPipelines( device, VK_NULL_HANDLE, awaitingPipelines.GetSize(),
			pipelineCreateInfos.GetData(), nullptr, pipelines.GetData() );
		if( result != VK_SUCCESS ) {
			printf( "Could not create graphics pipelines\n" );
			return;
		}

		for( uint32_t i = 0; i < awaitingPipelines.GetSize(); i++ ) {
			awaitingPipelines[i].techniqueBinding->meshLayoutToPipelineMap[awaitingPipelines[i].meshLayout] = pipelines[i];
		}
		awaitingPipelines.Clear();
	}

	void TechniqueBinding::Reset() {
		for( auto& layout : layouts ) {
			vkDestroyDescriptorSetLayout( device, layout, nullptr );
		}
		layouts.Clear();
		
		for( auto& shaderModule : shaderModuleInfos ) {
			vkDestroyShaderModule( device, shaderModule.module, nullptr );
		}
		shaderModuleInfos.Clear();

		if( pipelineLayout != VK_NULL_HANDLE ) {
			vkDestroyPipelineLayout( device, pipelineLayout, nullptr );
			pipelineLayout = VK_NULL_HANDLE;
		}

		for( auto& kvp : meshLayoutToPipelineMap ) {
			if( kvp.value != VK_NULL_HANDLE ) {
				vkDestroyPipeline( device, kvp.value, nullptr );
			}
		}
		meshLayoutToPipelineMap.Clear();
	}

	void TechniqueBinding::RemoveBinding() {
		const uint64_t ptrDiff = (uint64_t) this - (uint64_t) bindings.GetData();
		bindings.Remove( (uint32_t) ( ptrDiff / sizeof( TechniqueBinding ) ) );
	}

	TechniqueBinding::PipelineCreateInfo::PipelineCreateInfo( TechniqueBinding* techniqueBinding, const uint16_t meshLayout ) :
		techniqueBinding( techniqueBinding ),
		meshLayout( meshLayout ) {}

	TechniqueBinding::PipelineCreateInfo::operator struct VkGraphicsPipelineCreateInfo() {
		
		vertexInputStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			inputBindings.GetSize(),
			inputBindings.GetData(),
			inputAttributes.GetSize(),
			inputAttributes.GetData()
		};

		viewports = Array<VkViewport>{
			{
				0,
				0,
				(float) swapchainExtents.width,
				(float) swapchainExtents.height,
				0,
				1
			}
		};

		scissorRects = Array<VkRect2D>{
			{
				{
					0,
					0
				},
				{
					swapchainExtents.width,
					swapchainExtents.height
				}
			}
		};

		viewportStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,
			viewports.GetSize(),
			viewports.GetData(),
			scissorRects.GetSize(),
			scissorRects.GetData()
		};

		dynamicStates = Array<VkDynamicState>{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		dynamicStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			nullptr,
			0,
			dynamicStates.GetSize(),
			dynamicStates.GetData()
		};

		return VkGraphicsPipelineCreateInfo{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			nullptr,
			0,
			shaderStages.GetSize(),
			shaderStages.GetData(),
			&vertexInputStateCreateInfo,
			&inputAssemblyInfo,
			nullptr,
			&viewportStateCreateInfo,
			&rasterizationInfo,
			&multisampleInfo,
			&depthStencilInfo,
			&colorBlendInfo,
			&dynamicStateCreateInfo,
			techniqueBinding->pipelineLayout,
			renderPass,
			0,
			VK_NULL_HANDLE,
			0
		};
	}

	TechniqueBinding::PipelineCreateInfo::operator struct VkGraphicsPipelineCreateInfo() const {
		PipelineCreateInfo& nonConst = const_cast<PipelineCreateInfo&>(*this);
		return (VkGraphicsPipelineCreateInfo) nonConst;
	}
}
