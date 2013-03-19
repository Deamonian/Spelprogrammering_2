// Camera.h

#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

class Camera 
{
public:
	Camera();

	void Update();

	void MoveForward(float amount);
	void MoveSideways(float amount);
	void MoveUp(float amount);
	void RotateX(float amount);
	void RotateY(float amount);
	void RotateZ(float amount);

	void SetPerspective(float aspect, float fov, float znear, float zfar);

	Mat4 GetProj();
	Mat4 GetView();
	Vec3 GetPosition() const;

private:
	bool mDirty;

	float mYaw;
	float mPitch;
	float mRoll;

	Vec3 mRight;
	Vec3 mUp;
	Vec3 mForward;
	Vec3 mPosition;

	Mat4 mView;
	Mat4 mProj;
};

#endif // CAMERA_H_INCLUDED
