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

//inline size_t serialize_size(std::wstring const& s) {
//	return sizeof(uint32_t) + sizeof(wchar_t) * s.size();
//}

inline size_t serialize_size(std::string const& s) {
	return sizeof(uint32_t) + sizeof(char) * s.size();
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

//inline uint8_t* serialize(uint8_t* ptr_in, std::wstring const& s) {
//	uint32_t length = uint32_t(s.size());
//	memcpy(ptr_in, &length, sizeof(uint32_t));
//	memcpy(ptr_in + sizeof(uint32_t), s.data(), sizeof(wchar_t) * s.size());
//	return ptr_in + sizeof(uint32_t) + sizeof(wchar_t) * s.size();
//}

//inline uint8_t const* deserialize(uint8_t const* ptr_in, std::wstring& s) {
//	uint32_t length = 0;
//	memcpy(&length, ptr_in, sizeof(uint32_t));
//	s.resize(length);
//	memcpy(s.data(), ptr_in + sizeof(uint32_t), sizeof(wchar_t) * length);
//	return ptr_in + sizeof(uint32_t) + sizeof(wchar_t) * length;
//}

inline uint8_t* serialize(uint8_t* ptr_in, std::string const& s) {
	uint32_t length = uint32_t(s.size());
	memcpy(ptr_in, &length, sizeof(uint32_t));
	memcpy(ptr_in + sizeof(uint32_t), s.data(), sizeof(char) * s.size());
	return ptr_in + sizeof(uint32_t) + sizeof(char) * s.size();
}

inline uint8_t const* deserialize(uint8_t const* ptr_in, std::string& s) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));
	s.resize(length);
	memcpy(s.data(), ptr_in + sizeof(uint32_t), sizeof(char) * length);
	return ptr_in + sizeof(uint32_t) + sizeof(char) * length;
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
		ankerl::unordered_dense::map<dcon::text_key, uint32_t, text::vector_backed_ci_hash, text::vector_backed_ci_eq> const&
				vec) {
	return serialize_size(vec.values());
}

inline size_t serialize_size(
		ankerl::unordered_dense::set<dcon::text_key, text::vector_backed_ci_hash, text::vector_backed_ci_eq> const&
				vec) {
	return serialize_size(vec.values());
}

inline uint8_t* serialize(uint8_t* ptr_in,
		ankerl::unordered_dense::map<dcon::text_key, uint32_t, text::vector_backed_ci_hash, text::vector_backed_ci_eq> const&
				vec) {
	return serialize(ptr_in, vec.values());
}
inline uint8_t* serialize(uint8_t* ptr_in,
		ankerl::unordered_dense::set<dcon::text_key, text::vector_backed_ci_hash, text::vector_backed_ci_eq> const&
				vec) {
	return serialize(ptr_in, vec.values());
}
inline uint8_t const* deserialize(uint8_t const* ptr_in,
		ankerl::unordered_dense::map<dcon::text_key, uint32_t, text::vector_backed_ci_hash, text::vector_backed_ci_eq>& vec) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));

	std::remove_cvref_t<decltype(vec.values())> new_vec;
	new_vec.resize(length);
	memcpy(new_vec.data(), ptr_in + sizeof(uint32_t), sizeof(vec.values()[0]) * length);
	vec.replace(std::move(new_vec));

	return ptr_in + sizeof(uint32_t) + sizeof(vec.values()[0]) * length;
}
inline uint8_t const* deserialize(uint8_t const* ptr_in,
		ankerl::unordered_dense::set<dcon::text_key, text::vector_backed_ci_hash, text::vector_backed_ci_eq>& vec) {
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

constexpr inline uint32_t save_file_version = 44;
constexpr inline uint32_t scenario_file_version = 139 + save_file_version;

struct scenario_header {
	uint32_t version = scenario_file_version;
	uint32_t count = 0;
	uint64_t timestamp = 0;
	checksum_key checksum;
	char mod_save_dir[128] = { 0 };
};

struct save_header {
	uint32_t version = save_file_version;
	uint32_t count = 0;
	uint64_t timestamp = 0;
	checksum_key checksum;
	dcon::national_identity_id tag;
	dcon::government_type_id cgov;
	sys::date d;
	char save_name[64];
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
uint8_t const* read_scenario_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state, bool for_mp_checksum = false);
uint8_t const* read_save_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state, bool for_mp_checksum = false);
uint8_t* write_scenario_section(uint8_t* ptr_in, sys::state& state, bool for_mp_checksum = false);
uint8_t* write_save_section(uint8_t* ptr_in, sys::state& state, bool for_mp_checksum = false);
struct scenario_size {
	size_t total_size;
	size_t checksum_offset;
};
scenario_size sizeof_scenario_section(sys::state& state, bool for_mp_checksum = false);
size_t sizeof_save_section(sys::state& state, bool for_mp_checksum = false);

size_t sizeof_mp_data(sys::state& state);
uint8_t* write_mp_data(uint8_t* ptr_in, sys::state& state);
uint8_t const* read_mp_data(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state);

// for serializing entire MP state, for OOS reporting when someone ooses
uint8_t const* read_entire_mp_state(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state);
uint8_t* write_entire_mp_state(uint8_t* ptr_in, sys::state& state);
size_t sizeof_entire_mp_state(sys::state& state);

// combines load record settings by OR-ing them together
void combine_load_records(dcon::load_record& affected_record, const dcon::load_record& other_record);

void write_scenario_file(sys::state& state, native_string_view name, uint32_t count);
bool try_read_scenario_file(sys::state& state, native_string_view name);
bool try_read_scenario_and_save_file(sys::state& state, native_string_view name);
bool try_read_scenario_as_save_file(sys::state& state, native_string_view name);

std::string get_default_save_name(sys::state& state, save_type type);

void write_save_file(sys::state& state, sys::save_type type = sys::save_type::normal, std::string const& name = std::string(""), const std::string& file_name = std::string(""));
bool try_read_save_file(sys::state& state, native_string_view name);

} // namespace sys
