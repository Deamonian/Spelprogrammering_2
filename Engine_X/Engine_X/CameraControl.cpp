// CameraControl.cpp

#include "stdafx.h"
#include "MessageSystem.h"
#include "Camera.h"
#include "CameraControl.h"

CameraControl::CameraControl(Camera* camera) 
{
	mCamera = camera;
	::memset(mKeyboard.mKeys, 0, sizeof(mKeyboard.mKeys));
	::memset(mMouse.mButton, 0, sizeof(mMouse.mButton));
	mMouse.mDX = 0; 
	mMouse.mDY = 0;
	mMouse.mX = 0; 
	mMouse.mY = 0;
	Settings();
};

void CameraControl::Init(MessageSystem* message_system) 
{
	message_system->Register(EVT_MOUSEMOVE, this, &CameraControl::OnMouseMove);
	message_system->Register(EVT_MOUSEBUTTON, this, &CameraControl::OnMouseButton);
	message_system->Register(EVT_KEYBOARD, this, &CameraControl::OnKeyboard);
};

void CameraControl::Shut(MessageSystem *message_system) 
{
	message_system->Unregister(EVT_MOUSEMOVE, this);
	message_system->Unregister(EVT_MOUSEBUTTON, this);
	message_system->Unregister(EVT_KEYBOARD, this);
};

void CameraControl::Settings(float camera_speed, float yaw, float pitch, float sensitivity) 
{
	mKeyboard.mSpeed = camera_speed;
	mMouse.mYaw = yaw;
	mMouse.mPitch = pitch;
	mMouse.mSensitivity = sensitivity;
};

void CameraControl::Update(float deltatime) 
{
	const float dt = deltatime;
	const float amount = dt * mKeyboard.mSpeed;

	if(mKeyboard.mKeys[0]) { mCamera->MoveForward(amount); };
	if(mKeyboard.mKeys[1]) { mCamera->MoveForward(-amount); };
	if(mKeyboard.mKeys[2]) { mCamera->MoveSideways(-amount); };
	if(mKeyboard.mKeys[3]) { mCamera->MoveSideways(amount); };

	if(mMouse.mButton[MB_Right]) 
	{
		if(mMouse.mDX != 0 || mMouse.mDY != 0) 
		{
			float dx = dt * mMouse.mSensitivity * mMouse.mPitch * (float)mMouse.mDX;
			float dy = dt * mMouse.mSensitivity * mMouse.mYaw * (float)mMouse.mDY;

			mCamera->RotateX(-dy);
			mCamera->RotateY(dx);
		};
	};
	mMouse.mDX = 0; 
	mMouse.mDY = 0;

	mCamera->Update();
};

void CameraControl::OnKeyboard(IEvent* event) 
{
	KeyboardEvent* evt = static_cast<KeyboardEvent*>(event);
	if(evt->mKeycode == KC_w) { mKeyboard.mKeys[0] = evt->mState; };
	if(evt->mKeycode == KC_s) { mKeyboard.mKeys[1] = evt->mState; };
	if(evt->mKeycode == KC_a) { mKeyboard.mKeys[2] = evt->mState; };
	if(evt->mKeycode == KC_d) { mKeyboard.mKeys[3] = evt->mState; };
};

void CameraControl::OnMouseMove(IEvent* event) 
{
	MouseMoveEvent* evt = static_cast<MouseMoveEvent*>(event);
	mMouse.mDX = mMouse.mX - evt->mX;
	mMouse.mDY = mMouse.mY - evt->mY;
	mMouse.mX = evt->mX;
	mMouse.mY = evt->mY;
};

void CameraControl::OnMouseButton(IEvent* event) 
{
	MouseButtonEvent* evt = static_cast<MouseButtonEvent*>(event);
	if(evt->mButton == MB_Left) { mMouse.mButton[0] = evt->mState; }
	else if(evt->mButton == MB_Middle) { mMouse.mButton[1]=evt->mState; }
	else if(evt->mButton == MB_Right) { mMouse.mButton[2]=evt->mState; }
};