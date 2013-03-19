// uMath.cpp

#include "stdafx.h"
#include <cmath>
#include <ctime>
#include <random>
#include "uMath.h"

static const float TORAD = Math::PI / 180.0f;
static const float TODEG = 180.0f / Math::PI;

namespace Math 
{
	float ToRad(float degree) { return degree * TORAD; }
	float ToDeg(float radian) {return radian*TODEG; }
	float Sqrt(float value) { return sqrtf(value); }
	float Sin(float radian) { return sinf(radian); }
	float Cos(float radian) { return cosf(radian); }
	float Tan(float radian) { return tanf(radian); }
	float ACos(float radian) { return acosf(radian); }
	float ASin(float radian) { return asinf(radian); }
	float Atan(float radian) { return atanf(radian); }
	float Atan2(float radian1, float radian2) { return atan2f(radian1, radian2); }
}; // namespace Math

typedef std::mt19937 mt_t;

namespace Random 
{
	static std::uniform_int_distribution<mt_t::result_type> s_engine;
	static mt_t s_mtwister;

	void Init() { s_mtwister.seed((mt_t::result_type)time(0)); }
	int Random() { return s_engine(s_mtwister); }
	float UnitRandom() { return (float)s_engine(s_mtwister) / (float)s_mtwister.max(); }
	float Random(float min, float max) { return min + UnitRandom() * (max-min); }
}; // namespace Random

Vector2::Vector2() { }
Vector2::Vector2(const Vector2& rhs)				{ x = rhs.x; y = rhs.y; }
Vector2::Vector2(float X, float Y) : x(X), y(Y)		{ };
Vector2& Vector2::operator=(const Vector2& rhs)		{ x = rhs.x; y = rhs.y; return *this; }
void Vector2::operator=(const Vec2& rhs)			{ x = rhs.x; y = rhs.y; }
float Vector2::Length()								{ return sqrtf(x * x + y * y); }
void Vector2::Normalize()							{ float l = Length(); x /= l; y /= l; }

Vector3::Vector3()									{ }
Vector3::Vector3(const Vector3& rhs)				{ x = rhs.x; y = rhs.y; z = rhs.z; }
Vector3::Vector3(float X,float Y,float Z) : x(X),y(Y),z(Z) { };
Vector3& Vector3::operator=(const Vector3& rhs)		{ x =rhs.x; y = rhs.y; z = rhs.z; return *this; }
void Vector3::operator=(const Vec3& rhs)			{ x = rhs.x; y = rhs.y; z = rhs.z; }
Vector3 Vector3::operator+(const Vector3& rhs)		{ return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
Vector3 Vector3::operator-(const Vector3& rhs)		{ return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
Vector3& Vector3::operator+=(const Vector3& rhs)	{ x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
Vector3& Vector3::operator-=(const Vector3& rhs)	{ x -= rhs.x; y -= rhs.y; z-= rhs.z; return *this; }
float Vector3::Length()								{ return sqrtf(x * x + y * y + z * z); }
void Vector3::Normalize()							{ float l = Length(); x /= l; y /= l; z /= l; }

Vector3 Vector3::Cross(const Vector3& rhs) 
{
	return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

Vector3& Vector3::operator/=(const float v)			{ x /= v; y /= v; z /= v; return *this; }
Vector3 Vector3::operator*(const float v)			{ return Vector3(x * v, y * v, z * v); }
