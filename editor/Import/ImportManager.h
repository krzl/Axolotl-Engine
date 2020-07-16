#pragma once
#include <FileSystem/Guid.h>
#include <Collections/Set.h>
#include <Collections/Map.h>

namespace axlt::editor {
	class ImportManager {

	public:

		static void ImportAll();
		static Guid GenerateGuid();
		
		static Map<Guid, String> guidToFilepath;
		
	private:

		static Set<Guid> occupiedGuids;
	};
}
