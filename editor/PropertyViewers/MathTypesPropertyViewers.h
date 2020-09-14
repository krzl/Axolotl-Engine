#pragma once

#include "BasicTypesPropertyViewers.h"

namespace axlt::editor {
	template<typename T, int Size>
	bool VectorPropertyViewer( const String& name, T& value ) {
		DrawPropertyLabel( name );
		return ImGui::InputScalarN( ( String( "##" ) + name).GetData(), GetImguiScalarType<float>(), &value.x, Size );
	}

	bool Matrix4PropertyViewer( const String& name, Matrix4& value );
	bool Rect2DPropertyViewer( const String& name, Rect2D& value );
}
