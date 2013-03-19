#include "stdafx.h"
#include "EntityManager.h"

#include "RenderSystem.h"

EntityManager::EntityManager(void)
{
}


EntityManager::~EntityManager(void)
{
}

bool EntityManager::Initialize(RenderSystem* rendersystem)
{
	mRenderSystem = rendersystem;
	mNumEntities = 0;
	return true;
};
void EntityManager::CreateEntity(String type)
{
	if (type == "triangle")
	{
		Entity* obj = new Entity;
		obj->color = 0xffff0000;
		obj->id = mNumEntities+1;
		mNumEntities++;
		obj->killMe = false;

		mRenderSystem->CreateVertexBuffer(&(obj->mRenderable->mVertexBuffer), BM_DEFAULT, 5, sizeof(Vertex), obj->vertices);
		mRenderSystem->CreateIndexBuffer(&(obj->mRenderable->mIndexBuffer), 5*6, sizeof(unsigned short), obj->indices);
		
		mRenderSystem->CreateShader(obj->mShader, "../data/shader/vs.hlsl", "../data/shader/ps.hlsl");
		
		mRenderSystem->CreateDepthState(obj->mDepthState, true, true, CM_LESS, false, CM_ALWAYS, CM_ALWAYS, SO_KEEP, SO_KEEP, SO_KEEP, SO_KEEP, SO_REPLACE, SO_KEEP);
		mRenderSystem->CreateRasterizerState(obj->mRasterizerState, CULL_BACK, FILL_SOLID);

		D3D10_INPUT_ELEMENT_DESC desc[]={
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D10_INPUT_PER_VERTEX_DATA,0}
		};
		mRenderSystem->CreateInputLayout(obj->mShader, desc, 2);
	}
};
void EntityManager::KillEntity(int id)
{

};
void EntityManager::Update()
{
	
};