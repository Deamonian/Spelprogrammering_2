// Skydome.cpp

#include "stdafx.h"
#include "RenderSystem.h"
#include "Camera.h"
#include "Skydome.h"

void Skydome::Init(RenderSystem *render_system) {
	// allocate constants and create shader
	int sizes[]={sizeof(OncePerFrame)};
	m_shader.AllocateVSConstants(1,sizes);
	render_system->CreateConstantBuffers(&m_shader);
	render_system->CreateShader(&m_shader,
		"../data/shader/skydome_vs.hlsl",
		"../data/shader/skydome_ps.hlsl");

	// create input layout
	D3D10_INPUT_ELEMENT_DESC desc[]={
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0}
	};
	render_system->CreateInputLayout(&m_shader,desc,sizeof(desc)/sizeof(desc[0]));

	// create sampler
	Sampler sampler;
	render_system->CreateSamplerState(&sampler,AM_CLAMP,AM_CLAMP,AM_CLAMP,FM_LINEAR,CM_ALWAYS);
	m_shader.AllocateSamplers(1);
	m_shader.SetSampler(0,sampler);

	// create texture
	render_system->CreateTexture(&m_texture,"../data/texture/gradient_sky.tga");
	
	// create depth state
	render_system->CreateDepthState(&m_depth_state,false,false);

	// create icosahedron
	const float X=0.525731112119133606f, Z=0.850650808352039932f;
	Vertex vertices[]={
		Vector3(-X,0.0f,Z), Vector3(X,0.0f,Z), Vector3(-X,0.0f,-Z), Vector3(X,0.0f,-Z),
		Vector3(0.0f,Z,X), Vector3(0.0f,Z,-X), Vector3(0.0f,-Z,X), Vector3(0.0f,-Z,-X),
		Vector3(Z,X,0.0f), Vector3(-Z,X,0.0f), Vector3(Z,-X,0.0f), Vector3(-Z,-X,0.0f)
	};
	struct Indices {
		unsigned short a,b,c;
	};
	Indices indices[]={
		{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
		{8,10,1},{8,3,10},{5,3,8}, {5,2,3}, {2,7,3},
		{7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
		{6,1,10},{9,0,11},{9,11,2},{9,2,5}, {7,2,11}
	};

	for(int i=0;i<20;i++) {
		Subdivide(vertices[indices[i].a].position,
			vertices[indices[i].b].position,
			vertices[indices[i].c].position,3);
	};
	
	// create vertex buffer
	m_renderable.m_count=m_subdivision.size();
	render_system->CreateVertexBuffer(&m_renderable.m_vertex_buffer,BM_DEFAULT,
		m_renderable.m_count,sizeof(Vertex),&m_subdivision[0]);
	m_subdivision.clear();
};

void Skydome::Shut() {
	m_shader.Release();
	m_depth_state.Release();
};

void Skydome::Render(class RenderSystem *render_system,class Camera *camera) {
	using namespace DirectX;
	
	XMFLOAT4X4 rot;
	XMStoreFloat4x4(&rot,camera->GetView());
	rot._41=0.0f; rot._42=0.0f; rot._43=0.0f;
	Mat4 view=XMLoadFloat4x4(&rot);

	OncePerFrame frame;
	frame.proj=camera->GetProj();
	frame.view=view;
	m_shader.SetVSConstant(0,sizeof(OncePerFrame),&frame);

	render_system->SelectShader(&m_shader);
	render_system->SelectTexture(&m_texture,0);
	render_system->SelectIndexBuffer(nullptr);
	render_system->SelectVertexBuffer(&m_renderable.m_vertex_buffer,0);
	render_system->SelectDepthState(&m_depth_state);
	render_system->Apply();
	render_system->Draw(PRIM_TRIANGLE_LIST,0,m_renderable.m_count);
};


// private
void Skydome::Subdivide(Vector3 &v0,Vector3 &v1,Vector3 &v2,int depth) {
	if(depth==0) {
		m_subdivision.push_back(v0);
		m_subdivision.push_back(v1);
		m_subdivision.push_back(v2);
		return;
	};

	Vector3 sv0,sv1,sv2;
	sv0=v0+v1;
	sv1=v1+v2;
	sv2=v2+v0;
	sv0.Normalize();
	sv1.Normalize();
	sv2.Normalize();

	m_subdivision.push_back(sv0);
	m_subdivision.push_back(sv1);
	m_subdivision.push_back(sv2);

	Subdivide(v0,sv0,sv2,depth-1);
	Subdivide(v1,sv1,sv0,depth-1);
	Subdivide(v2,sv2,sv1,depth-1);
	Subdivide(sv0,sv1,sv2,depth-1);
};
