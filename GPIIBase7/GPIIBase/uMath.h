// uMath.h

#ifndef UMATH_H_INCLUDED
#define UMATH_H_INCLUDED

namespace Math {
	const float PI=3.1415f;
	float ToRad(float degree);
	float ToDeg(float radian);
	float Sqrt(float value);
	float Sin(float radian);
	float Cos(float radian);
	float Tan(float radian);
	float ACos(float radian);
	float ASin(float radian);
	float Atan(float radian);
	float Atan2(float radian1,float radian2);

	template <typename T>
	T Max(T a,T b) {return a>b?a:b;};

	template <typename T>
	T Abs(T value) {return value<0?-value:value;};

	template <typename T>
	T Sign(T value) {return value<0?-1:1;};

	template <typename T>
	T Clamp(T value,T min,T max) {return value<min?min:value>max?max:value;};
}; // namespace Math

namespace Random {
	void Init();
	int Random();
	float UnitRandom();
	float Random(float min,float max);
}; // namespace Random

typedef DirectX::XMFLOAT2 Vec2;
typedef DirectX::XMFLOAT3 Vec3;
typedef DirectX::XMFLOAT4 Vec4;
typedef DirectX::XMMATRIX Mat4;

struct Vector2 {
	Vector2();
	Vector2(const Vector2 &rhs);
	Vector2(float X,float Y);

	Vector2 &operator=(const Vector2 &rhs);
	void operator=(const Vec2 &rhs);

	float Length();
	void Normalize();

	float x,y;
};

struct Vector3 {
	Vector3();
	Vector3(const Vector3 &rhs);
	Vector3(float X,float Y,float Z);

	Vector3 &operator=(const Vector3 &rhs);
	void operator=(const Vec3 &rhs);
	Vector3 operator+(const Vector3 &rhs);
	Vector3 operator-(const Vector3 &rhs);
	Vector3 &operator+=(const Vector3 &rhs);
	Vector3 &operator-=(const Vector3 &rhs);
	Vector3 &operator/=(const float v);

	Vector3 operator*(const float v);

	Vector3 Cross(const Vector3 &rhs);
	float Length();
	void Normalize();

	float x,y,z;
};

#endif // UMATH_H_INCLUDED
