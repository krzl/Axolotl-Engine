#pragma once
#include "FileSystem/File.h"

namespace axlt {

#define MAX_COLOR_CHANNELS 1
#define MAX_UV_CHANNELS 4
	
	class ModelResource {

	public:

		Array<uint32_t, ExactHeapArrayAllocator> indices;
		Array<Vector3, ExactHeapArrayAllocator> vertices;
		Array<Vector3, ExactHeapArrayAllocator> normals;
		Array<Vector3, ExactHeapArrayAllocator> tangents;
		Array<Vector3, ExactHeapArrayAllocator> bitangents;
		Array<Color, ExactHeapArrayAllocator> colorChannels[MAX_COLOR_CHANNELS];
		Array<float, ExactHeapArrayAllocator> texCoordChannels[MAX_UV_CHANNELS];

		void Serialize( FILE* fp );
	};
}