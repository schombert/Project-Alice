#pragma once

#include "container_types.hpp"

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

namespace ogl {

class texture;

GLuint get_texture_handle(sys::state& state, dcon::texture_id id, bool keep_data);
GLuint get_flag_handle(sys::state& state, dcon::national_identity_id nat_id, culture::flag_type type);

enum {
	SOIL_FLAG_POWER_OF_TWO = 1,
	SOIL_FLAG_MIPMAPS = 2,
	SOIL_FLAG_TEXTURE_REPEATS = 4,
	SOIL_FLAG_MULTIPLY_ALPHA = 8,
	SOIL_FLAG_INVERT_Y = 16,
	SOIL_FLAG_COMPRESS_TO_DXT = 32,
	SOIL_FLAG_DDS_LOAD_DIRECT = 64,
	SOIL_FLAG_NTSC_SAFE_RGB = 128,
	SOIL_FLAG_CoCg_Y = 256,
	SOIL_FLAG_TEXTURE_RECTANGLE = 512,
	SOIL_FLAG_PVR_LOAD_DIRECT = 1024,
	SOIL_FLAG_ETC1_LOAD_DIRECT = 2048,
	SOIL_FLAG_GL_MIPMAPS = 4096,
	SOIL_FLAG_SRGB_COLOR_SPACE = 8192,
	SOIL_FLAG_NEAREST = 16384,
};

unsigned int SOIL_direct_load_DDS_from_memory(
		const unsigned char* const buffer,
		unsigned int buffer_length,
		unsigned int& width,
		unsigned int& height,
		int flags);

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
	friend GLuint load_file_and_return_handle(native_string const& native_name, simple_fs::file_system const& fs, texture& asset_texture, bool keep_data);
	friend GLuint get_flag_handle(sys::state& state, dcon::national_identity_id nat_id, culture::flag_type type);
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

uint32_t make_font_texture(simple_fs::file& file);

}
