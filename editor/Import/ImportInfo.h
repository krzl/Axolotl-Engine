// ReSharper disable CppClangTidyModernizePassByValue
#pragma once

#include <Resources/Serialization/Serializable.h>
#include <FileSystem/Guid.h>

namespace axlt::editor {
	class ImportInfo final : public Serializable {

	public:

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		ImportInfo() = default;
		
		ImportInfo( const String& resourceHash, const String& importHash, const uint32_t importVersion, const Array<Guid>& dependencies ) :
			resourceHash( resourceHash ),
			importHash( importHash ),
			importVersion( importVersion ) {
			for( const Guid& guid : dependencies ) {
				this->dependencies.Add( guid.ToString() );
			}
		}

		String resourceHash;
		String importHash;
		uint32_t importVersion;
		Array<String> dependencies;
	};
}
