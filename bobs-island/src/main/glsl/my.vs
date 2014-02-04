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

	mat4 worldRotation = worldTransform;
	worldRotation[3][0] = 0.0f;
	worldRotation[3][1] = 0.0f;
	worldRotation[3][2] = 0.0f;
	worldRotation[3][3] = 1.0f;
	vec4 worldNormal = worldRotation * vec4(normal, 1.0f);

	point.clipPosition = clipPosition;
	point.color = color;
	point.normal = worldNormal.xyz;
	point.texCoord = texCoord;
	point.worldPosition = worldPosition.xyz;

	gl_Position = clipPosition;
}
