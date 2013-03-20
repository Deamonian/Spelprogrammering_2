// TextureManager.cpp

#include "stdafx.h"
#include "RenderSystem.h"
#include "TextureManager.h"

TextureManager::TextureManager(RenderSystem *render_system) 
: m_render_system(render_system)
, m_allocator(512)
{
};

TextureManager::~TextureManager() {
	if(m_textures.empty()) {return;};
	TextureMap::iterator it=m_textures.begin();
	while(it!=m_textures.end()) {
		it->second->Release();
		++it;
	};
	m_textures.clear();
};

Texture *TextureManager::Create(const String &filename) {
	TextureMap::iterator it=m_textures.find(filename);
	if(it==m_textures.end()) {
		Texture *texture=m_allocator.Allocate();
		if(!m_render_system->CreateTexture(texture,filename)) {
			m_allocator.Free(texture);
			return nullptr;
		};
		m_textures.insert(std::make_pair(filename,texture));
		it=m_textures.find(filename);
	};
	return it->second;
};

void TextureManager::Destroy(Texture *texture) {
	TextureMap::iterator it=m_textures.begin();
	while(it!=m_textures.end()) {
		if(it->second==texture) {
			texture->Release();
			m_textures.erase(it);
			break;
		};
		++it;
	};
};
