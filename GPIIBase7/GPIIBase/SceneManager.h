// SceneManager.h

#ifndef SCENEMANAGER_H_INCLUDED
#define SCENEMANAGER_H_INCLUDED

#include "SceneManagerPrereq.h"

class Camera;
class RenderSystem;
class ShaderManager;
class TextureManager;
class Skydome;
class Heightmap;

class SceneManager {
public:
	SceneManager(ShaderManager *shader_manager,TextureManager *texture_manager);
	~SceneManager();

	bool Init(int pre_alloc_size=1024);
	void Shut();

	void Render(RenderSystem *render_system,Camera *camera);

	SceneObject *Create(Shader *shader,Texture *texture,Renderable *renderable);
	void Destroy(SceneObject *object);

	void SetSkydome(Skydome *skydome);
	void SetHeightmap(Heightmap *heightmap);
	void SetLightDirection(const Vector3 &direction);
	void SetLightDiffuse(float r,float g,float b);
	void SetLightAmbient(float r,float g,float b);
	void SetLightIntensity(float intensity);

private:
	void CullScene(Camera *camera);
	void SortScene();
	void RenderScene(RenderSystem *render_system);

private:
	OncePerFrame *m_frame;
	OncePerObject *m_object;
	DirectionalLight m_light;
	Frustum m_frustum;

	ShaderManager *m_shader_manager;
	TextureManager *m_texture_manager;

	class Skydome *m_skydome;
	class Heightmap *m_heightmap;
	Allocator<SceneObject> m_allocator;
	std::vector<SceneObject*> m_scene;
	std::vector<unsigned int> m_visible_scene;
	std::vector<unsigned int> m_sorted_scene;
};

#endif // SCENEMANAGER_H_INCLUDED
