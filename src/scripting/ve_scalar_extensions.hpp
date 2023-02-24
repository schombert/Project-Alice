#pragma once

namespace ve {

template<typename F>
auto apply(F const& func, int32_t a, int32_t b, int32_t c) {
	return func(a, b, c);
}

template<typename T>
inline T select(bool v, T a, T b) {
	return v ? a : b;
}

}
