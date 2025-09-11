#pragma once

#ifdef NDEBUG
#ifdef _MSC_VER
#define RELEASE_INLINE __forceinline
#else
#define RELEASE_INLINE inline __attribute__((always_inline))
#endif
#else
#define RELEASE_INLINE inline
#endif

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
RELEASE_INLINE VALUE min(const VALUE a, const VALUE b) {
	return std::min(a, b);
}

template<typename VALUE>
RELEASE_INLINE VALUE min(const VALUE a, const VALUE b) {
	return ve::min(a, b);
}

template<typename VALUE> requires has_default_order<VALUE>
RELEASE_INLINE VALUE max(const VALUE a, const VALUE b) {
	return std::max(a, b);
}

template<typename VALUE>
RELEASE_INLINE VALUE max(const VALUE a, const VALUE b) {
	return ve::max(a, b);
}

template<typename VALUE> requires abs_is_defined<VALUE>
RELEASE_INLINE VALUE abs(const VALUE a) {
	return std::abs(a);
}

template<typename VALUE>
RELEASE_INLINE VALUE abs(const VALUE a) {
	return ve::abs(a);
}

template<typename MASK, typename VALUE> requires is_vector<MASK> && is_vector<VALUE>
RELEASE_INLINE VALUE select(const MASK mask, const VALUE a, const VALUE b) {
	return ve::select(mask, a, b);
}

template<typename MASK, typename VALUE> requires is_vector<MASK>
RELEASE_INLINE VALUE select(const MASK mask, const VALUE a, const VALUE b) {
	return ve::select(mask, VALUE{ a }, VALUE{ b });
}

template<typename MASK, typename VALUE> requires std::same_as<MASK, bool>
RELEASE_INLINE VALUE select(const MASK mask, const VALUE a, const VALUE b) {
	return mask ? a : b;
}

template<typename MASK, typename VALUE>
RELEASE_INLINE VALUE select(const MASK mask, const VALUE a, const VALUE b) {
	return ve::select<VALUE::wrapped_value>(mask, a, b);
}

}
