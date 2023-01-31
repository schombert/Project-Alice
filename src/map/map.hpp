#pragma once

#include "system_state.hpp"

namespace map {

class display_data {
private:
	// Last update time, used for smooth map movement
	std::chrono::time_point<std::chrono::system_clock> last_update_time{};
public:
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

	int32_t size_x = 0;
	int32_t size_y = 0;
	float pos_x = 0.f;
	float vel_x = 0.f;
	float pos_y = 0.f;
	float vel_y = 0.f;
	float zoom = 0.3486784401f;
	float mouse_pan_mul = 0.f;

	void update();
	void on_key_down(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_mouse_wheel(int32_t x, int32_t y, sys::key_modifiers mod, float amount);
	void on_mouse_move(float rel_x, float rel_y, sys::key_modifiers mod);
	void on_mbuttom_down(int32_t x, int32_t y, sys::key_modifiers mod);
	void on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod);
};
void load_map(sys::state& state);
}
