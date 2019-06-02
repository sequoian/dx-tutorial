Texture2D colorMap : register(t0);
SamplerState linearSampler : register(s0);

cbuffer Constants : register(b0)
{
	row_major float4x4 worldMat;
	row_major float4x4 viewProjMat;
	float3 cameraPos;
	float3 lightDirection;
	float4 lightColor;
	float4 ambientColor;
	float4 specularColor;
}

struct VSInput
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
	float4 color : TEXCOORD1;
};

struct PSInput
{
	float4 pos : SV_Position;
	float2 uv : UV;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float3 viewDir : VIEWDIR;
};

PSInput vsmain(VSInput input)
{
	float4 worldPos = mul(float4(input.pos, 1.0f), worldMat);

	PSInput output;
	output.pos = mul(worldPos, viewProjMat);
	output.uv = input.uv;
	output.color = input.color;
	float3 normal = mul(input.normal, (float3x3)worldMat);
	output.normal = normal;
	output.viewDir = cameraPos - worldPos;

	return output;
}

float4 psmain(PSInput input) : SV_Target0
{
	float4 albedo = input.color * colorMap.Sample(linearSampler, input.uv);
	float3 normal = normalize(input.normal);
	float diffuseFactor = saturate(dot(normal, lightDirection));

	// calculate specular lighting
	float3 viewDirection = normalize(input.viewDir);
	float3 reflection = reflect(-lightDirection, normal);
	float specFactor = saturate(dot(reflection, viewDirection));
	float3 specular = specularColor.rgb * pow(specFactor, specularColor.a);

	float3 color = albedo.rgb * (lightColor * diffuseFactor + ambientColor) + specular;
	return float4(color, 1.0f);
}
