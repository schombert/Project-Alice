#include "map.hpp"

void map::load_map(sys::state& state) {
	auto root = simple_fs::get_root(state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto map_terrain_dir = simple_fs::open_directory(map_dir, NATIVE("terrain"));

	// TODO Better error handling and reporting ^^
	auto provinces_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
	ogl::load_texture_from_file(state, state.map_provinces_texture, *provinces_bmp, false);
	auto terrain_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
	ogl::load_texture_from_file(state, state.map_terrain_texture, *terrain_bmp, false);
	auto rivers_bmp = open_file(map_dir, NATIVE("rivers.bmp"));
	ogl::load_texture_from_file(state, state.map_rivers_texture, *rivers_bmp, false);
	auto texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.tga"));
	ogl::load_texture_array_from_file(state, state.map_terrainsheet_texture, *texturesheet, false, 8, 8);
	state.map_x_size = state.map_provinces_texture.size_x;
	state.map_y_size = state.map_provinces_texture.size_y;
}
