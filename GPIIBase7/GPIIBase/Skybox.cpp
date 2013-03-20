// Skybox.cpp

#include "stdafx.h"
#include "RenderSystem.h"
#include "Camera.h"
#include "Skybox.h"

Skybox::Skybox() {
};

Skybox::~Skybox() {
	Shut();
};

void Skybox::Init(RenderSystem *render_system) {
	// allocate constants and create shader
	int sizes[]={sizeof(OncePerFrame)};
	m_shader.AllocateVSConstants(1,sizes);
	render_system->CreateConstantBuffers(&m_shader);
	render_system->CreateShader(&m_shader,
		"../data/shader/skybox_vs.hlsl",
		"../data/shader/skybox_ps.hlsl");

	// create input layout
	D3D10_INPUT_ELEMENT_DESC desc[]={
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D10_INPUT_PER_VERTEX_DATA,0}
	};
	render_system->CreateInputLayout(&m_shader,desc,sizeof(desc)/sizeof(desc[0]));

	// create vertex buffer
	const float v=20.0f;
	const float w=1.0f;
	const float y=2.0f;
	const float z=0.5f;
	Vector3 
		a(-v, v, v),b( v, v, v),c( v, v,-v),d(-v, v,-v),
		e(-v,-w, v),f( v,-w, v),g( v,-w,-v),h(-v,-w,-v),

		i(-v, y, v),j( v, y, v),k( v, y,-v),l(-v, y,-v),

		m(-v, z, v),n( v, z, v),o( v, z,-v),p(-v, z,-v),
		q(-v,-z, v),r( v,-z, v),s( v,-z,-v),t(-v,-z,-v);

	unsigned int colors[]={
		0xFFF49936,
		0xFFFEFFC7,
		0xffCCBBAA};

	Vertex vertices[]={
		{a,colors[0]}, {b,colors[0]}, {c,colors[0]}, {d,colors[0]},
		{e,colors[2]}, {f,colors[2]}, {g,colors[2]}, {h,colors[2]},

		{i,colors[0]}, {j,colors[0]}, {k,colors[0]}, {l,colors[0]},

		{m,colors[1]}, {n,colors[1]}, {o,colors[1]}, {p,colors[1]},
		{q,colors[1]}, {r,colors[1]}, {s,colors[1]}, {t,colors[1]}
	};
	render_system->CreateVertexBuffer(&m_renderable.m_vertex_buffer,BM_DEFAULT,
		sizeof(vertices)/sizeof(vertices[0]),sizeof(Vertex),vertices);

	// create index buffer
	int A=0,B=1,C=2,D=3,E=4,F=5,G=6,H=7;
	int I=8,J=9,K=10,L=11,M=12,N=13,O=14,P=15;
	int Q=16,R=17,S=18,T=19;
	unsigned short indices[]={
		B,A,D, D,C,B, // 0
		E,F,G, G,H,E, // 2
		B,C,G, G,F,B,
		A,B,F, F,E,A, 
		D,A,E, E,H,D,
		C,D,H, H,G,C,
		// 0
		/*B,C,K, K,J,B,
		A,B,J, J,I,A,
		D,A,I, I,L,D,
		C,D,L, L,K,C,
		// 1
		J,K,O, O,N,J,
		I,J,N, N,M,I,
		L,I,M, M,P,L,
		K,L,P, P,O,K,
		// 1
		N,O,S, S,R,N,
		M,N,R, R,Q,M,
		P,M,Q, Q,T,P,
		O,P,T, T,S,O,
		// 2
		R,S,G, G,F,R,
		Q,R,F, F,E,Q,
		T,Q,E, E,H,T, 
		S,T,H, H,G,S,*/
	};
	m_renderable.m_count=sizeof(indices)/sizeof(indices[0]);
	render_system->CreateIndexBuffer(&m_renderable.m_index_buffer,
		m_renderable.m_count,sizeof(unsigned short),indices);

	// create depth state
	render_system->CreateDepthState(&m_depth_state,false,false);
};

void Skybox::Render(RenderSystem *render_system,Camera *camera) {
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
	render_system->SelectIndexBuffer(&m_renderable.m_index_buffer);
	render_system->SelectVertexBuffer(&m_renderable.m_vertex_buffer,0);
	render_system->SelectDepthState(&m_depth_state);
	render_system->Apply();
	render_system->DrawIndexed(PRIM_TRIANGLE_LIST,0,0,m_renderable.m_count);
};


void Skybox::Shut() {
	m_shader.Release();
	m_depth_state.Release();
};
