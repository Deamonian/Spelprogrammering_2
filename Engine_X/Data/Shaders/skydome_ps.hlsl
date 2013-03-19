// skybox_ps.hlsl

struct PixelShaderInput {
    float4 pos : SV_POSITION;
	float4 color : COLOR;
};

SamplerState defaultSampler : register(s0);
Texture2D diffuse : register(t0);

float4 main(PixelShaderInput input) : SV_TARGET {
	return diffuse.Sample(defaultSampler,saturate(input.color.y+0.1));
};