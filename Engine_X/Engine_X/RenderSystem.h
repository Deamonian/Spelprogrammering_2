// RenderSystem.h

#ifndef RENDERSYSTEM_H_INCLUDED
#define RENDERSYSTEM_H_INCLUDED

#include "RenderSystemPrereq.h"

class RenderSystem 
{
public:
	RenderSystem();
	~RenderSystem();

	bool Init(int width, int height, HWND window_handle);
	void Shut();

	void Clear();
	void Present();

	bool CreateShader(Shader* shader, const String& vertex_shader, const String& pixel_shader);
	bool CreateIndexBuffer(IndexBuffer* index_buffer, int count, int size, void* buffer);
	bool CreateVertexBuffer(VertexBuffer* vertex_buffer, EAccessMode access, int count, int size, void* buffer);
	bool CreateInputLayout(Shader* shader, D3D10_INPUT_ELEMENT_DESC* desc, int count);
	bool CreateConstantBuffers(Shader* shader);
	bool CreateSamplerState(Sampler* sampler,
		EAddressMode addr_u, EAddressMode addr_v, EAddressMode addr_w,
		EFilterMode filter, ECompareMode compare);

	bool CreateDepthState(DepthState* state, bool depth_test = true, bool depth_write = true, ECompareMode depth_func = CM_LESS,
		bool stencil_test = false, ECompareMode stencil_func_front = CM_ALWAYS, ECompareMode stencil_func_back = CM_ALWAYS,
		EStencilOp stencil_fail_front = SO_KEEP, EStencilOp stencil_fail_back = SO_KEEP,
		EStencilOp depth_fail_front = SO_KEEP, EStencilOp depth_fail_back = SO_KEEP,
		EStencilOp stencil_pass_front = SO_REPLACE, EStencilOp stencil_pass_back = SO_KEEP);

	bool CreateBlendState(BlendState* state, EBlendType src_rgb, EBlendType dst_rgb,
		EBlendType src_alpha, EBlendType dst_alpha,
		EBlendMode blend_rgb, EBlendMode blend_alpha, unsigned char mask = 0x0f);
	
	bool CreateRasterizerState(RasterizerState* state, ECullMode cull, EFillMode fill);

	bool CreateTexture(Texture *texture,const String &filename);
	bool CreateTexture(Texture *texture,EAccessMode access,EBmpFmt format,int width,int height,int depth=1);

	void SelectShader(Shader* shader);
	void SelectIndexBuffer(IndexBuffer* buffer);
	void SelectVertexBuffer(VertexBuffer* buffer, unsigned int slot);
	void SelectDepthState(DepthState* state);
	void SelectBlendState(BlendState* state);
	void SelectRasterizerState(RasterizerState* state);
	void SelectTexture(Texture* texture,unsigned int slot);

	void Apply();

	void Draw(EPrimitive prim, int start, int count);
	void DrawIndexed(EPrimitive prim, int index_start, int vertex_start, int count);

	void UpdateConstantBuffers(Shader* shader);
	void UpdateVertexBuffer(VertexBuffer* buffer, int size, void* data);
	void UpdateTexture(Texture *texture,class Bitmap *bitmap);

private:
	void ApplyShader();
	void ApplyConstants();
	void ApplySamplers();
	void ApplyIndexBuffer();
	void ApplyVertexBuffers();
	void ApplyDepthState();
	void ApplyBlendState();
	void ApplyRasterizerState();
	void ApplyTextures();

private:
	int mWidth, mHeight;
	ID3D10Device* mDevice;
	IDXGISwapChain* mSwapChain;
	ID3D10RenderTargetView* mRenderTargetView;
	ID3D10DepthStencilView* mDepthStencilView;

	RenderState mSelected;
	RenderState mCurrent;
};

#endif // RENDERSYSTEM_H_INCLUDED
