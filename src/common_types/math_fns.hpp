#pragma once
#include <cmath>

namespace math {

// TODO: all this functions need to be defined in terms of
// basic floating point operations (+, -, *, /)

inline constexpr float pi = 3.14159265358979323846f;
inline constexpr float pi_2 = pi / 2.f;

inline void internal_check(float v, float err, float lower, float upper) noexcept {
	assert(err >= 0.f); // error must be positive
	assert(lower <= upper); // conflicting definitions
	assert(v + err >= lower && v - err <= upper); // unreasonable
}

inline float sin(float v) noexcept {
	// Bhaskara formula, expanded for -2*pi <-> pi, error of 0.0016
	assert(v >= -2.f * pi && v <= 2.f * pi);
	if(v < -pi) {
		v = -v - pi;
	} else if(v < 0.f) {
		v = -v;
	} else if(v > pi) {
		v = v - pi;
	}
	if((v >= -pi && v <= 0.f) || (v >= pi && v <= 2.f * pi)) {
		float r = -(-16.f * v * (v - pi) / (4.f * v * (v - pi) + 49.3480220054468f));
		internal_check(r, 0.0016f, -1.f, 1.f);
		return r;
	} else if((v >= 0.f && v <= pi) || (v >= -2.f * pi && v <= -pi)) {
		float r = -16.f * v * (v - pi) / (4.f * v * (v - pi) + 49.3480220054468f);
		internal_check(r, 0.0016f, -1.f, 1.f);
		return r;
	} else {
		assert(0);
	}
}

inline float cos(float v) noexcept {
	float r = math::sin(pi_2 - v);
	internal_check(r, 0.0016f, -1.f, 1.f);
	return r;
}

inline float acos(float v) noexcept {
	// Lagrange polynomial - https://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm
	// Maximum absolute error of 0.017
	assert(v >= -1.f && v <= 1.f);
	float r = ((0.4643653210307f * v * v * v + 0.921784152891457f * v * v - 2.0178302343512f * v - 0.939115566365855f) * v + 1.5707963267949f) / ((0.295624144969963f * v * v - 1.28459062446908f) * (v * v) + 1.f);
	internal_check(r, 0.017f, 0.f, pi);
	return r;
}

inline float sqrt(float x) noexcept {
	union {
		float f;
		int i;
	} u = {x};
	u.i = 0x5f375a86 - (u.i >> 1);
	u.f = u.f * (1.5f - (0.5f * x) * u.f * u.f);
	u.f = u.f * (1.5f - (0.5f * x) * u.f * u.f);
	return u.f * x;
}

}
