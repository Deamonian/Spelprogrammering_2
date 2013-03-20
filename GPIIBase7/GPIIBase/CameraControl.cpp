// CameraControl.cpp

#include "stdafx.h"
#include "MessageSystem.h"
#include "Camera.h"
#include "CameraControl.h"

CameraControl::CameraControl(Camera *camera) {
	m_camera=camera;
	::memset(m_keyboard.m_keys,0,sizeof(m_keyboard.m_keys));
	::memset(m_mouse.m_button,0,sizeof(m_mouse.m_button));
	m_mouse.m_dx=0; m_mouse.m_dy=0;
	m_mouse.m_x=0; m_mouse.m_y=0;
	Settings();
};

void CameraControl::Init(MessageSystem *message_system) {
	message_system->Register(EVT_MOUSEMOVE,this,&CameraControl::OnMouseMove);
	message_system->Register(EVT_MOUSEBUTTON,this,&CameraControl::OnMouseButton);
	message_system->Register(EVT_KEYBOARD,this,&CameraControl::OnKeyboard);
};

void CameraControl::Shut(MessageSystem *message_system) {
	message_system->Unregister(EVT_MOUSEMOVE,this);
	message_system->Unregister(EVT_MOUSEBUTTON,this);
	message_system->Unregister(EVT_KEYBOARD,this);
};

void CameraControl::Settings(float camera_speed,float yaw,float pitch,float sensitivity) {
	m_keyboard.m_speed=camera_speed;
	m_mouse.m_yaw=yaw;
	m_mouse.m_pitch=pitch;
	m_mouse.m_sensitivity=sensitivity;
};

void CameraControl::Update(float deltatime) {
	const float dt=deltatime;
	const float amount=dt*m_keyboard.m_speed;

	if(m_keyboard.m_keys[0]) {m_camera->MoveForward(amount);};
	if(m_keyboard.m_keys[1]) {m_camera->MoveForward(-amount);};
	if(m_keyboard.m_keys[2]) {m_camera->MoveSideways(-amount);};
	if(m_keyboard.m_keys[3]) {m_camera->MoveSideways(amount);};

	if(m_mouse.m_button[MB_Right]) {
		if(m_mouse.m_dx!=0||m_mouse.m_dy!=0) {
			float dx=dt*m_mouse.m_sensitivity*m_mouse.m_pitch*(float)m_mouse.m_dx;
			float dy=dt*m_mouse.m_sensitivity*m_mouse.m_yaw*(float)m_mouse.m_dy;

			m_camera->RotateX(dy);
			m_camera->RotateY(dx);
		};
	};
	m_mouse.m_dx=0; 
	m_mouse.m_dy=0;

	m_camera->Update();
};

void CameraControl::OnKeyboard(IEvent *event) {
	KeyboardEvent *evt=static_cast<KeyboardEvent*>(event);
	if(evt->m_keycode==KC_w) {m_keyboard.m_keys[0]=evt->m_state;};
	if(evt->m_keycode==KC_s) {m_keyboard.m_keys[1]=evt->m_state;};
	if(evt->m_keycode==KC_a) {m_keyboard.m_keys[2]=evt->m_state;};
	if(evt->m_keycode==KC_d) {m_keyboard.m_keys[3]=evt->m_state;};
};

void CameraControl::OnMouseMove(IEvent *event) {
	MouseMoveEvent *evt=static_cast<MouseMoveEvent*>(event);
	m_mouse.m_dx=m_mouse.m_x-evt->m_x;
	m_mouse.m_dy=m_mouse.m_y-evt->m_y;
	m_mouse.m_x=evt->m_x;
	m_mouse.m_y=evt->m_y;
};

void CameraControl::OnMouseButton(IEvent *event) {
	MouseButtonEvent *evt=static_cast<MouseButtonEvent*>(event);
	if(evt->m_button==MB_Left) {m_mouse.m_button[0]=evt->m_state;}
	else if(evt->m_button==MB_Middle) {m_mouse.m_button[1]=evt->m_state;}
	else if(evt->m_button==MB_Right) {m_mouse.m_button[2]=evt->m_state;}
};