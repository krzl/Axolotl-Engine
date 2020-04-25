#include "stdafx.h"
#include "Vulkan.h"
#include "../RenderSystem.h"
#include "Graphics/CameraComponent.h"
#include "FileSystem/Guid.h"

namespace axlt::vk {

	struct PerCameraUniformBuffer {
		Matrix4 projectionMatrix;
		Matrix4 viewMatrix;
		Matrix4 pvMatrix;
		Vector3 cameraPosition;
	} perCameraUniformBuffer;

	struct PerDrawUniformBuffer {
		Matrix4 modelMatrix;
		Matrix4 vmMatrix;
		Matrix4 pvmMatrix;
	} perDrawUniformBuffer;

	struct DrawBuffers {
		Array<VkBuffer, ExactHeapArrayAllocator> vertexBuffers;
		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	Map<Guid, Array<DrawBuffers, ExactHeapArrayAllocator>> meshBuffersMap;

	Array<DrawBuffers, ExactHeapArrayAllocator>& CreateMeshBuffers( const ResourceHandle<ModelResource>& model ) {
		auto& buffersArray = meshBuffersMap.Add( model.guid, Array<DrawBuffers, ExactHeapArrayAllocator>() );

		buffersArray.AddEmpty( model->meshes.GetSize() );
		for( uint32_t i = 0; i < model->meshes.GetSize(); i++ ) {
			DrawBuffers& drawBuffers = buffersArray[i];
			auto& mesh = model->meshes[i];
			drawBuffers.vertexBuffers.AddEmpty( 4 + MAX_COLOR_CHANNELS + MAX_COLOR_CHANNELS );
			if( mesh.vertices.GetSize() != 0 ) {
				VkBuffer& buffer = drawBuffers.vertexBuffers[0];
				CreateBuffer( mesh.vertices.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
			}
			if( mesh.normals.GetSize() != 0 ) {
				VkBuffer& buffer = drawBuffers.vertexBuffers[1];
				CreateBuffer( mesh.normals.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
			}
			if( mesh.tangents.GetSize() != 0 ) {
				VkBuffer& buffer = drawBuffers.vertexBuffers[2];
				CreateBuffer( mesh.tangents.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
			}
			if( mesh.bitangents.GetSize() != 0 ) {
				VkBuffer& buffer = drawBuffers.vertexBuffers[3];
				CreateBuffer( mesh.bitangents.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
			}
			for( uint8_t j = 0; j < MAX_COLOR_CHANNELS; j++ ) {
				if( mesh.colorChannels[j].GetSize() != 0 ) {
					VkBuffer& buffer = drawBuffers.vertexBuffers[4 + j];
					CreateBuffer( mesh.colorChannels[j].GetSize() * 4 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
				}
			}
			for( uint8_t j = 0; j < MAX_UV_CHANNELS; j++ ) {
				if( mesh.texCoordChannels[j].GetSize() != 0 ) {
					VkBuffer& buffer = drawBuffers.vertexBuffers[4 + MAX_COLOR_CHANNELS + j];
					CreateBuffer( mesh.texCoordChannels[j].GetSize() * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
				}
			}
		}
	}

	void Update() {
		auto& cameras = CameraComponent::GetAllCameras();
		for( CameraComponent& camera : cameras ) {
			if( !camera.IsEnabled() ) continue;

			ComponentHandle<TransformComponent> cameraTransform = camera.GetEntity().GetComponent<TransformComponent>();
			if( !cameraTransform.IsValidHandle() ) continue;
			perCameraUniformBuffer.viewMatrix = cameraTransform->WorldToLocalMatrix();
			perCameraUniformBuffer.projectionMatrix = camera.ProjectionMatrix();
			perCameraUniformBuffer.pvMatrix = perCameraUniformBuffer.projectionMatrix * perCameraUniformBuffer.viewMatrix;
			perCameraUniformBuffer.cameraPosition = cameraTransform->GetPosition();

			auto& objectsToRender = g_RenderSystem_instance.m_componentTuples;
			for( auto& entityTuplePair : objectsToRender ) {
				auto&[transform, renderer] = entityTuplePair.value;

				perDrawUniformBuffer.modelMatrix = transform->LocalToWorldMatrix();
				perDrawUniformBuffer.vmMatrix = perCameraUniformBuffer.viewMatrix * perDrawUniformBuffer.modelMatrix;
				perDrawUniformBuffer.pvmMatrix = perCameraUniformBuffer.pvMatrix * perDrawUniformBuffer.modelMatrix;

				auto* buffersArray = meshBuffersMap.Find( renderer->model.guid );
				if( buffersArray == nullptr ) {
					buffersArray = &CreateMeshBuffers( renderer->model );
				}

				vkGetBufferMemoryRequirements(  )
			}
		}
	}
}
