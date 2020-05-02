#include "stdafx.h"
#include "MaterialResource.h"

namespace axlt {

	Serializer& operator<<( Serializer& s, MaterialResource& material ) {
		return s << material.technique << material.floatParameters << material.intParameters << material.vectorParameters;
	}

	Serializer& operator>>( Serializer& s, MaterialResource& material ) {
		s >> material.technique >> material.floatParameters >> material.intParameters >> material.vectorParameters;

		material.dirtyVariables.Clear();
		material.dirtyVariables.AddEmpty(
			material.floatParameters.GetSize() +
			material.intParameters.GetSize() +
			material.vectorParameters.GetSize()
		);
		
		uint32_t i = 0;
		for( auto& pair : material.floatParameters ) {
			material.dirtyVariables[i++] = pair.key;
		}
		for( auto& pair : material.intParameters ) {
			material.dirtyVariables[i++] = pair.key;
		}
		for( auto& pair : material.vectorParameters ) {
			material.dirtyVariables[i++] = pair.key;
		}
		
		return s;
	}
}
