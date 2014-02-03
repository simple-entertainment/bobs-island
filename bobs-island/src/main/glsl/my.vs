#version 330

// /////////////////////////
// Structures
// /////////////////////////

struct Point
{
	vec4 clipPosition;
	vec4 colour;
	vec3 normal;
	vec2 texCoord;
	vec3 worldPosition;
};

// /////////////////////////
// Variables
// /////////////////////////

layout (location = 0) in vec4 colour;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 position;
layout (location = 3) in vec2 texCoord;

uniform mat4 cameraTransform;
uniform mat4 worldTransform;

out Point point;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	vec4 worldPosition = worldTransform * vec4(position, 1.0);
	vec4 clipPosition = cameraTransform * worldPosition;

	point.clipPosition = clipPosition;
	point.colour = colour;
	point.normal = normal;
	point.texCoord = texCoord;
	point.worldPosition = worldPosition.xyz;

	gl_Position = clipPosition;
}
