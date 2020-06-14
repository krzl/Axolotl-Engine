#pragma once

#include "imgui.h"

namespace axlt::editor {

	const uint32_t defaultPropertyLabelWidth = 200;

	template<typename T>
	ImGuiDataType GetImguiScalarType() {
		return T::unimplemented_function;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<uint8_t>() {
		return ImGuiDataType_U8;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<uint16_t>() {
		return ImGuiDataType_U16;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<uint32_t>() {
		return ImGuiDataType_U32;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<uint64_t>() {
		return ImGuiDataType_U64;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<int8_t>() {
		return ImGuiDataType_S8;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<int16_t>() {
		return ImGuiDataType_S16;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<int32_t>() {
		return ImGuiDataType_S32;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<int64_t>() {
		return ImGuiDataType_S64;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<float>() {
		return ImGuiDataType_Float;
	}

	template<>
	inline ImGuiDataType GetImguiScalarType<double>() {
		return ImGuiDataType_Double;
	}
	
	void DrawPropertyLabel( const String& name );

	template<typename T>
	bool NumericPropertyViewer( const String& name, T& value ) {
		DrawPropertyLabel( name );
		return ImGui::InputScalar( ( String( "##" ) + name ).GetData(), GetImguiScalarType<T>(), &value, 0, 0 );
	}

	bool BoolPropertyViewer( const String& name, bool& value );
	bool StringPropertyViewer( const String& name, String& value );
}
