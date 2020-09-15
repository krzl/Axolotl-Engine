#include "stdafx.h"
#include "TextureImport.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_DXT_IMPLEMENTATION

#include <Graphics/TextureResource.h>
#include "FileSystem/File.h"

#include "Import/ImporterRegistrator.h"

#include <stb_image.h>
#include <stb_dxt.h>

namespace axlt::editor {
	TextureResource* TextureImporter( const File& file, TextureImport& import, Array<Guid>& dependencies ) {

		int32_t x, y, n;
		stbi_uc* data = stbi_load( file.AbsolutePath().GetData(), &x, &y, &n, 4 );

		TextureResource* texture = new TextureResource();

		texture->width = x;
		texture->height = y;
		texture->depth = 1;
		texture->channelCount = n;
		
		if( import.format == TextureFormat::UNKNOWN ) {
			texture->format = n == 4 ? TextureFormat::DXT5 : TextureFormat::DXT1;
		} else {
			texture->format = import.format;
		}

		texture->dimension = TextureDimension::TEX_2D;
		texture->mipmapCount = 1;
		texture->arrayCount = 1;
		texture->sampleCount = import.sampleCount;

		texture->minFilter = import.minFilter;
		texture->magFilter = import.magFilter;

		texture->mipmapFilterLinear = import.mipmapFilterLinear;

		texture->addressModeU = import.addressModeU;
		texture->addressModeV = import.addressModeV;
		texture->addressModeW = import.addressModeW;

		texture->mipLodBias = import.mipLodBias;
		texture->anisotropicFiltering = import.anisotropicFiltering;

		switch( texture->format ) {
			case TextureFormat::DXT1:
			case TextureFormat::DXT5:
				texture->textureData.AddEmpty( x * y * n );
				rygCompress( texture->textureData.GetData(), data, x, y, n == 4 );
				break;
			case TextureFormat::RGBA32:
			default:
				texture->textureData.AddRange( data, x * y * n );
				break;
		};
		
		return texture;
	}

	static ImporterRegistrator textureImporterRegistrator = ImporterRegistrator( TextureImporter, 1, { "png" } );
}
