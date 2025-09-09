#pragma once

namespace adaptive_ve {
template<typename VALUE>
VALUE min(VALUE a, VALUE b) {
	if constexpr(std::same_as<VALUE, float>) {
		return std::min(a, b);
	} else {
		return ve::min(a, b);
	}
}
template<typename VALUE>
VALUE max(VALUE a, VALUE b) {
	if constexpr(std::same_as<VALUE, float>) {
		return std::max(a, b);
	} else {
		return ve::max(a, b);
	}
}
}
