// ReSharper disable CppClangTidyBugproneMacroParentheses
#include "../stdafx.h"
#include "FileImport.h"

namespace axlt::editor {
	uint32_t GetCurrentImportVersion( const String& fileExtension ) {
	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )		\
			if( fileExtension == String(Ext) ) {						\
				return Version;											\
			}

		#include "ImportExtensionTypes.inl"

		return 0xFFFFFFFF;
	}
	
	uint32_t GetImportTypeHash( const String& fileExtension ) {
	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )		\
			if( fileExtension == String(Ext) ) {						\
				return GetTypeHash<Type>();								\
			}

		#include "ImportExtensionTypes.inl"

		return 0xFFFFFFFF;
	}

	Array<Guid> ImportFile( File& file, File& importFile, const Guid& guid ) {
		Array<Guid> dependencies;
	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )		\
		if( file.Extension() == String(Ext) ) {							\
			Type* dataPtr = ImportFunction( file, dependencies );		\
			uint32_t typeHash = GetTypeHash<Type>();					\
			auto handle = ResourceHandle<Type>( dataPtr, guid );		\
			handle.Serialize( importFile, typeHash );					\
			return dependencies;										\
		}

	#include "ImportExtensionTypes.inl"

		return dependencies;
	}
}
