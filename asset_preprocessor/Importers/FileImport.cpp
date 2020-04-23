// ReSharper disable CppClangTidyBugproneMacroParentheses
#include "../stdafx.h"
#include "FileImport.h"

namespace axlt::editor {
	Array<Guid> ImportFile( File& file, File& importFile, const Guid& guid ) {
		Array<Guid> dependencies;
	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )									\
		if( file.Extension() == String(Ext) ) {														\
			Type* dataPtr = ImportFunction( file, dependencies );									\
			auto handle = ResourceHandle<Type>( guid, Version, GetHash( String(#Type) ), dataPtr );	\
			handle.Serialize( importFile );															\
			return dependencies;																	\
		}

	#include "ImportExtensionTypes.inl"

		return dependencies;
	}
}
