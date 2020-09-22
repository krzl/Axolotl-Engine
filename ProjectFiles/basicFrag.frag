#version 450
  
layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 FragColor;

layout(set=1, binding=1) uniform sampler2D albedo;

void main() {
    FragColor = texture( albedo, uv ) * 2.0;
} 