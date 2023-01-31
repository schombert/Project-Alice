#pragma once

#include "system_state.hpp"

namespace map {

class display_data {
public:
	// map data
	GLuint provinces_texture_handle = 0;
	GLuint terrain_texture_handle = 0;
	GLuint rivers_texture_handle = 0;
	ogl::texture terrainsheet_texture;
	int32_t map_x_size = 0;
	int32_t map_y_size = 0;
	float map_x_pos = 0.f;
	float map_x_vel = 0.f;
	float map_y_pos = 0.f;
	float map_y_vel = 0.f;
	float map_zoom = 0.3486784401f;
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
