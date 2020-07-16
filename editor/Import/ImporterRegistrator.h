#pragma once
#include "BaseImporter.h"

namespace axlt::editor {
	
	class ImporterRegistrator {
		
	public:
		
		template<typename ResourceType, typename ImportType>
		explicit ImporterRegistrator( const ImportFunction<ResourceType, ImportType> importFunction, const uint32_t version, std::initializer_list<String> extensions ) {
			for( const String& extension : extensions ) {
				BaseImporter::RegisterImporter( importFunction, extension, version );
			}
		}
	};
}
