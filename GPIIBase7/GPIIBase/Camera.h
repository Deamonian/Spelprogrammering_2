// Camera.h

#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

class Camera {
public:
	Camera();

	void Update();

	void MoveForward(float amount);
	void MoveSideways(float amount);
	void MoveUp(float amount);
	void RotateX(float amount);
	void RotateY(float amount);
	void RotateZ(float amount);

	void SetPerspective(float aspect,float fov,float znear,float zfar);

	Mat4 GetProj();
	Mat4 GetView();
	Vec3 GetPosition() const;

private:
	bool m_dirty;

	float m_yaw;
	float m_pitch;
	float m_roll;

	Vec3 m_right;
	Vec3 m_up;
	Vec3 m_forward;
	Vec3 m_position;

	Mat4 m_view;
	Mat4 m_proj;
};

#endif // CAMERA_H_INCLUDED
