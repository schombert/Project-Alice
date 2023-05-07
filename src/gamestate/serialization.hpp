#pragma once
#include <vector>
#include "container_types.hpp"
#include "unordered_dense.h"
#include "text.hpp"
#include "simple_fs.hpp"

namespace sys {

template<typename T>
size_t serialize_size(std::vector<T> const& vec) {
	return sizeof(uint32_t) + sizeof(T) * vec.size();
}

template<typename T>
uint8_t* serialize(uint8_t* ptr_in, std::vector<T> const& vec) {
	uint32_t length = uint32_t(vec.size());
	memcpy(ptr_in, &length, sizeof(uint32_t));
	memcpy(ptr_in + sizeof(uint32_t), vec.data(), sizeof(T) * vec.size());
	return ptr_in + sizeof(uint32_t) + sizeof(T) * vec.size();
}

template<typename T>
uint8_t const* deserialize(uint8_t const* ptr_in, std::vector<T>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));
	vec.resize(length);
	memcpy(vec.data(), ptr_in + sizeof(uint32_t), sizeof(T) * length);
	return ptr_in + sizeof(uint32_t) + sizeof(T) * length;
}

template<typename T>
uint8_t* memcpy_serialize(uint8_t* ptr_in, T const& obj) {
	memcpy(ptr_in, &obj, sizeof(T));
	return ptr_in + sizeof(T);
}

template<typename T>
uint8_t const* memcpy_deserialize(uint8_t const* ptr_in, T& obj) {
	memcpy(&obj, ptr_in, sizeof(T));
	return ptr_in + sizeof(T);
}


template<typename T, typename tag_type>
size_t serialize_size(tagged_vector<T, tag_type> const& vec) {
	return sizeof(uint32_t) + sizeof(T) * vec.size();
}

template<typename T, typename tag_type>
uint8_t* serialize(uint8_t* ptr_in, tagged_vector<T, tag_type> const& vec) {
	uint32_t length = uint32_t(vec.size());
	memcpy(ptr_in, &length, sizeof(uint32_t));
	memcpy(ptr_in + sizeof(uint32_t), vec.data(), sizeof(T) * vec.size());
	return ptr_in + sizeof(uint32_t) + sizeof(T) * vec.size();
}

template<typename T, typename tag_type>
uint8_t const* deserialize(uint8_t const* ptr_in, tagged_vector<T, tag_type>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));
	vec.resize(length);
	memcpy(vec.data(), ptr_in + sizeof(uint32_t), sizeof(T) * length);
	return ptr_in + sizeof(uint32_t) + sizeof(T) * length;
}

inline size_t serialize_size(ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq> const& vec) {
	return serialize_size(vec.values());
}

inline uint8_t* serialize(uint8_t* ptr_in, ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq> const& vec) {
	return serialize(ptr_in, vec.values());
}
uint8_t const* deserialize(uint8_t const* ptr_in, ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));

	std::remove_cvref_t<decltype(vec.values())> new_vec;
	new_vec.resize(length);
	memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
	vec.replace(std::move(new_vec));

	return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
}

inline size_t serialize_size(ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash> const& vec) {
	return serialize_size(vec.values());
}

inline uint8_t* serialize(uint8_t* ptr_in, ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash> const& vec) {
	return serialize(ptr_in, vec.values());
}
uint8_t const* deserialize(uint8_t const* ptr_in, ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));

	std::remove_cvref_t<decltype(vec.values())> new_vec;
	new_vec.resize(length);
	memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
	vec.replace(std::move(new_vec));

	return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
}

inline size_t serialize_size(ankerl::unordered_dense::map<uint16_t, dcon::text_key> const& vec) {
	return serialize_size(vec.values());
}

inline uint8_t* serialize(uint8_t* ptr_in, ankerl::unordered_dense::map<uint16_t, dcon::text_key> const& vec) {
	return serialize(ptr_in, vec.values());
}
uint8_t const* deserialize(uint8_t const* ptr_in, ankerl::unordered_dense::map<uint16_t, dcon::text_key>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));

	std::remove_cvref_t<decltype(vec.values())> new_vec;
	new_vec.resize(length);
	memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
	vec.replace(std::move(new_vec));

	return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
}

constexpr inline uint32_t save_file_version = 16;
constexpr inline uint32_t scenario_file_version = 37 + save_file_version;

struct scenario_header {
	uint32_t version = scenario_file_version;
};

struct save_header {
	uint32_t version = save_file_version;
};

uint8_t const* read_scenario_header(uint8_t const* ptr_in, scenario_header& header_out);
uint8_t const* read_save_header(uint8_t const* ptr_in, save_header& header_out);
uint8_t* write_scenario_header(uint8_t* ptr_in, scenario_header const& header_in);
uint8_t* write_save_header(uint8_t* ptr_in, save_header const& header_in);
size_t sizeof_scenario_header(scenario_header const& header_in);
size_t sizeof_save_header(save_header const& header_in);

uint8_t* write_compressed_section(uint8_t* ptr_out, uint8_t const* ptr_in, uint32_t uncompressed_size);

// Note: these functions are for read / writing the *uncompressed* data
uint8_t const* read_scenario_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state);
uint8_t const* read_save_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state);
uint8_t* write_scenario_section(uint8_t* ptr_in, sys::state& state);
uint8_t* write_save_section(uint8_t* ptr_in, sys::state& state);
size_t sizeof_scenario_section(sys::state& state);
size_t sizeof_save_section(sys::state& state);

void write_scenario_file(sys::state& state, native_string_view name);
bool try_read_scenario_file(sys::state& state, native_string_view name);
bool try_read_scenario_and_save_file(sys::state& state, native_string_view name);

void write_save_file(sys::state& state, native_string_view name);
bool try_read_save_file(sys::state& state, native_string_view name);


}
