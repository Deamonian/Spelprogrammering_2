// TextureManager.h

#ifndef TEXTUREMANAGER_H_INCLUDED
#define TEXTUREMANAGER_H_INCLUDED

class RenderSystem;

class TextureManager
{
public:
	TextureManager(RenderSystem* render_system);
	~TextureManager();

	Texture* Create(const String& filename);
	void Destroy(Texture* texture);

private:
	typedef std::map<String, Texture*> TextureMap;
	RenderSystem* mRenderSystem;
	Allocator<Texture> mAllocator;
	TextureMap mTextures;
};

#endif // TEXTUREMANAGER_H_INCLUDED
