#include "stdafx.h"
#include "Serializer.h"
#include "FileSystem/File.h"

namespace axlt {

	Serializer::Serializer( const char* filePath, const char* accessFlags ) : fp( nullptr ) {
		fopen_s( &fp, filePath, accessFlags );
	}

	Serializer::Serializer( const File& file, const char* accessFlags ) : fp( nullptr ) {
		fopen_s( &fp, file.AbsolutePath().GetData(), accessFlags );
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void Serializer::operator<<( SerializerEnd end ) {
		fclose( fp );
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void Serializer::operator>>( SerializerEnd end ) {
		fclose( fp );
	}
}