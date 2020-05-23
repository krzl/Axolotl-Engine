#include "stdafx.h"
#include "File.h"
#include "FileSystem.h"
#include <md5.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace axlt {

	File::File( String path, FileSystem& fileSystem, const uint32_t parentDirectoryIndex ) :
		fileName( Move( path ) ),
		directoryIndex( parentDirectoryIndex ),
		fileSystem( fileSystem ) {}

	File::File( File&& other ) noexcept :
		fileName( Move( other.fileName ) ),
		directoryIndex( other.directoryIndex ),
		fileSystem( other.fileSystem ) {}

	File& File::operator=( const File& other ) {
		if( this != &other ) {
			fileName = other.fileName;
			directoryIndex = other.directoryIndex;
			fileSystem = other.fileSystem;
		}
		return *this;
	}

	File& File::operator=( File&& other ) noexcept {
		if( this != &other ) {
			fileName = Move( other.fileName );
			directoryIndex = other.directoryIndex;
			fileSystem = other.fileSystem;
		}
		return *this;
	}

	String File::AbsolutePath() const {
		return ParentDirectory().AbsolutePath() + '/' + fileName;
	}

	String File::Extension() const {
		const uint32_t index = fileName.LastIndexOf( '.' );
		if( index == String::indexNotFound ) {
			return String();
		}
		return fileName.Substring( index + 1, fileName.Length() - index - 1 );
	}

	Directory& File::ParentDirectory() const {
		return fileSystem.directories[directoryIndex];
	}

	uint32_t File::Index() const {
		return (uint32_t) ( ( SparseArray<File>::ArrayElement* ) this - fileSystem.files.GetData() );
	}

	void File::DeleteFromDisk() const {
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

	void File::ChangeParentDirectory( Directory& directory ) {
		Directory& oldParent = ParentDirectory();
		for (uint32_t i = 0; i < oldParent.childFileIndices.GetSize(); i++) {
			if( oldParent.childFileIndices[ i ] == Index()) {
				oldParent.childFileIndices.Remove( i );
				break;
			}
		}
		directoryIndex = directory.Index();
		directory.childFileIndices.Add( Index() );
	}
}
