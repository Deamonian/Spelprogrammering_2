// SceneManagerPrereq.h

#ifndef SCENEMANAGERPREREQ_H_INCLUDED
#define SCENEMANAGERPREREQ_H_INCLUDED

struct __declspec(align(16)) OncePerFrame 
{
	Mat4 projection;
	Mat4 view;
};

struct __declspec(align(16)) OncePerObject 
{
	Mat4 world;
};

struct Color 
{
	Color();
	Color(float R, float G, float B, float A);
	float r, g, b, a;
};

struct DirectionalLight 
{
	DirectionalLight();

	Vector3 direction; 
	float intensity;
	Color diffuse;
	Color ambient;
	Vector3 eye; 
	float pad0;
};

struct BoundingSphere 
{
	BoundingSphere();
	BoundingSphere(const Vec3& position, const float radius);
	Vec3 mPosition; 
	float mRadius;
};

struct Frustum 
{
	DirectX::BoundingFrustum mFrustum;
};

struct States 
{
	States();

	BlendState* mBlend;
	DepthState* mDepth;
	RasterizerState* mRasterizer;
};

struct Material 
{
	Material();
	Shader* mShader;
	Texture* mTexture;
	States mStates;
};

struct SceneObject 
{
	SceneObject();

	unsigned int mID;
	unsigned int mDepth;
	Material mMaterial;
	Mat4 mTransform;
	BoundingSphere mBoundingVolume;
	Renderable* mRenderable;

private:
	static unsigned int msIDGen;
};
#endif // SCENEMANAGERPREREQ_H_INCLUDED
