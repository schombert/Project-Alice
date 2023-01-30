#pragma once

#include <vector>

// this is here simply to declare the state struct in a very general location
namespace sys {
struct state;

inline float red_from_int(uint32_t v) {
	return float(v & 0xFF) / 255.0f;
}
inline float green_from_int(uint32_t v) {
	return float((v >> 8) & 0xFF) / 255.0f;
}
inline float blue_from_int(uint32_t v) {
	return float((v >> 16) & 0xFF) / 255.0f;
}
inline 	uint32_t pack_color(float r, float g, float b) {
	return ((uint32_t(r * 255.0f) & 0xFF) << 0) | ((uint32_t(g * 255.0f) & 0xFF) << 8) | ((uint32_t(b * 255.0f) & 0xFF) << 16);
}

}

template<typename value_type, typename tag_type, typename allocator = std::allocator<value_type>>
class tagged_vector {
private:
	std::vector<value_type, allocator> storage;
public:
	using public_value_type = value_type;
	using public_tag_type = tag_type;

	tagged_vector() {
	}
	value_type const& operator[](tag_type t) const {
		return *(storage.data() + t.index());
	}
	value_type& operator[](tag_type t) {
		return *(storage.data() + t.index());
	}
	template<typename ...T>
	tag_type emplace_back(T&& ... ts) {
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
		storage.resize(size );
	}
	void reserve(size_t size) {
		storage.reserve(size);
	}
	void pop_back() {
		storage.pop_back();
	}
	void push_back(value_type const& v) {
		storage.push_back(v);
	}
	void push_back(value_type&& v) {
		storage.push_back(std::move(v));
	}
	value_type& back() {
		return storage.back();
	}
	value_type const& back() const {
		return storage.back();
	}
};
