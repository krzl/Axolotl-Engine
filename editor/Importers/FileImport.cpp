// ReSharper disable CppClangTidyBugproneMacroParentheses
#include "../stdafx.h"
#include "FileImport.h"

namespace axlt::editor {
	uint32_t GetCurrentImportVersion( const String& fileExtension ) {
	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )		\
		if( fileExtension == String(Ext) ) {							\
			return Version;												\
		}

		#include "ImportExtensionTypes.inl"

		return 0xFFFFFFFF;
	}
	
	uint32_t GetImportTypeHash( const String& fileExtension ) {
	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )		\
		if( fileExtension == String(Ext) ) {							\
			return GetTypeHash<Type>();									\
		}

		#include "ImportExtensionTypes.inl"

		return 0xFFFFFFFF;
	}

	template<typename T>
	void ImportFileInner( T* (*importFunction)( File&, Array<Guid>& ), File& file, File& importFile, Array<Guid>& dependencies ) {
		T* dataPtr = importFunction( file, dependencies );
		ResourceHandle<T> handle = ResourceHandle<T>( dataPtr );
		Serializer serializer( importFile, "wb" );
		serializer << *handle.GetData() << Serializer::end;
	}

	Array<Guid> ImportFile( File& file, File& importFile, const Guid& guid ) {
		Array<Guid> dependencies;
	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )						\
		if( file.Extension() == String(Ext) ) {											\
			ImportFileInner<Type>( ImportFunction, file, importFile, dependencies );	\
			return dependencies;														\
		}

		#include "ImportExtensionTypes.inl"

		return dependencies;
	}
}
