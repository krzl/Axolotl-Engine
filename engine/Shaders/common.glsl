layout(set=0, binding=0) uniform PerCameraUniformBuffer {
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 pvMatrix;
	vec3 cameraPosition;
};

layout( push_constant ) uniform PerDrawUniformBuffer {
	mat4 modelMatrix;
	mat4 pvmMatrix;
};