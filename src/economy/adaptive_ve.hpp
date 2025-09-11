#pragma once

namespace adaptive_ve {

template<typename T>
concept has_default_order = requires (T x, T y) {
	{ x < y	} -> std::same_as<bool>;
};
template<typename T>
concept abs_is_defined = requires (T x) {
	std::abs(x);
};

template<typename T>
concept is_vector = (std::same_as<T, ve::fp_vector> || std::same_as<T, ve::int_vector> || std::same_as<T, ve::mask_vector>);

template<typename T>
using convert_to_float = std::conditional_t<ve::is_vector_type_s<T>::value, ve::fp_vector, float>;
template<typename T>
using convert_to_int = std::conditional_t<ve::is_vector_type_s<T>::value, ve::int_vector, int>;
template<typename T>
using convert_to_bool = std::conditional_t<ve::is_vector_type_s<T>::value, ve::mask_vector, bool>;

template<typename VALUE> requires has_default_order<VALUE>
VALUE min(const VALUE a, const VALUE b) {
	return std::min(a, b);
}

template<typename VALUE>
VALUE min(const VALUE a, const VALUE b) {
	return ve::min(a, b);
}

template<typename VALUE> requires has_default_order<VALUE>
VALUE max(const VALUE a, const VALUE b) {
	return std::max(a, b);
}

template<typename VALUE>
VALUE max(const VALUE a, const VALUE b) {
	return ve::max(a, b);
}

template<typename VALUE> requires abs_is_defined<VALUE>
VALUE abs(const VALUE a) {
	return std::abs(a);
}

template<typename VALUE>
VALUE abs(const VALUE a) {
	return ve::abs(a);
}

template<typename MASK, typename VALUE> requires is_vector<MASK> && is_vector<VALUE>
VALUE select(const MASK mask, const VALUE a, const VALUE b) {
	return ve::select(mask, a, b);
}

template<typename MASK, typename VALUE> requires is_vector<MASK>
VALUE select(const MASK mask, const VALUE a, const VALUE b) {
	return ve::select(mask, VALUE{ a }, VALUE{ b });
}

template<typename MASK, typename VALUE> requires std::same_as<MASK, bool>
VALUE select(const MASK mask, const VALUE a, const VALUE b) {
	return mask ? a : b;
}

template<typename MASK, typename VALUE>
VALUE select(const MASK mask, const VALUE a, const VALUE b) {
	return ve::select<VALUE::wrapped_value>(mask, a, b);
}

}
