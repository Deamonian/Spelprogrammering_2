// RenderSystem.cpp

#include "stdafx.h"
#include "uBitmap.h"
#include "Dds.h"
#include "RenderSystem.h"

#define SAFE_RELEASE(x) if(x) {x->Release(); x=nullptr;}

static const DXGI_FORMAT formats[]={DXGI_FORMAT_UNKNOWN,DXGI_FORMAT_R8_UNORM,DXGI_FORMAT_R8G8_UNORM,DXGI_FORMAT_UNKNOWN,DXGI_FORMAT_R8G8B8A8_UNORM};
static const D3D10_USAGE usages[]={D3D10_USAGE_DEFAULT,D3D10_USAGE_DYNAMIC,D3D10_USAGE_IMMUTABLE};

RenderSystem::RenderSystem() {
	m_width=0; m_height=0;
	m_device=nullptr;
	m_swapchain=nullptr;
	m_rendertargetview=nullptr;
	m_depthstencilview=nullptr;
};

RenderSystem::~RenderSystem() {
	Shut();
};

bool RenderSystem::Init(int width,int height,HWND window_handle) {
	m_width=width; m_height=height;

	DXGI_SWAP_CHAIN_DESC sc_desc={0};
	sc_desc.BufferCount=1;
	sc_desc.BufferDesc.Width=m_width;
	sc_desc.BufferDesc.Height=m_height;
	sc_desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc.OutputWindow=window_handle;
	sc_desc.SampleDesc.Count=1;
	sc_desc.SampleDesc.Quality=0;
	sc_desc.Windowed=TRUE;

	HRESULT hr=D3D10CreateDeviceAndSwapChain(
		NULL,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
#if (_DEBUG)
		D3D10_CREATE_DEVICE_DEBUG,
#else
		0,
#endif
		D3D10_SDK_VERSION,
		&sc_desc,
		&m_swapchain,
		&m_device);
	if(hr!=S_OK) {
		::MessageBoxA(NULL,"Could not create DirectX SwapChain and/or Device!","ERROR!",MB_OK|MB_ICONERROR);
		return false;
	};

	ID3D10Texture2D *backbuffer=nullptr;
	hr=m_swapchain->GetBuffer(0,__uuidof(ID3D10Texture2D),(void**)&backbuffer);
	if(hr!=S_OK) {
		return false;
	};

	hr=m_device->CreateRenderTargetView(backbuffer,NULL,&m_rendertargetview);
	if(hr!=S_OK) {
		return false;
	};
	backbuffer->Release();

	// create depth buffer
	D3D10_TEXTURE2D_DESC tdesc;
	tdesc.Width=width;
	tdesc.Height=height;
	tdesc.MipLevels=1;
	tdesc.ArraySize=1;
	tdesc.Format=DXGI_FORMAT_D32_FLOAT;
	tdesc.SampleDesc.Count=1;
	tdesc.SampleDesc.Quality=0;
	tdesc.Usage=D3D10_USAGE_DEFAULT;
	tdesc.BindFlags=D3D10_BIND_DEPTH_STENCIL;
	tdesc.CPUAccessFlags=0;
	tdesc.MiscFlags=0;

	ID3D10Texture2D *texture=nullptr;
	hr=m_device->CreateTexture2D(&tdesc,NULL,&texture);
	if(hr!=S_OK) {
		return false;
	};

	D3D10_DEPTH_STENCIL_VIEW_DESC vdesc;
	vdesc.Format=tdesc.Format;
	vdesc.ViewDimension=D3D10_DSV_DIMENSION_TEXTURE2D;
	vdesc.Texture2D.MipSlice=0;

	hr=m_device->CreateDepthStencilView(texture,&vdesc,&m_depthstencilview);
	if(hr!=S_OK) {
		return false;
	};

	m_device->OMSetRenderTargets(1,&m_rendertargetview,m_depthstencilview);

	D3D10_VIEWPORT viewport={0,0,m_width,m_height,0.0f,1.0f};
	m_device->RSSetViewports(1,&viewport);

	m_device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
};

void RenderSystem::Shut() {
	if(m_device) {
		m_device->VSSetShader(NULL);
		m_device->PSSetShader(NULL);
		m_device->OMSetRenderTargets(0,NULL,NULL);
	};

	SAFE_RELEASE(m_depthstencilview);
	SAFE_RELEASE(m_rendertargetview);
	SAFE_RELEASE(m_swapchain);
	SAFE_RELEASE(m_device);
};

