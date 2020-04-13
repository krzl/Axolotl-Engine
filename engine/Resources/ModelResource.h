#pragma once
#include "FileSystem/File.h"

namespace axlt {
	class ModelResource {

	public:

		Array<uint8_t, ExactHeapArrayAllocator> indexData;
		Array<uint8_t, ExactHeapArrayAllocator> vertexData;
	};
}