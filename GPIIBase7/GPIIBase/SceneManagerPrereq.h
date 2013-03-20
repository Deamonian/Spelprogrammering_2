// SceneManagerPrereq.h

#ifndef SCENEMANAGERPREREQ_H_INCLUDED
#define SCENEMANAGERPREREQ_H_INCLUDED

struct __declspec(align(16)) OncePerFrame {
	Mat4 projection;
	Mat4 view;
};

struct __declspec(align(16)) OncePerObject {
	Mat4 world;
};

struct Color {
	Color();
	Color(float R,float G,float B,float A);
	float r,g,b,a;
};

struct DirectionalLight {
	DirectionalLight();

	Vector3 direction; 
	float intensity;
	Color diffuse;
	Color ambient;
	Vector3 eye; 
	float pad0;
};

struct BoundingSphere {
	BoundingSphere();
	BoundingSphere(const Vec3 &position,const float radius);
	Vec3 m_position; 
	float m_radius;
};

struct Frustum {
	DirectX::BoundingFrustum m_frustum;
};

struct States {
	States();

	BlendState *m_blend;
	DepthState *m_depth;
	RasterizerState *m_rasterizer;
};

struct Material {
	Material();
	Shader *m_shader;
	Texture *m_texture;
	States m_states;
};

struct SceneObject {
	SceneObject();

	unsigned int m_id;
	unsigned int m_depth;
	Material m_material;
	Mat4 m_transform;
	BoundingSphere m_bounding_volume;
	Renderable *m_renderable;

private:
	static unsigned int ms_id_gen;
};

#endif // SCENEMANAGERPREREQ_H_INCLUDED
