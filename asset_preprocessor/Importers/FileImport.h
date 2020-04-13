#pragma once
#include <FileSystem/File.h>

#include <Resources/ResourceData.h>
#include <Resources/ModelResource.h>

namespace axlt::editor {
	ResourceData ImportFile( File& file );

	ModelResource* ImportFbx( File& file );
}
