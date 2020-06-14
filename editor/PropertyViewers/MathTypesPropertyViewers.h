#pragma once

#include "BasicTypesPropertyViewers.h"

namespace axlt::editor {
	template<typename T, int Size>
	bool VectorPropertyViewer( const String& name, T& value ) {
		void* ptr = &value;
		DrawPropertyLabel( name );
		return ImGui::InputScalarN( (String( "##" ) + name).GetData(), GetImguiScalarType<float>(), &value.x, Size );
	}

	bool Matrix4PropertyViewer( const String& name, Matrix4& value );
}