// heightmap_ps.hlsl

cbuffer DirectionalLight : register(b0) {
	float4 light_direction;
	float4 light_diffuse;
	float4 light_ambient;
	float4 eye_position;
};

struct PixelShaderInput {
    float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 nor : NORMAL;
};

SamplerState defaultSampler : register(s0);
Texture2D diffuse : register(t0);

float4 CalculatePhongLighting(float3 normal,float3 light_dir) {
	float4 ia=light_ambient;
	float4 id=saturate(dot(normal,light_dir));
	return (ia+(id*light_diffuse)*light_direction.w);
};

float4 main(PixelShaderInput input) : SV_TARGET {
	float4 lighting = CalculatePhongLighting(normalize(input.nor),-light_direction.xyz);
	float4 texel = diffuse.Sample(defaultSampler,input.tex*16);
	return texel * lighting;
};