#pragma once

namespace ve {

template<typename T>
inline T select(bool v, T a, T b) {
	return v ? a : b;
}

}
