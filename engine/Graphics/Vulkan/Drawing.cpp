#include "stdafx.h"
#include "Vulkan.h"
#include "Graphics/RenderSystem.h"
#include "Graphics/CameraComponent.h"

namespace axlt::vk {

	VkDeviceSize emptyOffsets[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	void Draw() {
		uint32_t currentCommandBufferIndex;
		VkResult result = vkAcquireNextImageKHR( device, swapchain, 1000000000u, imageAvailableSemaphore, VK_NULL_HANDLE, &currentCommandBufferIndex );
		if( result != VK_SUCCESS ) {
			printf( "Could not acquire next image\n" );
			return;
		}
		
		result = vkWaitForFences( device, 1, &renderFences[currentCommandBufferIndex], VK_TRUE, 1000000000 );
		if( result != VK_SUCCESS ) {
			printf( "Could not wait for current render fence\n" );
			return;
		}

		result = vkResetFences( device, 1, &renderFences[currentCommandBufferIndex] );
		if( result != VK_SUCCESS ) {
			printf( "Could not reset current render fence\n" );
			return;
		}
	
		VkCommandBuffer& currentCommandBuffer = commandBuffers[currentCommandBufferIndex];
		result = vkResetCommandBuffer( currentCommandBuffer, 0 );
		if( result != VK_SUCCESS ) {
			printf( "Could not reset command buffer\n" );
			return;
		}

		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			nullptr
		};

		result = vkBeginCommandBuffer( currentCommandBuffer, &commandBufferBeginInfo );
		if( result != VK_SUCCESS ) {
			printf( "Could not begin command buffer\n" );
			return;
		}

		VkImageMemoryBarrier presentToDrawBarrier = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			queueFamilyIndices[0] == presentationQueueFamilyIndex ? VK_QUEUE_FAMILY_IGNORED : presentationQueueFamilyIndex,
			queueFamilyIndices[0] == presentationQueueFamilyIndex ? VK_QUEUE_FAMILY_IGNORED : queueFamilyIndices[0],
			swapchainImages[currentCommandBufferIndex],
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				1,
				0,
				1
			}
		};

		vkCmdPipelineBarrier( currentCommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
							  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &presentToDrawBarrier );

		auto& cameras = CameraComponent::GetAllCameras();
		for( CameraComponent& camera : cameras ) {
			if( !camera.IsEnabled() ) continue;

			ComponentHandle<TransformComponent> cameraTransform = camera.GetEntity().GetComponent<TransformComponent>();
			if( !cameraTransform.IsValidHandle() ) continue;
			perCameraUniformBuffer.viewMatrix = camera.ViewMatrix();
			perCameraUniformBuffer.projectionMatrix = camera.ProjectionMatrix();
			perCameraUniformBuffer.pvMatrix = perCameraUniformBuffer.projectionMatrix * perCameraUniformBuffer.viewMatrix;
			perCameraUniformBuffer.cameraPosition = cameraTransform->GetPosition();

			void* bufferTempStorage;
			result = vkMapMemory( device, perCameraBuffersMemory[currentCommandBufferIndex], 0, VK_WHOLE_SIZE, 0, &bufferTempStorage );
			if( result != VK_SUCCESS ) {
				printf( "Could not map per camera uniform buffer memory\n" );
				return;
			}
			memcpy( bufferTempStorage, &perCameraUniformBuffer, sizeof PerCameraUniformBuffer );
			vkUnmapMemory( device, perCameraBuffersMemory[currentCommandBufferIndex] );

			VkMappedMemoryRange uniformBufferMemoryRange = {
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				nullptr,
				perCameraBuffersMemory[currentCommandBufferIndex],
				0,
				VK_WHOLE_SIZE
			};
			
			result = vkFlushMappedMemoryRanges( device, 1, &uniformBufferMemoryRange );
			if( result != VK_SUCCESS ) {
				printf( "Could not flush per camera uniform buffer memory\n" );
				return;
			}

			VkClearValue clearColor = {
				{
					{
						0.1f,
						0.1f,
						0.1f,
						1.0f
					}
				}
			};

			VkRenderPassBeginInfo renderPassBeginInfo = {
				VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				nullptr,
				renderPass,
				framebuffers[currentCommandBufferIndex],
				{
					{
						0,
						0
					},
					swapchainExtents
				},
				1,
				&clearColor
			};

			vkCmdBeginRenderPass( currentCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE );

			auto& objectsToRender = g_RenderSystem_instance.m_componentTuples;
			for( auto& entityTuplePair : objectsToRender ) {
				auto&[transform, renderer] = entityTuplePair.value;

				DrawBuffers& drawBuffers = *meshBuffers.Find( renderer->model.guid );
				TechniqueData& techniqueData = *techniqueDataArray.Find( renderer->material->technique.guid );
				MaterialData& materialData = *materialDataArray.Find( renderer->material.guid );
				MaterialData::PerCommandBuffer& materialPerFrameData = materialData.perFrameData[currentCommandBufferIndex];

				perDrawUniformBuffer.modelMatrix = transform->LocalToWorldMatrix();
				perDrawUniformBuffer.pvmMatrix = perCameraUniformBuffer.pvMatrix * perDrawUniformBuffer.modelMatrix;

				vkCmdPushConstants( currentCommandBuffer, techniqueData.pipelineLayout, VK_SHADER_STAGE_ALL, 0,
									sizeof( PerDrawUniformBuffer ), &perDrawUniformBuffer );

				vkCmdBindDescriptorSets( currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, techniqueData.pipelineLayout, 0,
										 techniqueData.layouts.GetSize(), materialPerFrameData.descriptorSets.GetData(), 0, nullptr );

				vkCmdBindPipeline( currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, techniqueData.pipeline );

				const uint32_t meshCount = drawBuffers.buffers.GetSize() / buffersPerMesh;
				for( uint32_t i = 0; i < meshCount; i++ ) {
					uint32_t startAdjacentBuffersIndex = 0xFFFFFFFF;
					uint32_t currentBinding = 0;
					for( uint32_t j = 0; j < buffersPerMesh - 1; j++ ) {
						if( drawBuffers.buffers[i * buffersPerMesh + j] != VK_NULL_HANDLE &&
							( techniqueData.usedBuffers & ( 1 << j ) ) != 0 ) {
							if( startAdjacentBuffersIndex == 0xFFFFFFFF ) {
								startAdjacentBuffersIndex = j;
							}
						} else {
							if( startAdjacentBuffersIndex != 0xFFFFFFFF ) {
								vkCmdBindVertexBuffers( currentCommandBuffer, currentBinding, j - startAdjacentBuffersIndex,
														&drawBuffers.buffers[i * buffersPerMesh + j - startAdjacentBuffersIndex - 1], emptyOffsets );

								currentBinding += j - startAdjacentBuffersIndex;
								startAdjacentBuffersIndex = 0xFFFFFFFF;
							}
						}
					}

					vkCmdBindIndexBuffer( currentCommandBuffer, drawBuffers.buffers[( i + 1 ) * buffersPerMesh - 1], 0, VK_INDEX_TYPE_UINT32 );

					vkCmdDrawIndexed( currentCommandBuffer, renderer->model->meshes[i].indices.GetSize(), 1, 0, 0, 0 );
				}
			}

			vkCmdEndRenderPass( currentCommandBuffer );
		}

		if( queueFamilyIndices[0] != presentationQueueFamilyIndex ) {

			VkImageMemoryBarrier drawToPresentBarrier = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				queueFamilyIndices[0],
				presentationQueueFamilyIndex,
				swapchainImages[currentCommandBufferIndex],
				{
					VK_IMAGE_ASPECT_COLOR_BIT,
					0,
					1,
					0,
					1
				}
			};

			vkCmdPipelineBarrier( currentCommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr,
								  1, &drawToPresentBarrier );
		}

		result = vkEndCommandBuffer( currentCommandBuffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not end command buffer\n" );
			return;
		}

		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		VkSubmitInfo queueSubmitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			1,
			&imageAvailableSemaphore,
			&waitDstStageMask,
			1,
			&currentCommandBuffer,
			1,
			&renderingFinishedSemaphore
		};
		
		result = vkQueueSubmit( queues[0][0], 1, &queueSubmitInfo, renderFences[ currentCommandBufferIndex ] );
		if( result != VK_SUCCESS ) {
			printf( "Could not submit queue\n" );
			return;
		}

		VkPresentInfoKHR presentInfo = {
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			nullptr,
			1,
			&renderingFinishedSemaphore,
			1,
			&swapchain,
			&currentCommandBufferIndex,
			nullptr
		};

		result = vkQueuePresentKHR( presentationQueue, &presentInfo );
		if( result != VK_SUCCESS ) {
			printf( "Could not present queue\n" );
			return;
		}


	}
}
