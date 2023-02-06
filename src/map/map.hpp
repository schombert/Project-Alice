#pragma once

#include "system_state.hpp"
#include <glm/vec2.hpp>

namespace map {

enum class map_mode : uint8_t {
	terrain = 0x01,
	political = 0x02,
	revolt = 0x03,
	diplomatic = 0x04,
	region = 0x05,
	infrastructure = 0x06,
	colonial = 0x07,
	admin = 0x08,
	recruitment = 0x09,
	national_focus = 0x0A,
	rgo_output = 0x0B,
	population = 0x0C,
	nationality = 0x0D,
	sphere = 0x0E,
	supply = 0x0F,
	party_loyalty = 0x10,
	rank = 0x11,
	migration = 0x12,
	civilization_level = 0x13,
	relation = 0x14,
	crisis = 0x15,
	naval = 0x16
};

class display_data {
public:
	display_data() {};
	~display_data();

	// Called to load the map. Will load the texture and shaders from disk
	void load_map(sys::state& state);

	map_mode active_map_mode = map_mode::terrain;

	void render(uint32_t screen_x, uint32_t screen_y);
	void set_province_color(std::vector<uint32_t> const& prov_color);

	// Set the position of camera. Position relative from 0-1
	void set_pos(glm::vec2 pos);

	// Input methods
	void on_key_down(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_key_up(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_mouse_wheel(int32_t x, int32_t y, sys::key_modifiers mod, float amount);
	void on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod);

private:
	// Last update time, used for smooth map movement
	std::chrono::time_point<std::chrono::system_clock> last_update_time{};

	// Time in seconds, send to the map shader for animations
	float time_counter = 0;

	// Meshes
	GLuint water_vbo = 0;
	GLuint land_vbo = 0;
	GLuint vao = 0;
	uint32_t water_indicies = 0;
	uint32_t land_indicies = 0;

	// Textures
	GLuint provinces_texture_handle = 0;
	GLuint terrain_texture_handle = 0;
	GLuint rivers_texture_handle = 0;
	GLuint terrainsheet_texture_handle = 0;
	GLuint water_normal = 0;
	GLuint colormap_water = 0;
	GLuint colormap_terrain = 0;
	GLuint overlay = 0;
	GLuint province_color = 0;

	// Shaders
	GLuint map_shader_program = 0;
	GLuint map_water_shader_program = 0;

	// Position and movement
	glm::vec2 pos = glm::vec2(0.5f, 0.5f);
	glm::vec2 pos_velocity = glm::vec2(0.f);
	glm::vec2 last_camera_drag_pos;
	bool is_dragging = false;
	glm::vec2 size; // Map size
	float offset_x = 0.f;
	float offset_y = 0.f;
	float zoom = 1;

	void update();

	glm::vec2 screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size);

	void load_shaders(simple_fs::directory& root);
	void create_meshes(simple_fs::file& file);
};
}
