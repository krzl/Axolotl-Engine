#pragma once
#include <FileSystem/File.h>

#include <Resources/ResourceData.h>
#include <Resources/ModelResource.h>
#include <Resources/BinaryResource.h>

namespace axlt::editor {
	ResourceData ImportFile( File& file, const Guid& guid );

	ModelResource* ImportModel( File& file );
	BinaryResource* ImportShader( File& file );
}
