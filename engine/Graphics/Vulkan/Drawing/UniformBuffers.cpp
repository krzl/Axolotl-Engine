#include "stdafx.h"
#include "Drawing.h"

#include "Graphics/Vulkan/Bindings/TechniqueBinding.h"
#include "Entities/TransformComponent.h"
#include "Graphics/CameraComponent.h"
#include "Graphics/RendererComponent.h"

namespace axlt::vk {
	void UpdatePerDrawBuffer( ComponentHandle<TransformComponent>& transform,
		const TechniqueBinding& techniqueBinding, const VkCommandBuffer commandBuffer ) {

		perDrawUniformBuffer.modelMatrix = transform->LocalToWorldMatrix();
		perDrawUniformBuffer.pvmMatrix = perCameraUniformBuffer.pvMatrix * perDrawUniformBuffer.modelMatrix;

		vkCmdPushConstants( commandBuffer, techniqueBinding.pipelineLayout, VK_SHADER_STAGE_ALL, 0,
			sizeof( PerDrawUniformBuffer ), &perDrawUniformBuffer );
	}
	
	void SetupCamera( const ComponentHandle<CameraComponent>& camera, const uint8_t commandBufferIndex ) {

		FixedArray<VkClearValue, 2> clearValues( 2 );
		clearValues[ 0 ].color = {
			{
				0.1f,
				0.1f,
				0.1f,
				1.0f,
			}
		};
		clearValues[ 1 ].depthStencil = {
			1.0f,
			0
		};

		VkRenderPassBeginInfo renderPassBeginInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			renderPass,
			framebuffers[commandBufferIndex],
			{
				{
					0,
					0
				},
				swapchainExtents
			},
			clearValues.GetSize(),
			clearValues.GetData()
		};

		vkCmdBeginRenderPass( commandBuffers[commandBufferIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE );
		
		VkViewport viewport = {
			(float) swapchainExtents.width * camera->viewport.GetMinX(),
			(float) swapchainExtents.height * camera->viewport.GetMinY(),
			(float) swapchainExtents.width * camera->viewport.GetMaxX(),
			(float) swapchainExtents.height * camera->viewport.GetMaxY(),
			0,
			1
		};

		VkRect2D scissor = {
			{
				(int32_t) ( swapchainExtents.width * camera->scissor.GetMinX() ),
				(int32_t) ( swapchainExtents.height * camera->scissor.GetMinY() )
			},
			{
				(uint32_t) ( swapchainExtents.width * camera->scissor.GetMaxX() ),
				(uint32_t) ( swapchainExtents.height * camera->scissor.GetMaxY() )
			}
		};
			
		vkCmdSetViewport( commandBuffers[commandBufferIndex], 0, 1, &viewport );
		vkCmdSetScissor( commandBuffers[commandBufferIndex], 0, 1, &scissor );
	}
	
	void UpdatePerCameraBuffer( const ComponentHandle<CameraComponent>& camera, 
		const ComponentHandle<TransformComponent>& cameraTransform, const uint32_t commandBufferIndex ) {
		
		perCameraUniformBuffer.viewMatrix = camera->ViewMatrix();
		perCameraUniformBuffer.projectionMatrix = camera->ProjectionMatrix();
		perCameraUniformBuffer.pvMatrix = perCameraUniformBuffer.projectionMatrix * perCameraUniformBuffer.viewMatrix;
		perCameraUniformBuffer.cameraPosition = cameraTransform->GetPosition();

		void* bufferTempStorage;
		VkResult result = vkMapMemory( device, perCameraBuffersMemory[commandBufferIndex], 0, VK_WHOLE_SIZE, 0, &bufferTempStorage );
		if (result != VK_SUCCESS) {
			printf( "Could not map per camera uniform buffer memory\n" );
			return;
		}
		memcpy( bufferTempStorage, &perCameraUniformBuffer, sizeof PerCameraUniformBuffer );
		vkUnmapMemory( device, perCameraBuffersMemory[commandBufferIndex] );

		VkMappedMemoryRange uniformBufferMemoryRange = {
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			perCameraBuffersMemory[commandBufferIndex],
			0,
			VK_WHOLE_SIZE
		};

		result = vkFlushMappedMemoryRanges( device, 1, &uniformBufferMemoryRange );
		if (result != VK_SUCCESS) {
			printf( "Could not flush per camera uniform buffer memory\n" );
			return;
		}
	}
	
	void UpdateUniformBuffers( MaterialBinding& materialBinding, 
		MaterialBinding::PerCommandBuffer& perCommandBuffer, 
		ComponentHandle<RendererComponent>& renderer ) {

		const BitArray<> dirtyUniformBlocks = renderer->material->PopDirtyUniforms();
		for( auto& perFrameData : materialBinding.perFrameDataArray ) {
			for( uint32_t i = 0; i < dirtyUniformBlocks.GetSize(); i++ ) {
				perFrameData.dirtyUniformBuffers[i] = perFrameData.dirtyUniformBuffers[i] | dirtyUniformBlocks[i];
			}
		}
		
		for( uint32_t i = 0; i < perCommandBuffer.dirtyUniformBuffers.GetSize(); i++ ) {
			if( !perCommandBuffer.dirtyUniformBuffers[i] ) continue;
			if( perCommandBuffer.uniformBuffersMemory[i] == VK_NULL_HANDLE ) continue;

			void* uniformBufferTempStorage;
			VkResult result = vkMapMemory( device, perCommandBuffer.uniformBuffersMemory[i], 
				0, VK_WHOLE_SIZE, 0, &uniformBufferTempStorage );
			
			if( result != VK_SUCCESS ) {
				printf( "Could not map material uniform buffer memory\n" );
				continue;
			}
			
			memcpy( uniformBufferTempStorage, 
					renderer->material->GetUniformData( i ), 
					renderer->material->GetTechnique()->GetShaderUniformBlock( i ).size
			);
			vkUnmapMemory( device, perCommandBuffer.uniformBuffersMemory[i] );

			VkMappedMemoryRange memoryRange = {
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				nullptr,
				perCommandBuffer.uniformBuffersMemory[i],
				0,
				VK_WHOLE_SIZE
			};

			result = vkFlushMappedMemoryRanges( device, 1, &memoryRange );
			if( result != VK_SUCCESS ) {
				printf( "Could not flush material uniform buffer memory\n" );
				continue;
			}

			perCommandBuffer.dirtyUniformBuffers[i] = false;
		}
	}

}
