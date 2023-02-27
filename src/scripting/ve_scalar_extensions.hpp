#pragma once

namespace ve {

template<typename T>
inline T select(bool v, T a, T b) {
	return v ? a : b;
}

inline float to_float(int32_t a) {
	return float(a);
}
inline float to_float(uint32_t a) {
	return float(a);
}
inline float to_float(uint16_t a) {
	return float(a);
}
inline float to_float(int16_t a) {
	return float(a);
}
inline float to_float(uint8_t a) {
	return float(a);
}
inline float to_float(int8_t a) {
	return float(a);
}

}
