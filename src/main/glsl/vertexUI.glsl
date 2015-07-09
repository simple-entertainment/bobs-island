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

out Point point;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	point.texCoord = texCoord;

	gl_Position = vec4(position, 1.0);
}
