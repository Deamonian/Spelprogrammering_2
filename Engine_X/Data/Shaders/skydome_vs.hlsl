// skybox_vs.hlsl

cbuffer OncePerFrame : register(b0) {
	row_major float4x4 projection;
	row_major float4x4 view;
};

struct VertexShaderInput {
    float3 pos : POSITION;
};

struct PixelShaderInput {
    float4 pos : SV_POSITION;
	float4 color : COLOR;
};

PixelShaderInput main(VertexShaderInput input) {
	float4 pos = float4(input.pos,1.0f);
	pos = mul(pos,view);
	pos = mul(pos,projection);
	
	PixelShaderInput OUT;
    OUT.pos = pos;
	OUT.color = input.pos.y;
    return OUT;
};