// Application.h

#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include "Camera.h"

class MessageSystem;
class RenderSystem;
class ShaderManager;
class TextureManager;
class SceneManager;

class Application {
public:
	Application();
	~Application();

	bool Init(MessageSystem *message_system,HWND window_handle,ConfigFile &cfg);
	void Shut();

	void Tick();

protected:
	MessageSystem *m_message_system;
	RenderSystem *m_render_system;
	ShaderManager *m_shader_manager;
	TextureManager *m_texture_manager;
	SceneManager *m_scene_manager;

	Timer m_timer;
	Camera m_camera;
	class CameraControl *m_camera_control;
}; 

#endif // APPLICATION_H_INCLUDED
