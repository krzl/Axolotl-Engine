#pragma once
#include <Resources/ResourceHandle.h>
#include <Resources/ModelResource.h>
#include <Resources/BinaryResource.h>
#include <Graphics/TechniqueResource.h>
#include <Graphics/MaterialResource.h>
#include <Graphics/TextureResource.h>

namespace axlt::editor {
	ResourceHandle<void> ImportFile( File& file, const Guid& guid );

	ModelResource* ImportModel( File& file );
	BinaryResource* ImportShader( File& file );
	TechniqueResource* ImportTechnique( File& file );
	MaterialResource* ImportMaterial( File& file );
	TextureResource* ImportTexture( File& file );
}
