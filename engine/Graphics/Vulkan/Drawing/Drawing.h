#pragma once

#include "Entities/ComponentHandle.h"
#include <vulkan/vulkan.h>
#include "Graphics/Vulkan/Bindings/MaterialBinding.h"

namespace axlt {
	class RendererComponent;
	class CameraComponent;
	class TransformComponent;
}

namespace axlt::vk {
	class TechniqueBinding;

	int8_t BeginDrawCommand();
	void SubmitDrawCommand( int8_t commandBufferIndex );
	VkCommandBuffer CreateCopyCommandBuffer();
	void SubmitCopyCommandBuffer( VkCommandBuffer copyCommandBuffer );
	
	void UpdatePerCameraBuffer( const ComponentHandle<CameraComponent>& camera,
		const ComponentHandle<TransformComponent>& cameraTransform, uint32_t commandBufferIndex );
	void SetupCamera( const ComponentHandle<CameraComponent>& camera, uint8_t commandBufferIndex );
	void UpdatePerDrawBuffer( ComponentHandle<TransformComponent>& transform,
		const TechniqueBinding& techniqueBinding, VkCommandBuffer commandBuffer );
	void UpdateUniformBuffers( MaterialBinding& materialBinding, MaterialBinding::PerCommandBuffer& perCommandBuffer,
		ComponentHandle<RendererComponent>& renderer );

	void UpdateBindings( ComponentHandle<RendererComponent>& renderer );
	void SubmitUpdateBindings();
	void BindTechnique( const MeshBufferBinding& meshBinding, const TechniqueBinding& techniqueBinding, 
		const MaterialBinding::PerCommandBuffer& materialPerFrameData, VkCommandBuffer commandBuffer );

	bool CullTest( const ComponentHandle<CameraComponent>& camera, ComponentHandle<RendererComponent>& renderer,
		ComponentHandle<TransformComponent>& transform );
}
