// Camera.cpp

#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;

Camera::Camera() 
{
	m_dirty=true;
	m_yaw=0.0f;
	m_pitch=0.0f;
	m_roll=0.0f;

	m_right=Vec3(1.0f,0.0f,0.0f);
	m_up=Vec3(0.0f,1.0f,0.0f);
	m_forward=Vec3(0.0f,0.0f,1.0f);
	m_position=Vec3(0.0f,0.0f,-5.0f);

	XMVECTOR eye={0.0f,0.0f,-2.0f,0.0f},
		look={0.0f,0.0f,0.0f,0.0f},
		up={0.0f,1.0f,0.0f,0.0f};

	m_view=XMMatrixLookAtLH(eye,look,up);
	m_proj=XMMatrixIdentity();
};

void Camera::Update() {
	if(!m_dirty) {return;};
	m_dirty=false;

	XMVECTOR r={1.0f,0.0f,0.0f},
		u={0.0f,1.0f,0.0f},
		f={0.0f,0.0f,1.0f};

	Mat4 ry=XMMatrixRotationAxis(u,m_yaw);
	f=XMVector3TransformNormal(f,ry);
	r=XMVector3TransformNormal(r,ry);

	Mat4 rx=XMMatrixRotationAxis(r,m_pitch);
	f=XMVector3TransformNormal(f,rx);
	u=XMVector3TransformNormal(u,rx);

	Mat4 rz=XMMatrixRotationAxis(f,m_roll);
	u=XMVector3TransformNormal(u,rz);
	r=XMVector3TransformNormal(r,rz);

	r=XMVector3Normalize(r);
	u=XMVector3Normalize(u);
	f=XMVector3Normalize(f);

	XMStoreFloat3(&m_right,r);
	XMStoreFloat3(&m_up,u);
	XMStoreFloat3(&m_forward,f);

	XMVECTOR p(XMLoadFloat3(&m_position));

	m_view=XMMatrixLookAtLH(p,p+f,u);
};

void Camera::MoveForward(float amount) {
	m_dirty=true;

	XMVECTOR p=XMLoadFloat3(&m_position);
	XMVECTOR f=XMLoadFloat3(&m_forward);
	XMVECTOR d=f*amount;
	p+=d;

	XMStoreFloat3(&m_position,p);
};

void Camera::MoveSideways(float amount) {
	m_dirty=true;

	XMVECTOR p=XMLoadFloat3(&m_position);
	XMVECTOR r=XMLoadFloat3(&m_right);
	XMVECTOR d=r*amount;
	p+=d;

	XMStoreFloat3(&m_position,p);
};

void Camera::MoveUp(float amount) {
	m_dirty=true;

	XMVECTOR p=XMLoadFloat3(&m_position);
	XMVECTOR u=XMLoadFloat3(&m_up);
	XMVECTOR d=u*amount;
	p+=d;

	XMStoreFloat3(&m_position,p);
};

void Camera::RotateX(float amount) {
	m_dirty=true;
	m_pitch+=amount;
};

void Camera::RotateY(float amount) {
	m_dirty=true;
	m_yaw+=amount;
};

void Camera::RotateZ(float amount) {
	m_dirty=true;
	m_roll+=amount;
};

void Camera::SetPerspective(float aspect,float fov,float znear,float zfar) {
	m_proj=XMMatrixPerspectiveFovLH(fov,aspect,znear,zfar);
};

Mat4 Camera::GetProj() {
	return m_proj;
};

Mat4 Camera::GetView() {
	return m_view;
};

Vec3 Camera::GetPosition() const {
	return m_position;
};
