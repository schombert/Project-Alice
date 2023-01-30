#include "map.hpp"

void map::load_map(sys::state& state) {
	// TODO Better error handling and reporting ^^
	ogl::load_texture_from_file(state, state.map_provinces_texture, simple_fs::remove_double_backslashes("map\\provinces.bmp"), false);
	if(!state.map_provinces_texture.loaded) {
		printf("failed to load provinces.bmp\n");
		std::abort();
	}
	ogl::load_texture_from_file(state, state.map_terrain_texture, simple_fs::remove_double_backslashes("map\\terrain.bmp"), false);
	if(!state.map_terrain_texture.loaded) {
		printf("failed to load terrain.bmp\n");
		std::abort();
	}
	ogl::load_texture_from_file(state, state.map_rivers_texture, simple_fs::remove_double_backslashes("map\\rivers.bmp"), false);
	if(!state.map_rivers_texture.loaded) {
		printf("failed to load rivers.bmp\n");
		std::abort();
	}
	ogl::load_texture_array_from_file(state, state.map_terrainsheet_texture, simple_fs::remove_double_backslashes("map\\terrain\\texturesheet.tga"), false, 8, 8);
	if(!state.map_terrainsheet_texture.loaded) {
		printf("failed to load texturesheet.tga\n");
		std::abort();
	}
	state.map_x_size = state.map_provinces_texture.size_x;
	state.map_y_size = state.map_provinces_texture.size_y;
}
