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
// Functions
// /////////////////////////

float applyWave(vec3 position, float time)
{
	float amplitude = 0.5;
	float waveLengthX = -10.15;
	float waveLengthY = 9.37;
	float waveTimeX = time * 0.4;
	float waveTimeY = time * 0.7;

	return cos(position.z / waveLengthY + waveTimeY) * amplitude;
}

// /////////////////////////
// Variables
// /////////////////////////

layout (location = 0) in vec4 color;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 position;
layout (location = 3) in vec2 texCoord;

uniform mat4 cameraTransform;
uniform float time = 0.0;
uniform mat4 worldTransform;

out Point point;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	vec4 worldPosition = worldTransform * vec4(position, 1.0);
	worldPosition.y = applyWave(worldPosition.xyz, time);
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
