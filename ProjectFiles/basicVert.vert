#version 450
#extension GL_GOOGLE_include_directive : require
#include "common.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 5) in vec2 aUV;

layout (location = 0) out vec2 uv;

void main()
{
	gl_Position = pvMatrix * modelMatrix * vec4(aPos, 1.0f);
	uv = aUV;
}