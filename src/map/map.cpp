#include "map.hpp"
#include "texture.hpp"
#include <cmath>
#include <glm/glm.hpp>

void set_gltex_parameters(GLuint texture_type, GLuint filter, GLuint wrap) {
	if (filter == GL_LINEAR_MIPMAP_NEAREST) {
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(texture_type);
	} else {
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, filter);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

GLuint load_texture_from_file(simple_fs::file& file, GLuint filter) {
	auto content = simple_fs::view_contents(file);
	int32_t file_channels, size_x, size_y;

	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size),
		&size_x, &size_y, &file_channels, 4);

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RGBA, GL_UNSIGNED_BYTE, data);
		set_gltex_parameters(GL_TEXTURE_2D, filter, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	STBI_FREE(data);
	return texture_handle;
}

GLuint load_texture_array_from_file(sys::state& state, simple_fs::file& file, int32_t tiles_x, int32_t tiles_y) {
	auto content = simple_fs::view_contents(file);
	int32_t file_channels, size_x, size_y;

	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size),
		&size_x, &size_y, &file_channels, 4);

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);

		size_t p_dx = size_x / tiles_x; // Pixels of each tile in x
		size_t p_dy = size_y / tiles_y; // Pixels of each tile in y
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GLsizei(p_dx), GLsizei(p_dy), GLsizei(tiles_x * tiles_y), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, size_x);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, size_y);

		for(int32_t x = 0; x < tiles_x; x++)
			for(int32_t y = 0; y < tiles_y; y++)
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
					0, 0,
					0, GLint(x * tiles_x + y),
					GLsizei(p_dx), GLsizei(p_dy),
					1,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					((uint32_t const*)data) + (x * p_dy * size_x + y * p_dx));

		set_gltex_parameters(GL_TEXTURE_2D_ARRAY, GL_LINEAR_MIPMAP_NEAREST, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
	}

	STBI_FREE(data);

	return texture_handle;
}

// Load the terrain texture, will read the BMP file directly
// The image is flipped for some reason
GLuint load_terrain_texture(simple_fs::file& file, int32_t& size_x, int32_t& size_y) {
	auto content = simple_fs::view_contents(file);
	uint8_t* start = (uint8_t*)(content.data);

	// TODO make a check for when the bmp format is unsupporeted

	// Data offset is where the pixel data starts
	uint8_t* ptr = start + 10;
	uint32_t data_offset = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	// The width & height of the image
	ptr = start + 18;
	size_x = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];
	ptr = start + 22;
	size_y = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	char const* data = content.data + data_offset;

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		// Create a texture with only one byte color
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RED, GL_UNSIGNED_BYTE, data);
		set_gltex_parameters(GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	return texture_handle;
}

GLuint load_dds_texture(simple_fs::directory const& dir, native_string_view file_name) {
	auto file = simple_fs::open_file(dir, file_name);
	auto content = simple_fs::view_contents(*file);
	uint32_t size_x, size_y;
	uint8_t const* data = (uint8_t const*)(content.data);
	return ogl::SOIL_direct_load_DDS_from_memory(data, content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS);
}

void map::load_map(sys::state& state) {
	auto root = simple_fs::get_root(state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto map_terrain_dir = simple_fs::open_directory(map_dir, NATIVE("terrain"));

	map::display_data& map_display = state.map_display;

	auto terrain_bmp = open_file(map_dir, NATIVE("terrain.bmp"));
	int32_t size_x, size_y;
	map_display.terrain_texture_handle = load_terrain_texture(*terrain_bmp, size_x, size_y);
	map_display.size = glm::vec2(size_x, size_y);

	// TODO Better error handling and reporting ^^
	auto provinces_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
	map_display.provinces_texture_handle = load_texture_from_file(*provinces_bmp, GL_NEAREST);
	auto rivers_bmp = open_file(map_dir, NATIVE("rivers.bmp"));
	map_display.rivers_texture_handle = load_texture_from_file(*rivers_bmp, GL_NEAREST);
	auto texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.tga"));
	map_display.terrainsheet_texture_handle = load_texture_array_from_file(state, *texturesheet, 8, 8);

	map_display.water_normal = load_dds_texture(map_terrain_dir, NATIVE("sea_normal.dds"));
	map_display.colormap_water = load_dds_texture(map_terrain_dir, NATIVE("colormap_water.dds"));
	map_display.colormap_terrain = load_dds_texture(map_terrain_dir, NATIVE("colormap.dds"));
	map_display.overlay = load_dds_texture(map_terrain_dir, NATIVE("map_overlay_tile.dds"));

	glUseProgram(state.open_gl.map_shader_program);

	// uniform vec2 map_size
	glUniform2f(3, GLfloat(map_display.size.x), GLfloat(map_display.size.y));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, map_display.provinces_texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, map_display.terrain_texture_handle);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, map_display.rivers_texture_handle);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, map_display.terrainsheet_texture_handle);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, map_display.water_normal);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, map_display.colormap_water);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, map_display.colormap_terrain);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, map_display.overlay);
}

