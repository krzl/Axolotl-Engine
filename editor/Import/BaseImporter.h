#pragma once
#include <Collections/Tuple.h>
#include <Collections/Map.h>
#include "Helpers/JsonSerializable.h"
#include <FileSystem/File.h>
#include <FileSystem/Directory.h>
#include <FileSystem/Guid.h>
#include "ImportManager.h"
#include "ImportInfo.h"
#include "EditorFileManager.h"

namespace axlt {
	class Guid;
}

namespace axlt::editor {

	template<typename ResourceType, typename ImportType>
	using ImportFunction = ResourceType* (*)(const File&, ImportType&, Array<Guid>&);

	typedef Serializable* (*ImportFunctionTypeless)(const File&, Serializable&, Array<Guid>&);

	struct ImportResult {
		enum Result {
			SUCCESS,
			UP_TO_DATE,
			NO_IMPORTER_FOUND,
			FAIL
		};
		
		Result isSuccess = NO_IMPORTER_FOUND;
		Array<Guid> dependentResources{};
		uint32_t importVersion = 0;
		Guid guid = Guid::invalidGuid;
	};
	
	class BaseImporter {

	public:

		template<typename ResourceType, typename ImportType>
		static void RegisterImporter( 
			const ImportFunction<ResourceType, ImportType> importFunction, 
			const String& extension, const uint32_t version ) {

			Tuple<ImportResult(*)(const File&, ImportFunctionTypeless, uint32_t), ImportFunctionTypeless, uint32_t> importTuple( (ImportResult(*)( const File&, ImportFunctionTypeless, uint32_t)) ImportInner<ResourceType, ImportType>, (ImportFunctionTypeless) importFunction, version );
;			GetImportFunctions().Emplace( extension, importTuple );
		}

		template<typename ResourceType, typename ImportType>
		static ImportResult ImportInner( const File& file, const ImportFunction<ResourceType, ImportType> importFunction, const uint32_t version ) {
			const File* filePtr = &file;
			File* importFile = filePtr->ParentDirectory().GetFileByName( filePtr->fileName + ".import" );//TODO:
			if( !importFile ) {
				const uint32_t fileIndex = filePtr->Index();
				importFile = &filePtr->ParentDirectory().CreateFile( filePtr->fileName + ".import" );
				filePtr = &EditorFileManager::assetsFileSystem.files[ fileIndex ];
			}
			rapidjson::Document importJson = importFile->ToJson();
			ImportResult result = {
				ImportResult::FAIL,
				Array<Guid>(),
				version,
				Guid::invalidGuid
			};

			ResourceType* resource = nullptr;
			try {
				Guid guid = Guid::invalidGuid;
				ImportType import = CreateSerializableFromJsonWithGuid<ImportType>( importJson, guid );
				if( guid == Guid::invalidGuid ) {
					guid = ImportManager::GenerateGuid();
				}
				result.guid = guid;
				resource = importFunction( *filePtr, import, result.dependentResources );
				SaveSerializableToJsonWithGuid( import, *importFile, guid );

				File& importHashFile = EditorFileManager::importFileSystem.FindOrCreateFile( guid.ToString() + ".hash", 0 );
				ImportInfo importInfo( filePtr->CalculateMd5(), importFile->CalculateMd5(), result.importVersion, result.dependentResources );
				SaveSerializableToJson( importInfo, importHashFile );

				File& importDataFile = EditorFileManager::importFileSystem.FindOrCreateFile( guid.ToString(), 0 );
				Serializer serializer( importDataFile, "wb" );
				serializer << *resource << Serializer::end;

				delete resource;
				resource = nullptr;
				result.isSuccess = ImportResult::SUCCESS;
				return result;
			} catch( int ) {
				printf( "Could not import file %s", filePtr->AbsolutePath().GetData() );
				delete resource;
				return result;
			}
		}

		static ImportResult Import( const File& file );
		static ImportResult ImportIfOldVersion( const File& file, uint32_t version );

		static bool IsImporterDefined( const String& extension );
		
	private:

		static Map<String, Tuple<ImportResult(*)( const File&, ImportFunctionTypeless, uint32_t), ImportFunctionTypeless, uint32_t>>& GetImportFunctions();
	};
}
