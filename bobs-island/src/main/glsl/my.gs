#version 330

// /////////////////////////
// Constants
// /////////////////////////

float PI = 3.1416;

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
// Functions
// /////////////////////////

mat3 createRotationMatrix(vec3 axis, float angle)
{
	mat3 rotationMatrix;

	rotationMatrix[0][0] = cos(angle) + pow(axis.x, 2) * (1 - cos(angle));
	rotationMatrix[0][1] = axis.x * axis.y * (1 - cos(angle)) - axis.z * sin(angle);
	rotationMatrix[0][2] = axis.x * axis.z * (1 - cos(angle)) + axis.y * sin(angle);

	rotationMatrix[1][0] = axis.y * axis.x * (1 - cos(angle)) + axis.z * sin(angle);
	rotationMatrix[1][1] = cos(angle) + pow(axis.y, 2) * (1 - cos(angle));
	rotationMatrix[1][2] = axis.y * axis.z * (1 - cos(angle)) - axis.x * sin(angle);

	rotationMatrix[2][0] = axis.z * axis.x * (1 - cos(angle)) - axis.y * sin(angle);
	rotationMatrix[2][1] = axis.z * axis.y * (1 - cos(angle)) + axis.x * sin(angle);
	rotationMatrix[2][2] = cos(angle) + pow(axis.z, 2) * (1 - cos(angle));

	return rotationMatrix;
}

vec3 flip(
	float distanceToEffectOrigin,
	float effectSpeed,
	float effectTime,
	float flipDuration,
	vec3 flipVertexPosition,
	vec3 otherVertexAPosition,
	vec3 otherVertexBPosition)
{
	float flipStartTime = distanceToEffectOrigin / effectSpeed;

	if (effectTime < flipStartTime && false)
	{
		return otherVertexAPosition;
	}

	if (effectTime < flipStartTime || effectTime > flipStartTime + flipDuration)
	{
		return flipVertexPosition;
	}

	vec3 flipAxis = otherVertexBPosition - otherVertexAPosition;
	vec3 flipAxisCenter = otherVertexAPosition + (flipAxis / 2.0f);
	vec3 flipAxisCenterToFlipVertex = flipVertexPosition - flipAxisCenter;
	float flipFraction = (effectTime - flipStartTime) / flipDuration;

	mat3 rotation = createRotationMatrix(normalize(flipAxis), PI - flipFraction * PI);

	return flipAxisCenter + flipAxisCenterToFlipVertex * rotation;
}

// /////////////////////////
// Variables
// /////////////////////////

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

in Point point[];

uniform mat4 cameraTransform;
uniform vec3 effectOrigin;
uniform float effectSpeed;
uniform float effectTime;

out Point point1;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	float maxDistanceToEffectOrigin = length(effectOrigin - point[0].worldPosition);
	uint flipVertexIndex = 0u;
	uint otherVertexAIndex = 1u;
	uint otherVertexBIndex = 2u;

	if (length(effectOrigin - point[1].worldPosition) > maxDistanceToEffectOrigin)
	{
		maxDistanceToEffectOrigin = length(effectOrigin - point[1].worldPosition);
		flipVertexIndex = 1u;
		otherVertexAIndex = 2u;
		otherVertexBIndex = 0u;
	}

	if (length(effectOrigin - point[2].worldPosition) > maxDistanceToEffectOrigin)
	{
		maxDistanceToEffectOrigin = length(effectOrigin - point[2].worldPosition);
		flipVertexIndex = 2u;
		otherVertexAIndex = 0u;
		otherVertexBIndex = 1u;
	}

	vec3 flipVertexPosition = flip(
		maxDistanceToEffectOrigin,
		effectSpeed,
		effectTime,
		1.0f,
		point[flipVertexIndex].worldPosition,
		point[otherVertexAIndex].worldPosition,
		point[otherVertexBIndex].worldPosition);

	point1.clipPosition = cameraTransform * vec4(flipVertexPosition, 1.0f);
	point1.colour = point[flipVertexIndex].colour;
	point1.normal = point[flipVertexIndex].normal;
	point1.texCoord = point[flipVertexIndex].texCoord;
	point1.worldPosition = flipVertexPosition;

	gl_Position = point1.clipPosition;
	EmitVertex();

	point1.clipPosition = point[otherVertexAIndex].clipPosition;
	point1.colour = point[otherVertexAIndex].colour;
	point1.normal = point[otherVertexAIndex].normal;
	point1.texCoord = point[otherVertexAIndex].texCoord;
	point1.worldPosition = point[otherVertexAIndex].worldPosition;

	gl_Position = point1.clipPosition;
	EmitVertex();

	point1.clipPosition = point[otherVertexBIndex].clipPosition;
	point1.colour = point[otherVertexBIndex].colour;
	point1.normal = point[otherVertexBIndex].normal;
	point1.texCoord = point[otherVertexBIndex].texCoord;
	point1.worldPosition = point[otherVertexBIndex].worldPosition;

	gl_Position = point1.clipPosition;
	EmitVertex();

	point1.clipPosition = cameraTransform * vec4(flipVertexPosition, 1.0f);
	point1.colour = point[flipVertexIndex].colour;
	point1.normal = point[flipVertexIndex].normal;
	point1.texCoord = point[flipVertexIndex].texCoord;
	point1.worldPosition = flipVertexPosition;

	gl_Position = point1.clipPosition;
	EmitVertex();

	EndPrimitive();
}
