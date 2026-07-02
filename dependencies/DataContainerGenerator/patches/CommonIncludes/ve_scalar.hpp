#pragma once

//
// Scalar four-lane fallback backend for DataContainer ve
//

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace ve {
	constexpr int32_t vector_size = 4;

	struct int_vector;

	template<typename tag_type>
	struct tagged_vector;

	struct fp_vector;
	struct mask_vector;

	template<typename T>
	struct ve_identity {
		using type = T;
	};

	struct vbitfield_type {
		using storage = uint8_t;
		uint8_t v = 0;
	};

	RELEASE_INLINE vbitfield_type operator&(vbitfield_type a, vbitfield_type b) { return vbitfield_type{ uint8_t(a.v & b.v) }; }
	RELEASE_INLINE vbitfield_type operator|(vbitfield_type a, vbitfield_type b) { return vbitfield_type{ uint8_t(a.v | b.v) }; }
	RELEASE_INLINE vbitfield_type operator^(vbitfield_type a, vbitfield_type b) { return vbitfield_type{ uint8_t(a.v ^ b.v) }; }
	RELEASE_INLINE vbitfield_type operator~(vbitfield_type a) { return vbitfield_type{ uint8_t(~a.v) }; }
	RELEASE_INLINE vbitfield_type operator!(vbitfield_type a) { return vbitfield_type{ uint8_t(~a.v) }; }
	RELEASE_INLINE vbitfield_type and_not(vbitfield_type a, vbitfield_type b) { return vbitfield_type{ uint8_t(a.v & ~b.v) }; }
	RELEASE_INLINE vbitfield_type operator!=(vbitfield_type a, vbitfield_type b) { return vbitfield_type{ uint8_t(a.v ^ b.v) }; }
	RELEASE_INLINE vbitfield_type operator==(vbitfield_type a, vbitfield_type b) { return vbitfield_type{ uint8_t(~(a.v ^ b.v)) }; }

	struct mask_vector {
		using wrapped_value = bool;
		std::array<bool, 4> value{};

		RELEASE_INLINE mask_vector() = default;
		RELEASE_INLINE mask_vector(bool b) : value{ b, b, b, b } {}
		RELEASE_INLINE mask_vector(bool a, bool b, bool c, bool d) : value{ a, b, c, d } {}
		RELEASE_INLINE mask_vector(vbitfield_type b)
			: value{
				(b.v & 0x01) != 0,
				(b.v & 0x02) != 0,
				(b.v & 0x04) != 0,
				(b.v & 0x08) != 0
			} {}

		RELEASE_INLINE bool operator[](uint32_t i) const noexcept { return value[i]; }
		RELEASE_INLINE void set(uint32_t i, bool v) noexcept { value[i] = v; }
		RELEASE_INLINE operator vbitfield_type() const noexcept {
			return vbitfield_type{
				uint8_t((value[0] ? 0x01 : 0x00) |
					(value[1] ? 0x02 : 0x00) |
					(value[2] ? 0x04 : 0x00) |
					(value[3] ? 0x08 : 0x00))
			};
		}
	};

	struct fp_vector {
		using wrapped_value = float;
		std::array<float, 4> value{};

		RELEASE_INLINE fp_vector() = default;
		RELEASE_INLINE fp_vector(float v) : value{ v, v, v, v } {}
		RELEASE_INLINE fp_vector(float a, float b, float c, float d) : value{ a, b, c, d } {}

		RELEASE_INLINE float reduce() const { return value[0] + value[1] + value[2] + value[3]; }
		RELEASE_INLINE float operator[](uint32_t i) const noexcept { return value[i]; }
		RELEASE_INLINE void set(uint32_t i, float v) noexcept { value[i] = v; }
	};

	struct int_vector {
		using wrapped_value = int32_t;
		std::array<int32_t, 4> value{};

		RELEASE_INLINE int_vector() = default;
		RELEASE_INLINE int_vector(int32_t v) : value{ v, v, v, v } {}
		RELEASE_INLINE int_vector(uint32_t v) : value{ int32_t(v), int32_t(v), int32_t(v), int32_t(v) } {}
		RELEASE_INLINE int_vector(int32_t a, int32_t b, int32_t c, int32_t d) : value{ a, b, c, d } {}
		RELEASE_INLINE int_vector(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
			: value{ int32_t(a), int32_t(b), int32_t(c), int32_t(d) } {}

		RELEASE_INLINE int32_t operator[](uint32_t i) const noexcept { return value[i]; }
		RELEASE_INLINE void set(uint32_t i, int32_t v) noexcept { value[i] = v; }
	};

	template<typename tag_type>
	struct tagged_vector {
		using wrapped_value = tag_type;
		static_assert(sizeof(typename tag_type::value_base_t) <= 4);

		int_vector value = int_vector(-1);

		RELEASE_INLINE tagged_vector() = default;
		RELEASE_INLINE explicit tagged_vector(int_vector v)
			: value(tag_type::zero_is_null_t::value
				? int_vector(v[0] - 1, v[1] - 1, v[2] - 1, v[3] - 1)
				: v) {}
		RELEASE_INLINE tagged_vector(tag_type v)
			: value(tag_type::zero_is_null_t::value ? int_vector(v.index() - 1) : int_vector(v.index())) {}
		RELEASE_INLINE tagged_vector(tag_type a, tag_type b, tag_type c, tag_type d)
			: value(
				tag_type::zero_is_null_t::value ? a.index() - 1 : a.index(),
				tag_type::zero_is_null_t::value ? b.index() - 1 : b.index(),
				tag_type::zero_is_null_t::value ? c.index() - 1 : c.index(),
				tag_type::zero_is_null_t::value ? d.index() - 1 : d.index()) {}
		RELEASE_INLINE tagged_vector(int_vector v, std::true_type) : value(v) {}
		template<typename other_tag_type>
		RELEASE_INLINE tagged_vector(tagged_vector<other_tag_type> v, std::true_type) : value(v.value) {}

		RELEASE_INLINE int_vector to_original_values() const {
			return tag_type::zero_is_null_t::value ? value + int_vector(1) : value;
		}
		RELEASE_INLINE operator int_vector() const { return value; }

		RELEASE_INLINE tag_type operator[](uint32_t i) const noexcept {
			if constexpr(tag_type::zero_is_null_t::value) {
				tag_type r;
				r.value = typename tag_type::value_base_t(value[i] + 1);
				return r;
			} else {
				return tag_type(typename tag_type::value_base_t(value[i]));
			}
		}

		RELEASE_INLINE void set(uint32_t i, tag_type v) noexcept {
			value.set(i, tag_type::zero_is_null_t::value ? v.index() - 1 : v.index());
		}
	};

	template<>
	struct tagged_vector<int32_t> {
		using wrapped_value = int32_t;
		int_vector value = int_vector(-1);

		RELEASE_INLINE tagged_vector() = default;
		RELEASE_INLINE explicit tagged_vector(int_vector v) : value(v) {}
		RELEASE_INLINE tagged_vector(int32_t v) : value(v) {}
		RELEASE_INLINE tagged_vector(int32_t a, int32_t b, int32_t c, int32_t d) : value(a, b, c, d) {}
		RELEASE_INLINE tagged_vector(int_vector v, std::true_type) : value(v) {}
		template<typename other_tag_type>
		RELEASE_INLINE tagged_vector(tagged_vector<other_tag_type> v, std::true_type) : value(v.value) {}

		RELEASE_INLINE int_vector to_original_values() const { return value; }
		RELEASE_INLINE operator int_vector() const { return value; }
		RELEASE_INLINE int32_t operator[](uint32_t i) const noexcept { return value[i]; }
		RELEASE_INLINE void set(uint32_t i, int32_t v) noexcept { value.set(i, v); }
	};

	template<typename tag_type>
	struct contiguous_tags_base {
		uint32_t value = 0;
		using wrapped_value = tag_type;

		constexpr contiguous_tags_base() = default;
		constexpr explicit contiguous_tags_base(uint32_t v) : value(v) {}
		constexpr contiguous_tags_base(contiguous_tags_base const&) noexcept = default;
		constexpr contiguous_tags_base(contiguous_tags_base&&) noexcept = default;

		template<typename T, typename = std::enable_if_t<std::is_constructible_v<tag_type, T> && !std::is_same_v<tag_type, T>>>
		constexpr contiguous_tags_base(contiguous_tags_base<T> v) : value(v.value) {}

		contiguous_tags_base& operator=(contiguous_tags_base&&) noexcept = default;
		contiguous_tags_base& operator=(contiguous_tags_base const&) noexcept = default;

		template<typename T>
		std::enable_if_t<std::is_constructible_v<tag_type, T> && !std::is_same_v<tag_type, T>, contiguous_tags_base&> operator=(contiguous_tags_base<T> v) noexcept {
			value = v.value;
			return *this;
		}

		RELEASE_INLINE tag_type operator[](uint32_t i) const noexcept { return tag_type(typename tag_type::value_base_t(value + i)); }
		constexpr bool operator==(contiguous_tags_base<tag_type> o) const noexcept { return value == o.value; }
		constexpr bool operator!=(contiguous_tags_base<tag_type> o) const noexcept { return value != o.value; }
	};

	template<>
	struct contiguous_tags_base<int32_t> {
		uint32_t value = 0;
		using tag_type = int32_t;
		using wrapped_value = tag_type;

		constexpr contiguous_tags_base() = default;
		constexpr explicit contiguous_tags_base(uint32_t v) : value(v) {}
		constexpr contiguous_tags_base(contiguous_tags_base const&) noexcept = default;
		constexpr contiguous_tags_base(contiguous_tags_base&&) noexcept = default;

		template<typename T, typename = std::enable_if_t<std::is_constructible_v<tag_type, T> && !std::is_same_v<tag_type, T>>>
		constexpr contiguous_tags_base(contiguous_tags_base<T> v) : value(v.value) {}

		contiguous_tags_base& operator=(contiguous_tags_base&&) noexcept = default;
		contiguous_tags_base& operator=(contiguous_tags_base const&) noexcept = default;

		template<typename T>
		std::enable_if_t<std::is_constructible_v<tag_type, T> && !std::is_same_v<tag_type, T>, contiguous_tags_base&> operator=(contiguous_tags_base<T> v) noexcept {
			value = v.value;
			return *this;
		}

		RELEASE_INLINE tag_type operator[](uint32_t i) const noexcept { return int32_t(value + i); }
		constexpr bool operator==(contiguous_tags_base<tag_type> o) const noexcept { return value == o.value; }
		constexpr bool operator!=(contiguous_tags_base<tag_type> o) const noexcept { return value != o.value; }
	};

	template<typename tag_type>
	struct unaligned_contiguous_tags : public contiguous_tags_base<tag_type> {
		constexpr unaligned_contiguous_tags() = default;
		constexpr explicit unaligned_contiguous_tags(uint32_t v) : contiguous_tags_base<tag_type>(v) {}
		constexpr unaligned_contiguous_tags(unaligned_contiguous_tags const&) noexcept = default;
		constexpr unaligned_contiguous_tags(unaligned_contiguous_tags&&) noexcept = default;

		template<typename T, typename = std::enable_if_t<std::is_constructible_v<tag_type, T> && !std::is_same_v<tag_type, T>>>
		constexpr unaligned_contiguous_tags(unaligned_contiguous_tags<T> v) : contiguous_tags_base<tag_type>(v.value) {}
	};

	template<typename tag_type>
	struct contiguous_tags : public contiguous_tags_base<tag_type> {
		constexpr contiguous_tags() = default;
		constexpr explicit contiguous_tags(uint32_t v) : contiguous_tags_base<tag_type>(v) {}
		constexpr contiguous_tags(contiguous_tags const&) noexcept = default;
		constexpr contiguous_tags(contiguous_tags&&) noexcept = default;

		template<typename T, typename = std::enable_if_t<std::is_constructible_v<tag_type, T> && !std::is_same_v<tag_type, T>>>
		constexpr contiguous_tags(contiguous_tags<T> v) : contiguous_tags_base<tag_type>(v.value) {}
	};

	template<typename tag_type>
	struct partial_contiguous_tags {
		using wrapped_value = tag_type;
		uint32_t value = 0;
		uint32_t subcount = vector_size;

		constexpr partial_contiguous_tags() = default;
		constexpr explicit partial_contiguous_tags(uint32_t v, uint32_t s) : value(v), subcount(s) {}
		constexpr partial_contiguous_tags(partial_contiguous_tags const&) noexcept = default;
		constexpr partial_contiguous_tags(partial_contiguous_tags&&) noexcept = default;

		template<typename T, typename = std::enable_if_t<std::is_constructible_v<tag_type, T> && !std::is_same_v<tag_type, T>>>
		constexpr partial_contiguous_tags(partial_contiguous_tags<T> v) : value(v.value), subcount(v.subcount) {}

		RELEASE_INLINE tag_type operator[](uint32_t i) const noexcept { return tag_type(typename tag_type::value_base_t(value + i)); }
	};

	template<typename T>
	struct value_to_vector_type_s;

	template<> struct value_to_vector_type_s<int32_t> { using type = int_vector; };
	template<> struct value_to_vector_type_s<int16_t> { using type = int_vector; };
	template<> struct value_to_vector_type_s<uint16_t> { using type = int_vector; };
	template<> struct value_to_vector_type_s<int8_t> { using type = int_vector; };
	template<> struct value_to_vector_type_s<uint8_t> { using type = int_vector; };
	template<> struct value_to_vector_type_s<void> { using type = void; };
	template<> struct value_to_vector_type_s<uint32_t> { using type = int_vector; };
	template<> struct value_to_vector_type_s<float> { using type = fp_vector; };
	template<> struct value_to_vector_type_s<bool> { using type = mask_vector; };
	template<> struct value_to_vector_type_s<int_vector> { using type = int_vector; };
	template<> struct value_to_vector_type_s<fp_vector> { using type = fp_vector; };
	template<> struct value_to_vector_type_s<mask_vector> { using type = mask_vector; };
	template<typename T> struct value_to_vector_type_s<tagged_vector<T>> { using type = tagged_vector<T>; };
	template<typename T> struct value_to_vector_type_s<contiguous_tags<T>> { using type = contiguous_tags<T>; };
	template<typename T> struct value_to_vector_type_s<unaligned_contiguous_tags<T>> { using type = unaligned_contiguous_tags<T>; };
	template<typename T> struct value_to_vector_type_s<partial_contiguous_tags<T>> { using type = partial_contiguous_tags<T>; };

	template<typename T>
	using value_to_vector_type = typename value_to_vector_type_s<T>::type;

	template<typename T>
	struct is_vector_type_s { constexpr static bool value = false; };
	template<> struct is_vector_type_s<int_vector> { constexpr static bool value = true; };
	template<> struct is_vector_type_s<fp_vector> { constexpr static bool value = true; };
	template<> struct is_vector_type_s<mask_vector> { constexpr static bool value = true; };
	template<typename T> struct is_vector_type_s<tagged_vector<T>> { constexpr static bool value = true; };
	template<typename T> struct is_vector_type_s<contiguous_tags<T>> { constexpr static bool value = true; };
	template<typename T> struct is_vector_type_s<unaligned_contiguous_tags<T>> { constexpr static bool value = true; };
	template<typename T> struct is_vector_type_s<partial_contiguous_tags<T>> { constexpr static bool value = true; };

	template<typename T>
	constexpr bool is_vector_type = is_vector_type_s<T>::value;

	template<typename... T>
	struct any_is_vector_type;
	template<> struct any_is_vector_type<> { constexpr static bool value = false; };
	template<typename F, typename... R>
	struct any_is_vector_type<F, R...> { constexpr static bool value = is_vector_type<F> || any_is_vector_type<R...>::value; };

	template<typename TO, typename FROM>
	RELEASE_INLINE auto widen_to(FROM v) -> std::conditional_t<is_vector_type<TO>, value_to_vector_type<std::remove_cv_t<FROM>>, FROM> { return v; }
	template<typename TO, typename... REST, typename FROM>
	RELEASE_INLINE auto widen_to(FROM v) -> std::conditional_t<is_vector_type<TO>, value_to_vector_type<std::remove_cv_t<FROM>>, decltype(widen_to<REST...>(v))> { return v; }

	template<uint32_t i, typename T>
	RELEASE_INLINE std::enable_if_t<is_vector_type<T>, typename T::wrapped_value> nth_item(T v) { return v[i]; }
	template<uint32_t i, typename T>
	RELEASE_INLINE std::enable_if_t<!is_vector_type<T>, T> nth_item(T v) { return v; }

	template<typename... T>
	struct any_is_partial_s;
	template<> struct any_is_partial_s<> { constexpr static bool value = false; };
	template<typename ttype, typename... T>
	struct any_is_partial_s<partial_contiguous_tags<ttype>, T...> { constexpr static bool value = true; };
	template<typename first, typename... T>
	struct any_is_partial_s<first, T...> { constexpr static bool value = any_is_partial_s<T...>::value; };

	template<typename... T>
	constexpr bool any_is_partial = any_is_partial_s<T...>::value;

	RELEASE_INLINE constexpr uint32_t minimum_partial() { return uint32_t(vector_size); }
	template<typename ttype, typename... T>
	RELEASE_INLINE uint32_t minimum_partial(partial_contiguous_tags<ttype> p, T... args) { return std::min(p.subcount, minimum_partial(args...)); }
	template<typename first, typename... T>
	RELEASE_INLINE uint32_t minimum_partial(first, T... args) { return minimum_partial(args...); }

	template<typename FUNC, typename... PARAMS>
	RELEASE_INLINE auto apply(FUNC&& f, PARAMS... params) {
		if constexpr(any_is_partial<PARAMS...>) {
			const uint32_t limit = minimum_partial(params...);
			if constexpr(std::is_same_v<decltype(f(nth_item<uint32_t(0)>(params)...)), void>) {
				switch(limit) {
				default:;
				case 4: f(nth_item<uint32_t(3)>(params)...);
				case 3: f(nth_item<uint32_t(2)>(params)...);
				case 2: f(nth_item<uint32_t(1)>(params)...);
				case 1: f(nth_item<uint32_t(0)>(params)...);
				case 0: break;
				}
			} else {
				value_to_vector_type<decltype(f(nth_item<uint32_t(0)>(params)...))> temp;
				switch(limit) {
				default:;
				case 4: temp.set(3, f(nth_item<uint32_t(3)>(params)...));
				case 3: temp.set(2, f(nth_item<uint32_t(2)>(params)...));
				case 2: temp.set(1, f(nth_item<uint32_t(1)>(params)...));
				case 1: temp.set(0, f(nth_item<uint32_t(0)>(params)...));
				case 0: break;
				}
				return temp;
			}
		} else if constexpr(any_is_vector_type<PARAMS...>::value) {
			if constexpr(std::is_same_v<decltype(f(nth_item<uint32_t(0)>(params)...)), void>) {
				f(nth_item<uint32_t(0)>(params)...);
				f(nth_item<uint32_t(1)>(params)...);
				f(nth_item<uint32_t(2)>(params)...);
				f(nth_item<uint32_t(3)>(params)...);
			} else {
				return value_to_vector_type<decltype(f(nth_item<uint32_t(0)>(params)...))>(
					f(nth_item<uint32_t(0)>(params)...),
					f(nth_item<uint32_t(1)>(params)...),
					f(nth_item<uint32_t(2)>(params)...),
					f(nth_item<uint32_t(3)>(params)...));
			}
		} else {
			return f(params...);
		}
	}

	template<typename FUNC, typename... PARAMS>
	RELEASE_INLINE auto apply_with_indices(FUNC&& f, PARAMS... params)
		-> value_to_vector_type<decltype(f(uint32_t(0), nth_item<uint32_t(0)>(params)...))> {
		if constexpr(any_is_partial<PARAMS...>) {
			const uint32_t limit = minimum_partial(params...);
			if constexpr(std::is_same_v<decltype(f(uint32_t(0), nth_item<uint32_t(0)>(params)...)), void>) {
				switch(limit) {
				default:;
				case 4: f(uint32_t(3), nth_item<uint32_t(3)>(params)...);
				case 3: f(uint32_t(2), nth_item<uint32_t(2)>(params)...);
				case 2: f(uint32_t(1), nth_item<uint32_t(1)>(params)...);
				case 1: f(uint32_t(0), nth_item<uint32_t(0)>(params)...);
				case 0: break;
				}
			} else {
				value_to_vector_type<decltype(f(uint32_t(0), nth_item<uint32_t(0)>(params)...))> temp;
				switch(limit) {
				default:;
				case 4: temp.set(3, f(uint32_t(3), nth_item<uint32_t(3)>(params)...));
				case 3: temp.set(2, f(uint32_t(2), nth_item<uint32_t(2)>(params)...));
				case 2: temp.set(1, f(uint32_t(1), nth_item<uint32_t(1)>(params)...));
				case 1: temp.set(0, f(uint32_t(0), nth_item<uint32_t(0)>(params)...));
				case 0: break;
				}
				return temp;
			}
		} else if constexpr(std::is_same_v<decltype(f(uint32_t(0), nth_item<uint32_t(0)>(params)...)), void>) {
			f(uint32_t(0), nth_item<uint32_t(0)>(params)...);
			f(uint32_t(1), nth_item<uint32_t(1)>(params)...);
			f(uint32_t(2), nth_item<uint32_t(2)>(params)...);
			f(uint32_t(3), nth_item<uint32_t(3)>(params)...);
		} else {
			return value_to_vector_type<decltype(f(uint32_t(0), nth_item<uint32_t(0)>(params)...))>(
				f(uint32_t(0), nth_item<uint32_t(0)>(params)...),
				f(uint32_t(1), nth_item<uint32_t(1)>(params)...),
				f(uint32_t(2), nth_item<uint32_t(2)>(params)...),
				f(uint32_t(3), nth_item<uint32_t(3)>(params)...));
		}
	}

	RELEASE_INLINE fp_vector to_float(int_vector v) {
		return fp_vector(float(v[0]), float(v[1]), float(v[2]), float(v[3]));
	}
	RELEASE_INLINE int_vector to_int(fp_vector v) {
		return int_vector(
			int32_t(std::lrintf(v[0])),
			int32_t(std::lrintf(v[1])),
			int32_t(std::lrintf(v[2])),
			int32_t(std::lrintf(v[3])));
	}

	RELEASE_INLINE fp_vector operator+(fp_vector a, fp_vector b) { return fp_vector(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]); }
	RELEASE_INLINE fp_vector operator-(fp_vector a, fp_vector b) { return fp_vector(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]); }
	RELEASE_INLINE fp_vector operator*(fp_vector a, fp_vector b) { return fp_vector(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]); }
	RELEASE_INLINE fp_vector operator/(fp_vector a, fp_vector b) { return fp_vector(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]); }
	RELEASE_INLINE fp_vector operator-(fp_vector a) { return fp_vector() - a; }

	RELEASE_INLINE int_vector operator+(int_vector a, int_vector b) { return int_vector(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]); }
	RELEASE_INLINE int_vector operator-(int_vector a, int_vector b) { return int_vector(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]); }
	RELEASE_INLINE int_vector operator*(int_vector a, int_vector b) {
		return int_vector(
			int32_t(int64_t(a[0]) * int64_t(b[0])),
			int32_t(int64_t(a[1]) * int64_t(b[1])),
			int32_t(int64_t(a[2]) * int64_t(b[2])),
			int32_t(int64_t(a[3]) * int64_t(b[3])));
	}
	RELEASE_INLINE int_vector operator-(int_vector a) { return int_vector() - a; }
	RELEASE_INLINE int_vector operator|(int_vector a, int_vector b) { return int_vector(a[0] | b[0], a[1] | b[1], a[2] | b[2], a[3] | b[3]); }
	RELEASE_INLINE int_vector operator&(int_vector a, int_vector b) { return int_vector(a[0] & b[0], a[1] & b[1], a[2] & b[2], a[3] & b[3]); }
	RELEASE_INLINE int_vector operator^(int_vector a, int_vector b) { return int_vector(a[0] ^ b[0], a[1] ^ b[1], a[2] ^ b[2], a[3] ^ b[3]); }
	RELEASE_INLINE int_vector operator~(int_vector a) { return int_vector(~a[0], ~a[1], ~a[2], ~a[3]); }

	RELEASE_INLINE mask_vector operator&(mask_vector a, mask_vector b) { return mask_vector(a[0] && b[0], a[1] && b[1], a[2] && b[2], a[3] && b[3]); }
	RELEASE_INLINE mask_vector operator|(mask_vector a, mask_vector b) { return mask_vector(a[0] || b[0], a[1] || b[1], a[2] || b[2], a[3] || b[3]); }
	RELEASE_INLINE mask_vector operator^(mask_vector a, mask_vector b) { return mask_vector(a[0] != b[0], a[1] != b[1], a[2] != b[2], a[3] != b[3]); }
	RELEASE_INLINE mask_vector operator~(mask_vector a) { return mask_vector(!a[0], !a[1], !a[2], !a[3]); }
	RELEASE_INLINE mask_vector operator!(mask_vector a) { return ~a; }
	RELEASE_INLINE mask_vector and_not(mask_vector a, mask_vector b) { return mask_vector(a[0] && !b[0], a[1] && !b[1], a[2] && !b[2], a[3] && !b[3]); }
	RELEASE_INLINE mask_vector operator&&(mask_vector a, mask_vector b) { return a & b; }
	RELEASE_INLINE mask_vector operator||(mask_vector a, mask_vector b) { return a | b; }

	RELEASE_INLINE fp_vector inverse(fp_vector a) { return fp_vector(1.0f / a[0], 1.0f / a[1], 1.0f / a[2], 1.0f / a[3]); }
	RELEASE_INLINE fp_vector sqrt(fp_vector a) { return fp_vector(std::sqrt(a[0]), std::sqrt(a[1]), std::sqrt(a[2]), std::sqrt(a[3])); }
	RELEASE_INLINE fp_vector inverse_sqrt(fp_vector a) { return fp_vector(1.0f / std::sqrt(a[0]), 1.0f / std::sqrt(a[1]), 1.0f / std::sqrt(a[2]), 1.0f / std::sqrt(a[3])); }

	RELEASE_INLINE int_vector abs(int_vector a) { return int_vector(std::abs(a[0]), std::abs(a[1]), std::abs(a[2]), std::abs(a[3])); }
	RELEASE_INLINE fp_vector multiply_and_add(fp_vector a, fp_vector b, fp_vector c) { return (a * b) + c; }
	RELEASE_INLINE fp_vector multiply_and_subtract(fp_vector a, fp_vector b, fp_vector c) { return (a * b) - c; }
	RELEASE_INLINE fp_vector negate_multiply_and_add(fp_vector a, fp_vector b, fp_vector c) { return c - (a * b); }
	RELEASE_INLINE fp_vector negate_multiply_and_subtract(fp_vector a, fp_vector b, fp_vector c) { return (fp_vector(0.0f) - c) - (a * b); }

	RELEASE_INLINE fp_vector select(mask_vector mask, fp_vector a, fp_vector b) {
		return fp_vector(mask[0] ? a[0] : b[0], mask[1] ? a[1] : b[1], mask[2] ? a[2] : b[2], mask[3] ? a[3] : b[3]);
	}
	RELEASE_INLINE int_vector select(mask_vector mask, int_vector a, int_vector b) {
		return int_vector(mask[0] ? a[0] : b[0], mask[1] ? a[1] : b[1], mask[2] ? a[2] : b[2], mask[3] ? a[3] : b[3]);
	}
	template<typename T>
	RELEASE_INLINE tagged_vector<T> select(mask_vector mask, tagged_vector<T> a, tagged_vector<typename ve_identity<T>::type> b) {
		return tagged_vector<T>(
			mask[0] ? a[0] : b[0],
			mask[1] ? a[1] : b[1],
			mask[2] ? a[2] : b[2],
			mask[3] ? a[3] : b[3]);
	}

	RELEASE_INLINE fp_vector min(fp_vector a, fp_vector b) {
		return fp_vector(std::min(a[0], b[0]), std::min(a[1], b[1]), std::min(a[2], b[2]), std::min(a[3], b[3]));
	}
	RELEASE_INLINE fp_vector max(fp_vector a, fp_vector b) {
		return fp_vector(std::max(a[0], b[0]), std::max(a[1], b[1]), std::max(a[2], b[2]), std::max(a[3], b[3]));
	}
	RELEASE_INLINE fp_vector abs(fp_vector a) { return ve::max(a, -a); }
	RELEASE_INLINE fp_vector floor(fp_vector a) { return fp_vector(std::floor(a[0]), std::floor(a[1]), std::floor(a[2]), std::floor(a[3])); }
	RELEASE_INLINE fp_vector ceil(fp_vector a) { return fp_vector(std::ceil(a[0]), std::ceil(a[1]), std::ceil(a[2]), std::ceil(a[3])); }

	RELEASE_INLINE mask_vector operator<(fp_vector a, fp_vector b) { return mask_vector(a[0] < b[0], a[1] < b[1], a[2] < b[2], a[3] < b[3]); }
	RELEASE_INLINE mask_vector operator>(fp_vector a, fp_vector b) { return mask_vector(a[0] > b[0], a[1] > b[1], a[2] > b[2], a[3] > b[3]); }
	RELEASE_INLINE mask_vector operator<=(fp_vector a, fp_vector b) { return mask_vector(a[0] <= b[0], a[1] <= b[1], a[2] <= b[2], a[3] <= b[3]); }
	RELEASE_INLINE mask_vector operator>=(fp_vector a, fp_vector b) { return mask_vector(a[0] >= b[0], a[1] >= b[1], a[2] >= b[2], a[3] >= b[3]); }
	RELEASE_INLINE mask_vector operator==(fp_vector a, fp_vector b) { return mask_vector(a[0] == b[0], a[1] == b[1], a[2] == b[2], a[3] == b[3]); }
	RELEASE_INLINE mask_vector operator!=(fp_vector a, fp_vector b) { return mask_vector(a[0] != b[0], a[1] != b[1], a[2] != b[2], a[3] != b[3]); }

	RELEASE_INLINE mask_vector operator<(int_vector a, int_vector b) { return mask_vector(a[0] < b[0], a[1] < b[1], a[2] < b[2], a[3] < b[3]); }
	RELEASE_INLINE mask_vector operator>(int_vector a, int_vector b) { return mask_vector(a[0] > b[0], a[1] > b[1], a[2] > b[2], a[3] > b[3]); }
	RELEASE_INLINE mask_vector operator==(int_vector a, int_vector b) { return mask_vector(a[0] == b[0], a[1] == b[1], a[2] == b[2], a[3] == b[3]); }
	RELEASE_INLINE mask_vector operator!=(int_vector a, int_vector b) { return mask_vector(a[0] != b[0], a[1] != b[1], a[2] != b[2], a[3] != b[3]); }
	RELEASE_INLINE mask_vector operator<=(int_vector a, int_vector b) { return mask_vector(a[0] <= b[0], a[1] <= b[1], a[2] <= b[2], a[3] <= b[3]); }
	RELEASE_INLINE mask_vector operator>=(int_vector a, int_vector b) { return mask_vector(a[0] >= b[0], a[1] >= b[1], a[2] >= b[2], a[3] >= b[3]); }
	RELEASE_INLINE mask_vector operator==(mask_vector a, mask_vector b) { return mask_vector(a[0] == b[0], a[1] == b[1], a[2] == b[2], a[3] == b[3]); }
	RELEASE_INLINE mask_vector operator!=(mask_vector a, mask_vector b) { return mask_vector(a[0] != b[0], a[1] != b[1], a[2] != b[2], a[3] != b[3]); }

	template<typename T>
	RELEASE_INLINE mask_vector operator==(tagged_vector<T> a, tagged_vector<T> b) { return a.value == b.value; }
	template<typename T>
	RELEASE_INLINE mask_vector operator!=(tagged_vector<T> a, tagged_vector<T> b) { return a.value != b.value; }
	template<typename T>
	RELEASE_INLINE mask_vector operator==(tagged_vector<T> a, typename ve_identity<T>::type b) { return a == tagged_vector<T>(b); }
	template<typename T>
	RELEASE_INLINE mask_vector operator!=(tagged_vector<T> a, typename ve_identity<T>::type b) { return a != tagged_vector<T>(b); }
	template<typename T>
	RELEASE_INLINE mask_vector operator==(typename ve_identity<T>::type a, tagged_vector<T> b) { return b == tagged_vector<T>(a); }
	template<typename T>
	RELEASE_INLINE mask_vector operator!=(typename ve_identity<T>::type a, tagged_vector<T> b) { return b != tagged_vector<T>(a); }

	template<typename tag_type>
	RELEASE_INLINE mask_vector operator==(contiguous_tags_base<tag_type> a, tagged_vector<typename ve_identity<tag_type>::type> b) {
		return tagged_vector<tag_type>(
			tag_type(typename tag_type::value_base_t(a.value)),
			tag_type(typename tag_type::value_base_t(a.value + 1)),
			tag_type(typename tag_type::value_base_t(a.value + 2)),
			tag_type(typename tag_type::value_base_t(a.value + 3))) == b;
	}
	template<typename tag_type>
	RELEASE_INLINE mask_vector operator!=(contiguous_tags_base<tag_type> a, tagged_vector<typename ve_identity<tag_type>::type> b) {
		return tagged_vector<tag_type>(
			tag_type(typename tag_type::value_base_t(a.value)),
			tag_type(typename tag_type::value_base_t(a.value + 1)),
			tag_type(typename tag_type::value_base_t(a.value + 2)),
			tag_type(typename tag_type::value_base_t(a.value + 3))) != b;
	}
	template<typename tag_type>
	RELEASE_INLINE mask_vector operator==(tagged_vector<typename ve_identity<tag_type>::type> b, contiguous_tags_base<tag_type> a) { return a == b; }
	template<typename tag_type>
	RELEASE_INLINE mask_vector operator!=(tagged_vector<typename ve_identity<tag_type>::type> b, contiguous_tags_base<tag_type> a) { return a != b; }

	RELEASE_INLINE mask_vector is_non_zero(int_vector i) { return i != int_vector(); }
	RELEASE_INLINE mask_vector is_zero(int_vector i) { return i == int_vector(); }
	template<typename T>
	RELEASE_INLINE mask_vector is_valid_index(tagged_vector<T> i) { return i != tagged_vector<T>(); }
	template<typename T>
	RELEASE_INLINE mask_vector is_invalid(tagged_vector<T> i) { return i == tagged_vector<T>(); }

	RELEASE_INLINE vbitfield_type compress_mask(mask_vector mask) { return static_cast<vbitfield_type>(mask); }

	template<typename T>
	RELEASE_INLINE vbitfield_type load(contiguous_tags<T> e, dcon::bitfield_type const* source) {
		return vbitfield_type{ uint8_t(((source[e.value / uint32_t(8)].v) >> (e.value & 0x00000004)) & 0x0000000F) };
	}
	template<typename T>
	RELEASE_INLINE vbitfield_type load(unaligned_contiguous_tags<T> e, dcon::bitfield_type const* source) {
		return vbitfield_type{ uint8_t(0x0F & (((source[e.value / uint32_t(8)].v) >> (e.value & 0x07)) | ((((e.value & 0x7) > 4) ? (source[1 + e.value / uint32_t(8)].v) : 0) << (8 - (e.value & 0x07))))) };
	}
	template<typename T>
	RELEASE_INLINE vbitfield_type load(partial_contiguous_tags<T> e, dcon::bitfield_type const* source) {
		return vbitfield_type{ uint8_t((0x00FF >> (8 - e.subcount)) & (((source[e.value / uint32_t(8)].v) >> (e.value & 0x07)) | ((((e.value & 0x7) > 4) ? (source[1 + e.value / uint32_t(8)].v) : 0) << (8 - (e.value & 0x07))))) };
	}

	template<typename T>
	RELEASE_INLINE fp_vector load(contiguous_tags<T> e, float const* source) { return fp_vector(source[e.value], source[e.value + 1], source[e.value + 2], source[e.value + 3]); }
	template<typename T>
	RELEASE_INLINE int_vector load(contiguous_tags<T> e, int32_t const* source) { return int_vector(source[e.value], source[e.value + 1], source[e.value + 2], source[e.value + 3]); }
	template<typename T>
	RELEASE_INLINE int_vector load(contiguous_tags<T> e, uint32_t const* source) { return int_vector(source[e.value], source[e.value + 1], source[e.value + 2], source[e.value + 3]); }
	template<typename T, typename U>
	RELEASE_INLINE auto load(contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 4, tagged_vector<U>> {
		return tagged_vector<U>(
			int_vector(
				source[e.value].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 1].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 2].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 3].index() - (U::zero_is_null_t::value ? 1 : 0)),
			std::true_type{});
	}

	template<typename T>
	RELEASE_INLINE fp_vector load(unaligned_contiguous_tags<T> e, float const* source) { return load(contiguous_tags<T>(e.value), source); }
	template<typename T>
	RELEASE_INLINE int_vector load(unaligned_contiguous_tags<T> e, int32_t const* source) { return load(contiguous_tags<T>(e.value), source); }
	template<typename T>
	RELEASE_INLINE int_vector load(unaligned_contiguous_tags<T> e, uint32_t const* source) { return load(contiguous_tags<T>(e.value), source); }
	template<typename T, typename U>
	RELEASE_INLINE auto load(unaligned_contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 4, tagged_vector<U>> {
		return load(contiguous_tags<T>(e.value), source);
	}

	template<typename T>
	RELEASE_INLINE fp_vector load(partial_contiguous_tags<T> e, float const* source) {
		fp_vector r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.set(i, source[e.value + i]);
		return r;
	}
	template<typename T>
	RELEASE_INLINE int_vector load(partial_contiguous_tags<T> e, int32_t const* source) {
		int_vector r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.set(i, source[e.value + i]);
		return r;
	}
	template<typename T>
	RELEASE_INLINE int_vector load(partial_contiguous_tags<T> e, uint32_t const* source) {
		int_vector r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.set(i, int32_t(source[e.value + i]));
		return r;
	}
	template<typename T, typename U>
	RELEASE_INLINE auto load(partial_contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 4, tagged_vector<U>> {
		tagged_vector<U> r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.value.set(i, source[e.value + i].index() - (U::zero_is_null_t::value ? 1 : 0));
		return r;
	}

	template<typename U>
	RELEASE_INLINE fp_vector load(tagged_vector<U> indices, float const* source) {
		auto original = indices.to_original_values();
		return fp_vector(source[original[0]], source[original[1]], source[original[2]], source[original[3]]);
	}
	template<typename U, typename I>
	RELEASE_INLINE auto load(tagged_vector<U> indices, I const* source) -> std::enable_if_t<std::numeric_limits<I>::is_integer && sizeof(I) <= 4, int_vector> {
		auto original = indices.to_original_values();
		return int_vector(int32_t(source[original[0]]), int32_t(source[original[1]]), int32_t(source[original[2]]), int32_t(source[original[3]]));
	}
	template<typename U, typename T>
	RELEASE_INLINE auto load(tagged_vector<U> indices, T const* source) -> std::enable_if_t<!std::numeric_limits<T>::is_integer, tagged_vector<T>> {
		auto original = indices.to_original_values();
		return tagged_vector<T>(
			int_vector(
				source[original[0]].index() - (T::zero_is_null_t::value ? 1 : 0),
				source[original[1]].index() - (T::zero_is_null_t::value ? 1 : 0),
				source[original[2]].index() - (T::zero_is_null_t::value ? 1 : 0),
				source[original[3]].index() - (T::zero_is_null_t::value ? 1 : 0)),
			std::true_type{});
	}
	template<typename U>
	RELEASE_INLINE vbitfield_type load(tagged_vector<U> indices, dcon::bitfield_type const* source) {
		auto original = indices.to_original_values();
		return vbitfield_type{ uint8_t((int32_t(dcon::bit_vector_test(source, original[0])) << 0) | (int32_t(dcon::bit_vector_test(source, original[1])) << 1) | (int32_t(dcon::bit_vector_test(source, original[2])) << 2) | (int32_t(dcon::bit_vector_test(source, original[3])) << 3)) };
	}

	template<typename U>
	RELEASE_INLINE fp_vector load(tagged_vector<U> indices, mask_vector mask, float const* source) {
		auto original = indices.to_original_values();
		return fp_vector(mask[0] ? source[original[0]] : 0.0f, mask[1] ? source[original[1]] : 0.0f, mask[2] ? source[original[2]] : 0.0f, mask[3] ? source[original[3]] : 0.0f);
	}
	template<typename U, typename I>
	RELEASE_INLINE auto load(tagged_vector<U> indices, mask_vector mask, I const* source) -> std::enable_if_t<std::numeric_limits<I>::is_integer && sizeof(I) <= 4, int_vector> {
		auto original = indices.to_original_values();
		return int_vector(mask[0] ? int32_t(source[original[0]]) : 0, mask[1] ? int32_t(source[original[1]]) : 0, mask[2] ? int32_t(source[original[2]]) : 0, mask[3] ? int32_t(source[original[3]]) : 0);
	}
	template<typename U, typename T>
	RELEASE_INLINE auto load(tagged_vector<U> indices, mask_vector mask, T const* source) -> std::enable_if_t<!std::numeric_limits<T>::is_integer, tagged_vector<T>> {
		auto original = indices.to_original_values();
		return tagged_vector<T>(
			int_vector(
				mask[0] ? source[original[0]].index() - (T::zero_is_null_t::value ? 1 : 0) : -1,
				mask[1] ? source[original[1]].index() - (T::zero_is_null_t::value ? 1 : 0) : -1,
				mask[2] ? source[original[2]].index() - (T::zero_is_null_t::value ? 1 : 0) : -1,
				mask[3] ? source[original[3]].index() - (T::zero_is_null_t::value ? 1 : 0) : -1),
			std::true_type{});
	}
	template<typename U>
	RELEASE_INLINE vbitfield_type load(tagged_vector<U> indices, mask_vector mask, dcon::bitfield_type const* source) {
		auto original = indices.to_original_values();
		return vbitfield_type{ uint8_t((int32_t(mask[0] ? dcon::bit_vector_test(source, original[0]) : false) << 0) | (int32_t(mask[1] ? dcon::bit_vector_test(source, original[1]) : false) << 1) | (int32_t(mask[2] ? dcon::bit_vector_test(source, original[2]) : false) << 2) | (int32_t(mask[3] ? dcon::bit_vector_test(source, original[3]) : false) << 3)) };
	}

	template<typename U>
	RELEASE_INLINE fp_vector load(tagged_vector<U> indices, vbitfield_type mask, float const* source) { return load(indices, mask_vector(mask), source); }
	template<typename U, typename I>
	RELEASE_INLINE auto load(tagged_vector<U> indices, vbitfield_type mask, I const* source) -> std::enable_if_t<std::numeric_limits<I>::is_integer && sizeof(I) <= 4, int_vector> { return load(indices, mask_vector(mask), source); }
	template<typename U, typename T>
	RELEASE_INLINE auto load(tagged_vector<U> indices, vbitfield_type mask, T const* source) -> std::enable_if_t<!std::numeric_limits<T>::is_integer, tagged_vector<T>> { return load(indices, mask_vector(mask), source); }
	template<typename U>
	RELEASE_INLINE vbitfield_type load(tagged_vector<U> indices, vbitfield_type mask, dcon::bitfield_type const* source) { return load(indices, mask_vector(mask), source); }

	namespace detail {
		template<typename T>
		constexpr auto zero_is_null_wrapper(int) -> std::enable_if_t<T::zero_is_null_t::value, bool> { return true; }
		template<typename T>
		constexpr auto zero_is_null_wrapper(...) { return false; }
	}

	template<typename T>
	RELEASE_INLINE int_vector load(contiguous_tags<T> e, int16_t const* source) {
		return int_vector(int32_t(source[e.value]), int32_t(source[e.value + 1]), int32_t(source[e.value + 2]), int32_t(source[e.value + 3]));
	}
	template<typename T>
	RELEASE_INLINE int_vector load(contiguous_tags<T> e, uint16_t const* source) {
		return int_vector(uint32_t(source[e.value]), uint32_t(source[e.value + 1]), uint32_t(source[e.value + 2]), uint32_t(source[e.value + 3]));
	}
	template<typename T, typename U>
	RELEASE_INLINE auto load(contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 2, tagged_vector<U>> {
		return tagged_vector<U>(
			int_vector(
				source[e.value].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 1].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 2].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 3].index() - (U::zero_is_null_t::value ? 1 : 0)),
			std::true_type{});
	}
	template<typename T>
	RELEASE_INLINE int_vector load(unaligned_contiguous_tags<T> e, int16_t const* source) { return load(contiguous_tags<T>(e.value), source); }
	template<typename T>
	RELEASE_INLINE int_vector load(unaligned_contiguous_tags<T> e, uint16_t const* source) { return load(contiguous_tags<T>(e.value), source); }
	template<typename T, typename U>
	RELEASE_INLINE auto load(unaligned_contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 2, tagged_vector<U>> { return load(contiguous_tags<T>(e.value), source); }
	template<typename T>
	RELEASE_INLINE int_vector load(partial_contiguous_tags<T> e, int16_t const* source) {
		int_vector r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.set(i, int32_t(source[e.value + i]));
		return r;
	}
	template<typename T>
	RELEASE_INLINE int_vector load(partial_contiguous_tags<T> e, uint16_t const* source) {
		int_vector r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.set(i, int32_t(source[e.value + i]));
		return r;
	}
	template<typename T, typename U>
	RELEASE_INLINE auto load(partial_contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 2, tagged_vector<U>> {
		tagged_vector<U> r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.value.set(i, source[e.value + i].index() - (U::zero_is_null_t::value ? 1 : 0));
		return r;
	}

	template<typename T>
	RELEASE_INLINE int_vector load(contiguous_tags<T> e, int8_t const* source) {
		return int_vector(int32_t(source[e.value]), int32_t(source[e.value + 1]), int32_t(source[e.value + 2]), int32_t(source[e.value + 3]));
	}
	template<typename T>
	RELEASE_INLINE int_vector load(contiguous_tags<T> e, uint8_t const* source) {
		return int_vector(uint32_t(source[e.value]), uint32_t(source[e.value + 1]), uint32_t(source[e.value + 2]), uint32_t(source[e.value + 3]));
	}
	template<typename T, typename U>
	RELEASE_INLINE auto load(contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 1 && !std::is_same_v<U, dcon::bitfield_type>, tagged_vector<U>> {
		return tagged_vector<U>(
			int_vector(
				source[e.value].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 1].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 2].index() - (U::zero_is_null_t::value ? 1 : 0),
				source[e.value + 3].index() - (U::zero_is_null_t::value ? 1 : 0)),
			std::true_type{});
	}
	template<typename T>
	RELEASE_INLINE int_vector load(unaligned_contiguous_tags<T> e, int8_t const* source) { return load(contiguous_tags<T>(e.value), source); }
	template<typename T>
	RELEASE_INLINE int_vector load(unaligned_contiguous_tags<T> e, uint8_t const* source) { return load(contiguous_tags<T>(e.value), source); }
	template<typename T, typename U>
	RELEASE_INLINE auto load(unaligned_contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 1 && !std::is_same_v<U, dcon::bitfield_type>, tagged_vector<U>> { return load(contiguous_tags<T>(e.value), source); }
	template<typename T>
	RELEASE_INLINE int_vector load(partial_contiguous_tags<T> e, int8_t const* source) {
		int_vector r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.set(i, int32_t(source[e.value + i]));
		return r;
	}
	template<typename T>
	RELEASE_INLINE int_vector load(partial_contiguous_tags<T> e, uint8_t const* source) {
		int_vector r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.set(i, int32_t(source[e.value + i]));
		return r;
	}
	template<typename T, typename U>
	RELEASE_INLINE auto load(partial_contiguous_tags<T> e, U const* source) -> std::enable_if_t<sizeof(U) == 1 && !std::is_same_v<U, dcon::bitfield_type>, tagged_vector<U>> {
		tagged_vector<U> r;
		for(uint32_t i = 0; i < e.subcount; ++i) r.value.set(i, source[e.value + i].index() - (U::zero_is_null_t::value ? 1 : 0));
		return r;
	}

	template<typename T>
	RELEASE_INLINE void store(contiguous_tags<T> e, float* dest, fp_vector values) {
		for(int i = 0; i < vector_size; ++i) dest[e.value + i] = values[i];
	}
	template<typename T>
	RELEASE_INLINE void store(contiguous_tags<T> e, int32_t* dest, int_vector values) {
		for(int i = 0; i < vector_size; ++i) dest[e.value + i] = values[i];
	}
	template<typename T>
	RELEASE_INLINE void store(contiguous_tags<T> e, uint32_t* dest, int_vector values) {
		for(int i = 0; i < vector_size; ++i) dest[e.value + i] = uint32_t(values[i]);
	}
	template<typename T, typename U>
	RELEASE_INLINE auto store(contiguous_tags<T> e, U* dest, tagged_vector<U> values) -> std::enable_if_t<sizeof(U) == 4, void> {
		for(int i = 0; i < vector_size; ++i) dest[e.value + i] = values[i];
	}
	template<typename T>
	RELEASE_INLINE void store(unaligned_contiguous_tags<T> e, float* dest, fp_vector values) { store(contiguous_tags<T>(e.value), dest, values); }
	template<typename T>
	RELEASE_INLINE void store(unaligned_contiguous_tags<T> e, int32_t* dest, int_vector values) { store(contiguous_tags<T>(e.value), dest, values); }
	template<typename T>
	RELEASE_INLINE void store(unaligned_contiguous_tags<T> e, uint32_t* dest, int_vector values) { store(contiguous_tags<T>(e.value), dest, values); }
	template<typename T, typename U>
	RELEASE_INLINE auto store(unaligned_contiguous_tags<T> e, U* dest, tagged_vector<U> values) -> std::enable_if_t<sizeof(U) == 4, void> { store(contiguous_tags<T>(e.value), dest, values); }

	template<typename T>
	RELEASE_INLINE void store(contiguous_tags_base<T> e, int16_t* dest, int_vector values) { for(int i = 0; i < vector_size; ++i) dest[e.value + i] = int16_t(values[i]); }
	template<typename T>
	RELEASE_INLINE void store(contiguous_tags_base<T> e, uint16_t* dest, int_vector values) { for(int i = 0; i < vector_size; ++i) dest[e.value + i] = uint16_t(values[i]); }
	template<typename T>
	RELEASE_INLINE void store(contiguous_tags_base<T> e, int8_t* dest, int_vector values) { for(int i = 0; i < vector_size; ++i) dest[e.value + i] = int8_t(values[i]); }
	template<typename T>
	RELEASE_INLINE void store(contiguous_tags_base<T> e, uint8_t* dest, int_vector values) { for(int i = 0; i < vector_size; ++i) dest[e.value + i] = uint8_t(values[i]); }
	template<typename T, typename U>
	RELEASE_INLINE auto store(contiguous_tags_base<T> e, U* dest, tagged_vector<U> values) -> std::enable_if_t<sizeof(U) < 4, void> { for(int i = 0; i < vector_size; ++i) dest[e.value + i] = values[i]; }

	template<typename T>
	RELEASE_INLINE void store(partial_contiguous_tags<T> e, float* dest, fp_vector values) { for(uint32_t i = 0; i < e.subcount; ++i) dest[e.value + i] = values[i]; }
	template<typename T, typename I>
	RELEASE_INLINE void store(partial_contiguous_tags<T> e, I* dest, int_vector values) { for(uint32_t i = 0; i < e.subcount; ++i) dest[e.value + i] = I(values[i]); }
	template<typename T, typename U>
	RELEASE_INLINE void store(partial_contiguous_tags<T> e, U* dest, tagged_vector<U> values) { for(uint32_t i = 0; i < e.subcount; ++i) dest[e.value + i] = values[i]; }

	template<typename T>
	RELEASE_INLINE void store(contiguous_tags<T> e, dcon::bitfield_type* dest, vbitfield_type values) {
		auto old_value = dest[e.value / uint32_t(8)].v;
		auto mask = 0x0F << (e.value & 0x04);
		auto adju_value = values.v << (e.value & 0x04);
		dest[e.value / uint32_t(8)].v = uint8_t((old_value & ~mask) | adju_value);
	}
	template<typename T>
	RELEASE_INLINE void store(unaligned_contiguous_tags<T> e, dcon::bitfield_type* dest, vbitfield_type values) {
		{
			auto old_value = dest[e.value / uint32_t(8)].v;
			auto mask = 0x0F << (e.value & 0x07);
			auto adju_value = values.v << (e.value & 0x07);
			dest[e.value / uint32_t(8)].v = uint8_t((old_value & ~mask) | adju_value);
		}
		if((e.value & 0x07) > 4) {
			auto old_value = dest[1 + e.value / uint32_t(8)].v;
			auto mask = 0x0F >> (8 - (e.value & 0x07));
			auto adju_value = values.v >> (8 - (e.value & 0x07));
			dest[1 + e.value / uint32_t(8)].v = uint8_t((old_value & ~mask) | (adju_value & mask));
		}
	}
	template<typename T>
	RELEASE_INLINE void store(partial_contiguous_tags<T> e, dcon::bitfield_type* dest, vbitfield_type values) {
		{
			auto old_value = dest[e.value / uint32_t(8)].v;
			auto mask = (0x0F >> e.subcount) << (e.value & 0x07);
			auto adju_value = values.v << (e.value & 0x07);
			dest[e.value / uint32_t(8)].v = uint8_t((old_value & ~mask) | (adju_value & mask));
		}
		if((e.value & 0x07) > 4) {
			auto old_value = dest[1 + e.value / uint32_t(8)].v;
			auto mask = (0x0F >> e.subcount) >> (8 - (e.value & 0x07));
			auto adju_value = values.v >> (8 - (e.value & 0x07));
			dest[1 + e.value / uint32_t(8)].v = uint8_t((old_value & ~mask) | (adju_value & mask));
		}
	}

	template<typename T>
	RELEASE_INLINE void store(contiguous_tags<T> e, dcon::bitfield_type* dest, mask_vector values) { store(e, dest, compress_mask(values)); }
	template<typename T>
	RELEASE_INLINE void store(unaligned_contiguous_tags<T> e, dcon::bitfield_type* dest, mask_vector values) { store(e, dest, compress_mask(values)); }
	template<typename T>
	RELEASE_INLINE void store(partial_contiguous_tags<T> e, dcon::bitfield_type* dest, mask_vector values) { store(e, dest, compress_mask(values)); }

	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, float* dest, fp_vector values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) dest[original[i]] = values[i];
	}
	template<typename U, typename I>
	RELEASE_INLINE auto store(tagged_vector<U> indices, I* dest, int_vector values) -> std::enable_if_t<std::numeric_limits<I>::is_integer && sizeof(I) <= 4, void> {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) dest[original[i]] = I(values[i]);
	}
	template<typename U, typename T>
	RELEASE_INLINE void store(tagged_vector<U> indices, T* dest, tagged_vector<T> values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) dest[original[i]] = values[i];
	}
	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, dcon::bitfield_type* dest, vbitfield_type values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) dcon::bit_vector_set(dest, original[i], ((values.v >> i) & 1) != 0);
	}
	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, dcon::bitfield_type* dest, mask_vector values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) dcon::bit_vector_set(dest, original[i], values[i]);
	}

	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, vbitfield_type mask, float* dest, fp_vector values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(((mask.v >> i) & 1) != 0) dest[original[i]] = values[i];
	}
	template<typename U, typename I>
	RELEASE_INLINE auto store(tagged_vector<U> indices, vbitfield_type mask, I* dest, int_vector values) -> std::enable_if_t<std::numeric_limits<I>::is_integer && sizeof(I) <= 4, void> {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(((mask.v >> i) & 1) != 0) dest[original[i]] = I(values[i]);
	}
	template<typename U, typename T>
	RELEASE_INLINE void store(tagged_vector<U> indices, vbitfield_type mask, T* dest, tagged_vector<T> values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(((mask.v >> i) & 1) != 0) dest[original[i]] = values[i];
	}
	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, vbitfield_type mask, dcon::bitfield_type* dest, vbitfield_type values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(((mask.v >> i) & 1) != 0) dcon::bit_vector_set(dest, original[i], ((values.v >> i) & 1) != 0);
	}
	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, vbitfield_type mask, dcon::bitfield_type* dest, mask_vector values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(((mask.v >> i) & 1) != 0) dcon::bit_vector_set(dest, original[i], values[i]);
	}

	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, mask_vector mask, float* dest, fp_vector values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(mask[i]) dest[original[i]] = values[i];
	}
	template<typename U, typename I>
	RELEASE_INLINE auto store(tagged_vector<U> indices, mask_vector mask, I* dest, int_vector values) -> std::enable_if_t<std::numeric_limits<I>::is_integer && sizeof(I) <= 4, void> {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(mask[i]) dest[original[i]] = I(values[i]);
	}
	template<typename U, typename T>
	RELEASE_INLINE void store(tagged_vector<U> indices, mask_vector mask, T* dest, tagged_vector<T> values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(mask[i]) dest[original[i]] = values[i];
	}
	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, mask_vector mask, dcon::bitfield_type* dest, vbitfield_type values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(mask[i]) dcon::bit_vector_set(dest, original[i], ((values.v >> i) & 1) != 0);
	}
	template<typename U>
	RELEASE_INLINE void store(tagged_vector<U> indices, mask_vector mask, dcon::bitfield_type* dest, mask_vector values) {
		auto original = indices.to_original_values();
		for(int32_t i = 0; i < vector_size; ++i) if(mask[i]) dcon::bit_vector_set(dest, original[i], values[i]);
	}
}
