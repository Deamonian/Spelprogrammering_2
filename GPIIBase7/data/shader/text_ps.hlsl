// pixelshader.hlsl

struct PixelShaderInput {
    float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

SamplerState defaultSampler : register(s0);
Texture2D diffuse : register(t0);

float4 main(PixelShaderInput input) : SV_TARGET {
	float4 color = diffuse.Sample(defaultSampler,input.tex);
	if(color.a<0.001) {discard;};
    return input.color*color.a;
};
