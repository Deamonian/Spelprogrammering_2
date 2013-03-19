// ShaderManager.cpp

#include "stdafx.h"
#include "RenderSystem.h"
#include "ShaderManager.h"

ShaderManager::ShaderManager(RenderSystem *render_system) 
: mRenderSystem(render_system)
, mAllocator(128)
{

};

ShaderManager::~ShaderManager() 
{
	if(mShaders.empty()) { return; };
	ShaderMap::iterator it = mShaders.begin();
	while(it != mShaders.end()) 
	{
		it->second->Release();
		++it;
	};
	mShaders.clear();
};

Shader* ShaderManager::Create(const String& vertex_shader, const String& pixel_shader) 
{
	ShaderMap::iterator it = mShaders.find(std::make_pair(vertex_shader, pixel_shader));
	if(it == mShaders.end()) 
	{
		Shader* shader = mAllocator.Allocate();
		if(!mRenderSystem->CreateShader(shader, vertex_shader, pixel_shader)) 
		{
			mAllocator.Free(shader);
			return nullptr;
		};
		mShaders.insert(std::make_pair(std::make_pair(vertex_shader, pixel_shader), shader));
		it = mShaders.find(std::make_pair(vertex_shader, pixel_shader));
	};
	return it->second;
};

void ShaderManager::Destroy(Shader* shader) 
{
	ShaderMap::iterator it = mShaders.begin();
	while(it != mShaders.end()) 
	{
		if(it->second == shader) 
		{
			shader->Release();
			mShaders.erase(it);
			break;
		};
		++it;
	};
};
