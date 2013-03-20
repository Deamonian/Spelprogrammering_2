// Application.cpp

#include "stdafx.h"
#include "MessageSystem.h"
#include "RenderSystem.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "Skydome.h"
#include "Heightmap.h"
#include "CameraControl.h"
#include "Application.h"

using namespace DirectX;

Application::Application() {
	m_message_system=nullptr;
	m_render_system=nullptr;
	m_shader_manager=nullptr;
	m_texture_manager=nullptr;
	m_scene_manager=nullptr;
};

Application::~Application() {
};

bool Application::Init(MessageSystem *message_system,HWND window_handle,ConfigFile &cfg) {
	float width=cfg.HasKey("width")?cfg.AsFloat("width"):1280;
	float height=cfg.HasKey("height")?cfg.AsFloat("height"):720;

	m_message_system=message_system;
	m_render_system=new RenderSystem;
	if(!m_render_system->Init((int)width,(int)height,window_handle)) {
		System::Notify("Could not initialize RenderSystem!\n"\
			"Read log.txt for more information.","Error!");
		return false;
	};

	m_shader_manager=new ShaderManager(m_render_system);
	m_texture_manager=new TextureManager(m_render_system);
	m_scene_manager=new SceneManager(m_shader_manager,m_texture_manager);
	m_scene_manager->Init();

	// set up camera
	m_camera.SetPerspective(width/height,XM_PIDIV4,cfg.AsFloat("c_znear"),cfg.AsFloat("c_zfar"));
	m_camera_control=new CameraControl(&m_camera);
	m_camera_control->Init(m_message_system);
	m_camera_control->Settings(cfg.AsFloat("c_speed"),cfg.AsFloat("m_yaw"),cfg.AsFloat("m_pitch"),cfg.AsFloat("m_sensitivity"));

	// skydome
	Skydome *skydome=new Skydome;
	skydome->Init(m_render_system);
	m_scene_manager->SetSkydome(skydome);

	// heightmap
	Heightmap *heightmap=new Heightmap;
	heightmap->Create(m_render_system,"../data/texture/heightmap.tga");
	m_scene_manager->SetHeightmap(heightmap);

	return true;
};

void Application::Shut() {
	if(m_camera_control) {
		m_camera_control->Shut(m_message_system);
		delete m_camera_control;
		m_camera_control=nullptr;
	};

	if(m_texture_manager) {
		delete m_texture_manager;
		m_texture_manager=nullptr;
	};

	if(m_shader_manager) {
		delete m_shader_manager;
		m_shader_manager=nullptr;
	};

	if(m_scene_manager) {
		m_scene_manager->Shut();
		delete m_scene_manager;
		m_scene_manager=nullptr;
	};

	if(m_render_system) {
		m_render_system->Shut();
		delete m_render_system;
		m_render_system=nullptr;
	};
};

void Application::Tick() {
	m_timer.Update();
	m_camera_control->Update(m_timer.GetDelta());

	m_render_system->Clear();
	m_scene_manager->Render(m_render_system,&m_camera);
	m_render_system->Present();
};

// protected:

