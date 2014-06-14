struct Input
{
	float4 clipPosition : SV_POSITION;
	float4 color : COLOR0;
	float2 normal : NORMAL0;
	float2 texCoord : TEXCOORD0;
	float3 worldPosition : POSITION;
};

float4 main(Input input) : SV_TARGET
{
	return input.color;
}
