cbuffer ConstantBuffer : register(b0)
{
	matrix cameraTransform;
	matrix worldTransform;
};

struct VertexShaderInput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 clipPosition : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float3 worldPosition : POSITION;
};

VertexShaderOutput main(VertexShaderInput input)
{
	float4 worldPosition = mul(float4(input.position, 1.0f), worldTransform);
	float4 clipPosition = mul(worldPosition, cameraTransform);

	matrix worldRotation = worldTransform;
	worldRotation[3][0] = 0.0f;
	worldRotation[3][1] = 0.0f;
	worldRotation[3][2] = 0.0f;
	worldRotation[3][3] = 1.0f;
	float4 worldNormal = mul(float4(input.normal, 1.0f), worldRotation);

	VertexShaderOutput output;
	output.clipPosition = clipPosition;
	output.color = input.color;
	output.normal = float3(worldNormal.x, worldNormal.y, worldNormal.z);
	output.texCoord = input.texCoord;
	output.worldPosition = float3(worldPosition.x, worldPosition.y, worldPosition.z);

	return output;
}
