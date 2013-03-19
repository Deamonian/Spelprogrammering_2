// Camera.cpp

#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;

Camera::Camera() 
{
	mDirty = true;
	mYaw = 0.0f;
	mPitch = 0.0f;
	mRoll = 0.0f;

	mRight =		Vec3(1.0f,0.0f,0.0f);
	mUp =			Vec3(0.0f,1.0f,0.0f);
	mForward =		Vec3(0.0f,0.0f,1.0f);
	mPosition =		Vec3(0.0f,0.0f,-5.0f);

	XMVECTOR eye =	{0.0f,0.0f,-2.0f,0.0f},
		look =		{0.0f,0.0f,0.0f,0.0f},
		up =		{0.0f,1.0f,0.0f,0.0f};

	mView = XMMatrixLookAtLH(eye, look, up);
	mProj = XMMatrixIdentity();
};

void Camera::Update() 
{
	if(!mDirty) { return; }
	mDirty = false;

	XMVECTOR r = {1.0f,0.0f,0.0f},
			 u = {0.0f,1.0f,0.0f},
			 f = {0.0f,0.0f,1.0f};

	Mat4 ry  =XMMatrixRotationAxis(u, mYaw);
	f = XMVector3TransformNormal(f, ry);
	r = XMVector3TransformNormal(r, ry);

	Mat4 rx = XMMatrixRotationAxis(r, mPitch);
	f = XMVector3TransformNormal(f, rx);
	u = XMVector3TransformNormal(u, rx);

	Mat4 rz = XMMatrixRotationAxis(f, mRoll);
	u = XMVector3TransformNormal(u, rz);
	r = XMVector3TransformNormal(r, rz);

	r = XMVector3Normalize(r);
	u = XMVector3Normalize(u);
	f = XMVector3Normalize(f);

	XMStoreFloat3(&mRight, r);
	XMStoreFloat3(&mUp, u);
	XMStoreFloat3(&mForward, f);

	XMVECTOR p(XMLoadFloat3(&mPosition));

	mView=XMMatrixLookAtLH(p, p + f, u);
};

void Camera::MoveForward(float amount) 
{
	mDirty = true;

	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMVECTOR f = XMLoadFloat3(&mForward);
	XMVECTOR d = f * amount;
	p += d;

	XMStoreFloat3(&mPosition, p);
};

void Camera::MoveSideways(float amount) 
{
	mDirty = true;

	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR d = r * amount;
	p += d;

	XMStoreFloat3(&mPosition, p);
};

void Camera::MoveUp(float amount) 
{
	mDirty = true;

	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMVECTOR u = XMLoadFloat3(&mUp);
	XMVECTOR d = u * amount;
	p += d;

	XMStoreFloat3(&mPosition, p);
};

void Camera::RotateX(float amount) 
{
	mDirty = true;
	mPitch += amount;
};

void Camera::RotateY(float amount) 
{
	mDirty = true;
	mYaw += amount;
};

void Camera::RotateZ(float amount) 
{
	mDirty = true;
	mRoll += amount;
};

void Camera::SetPerspective(float aspect, float fov, float znear, float zfar) 
{
	mProj = XMMatrixPerspectiveFovLH(fov, aspect, znear, zfar);
};

Mat4 Camera::GetProj() 
{
	return mProj;
};

Mat4 Camera::GetView() 
{
	return mView;
};

Vec3 Camera::GetPosition() const 
{
	return mPosition;
};
