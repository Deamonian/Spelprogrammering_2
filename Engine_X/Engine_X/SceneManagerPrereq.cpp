// SceneManagerPrereq.cpp

#include "stdafx.h"
#include "SceneManagerPrereq.h"

Color::Color() 
{
	r = g = b = a = 1.0f;
};

Color::Color(float R, float G, float B, float A) 
{
	r = R; g = G; b = B; a = A;
};

DirectionalLight::DirectionalLight() 
{
	direction = Vector3(0, -1, 0); 
	intensity = 1.0f;
	diffuse = Color(0.3f, 0.3f, 0.3f, 1.0f);
	ambient = Color(0.8f, 0.8f, 0.8f, 1.0f);
	eye = Vector3(0.0f, 0.0f, 0.0f); 
	pad0 = 0.0f;
};

BoundingSphere::BoundingSphere() 
: mPosition(0.0f, 0.0f, 0.0f)
, mRadius(0.0f) 
{
};

BoundingSphere::BoundingSphere(const Vec3& position, const float radius) 
{
	mPosition = position;
	mRadius = radius;
};

States::States() 
{
	mBlend = nullptr;
	mDepth = nullptr;
	mRasterizer = nullptr;
};

Material::Material() 
{
	mShader = nullptr; 
	mTexture = nullptr;
};

unsigned int SceneObject::msIDGen = 0;
SceneObject::SceneObject() 
{
	mID = ++msIDGen;
	mDepth = 0;
	mTransform = DirectX::XMMatrixIdentity();
	mRenderable = nullptr;
};