#include "stdafx.h"
#include "ResourceDatabase.h"
#include <FileSystem/File.h>
#include <Common/Random.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

namespace axlt::editor {

	Set<uint32_t> usedFilesSet;

	ResourceDatabase::ResourceDatabase() :
		resourceFileSystem( "../ProjectFiles" ),
		importsFileSystem( "../ImportedFiles" ) {

		for( File& file : resourceFileSystem.files ) {
			if( file.Extension() == "import" ) {
				continue;
			}
			String md5 = file.CalculateMd5();
			File& importFile = GetImportFile( file );

			usedFilesSet.Add( importFile.Index() );

			String importMd5 = importFile.CalculateMd5();
			rapidjson::Document importJson = importFile.ToJson();

			String guid;
			if( !importJson.HasMember( "guid" ) ) {
				guid = GenerateGuid();

				rapidjson::Document importSettings = importFile.ToJson();
				rapidjson::Document::AllocatorType& importSettingsAllocator = importSettings.GetAllocator();

				rapidjson::Value val;
				val.SetString( guid.GetData(), static_cast<rapidjson::SizeType>( guid.Length() ), importSettingsAllocator );
				importSettings.AddMember( "guid", val, importSettingsAllocator );

				importFile.FromJson( importSettings );
			} else {
				guid = importJson["guid"].GetString();
			}

			File* importHashFile = importsFileSystem.RootDirectory().GetFileByName( guid + ".hash" );

			if( importHashFile != nullptr ) {
				rapidjson::Document importHashJson = importHashFile->ToJson();

				String resourceHash = importHashJson["resource"].GetString();
				String importHash = importHashJson["import"].GetString();

				if( resourceHash != md5 || importHash != importMd5 ) {
					ImportFile( file, md5, importMd5, guid );
				} else {
					File* importDataFile = importsFileSystem.FindFile( guid, 0 );

					if( importDataFile == nullptr ) {
						ImportFile( file, md5, importMd5, guid );
					} else {
						usedFilesSet.Add( importDataFile->Index() );
						usedFilesSet.Add( importHashFile->Index() );
					}
				}
			} else {
				ImportFile( file, md5, importMd5, guid );
			}
		}

		uint32_t initialImportsFilesCount = importsFileSystem.files.GetSize();
		for( uint32_t i = 0; i < initialImportsFilesCount; i++ ) {
			if( !usedFilesSet.Contains( i ) ) {
				importsFileSystem.files[i].DeleteFile();
			}
		}
	}

	File& ResourceDatabase::GetImportFile( const File& file ) {
		const String importFileName = file.fileName + ".import";
		for( uint32_t fileIndex : file.ParentDirectory().childFileIndices ) {
			File& candidate = resourceFileSystem.files[fileIndex];
			if( &candidate != &file && importFileName == candidate.fileName ) {
				return candidate;
			}
		}
		File& newImportFile = file.ParentDirectory().CreateFile( file.fileName + ".import" );

		String guid = GenerateGuid();

		rapidjson::Document importSettings = newImportFile.ToJson();
		rapidjson::Document::AllocatorType& importSettingsAllocator = importSettings.GetAllocator();

		rapidjson::Value val;
		val.SetString( guid.GetData(), static_cast<rapidjson::SizeType>( guid.Length() ), importSettingsAllocator );
		importSettings.AddMember( "guid", val, importSettingsAllocator );

		newImportFile.FromJson( importSettings );

		return newImportFile;
	}

	bool ResourceDatabase::IsGuidOccupied( const String& guid ) {
		for( File& file : importsFileSystem.files ) {
			if( file.fileName == guid ) {
				return true;
			}
		}
		return false;
	}

	String ResourceDatabase::GenerateGuid() {

		String guidString = "00000000000000000000000000000000";

		for( uint8_t i = 0; i < 2; i++ ) {
			snprintf( &( guidString[16 * i] ),
					  17,
					  "%016llx",
					  RandomNumber<uint64_t>()
			);
		}

		while( IsGuidOccupied( guidString ) ) {
			for( uint8_t i = 0; i < 2; i++ ) {
				snprintf( &( guidString[16 * i] ),
						  17,
						  "%016llx",
						  RandomNumber<uint64_t>()
				);
			}
		}
		return guidString;
	}

	void ResourceDatabase::ImportFile( File& file, const String& md5, const String& importMd5, const String& guid ) {

		File& importFile = importsFileSystem.FindOrCreateFile( guid, 0 );

		FILE* fp;
		fopen_s( &fp, importFile.AbsolutePath().GetData(), "w+" );
		fprintf( fp, "%s", md5.GetData() );
		fclose( fp );

		usedFilesSet.Add( importFile.Index() );

		File& importHash = importsFileSystem.FindOrCreateFile( guid + ".hash", 0 );

		rapidjson::Document importHashesJson;
		importHashesJson.SetObject();

		rapidjson::Document::AllocatorType& importHashesAllocator = importHashesJson.GetAllocator();

		rapidjson::Value val;
		val.SetString( md5.GetData(), static_cast<rapidjson::SizeType>( guid.Length() ), importHashesAllocator );
		importHashesJson.AddMember( "resource", val, importHashesAllocator );
		val.SetString( importMd5.GetData(), static_cast<rapidjson::SizeType>( guid.Length() ), importHashesAllocator );
		importHashesJson.AddMember( "import", val, importHashesAllocator );

		importHash.FromJson( importHashesJson );

		usedFilesSet.Add( importHash.Index() );
	}
}