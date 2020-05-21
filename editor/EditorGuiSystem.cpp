#include "stdafx.h"
#include "EditorGuiSystem.h"

#include "Importers/FileImport.h"

#include <imgui.h>

#include <Game.h>
#include <Graphics/Vulkan/Vulkan.h>

#include <Entities/Entity.h>
#include <Graphics/RendererComponent.h>
#include <Entities/TransformComponent.h>

namespace axlt {
	class RendererComponent;

	class TransformComponent;
}

namespace axlt::editor {

	ResourceHandle<ModelResource> uiModel;
	ResourceHandle<MaterialResource> uiMaterial;
	MeshResource* mesh;

	Entity* entity;

	void EditorGuiSystem::OnInitialize() {
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();

		io.DisplaySize = ImVec2( GameInstance.GetWindow().GetWidth(), GameInstance.GetWindow().GetHeight() );
		io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );

		uint8_t* fontData;
		int32_t texWidth, texHeight, bytesPerPixel;

		io.Fonts->GetTexDataAsRGBA32( &fontData, &texWidth, &texHeight, &bytesPerPixel );
		
		ResourceHandle<TextureResource> fontTexture = ResourceHandle<TextureResource>::CreateEmptyResource( Guid( 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF00 ) );
		fontTexture->width = texWidth;
		fontTexture->height = texHeight;
		fontTexture->channelCount = 4;
		fontTexture->textureData.AddRange( fontData, texWidth * texHeight * bytesPerPixel );
		fontTexture->format = TextureFormat::RGBA32;
		
		uiModel = ResourceHandle<ModelResource>::CreateEmptyResource( Guid( 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF01 ) );
		uiModel->meshes.AddEmpty( 1 );
		mesh = &uiModel->meshes[ 0 ];

		entity = new Entity();
		entity->AddComponent<TransformComponent>();
		
		ComponentHandle<RendererComponent> renderer = entity->AddComponent<RendererComponent>();

		renderer->model = uiModel;
		renderer->material = ResourceHandle<MaterialResource>::Load( Guid::FromString( "ffffffffffffffffffffffffffffff05" ) );
		uiMaterial = renderer->material;
		
		uiMaterial->SetUniform( "translate", Vector4( -1.0f, -1.0f, 0.0f, 0.0f ) );
		uiMaterial->SetTexture( "fontSampler", fontTexture );
	}
	
	void EditorGuiSystem::Update() {
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2( (float)GameInstance.GetWindow().GetWidth(), (float)GameInstance.GetWindow().GetHeight() );

		const Vector2Int mousePos = input::GetMousePos();
		io.MousePos = ImVec2( mousePos.x, mousePos.y );
		io.MouseDown[ 0 ] = input::GetKey( Key::LEFT_MOUSE_BUTTON );
		io.MouseDown[ 1 ] = input::GetKey( Key::RIGHT_MOUSE_BUTTON );
		
		ImGui::NewFrame();
		ImGui::SetNextWindowPos( ImVec2( 650, 20 ), ImGuiCond_FirstUseEver );
		ImGui::ShowDemoWindow();
		ImGui::Render();

		ImDrawData* imDrawData = ImGui::GetDrawData();
		
		if( (uint32_t) imDrawData->TotalVtxCount != mesh->vertices.GetSize() ) {
			mesh->vertices.Clear();
			mesh->colorChannels[ 0 ].Clear();
			mesh->texCoordChannels[ 0 ].Clear();
			
			mesh->vertices.AddEmpty( (uint32_t)imDrawData->TotalVtxCount );
			mesh->colorChannels[ 0 ].AddEmpty( (uint32_t)imDrawData->TotalVtxCount );
			mesh->texCoordChannels[0].AddEmpty( (uint32_t)imDrawData->TotalVtxCount * 2 );
		}

		if ((uint32_t)imDrawData->TotalIdxCount != mesh->indices.GetSize()) {
			mesh->indices.Clear();
			mesh->indices.AddEmpty( (uint32_t)imDrawData->TotalIdxCount );
		}

		uint32_t currentVert = 0;
		uint32_t currentIndex = 0;

		for (uint32_t i = 0; i < (uint32_t) imDrawData->CmdListsCount; i++) {
			const ImDrawList* drawList = imDrawData->CmdLists[ i ];
			for (uint32_t j = 0; j < (uint32_t)drawList->VtxBuffer.Size; j++) {
				mesh->vertices[ currentVert ] = Vector3( drawList->VtxBuffer[ j ].pos.x, drawList->VtxBuffer[ j ].pos.y, 0.0f );
				mesh->colorChannels[ 0 ][ currentVert ] = Color( drawList->VtxBuffer[ j ].col );
				mesh->texCoordChannels[ 0 ][ currentVert * 2 ] = drawList->VtxBuffer[ j ].uv.x;
				mesh->texCoordChannels[ 0 ][ currentVert * 2 + 1 ] = drawList->VtxBuffer[ j ].uv.y;
				currentVert++;
			}
			
			memcpy( &mesh->indices[ currentIndex], drawList->IdxBuffer.Data, drawList->IdxBuffer.Size * sizeof( ImDrawIdx ) );
			currentIndex += drawList->IdxBuffer.Size;
		}
		
		uiModel->Flush();

		uiMaterial->SetUniform( "scale", Vector4( 2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y, 0.0f, 0.0f ) );
	}

	DEFINE_SYSTEM( EditorGuiSystem, 10000 )
}