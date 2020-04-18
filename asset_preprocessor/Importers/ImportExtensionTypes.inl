// ReSharper disable once CppMissingIncludeGuard

#ifndef IMPORT_FUNCTION
#define IMPORT_FUNCTION( Function, Version, Extension, ResourceType )
#endif

IMPORT_FUNCTION( ImportModel, 1, "fbx", ModelResource )
IMPORT_FUNCTION( ImportShader, 1, "vert", BinaryResource )
IMPORT_FUNCTION( ImportShader, 1, "frag", BinaryResource )

#undef IMPORT_FUNCTION