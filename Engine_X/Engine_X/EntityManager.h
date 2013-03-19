#pragma once
struct Vertex
{
	Vector3 pos;
	unsigned int color;
};
struct Entity
{
	Renderable* mRenderable;
	Shader* mShader;
	Constant* mConstants;
	DepthState* mDepthState;
	RasterizerState* mRasterizerState;
	int id;
	bool killMe;
	unsigned int color;
	Vertex* vertices;
	unsigned short* indices;
};
class RenderSystem;
class EntityManager
{
public:
	EntityManager(void);
	~EntityManager(void);
	bool Initialize(RenderSystem* rendersystem);
	void CreateEntity(String type);
	void KillEntity(int id);
	void Update();
private:
	std::vector<Entity*> mEntities;
	RenderSystem* mRenderSystem;
	int mNumEntities;
};

