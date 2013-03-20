// SceneManagerPrereq.cpp

#include "stdafx.h"
#include "SceneManagerPrereq.h"

Color::Color() {
	r=g=b=a=1.0f;
};

Color::Color(float R,float G,float B,float A) {
	r=R; g=G; b=B; a=A;
};

DirectionalLight::DirectionalLight() {
	direction=Vector3(0,-1,0); 
	intensity=1.0f;
	diffuse=Color(0.3f,0.3f,0.3f,1.0f);
	ambient=Color(0.8f,0.8f,0.8f,1.0f);
	eye=Vector3(0.0f,0.0f,0.0f); 
	pad0=0.0f;;
};

BoundingSphere::BoundingSphere() 
: m_position(0.0f,0.0f,0.0f)
, m_radius(0.0f) 
{
};

BoundingSphere::BoundingSphere(const Vec3 &position,const float radius) {
	m_position=position;
	m_radius=radius;
};

States::States() {
	m_blend=nullptr;
	m_depth=nullptr;
	m_rasterizer=nullptr;
};

Material::Material() {
	m_shader=nullptr; 
	m_texture=nullptr;
};

unsigned int SceneObject::ms_id_gen=0;
SceneObject::SceneObject() {
	m_id=ms_id_gen++;
	m_depth=0;
	m_transform=DirectX::XMMatrixIdentity();
	m_renderable=nullptr;
};
