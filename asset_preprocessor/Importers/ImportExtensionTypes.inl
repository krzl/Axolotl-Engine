// ReSharper disable once CppMissingIncludeGuard

#ifndef IMPORT_FUNCTION
#define IMPORT_FUNCTION( Function, Version, Extension, ResourceType )
#endif

IMPORT_FUNCTION( ImportModel, 1, "fbx", ModelResource )
IMPORT_FUNCTION( ImportShader, 1, "vert", BinaryResource )
IMPORT_FUNCTION( ImportShader, 1, "frag", BinaryResource )
IMPORT_FUNCTION( ImportTechnique, 1, "tnq", TechniqueResource )
IMPORT_FUNCTION( ImportMaterial, 1, "mat", MaterialResource )
IMPORT_FUNCTION( ImportTexture, 1, "png", TextureResource )

#undef IMPORT_FUNCTION