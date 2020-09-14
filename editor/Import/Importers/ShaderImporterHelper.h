#pragma once

#undef max
#include <glslang/Public/ShaderLang.h>
#include <glslang/Include/Types.h>

namespace axlt::editor {

	void TryInitializeGlslang();

	EShLanguage GetStageFromExtension( const File& file );
	void ParseShader( const File& file, glslang::TShader& shader );

}
