#include "stdafx.h"
#include "Drawing.h"

#include "Graphics/Vulkan/Bindings/ModelBufferBinding.h"
#include "Graphics/RendererComponent.h"
#include "Graphics/Vulkan/Bindings/TechniqueBinding.h"
#include "Graphics/Vulkan/Bindings/TextureBinding.h"

namespace axlt::vk {
	void UpdateBindings( ComponentHandle<RendererComponent>& renderer ) {

		ModelBufferBinding* modelBinding = VulkanBinding::GetBinding<ModelBufferBinding>( renderer->model );
		if( modelBinding == nullptr ) {
			modelBinding = &ModelBufferBinding::CreateBinding( renderer->model );
			modelBinding->UpdateBinding( renderer->model );
		} else if( renderer->model->isDirty ) {
			VulkanBinding::Destroy( renderer->model->GetInstanceId(), commandBuffers.GetSize() );
			modelBinding = &ModelBufferBinding::CreateBinding( renderer->model );
			modelBinding->UpdateBinding( renderer->model );
			renderer->model->isDirty = false;
		}

		TechniqueBinding* techniqueBinding = VulkanBinding::GetBinding<TechniqueBinding>( renderer->material->GetTechnique() );
		if( techniqueBinding == nullptr ) {
			techniqueBinding = &TechniqueBinding::CreateBinding( renderer->material->GetTechnique() );
			techniqueBinding->UpdateBinding( renderer->material->GetTechnique() );
		} else if( renderer->material->GetTechnique()->isDirty ) {
			VulkanBinding::Destroy( renderer->material->GetTechnique()->GetInstanceId(), commandBuffers.GetSize() );
			techniqueBinding = &TechniqueBinding::CreateBinding( renderer->material->GetTechnique() );
			techniqueBinding->UpdateBinding( renderer->material->GetTechnique() );
		}
		for( auto& meshBinding : modelBinding->meshBindings ) {
			if (!techniqueBinding->CheckIfPipelineExists( meshBinding->GetMeshLayout() )) {
				techniqueBinding->CreatePipeline( renderer->material->GetTechnique(), meshBinding->GetMeshLayout() );
			}
		}

		for( const auto& tex : renderer->material->GetTextureParameters() ) {
			const ShaderSampler* sampler = renderer->material->GetTechnique()->GetShaderSampler( tex.key );
			if( sampler == nullptr ) {
				continue;
			}

			TextureBinding* textureBinding = VulkanBinding::GetBinding<TextureBinding>( tex.value );
			if( textureBinding == nullptr ) {
				textureBinding = &TextureBinding::CreateBinding( tex.value );
				textureBinding->UpdateBinding( tex.value );
			} else if( renderer->material->GetTechnique()->isDirty ) {
				VulkanBinding::Destroy( tex.value->GetInstanceId(), commandBuffers.GetSize() );
				textureBinding = &TextureBinding::CreateBinding( tex.value );
				textureBinding->UpdateBinding( tex.value );
			}
		}

		MaterialBinding* materialBinding = VulkanBinding::GetBinding<MaterialBinding>( renderer->material );
		if( materialBinding == nullptr ) {
			materialBinding = &MaterialBinding::CreateBinding( renderer->material );
			materialBinding->UpdateBinding( renderer->material );
		} else if( renderer->material->GetTechnique()->GetInstanceId() != materialBinding->techniqueId ) {
			VulkanBinding::Destroy( renderer->material->GetInstanceId(), commandBuffers.GetSize() );
			materialBinding = &MaterialBinding::CreateBinding( renderer->material );
			materialBinding->UpdateBinding( renderer->material );
		}

		renderer->material->GetTechnique()->isDirty = false;
		
		MaterialBinding::CreateAwaitingDescriptorSets();
	}

	void SubmitUpdateBindings() {
		if( MeshBufferBinding::HasAwaitingStagingBuffers() || TextureBinding::HasAwaitingStagingBuffers() ) {
			const VkCommandBuffer copyCommandBuffer = CreateCopyCommandBuffer();
			if( copyCommandBuffer == VK_NULL_HANDLE ) {
				return;
			}
			MeshBufferBinding::SubmitAwaitingStagingBuffers( copyCommandBuffer );
			TextureBinding::SubmitAwaitingStagingBuffers( copyCommandBuffer );

			SubmitCopyCommandBuffer( copyCommandBuffer );

			MeshBufferBinding::CleanupAwaitingStagingBuffers();
			TextureBinding::CleanupAwaitingStagingBuffers();
		}
	}

	void BindTechnique( const MeshBufferBinding& meshBinding, const TechniqueBinding& techniqueBinding,
		const MaterialBinding::PerCommandBuffer& materialPerFrameData, const VkCommandBuffer commandBuffer ) {

		TechniqueBinding::CreateAwaitingPipelines();
		
		vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, techniqueBinding.pipelineLayout,
			0, techniqueBinding.layouts.GetSize(), materialPerFrameData.descriptorSets.GetData(), 0, nullptr );
		vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, techniqueBinding.GetPipeline( meshBinding.GetMeshLayout() ) );
	}
}
