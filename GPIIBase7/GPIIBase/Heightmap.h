// Heightmap.h

#ifndef HEIGHTMAP_H_INCLUDED
#define HEIGHTMAP_H_INCLUDED

class Heightmap {
	struct Vertex {
		Vector3 position;
		Vector2 texcoord;
		Vector3 normal;
	};
public:
	Heightmap();

	bool Create(class RenderSystem *render_system,const String &filename);
	void Destroy();

	void Render(class RenderSystem *render_system,OncePerFrame *frame,DirectionalLight *light);

private:
	Shader m_shader;
	Texture m_texture;
	DepthState m_depth;
	RasterizerState m_rasterizer;
	Renderable m_renderable;
};

#endif // HEIGHTMAP_H_INCLUDED
