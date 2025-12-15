#pragma once

#include <vector>
#include "unordered_dense.h"
#include "dcon_generated_ids.hpp"



namespace sys {
struct state; // this is here simply to declare the state struct in a very general location

inline float red_from_int(uint32_t v) {
	return float(v & 0xFF) / 255.0f;
}
inline float green_from_int(uint32_t v) {
	return float((v >> 8) & 0xFF) / 255.0f;
}
inline float blue_from_int(uint32_t v) {
	return float((v >> 16) & 0xFF) / 255.0f;
}
inline uint32_t pack_color(float r, float g, float b) {
	return ((uint32_t(r * 255.0f) & 0xFF) << 0) | ((uint32_t(g * 255.0f) & 0xFF) << 8) | ((uint32_t(b * 255.0f) & 0xFF) << 16);
}

inline int32_t int_red_from_int(uint32_t v) {
	return int32_t(v & 0xFF);
}
inline int32_t int_green_from_int(uint32_t v) {
	return int32_t((v >> 8) & 0xFF);
}
inline int32_t int_blue_from_int(uint32_t v) {
	return int32_t((v >> 16) & 0xFF);
}
inline uint32_t pack_color(int32_t r, int32_t g, int32_t b) {
	return ((uint32_t(r) & 0xFF) << 0) | ((uint32_t(g) & 0xFF) << 8) | ((uint32_t(b) & 0xFF) << 16);
}

struct hsv {
	float h;
	float s;
	float v;
};

inline hsv rgb_to_hsv(uint32_t v) {
	auto r = red_from_int(v);
	auto g = green_from_int(v);
	auto b = blue_from_int(v);

	auto cmin = std::min(r, std::min(g, b));
	auto cmax = std::max(r, std::max(g, b));
	auto delta = cmax - cmin;

	float h = 0.0f;
	if(delta == 0.0f) {
		h = 0.0f;
	} else if(cmax == r) {
		h = 60.0f * (fmod((g - b) / delta, 6.0f));
	} else if(cmax == g) {
		h = 60.0f * ((b - r) / delta + 2.0f);
	} else /*if(cmax == b)*/ {
		h = 60.0f * ((r - g) / delta + 4.0f);
	}

	return hsv{
		h,
		cmax == 0.0f ? 0.0f : delta / cmax,
		cmax
	};
}

inline uint32_t hsv_to_rgb(hsv v) {
	auto c = v.v * v.s;
	auto x = c * (1.0f - std::abs(fmod(v.h / 60.0f, 2.0f) - 1.0f));
	auto m = v.v - c;
	float r = 0.0f; float g = 0.0f; float b = 0.0f;
	if(0.0f <= v.h && v.h < 60.0f) {
		r = c; g = x; b = 0.0f;
	} else if(60.0f <= v.h && v.h < 120.0f) {
		r = x; g = c; b = 0.0f;
	} else if(120.0f <= v.h && v.h < 180.0f) {
		r = 0.0f; g = c; b = x;
	} else if(180.0f <= v.h && v.h < 240.0f) {
		r = 0.0f; g = x; b = c;
	} else if(240.0f <= v.h && v.h < 300.0f) {
		r = x; g = 0.0f; b = c;
	} else /* if(300.0f <= v.h && v.h < 360.0f) */ {
		r = c; g = 0.0f; b = x;
	}
	return pack_color(
		std::clamp(r + m, 0.0f, 255.0f),
		std::clamp(g + m, 0.0f, 255.0f),
		std::clamp(b + m, 0.0f, 255.0f)
	);
}






struct modifier_hash {
	using is_avalanching = void;

	modifier_hash() { }

	auto operator()(dcon::modifier_id m) const noexcept -> uint64_t {
		int32_t index = m.index();
		return ankerl::unordered_dense::hash<int32_t>()(index);
	}
};

struct province_hash {
	using is_avalanching = void;

	province_hash() {
	}

	auto operator()(dcon::province_id p) const noexcept -> uint64_t {
		int32_t index = p.index();
		return ankerl::unordered_dense::hash<int32_t>()(index);
	}
};

struct pop_hash {
	using is_avalanching = void;

	pop_hash() {
	}

	auto operator()(dcon::pop_id p) const noexcept -> uint64_t {
		int32_t index = p.index();
		return ankerl::unordered_dense::hash<int32_t>()(index);
	}
};

struct regiment_hash {
	using is_avalanching = void;

