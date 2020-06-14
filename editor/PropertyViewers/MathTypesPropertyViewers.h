#pragma once

#include "BasicTypesPropertyViewers.h"

namespace axlt::editor {
	template<typename T, int Size>
	void VectorPropertyViewer( const String& name, T& value ) {
		void* ptr = &value;
		DrawPropertyLabel( name );
		ImGui::InputScalarN( (String( "##" ) + name).GetData(), GetImguiScalarType<float>(), &value.x, Size );
	}

	void Matrix4PropertyViewer( const String& name, Matrix4& value );
}