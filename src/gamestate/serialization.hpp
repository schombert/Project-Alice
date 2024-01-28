#pragma once
#include <vector>
#include "container_types.hpp"
#include "unordered_dense.h"
#include "text.hpp"
#include "simple_fs.hpp"

namespace sys {

template<typename T>
inline size_t serialize_size(std::vector<T> const& vec) {
	return sizeof(uint32_t) + sizeof(T) * vec.size();
}

template<typename T>
inline uint8_t* serialize(uint8_t* ptr_in, std::vector<T> const& vec) {
	uint32_t length = uint32_t(vec.size());
	memcpy(ptr_in, &length, sizeof(uint32_t));
	memcpy(ptr_in + sizeof(uint32_t), vec.data(), sizeof(T) * vec.size());
	return ptr_in + sizeof(uint32_t) + sizeof(T) * vec.size();
}

template<typename T>
inline uint8_t const* deserialize(uint8_t const* ptr_in, std::vector<T>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));
	vec.resize(length);
	memcpy(vec.data(), ptr_in + sizeof(uint32_t), sizeof(T) * length);
	return ptr_in + sizeof(uint32_t) + sizeof(T) * length;
}

template<typename T>
inline uint8_t* memcpy_serialize(uint8_t* ptr_in, T const& obj) {
	memcpy(ptr_in, &obj, sizeof(T));
	return ptr_in + sizeof(T);
}

template<typename T>
inline uint8_t const* memcpy_deserialize(uint8_t const* ptr_in, T& obj) {
	memcpy(&obj, ptr_in, sizeof(T));
	return ptr_in + sizeof(T);
}

template<typename T, typename tag_type>
inline size_t serialize_size(tagged_vector<T, tag_type> const& vec) {
	return sizeof(uint32_t) + sizeof(T) * vec.size();
}

template<typename T, typename tag_type>
inline uint8_t* serialize(uint8_t* ptr_in, tagged_vector<T, tag_type> const& vec) {
	uint32_t length = uint32_t(vec.size());
	memcpy(ptr_in, &length, sizeof(uint32_t));
	memcpy(ptr_in + sizeof(uint32_t), vec.data(), sizeof(T) * vec.size());
	return ptr_in + sizeof(uint32_t) + sizeof(T) * vec.size();
}

template<typename T, typename tag_type>
inline uint8_t const* deserialize(uint8_t const* ptr_in, tagged_vector<T, tag_type>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));
	vec.resize(length);
	memcpy(vec.data(), ptr_in + sizeof(uint32_t), sizeof(T) * length);
	return ptr_in + sizeof(uint32_t) + sizeof(T) * length;
}

inline size_t serialize_size(
		ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq> const&
				vec) {
	return serialize_size(vec.values());
}

inline uint8_t* serialize(uint8_t* ptr_in,
		ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq> const&
				vec) {
	return serialize(ptr_in, vec.values());
}
inline uint8_t const* deserialize(uint8_t const* ptr_in,
		ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));

	std::remove_cvref_t<decltype(vec.values())> new_vec;
	new_vec.resize(length);
	memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
	vec.replace(std::move(new_vec));

	return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
}

inline size_t serialize_size(
		ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash> const& vec) {
	return serialize_size(vec.values());
}

inline uint8_t* serialize(uint8_t* ptr_in,
		ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash> const& vec) {
	return serialize(ptr_in, vec.values());
}
inline uint8_t const* deserialize(uint8_t const* ptr_in,
		ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash>& vec) {
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
inline uint8_t const* deserialize(uint8_t const* ptr_in, ankerl::unordered_dense::map<uint16_t, dcon::text_key>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));

	std::remove_cvref_t<decltype(vec.values())> new_vec;
	new_vec.resize(length);
	memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
	vec.replace(std::move(new_vec));

	return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
}

constexpr inline uint32_t save_file_version = 35;
constexpr inline uint32_t scenario_file_version = 120 + save_file_version;

struct scenario_header {
	uint32_t version = scenario_file_version;
	uint32_t count = 0;
	uint64_t timestamp = 0;
	checksum_key checksum;
};

struct save_header {
	uint32_t version = save_file_version;
	uint32_t count = 0;
	uint64_t timestamp = 0;
	checksum_key checksum;
	dcon::national_identity_id tag;
	dcon::government_type_id cgov;
	sys::date d;
};

struct mod_identifier {
	native_string mod_path;
	uint64_t timestamp = 0;
	uint32_t count = 0;
};

void read_mod_path(uint8_t const* ptr_in, uint8_t const* lim, native_string& path_out);
uint8_t const* load_mod_path(uint8_t const* ptr_in, sys::state& state);
uint8_t* write_mod_path(uint8_t* ptr_in, native_string const& path_in);
size_t sizeof_mod_path(native_string const& path_in);

uint8_t const* read_scenario_header(uint8_t const* ptr_in, scenario_header& header_out);
uint8_t const* read_save_header(uint8_t const* ptr_in, save_header& header_out);
uint8_t* write_scenario_header(uint8_t* ptr_in, scenario_header const& header_in);
uint8_t* write_save_header(uint8_t* ptr_in, save_header const& header_in);
size_t sizeof_scenario_header(scenario_header const& header_in);
size_t sizeof_save_header(save_header const& header_in);

mod_identifier extract_mod_information(uint8_t const* ptr_in, uint64_t file_size);

uint8_t* write_compressed_section(uint8_t* ptr_out, uint8_t const* ptr_in, uint32_t uncompressed_size);

// Note: these functions are for read / writing the *uncompressed* data
uint8_t const* read_scenario_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state);
uint8_t const* read_save_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state);
uint8_t* write_scenario_section(uint8_t* ptr_in, sys::state& state);
uint8_t* write_save_section(uint8_t* ptr_in, sys::state& state);
size_t sizeof_scenario_section(sys::state& state);
size_t sizeof_save_section(sys::state& state);

void write_scenario_file(sys::state& state, native_string_view name, uint32_t count);
bool try_read_scenario_file(sys::state& state, native_string_view name);
bool try_read_scenario_and_save_file(sys::state& state, native_string_view name);
bool try_read_scenario_as_save_file(sys::state& state, native_string_view name);

void write_save_file(sys::state& state, bool autosave = false);
bool try_read_save_file(sys::state& state, native_string_view name);

} // namespace sys
