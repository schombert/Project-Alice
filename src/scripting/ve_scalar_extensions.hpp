#pragma once

namespace ve {

template<typename T>
inline T select(bool v, T a, T b) { return v ? a : b; }

inline float to_float(int32_t a) { return float(a); }
inline float to_float(uint32_t a) { return float(a); }
inline float to_float(uint16_t a) { return float(a); }
inline float to_float(int16_t a) { return float(a); }
inline float to_float(uint8_t a) { return float(a); }
inline float to_float(int8_t a) { return float(a); }

inline int32_t to_int(int32_t a) { return int32_t(a); }
inline int32_t to_int(uint32_t a) { return int32_t(a); }
inline int32_t to_int(uint16_t a) { return int32_t(a); }
inline int32_t to_int(int16_t a) { return int32_t(a); }
inline int32_t to_int(uint8_t a) { return int32_t(a); }
inline int32_t to_int(int8_t a) { return int32_t(a); }
inline int32_t to_int(float a) { return int32_t(a); }

inline bool compress_mask(bool v) { return v; }

template<typename tag_type>
mask_vector operator==(contiguous_tags_base<tag_type> a, tag_type b) {
	return a == tagged_vector<tag_type>(b);
}
template<typename tag_type>
mask_vector operator!=(contiguous_tags_base<tag_type> a, tag_type b) {
	return a != tagged_vector<tag_type>(b);
}

template<typename tag_type>
mask_vector operator==(tag_type b, contiguous_tags_base<tag_type> a) {
	return a == tagged_vector<tag_type>(b);
}
template<typename tag_type>
mask_vector operator!=(tag_type b, contiguous_tags_base<tag_type> a) {
	return a != tagged_vector<tag_type>(b);
}

} // namespace ve
