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

in Point point;

uniform sampler2D sampler;
uniform int samplerEnabled;

layout(location = 0) out vec4 color;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	if (samplerEnabled == 1)
	{
		color = texture2D(sampler, point.texCoord.st);
	}
	else
	{
		color = point.color;
	}
}
