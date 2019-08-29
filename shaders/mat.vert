#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

out vec2 vTexCoords;
out vec3 vPosition;
out vec3 vNormal;

uniform mat4 viewProj;
uniform mat4 model;
uniform mat3 normalMat;

void main()
{
	vec4 worldPosition = model * vec4(position,1.0);
	vPosition = worldPosition.xyz;
	vNormal = normalMat * normal;
    gl_Position = viewProj * worldPosition;
	vTexCoords = texcoords;
}