#include "../stdafx.h"
#include "FileImport.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_DXT_IMPLEMENTATION

#include <stb_image.h>
#include <stb_dxt.h>

namespace axlt::editor {
	TextureResource* ImportTexture( File& file, Array<Guid>& dependencies ) {

		int32_t x, y, n;
		stbi_uc* data = stbi_load( file.AbsolutePath().GetData(), &x, &y, &n, 4 );

		TextureResource* texture = new TextureResource();

		texture->width = x;
		texture->height = y;
		texture->channelCount = n;
		texture->format = n == 4 ? TextureFormat::DXT5 : TextureFormat::DXT1;

		texture->textureData.AddRange( data, x * y / ( n == 4 ? 1 : 2 ) );

		rygCompress( texture->textureData.GetData(), data, x, y, n == 4 );

		return texture;
	}
}