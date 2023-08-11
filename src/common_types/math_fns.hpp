#pragma once
#include <cmath>

namespace math {

// TODO: all this functions need to be defined in terms of
// basic floating point operations (+, -, *, /)

inline constexpr float pi = 3.14159265358979323846f;

inline constexpr float internal_check(float x, float err, float lower, float upper) noexcept {
	assert(err >= 0.f); // error must be positive
	assert(lower <= upper); // conflicting definitions
	assert(x + err >= lower && x - err <= upper); // unreasonable
	return (x < lower) ? lower : ((x > upper) ? upper : x);
}

inline float sin(float x) noexcept {
	// based on
	// https://web.archive.org/web/20200628195036/http://mooooo.ooo/chebyshev-sine-approximation/
	assert(x >= -2.f * pi && x <= 2.f * pi);
	if(x < -pi) {
		x += 2.f * pi;
	} else if(x > pi) {
		x -= 2.f * pi;
	}
    static const float coeffs[] = {
        -0.10132118f,          // x
         0.0066208798f,        // x^3
        -0.00017350505f,       // x^5
         0.0000025222919f,     // x^7
        -0.000000023317787f,   // x^9
         0.00000000013291342f, // x^11
	};
    float pi_major = 3.1415927f;
    float pi_minor = -0.00000008742278f;
    float x2 = x * x;
    float p11 = coeffs[5];
    float p9 = p11 * x2 + coeffs[4];
    float p7 = p9 * x2 + coeffs[3];
    float p5 = p7 * x2 + coeffs[2];
    float p3 = p5 * x2 + coeffs[1];
    float p1 = p3 * x2 + coeffs[0];
    float r = (x - pi_major - pi_minor) * (x + pi_major + pi_minor) * p1 * x;
	return internal_check(r, 0.0016f, -1.f, 1.f);
}

inline float cos(float x) noexcept {
	float r = math::sin(pi / 2.f - x);
	return internal_check(r, 0.0016f, -1.f, 1.f);
}

inline float acos(float x) noexcept {
	// Lagrange polynomial - https://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm
	// Maximum absolute error of 0.017
	constexpr float acos_input_err = 0.001f;
	assert(x >= -1.f - acos_input_err && x <= 1.f + acos_input_err);
	if(x >= 1.f) { // clamp max
		assert(x <= 1.f + acos_input_err);
		return 0.f;
	} else if(x <= -1.f) { // clamp min
		assert(x >= -1.f - acos_input_err);
		return pi;
	}
	float r = ((0.4643653210307f * x * x * x + 0.921784152891457f * x * x - 2.0178302343512f * x - 0.939115566365855f) * x + 1.5707963267949f) / ((0.295624144969963f * x * x - 1.28459062446908f) * (x * x) + 1.f);
	assert((x >= 1.f) ? r == 0.f : r > 0.f); //we need not to give 0 on distances
	return internal_check(r, 0.017f, 0.f, pi);
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
