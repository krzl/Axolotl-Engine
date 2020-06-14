#include "stdafx.h"
#include "TransformComponentPropertyViewer.h"
#include "PropertyViewerRegistrator.h"
#include "MathTypesPropertyViewers.h"

namespace axlt::editor {
	static PropertyViewerRegistrator propertyViewer( TransformComponentPropertyViewer );
	
	bool TransformComponentPropertyViewer( const String& name, TransformComponent& value ) {
		bool hasChanged = false;
		Vector3 localPosition = value.GetLocalPosition();
		if( VectorPropertyViewer<Vector3, 3>( "Local Position", localPosition ) ) {
			hasChanged = true;
			value.SetLocalPosition( localPosition );
		}
		Quaternion localRotation = value.GetLocalRotation();
		if( VectorPropertyViewer<Quaternion, 4>( "Local Rotation", localRotation ) ) {
			hasChanged = true;
			value.SetLocalRotation( localRotation );
		}
		Vector3 localScale = value.GetLocalScale();
		if( VectorPropertyViewer<Vector3, 3>( "Local Scale", localScale ) ) {
			hasChanged = true;
			value.SetLocalScale( localScale );
		}
		return hasChanged;
	}
}
