// ShaderManager.cpp

#include "stdafx.h"
#include "RenderSystem.h"
#include "ShaderManager.h"

ShaderManager::ShaderManager(RenderSystem *render_system) 
: m_render_system(render_system)
, m_allocator(128)
{

};

ShaderManager::~ShaderManager() {
	if(m_shaders.empty()) {return;};
	ShaderMap::iterator it=m_shaders.begin();
	while(it!=m_shaders.end()) {
		it->second->Release();
		++it;
	};
	m_shaders.clear();
};

Shader *ShaderManager::Create(const String &vertex_shader,const String &pixel_shader) {
	ShaderMap::iterator it=m_shaders.find(std::make_pair(vertex_shader,pixel_shader));
	if(it==m_shaders.end()) {
		Shader *shader=m_allocator.Allocate();
		if(!m_render_system->CreateShader(shader,vertex_shader,pixel_shader)) {
			m_allocator.Free(shader);
			return nullptr;
		};
		m_shaders.insert(std::make_pair(std::make_pair(vertex_shader,pixel_shader),shader));
		it=m_shaders.find(std::make_pair(vertex_shader,pixel_shader));
	};
	return it->second;
};

void ShaderManager::Destroy(Shader *shader) {
	ShaderMap::iterator it=m_shaders.begin();
	while(it!=m_shaders.end()) {
		if(it->second==shader) {
			shader->Release();
			m_shaders.erase(it);
			break;
		};
		++it;
	};
};
