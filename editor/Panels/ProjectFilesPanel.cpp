// ReSharper disable CppClangTidyClangDiagnosticFormatSecurity
#include "stdafx.h"
#include "ProjectFilesPanel.h"

#include "Globals.h"

#include <Resources/ResourceHandle.h>
#include <imgui.h>

namespace axlt::editor {

	static Directory* selectedDirectory = nullptr;
	static File* selectedFile = nullptr;

	void DrawFile( File& file ) {
		if (file.Extension() == "import") return;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		if(selectedFile == &file ) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		ImGui::TreeNodeEx( &file, flags, file.fileName.GetData() );
		if( ImGui::BeginDragDropSource() ) {
			uint32_t index = file.Index();
			ImGui::SetDragDropPayload( "File", &index, sizeof( uint32_t ) );
			ImGui::Text( file.fileName.GetData() );
			ImGui::EndDragDropSource();
		}
		if (ImGui::IsItemClicked()) {
			selectedFile = &file;
		}
	}

	void DrawFileList() {
		if( selectedDirectory != nullptr ) {
			for (uint32_t i = 0; i < selectedDirectory->childFileIndices.GetSize(); i++) {
				File& childFile = selectedDirectory->GetChildFile( i );
				DrawFile( childFile );
			}
		}
	}

	static ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf;
	static ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	void DrawDirectoryList( Directory& directory, const bool isFirst ) {
		ImGuiTreeNodeFlags flags = directory.childDirectoryIndices.GetSize() == 0 ? leafFlags : nodeFlags;
		if(selectedDirectory == &directory ) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		const bool isVisible = ImGui::TreeNodeEx( &directory, flags, isFirst ? "Root" : directory.path.GetData() );
		if (!isFirst && ImGui::BeginDragDropSource()) {
			uint32_t index = directory.Index();
			ImGui::SetDragDropPayload( "Directory", &index, sizeof( uint32_t ) );
			ImGui::Text( directory.path.GetData() );
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload;
			if( ( payload = ImGui::AcceptDragDropPayload( "Directory" ) ) ) {
				//TODO: Handle changing file directory on disk
				const uint32_t index = *(const uint32_t*) payload->Data;
				Directory& movedDirectory = resourceDatabase.resourceFileSystem.directories[ index ];
				movedDirectory.ChangeParentDirectory( directory );
			} else if ( ( payload = ImGui::AcceptDragDropPayload( "File" ) ) ) {
				//TODO: Handle changing file directory on disk
				const uint32_t index = *(const uint32_t*)payload->Data;
				File& movedFile = resourceDatabase.resourceFileSystem.files[ index ];
				movedFile.ChangeParentDirectory( directory );
			}
		}
		if( ImGui::IsItemClicked() ) {
			selectedDirectory = &directory;
		}
		if( isVisible ) {
			for (uint32_t i = 0; i < directory.childDirectoryIndices.GetSize(); i++ ) {
				Directory& childDirectory = directory.GetChildDirectory( i );
				DrawDirectoryList( childDirectory, false );
			}
			ImGui::TreePop();
		}
	}
	
	void ProjectFilesPanel::Update() {
		if (!ImGui::Begin( "Project Files", &isPanelOpened, ImGuiWindowFlags_NoScrollbar )) {
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 2, 2 ) );
		ImGui::Columns( 2 );

		ImGui::BeginChild( "Directories", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar );
		DrawDirectoryList( resourceDatabase.resourceFileSystem.RootDirectory(), true );
		ImGui::EndChild();
		
		ImGui::NextColumn();

		ImGui::BeginChild( "Files", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar );
		DrawFileList();
		ImGui::EndChild();
		
		ImGui::PopStyleVar();
		ImGui::End();
	}
}
