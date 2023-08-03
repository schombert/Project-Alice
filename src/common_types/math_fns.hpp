#pragma once
#include <cmath>

namespace math {

// TODO: all this functions need to be defined in terms of
// basic floating point operations (+, -, *, /)

inline constexpr float pi = 3.14159265358979323846f;

inline float acos(float v) noexcept { return std::acos(v); }

inline float sin(float v) noexcept { return std::sin(v); }

inline float cos(float v) noexcept { return std::cos(v); }

inline float sqrt(float v) noexcept { return std::sqrt(v); }

} // namespace math
