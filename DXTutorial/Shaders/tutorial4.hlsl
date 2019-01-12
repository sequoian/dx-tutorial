// tutorial4.hlsl
Texture2D colorMap : register(t0);
SamplerState linearSampler : register(s0);

cbuffer Constants : register(b0)
{
	row_major float4x4 worldMat;
	row_major float4x4 viewProjMat;
}

struct VSInput
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct PSInput
{
	float4 pos : SV_Position;
	float2 uv : COLOR;
};

PSInput vsmain(VSInput input)
{
	float4 worldPos = mul(float4(input.pos, 1.0f), worldMat);

	PSInput output;
	output.pos = mul(worldPos, viewProjMat);
	output.uv = input.uv;

	return output;
}

float4 psmain(PSInput input) : SV_Target0
{
	return colorMap.Sample(linearSampler, input.uv);
}
