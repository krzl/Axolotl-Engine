#include "stdafx.h"
#include "EditorGuiSystem.h"
#include <examples/imgui_impl_win32.h>
#include <Game.h>
#include <examples/imgui_impl_vulkan.h>

#include <Graphics/Vulkan/Vulkan.h>

namespace axlt::editor {

	ResourceHandle<ModelResource> uiModel;
	MeshResource* mesh;

	void EditorGuiSystem::OnInitialize() {
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();

		io.DisplaySize = ImVec2( GameInstance.GetWindow().GetWidth(), GameInstance.GetWindow().GetHeight() );
		io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );

		uint8_t* fontData;
		int32_t texWidth, texHeight, bytesPerPixel;

		io.Fonts->GetTexDataAsRGBA32( &fontData, &texWidth, &texHeight, &bytesPerPixel );
		
		ResourceHandle<TextureResource> fontTexture = ResourceHandle<TextureResource>::CreateEmptyResource( Guid( 0xFFFFFFFFFFFFFFFF, 0x01 ) );


		fontTexture->width = texWidth;
		fontTexture->height = texHeight;
		fontTexture->channelCount = 4;
		fontTexture->textureData.AddRange( fontData, texWidth * texHeight * bytesPerPixel );
		fontTexture->format = TextureFormat::RGBA32;
		
		uiModel = ResourceHandle<ModelResource>::CreateEmptyResource( Guid( 0xFFFFFFFFFFFFFFFF, 0x02 ) );

		uiModel->meshes.AddEmpty( 1 );
		mesh = &uiModel->meshes[ 0 ];
	}
	
	void EditorGuiSystem::Update() {
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Render();

		ImDrawData* imDrawData = ImGui::GetDrawData();
		
		if( (uint32_t) imDrawData->TotalVtxCount > mesh->vertices.GetSize() ) {
			uint32_t targetSize = max( imDrawData->TotalVtxCount, 1024 * 8 );
			targetSize = max( targetSize, mesh->vertices.GetSize() * 2 );

			const uint32_t elementsToAdd = targetSize - mesh->vertices.GetSize();
			
			mesh->vertices.AddEmpty( elementsToAdd );
			mesh->colorChannels[ 0 ].AddEmpty( elementsToAdd );
			mesh->texCoordChannels[0].AddEmpty( elementsToAdd * 2 );
		}

		if ((uint32_t)imDrawData->TotalIdxCount > mesh->indices.GetSize()) {
			uint32_t targetSize = max( imDrawData->TotalIdxCount, 1024 * 8 );
			targetSize = max( targetSize, mesh->indices.GetSize() * 2 );
			mesh->indices.AddEmpty( targetSize - mesh->indices.GetSize() );
		}

		uint32_t currentVert = 0;
		uint32_t currentIndex = 0;

		for (uint32_t i = 0; i < imDrawData->CmdListsCount; i++) {
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
		
		mesh.Flush();
	}

	DEFINE_SYSTEM( EditorGuiSystem, 10000 )
}
