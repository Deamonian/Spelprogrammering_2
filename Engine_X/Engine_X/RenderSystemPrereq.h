// RenderSystemPrereq.h

#ifndef RENDERSYSTEMPREREQ_H_INCLUDED
#define RENDERSYSTEMPREREQ_H_INCLUDED

enum EPrimitive 
{
	PRIM_POINT,
	PRIM_LINE_LIST,
	PRIM_LINE_STRIP,
	PRIM_TRIANGLE_LIST,
	PRIM_TRIANGLE_STRIP,
};

enum EAccessMode 
{
	BM_DEFAULT,
	BM_DYNAMIC,
	BM_STATIC,
};

enum EAddressMode 
{
	AM_WRAP,
	AM_CLAMP,
	AM_MIRROR,
};

enum EFilterMode 
{
	FM_NEAREST,
	FM_LINEAR,
};

enum ECompareMode 
{
	CM_UNKNOWN,
	CM_NEVER,
	CM_LESS,
	CM_EQUAL,
	CM_LESS_EQUAL,
	CM_GREATER,
	CM_NOT_EQUAL,
	CM_GREATER_EQUAL,
	CM_ALWAYS,
};

enum EStencilOp 
{
	SO_UNKNOWN,
	SO_KEEP,
	SO_ZERO,
	SO_REPLACE,
	SO_INCR_SATURATE,
	SO_DECR_SATURATE,
	SO_INVERT,
	SO_INCREMENT,
	SO_DECREMENT,
};

enum EBlendMode 
{
	BM_ADD,
	BM_SUBTRACT,
	BM_REVERSE_SUBTRACT,
	BM_MIN,
	BM_MAX,
};

enum EBlendType 
{
	BT_ZERO,
    BT_ONE,
    BT_SRC_COLOR,
    BT_INV_SRC_COLOR,
    BT_SRC_ALPHA,
    BT_INV_SRC_ALPHA,
    BT_DEST_ALPHA,
    BT_INV_DEST_ALPHA,
    BT_DEST_COLOR,
    BT_INV_DEST_COLOR,
    BT_SRC_ALPHA_SAT,
    BT_BT_FACTOR,
    BT_INV_BLEND_FACTOR,
    BT_SRC1_COLOR,
    BT_INV_SRC1_COLOR,
    BT_SRC1_ALPHA,
    BT_INV_SRC1_ALPHA
};

enum ECullMode 
{
	CULL_NONE,
	CULL_BACK,
	CULL_FRONT,
};

enum EFillMode 
{
	FILL_WIREFRAME,
	FILL_SOLID,
};

struct Constant 
{
	Constant();
	void Release();

	unsigned int mSize;
	unsigned char* mData;
	ID3D10Buffer* mBuffer;
};

struct Sampler 
{
	Sampler();
	Sampler(const Sampler& rhs);
	void operator=(const Sampler& rhs);
	void Release();

	ID3D10SamplerState* mState;
};

struct Shader 
{
	Shader();

	void AllocateVSConstants(int count, int size[]);
	void SetVSConstant(int index, int size, void* data);
	void AllocatePSConstants(int count, int size[]);
	void SetPSConstant(int index, int size, void* data);
	void AllocateSamplers(int count);
	void SetSampler(int index, Sampler& sampler);
	void Release();

	static unsigned int msIDGen;
	unsigned int mID;

	ID3D10VertexShader* mVertexShader;
	ID3D10PixelShader* mPixelShader;
	ID3D10InputLayout* mInputLayout;
	ID3D10Blob* mSignature;

	int mNumVSConstants;
	Constant* mVSConstants;
	bool* mVSDirty;

	int mNumPSConstants;
	Constant* mPSConstants;
	bool* mPSDirty;

	int mNumSamplers;
	Sampler* mSamplers;
};

struct Texture 
{
	Texture();
	void Release();

	static unsigned int msIDGen;
	unsigned int mID;
	DXGI_FORMAT mFormat;

	ID3D10Resource* mResource;
	ID3D10ShaderResourceView* mShaderResourceView;
	ID3D10RenderTargetView* mRenderTargetView;
	ID3D10DepthStencilView* mDepthStencilView;
};

struct IndexBuffer 
{
	IndexBuffer();
	void Release();
	static unsigned int msIDGen;
	unsigned int mID;
	int mSize;
	int mCount;
	ID3D10Buffer* mBuffer;
};

struct VertexBuffer 
{
	VertexBuffer();
	void Release();

	static unsigned int msIDGen;
	unsigned int mID;
	int mSize;
	int mCount;
	float mRadius;
	ID3D10Buffer* mBuffer;
};

struct DepthState 
{
	DepthState();
	void Release();

	static unsigned int msIDGen;
	unsigned int mID;
	ID3D10DepthStencilState* mState;
};

struct BlendState 
{
	BlendState();
	void Release();

	static unsigned int msIDGen;
	unsigned int mID;
	ID3D10BlendState* mState;
};

struct RasterizerState 
{
	RasterizerState();
	void Release();

	static unsigned int msIDGen;
	unsigned int mID;
	ID3D10RasterizerState* mState;
};

struct Renderable 
{
	Renderable();
	void Release();

	VertexBuffer mVertexBuffer;
	IndexBuffer mIndexBuffer;
	unsigned int mCount;
};

struct RenderState 
{
	RenderState();
	void Reset();

	Shader* mShader;
	Texture* mTexture[8];
	IndexBuffer* mIndexBuffer;
	VertexBuffer* mVertexBuffer[4];
	DepthState* mDepthState;
	BlendState* mBlendState;
	RasterizerState* mRasterizerState;
};

#endif // RENDERSYSTEMPREREQ_H_INCLUDED
