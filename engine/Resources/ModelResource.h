#pragma once
#include "Serialization/Serializer.h"

namespace axlt {

#define MAX_COLOR_CHANNELS 1
#define MAX_UV_CHANNELS 4

	class MeshResource {

	public:

		Array<uint32_t, ExactHeapArrayAllocator> indices;
		Array<Vector3, ExactHeapArrayAllocator> vertices;
		Array<Vector3, ExactHeapArrayAllocator> normals;
		Array<Vector3, ExactHeapArrayAllocator> tangents;
		Array<Vector3, ExactHeapArrayAllocator> bitangents;
		Array<Color, ExactHeapArrayAllocator> colorChannels[MAX_COLOR_CHANNELS];
		Array<float, ExactHeapArrayAllocator> texCoordChannels[MAX_UV_CHANNELS];
	};

	Serializer& operator<<( Serializer& s, MeshResource& mesh );
	Serializer& operator>>( Serializer& s, MeshResource& mesh );

	namespace vk {
		void BindResources();
	}
	
	class ModelResource {

		friend void vk::BindResources();

	public:

		Array<MeshResource, ExactHeapArrayAllocator> meshes;

		void Flush();

	private:

		bool isDirty = true;
		
		DEFINE_TYPE_HASH( ModelResource );
	};

	Serializer& operator<<( Serializer& s, ModelResource& model );
	Serializer& operator>>( Serializer& s, ModelResource& model );
}
