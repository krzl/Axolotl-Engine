#pragma once

#include <glslang/Public/ShaderLang.h>

namespace axlt::editor {

	void TryInitializeGlslang();

	EShLanguage GetStageFromExtension( const File& file );
	void ParseShader( const File& file, glslang::TShader& shader );

}
