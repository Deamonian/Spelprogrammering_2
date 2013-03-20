// RenderSystemPrereq.cpp

#include "stdafx.h"
#include <cassert>
#include "RenderSystemPrereq.h"

#define SAFE_RELEASE(x) if(x) {x->Release(); x=nullptr;}

// -------------------------------------------------------
unsigned int Shader::ms_id_gen=0;
unsigned int Texture::ms_id_gen=0;
unsigned int IndexBuffer::ms_id_gen=0;
unsigned int VertexBuffer::ms_id_gen=0;
unsigned int BlendState::ms_id_gen=0;
unsigned int DepthState::ms_id_gen=0;
unsigned int RasterizerState::ms_id_gen=0;

// -------------------------------------------------------
Constant::Constant() {
	m_size=0;
	m_data=nullptr;
	m_buffer=nullptr;
};

void Constant::Release() {
	m_size=0;
	if(m_data) {
		delete[] m_data;
		m_data=nullptr;
	};
	SAFE_RELEASE(m_buffer);
};

Sampler::Sampler() {
	m_state=nullptr;
};

Sampler::Sampler(const Sampler &rhs) {
	m_state=rhs.m_state;
};
void Sampler::operator=(const Sampler &rhs) {
	m_state=rhs.m_state;
};
void Sampler::Release() {
	SAFE_RELEASE(m_state);
};

Shader::Shader() {
	m_id=ms_id_gen++;
	m_vertex_shader=nullptr;
	m_pixel_shader=nullptr;
	m_input_layout=nullptr;
	m_signature=nullptr;

	m_num_vs_constants=0;
	m_vs_constants=nullptr;
	m_vs_dirty=nullptr;

	m_num_ps_constants=0;
	m_ps_constants=nullptr;
	m_ps_dirty=nullptr;

	m_num_samplers=0;
	m_samplers=nullptr;
};

void Shader::AllocateVSConstants(int count,int size[]) {
	m_num_vs_constants=count;
	m_vs_constants=new Constant[count];
	m_vs_dirty=new bool[count];
	for(int i=0;i<count;i++) {
		m_vs_constants[i].m_size=size[i];
		m_vs_constants[i].m_data=new unsigned char[m_vs_constants[i].m_size];
		m_vs_dirty[i]=false;
	};
};

void Shader::SetVSConstant(int index,int size,void *data) {
	assert(index<m_num_vs_constants);
	if(m_vs_constants[index].m_data==nullptr) {
		::memcpy(m_vs_constants[index].m_data,data,size);
		m_vs_dirty[index]=true;
	};
	if(::memcmp(m_vs_constants[index].m_data,data,size)!=0) {
		::memcpy(m_vs_constants[index].m_data,data,size);
		m_vs_dirty[index]=true;
	};
};

void Shader::AllocatePSConstants(int count,int size[]) {
	m_num_ps_constants=count;
	m_ps_constants=new Constant[count];
	m_ps_dirty=new bool[count];
	for(int i=0;i<count;i++) {
		m_ps_constants[i].m_size=size[i];
		m_ps_constants[i].m_data=new unsigned char[m_ps_constants[i].m_size];
		m_ps_dirty[i]=false;
	};
};

void Shader::SetPSConstant(int index,int size,void *data) {
	assert(index<m_num_ps_constants);
	if(m_ps_constants[index].m_data==nullptr) {
		::memcpy(m_ps_constants[index].m_data,data,size);
		m_ps_dirty[index]=true;
	}
	if(::memcmp(m_ps_constants[index].m_data,data,size)!=0) {
		::memcpy(m_ps_constants[index].m_data,data,size);
		m_ps_dirty[index]=true;
	};
};

void Shader::AllocateSamplers(int count) {
	m_num_samplers=count;
	m_samplers=new Sampler[m_num_samplers];
};

void Shader::SetSampler(int index,Sampler &sampler) {
	assert(index<m_num_samplers);
	m_samplers[index]=sampler;
};

void Shader::Release() {
	SAFE_RELEASE(m_vertex_shader);
	SAFE_RELEASE(m_pixel_shader);
	SAFE_RELEASE(m_input_layout);
	SAFE_RELEASE(m_signature);

	m_num_vs_constants=0;
	if(m_vs_constants) {
		delete[] m_vs_constants;
		m_vs_constants=nullptr;
	};
	if(m_vs_dirty) {
		delete[] m_vs_dirty;
		m_vs_dirty=nullptr;
	};

	m_num_ps_constants=0;
	if(m_ps_constants) {
		delete[] m_ps_constants;
		m_ps_constants=nullptr;
	};
	if(m_ps_dirty) {
		delete[] m_ps_dirty;
		m_ps_dirty=nullptr;
	};

	m_num_samplers=0;
	if(m_samplers) {
		delete[] m_samplers;
		m_samplers=nullptr;
	};
};

Texture::Texture() {
	m_id=ms_id_gen++;
	m_format=DXGI_FORMAT_UNKNOWN;
	m_resource=nullptr;
	m_shader_resource_view=nullptr;
	m_render_target_view=nullptr;
	m_depth_stencil_view=nullptr;
};

void Texture::Release() {
	SAFE_RELEASE(m_resource);
	SAFE_RELEASE(m_shader_resource_view);
};

IndexBuffer::IndexBuffer() {m_id=ms_id_gen++;m_buffer=nullptr;};
void IndexBuffer::Release() {SAFE_RELEASE(m_buffer);};

VertexBuffer::VertexBuffer() {m_id=ms_id_gen++;m_size=0;m_count=0;m_radius=0.0f;m_buffer=nullptr;};
void VertexBuffer::Release() {SAFE_RELEASE(m_buffer);};

DepthState::DepthState() {m_id=ms_id_gen++;m_state=nullptr;};
void DepthState::Release() {SAFE_RELEASE(m_state);};

BlendState::BlendState() {m_id=ms_id_gen++;m_state=nullptr;};
void BlendState::Release() {SAFE_RELEASE(m_state);};

RasterizerState::RasterizerState() {m_id=ms_id_gen++;m_state=nullptr;};
void RasterizerState::Release() {SAFE_RELEASE(m_state);};

Renderable::Renderable() {m_count=0;};
void Renderable::Release() {
	m_index_buffer.Release();
	m_vertex_buffer.Release();
};

// ------------------------------------------------------------------------
RenderState::RenderState() {
	Reset();
};

void RenderState::Reset() {
	m_shader=nullptr;
	for(int i=0;i<8;i++) {m_texture[i]=nullptr;};
	m_index_buffer=nullptr;
	for(int i=0;i<4;i++) {m_vertex_buffer[i]=nullptr;};
	m_depth_state=nullptr;
	m_blend_state=nullptr;
	m_rasterizer_state=nullptr;
};
