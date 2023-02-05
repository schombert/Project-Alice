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
private:
	// Last update time, used for smooth map movement
	std::chrono::time_point<std::chrono::system_clock> last_update_time{};

	glm::vec2 pos = glm::vec2(0.5f, 0.5f);
	glm::vec2 pos_velocity = glm::vec2(0.f);
	glm::vec2 last_camera_drag_pos;
	bool is_dragging = false;

	GLuint map_shader_program = 0;
	GLuint map_water_shader_program = 0;

public:
	void load_shaders(simple_fs::directory& root);

	GLuint water_vbo;
	GLuint land_vbo;
	GLuint vao;
	uint32_t water_indicies;
	uint32_t land_indicies;
	void create_meshes(simple_fs::file& file);
	// Time in seconds, send to the map shader for animations
	float time_counter = 0;

	GLuint provinces_texture_handle = 0;
	GLuint terrain_texture_handle = 0;
	GLuint rivers_texture_handle = 0;
	GLuint terrainsheet_texture_handle = 0;
	GLuint water_normal = 0;
	GLuint colormap_water = 0;
	GLuint colormap_terrain = 0;
	GLuint overlay = 0;
	GLuint province_color = 0;

	map_mode active_map_mode = map_mode::terrain;
	glm::vec2 size;
	float offset_x = 0.f;
	float offset_y = 0.f;
	float zoom = 1;

	void render(uint32_t screen_x, uint32_t screen_y);
	void set_province_color(std::vector<uint32_t> const& prov_color);
	void update();
	void set_pos(glm::vec2 pos);
	glm::vec2 screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size);
	void on_key_down(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_key_up(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_mouse_wheel(int32_t x, int32_t y, sys::key_modifiers mod, float amount);
	void on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod);
};
void load_map(sys::state& state);
}
