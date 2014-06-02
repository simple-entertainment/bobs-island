cbuffer ConstantBuffer : register(b0)
{
	matrix cameraTransform;
	matrix worldTransform;
	float2 translation;
};

struct VertexShaderInput
{
	float4 color : COLOR0;
	float3 normal : NORMAL0;
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
	float4 clipPosition : SV_POSITION;
	float4 color : COLOR0;
	float3 normal : NORMAL0;
	float2 texCoord : TEXCOORD0;
	float3 worldPosition : POSITION;
};

VertexShaderOutput main(VertexShaderInput input)
{
	float4 clipPosition = float4(input.position, 1.0f);
	clipPosition.x += translation.x;
	clipPosition.y += translation.y;

	VertexShaderOutput output;
	output.clipPosition = clipPosition;
	output.color = input.color;
	output.normal = float3(0.0f, 0.0f, 0.0f);
	output.texCoord = input.texCoord;
	output.worldPosition = float3(0.0f, 0.0f, 0.0f);

	return output;
}
