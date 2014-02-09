#version 330

// /////////////////////////
// Structures
// /////////////////////////

struct Point
{
	vec4 clipPosition;
	vec4 color;
	vec3 normal;
	vec2 texCoord;
	vec3 worldPosition;
};

// /////////////////////////
// Variables
// /////////////////////////

layout (location = 0) in vec4 color;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 position;
layout (location = 3) in vec2 texCoord;

uniform vec2 translation;

out Point point;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	vec4 clipPosition = vec4(position, 1.0);
	clipPosition.x += translation.x;
	clipPosition.y += translation.y;

	point.clipPosition = clipPosition;
	point.color = color;
	point.texCoord = texCoord;

	gl_Position = clipPosition;
}
