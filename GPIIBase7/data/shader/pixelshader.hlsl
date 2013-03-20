// pixelshader.hlsl

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

float4 main(PixelShaderInput input) : SV_TARGET {
	float4 color = diffuse.Sample(defaultSampler,input.tex);
	float3 normal = normalize(input.nor);
	float NdL = dot(normal,light_direction.xyz);
	
    return color * NdL;
};
