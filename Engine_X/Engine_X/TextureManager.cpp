// TextureManager.cpp

#include "stdafx.h"
#include "RenderSystem.h"
#include "TextureManager.h"

TextureManager::TextureManager(RenderSystem* render_system) 
: mRenderSystem(render_system)
, mAllocator(512)
{
};

TextureManager::~TextureManager() 
{
	if(mTextures.empty()) { return; };
	TextureMap::iterator it = mTextures.begin();
	while(it != mTextures.end()) 
	{
		it->second->Release();
		++it;
	};
	mTextures.clear();
};

Texture* TextureManager::Create(const String& filename) 
{
	TextureMap::iterator it = mTextures.find(filename);
	if(it == mTextures.end()) 
	{
		Texture* texture = mAllocator.Allocate();
		if(!mRenderSystem->CreateTexture(texture, filename)) 
		{
			mAllocator.Free(texture);
			return nullptr;
		};
		mTextures.insert(std::make_pair(filename, texture));
		it = mTextures.find(filename);
	};
	return it->second;
};

void TextureManager::Destroy(Texture* texture) 
{
	TextureMap::iterator it = mTextures.begin();
	while(it != mTextures.end()) 
	{
		if(it->second == texture)
		{
			texture->Release();
			mTextures.erase(it);
			break;
		};
		++it;
	};
};
