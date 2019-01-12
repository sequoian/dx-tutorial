// tutorial3.hlsl
struct VSInput
{
	float3 pos : POSITION;
	float4 color : TEXCOORD0;
};

struct PSInput
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};

PSInput vsmain(VSInput input)
{
	PSInput output;
	output.pos = float4(input.pos, 1.0f);
	output.color = input.color;

	return output;
}

float4 psmain(PSInput input) : SV_Target0
{
	return input.color;
}
