// SceneManager.cpp

#include "stdafx.h"
#include "Skydome.h"
//#include "Skybox.h"
//#include "Heightmap.h"
#include "Camera.h"
#include "RenderSystem.h"
#include "SceneManager.h"

SceneManager::SceneManager(ShaderManager* shader_manager, TextureManager* texture_manager) 
	: mShaderManager(shader_manager),
	mTextureManager(texture_manager),
	mAllocator(1024)
{
	mSkydome = nullptr;
	//mHeightmap=nullptr;
	mFrame = new (_aligned_malloc(sizeof(OncePerFrame), 16)) OncePerFrame;
	mObject = new (_aligned_malloc(sizeof(OncePerObject), 16)) OncePerObject;
};

SceneManager::~SceneManager() 
{
	_aligned_free(mFrame);
	_aligned_free(mObject);
};

bool SceneManager::Init(int pre_alloc_size) 
{
	mScene.reserve(pre_alloc_size);
	mVisibleScene.reserve(pre_alloc_size);
	mSortedScene.reserve(pre_alloc_size);

	return true;
};

void SceneManager::Shut() 
{
	if(mSkydome) 
	{
		mSkydome->Shut();
		delete mSkydome;
		mSkydome=nullptr;
	};
};

void SceneManager::Render(RenderSystem* render_system, Camera* camera) 
{
	CullScene(camera);
	SortScene();
	if(mSkydome) { mSkydome->Render(render_system, camera); };
	//if(m_heightmap) {m_heightmap->Render(render_system,m_frame,&m_light);};
	RenderScene(render_system);
};

SceneObject* SceneManager::Create(Shader* shader, Texture* texture, Renderable* renderable) 
{
	SceneObject* object = mAllocator.Allocate();
	object->mRenderable = renderable;
	object->mMaterial.mShader = shader;
	object->mMaterial.mTexture = texture;
	object->mBoundingVolume.mRadius = renderable->mVertexBuffer.mRadius;
	mScene.push_back(object);
	return object;
};

void SceneManager::Destroy(SceneObject* object) 
{
	std::vector<SceneObject*>::iterator it = mScene.begin();
	while(it != mScene.end()) 
	{
		if((*it) == object) 
		{
			mAllocator.Free(object);
			mScene.erase(it);
			break;
		};
		++it;
	};
};

void SceneManager::SetSkydome(Skydome* skydome) 
{
	mSkydome = skydome;
};

//void SceneManager::SetHeightmap(Heightmap *heightmap) {
//	m_heightmap=heightmap;
//};

void SceneManager::SetLightDirection(const Vector3& direction) 
{
	mLight.direction = direction;
	mLight.direction.Normalize();
};

void SceneManager::SetLightDiffuse(float r, float g, float b) 
{
	mLight.diffuse.r = r; 
	mLight.diffuse.g = g; 
	mLight.diffuse.b = b; 
};

void SceneManager::SetLightAmbient(float r, float g, float b) 
{
	mLight.ambient.r = r; 
	mLight.ambient.g = g; 
	mLight.ambient.b = b; 
};

void SceneManager::SetLightIntensity(float intensity) 
{
	mLight.intensity = intensity;
};

void SceneManager::CullScene(Camera* camera) 
{
	mFrame->projection = camera->GetProj();
	mFrame->view = camera->GetView();
	mLight.eye = camera->GetPosition();

	DirectX::BoundingFrustum::CreateFromMatrix(mFrustum.mFrustum, mFrame->projection);
	mFrustum.mFrustum.Transform(mFrustum.mFrustum, mFrame->view);

	//  cull all objects against the frustum
	for(unsigned int i = 0; i < mScene.size(); ++i)
	{
		//m_scene[i]
		DirectX::BoundingSphere sphere(
			mScene[i]->mBoundingVolume.mPosition,
			mScene[i]->mBoundingVolume.mRadius);
		if(mFrustum.mFrustum.Intersects(sphere)) 
		{
			mVisibleScene.push_back(i);
		}
	}
};


template <typename T>
void Swap(T& a, T& b)
{
	T c = a; a = b; b = c;
};

void SceneManager::SortScene() 
{
	// sort all objects based on shader 
	unsigned int cid = 0;
	do
	{
		cid = 0;
		for(unsigned int i = 0; i < mVisibleScene.size(); ++i)
		{
			unsigned int id0 = mScene[mVisibleScene[i]]->mMaterial.mShader->mID;
			unsigned int id1 = mScene[mVisibleScene[i + 1]]->mMaterial.mShader->mID;
			if(id0 > id1)
			{
				Swap(mVisibleScene[i], mVisibleScene[i + 1]);
				cid++;
			}
		}
	} while (cid > 0);
};

void SceneManager::RenderScene(RenderSystem* render_system) 
{
	// render the culled and sorted objects
	unsigned int sh_id = 0;
	for (unsigned int i = 0; i < mVisibleScene.size(); ++i)
	{
		SceneObject* object = mScene[mVisibleScene[i]];

		if(sh_id != object->mMaterial.mShader->mID)
		{
			object->mMaterial.mShader->SetVSConstant(0, sizeof(OncePerFrame),mFrame);
			sh_id = object->mMaterial.mShader->mID;
		}

		mObject->world = object->mTransform;
		object->mMaterial.mShader->SetVSConstant(1, sizeof(OncePerObject), mObject);

		render_system->SelectShader(object->mMaterial.mShader);
		render_system->SelectTexture(object->mMaterial.mTexture, 0);
		render_system->SelectBlendState(object->mMaterial.mStates.mBlend);
		render_system->SelectDepthState(object->mMaterial.mStates.mDepth);
		render_system->SelectRasterizerState(object->mMaterial.mStates.mRasterizer);
		render_system->SelectIndexBuffer(&object->mRenderable->mIndexBuffer);
		render_system->SelectVertexBuffer(&object->mRenderable->mVertexBuffer, 0);
		render_system->Apply();

		if (object->mRenderable->mIndexBuffer.mBuffer)
		{
			render_system->DrawIndexed(PRIM_TRIANGLE_LIST, 0, 0, object->mRenderable->mCount);
		}
		else
		{
			render_system->Draw(PRIM_TRIANGLE_LIST, 0, object->mRenderable->mCount);
		};
	}
};
