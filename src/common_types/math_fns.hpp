#pragma once
#include <cmath>

namespace math {

// TODO: all this functions need to be defined in terms of
// basic floating point operations (+, -, *, /)

inline constexpr float pi = 3.14159265358979323846f;

inline float sin(float v) noexcept {
	// Bhaskara formula, expanded for -2*pi <-> pi, error of 0.0016
	assert(v >= -2.f * pi && v <= 2.f * pi);
	if(v < 0.f) { // Negative domain, simply flip the sign of the result and input :D
		v = -v;
		if(v < -pi) { // Cover domain -2*pi <-> -pi
			v -= pi;
		}
		return -(-16 * v * (v - pi) / (4 * v * (v - pi) + 49.3480220054468));
	}
	if(v > pi) { // Cover domain 2*pi <-> pi
		v -= pi;
	}
	return -16 * v * (v - pi) / (4 * v * (v - pi) + 49.3480220054468);
}

inline float cos(float v) noexcept {
	// Bhaskara formula, expanded for -2*pi <-> pi, error of 0.0016
	assert(v >= -2.f * pi && v <= 2.f * pi);
	if(v < 0.f) { // Negative domain, simply flip the sign of the result and input :D
		v = -v;
		if(v < -pi) { // Cover domain -2*pi <-> -pi
			v -= pi;
		}
		return -((39.4784176043574 - 16.0 * v * v) / (4.0 * v * v + 39.4784176043574));
	}
	if(v > pi) { // Cover domain 2*pi <-> pi
		v -= pi;
	}
	return (39.4784176043574 - 16.0 * v * v) / (4.0 * v * v + 39.4784176043574);
}

inline float acos(float v) noexcept {
	// Lagrange polynomial - https://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm
	// Maximum absolute error of 0.017
	assert(v >= -1.f && v <= 1.f);
	return ((0.4643653210307 * v * v * v + 0.921784152891457 * v * v - 2.0178302343512 * v - 0.939115566365855) * v + 1.5707963267949) / ((0.295624144969963 * v * v - 1.28459062446908) * (v * v) + 1);
}

inline float sqrt(float v) noexcept {
	assert(v >= 0.f);
	union {
		float f;
		uint32_t i;
	} u = {v};
	static_assert(sizeof(float) == sizeof(uint32_t));
	u.i = 0x1FBB4F2E + (u.i >> 1);
	return u.f;
}

}