	regiment_hash() {
	}

	auto operator()(dcon::regiment_id p) const noexcept -> uint64_t {
		int32_t index = p.index();
		return ankerl::unordered_dense::hash<int32_t>()(index);
	}
};

struct gamerule_hash {
	using is_avalanching = void;

	gamerule_hash() {
	}

	auto operator()(dcon::gamerule_id p) const noexcept -> uint64_t {
		int32_t index = p.index();
		return ankerl::unordered_dense::hash<int32_t>()(index);
	}
};

struct nation_hash {
	using is_avalanching = void;

	nation_hash() {
	}

	auto operator()(dcon::nation_id p) const noexcept -> uint64_t {
		int32_t index = p.index();
		return ankerl::unordered_dense::hash<int32_t>()(index);
	}
};

} // namespace sys

template<typename value_type, typename tag_type, typename allocator = std::allocator<value_type>>
class tagged_vector {
private:
	std::vector<value_type, allocator> storage;

public:
	using public_value_type = value_type;
	using public_tag_type = tag_type;

	tagged_vector() { }
	tagged_vector(tagged_vector<value_type, tag_type, allocator> const& other) noexcept : storage(other.storage) { }
	tagged_vector(tagged_vector<value_type, tag_type, allocator>&& other) noexcept : storage(std::move(other.storage)) { }
	tagged_vector(size_t size) {
		storage.resize(size);
	}

	tagged_vector& operator=(tagged_vector<value_type, tag_type, allocator> const& other) noexcept {
		storage = other.storage;
		return *this;
	}
	tagged_vector& operator=(tagged_vector<value_type, tag_type, allocator>&& other) noexcept {
		storage = std::move(other.storage);
		return *this;
	}
	value_type const& operator[](tag_type t) const {
		assert(size_t(t.index()) < storage.size());
		return *(storage.data() + t.index());
	}
	value_type& operator[](tag_type t) {
		assert(size_t(t.index()) < storage.size());
		return *(storage.data() + t.index());
	}
	template<typename... T>
	tag_type emplace_back(T&&... ts) {
		storage.emplace_back(std::forward<T>(ts)...);
		return tag_type(typename tag_type::value_base_t(storage.size() - 1));
	}
	value_type& safe_get(tag_type t) {
		if(t.index() >= std::ssize(storage))
			storage.resize(t.index() + 1);
		return storage[t.index()];
	}
	auto data() const {
		return storage.data();
	}
	auto data() {
		return storage.data();
	}
	auto array() const {
		return storage.data();
	}
	auto array() {
		return storage.data();
	}
	auto begin() const {
		return storage.begin();
	}
	auto end() const {
		return storage.end();
	}
	auto begin() {
		return storage.begin();
	}
	auto end() {
		return storage.end();
	}
	auto size() const {
		return storage.size();
	}
	auto ssize() const {
		return std::ssize(storage);
	}
	void resize(size_t size) {
		storage.resize(size);
	}
	void reserve(size_t size) {
		storage.reserve(size);
	}
	void pop_back() {
		storage.pop_back();
	}
	tag_type push_back(value_type const& v) {
		storage.push_back(v);
		return tag_type(typename tag_type::value_base_t(storage.size() - 1));
	}
	tag_type push_back(value_type&& v) {
		storage.push_back(std::move(v));
		return tag_type(typename tag_type::value_base_t(storage.size() - 1));
	}
	value_type& back() {
		return storage.back();
	}
	value_type const& back() const {
		return storage.back();
	}
};


namespace sys {

struct checksum_key {
	static constexpr uint32_t key_size = 32;
	uint8_t key[key_size] = { 0 };

	bool is_equal(const checksum_key& a) noexcept {
		for(size_t i = 0; i < key_size; i++)
			if(key[i] != a.key[i])
				return false;
		return true;
	}

	std::string_view to_string_view() noexcept {
		return std::string_view{ reinterpret_cast<const char*>(&key[0]), key_size };
	}
	std::string to_string() noexcept {
		return std::string(to_string_view());
	}
	const char* to_char() noexcept {
		return reinterpret_cast<const char*>(&key[0]);
	}
};
static_assert(sizeof(checksum_key) == sizeof(checksum_key::key));


struct macro_builder_template {
	static constexpr uint32_t max_types = 48;
	sys::checksum_key scenario_checksum;
	dcon::nation_id source;
	char name[8] = { 0 };
	uint8_t amounts[max_types] = { 0 };

