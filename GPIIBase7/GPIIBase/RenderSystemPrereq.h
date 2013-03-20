// RenderSystemPrereq.h

#ifndef RENDERSYSTEMPREREQ_H_INCLUDED
#define RENDERSYSTEMPREREQ_H_INCLUDED

enum EPrimitive {
	PRIM_POINT,
	PRIM_LINE_LIST,
	PRIM_LINE_STRIP,
	PRIM_TRIANGLE_LIST,
	PRIM_TRIANGLE_STRIP,
};

enum EAccessMode {
	BM_DEFAULT,
	BM_DYNAMIC,
	BM_STATIC,
};

enum EAddressMode {
	AM_WRAP,
	AM_CLAMP,
	AM_MIRROR,
};

enum EFilterMode {
	FM_NEAREST,
	FM_LINEAR,
};

enum ECompareMode {
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

enum EStencilOp {
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

enum EBlendMode {
	BM_ADD,
	BM_SUBTRACT,
	BM_REVERSE_SUBTRACT,
	BM_MIN,
	BM_MAX,
};

enum EBlendType {
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

enum ECullMode {
	CULL_NONE,
	CULL_BACK,
	CULL_FRONT,
};

enum EFillMode {
	FILL_WIREFRAME,
	FILL_SOLID,
};

struct Constant {
	Constant();
	void Release();

	unsigned int m_size;
	unsigned char *m_data;
	ID3D10Buffer *m_buffer;
};

struct Sampler {
	Sampler();
	Sampler(const Sampler &rhs);
	void operator=(const Sampler &rhs);
	void Release();

	ID3D10SamplerState *m_state;
};

struct Shader {
	Shader();

	void AllocateVSConstants(int count,int size[]);
	void SetVSConstant(int index,int size,void *data);
	void AllocatePSConstants(int count,int size[]);
	void SetPSConstant(int index,int size,void *data);
	void AllocateSamplers(int count);
	void SetSampler(int index,Sampler &sampler);
	void Release();

	static unsigned int ms_id_gen;
	unsigned int m_id;

	ID3D10VertexShader *m_vertex_shader;
	ID3D10PixelShader *m_pixel_shader;
	ID3D10InputLayout *m_input_layout;
	ID3D10Blob *m_signature;

	int m_num_vs_constants;
	Constant *m_vs_constants;
	bool *m_vs_dirty;

	int m_num_ps_constants;
	Constant *m_ps_constants;
	bool *m_ps_dirty;

	int m_num_samplers;
	Sampler *m_samplers;
};

struct Texture {
	Texture();
	void Release();

	static unsigned int ms_id_gen;
	unsigned int m_id;
	DXGI_FORMAT m_format;

	ID3D10Resource *m_resource;
	ID3D10ShaderResourceView *m_shader_resource_view;
	ID3D10RenderTargetView *m_render_target_view;
	ID3D10DepthStencilView *m_depth_stencil_view;
};

struct IndexBuffer {
	IndexBuffer();
	void Release();
	static unsigned int ms_id_gen;
	unsigned int m_id;
	int m_size;
	int m_count;
	ID3D10Buffer *m_buffer;
};

struct VertexBuffer {
	VertexBuffer();
	void Release();

	static unsigned int ms_id_gen;
	unsigned int m_id;
	int m_size;
	int m_count;
	float m_radius;
	ID3D10Buffer *m_buffer;
};

struct DepthState {
	DepthState();
	void Release();

	static unsigned int ms_id_gen;
	unsigned int m_id;
	ID3D10DepthStencilState *m_state;
};

struct BlendState {
	BlendState();
	void Release();

	static unsigned int ms_id_gen;
	unsigned int m_id;
	ID3D10BlendState *m_state;
};

struct RasterizerState {
	RasterizerState();
	void Release();

	static unsigned int ms_id_gen;
	unsigned int m_id;
	ID3D10RasterizerState *m_state;
};

struct Renderable {
	Renderable();
	void Release();

	VertexBuffer m_vertex_buffer;
	IndexBuffer m_index_buffer;
	unsigned int m_count;
};

struct RenderState {
	RenderState();
	void Reset();

	Shader *m_shader;
	Texture *m_texture[8];
	IndexBuffer *m_index_buffer;
	VertexBuffer *m_vertex_buffer[4];
	DepthState *m_depth_state;
	BlendState *m_blend_state;
	RasterizerState *m_rasterizer_state;
};

#endif // RENDERSYSTEMPREREQ_H_INCLUDED
