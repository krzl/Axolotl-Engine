#include "stdafx.h"
#include "EditorGuiSystem.h"

#include "Panels/EditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_win32.h>

#include <Game.h>
#include <Graphics/Vulkan/Vulkan.h>

#include <Entities/Entity.h>
#include <Graphics/RendererComponent.h>
#include <Entities/TransformComponent.h>
#include "Panels/ProjectFilesPanel.h"
#include "Panels/EntitiesPanel.h"
#include "Panels/PropertiesPanel.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace axlt {
	class RendererComponent;

	class TransformComponent;
}

namespace axlt::editor {

	ResourceHandle<ModelResource> uiModel;
	ResourceHandle<MaterialResource> uiMaterial;
	Mesh* mesh;

	Entity* entity;

	Array<EditorPanel*> panels;

	void EditorGuiSystem::Setup() {
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.DisplaySize = ImVec2( (float) GameInstance.GetWindow().GetWidth(), (float) GameInstance.GetWindow().GetHeight() );
		io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );

		uint8_t* fontData;
		int32_t texWidth, texHeight, bytesPerPixel;

		io.Fonts->GetTexDataAsRGBA32( &fontData, &texWidth, &texHeight, &bytesPerPixel );
		
		ResourceHandle<TextureResource> fontTexture = new TextureResource();
		fontTexture->width = texWidth;
		fontTexture->height = texHeight;
		fontTexture->channelCount = 4;
		fontTexture->textureData.AddRange( fontData, texWidth * texHeight * bytesPerPixel );
		fontTexture->format = TextureFormat::RGBA32;
		
		uiModel = new ModelResource();
		uiModel->meshes.AddEmpty( 1 );
		mesh = &uiModel->meshes[ 0 ];

		entity = new Entity( "Editor GUI" );
		entity->AddComponent<TransformComponent>();
		
		ComponentHandle<RendererComponent> renderer = entity->AddComponent<RendererComponent>();

		renderer->model = uiModel;
		renderer->material = ResourceHandle<MaterialResource>::Load( Guid::FromString( "ffffffffffffffffffffffffffffff05" ) );
		uiMaterial = renderer->material;
		
		uiMaterial->SetUniform( "translate", Vector4( -1.0f, -1.0f, 0.0f, 0.0f ) );
		uiMaterial->SetTexture( "fontSampler", fontTexture );

		const_cast<Window&>( GameInstance.GetWindow() ).SetCustomCallbackHandler( ImGui_ImplWin32_WndProcHandler );

		ImGui_ImplWin32_Init( GameInstance.GetWindow().GetHandle() );

		EditorPanel::CreatePanel<ProjectFilesPanel>();
		EditorPanel::CreatePanel<EntitiesPanel>();
		EditorPanel::CreatePanel<PropertiesPanel>();
	}
	
	void EditorGuiSystem::Update() {
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2( (float)GameInstance.GetWindow().GetWidth(), (float)GameInstance.GetWindow().GetHeight() );

		const Vector2Int mousePos = input::GetMousePos();
		io.MousePos = ImVec2( (float) mousePos.x, (float) mousePos.y );
		io.MouseDown[ 0 ] = input::GetKey( Key::LEFT_MOUSE_BUTTON );
		io.MouseDown[ 1 ] = input::GetKey( Key::RIGHT_MOUSE_BUTTON );
		
		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos( viewport->GetWorkPos() );
		ImGui::SetNextWindowSize( viewport->GetWorkSize() );
		ImGui::SetNextWindowViewport( viewport->ID );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
		bool isDockSpaceOpen = true;
		
		ImGui::Begin( "DockSpace", &isDockSpaceOpen, window_flags );
		ImGui::PopStyleVar( 3 );

		ImGui::DockSpace( ImGui::GetID( "DockSpace" ), ImVec2( 0.0f, 0.0f ), 0 );
		
		ImGui::End();
		
		ImGui::SetNextWindowPos( ImVec2( (float) 650, (float) 20 ), ImGuiCond_FirstUseEver );
		ImGui::ShowDemoWindow();
		for( auto panel : panels ) { panel->Update(); }
		for ( uint32_t i = 0; i < panels.GetSize(); i++ ) {
			EditorPanel* panel = panels[ i ];
			if( !panel->isPanelOpened) {
				panel->Shutdown();
				i--;
			}
		}

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu( "Windows" )) {
				if (ImGui::MenuItem( "Entities", NULL )) {
					EditorPanel::CreatePanel<EntitiesPanel>();
				}
				if (ImGui::MenuItem( "Project Files", NULL )) {
					EditorPanel::CreatePanel<ProjectFilesPanel>();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		
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

		uint32_t vertOffset = 0;

		for (uint32_t i = 0; i < (uint32_t) imDrawData->CmdListsCount; i++) {
			const ImDrawList* drawList = imDrawData->CmdLists[ i ];
			for (uint32_t j = 0; j < (uint32_t)drawList->VtxBuffer.Size; j++) {
				mesh->vertices[ currentVert ] = Vector3( drawList->VtxBuffer[ j ].pos.x, drawList->VtxBuffer[ j ].pos.y, 0.0f );
				mesh->colorChannels[ 0 ][ currentVert ] = Color( drawList->VtxBuffer[ j ].col );
				mesh->texCoordChannels[ 0 ][ currentVert * 2 ] = drawList->VtxBuffer[ j ].uv.x;
				mesh->texCoordChannels[ 0 ][ currentVert * 2 + 1 ] = drawList->VtxBuffer[ j ].uv.y;
				currentVert++;
			}

			for( uint32_t j = 0; j < (uint32_t)drawList->IdxBuffer.Size; j++ ) {
				mesh->indices[ currentIndex ] = vertOffset + drawList->IdxBuffer.Data[j];
				currentIndex++;
			}

			vertOffset += drawList->VtxBuffer.Size;
		}
		
		uiModel->Flush();

		uiMaterial->SetUniform( "scale", Vector4( 2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y, 0.0f, 0.0f ) );
	}

	void EditorGuiSystem::RegisterPanel( EditorPanel* editorPanel ) {
		panels.Add( editorPanel );
	}

	void EditorGuiSystem::UnregisterPanel( EditorPanel* editorPanel ) {
		for (uint32_t i = 0; i < panels.GetSize(); i++) {
			if (panels[ i ] == editorPanel) {
				panels.Remove( i );
				return;
			}
		}

		AXLT_ASSERT( false, "Should not be here" );
	}

	DEFINE_SYSTEM( EditorGuiSystem, 10000 )
}
