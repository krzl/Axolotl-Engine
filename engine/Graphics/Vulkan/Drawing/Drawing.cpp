#include "stdafx.h"
#include "Drawing.h"

#include "../Vulkan.h"
#include "Graphics/RenderSystem.h"
#include "Graphics/Vulkan/Bindings/VulkanBinding.h"
#include "Graphics/CameraComponent.h"
#include "Graphics/Vulkan/Bindings/ModelBufferBinding.h"
#include "Graphics/Vulkan/Bindings/TechniqueBinding.h"
#include "Graphics/Vulkan/Bindings/MaterialBinding.h"

namespace axlt::vk {

	void Draw() {
		const int8_t commandBufferIndex = BeginDrawCommand();
		if( commandBufferIndex == -1 ) {
			return;
		}
		VkCommandBuffer& commandBuffer = commandBuffers[commandBufferIndex];

		VulkanBinding::CheckDelayedDestroy();

		auto& rendererComponents = RenderSystem::GetSystem()->GetTuples();
		Array<ComponentHandle<CameraComponent>>& cameras = CameraComponent::GetAllCameras();
		for( ComponentHandle<CameraComponent> camera : cameras ) {
			ComponentHandle<TransformComponent> cameraTransform = camera->GetEntity().GetComponent<TransformComponent>();
			if( !cameraTransform.IsValidHandle() ) continue;

			UpdatePerCameraBuffer( camera, cameraTransform, commandBufferIndex );
			SetupCamera( camera, commandBufferIndex );
			UpdateCameraBindings( camera );

			for( auto& entityRendererPair : rendererComponents ) {
				auto&[ transform, renderer ] = entityRendererPair.value;

				if( !CullTest( camera, renderer, transform ) ) {
					continue;
				}

				UpdateBindings( renderer );

				ModelBufferBinding& modelBinding = *VulkanBinding::GetBinding<ModelBufferBinding>( renderer->model );
				TechniqueBinding& techniqueBinding = *VulkanBinding::GetBinding<TechniqueBinding>( renderer->material->GetTechnique() );
				MaterialBinding& materialBinding = *VulkanBinding::GetBinding<MaterialBinding>( renderer->material );
				MaterialBinding::PerCommandBuffer& materialPerFrameData = materialBinding.perFrameDataArray[commandBufferIndex];

				UpdatePerDrawBuffer( transform, techniqueBinding, commandBuffer );
				UpdateUniformBuffers( materialBinding, materialPerFrameData, renderer );

				for( MeshBufferBinding* meshBinding : modelBinding.meshBindings ) {
					BindTechnique( *meshBinding, techniqueBinding, materialPerFrameData, commandBuffer );
					VkDeviceSize zeroOffset = 0;
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, &meshBinding->vertexBuffer, &zeroOffset );
					vkCmdBindIndexBuffer( commandBuffer, meshBinding->indexBuffer, 0, VK_INDEX_TYPE_UINT32 );

					vkCmdDrawIndexed( commandBuffer, meshBinding->indexCount, 1, 0, 0, 0 );
				}
			}
			
			vkCmdEndRenderPass( commandBuffer );
		}
		
		SubmitUpdateBindings();
		SubmitDrawCommand( commandBufferIndex );
	}

}
