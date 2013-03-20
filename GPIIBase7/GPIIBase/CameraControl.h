// CameraControl.h

#ifndef CAMERACONTROL_H_INCLUDED
#define CAMERACONTROL_H_INCLUDED

class MessageSystem;
class Camera;

class CameraControl {
public:
	CameraControl(Camera *camera);

	void Init(MessageSystem *message_system);
	void Shut(MessageSystem *message_system);
	void Settings(float camera_speed=20.0f,float yaw=0.022f,float pitch=-0.022f,float sensitivity=50.0f);

	void Update(float deltatime);

	// callbacks
	void OnKeyboard(IEvent *event);
	void OnMouseMove(IEvent *event);
	void OnMouseButton(IEvent *event);

private:
	Camera *m_camera;

	struct {
		int m_x,m_y;
		int m_dx,m_dy;
		// left,middle,right,padding
		bool m_button[4];
		float m_sensitivity;
		float m_pitch;
		float m_yaw;
	} m_mouse;

	struct {
		// up,down,left,right
		bool m_keys[4];
		float m_speed;
	} m_keyboard;
};

#endif // CAMERACONTROL_H_INCLUDED
