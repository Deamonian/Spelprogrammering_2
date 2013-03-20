// Skybox.h

#ifndef SKYBOX_H_INCLUDED
#define SKYBOX_H_INCLUDED

class Skybox {
	struct OncePerFrame {
		Mat4 proj;
		Mat4 view;
	};

	struct Vertex {
		Vector3 position;
		unsigned int color;
	};

public:
	Skybox();
	~Skybox();

	void Init(class RenderSystem *render_system);
	void Render(class RenderSystem *render_system,class Camera *camera);

private:
	void Shut();

private:
	Shader m_shader;
	DepthState m_depth_state;
	Renderable m_renderable;
};

#endif // SKYBOX_H_INCLUDED
