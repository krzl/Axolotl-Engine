// ReSharper disable CppDeclaratorNeverUsed
#include "stdafx.h"
#include "MathTypesPropertyViewers.h"
#include "PropertyViewerRegistrator.h"

namespace axlt::editor {
	static PropertyViewerRegistrator vector2PropertyViewer( VectorPropertyViewer<Vector2, 2> );
	static PropertyViewerRegistrator vector3PropertyViewer( VectorPropertyViewer<Vector3, 3> );
	static PropertyViewerRegistrator vector4PropertyViewer( VectorPropertyViewer<Vector4, 4> );
	static PropertyViewerRegistrator vector2IntPropertyViewer( VectorPropertyViewer<Vector2Int, 2> );
	static PropertyViewerRegistrator quaternionPropertyViewer( VectorPropertyViewer<Quaternion, 4> );
	static PropertyViewerRegistrator matrix4PropertyViewer( Matrix4PropertyViewer );

	void Matrix4PropertyViewer( const String& name, Matrix4& value ) {
		static String labelPrefix[ 4 ] = { "##0", "##1", "##2", "##3" };
		for( uint32_t i = 0; i < 4; i++ ) {
			if( i == 0 ) {
				DrawPropertyLabel( name );
			} else {
				DrawPropertyLabel( "" );
			}
			ImGui::InputScalarN( (labelPrefix[i] + name).GetData(), GetImguiScalarType<float>(), &value.vectors[i].x, 4 );
		}
	}
}
