#include "map.hpp"

void map::load_map(sys::state& state) {
	auto root = simple_fs::get_root(state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto map_terrain_dir = simple_fs::open_directory(map_dir, NATIVE("terrain"));

	map::display_data& map_display = state.map_display;

	// TODO Better error handling and reporting ^^
	auto provinces_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
	ogl::load_texture_from_file(state, map_display.map_provinces_texture, *provinces_bmp, false);
	auto terrain_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
	ogl::load_texture_from_file(state, map_display.map_terrain_texture, *terrain_bmp, false);
	auto rivers_bmp = open_file(map_dir, NATIVE("rivers.bmp"));
	ogl::load_texture_from_file(state, map_display.map_rivers_texture, *rivers_bmp, false);
	auto texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.tga"));
	ogl::load_texture_array_from_file(state, map_display.map_terrainsheet_texture, *texturesheet, false, 8, 8);
	map_display.map_x_size = map_display.map_provinces_texture.size_x;
	map_display.map_y_size = map_display.map_provinces_texture.size_y;
}

void map::display_data::update() {
	map_x_pos += map_x_vel;
	map_y_pos += map_y_vel;
	auto const velocity_fn = [this](auto& v, auto const v_scale) {
		auto const stop_movement_threshold = (1.f / map_zoom) * 0.0001f;
		v *= 0.975f;

		if(v > 0.f && v < v_scale * stop_movement_threshold) {
			v = 0.f;
		} else if(v < 0.f && v > -v_scale * stop_movement_threshold) {
			v = 0.f;
		}
	};
	velocity_fn(map_x_vel, 1.f / map_x_size);
	velocity_fn(map_y_vel, 1.f / map_y_size);
	map_x_pos = std::fmod(map_x_pos + 0.5f, 1.f) - 0.5;
	map_y_pos = std::clamp(map_y_pos, -0.5f, 0.5f);
}

void map::display_data::on_key_down(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::A) {
		map_x_vel += (1.f / map_x_size) * (1.f / map_zoom) * 0.0125f;
	} else if(keycode == sys::virtual_key::D) {
		map_x_vel -= (1.f / map_x_size) * (1.f / map_zoom) * 0.0125f;
	} else if(keycode == sys::virtual_key::W) {
		map_y_vel -= (1.f / map_y_size) * (1.f / map_zoom) * 0.0125f;
	} else if(keycode == sys::virtual_key::S) {
		map_y_vel += (1.f / map_y_size) * (1.f / map_zoom) * 0.0125f;
	} else if(keycode == sys::virtual_key::Q) {
		map_zoom *= 1.1f;
	} else if(keycode == sys::virtual_key::E) {
		map_zoom *= 0.9f;
	}
}
void map::display_data::on_mouse_wheel(int32_t x, int32_t y, sys::key_modifiers mod, float amount) {
	map_zoom *= 1.f + amount / 5.f;
}
