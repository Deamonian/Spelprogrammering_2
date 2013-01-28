// uMath.cpp

#include "stdafx.h"
#include <cmath>
#include <ctime>
#include <random>
#include "uMath.h"

static const float TORAD=Math::PI/180.0f;
static const float TODEG=180.0f/Math::PI;

namespace Math {
	float ToRad(float degree) {return degree*TORAD;};
	float ToDeg(float radian) {return radian*TODEG;};
	float Sqrt(float value) {return sqrtf(value);};
	float Sin(float radian) {return sinf(radian);};
	float Cos(float radian) {return cosf(radian);};
	float Tan(float radian) {return tanf(radian);};
	float ACos(float radian) {return acosf(radian);};
	float ASin(float radian) {return asinf(radian);};
	float Atan(float radian) {return atanf(radian);};
	float Atan2(float radian1,float radian2) {return atan2f(radian1,radian2);};
}; // namespace Math

typedef std::mt19937 mt_t;

namespace Random {
	static std::uniform_int_distribution<mt_t::result_type> s_engine;
	static mt_t s_mtwister;

	void Init() {s_mtwister.seed((mt_t::result_type)time(0));};
	int Random() {return s_engine(s_mtwister);};
	float UnitRandom() {return (float)s_engine(s_mtwister)/(float)s_mtwister.max();};
	float Random(float min,float max) {return min+UnitRandom()*(max-min);};
}; // namespace Random
