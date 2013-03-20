// text_vs.hlsl

cbuffer OncePerFrame : register(b0) {
	row_major float4x4 projection;
};

struct VertexShaderInput {
    float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

struct PixelShaderInput {
    float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

PixelShaderInput main(VertexShaderInput input) {
	float4 pos = float4(input.pos,1.0f);
	pos = mul(pos,projection);
		
	PixelShaderInput OUT;
    OUT.pos = pos;
	OUT.tex = input.tex;
	OUT.color = input.color;
    return OUT;
};
