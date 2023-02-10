#pragma once

#include "system_state.hpp"
#include "map_modes.hpp"
#include <glm/vec2.hpp>

namespace map {

class display_data {
public:
	display_data() {};
	~display_data();

	// Called to load the terrain and province map data
	void load_map_data(sys::state& state, ankerl::unordered_dense::map<uint32_t, dcon::province_id> const& color_map);
	// Called to load the map. Will load the texture and shaders from disk
	void load_map(sys::state& state);

	map_mode::mode active_map_mode = map_mode::mode::terrain;

	void render(uint32_t screen_x, uint32_t screen_y);
	void set_province_color(std::vector<uint32_t> const& prov_color, map_mode::mode map_mode);
	void set_terrain_map_mode();

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
	GLuint border_vbo = 0;
	GLuint vao = 0;
	GLuint border_vao = 0;
	uint32_t water_indicies = 0;
	uint32_t land_indicies = 0;
	uint32_t border_indicies = 0;

	// Textures
	GLuint provinces_texture_handle = 0;
	GLuint terrain_texture_handle = 0;
	GLuint rivers_texture_handle = 0;
	GLuint terrainsheet_texture_handle = 0;
	GLuint water_normal = 0;
	GLuint colormap_water = 0;
	GLuint colormap_terrain = 0;
	GLuint colormap_political = 0;
	GLuint overlay = 0;
	GLuint province_color = 0;
	GLuint border_texture = 0;

	// Shaders
	GLuint terrain_shader = 0;
	GLuint terrain_political_far_shader = 0;
	GLuint terrain_political_close_shader = 0;
	GLuint water_shader = 0;
	GLuint water_political_shader = 0;
	GLuint vic2_border_shader = 0;
	GLuint line_border_shader = 0;

	std::vector<uint8_t> terrain_id_map;
	std::vector<uint16_t> province_id_map;
	std::vector<glm::vec2> province_mid_point;
	std::vector<uint8_t> median_terrain_type;

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
	void create_meshes();
	void create_borders1();
	void create_borders2();
};
}
