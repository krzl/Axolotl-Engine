#include "stdafx.h"
#include "File.h"
#include "FileSystem.h"
#include <md5.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace axlt {

	File::File( String path, const uint64_t fileSize, FileSystem& fileSystem, const uint32_t parentDirectoryIndex ) :
		fileName( Move( path ) ),
		directoryIndex( parentDirectoryIndex ),
		fileSystem( fileSystem ),
		fileSize( fileSize ) {}

	String File::AbsolutePath() const {
		return ParentDirectory().AbsolutePath() + '\\' + fileName;
	}

	String File::Extension() const {
		const uint32_t index = fileName.LastIndexOf( '.' );
		if( index == String::indexNotFound ) {
			return String();
		}
		return fileName.Substring( index + 1, fileName.Length() - index - 1 );
	}

	uint64_t File::FileSize() const {
		return fileSize;
	}

	Directory& File::ParentDirectory() const {
		return fileSystem.directories[directoryIndex];
	}

	uint32_t File::Index() const {
		return (uint32_t) ( ( SparseArray<File>::ArrayElement* ) this - fileSystem.files.GetData() );
	}

	void File::DeleteFile() const {
		FileSystem::DeleteFileFromDisk( AbsolutePath().GetData() );
		fileSystem.files.Remove( Index() );
	}

	static char readBuffer[1024 * 64];

	String File::CalculateMd5() const {
		MD5 md5Obj;
		return md5Obj.digestFile( AbsolutePath().GetData() );
	}

	rapidjson::Document File::ToJson() const {
		FILE* fp;
		fopen_s( &fp, AbsolutePath().GetData(), "rb" );
		
		if( fp == nullptr ) {
			rapidjson::Document empty;
			empty.SetObject();

			return empty;
		}
		
		rapidjson::FileReadStream is( fp, readBuffer, sizeof( readBuffer ) );

		rapidjson::Document d;
		d.ParseStream( is );

		fclose( fp );

		if( d.HasParseError() ) {
			rapidjson::Document empty;
			empty.SetObject();

			return empty;
		}

		return d;
	}

	void File::FromJson( const rapidjson::Document& doc ) const {

		rapidjson::StringBuffer strbuf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer( strbuf );
		doc.Accept( writer );

		FILE* fp;
		fopen_s( &fp, AbsolutePath().GetData(), "w+" );
		fprintf( fp, "%s", strbuf.GetString() );
		fclose( fp );
	}
}
