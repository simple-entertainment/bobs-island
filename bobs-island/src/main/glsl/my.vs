#version 330

// /////////////////////////
// Structures
// /////////////////////////

struct VertexVS
{
	vec4 colour;
	vec3 normal;
	vec3 position;
	vec2 texCoord;
};

struct VertexFS
{
	vec3 clipPosition;
	vec4 colour;
	vec3 normal;
	vec2 texCoord;
	vec3 worldPosition;
};

// /////////////////////////
// Variables
// /////////////////////////

layout (location = 0) in VertexVS vertexVS;

uniform mat4 cameraTransform;
uniform mat4 worldTransform;

out VertexFS vertexFS;

// /////////////////////////
// Shader
// /////////////////////////

void main()
{
	vec4 worldPosition4 = worldTransform * vec4(vertexVS.position, 1.0);
	vec4 clipPosition4 = cameraTransform * worldTransform * vec4(vertexVS.position, 1.0);

	vertexFS.clipPosition = clipPosition4.xyz;
	vertexFS.colour = vertexVS.colour;
	vertexFS.normal = vertexVS.normal;
	vertexFS.texCoord = vertexVS.texCoord;
	vertexFS.worldPosition = worldPosition4.xyz;

	gl_Position = clipPosition4;
}
