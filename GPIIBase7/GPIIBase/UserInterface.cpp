#include "stdafx.h"
#include "UserInterface.h"
#include "RenderSystem.h"
#include "MessageSystem.h"

namespace Gui{
		Widget::Widget(EWidgetType type)
		{
			m_type = type;
			m_x=m_y=0.0f;

		};
		EWidgetType Widget::GetType() const{return m_type;};
		float Widget::GetX() const{return m_x;};
		float Widget::GetY() const{return m_y;};
		float Widget::GetWidth() const{return m_width;};
		float Widget::GetHeight() const{return m_height;};

		void Widget::SetX(float x){m_dirty = true; m_x=x;};
		void Widget::setY(float y){m_dirty = true; m_y = y;};
		void Widget::SetWidth(float width){m_width = width; m_dirty = true;};
		void Widget::setHeight(float height){m_dirty = true; m_height = height;};

		bool Widget::IsVisible() const{return m_visible;};
		bool Widget::IsEnabled() const{return m_enabled;};
		bool Widget::IsDirty() const{return m_dirty;};
		void Widget::Clean(){m_dirty = false;};

		bool Widget::IsInside(int x, int y){
			if (x > m_x && x < m_width + m_x)
			{
				if (y > m_y && y < m_height + m_y)
				{
					return true;
				}
			}
			return false;	
		};

		Button::Button() : Widget(WT_BUTTON){
			m_color=0xffffffff;
		};

		bool Button::IsInside(int x, int y)
		{
			if (x > m_x && x < m_width + m_x)
			{
				if (y > m_y && y < m_height + m_y)
				{
					return true;
				}
			}
			return false;
		};

		unsigned int Button::GetColor() const {return m_color;};
		void Button::SetColor(unsigned int color){m_dirty=true; m_color = color;};
		String Button::GetText() const {return m_text;};
		void Button::SetText(const String &text){ m_dirty = true; m_text = text;};
};

GuiManager::GuiManager(RenderSystem *render_system, MessageSystem *message_system){
	m_render_system =render_system;
	m_message_system = message_system;
};

GuiManager::~GuiManager(){};

bool GuiManager::Init(int height, int width, unsigned int max_vertices){
	m_message_system->Register(EVT_MOUSEMOVE, this, &GuiManager::OnMouseMove);
	m_message_system->Register(EVT_MOUSEBUTTON, this, &GuiManager::OnMouseButton);

	//create shader
	m_render_system->CreateShader(&m_gui.m_shader, "../data/shader/gui_vs.hlsl", "../data/shader/gui_ps.hlsl");
	int vss[]={sizeof(OncePerFrame)};
	m_gui.m_shader.AllocateVSConstants(1,vss);

	OncePerFrame frame;
	frame.projection=DirectX::XMMatrixOrthographicLH((float)width, (float)height, 0.0f, 1.0f);
	frame.view=DirectX::XMMatrixIdentity();
	m_gui.m_shader.SetVSConstant(0,sizeof(OncePerFrame), &frame);
	m_render_system->CreateConstantBuffers(&m_gui.m_shader);

	D3D10_INPUT_ELEMENT_DESC desc[]={
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D10_INPUT_PER_VERTEX_DATA,0}
	};
	m_render_system->CreateInputLayout(&m_gui.m_shader, desc, sizeof(desc)/sizeof(desc[0]));

	m_gui.m_max_vertices=max_vertices*4;
	unsigned short *indices=new unsigned short[max_vertices*6];
	for (unsigned int i = 0, index=0; i < max_vertices*6; i+=6)
	{
		indices[i+0]=index+0;
		indices[i+1]=index+1;
		indices[i+2]=index+2;

		indices[i+3]=index+2;
		indices[i+4]=index+3;
		indices[i+5]=index+0;
		index+=4;
	};

	m_gui.m_renderable.m_count=0;
	m_render_system->CreateIndexBuffer(
		&m_gui.m_renderable.m_index_buffer,max_vertices*6, sizeof(unsigned short), indices);
	delete[] indices;

	m_gui.m_vertices=new Gui::Vertex[m_gui.m_max_vertices];
	m_render_system->CreateVertexBuffer(
		&m_gui.m_renderable.m_vertex_buffer, BM_DYNAMIC, m_gui.m_max_vertices, sizeof(Gui::Vertex), m_gui.m_vertices);

	return true;
};

