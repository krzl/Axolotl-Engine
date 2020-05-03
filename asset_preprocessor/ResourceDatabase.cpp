#include "stdafx.h"
#include "ResourceDatabase.h"

#include <FileSystem/File.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include "Importers/FileImport.h"

namespace axlt::editor {

	ResourceDatabase* ResourceDatabase::instance; 
	
	Set<uint32_t> usedFilesSet;
	Set<Guid> changedFilesSet;

	ResourceDatabase::ResourceDatabase() :
		resourceFileSystem( "../ProjectFiles" ),
		importsFileSystem( "../ImportedFiles" ) {
		
		instance = this;

		//GENERATING GUIDS AND GUID TO FILEPATH MAP
		for( uint32_t i = 0; i < resourceFileSystem.files.GetSize(); i++ ) {
			File& file = resourceFileSystem.files[i];
			if( file.Extension() == "import" ) {
				continue;
			}
			String md5 = file.CalculateMd5();
			File& importFile = GetImportFile( file );
			file = resourceFileSystem.files[i];

			usedFilesSet.Add( importFile.Index() );

			String importMd5 = importFile.CalculateMd5();
			rapidjson::Document importJson = importFile.ToJson();

			if( !importJson.HasMember( "guid" ) ) {
				Guid guid = GenerateGuid();
				String guidString = guid.ToString();

				rapidjson::Document importSettings = importFile.ToJson();
				rapidjson::Document::AllocatorType& importSettingsAllocator = importSettings.GetAllocator();

				rapidjson::Value val;
				val.SetString( guidString.GetData(), static_cast<rapidjson::SizeType>( guidString.Length() ), importSettingsAllocator );
				importSettings.AddMember( "guid", val, importSettingsAllocator );

				importFile.FromJson( importSettings );
			} else {
				guidToFilepath.Add( Guid::FromString( importJson["guid"].GetString() ), file.AbsolutePath() );
			}
		}

		//IMPORTING FILES
		for( uint32_t i = 0; i < resourceFileSystem.files.GetSize(); i++ ) {
			File& file = resourceFileSystem.files[i];
			if( file.Extension() == "import" ) {
				continue;
			}
			String md5 = file.CalculateMd5();
			File& importFile = GetImportFile( file );
			file = resourceFileSystem.files[i];

			usedFilesSet.Add( importFile.Index() );

			String importMd5 = importFile.CalculateMd5();
			rapidjson::Document importJson = importFile.ToJson();
			String guidString = importJson["guid"].GetString();

			File* importHashFile = importsFileSystem.RootDirectory().GetFileByName( guidString + ".hash" );

			if( importHashFile != nullptr ) {
				rapidjson::Document importHashJson = importHashFile->ToJson();

				String resourceHash = importHashJson["resource"].GetString();
				String importHash = importHashJson["import"].GetString();

				if( resourceHash != md5 || importHash != importMd5 ) {
					ImportFile( file, md5, importMd5, guidString );
				} else {
					File* importDataFile = importsFileSystem.FindFile( guidString, 0 );

					if( importDataFile == nullptr ) {
						ImportFile( file, md5, importMd5, guidString );
					} else {
						bool dependencyChanged = false;
						if( importHashJson.HasMember( "dependencies" ) && importHashJson["dependencies"].IsArray() ) {
							for( auto& member : importHashJson["dependencies"].GetArray() ) {
								if( changedFilesSet.Find( Guid::FromString( member.GetString() ) ) != nullptr ) {
									dependencyChanged = true;
									break;
								}
							}
						}
						if( dependencyChanged ) {
							ImportFile( file, md5, importMd5, guidString );
						} else {
							usedFilesSet.Add( importDataFile->Index() );
							usedFilesSet.Add( importHashFile->Index() );
						}
					}
				}
			} else {
				ImportFile( file, md5, importMd5, guidString );
			}
		}
		
		//IMPORTING FILES CHANGED BECAUSE OF DEPENDENCIES
		uint32_t fileChangedCount = 0xFFFFFFFF;
		uint32_t reimportLimit = 5;
		while( fileChangedCount != 0 && reimportLimit > 0 ) {
			fileChangedCount = 0;
			for( uint32_t i = 0; i < resourceFileSystem.files.GetSize(); i++ ) {
				File& file = resourceFileSystem.files[i];
				if( file.Extension() == "import" ) {
					continue;
				}
				File& importFile = GetImportFile( file );
				file = resourceFileSystem.files[i];

				rapidjson::Document importJson = importFile.ToJson();

				String guidString = importJson["guid"].GetString();
				File* importHashFile = importsFileSystem.FindFile( guidString + ".hash", 0 );

				rapidjson::Document importHashJson = importHashFile->ToJson();
				
				bool dependencyChanged = false;
				if( importHashJson.HasMember( "dependencies" ) && importHashJson["dependencies"].IsArray() ) {
					for( auto& member : importHashJson["dependencies"].GetArray() ) {
						if( changedFilesSet.Find( Guid::FromString( member.GetString() ) ) != nullptr ) {
							dependencyChanged = true;
							break;
						}
					}
				}
				if( dependencyChanged ) {
					String resourceHash = importHashJson["resource"].GetString();
					String importHash = importHashJson["import"].GetString();
					ImportFile( file, resourceHash, importHash, guidString );
					fileChangedCount++;
				}
			}
			reimportLimit--;
		}
		
		//REMOVING UNUSED IMPORTED FILES
		//TODO: REMOVE UNUSED .IMPORT FILES
		uint32_t initialImportsFilesCount = importsFileSystem.files.GetSize();
		for( uint32_t i = 0; i < initialImportsFilesCount; i++ ) {
			if( !usedFilesSet.Contains( i ) ) {
				importsFileSystem.files[i].DeleteFromDisk();
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

		const Guid guid = GenerateGuid();
		const String guidString = guid.ToString();

		rapidjson::Document importSettings = newImportFile.ToJson();
		rapidjson::Document::AllocatorType& importSettingsAllocator = importSettings.GetAllocator();

		rapidjson::Value val;
		val.SetString( guidString.GetData(), static_cast<rapidjson::SizeType>( guidString.Length() ), importSettingsAllocator );
		importSettings.AddMember( "guid", val, importSettingsAllocator );

		newImportFile.FromJson( importSettings );

		return newImportFile;
	}

	bool ResourceDatabase::IsGuidOccupied( const Guid& guid ) {
		for( File& file : importsFileSystem.files ) {
			if( file.fileName == guid.ToString() ) {
				return true;
			}
		}
		return false;
	}

	Guid ResourceDatabase::GenerateGuid() {

		Guid guid = Guid::GenerateGuid();
		
		while( IsGuidOccupied( guid ) ) {
			guid = Guid::GenerateGuid();
		}
		
		return guid;
	}

	void ResourceDatabase::ImportFile( File& file, const String& md5, const String& importMd5, const String& guid ) {

		printf( "Importing file %s\n", file.AbsolutePath().GetData() );
		
		File& importFile = importsFileSystem.FindOrCreateFile( guid, 0 );
		Array<Guid> dependencies = editor::ImportFile( file, importFile, Guid::FromString( guid ) );

		changedFilesSet.Add( Guid::FromString( guid ) );
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
		if( dependencies.GetSize() != 0 ) {
			val.SetArray();
			rapidjson::Value arrayElement;
			for( auto& d : dependencies ) {
				arrayElement.SetString( d.ToString().GetData(), static_cast<rapidjson::SizeType>( guid.Length() ), importHashesAllocator );
				val.PushBack( arrayElement, importHashesAllocator );
			}
			importHashesJson.AddMember( "dependencies", val, importHashesAllocator );
		}
		
		importHash.FromJson( importHashesJson );

		usedFilesSet.Add( importHash.Index() );
	}
}
