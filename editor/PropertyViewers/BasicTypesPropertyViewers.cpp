// ReSharper disable CppDeclaratorNeverUsed
#include "stdafx.h"
#include "BasicTypesPropertyViewers.h"
#include "PropertyViewerRegistrator.h"

namespace axlt::editor {
	static PropertyViewerRegistrator uint8PropertyViewer( NumericPropertyViewer<uint8_t> );
	static PropertyViewerRegistrator int8PropertyViewer( NumericPropertyViewer<int8_t> );
	static PropertyViewerRegistrator uint16PropertyViewer( NumericPropertyViewer<uint16_t> );
	static PropertyViewerRegistrator int16PropertyViewer( NumericPropertyViewer<int16_t> );
	static PropertyViewerRegistrator uint32PropertyViewer( NumericPropertyViewer<uint32_t> );
	static PropertyViewerRegistrator int32PropertyViewer( NumericPropertyViewer<int32_t> );
	static PropertyViewerRegistrator uint64PropertyViewer( NumericPropertyViewer<uint64_t> );
	static PropertyViewerRegistrator int64PropertyViewer( NumericPropertyViewer<int64_t> );
	static PropertyViewerRegistrator boolPropertyViewer( BoolPropertyViewer );
	static PropertyViewerRegistrator floatPropertyViewer( NumericPropertyViewer<float> );
	static PropertyViewerRegistrator doublePropertyViewer( NumericPropertyViewer<double> );
	static PropertyViewerRegistrator stringPropertyViewer( StringPropertyViewer );

	bool BoolPropertyViewer( const String& name, bool& value ) {
		DrawPropertyLabel( name );
		return ImGui::Checkbox( (String( "##" ) + name).GetData(), &value );
	}

	bool StringPropertyViewer( const String& name, String& value ) {
		static char inputStorage[ 512 ];
		DrawPropertyLabel( name );
		if( ImGui::InputText( (String( "##" ) + name).GetData(), inputStorage, IM_ARRAYSIZE( inputStorage ),
			ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue ) || !ImGui::IsWindowFocused() ) {
			value = inputStorage;
			return true;
		}
		return false;
	}

	void DrawPropertyLabel( const String& name ) {
		const float labelWidth = defaultPropertyLabelWidth - ImGui::GetCursorPosX();
		ImGui::SetNextItemWidth( labelWidth > 0 ? labelWidth : 0 );
		ImGui::LabelText( "##", name.GetData() );
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text( name.GetData() );
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth( -1 );
	}
}
