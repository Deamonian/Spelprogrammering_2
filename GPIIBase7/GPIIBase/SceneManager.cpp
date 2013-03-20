// SceneManager.cpp

#include "stdafx.h"
#include "Skydome.h"
#include "Skybox.h"
#include "Heightmap.h"
#include "Camera.h"
#include "RenderSystem.h"
#include "SceneManager.h"

SceneManager::SceneManager(ShaderManager *shader_manager,TextureManager *texture_manager) 
: m_shader_manager(shader_manager)
, m_texture_manager(texture_manager)
, m_allocator(1024)
{
	m_skydome=nullptr;
	m_heightmap=nullptr;
	m_frame=new (_aligned_malloc(sizeof(OncePerFrame),16)) OncePerFrame;
	m_object=new (_aligned_malloc(sizeof(OncePerObject),16)) OncePerObject;
};

SceneManager::~SceneManager() {
	_aligned_free(m_frame);
	_aligned_free(m_object);
};

bool SceneManager::Init(int pre_alloc_size) {
	m_scene.reserve(pre_alloc_size);
	m_visible_scene.reserve(pre_alloc_size);
	m_sorted_scene.reserve(pre_alloc_size);

	return true;
};

void SceneManager::Shut() {
	// clean up this mess
	if(m_skydome) {
		m_skydome->Shut();
		delete m_skydome;
		m_skydome=nullptr;
	};
	if(m_heightmap) {
		m_heightmap->Destroy();
		delete m_heightmap;
		m_heightmap=nullptr;
	};
};

void SceneManager::Render(RenderSystem *render_system,Camera *camera) {
	// cull objects against frustum
	CullScene(camera);
	SortScene();
	if(m_skydome) {m_skydome->Render(render_system,camera);};
	if(m_heightmap) {m_heightmap->Render(render_system,m_frame,&m_light);};
	RenderScene(render_system);
};

SceneObject *SceneManager::Create(Shader *shader,Texture *texture,Renderable *renderable) {
	SceneObject *object=m_allocator.Allocate();
	object->m_renderable=renderable;
	object->m_material.m_shader=shader;
	object->m_material.m_texture=texture;
	object->m_bounding_volume.m_radius=renderable->m_vertex_buffer.m_radius;
	m_scene.push_back(object);
	return object;
};

void SceneManager::Destroy(SceneObject *object) {
	std::vector<SceneObject*>::iterator it=m_scene.begin();
	while(it!=m_scene.end()) {
		if((*it)==object) {
			m_allocator.Free(object);
			m_scene.erase(it);
			break;
		};
		++it;
	};
};

void SceneManager::SetSkydome(Skydome *skydome) {
	m_skydome=skydome;
};

void SceneManager::SetHeightmap(Heightmap *heightmap) {
	m_heightmap=heightmap;
};

void SceneManager::SetLightDirection(const Vector3 &direction) {
	m_light.direction=direction;
	m_light.direction.Normalize();
};

void SceneManager::SetLightDiffuse(float r,float g,float b) {
	m_light.diffuse.r=r; m_light.diffuse.g=g; m_light.diffuse.b=b; 
};

void SceneManager::SetLightAmbient(float r,float g,float b) {
	m_light.ambient.r=r; m_light.ambient.g=g; m_light.ambient.b=b; 
};

void SceneManager::SetLightIntensity(float intensity) {
	m_light.intensity=intensity;
};

void SceneManager::CullScene(Camera *camera) {
	m_frame->projection=camera->GetProj();
	m_frame->view=camera->GetView();
	m_light.eye=camera->GetPosition();

	DirectX::BoundingFrustum::CreateFromMatrix(m_frustum.m_frustum,m_frame->projection);
	m_frustum.m_frustum.Transform(m_frustum.m_frustum,m_frame->view);

	//  cull all objects against the frustum
	for(unsigned int i=0; i<m_scene.size(); i++)
	{
		//m_scene[i]
		DirectX::BoundingSphere sphere(
			m_scene[i]->m_bounding_volume.m_position,
			m_scene[i]->m_bounding_volume.m_radius);
		if(m_frustum.m_frustum.Intersects(sphere)) {
			m_visible_scene.push_back(i);
		}
	}
};


template <typename T>
void Swap(T &a, T &b){
	T c=a; a=b; b=c;
};

void SceneManager::SortScene() {
	// sort all objects based on shader 
	unsigned int cid=0;
	do{
		cid=0;
		for(unsigned int i=0; i<m_visible_scene.size(); i++)
		{
			unsigned int id0 = m_scene[m_visible_scene[i]]->m_material.m_shader->m_id;
			unsigned int id1 = m_scene[m_visible_scene[i+1]]->m_material.m_shader->m_id;
			if(id0>id1)
			{
				Swap(m_visible_scene[i], m_visible_scene[i+1]);
				cid++;
			}
		}
	}while (cid>0);
};

void SceneManager::RenderScene(RenderSystem *render_system) {
	// render the culled and sorted objects
	unsigned int sh_id=0;
	for (unsigned int i = 0; i < m_visible_scene.size(); i++)
	{
		SceneObject *object=m_scene[m_visible_scene[i]];

		if(sh_id!= object->m_material.m_shader->m_id){
			object->m_material.m_shader->SetVSConstant(0,sizeof(OncePerFrame),m_frame);
			sh_id=object->m_material.m_shader->m_id;
		}

		m_object->world=object->m_transform;
		object->m_material.m_shader->SetVSConstant(1,sizeof(OncePerObject),m_object);

		render_system->SelectShader(object->m_material.m_shader);
		render_system->SelectTexture(object->m_material.m_texture, 0);
		render_system->SelectBlendState(object->m_material.m_states.m_blend);
		render_system->SelectDepthState(object->m_material.m_states.m_depth);
		render_system->SelectRasterizerState(object->m_material.m_states.m_rasterizer);
		render_system->SelectIndexBuffer(&object->m_renderable->m_index_buffer);
		render_system->SelectVertexBuffer(&object->m_renderable->m_vertex_buffer,0);
		render_system->Apply();

		if (object->m_renderable->m_index_buffer.m_buffer){
			render_system->DrawIndexed(PRIM_TRIANGLE_LIST,0,0,object->m_renderable->m_count);
		}
		else
		{
			render_system->Draw(PRIM_TRIANGLE_LIST, 0, object->m_renderable->m_count);
		};
	}
};
