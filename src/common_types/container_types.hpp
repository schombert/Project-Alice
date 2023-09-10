#pragma once

#include <vector>
#include "dcon_generated.hpp"

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

struct value_modifier_segment {
	float factor = 0.0f;
	dcon::trigger_key condition;
};
struct value_modifier_description {
	float factor = 0.0f;
	float base = 0.0f;
	uint16_t first_segment_offset = 0;
	uint16_t segments_count = 0;
};

struct event_option {
	dcon::text_sequence_id name;
	dcon::value_modifier_key ai_chance;
	dcon::effect_key effect;
};

constexpr int32_t max_event_options = 6;

struct modifier_hash {
	using is_avalanching = void;

	modifier_hash() { }

	auto operator()(dcon::modifier_id m) const noexcept -> uint64_t {
		int32_t index = m.index();
		return ankerl::unordered_dense::hash<int32_t>()(index);
	}
};

struct crisis_join_offer {
	dcon::nation_id target;
	dcon::cb_type_id wargoal_type;
	dcon::state_definition_id wargoal_state;
	dcon::national_identity_id wargoal_tag;
	dcon::nation_id wargoal_secondary_nation;
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
		return *(storage.data() + t.index());
	}
	value_type& operator[](tag_type t) {
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

namespace economy {

struct commodity_set {
	static constexpr uint32_t set_size = 8;

	float commodity_amounts[set_size] = {0.0f};
	dcon::commodity_id commodity_type[set_size] = {dcon::commodity_id{}};
};
struct small_commodity_set {
	static constexpr uint32_t set_size = 2;

	float commodity_amounts[set_size] = {0.0f};
	dcon::commodity_id commodity_type[set_size] = {dcon::commodity_id{}};
};

} // namespace economy

namespace sys {

struct checksum_key {
	uint8_t data[64];

	bool is_equal(const checksum_key& a) noexcept {
		for(size_t i = 0; i < 64; i++)
			if(data[i] != a.data[i])
				return false;
		return true;
	}
};

} // namespace sys
