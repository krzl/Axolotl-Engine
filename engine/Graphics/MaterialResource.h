#pragma once
#include "Resources/ResourceHandle.h"
#include "TechniqueResource.h"

namespace axlt {
	class TechniqueResource;
	class MaterialResource;

	namespace editor {
		MaterialResource* ImportMaterial( File& file, Array<Guid>& dependencies );
	}

	class MaterialResource {

		friend MaterialResource* editor::ImportMaterial( File& file, Array<Guid>& dependencies );

		friend Serializer& operator<<( Serializer& s, MaterialResource& material );
		friend Serializer& operator>>( Serializer& s, MaterialResource& material );

	public:

		ResourceHandle<TechniqueResource> technique;

	private:

		Array<uint32_t> dirtyVariables;

		Map<uint32_t, float> floatParameters;
		Map<uint32_t, int32_t> intParameters;
		Map<uint32_t, Vector4> vectorParameters;

		DEFINE_TYPE_HASH( MaterialResource );
	};
}
