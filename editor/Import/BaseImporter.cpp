#include "stdafx.h"
#include "BaseImporter.h"

#include "FileSystem/File.h"
#include "EditorFileManager.h"

namespace axlt::editor {

	ImportResult BaseImporter::Import( const File& file ) {
		Tuple<ImportResult(*)( const File&, ImportFunctionTypeless, uint32_t), ImportFunctionTypeless, uint32_t>* importPairPtr = GetImportFunctions().Find( file.Extension() );
		if( importPairPtr != nullptr ) {
			return importPairPtr->Get<0>()( file, importPairPtr->Get<1>(), importPairPtr->Get<2>() );
		}

		return ImportResult{
			ImportResult::NO_IMPORTER_FOUND,
			Array<Guid>(),
			0,
			Guid::invalidGuid
		};
	}

	ImportResult BaseImporter::ImportIfOldVersion( const File& file, const uint32_t version ) {
		Tuple<ImportResult(*)( const File&, ImportFunctionTypeless, uint32_t), ImportFunctionTypeless, uint32_t >* importPairPtr = GetImportFunctions().Find( file.Extension() );
		if( importPairPtr != nullptr ) {
			if( version != importPairPtr->Get<2>() ) {
				return importPairPtr->Get<0>()(file, importPairPtr->Get<1>(), importPairPtr->Get<2>());
			}

			return ImportResult{
				ImportResult::UP_TO_DATE,
				Array<Guid>(),
				version,
				Guid::FromString( file.ParentDirectory().GetFileByName( file.fileName + ".import" )->ToJson()[ "guid" ].GetString() )
			};
		}

		return ImportResult{
			ImportResult::NO_IMPORTER_FOUND,
			Array<Guid>(),
			0,
			Guid::invalidGuid
		};
	}

	bool BaseImporter::IsImporterDefined( const String& extension ) {
		Tuple<ImportResult(*)( const File&, ImportFunctionTypeless, uint32_t), ImportFunctionTypeless, uint32_t>* importPairPtr = GetImportFunctions().Find( extension );
		return importPairPtr != nullptr;
	}

	Map<String, Tuple<ImportResult(*)( const File&, ImportFunctionTypeless, uint32_t), ImportFunctionTypeless, uint32_t>>& BaseImporter::GetImportFunctions() {
		static Map<String, Tuple<ImportResult(*)( const File&, ImportFunctionTypeless, uint32_t), ImportFunctionTypeless, uint32_t>> map;
		return map;
	}
}
