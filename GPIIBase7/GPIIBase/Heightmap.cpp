// Heightmap.cpp

#include "stdafx.h"
#include "RenderSystem.h"
#include "Heightmap.h"

Heightmap::Heightmap() {
};

bool Heightmap::Create(RenderSystem *render_system,const String &filename) {
	Bitmap bitmap;
	if(!bitmap.Load(filename)) {
		System::Notify("Could not load heightmap from file!","Create Heightmap Error!");
		System::Break();
		return false;
	};

	unsigned int width=bitmap.GetWidth();
	unsigned int height=bitmap.GetHeight();
	unsigned int bpp=Bitmap::GetFormatSize(bitmap.GetFormat());
	unsigned char *data=bitmap.GetData();

	// read verticesdata from bitmap, especially height
	unsigned int index=0;
	Vertex *vertices=new Vertex[width*height];
	for(unsigned int z=0;z<height;z++) {
		for(unsigned int x=0;x<width;x++) {
			float y=(float)data[(z*width+x)*bpp]*0.05f;
			vertices[index].position=Vector3((float)x,y,(float)z);
			vertices[index].texcoord=Vector2((float)x/(float)width,(float)z/(float)height);
			vertices[index].normal=Vector3(0.0f,1.0f,0.0f);
			index++;
		};
	};
	data=nullptr;
	bitmap.Release();

	// terrain smoothing
	const float factors[]={0.125f,0.5f};
	unsigned int smooth_passes=10;
	for(unsigned int p=0;p<smooth_passes;p++) {
		for(unsigned int z=1;z<height-1;z++) {
			for(unsigned int x=1;x<width-1;x++) {
				float y0=vertices[(z)*width+(x-1)].position.y;
				float y1=vertices[(z)*width+(x)  ].position.y;
				float y2=vertices[(z)*width+(x+1)].position.y;
				float y3=vertices[(z-1)*width+(x)].position.y;
				float y4=vertices[(z+1)*width+(x)].position.y;

				float y=y0*factors[0]+y2*factors[0]+y3*factors[0]+y4*factors[0] + y1*factors[1];
				vertices[(z)*width+(x)].position.y=y;
			};
		};
	};

	// create indices to connect everything into triangles (a triangle strip could be used)
	index=0;
	unsigned short *indices=new unsigned short[(width-1)*(height-1)*6];
	for(unsigned int z=0;z<height-1;z++) {
		for(unsigned int x=0;x<width-1;x++) {
			indices[index++]=(z)*width+(x);
			indices[index++]=(z+1)*width+(x);
			indices[index++]=(z+1)*width+(x+1);

			indices[index++]=(z+1)*width+(x+1);
			indices[index++]=(z)*width+(x);
			indices[index++]=(z)*width+(x+1);

			/*indices[index++]=(z)*width+(x);
			indices[index++]=(z+1)*width+(x);
			indices[index++]=(z+1)*width+(x+1);

			indices[index++]=(z+1)*width+(x+1);
			indices[index++]=(z)*width+(x);
			indices[index++]=(z)*width+(x+1);*/
		};
	};
	m_renderable.m_count=index;

	// calculate all normals for all triangles, add up the normal values
	for(unsigned int i=0;i<index;i+=3) {
		int i0=indices[i+0],i1=indices[i+1],i2=indices[i+2];
		Vector3 v0=vertices[i0].position;
		Vector3 v1=vertices[i1].position;
		Vector3 v2=vertices[i2].position;

		Vector3 p=v1-v0;
		Vector3 q=v2-v0;
		Vector3 n=p.Cross(q);
		n.Normalize();

		vertices[i0].normal+=n;
		vertices[i1].normal+=n;
		vertices[i2].normal+=n;
	};

	// finally normalize the total of all normals 
	// (since one vertex can be included in more than one triangle)
	// (approximation of the normal)
	index=width*height;
	for(unsigned int i=0;i<index;i++) {
		vertices[i].normal.Normalize();
	};

	render_system->CreateVertexBuffer(&m_renderable.m_vertex_buffer,BM_DEFAULT,width*height,sizeof(Vertex),vertices);
	render_system->CreateIndexBuffer(&m_renderable.m_index_buffer,m_renderable.m_count,sizeof(unsigned short),indices);
	delete[] vertices; delete[] indices;

	render_system->CreateShader(&m_shader,
		"../data/shader/heightmap_vs.hlsl",
		"../data/shader/heightmap_ps.hlsl");

	int vs_s[]={sizeof(OncePerFrame)};
	int ps_s[]={sizeof(DirectionalLight)};
	m_shader.AllocateVSConstants(1,vs_s);
	m_shader.AllocatePSConstants(1,ps_s);
	render_system->CreateConstantBuffers(&m_shader);

	Sampler sampler;
	render_system->CreateSamplerState(&sampler,AM_WRAP,AM_WRAP,AM_WRAP,FM_LINEAR,CM_NEVER);
	m_shader.AllocateSamplers(1);
	m_shader.SetSampler(0,sampler);

	D3D10_INPUT_ELEMENT_DESC desc[]={
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D10_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,20,D3D10_INPUT_PER_VERTEX_DATA,0}
	};
	render_system->CreateInputLayout(&m_shader,desc,3);

	// texture
	render_system->CreateTexture(&m_texture,"../data/texture/grass.dds");

	// depth
	render_system->CreateDepthState(&m_depth);

	// rasterizer
	render_system->CreateRasterizerState(&m_rasterizer,CULL_NONE,FILL_SOLID);

	return true;
};

void Heightmap::Destroy() {
	m_shader.Release();
	m_texture.Release();
	m_renderable.m_index_buffer.Release();
	m_renderable.m_vertex_buffer.Release();
};

void Heightmap::Render(RenderSystem *render_system,OncePerFrame *frame,DirectionalLight *light) {
	m_shader.SetVSConstant(0,sizeof(OncePerFrame),frame);
	m_shader.SetPSConstant(0,sizeof(DirectionalLight),light);

	render_system->SelectShader(&m_shader);
	render_system->SelectTexture(&m_texture,0);
	render_system->SelectIndexBuffer(&m_renderable.m_index_buffer);
	render_system->SelectVertexBuffer(&m_renderable.m_vertex_buffer,0);
	render_system->SelectDepthState(&m_depth);
	render_system->SelectRasterizerState(&m_rasterizer);
	render_system->Apply();
	render_system->DrawIndexed(PRIM_TRIANGLE_LIST,0,0,m_renderable.m_count);
};