void GuiManager::Shut(){
	delete[] m_gui.m_vertices;
	m_message_system->Unregister(EVT_MOUSEMOVE, this);
	m_message_system->Unregister(EVT_MOUSEBUTTON, this);
};

void GuiManager::Update(){
	unsigned int k = 0;
	for(unsigned int i=0; i<m_widgets.size(); i++)
	{
		if(m_widgets[i]->IsDirty()){
			k=m_widgets.size()+1;
			break;
		};
		k++;
	};

	if (k>m_widgets.size()){return;};

	unsigned int ic=0;
	for(unsigned int i = 0; i<m_widgets.size(); i++)
	{
		m_widgets[i]->Clean();
		switch(m_widgets[i]->GetType()){
		case Gui::WT_BUTTON:
			FillButton(m_widgets[i],m_gui.m_vertices,m_gui.m_max_vertices, ic);
			break;
		};
	};
	m_gui.m_renderable.m_count=ic/4*6;

	m_render_system->UpdateVertexBuffer(&m_gui.m_renderable.m_vertex_buffer,ic*sizeof(Gui::Vertex), m_gui.m_vertices);
};
void GuiManager::Render(){
	m_render_system->SelectShader(&m_gui.m_shader);
	m_render_system->SelectIndexBuffer(&m_gui.m_renderable.m_index_buffer);
	m_render_system->SelectVertexBuffer(&m_gui.m_renderable.m_vertex_buffer, 0);
	m_render_system->Apply();
	m_render_system->DrawIndexed(PRIM_TRIANGLE_LIST,0,0,m_gui.m_renderable.m_count);
};

void GuiManager::AttachWidget(Gui::Widget *widget){
	m_widgets.push_back(widget);
};
void GuiManager::DetachWidget(Gui::Widget *widget){};

void GuiManager::OnMouseMove(IEvent *event){
	MouseMoveEvent *evt=static_cast<MouseMoveEvent*>(event);
};

void GuiManager::OnMouseButton(IEvent *event){
	MouseButtonEvent *evt=static_cast<MouseButtonEvent*>(event);
};

static void draw_rect(float x, float y, float z, float w, float h, unsigned int color, Gui::Vertex *vertices, unsigned int size, unsigned int &pos){
	unsigned int i=0;
	vertices[pos+i].color = color;
	vertices[pos+i].position.x=x;
	vertices[pos+i].position.y=y;
	vertices[pos+i].position.z=z;
	i++;

	vertices[pos+i].color = color;
	vertices[pos+i].position.x=x+w;
	vertices[pos+i].position.y=y;
	vertices[pos+i].position.z=z;
	i++;

	vertices[pos+i].color = color;
	vertices[pos+i].position.x=x+w;
	vertices[pos+i].position.y=y+h;
	vertices[pos+i].position.z=z;
	i++;

	vertices[pos+i].color = color;
	vertices[pos+i].position.x=x;
	vertices[pos+i].position.y=y+h;
	vertices[pos+i].position.z=z;
	i++;
};

void GuiManager::FillButton(Gui::Widget *widget, Gui::Vertex *vertices, unsigned int size, unsigned int &pos){
	Gui::Button *button=static_cast<Gui::Button*>(widget);
	float x,y,z,w,h;
	x=button->GetX();
		y=button->GetY();
		z=0.999f;
		w=button->GetWidth();
		h=button->GetHeight();
	draw_rect(x, y, z, w, h, 0xffff00ff, vertices, size, pos);
	draw_rect(x-1, y-1,z,w-2,h-2,0xffffffff,vertices,size,pos);
};