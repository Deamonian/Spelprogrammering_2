// TextureManager.h

#ifndef TEXTUREMANAGER_H_INCLUDED
#define TEXTUREMANAGER_H_INCLUDED

class RenderSystem;

class TextureManager{
public:
	TextureManager(RenderSystem *render_system);
	~TextureManager();

	Texture *Create(const String &filename);
	void Destroy(Texture *texture);

private:
	typedef std::map<String,Texture*> TextureMap;
	RenderSystem *m_render_system;
	Allocator<Texture> m_allocator;
	TextureMap m_textures;
};

#endif // TEXTUREMANAGER_H_INCLUDED