void map::display_data::update() {
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	// Set the last_update_time if it hasn't been set yet
	if(last_update_time == std::chrono::time_point<std::chrono::system_clock>{})
		last_update_time = now;

	auto microseconds_since_last_update = std::chrono::duration_cast<std::chrono::microseconds>(now - last_update_time);
	float seconds_since_last_update = (float)(microseconds_since_last_update.count() / 1e6);
	last_update_time = now;

	time_counter += seconds_since_last_update;
	time_counter = (float)std::fmod(time_counter, 600.f); // Reset it after every 10 minutes

	auto velocity = pos_velocity * (seconds_since_last_update / zoom);
	velocity.x *= size.y / size.x;
	pos += velocity;

	pos.x = glm::mod(pos.x, 1.f);
	pos.y = glm::clamp(pos.y, 0.f, 1.0f);
	offset_x = glm::mod(pos.x, 1.f) - 0.5f;
	offset_y = pos.y - 0.5f;
}

void map::display_data::on_key_down(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::LEFT) {
		pos_velocity.x = -1.f;
	} else if(keycode == sys::virtual_key::RIGHT) {
		pos_velocity.x = +1.f;
	} else if(keycode == sys::virtual_key::UP) {
		pos_velocity.y = -1.f;
	} else if(keycode == sys::virtual_key::DOWN) {
		pos_velocity.y = +1.f;
	} else if(keycode == sys::virtual_key::Q) {
		zoom *= 1.1f;
	} else if(keycode == sys::virtual_key::E) {
		zoom *= 0.9f;
	}
}

void map::display_data::on_key_up(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::LEFT) {
		pos_velocity.x = 0;
	} else if(keycode == sys::virtual_key::RIGHT) {
		pos_velocity.x = 0;
	} else if(keycode == sys::virtual_key::UP) {
		pos_velocity.y = 0;
	} else if(keycode == sys::virtual_key::DOWN) {
		pos_velocity.y = 0;
	}
}


void map::display_data::set_pos(glm::vec2 new_pos) {
	pos.x = glm::mod(new_pos.x, 1.f);
	pos.y = glm::clamp(new_pos.y, 0.f, 1.0f);
	offset_x = glm::mod(pos.x, 1.f) - 0.5f;
	offset_y = pos.y - 0.5f;
}

void map::display_data::on_mouse_wheel(int32_t x, int32_t y, sys::key_modifiers mod, float amount) {
	zoom *= 1.f + amount / 5.f;
}

void map::display_data::on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	if(is_dragging) {  // Drag the map with middlemouse
		auto mouse_pos = glm::vec2(x, y);
		auto screen_size = glm::vec2(screen_size_x, screen_size_y);
		auto map_pos = screen_to_map(mouse_pos, screen_size);

		set_pos(pos + last_camera_drag_pos - glm::vec2(map_pos));
    }
}

glm::vec2 map::display_data::screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size) {
	screen_pos -= screen_size * 0.5f;
	screen_pos /= screen_size;
	screen_pos.x *= screen_size.x / screen_size.y;
	screen_pos.x /= size.x / size.y;

	screen_pos /= zoom;
	screen_pos += pos;
	return screen_pos;
}

void map::display_data::on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);

	auto map_pos = screen_to_map(mouse_pos, screen_size);

	last_camera_drag_pos = map_pos;
	is_dragging = true;
	pos_velocity = glm::vec2(0);
}

void map::display_data::on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod) {
	is_dragging = false;
}
