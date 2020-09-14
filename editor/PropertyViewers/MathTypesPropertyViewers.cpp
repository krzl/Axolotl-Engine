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
	static PropertyViewerRegistrator rect2DPropertyViewer( Rect2DPropertyViewer );

	bool Matrix4PropertyViewer( const String& name, Matrix4& value ) {
		static String labelPrefix[ 4 ] = { "##0", "##1", "##2", "##3" };
		bool hasChanged = false;
		for( uint32_t i = 0; i < 4; i++ ) {
			if( i == 0 ) {
				DrawPropertyLabel( name );
			} else {
				DrawPropertyLabel( "" );
			}
			hasChanged |= ImGui::InputScalarN( (labelPrefix[i] + name).GetData(), GetImguiScalarType<float>(), &value.vectors[i].x, 4 );
		}
		return hasChanged;
	}

	bool Rect2DPropertyViewer( const String& name, Rect2D& value ) {
		DrawPropertyLabel( name );
		const float totalWidth = ImGui::CalcItemWidth();

		bool hasChanged = false;
		float xMin = value.GetMinX();
		float xMax = value.GetMaxX();
		float yMin = value.GetMinY();
		float yMax = value.GetMaxY();

		ImGui::SetNextItemWidth( totalWidth / 2.0f );
		if( ImGui::InputFloat( "xMin", &xMin ) ) {
			value.SetMinX( xMin );
			hasChanged = true;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth( -1.0f );
		if (ImGui::InputFloat( "xMax", &xMax )) {
			value.SetMaxX( xMax );
			hasChanged = true;
		}

		DrawPropertyLabel( "" );
		ImGui::SetNextItemWidth( totalWidth / 2.0f );
		if( ImGui::InputFloat( "yMin", &yMin ) ) {
			value.SetMinY( yMin );
			hasChanged = true;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth( -1.0f );
		if (ImGui::InputFloat( "yMax", &yMax )) {
			value.SetMaxY( yMax );
			hasChanged = true;
		}

		return hasChanged;
	}
}
