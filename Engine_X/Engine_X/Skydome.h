// Skydome.h

#ifndef SKYDOME_H_INCLUDED
#define SKYDOME_H_INCLUDED

class Skydome {
	struct OncePerFrame {Mat4 proj; Mat4 view;};
	struct Vertex {Vector3 position;};
public:
	void Init(class RenderSystem *render_system);
	void Shut();

	void Render(class RenderSystem *render_system,class Camera *camera);

private:
	void Subdivide(Vector3& v0, Vector3& v1, Vector3& v2, int depth);
	std::vector<Vector3> mSubdivision;

private:
	Shader mShader;
	Texture mTexture;
	DepthState mDepthState;
	Renderable mRenderable;
};

#endif // SKYDOME_H_INCLUDED
