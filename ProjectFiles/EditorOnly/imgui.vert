#version 450
#extension GL_GOOGLE_include_directive : require
#include "common.glsl"

layout (location = 0) in vec3 inPos;
layout (location = 4) in vec4 inColor;
layout (location = 5) in vec2 inUV;

layout (set=1, binding=0) uniform ImguiData {
	vec4 scale;
	vec4 translate;
};

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

void main() {
	outUV = inUV;
	outColor = inColor;
	gl_Position = vec4(inPos.xy * scale.xy + translate.xy, 0.0, 1.0);
}