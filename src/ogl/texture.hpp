#pragma once

#include "container_types.hpp"

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

namespace ogl {

class texture;

GLuint load_texture_from_file(sys::state& state, ogl::texture& texture, std::string_view native_name, bool keep_data);
GLuint load_texture_array_from_file(sys::state& state, ogl::texture& texture, std::string_view fname, bool keep_data, int32_t tiles_x, int32_t tiles_y);
GLuint get_texture_handle(sys::state& state, dcon::texture_id id, bool keep_data);

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

	friend GLuint load_texture_from_file(sys::state& state, ogl::texture& texture, std::string_view native_name, bool keep_data);
	friend GLuint load_texture_array_from_file(sys::state& state, ogl::texture& texture, std::string_view fname, bool keep_data, int32_t tiles_x, int32_t tiles_y);
	friend GLuint get_texture_handle(sys::state& state, dcon::texture_id id, bool keep_data);
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

}
