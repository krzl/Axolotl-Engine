#pragma once
#include <Resources/ResourceHandle.h>
#include <Resources/ModelResource.h>
#include <Resources/BinaryResource.h>
#include <Graphics/TechniqueResource.h>
#include <Graphics/MaterialResource.h>
#include <Graphics/TextureResource.h>

namespace axlt::editor {
	Array<Guid> ImportFile( File& file, File& importFile, const Guid& guid );

	ModelResource* ImportModel( File& file, Array<Guid>& dependencies );
	BinaryResource* ImportShader( File& file, Array<Guid>& dependencies );
	TechniqueResource* ImportTechnique( File& file, Array<Guid>& dependencies );
	MaterialResource* ImportMaterial( File& file, Array<Guid>& dependencies );
	TextureResource* ImportTexture( File& file, Array<Guid>& dependencies );
}
