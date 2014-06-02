struct Input
{
	float4 clipPosition : SV_POSITION;
	float4 color : COLOR;
	float2 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float3 worldPosition : POSITION;
};

float4 main(Input input) : SV_TARGET
{
	return input.color;
}
