#pragma once
#include "FileSystem/Guid.h"
#include "FileSystem/File.h"

namespace axlt {
	class ResourceData {

	public:

		ResourceData( const Guid& guid, uint32_t version, uint32_t type, void* data );

		Guid guid;
		uint32_t version;
		uint32_t type;
		void* data;

		void Serialize( const File& file );
	};

	inline uint64_t GetHash( const ResourceData& resourceData ) {
		return GetHash( resourceData.guid );
	}
}
