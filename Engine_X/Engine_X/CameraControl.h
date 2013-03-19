// CameraControl.h

#ifndef CAMERACONTROL_H_INCLUDED
#define CAMERACONTROL_H_INCLUDED

class MessageSystem;
class Camera;

class CameraControl 
{
public:
	CameraControl(Camera* camera);

	void Init(MessageSystem* message_system);
	void Shut(MessageSystem* message_system);
	void Settings(float camera_speed = 20.0f, float yaw = 0.022f, float pitch = -0.022f, float sensitivity = 50.0f);

	void Update(float deltatime);

	// callbacks
	void OnKeyboard(IEvent* event);
	void OnMouseMove(IEvent* event);
	void OnMouseButton(IEvent* event);

private:
	Camera* mCamera;

	struct 
	{
		int mX, mY;
		int mDX, mDY;
		// left,middle,right,padding
		bool mButton[4];
		float mSensitivity;
		float mPitch;
		float mYaw;
	} mMouse;

	struct 
	{
		// up,down,left,right
		bool mKeys[4];
		float mSpeed;
	} mKeyboard;
};

#endif // CAMERACONTROL_H_INCLUDED