void RenderSystem::Clear() {
	const float color[]={0.11f,0.12f,0.13f,1.0f};
	m_device->ClearRenderTargetView(m_rendertargetview,color);
	m_device->ClearDepthStencilView(m_depthstencilview,D3D10_CLEAR_DEPTH,1,0);
};

void RenderSystem::Present() {
	m_swapchain->Present(0,0);
};

bool RenderSystem::CreateShader(Shader *shader,const String &vertex_shader,const String &pixel_shader) {
	shader->m_vertex_shader=nullptr;
	shader->m_pixel_shader=nullptr;
	shader->m_input_layout=nullptr;
	shader->m_signature=nullptr;

	int vs_size=System::FileSize(vertex_shader);
	char *vs_buffer=new char[vs_size+1]; vs_buffer[vs_size]='\0';
	System::FileRead(vertex_shader,vs_buffer,vs_size);

	HRESULT hr=S_OK;
	ID3D10Blob *err_msg=nullptr,*vs=nullptr;

	hr=D3D10CompileShader(
		vs_buffer,vs_size,
		vertex_shader.c_str(),NULL,NULL,
		"main","vs_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		&vs,&err_msg);
	delete[] vs_buffer;

	if(hr!=S_OK) {
		System::Notify((char*)err_msg->GetBufferPointer(),"Vertex Shader Compilation Error!");
		System::Break();
		err_msg->Release();
		return false;
	};

	hr=m_device->CreateVertexShader(vs->GetBufferPointer(),vs->GetBufferSize(),&shader->m_vertex_shader);
	if(hr!=S_OK) {
		System::Notify("Could not create vertex shader!","Vertex Shader Creation Error!");
		System::Break();
		return false;
	};

	int ps_size=System::FileSize(pixel_shader);
	char *ps_buffer=new char[ps_size+1]; ps_buffer[ps_size]='\0';
	System::FileRead(pixel_shader,ps_buffer,ps_size);

	ID3D10Blob *ps=nullptr;
	hr=D3D10CompileShader(
		ps_buffer,ps_size,
		pixel_shader.c_str(),NULL,NULL,
		"main","ps_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		&ps,&err_msg);
	delete[] ps_buffer;

	if(hr!=S_OK) {
		System::Notify((char*)err_msg->GetBufferPointer(),"Pixel Shader Compilation Error!");
		System::Break();
		err_msg->Release();
		return false;
	};

	hr=m_device->CreatePixelShader(ps->GetBufferPointer(),ps->GetBufferSize(),&shader->m_pixel_shader);
	if(hr!=S_OK) {
		System::Notify("Could not create pixel shader!","Pixel Shader Creation Error!");
		System::Break();
		shader->m_vertex_shader->Release();
		err_msg->Release();
		return false;
	};

	D3D10GetInputSignatureBlob(vs->GetBufferPointer(),vs->GetBufferSize(),&shader->m_signature);
	if(vs) vs->Release();
	if(ps) ps->Release();

	return true;
};

bool RenderSystem::CreateTexture(Texture *texture,const String &filename) {
	const char *ext=strstr(filename.c_str()+3,".");
	if(strcmp(ext,".dds")==0) {
		return CreateDDSTextureFromFile(m_device,filename.c_str(),&texture->m_shader_resource_view)==S_OK;
	};

	Bitmap bitmap;
	if(!bitmap.Load(filename)) {return false;};

	D3D10_TEXTURE2D_DESC desc;
	desc.Width=bitmap.GetWidth();
	desc.Height=bitmap.GetHeight();
	desc.Format=formats[(int)bitmap.GetFormat()];
	desc.BindFlags=D3D10_BIND_SHADER_RESOURCE;
	desc.Usage=D3D10_USAGE_DEFAULT;
	desc.CPUAccessFlags=0;
	desc.MiscFlags=0;
	
	desc.MipLevels=1;
	desc.ArraySize=1;
	desc.SampleDesc.Count=1;
	desc.SampleDesc.Quality=0;

	D3D10_SUBRESOURCE_DATA data;
	data.pSysMem=bitmap.GetData();
	data.SysMemPitch=bitmap.GetWidth()*Bitmap::GetFormatSize(bitmap.GetFormat());
	data.SysMemSlicePitch=bitmap.GetWidth()*bitmap.GetHeight()*
		Bitmap::GetFormatSize(bitmap.GetFormat());

	HRESULT hr=m_device->CreateTexture2D(&desc,&data,(ID3D10Texture2D**)&texture->m_resource);
	if(hr!=S_OK) {
		System::Notify("Could not create 2d texture!","Create Texture2D Error!");
		System::Break();
		return false;
	};

	D3D10_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format=desc.Format;
	srv_desc.ViewDimension=D3D10_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels=1;
	srv_desc.Texture2D.MostDetailedMip=0;
	hr=m_device->CreateShaderResourceView(texture->m_resource,&srv_desc,
		&texture->m_shader_resource_view);
	if(hr!=S_OK) {
		System::Notify("Could not create shader resource view!",
			"Create Shader Resource View Error!");
		System::Break();
		return false;
	};

	return true;
};

bool RenderSystem::CreateTexture(Texture *texture,EAccessMode access,EBmpFmt format,int width,int height,int depth) {
	D3D10_TEXTURE2D_DESC desc;
	desc.Width=width;
	desc.Height=width;
	desc.Format=formats[(int)format];
	desc.BindFlags=D3D10_BIND_SHADER_RESOURCE;
	desc.Usage=usages[(int)access];
	desc.CPUAccessFlags=0;
	desc.MiscFlags=0;
	desc.SampleDesc.Count=1;
	desc.SampleDesc.Quality=0;
	desc.MipLevels=1;
	desc.ArraySize=1;

	HRESULT hr=m_device->CreateTexture2D(&desc,NULL,(ID3D10Texture2D**)&texture->m_resource);
	if(hr!=S_OK) {
		System::Notify("Could not create 2d texture!","Create Texture2D Error!");
		System::Break();
		return false;
	};

	D3D10_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format=desc.Format;
	srv_desc.ViewDimension=D3D10_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels=1;
	srv_desc.Texture2D.MostDetailedMip=0;
	hr=m_device->CreateShaderResourceView(texture->m_resource,&srv_desc,
		&texture->m_shader_resource_view);
	if(hr!=S_OK) {
		System::Notify("Could not create shader resource view!",
			"Create Shader Resource View Error!");
		System::Break();
		return false;
	};

	return true;
};

bool RenderSystem::CreateIndexBuffer(IndexBuffer *index_buffer,int count,int size,void *buffer) {
	index_buffer->m_count=count;
	index_buffer->m_size=size;

	D3D10_BUFFER_DESC desc;
	desc.Usage=D3D10_USAGE_DEFAULT;
	desc.ByteWidth=count*size;
	desc.BindFlags=D3D10_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags=0;
	desc.MiscFlags=0;

	D3D10_SUBRESOURCE_DATA data;
	data.pSysMem=buffer;
	data.SysMemPitch=0;
	data.SysMemSlicePitch=0;

	HRESULT hr=m_device->CreateBuffer(&desc,&data,&index_buffer->m_buffer);
	if(hr!=S_OK) {
		System::Notify("Could not create index buffer!","Create Index Buffer Error!");
		System::Break();
		return false;
	};

	return true;
};

bool RenderSystem::CreateVertexBuffer(VertexBuffer *vertex_buffer,EAccessMode access,int count,int size,void *buffer) {
	static D3D10_USAGE usage[]={D3D10_USAGE_DEFAULT,D3D10_USAGE_DYNAMIC,D3D10_USAGE_IMMUTABLE};
	vertex_buffer->m_size=size;
	vertex_buffer->m_count=count;

	D3D10_BUFFER_DESC desc={0};
	desc.Usage=usage[access];
	desc.ByteWidth=count*size;
	desc.BindFlags=D3D10_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags=access==BM_DYNAMIC?D3D10_CPU_ACCESS_WRITE:0;
	desc.MiscFlags=0;

	D3D10_SUBRESOURCE_DATA data;
	data.pSysMem=buffer;
	data.SysMemPitch=0;
	data.SysMemSlicePitch=0;

	HRESULT hr=m_device->CreateBuffer(&desc,&data,&vertex_buffer->m_buffer);
	if(hr!=S_OK) {
		System::Notify("Could not create vertex buffer!","Create Buffer Error!");
		System::Break();
		return false;
	};

	// calculate radius
	Vector3 o(0.0f,0.0f,0.0f);
	char *ptr=static_cast<char*>(buffer);
	float *values=reinterpret_cast<float*>(ptr);
	for(int i=0;i<count;i++) {
		float x=Math::Abs(*(values+0)),y=Math::Abs(*(values+1)),z=Math::Abs(*(values+2));
		if(x>o.x) {o.x=x;};
		if(y>o.y) {o.y=y;};
		if(z>o.z) {o.z=z;};
		ptr+=size;
		values=reinterpret_cast<float*>(ptr);
	};
	vertex_buffer->m_radius=Math::Max(o.x,Math::Max(o.y,o.z));

	return true;
};

bool RenderSystem::CreateInputLayout(Shader *shader,D3D10_INPUT_ELEMENT_DESC *desc,int count) {
	HRESULT hr=m_device->CreateInputLayout(desc,count,
		shader->m_signature->GetBufferPointer(),
		shader->m_signature->GetBufferSize(),
		&shader->m_input_layout);
	if(hr!=S_OK) {
		System::Notify("Could not create input layout!","Input Layout Error!");
		System::Break();
		return false;
	};
	shader->m_signature->Release();
	shader->m_signature=nullptr;

	return true;
};

bool RenderSystem::CreateConstantBuffers(Shader *shader) {
	for(int i=0;i<shader->m_num_vs_constants;i++) {
		D3D10_BUFFER_DESC desc;
		desc.Usage=D3D10_USAGE_DEFAULT;
		desc.ByteWidth=shader->m_vs_constants[i].m_size;
		desc.BindFlags=D3D10_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags=0;
		desc.MiscFlags=0;

		HRESULT hr=m_device->CreateBuffer(&desc,NULL,&shader->m_vs_constants[i].m_buffer);
		if(hr!=S_OK) {
			System::Notify("Could not create constant buffer!","Create Constant Buffer Error!");
			System::Break();
			return false;
		};
	};

	for(int i=0;i<shader->m_num_ps_constants;i++) {
		D3D10_BUFFER_DESC desc;
		desc.Usage=D3D10_USAGE_DEFAULT;
		desc.ByteWidth=shader->m_ps_constants[i].m_size;
		desc.BindFlags=D3D10_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags=0;
		desc.MiscFlags=0;

		HRESULT hr=m_device->CreateBuffer(&desc,NULL,&shader->m_ps_constants[i].m_buffer);
		if(hr!=S_OK) {
			System::Notify("Could not create constant buffer!","Create Constant Buffer Error!");
			System::Break();
			return false;
		};
	};

	return true;
};

bool RenderSystem::CreateSamplerState(Sampler *sampler,
		EAddressMode addr_u,EAddressMode addr_v,EAddressMode addr_w,
		EFilterMode filter,ECompareMode compare) 
{
	static D3D10_FILTER filters[]={D3D10_FILTER_MIN_MAG_MIP_POINT,D3D10_FILTER_MIN_MAG_MIP_LINEAR};
	static D3D10_TEXTURE_ADDRESS_MODE address[]={D3D10_TEXTURE_ADDRESS_WRAP,D3D10_TEXTURE_ADDRESS_CLAMP,D3D10_TEXTURE_ADDRESS_MIRROR};

	D3D10_SAMPLER_DESC desc;
	desc.Filter=filters[filter];
	desc.AddressU=address[addr_u];
	desc.AddressV=address[addr_v];
	desc.AddressW=address[addr_w];
	desc.MipLODBias=0.0f;
	desc.MaxAnisotropy=1;
	desc.ComparisonFunc=(D3D10_COMPARISON_FUNC)compare;
	desc.BorderColor[0]=0;
	desc.BorderColor[1]=0;
	desc.BorderColor[2]=0;
	desc.BorderColor[3]=0;
	desc.MinLOD=0;
	desc.MaxLOD=D3D10_FLOAT32_MAX;

	HRESULT hr=m_device->CreateSamplerState(&desc,&sampler->m_state);
	if(hr!=S_OK) {
		System::Notify("Could not create sampler state!",
			"Create Sampler State Error!");
		System::Break();
		return false;
	};

	return true;
};

bool RenderSystem::CreateDepthState(DepthState *state,bool depth_test,bool depth_write,ECompareMode depth_func,
	bool stencil_test,ECompareMode stencil_func_front,ECompareMode stencil_func_back,
	EStencilOp stencil_fail_front,EStencilOp stencil_fail_back,
	EStencilOp depth_fail_front,EStencilOp depth_fail_back,
	EStencilOp stencil_pass_front,EStencilOp stencil_pass_back) 
{
	D3D10_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable=depth_test;
	desc.DepthFunc=(D3D10_COMPARISON_FUNC)depth_func;
	desc.DepthWriteMask=depth_write?D3D10_DEPTH_WRITE_MASK_ALL:D3D10_DEPTH_WRITE_MASK_ZERO;
	desc.StencilEnable=stencil_test;
	desc.StencilReadMask=D3D10_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask=D3D10_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilFunc=(D3D10_COMPARISON_FUNC)stencil_func_front;
	desc.FrontFace.StencilDepthFailOp=(D3D10_STENCIL_OP)depth_fail_front;
	desc.FrontFace.StencilFailOp=(D3D10_STENCIL_OP)stencil_fail_front;
	desc.FrontFace.StencilPassOp=(D3D10_STENCIL_OP)stencil_pass_front;
	desc.BackFace.StencilFunc=(D3D10_COMPARISON_FUNC)stencil_func_back;
	desc.BackFace.StencilDepthFailOp=(D3D10_STENCIL_OP)depth_fail_back;
	desc.BackFace.StencilFailOp=(D3D10_STENCIL_OP)stencil_fail_back;
	desc.BackFace.StencilPassOp=(D3D10_STENCIL_OP)stencil_pass_back;

	HRESULT hr=m_device->CreateDepthStencilState(&desc,&state->m_state);
	if(hr!=S_OK) {
		System::Notify("Could not create depthstencil state!","Create Depth Stencil State Error!");
		System::Break();
		return false;
	};

	return true;
};

bool RenderSystem::CreateBlendState(BlendState *state,
	EBlendType src_rgb,EBlendType dst_rgb,EBlendType src_alpha,EBlendType dst_alpha,
	EBlendMode blend_rgb,EBlendMode blend_alpha,unsigned char mask) 
{
	bool enable=src_rgb!=BT_ONE||dst_rgb!=BT_ZERO||src_alpha!=BT_ONE||dst_alpha!=BT_ZERO;

	D3D10_BLEND_DESC desc;
	::memset(&desc.BlendEnable,0,sizeof(desc.BlendEnable));
	::memset(&desc.RenderTargetWriteMask,0,sizeof(desc.RenderTargetWriteMask));
	desc.AlphaToCoverageEnable=false;
	desc.BlendEnable[0]=enable;
	desc.BlendOp=(D3D10_BLEND_OP)(blend_rgb+1);
	desc.BlendOpAlpha=(D3D10_BLEND_OP)(blend_alpha+1);
	desc.SrcBlend=(D3D10_BLEND)(src_rgb+1);
	desc.DestBlend=(D3D10_BLEND)(dst_rgb+1);
	desc.SrcBlendAlpha=(D3D10_BLEND)(src_alpha+1);
	desc.DestBlendAlpha=(D3D10_BLEND)(dst_alpha+1);
	desc.RenderTargetWriteMask[0]=mask;

	HRESULT hr=m_device->CreateBlendState(&desc,&state->m_state);
	if(hr!=S_OK) {
		System::Notify("Could not create blend state!","Create Blend State Error!");
		System::Break();
		return false;
	};

	return true;
};

bool RenderSystem::CreateRasterizerState(RasterizerState *state,ECullMode cull,EFillMode fill) {
	static D3D10_CULL_MODE culls[]={D3D10_CULL_NONE,D3D10_CULL_BACK,D3D10_CULL_FRONT};
	static D3D10_FILL_MODE fills[]={D3D10_FILL_WIREFRAME,D3D10_FILL_SOLID};

	D3D10_RASTERIZER_DESC desc;
	desc.CullMode=culls[cull];
	desc.FillMode=fills[fill];
	desc.FrontCounterClockwise=false;
	desc.DepthBias=0;
	desc.DepthBiasClamp=0.0f;
	desc.SlopeScaledDepthBias=0.0f;
	desc.DepthClipEnable=true;
	desc.ScissorEnable=false;
	desc.MultisampleEnable=false;
	desc.AntialiasedLineEnable=false;

	HRESULT hr=m_device->CreateRasterizerState(&desc,&state->m_state);
	if(hr!=S_OK) {
		System::Notify("Could not create rasterizer state!","Create Rasterizer State Error!");
		System::Break();
		return false;
	};

	return true;
};

void RenderSystem::SelectShader(Shader *shader) {
	m_selected.m_shader=shader;
};

void RenderSystem::SelectTexture(Texture *texture,unsigned int slot) {
	m_selected.m_texture[slot]=texture;
};

void RenderSystem::SelectIndexBuffer(IndexBuffer *buffer) {
	m_selected.m_index_buffer=buffer;
};

void RenderSystem::SelectVertexBuffer(VertexBuffer *buffer,unsigned int slot) {
	m_selected.m_vertex_buffer[slot]=buffer;
};

void RenderSystem::SelectDepthState(DepthState *state) {
	m_selected.m_depth_state=state;
};

void RenderSystem::SelectBlendState(BlendState *state) {
	m_selected.m_blend_state=state;
};

void RenderSystem::SelectRasterizerState(RasterizerState *state) {
	m_selected.m_rasterizer_state=state;
};

void RenderSystem::Apply() {
	ApplyShader();
	ApplyConstants();
	ApplySamplers();
	ApplyTextures();
	ApplyIndexBuffer();
	ApplyVertexBuffers();
	ApplyDepthState();
	ApplyBlendState();
	ApplyRasterizerState();
};

static const D3D10_PRIMITIVE_TOPOLOGY prims[]={
	D3D10_PRIMITIVE_TOPOLOGY_POINTLIST,
	D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
};

void RenderSystem::Draw(EPrimitive prim,int start,int count) {
	m_device->IASetPrimitiveTopology(prims[prim]);
	m_device->Draw(count,start);
};

void RenderSystem::DrawIndexed(EPrimitive prim,int index_start,int vertex_start,int count) {
	m_device->IASetPrimitiveTopology(prims[prim]);
	m_device->DrawIndexed(count,index_start,vertex_start);
};

void RenderSystem::UpdateConstantBuffers(Shader *shader) {
	for(int i=0;i<shader->m_num_vs_constants;i++) {
		if(!shader->m_vs_dirty[i]) {continue;};
		shader->m_vs_dirty[i]=false;
		Constant *constant=&shader->m_vs_constants[i];
		m_device->UpdateSubresource(constant->m_buffer,0,NULL,constant->m_data,0,0);
	};
	for(int i=0;i<shader->m_num_ps_constants;i++) {
		if(!shader->m_ps_dirty[i]) {continue;};
		shader->m_ps_dirty[i]=false;
		Constant *constant=&shader->m_ps_constants[i];
		m_device->UpdateSubresource(constant->m_buffer,0,NULL,constant->m_data,0,0);
	};
};

void RenderSystem::UpdateTexture(Texture *texture,class Bitmap *bitmap) {
	unsigned int pitch=bitmap->GetWidth()*Bitmap::GetFormatSize(bitmap->GetFormat());
	unsigned int size=pitch*bitmap->GetHeight();
	m_device->UpdateSubresource(texture->m_resource,0,NULL,bitmap->GetData(),pitch,size);
};

void RenderSystem::UpdateVertexBuffer(VertexBuffer *buffer,int size,void *data) {
	char *dst=nullptr;
	buffer->m_buffer->Map(D3D10_MAP_WRITE_DISCARD,0,(void**)&dst);
	::memcpy(dst,data,size);
	buffer->m_buffer->Unmap();
};


// private
void RenderSystem::ApplyShader() {
	if(m_current.m_shader==m_selected.m_shader) {return;};

	m_current.m_shader=m_selected.m_shader;

	m_device->VSSetShader(m_current.m_shader->m_vertex_shader);
	m_device->PSSetShader(m_current.m_shader->m_pixel_shader);
	m_device->IASetInputLayout(m_current.m_shader->m_input_layout);
};

void RenderSystem::ApplyConstants() {
	if(m_current.m_shader==nullptr) {return;};

	UpdateConstantBuffers(m_current.m_shader);

	static ID3D10Buffer *array[]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

	if(m_current.m_shader->m_num_vs_constants>0) {
		for(int i=0;i<m_current.m_shader->m_num_vs_constants;i++) {
			array[i]=m_current.m_shader->m_vs_constants[i].m_buffer;
		};
		m_device->VSSetConstantBuffers(0,m_current.m_shader->m_num_vs_constants,array);
	};

	if(m_current.m_shader->m_num_ps_constants>0) {
		for(int i=0;i<m_current.m_shader->m_num_ps_constants;i++) {
			array[i]=m_current.m_shader->m_ps_constants[i].m_buffer;
		};
		m_device->PSSetConstantBuffers(0,m_current.m_shader->m_num_ps_constants,array);
	};
};

void RenderSystem::ApplySamplers() {
	if(m_current.m_shader==nullptr) {return;};

	static ID3D10SamplerState *ss_array[]={NULL,NULL,NULL,NULL};
	if(m_current.m_shader->m_num_samplers>0) {
		for(int i=0;i<m_current.m_shader->m_num_samplers;i++) {
			ss_array[i]=m_current.m_shader->m_samplers[i].m_state;
		};
		m_device->PSSetSamplers(0,m_current.m_shader->m_num_samplers,ss_array);
	};
};

void RenderSystem::ApplyTextures() {
	int i=0,c=0;
	for(i=0;i<8;i++) {
		if(m_selected.m_texture[i]!=m_current.m_texture[i]) {
			break;
		};
	};
	if(i>=8) {return;};

	static ID3D10ShaderResourceView *array[]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	for(i=0;i<8;i++) {
		m_current.m_texture[i]=m_selected.m_texture[i];
		if(m_current.m_texture[i]!=nullptr) {
			array[i]=m_current.m_texture[i]->m_shader_resource_view; 
			c++;
		};
	};
	m_device->PSSetShaderResources(0,c,array);
};


void RenderSystem::ApplyIndexBuffer() {
	if(m_selected.m_index_buffer==m_current.m_index_buffer) {return;};
	m_current.m_index_buffer=m_selected.m_index_buffer;
	if(m_current.m_index_buffer==nullptr) {
		m_device->IASetIndexBuffer(NULL,DXGI_FORMAT_UNKNOWN,0);
		return;
	};

	m_device->IASetIndexBuffer(
		m_current.m_index_buffer->m_buffer,
		m_current.m_index_buffer->m_size==2?DXGI_FORMAT_R16_UINT:DXGI_FORMAT_R32_UINT,
		0);
};

void RenderSystem::ApplyVertexBuffers() {
	int i=0,c=0;
	for(int i=0;i<4;i++) {
		if(m_current.m_vertex_buffer[i]!=m_selected.m_vertex_buffer[i]) {break;};
	};
	if(i>=4) {return;};

	static ID3D10Buffer *array[]={NULL,NULL,NULL,NULL};
	static unsigned int stride[]={0,0,0,0};
	static unsigned int offset[]={0,0,0,0};
	for(int i=0;i<4;i++) {
		m_current.m_vertex_buffer[i]=m_selected.m_vertex_buffer[i];
		if(m_current.m_vertex_buffer[i]) {
			array[i]=m_current.m_vertex_buffer[i]->m_buffer;
			stride[i]=m_current.m_vertex_buffer[i]->m_size;
			c++;
		};
	};
	m_device->IASetVertexBuffers(0,c,array,stride,offset);
};

void RenderSystem::ApplyDepthState() {
	if(m_current.m_depth_state==m_selected.m_depth_state) {return;};
	m_current.m_depth_state=m_selected.m_depth_state;
	if(m_current.m_depth_state==nullptr) {
		m_device->OMSetDepthStencilState(nullptr,0);
		return;
	};
	m_device->OMSetDepthStencilState(m_current.m_depth_state->m_state,0);
};

void RenderSystem::ApplyBlendState() {
	if(m_current.m_blend_state==m_selected.m_blend_state) {return;};
	m_current.m_blend_state=m_selected.m_blend_state;
	if(m_current.m_blend_state==nullptr) {
		m_device->OMSetBlendState(nullptr,nullptr,0xffffffff);
		return;
	};

	const float factor[]={0.0f,0.0f,0.0f,0.0f};
	m_device->OMSetBlendState(m_current.m_blend_state->m_state,factor,0xffffffff);
};

void RenderSystem::ApplyRasterizerState() {
	if(m_current.m_rasterizer_state==m_selected.m_rasterizer_state) {return;};
	m_current.m_rasterizer_state=m_selected.m_rasterizer_state;
	if(m_current.m_rasterizer_state==nullptr) {
		m_device->RSSetState(nullptr);
	};

	m_device->RSSetState(m_current.m_rasterizer_state->m_state);
};
