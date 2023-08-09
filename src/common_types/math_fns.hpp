#pragma once
#include <cmath>

namespace math {

// TODO: all this functions need to be defined in terms of
// basic floating point operations (+, -, *, /)

inline constexpr float pi = 3.14159265358979323846f;
inline constexpr float pi_2 = pi / 2.f;

inline float sin(float v) noexcept {
	return std::sin(v);
	/*
	// Bhaskara formula, expanded for -2*pi <-> pi, error of 0.0016
	assert(v >= -2.f * pi && v <= 2.f * pi);
	if(v < 0.f) { // Negative domain, simply flip the sign of the result and input :D
		v = -v;
		if(v < -pi) { // Cover domain -2*pi <-> -pi
			v -= pi;
		}
		return -(-16.f * v * (v - pi) / (4.f * v * (v - pi) + 49.3480220054468f));
	}
	if(v > pi) { // Cover domain 2*pi <-> pi
		v -= pi;
	}
	return -16.f * v * (v - pi) / (4 * v * (v - pi) + 49.3480220054468f);
	*/
}

inline float cos(float v) noexcept {
	return math::sin(pi_2 - v);
}

inline float acos(float v) noexcept {
	// Lagrange polynomial - https://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm
	// Maximum absolute error of 0.017
	assert(v >= -1.f && v <= 1.f);
	return std::acos(v);
	/*
	return ((0.4643653210307f * v * v * v + 0.921784152891457f * v * v - 2.0178302343512f * v - 0.939115566365855f) * v + 1.5707963267949f) / ((0.295624144969963f * v * v - 1.28459062446908f) * (v * v) + 1);
	*/
}

inline float sqrt(float x) noexcept {
	return std::sqrt(x);
	/*
	union {
		float f;
		int i;
	} u = {x};
	u.i = 0x5f375a86 - (u.i >> 1);
	u.f = u.f * (1.5f - (0.5f * x) * u.f * u.f);
	u.f = u.f * (1.5f - (0.5f * x) * u.f * u.f);
	return u.f * x;
	*/
}

}
