// gui_vs.hlsl

struct PixelShaderInput {
    float4 pos : SV_POSITION;
	float4 color : COLOR;
};

float4 main(PixelShaderInput input) : SV_TARGET {
	float4 color = input.color;
	if(color.a<0.001) {discard;};
    return color;
};
