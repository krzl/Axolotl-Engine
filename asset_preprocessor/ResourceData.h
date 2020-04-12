#pragma once

#include <cstdint>

namespace axlt::editor {
	class ResourceData {

	public:

		String hash;
		uint64_t guid;
		uint32_t version;
		uint32_t type;
		uint64_t fileSize;
		void* data;
	};

	inline uint64_t GetHash( const ResourceData& resourceData ) {
		return resourceData.guid;
	}
}