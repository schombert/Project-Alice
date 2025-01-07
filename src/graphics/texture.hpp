#pragma once

#include "container_types.hpp"

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"
#include "culture.hpp"

namespace ogl {

class texture;

GLuint get_texture_handle(sys::state& state, dcon::texture_id id, bool keep_data);
native_string flag_type_to_name(sys::state& state, culture::flag_type type);
GLuint get_flag_handle(sys::state& state, dcon::national_identity_id nat_id, culture::flag_type type);
GLuint get_rebel_flag_handle(sys::state& state, dcon::rebel_faction_id faction);
GLuint load_file_and_return_handle(native_string const& native_name, simple_fs::file_system const& fs, texture& asset_texture, bool keep_data);
GLuint get_late_load_texture_handle(sys::state& state, dcon::texture_id& id, std::string_view asset_name);

enum {
	SOIL_FLAG_TEXTURE_REPEATS = 4,
};

GLuint SOIL_direct_load_DDS_from_memory(unsigned char const* const buffer, uint32_t buffer_length, uint32_t& width, uint32_t& height, int soil_flags);

class texture {
	GLuint texture_handle = 0;

public:
	uint8_t* data = nullptr;
	int32_t size_x = 0;
	int32_t size_y = 0;
	int32_t channels = 4;

	bool loaded = false;

	texture() { }
	texture(texture const&) = delete;
	texture(texture&& other) noexcept;
	~texture();

	texture& operator=(texture const&) = delete;
	texture& operator=(texture&& other) noexcept;

	GLuint get_texture_handle() const;

	friend GLuint get_texture_handle(sys::state& state, dcon::texture_id id, bool keep_data);
	friend GLuint load_file_and_return_handle(native_string const& native_name, simple_fs::file_system const& fs,
			texture& asset_texture, bool keep_data);
	friend GLuint get_flag_handle(sys::state& state, dcon::national_identity_id nat_id, culture::flag_type type);
	friend GLuint get_rebel_flag_handle(sys::state& state, dcon::rebel_faction_id faction);
	friend GLuint get_late_load_texture_handle(sys::state& state, dcon::texture_id& id, std::string_view asset_name);
};

class data_texture {
	GLuint texture_handle = 0;

public:
	uint8_t* data = nullptr;
	int32_t size = 0;
	int32_t channels = 4;

	bool data_updated = false;

	data_texture(int32_t sz, int32_t ch);
	data_texture(data_texture const&) = delete;
	data_texture(data_texture&& other) noexcept;

	data_texture& operator=(data_texture const&) = delete;
	data_texture& operator=(data_texture&& other) noexcept;

	GLuint handle();
	~data_texture();
};

struct font_texture_result {
	uint32_t handle = 0;
	uint32_t size = 0;
};

font_texture_result make_font_texture(simple_fs::file& f);

} // namespace ogl
