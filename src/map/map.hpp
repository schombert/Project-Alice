#pragma once

#include "system_state.hpp"

namespace map {

class display_data {
public:
	// map data
	ogl::texture map_provinces_texture;
	ogl::texture map_terrain_texture;
	ogl::texture map_rivers_texture;
	ogl::texture map_terrainsheet_texture;
	int32_t map_x_size = 0;
	int32_t map_y_size = 0;
	float map_x_pos = 0.f;
	float map_x_vel = 0.f;
	float map_y_pos = 0.f;
	float map_y_vel = 0.f;
	float map_zoom = 0.3486784401f;

	void update();
	void on_key_down(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_mouse_wheel(int32_t x, int32_t y, sys::key_modifiers mod, float amount);
};
void load_map(sys::state& state);
}