	bool operator!=(macro_builder_template& o) {
		return std::memcmp(this, &o, sizeof(*this));
	}
};

template<typename IT, typename PTR, typename CMP>
void merge_sort_interior(IT first, IT end, PTR buffer_start, PTR buffer_end, CMP const& cmp) noexcept {
	auto rng_size = end - first;
	if(rng_size == 0) {
		return;
	} else if(rng_size == 1) {
		*first = *buffer_start;
		return;
	} else if(rng_size == 2) {
		if(cmp(*buffer_start, *(buffer_start + 1))) {
			*first = *buffer_start;
			*(first + 1) = *(buffer_start + 1);
		} else {
			*first = *(buffer_start + 1);
			*(first + 1) = *buffer_start;
		}
		return;
	}

	auto half_way = first + rng_size / 2;
	auto b_half_way = buffer_start + rng_size / 2;

	merge_sort_interior(buffer_start, b_half_way, first, half_way, cmp);
	merge_sort_interior(b_half_way, buffer_end, half_way, end, cmp);

	auto temp_index = b_half_way;
	while(temp_index != buffer_end && buffer_start != b_half_way) {
		if(cmp(*temp_index, *buffer_start)) {
			*first = *temp_index;
			++temp_index;
			++first;
		} else {
			*first = *buffer_start;
			++buffer_start;
			++first;
		}
	}
	if(temp_index == buffer_end) {
		std::copy_n(buffer_start, (end - first), first);
	} else if(buffer_start == b_half_way) {
		std::copy_n(temp_index, (end - first), first);
	}
}

template<typename IT, typename CMP>
void merge_sort(IT first, IT end, CMP const& cmp) noexcept {
	auto rng_size = end - first;
	if(rng_size == 0 || rng_size == 1) {
		return;
	} else if(rng_size == 2) {
		if(!cmp(*first, *(first + 1))) {
			std::swap(*first, *(first + 1));
		}
		return;
	}

	using element_type = std::remove_cvref_t<decltype(*first)>;
	auto buffer = new element_type[rng_size];
	std::copy_n(first, rng_size, buffer);
	merge_sort_interior(first, end, buffer, buffer + rng_size, cmp);
	delete[] buffer;
}

struct full_wg {
	dcon::nation_id added_by;
	dcon::nation_id target_nation;
	dcon::nation_id secondary_nation;
	dcon::national_identity_id wg_tag;
	dcon::state_definition_id state;
	dcon::cb_type_id cb;

	bool operator==(const full_wg& other) const = default;
	bool operator!=(const full_wg& other) const = default;

};

struct aui_pending_bytes {
	char const* data = nullptr;
	size_t size = 0;
};

} // namespace sys


namespace ui {

struct chat_message {
	dcon::nation_id source{};
	dcon::nation_id target{};
	std::string body;
	// the reason the sender name is a unique_ptr and not a string or simple array is cause the Cyto:Any has a space limit of 64 bytes which it becomes encapsulated in later, and together with the body the struct will overflow with a array of size 24.
	std::unique_ptr<sys::player_name> sender_name;

	chat_message() {
		sender_name = std::make_unique<sys::player_name>();
	}
	chat_message(const chat_message& m) {
		sender_name = std::make_unique<sys::player_name>();
		source = m.source;
		target = m.target;
		body = m.body;
		memcpy(sender_name.get(), m.sender_name.get(), 24);
	}
	chat_message(chat_message&&) = default;
	chat_message& operator=(const chat_message& m) {
		if(this == &m)
			return *this;
		source = m.source;
		target = m.target;
		body = m.body;
		memcpy(sender_name.get(), m.sender_name.get(), 24);
		return *this;
	}
	chat_message& operator=(chat_message&&) = default;
	~chat_message() {
	}

	bool operator==(chat_message const& o) const {
		return source == o.source && target == o.target && body == o.body;
	}
	bool operator!=(chat_message const& o) const {
		return !(*this == o);
	}
	void set_sender_name(const sys::player_name& name) {
		sender_name.reset(new sys::player_name{ name });
	}
	sys::player_name& get_sender_name() const {
		return *sender_name;
	}
};
}


