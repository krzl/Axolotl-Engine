#include "stdafx.h"
#include "ImportManager.h"
#include "EditorFileManager.h"
#include "BaseImporter.h"

namespace axlt::editor {

	Set<Guid> ImportManager::occupiedGuids;
	Map<Guid, String> ImportManager::guidToFilepath;
	Map<String, Guid> ImportManager::filepathToGuid;

	bool HasDataFilesChanged( File& file, uint32_t& currentVersion ) {
		File* importFile = file.ParentDirectory().GetFileByName( file.fileName + ".import" );
		if( importFile == nullptr ) {
			return true;
		}
		rapidjson::Document importJson = importFile->ToJson();
		if( !importJson.HasMember( "guid") ) {
			return true;
		}
		
		String guid = importJson[ "guid" ].GetString();

		File* importDataFile = EditorFileManager::importFileSystem.RootDirectory().GetFileByName( guid );
		File* importInfoFile = EditorFileManager::importFileSystem.RootDirectory().GetFileByName( guid + ".hash" );

		if( importDataFile == nullptr || importInfoFile == nullptr ) {
			return true;
		}

		rapidjson::Document importDataJson = importInfoFile->ToJson();
		ImportInfo importInfo = CreateSerializableFromJson<ImportInfo>( importDataJson );

		currentVersion = importInfo.importVersion;

		if( importInfo.resourceHash != file.CalculateMd5() || importInfo.importHash != importFile->CalculateMd5() ) {
			return true;
		}

		return false;
	}

	void ImportManager::ImportAll() {
		Array<uint32_t> fileIndicesList;
		//Saving indices in file array to avoid errors when creating files during import process
		//Also generating used guid set to avoid collisions when generating new guids
		for( auto& file : EditorFileManager::assetsFileSystem.files ) {
			if( file.Extension() != "import" ) {
				fileIndicesList.Add( file.Index() );
			} else {
				rapidjson::Document json = file.ToJson();
				String guidString = json[ "guid" ].GetString();
				occupiedGuids.Add( Guid::FromString( guidString ) );
			}
		}

		Set<uint32_t> usedAssetFilesList;
		Set<uint32_t> usedImportFilesList;
		
		Set<uint32_t> importedFilesList;
		
		Set<Guid> guidChangedSet1;
		Set<Guid> guidChangedSet2;

		Set<Guid>* guidChangedCurrentIteration = &guidChangedSet1;
		Set<Guid>* guidChangedPreviousIteration = &guidChangedSet2;

		for( uint32_t index : fileIndicesList ) {
			File& file = EditorFileManager::assetsFileSystem.files[ index ];
			if( !BaseImporter::IsImporterDefined( file.Extension() ) ) {
				continue;
			}

			uint32_t currentVersion = 0;
			const ImportResult importResult = HasDataFilesChanged( file, currentVersion ) ? 
				BaseImporter::Import( file ) : BaseImporter::ImportIfOldVersion( file, currentVersion );

			file = EditorFileManager::assetsFileSystem.files[ index ];
			
			if( importResult.isSuccess == ImportResult::SUCCESS ) {
				importedFilesList.Add( file.Index() );
				guidChangedCurrentIteration->Add( importResult.guid );
			}

			if (importResult.isSuccess == ImportResult::SUCCESS || importResult.isSuccess == ImportResult::UP_TO_DATE) {
				usedAssetFilesList.Add( file.Index() );
				File& importFile = *file.ParentDirectory().GetFileByName( file.fileName + ".import" );
				usedAssetFilesList.Add( importFile.Index() );
				guidToFilepath.Add( importResult.guid, file.AbsolutePath() );
				filepathToGuid.Add( file.AbsolutePath(), importResult.guid );

				usedImportFilesList.Add( EditorFileManager::importFileSystem.RootDirectory().GetFileByName( importResult.guid.ToString() )->Index() );
				usedImportFilesList.Add( EditorFileManager::importFileSystem.RootDirectory().GetFileByName( importResult.guid.ToString() + ".hash" )->Index() );
			}
		}

		while( guidChangedCurrentIteration->GetSize() != 0 ) {
			Set<Guid>* temp = guidChangedCurrentIteration;
			guidChangedCurrentIteration = guidChangedPreviousIteration;
			guidChangedPreviousIteration = temp;

			guidChangedCurrentIteration->Clear();

			for( uint32_t index : fileIndicesList ) {
				if( importedFilesList.Contains( index ) ) {
					continue;
				}

				File& file = EditorFileManager::assetsFileSystem.files[ index ];
				File& importFile = *file.ParentDirectory().GetFileByName( file.fileName + ".import" );
				rapidjson::Document json = importFile.ToJson();
				String currentFileGuid = json[ "guid" ].GetString();
				File& importInfoFile = *EditorFileManager::importFileSystem.RootDirectory().GetFileByName( currentFileGuid + ".hash" );
				rapidjson::Document importInfoJson = importInfoFile.ToJson();
				ImportInfo importInfo = CreateSerializableFromJson<ImportInfo>( importInfoJson );
				for( String& dependency : importInfo.dependencies ) {
					Guid guid = Guid::FromString( dependency );
					if( guidChangedPreviousIteration->Contains( guid ) ) {
						const ImportResult importResult = BaseImporter::Import( file );

						if( importResult.isSuccess == ImportResult::SUCCESS ) {
							guidChangedCurrentIteration->Add( Guid::FromString( currentFileGuid ) );
						}
					}
				}
			}
		}

		uint32_t initialImportsFilesCount = EditorFileManager::importFileSystem.files.GetDataArraySize();
		for( uint32_t i = 0; i < initialImportsFilesCount; i++ ) {
			if( !EditorFileManager::importFileSystem.files.IsFree( i ) && !usedImportFilesList.Contains( i ) ) {
				EditorFileManager::importFileSystem.files[ i ].DeleteFromDisk();
			}
		}

		uint32_t initialAssetFilesCount = EditorFileManager::assetsFileSystem.files.GetDataArraySize();
		for( uint32_t i = 0; i < initialAssetFilesCount; i++ ) {
			File& file = EditorFileManager::assetsFileSystem.files[ i ];
			if( !EditorFileManager::assetsFileSystem.files.IsFree( i ) && !usedAssetFilesList.Contains( i ) && file.Extension() == "import" ) {
				EditorFileManager::assetsFileSystem.files[ i ].DeleteFromDisk();
			}
		}
	}

	Guid ImportManager::GenerateGuid() {
		Guid guid = Guid::GenerateGuid();
		while( occupiedGuids.Contains( guid ) ) {
			guid = Guid::GenerateGuid();
		}
		occupiedGuids.Add( guid );
		return guid;
	}
}
