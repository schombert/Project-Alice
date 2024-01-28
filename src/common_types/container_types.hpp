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

struct value_modifier_segment {
	float factor = 0.0f;
	dcon::trigger_key condition;
	uint16_t padding = 0;
};
static_assert(sizeof(value_modifier_segment) ==
	sizeof(value_modifier_segment::factor)
	+ sizeof(value_modifier_segment::condition)
	+ sizeof(value_modifier_segment::padding));

struct value_modifier_description {
	float factor = 0.0f;
	float base = 0.0f;
	uint16_t first_segment_offset = 0;
	uint16_t segments_count = 0;
};
static_assert(sizeof(value_modifier_description) ==
	sizeof(value_modifier_description::factor)
	+ sizeof(value_modifier_description::base)
	+ sizeof(value_modifier_description::first_segment_offset)
	+ sizeof(value_modifier_description::segments_count));

struct event_option {
	dcon::text_sequence_id name;
	dcon::value_modifier_key ai_chance;
	dcon::effect_key effect;
};
static_assert(sizeof(event_option) ==
	sizeof(event_option::name)
	+ sizeof(event_option::ai_chance)
	+ sizeof(event_option::effect));

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
	dcon::state_definition_id wargoal_state;
	dcon::national_identity_id wargoal_tag;
	dcon::nation_id wargoal_secondary_nation;
	dcon::cb_type_id wargoal_type;
	uint8_t padding = 0;
};
static_assert(sizeof(crisis_join_offer) ==
	sizeof(crisis_join_offer::target)
	+ sizeof(crisis_join_offer::wargoal_type)
	+ sizeof(crisis_join_offer::wargoal_state)
	+ sizeof(crisis_join_offer::wargoal_tag)
	+ sizeof(crisis_join_offer::wargoal_secondary_nation)
	+ sizeof(crisis_join_offer::padding));

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
static_assert(sizeof(commodity_set) ==
	sizeof(commodity_set::commodity_amounts)
	+ sizeof(commodity_set::commodity_type));

struct small_commodity_set {
	static constexpr uint32_t set_size = 6;

	float commodity_amounts[set_size] = {0.0f};
	dcon::commodity_id commodity_type[set_size] = {dcon::commodity_id{}};
	uint16_t padding = 0;
};
static_assert(sizeof(small_commodity_set) ==
	sizeof(small_commodity_set::commodity_amounts)
	+ sizeof(small_commodity_set::commodity_type)
	+ sizeof(small_commodity_set::padding));

} // namespace economy

namespace sys {

struct checksum_key {
	static constexpr uint32_t key_size = 64;
	uint8_t key[key_size] = { 0 };

	bool is_equal(const checksum_key& a) noexcept {
		for(size_t i = 0; i < key_size; i++)
			if(key[i] != a.key[i])
				return false;
		return true;
	}
};
static_assert(sizeof(checksum_key) == sizeof(checksum_key::key));

struct player_name {
	char data[48] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	std::string_view to_string_view() noexcept {
		for(int32_t i = sizeof(data) - 1; i >= 0; i--) {
			if(data[i] != ' ') {
				return std::string_view{ reinterpret_cast<const char*>(&data[0]), uint32_t(i) };
			}
		}
		return "???";
	}
};
static_assert(sizeof(player_name) == sizeof(player_name::data));

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

} // namespace sys
