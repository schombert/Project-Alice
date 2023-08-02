#include "map.hpp"

#include "province.hpp"
#include "system_state.hpp"
#include "parsers_declarations.hpp"

namespace map
{

// Used to load the terrain.bmp and the rivers.bmp
std::vector<uint8_t> load_bmp(parsers::scenario_building_context& context, native_string_view name, glm::ivec2 map_size, uint8_t fill) {
	auto root = simple_fs::get_root(context.state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto terrain_bmp = open_file(map_dir, name);
	auto content = simple_fs::view_contents(*terrain_bmp);
	uint8_t const* start = (uint8_t const*)(content.data);

	// Data offset is where the pixel data starts
	uint8_t const* ptr = start + 10;
	uint32_t data_offset = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	// The width & height of the image
	ptr = start + 18;
	uint32_t size_x = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];
	ptr = start + 22;
	uint32_t size_y = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	assert(size_x == uint32_t(map_size.x));

	uint8_t const* data = start + data_offset;

	// Calculate how much extra we add at the poles
	auto free_space = std::max(uint32_t(0), map_size.y - size_y); // schombert: find out how much water we need to add
	auto top_free_space = (free_space * 3) / 5;

	// Fill the output with the given data
	std::vector<uint8_t> output_data(map_size.x * map_size.y, fill);

	// Copy over the bmp data to the middle of the output_data
	for(int y = top_free_space + size_y - 1; y >= int(top_free_space); --y) {
		for(int x = 0; x < int(size_x); ++x) {
			output_data[y * size_x + x] = *data;
			data++;
		}
	}
	return output_data;
}

void display_data::load_terrain_data(parsers::scenario_building_context& context) {
	terrain_id_map = load_bmp(context, NATIVE("terrain.bmp"), glm::ivec2(size_x, size_y), 255);

	// Gets rid of any stray land terrain that has been painted outside the borders
	for(uint32_t y = 0; y < size_y; ++y) {
		for(uint32_t x = 0; x < size_x; ++x) {
			// If there is no province define at that location
			if(province_id_map[y * size_x + x] == 0)
				terrain_id_map[y * size_x + x] = uint8_t(255);
			// If the province defined there is a sea province
			if(province_id_map[y * size_x + x] >= province::to_map_id(context.state.province_definitions.first_sea_province))
				terrain_id_map[y * size_x + x] = uint8_t(255);
		}
	}

	// Load the terrain
	load_median_terrain_type(context);
}


void display_data::load_median_terrain_type(parsers::scenario_building_context& context) {
	median_terrain_type.resize(context.state.world.province_size() + 1);
	std::vector<std::array<int, 64>> terrain_histogram(context.state.world.province_size() + 1, std::array<int, 64>{});
	for(int i = size_x * size_y - 1; i-- > 0;) {
		auto prov_id = province_id_map[i];
		auto terrain_id = terrain_id_map[i];
		if(terrain_id < 64)
			terrain_histogram[prov_id][terrain_id] += 1;
	}

	for(int i = context.state.world.province_size(); i-- > 1;) { // map-id province 0 == the invalid province; we don't need to collect data for it
		int max_index = 64;
		int max = 0;
		for(int j = max_index; j-- > 0;) {
			if(terrain_histogram[i][j] > max) {
				max_index = j;
				max = terrain_histogram[i][j];
			}
		}
		median_terrain_type[i] = uint8_t(max_index);
	}
}

void display_data::load_provinces_mid_point(parsers::scenario_building_context& context) {
	std::vector<glm::ivec2> accumulated_tile_positions(context.state.world.province_size() + 1, glm::vec2(0));
	std::vector<int> tiles_number(context.state.world.province_size() + 1, 0);
	for(int i = size_x * size_y - 1; i-- > 0;) {
		auto prov_id = province_id_map[i];
		int x = i % size_x;
		int y = i / size_x;
		accumulated_tile_positions[prov_id] += glm::vec2(x, y);
		tiles_number[prov_id]++;
	}
	// schombert: needs to start from +1 here or you don't catch the last province
	for(int i = context.state.world.province_size() + 1; i-- > 1;) { // map-id province 0 == the invalid province; we don't need to collect data for it

		glm::vec2 tile_pos;

		assert(tiles_number[i] > 0); // yeah but a province without tiles is no bueno

		if(tiles_number[i] == 0) {
			tile_pos = glm::vec2(0, 0);
		} else {
			tile_pos = accumulated_tile_positions[i] / tiles_number[i];
		}
		context.state.world.province_set_mid_point(province::from_map_id(uint16_t(i)), tile_pos);
	}
}

void display_data::load_province_data(parsers::scenario_building_context& context, image& image) {
	uint32_t imsz = uint32_t(size_x * size_y);
	auto free_space = std::max(uint32_t(0), size_y - image.size_y); // schombert: find out how much water we need to add
	auto top_free_space = (free_space * 3) / 5;

	province_id_map.resize(imsz);
	uint32_t i = 0;
	for(; i < top_free_space * size_x; ++i) { // schombert: fill with nothing until the start of the real data
		province_id_map[i] = 0;
	}
	auto first_actual_map_pixel = top_free_space * size_x; // schombert: where the real data starts
	for(; i < first_actual_map_pixel + image.size_x * image.size_y; ++i) {
		uint8_t* ptr = image.data + (i - first_actual_map_pixel) * 4; // schombert: subtract to find our offset in the actual image data
		auto color = sys::pack_color(ptr[0], ptr[1], ptr[2]);
		if(auto it = context.map_color_to_province_id.find(color); it != context.map_color_to_province_id.end()) {
			assert(it->second);
			province_id_map[i] = province::to_map_id(it->second);
		} else {
			province_id_map[i] = 0;
		}
	}
	for(; i < imsz; ++i) { // schombert: fill remainder with nothing
		province_id_map[i] = 0;
	}

	load_provinces_mid_point(context);
}

void display_data::load_map_data(parsers::scenario_building_context& context) {
	auto root = simple_fs::get_root(context.state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));

	// Load the province map
	auto provinces_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
	auto provinces_image = load_stb_image(*provinces_bmp);

	size_x = uint32_t(provinces_image.size_x);
	size_y = uint32_t(provinces_image.size_x / 2); // schombert: force the correct map size

	load_province_data(context, provinces_image);
	load_terrain_data(context);
	load_border_data(context);

	auto size = glm::ivec2(size_x, size_y);
	auto river_data = load_bmp(context, NATIVE("rivers.bmp"), size, 255);
	river_vertices = create_river_vertices(*this, context, river_data);
}

// Called to load the terrain and province map data
void map_state::load_map_data(parsers::scenario_building_context& context) {
	map_data.load_map_data(context);
}


}
