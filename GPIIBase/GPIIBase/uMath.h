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

typedef DirectX::XMVECTOR Vec2;
typedef DirectX::XMVECTOR Vec3;
typedef DirectX::XMVECTOR Vec4;
typedef DirectX::XMMATRIX Mat4;

#endif // UMATH_H_INCLUDED
