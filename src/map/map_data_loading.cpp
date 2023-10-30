#include "map.hpp"

#include "province.hpp"
#include "system_state.hpp"
#include "parsers_declarations.hpp"

namespace map
{

// Used to load the terrain.bmp and the rivers.bmp
std::vector<uint8_t> load_bmp(parsers::scenario_building_context& context, native_string_view name, glm::ivec2 map_size, uint8_t fill) {
	std::vector<uint8_t> output_data(map_size.x * map_size.y, fill);

	auto root = simple_fs::get_root(context.state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto terrain_bmp = open_file(map_dir, name);

	if(!terrain_bmp)
		return output_data;

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
	

	// Copy over the bmp data to the middle of the output_data
	for(int y = top_free_space + size_y - 1; y >= int(top_free_space); --y) {
		for(int x = 0; x < int(size_x); ++x) {
			output_data[y * size_x + x] = *data;
			data++;
		}
	}
	return output_data;
}

ankerl::unordered_dense::map<uint32_t, uint8_t> internal_make_index_map() {
	ankerl::unordered_dense::map<uint32_t, uint8_t> m;

	m.insert_or_assign(sys::pack_color(0xEC, 0xEC, 0xEC), uint8_t(0));
	m.insert_or_assign(sys::pack_color(0xD2, 0xD2, 0xD2), uint8_t(1));
	m.insert_or_assign(sys::pack_color(0xB0, 0xB0, 0xB0), uint8_t(2));
	m.insert_or_assign(sys::pack_color(0x8C, 0x8C, 0x8C), uint8_t(3));
	m.insert_or_assign(sys::pack_color(0x70, 0x70, 0x70), uint8_t(4));
	m.insert_or_assign(sys::pack_color(0x56, 0x56, 0x56), uint8_t(5));
	m.insert_or_assign(sys::pack_color(0x4E, 0x4E, 0x4E), uint8_t(6));
	m.insert_or_assign(sys::pack_color(0x38, 0x38, 0x38), uint8_t(7));
	m.insert_or_assign(sys::pack_color(0x98, 0xD3, 0x83), uint8_t(8));
	m.insert_or_assign(sys::pack_color(0x86, 0xBF, 0x5C), uint8_t(9));
	m.insert_or_assign(sys::pack_color(0x6F, 0xA2, 0x39), uint8_t(10));
	m.insert_or_assign(sys::pack_color(0x56, 0x7C, 0x1B), uint8_t(11));
	m.insert_or_assign(sys::pack_color(0x40, 0x61, 0x0C), uint8_t(12));
	m.insert_or_assign(sys::pack_color(0x4C, 0x56, 0x04), uint8_t(13));
	m.insert_or_assign(sys::pack_color(0x27, 0x42, 0x00), uint8_t(14));
	m.insert_or_assign(sys::pack_color(0x21, 0x28, 0x00), uint8_t(15));
	m.insert_or_assign(sys::pack_color(0xA0, 0xD4, 0xDC), uint8_t(16));
	m.insert_or_assign(sys::pack_color(0x78, 0xB4, 0xCA), uint8_t(17));
	m.insert_or_assign(sys::pack_color(0x4B, 0x93, 0xAE), uint8_t(18));
	m.insert_or_assign(sys::pack_color(0x2D, 0x77, 0x92), uint8_t(19));
	m.insert_or_assign(sys::pack_color(0x25, 0x60, 0x7E), uint8_t(20));
	m.insert_or_assign(sys::pack_color(0x0F, 0x3F, 0x5A), uint8_t(21));
	m.insert_or_assign(sys::pack_color(0x06, 0x29, 0x4E), uint8_t(22));
	m.insert_or_assign(sys::pack_color(0x02, 0x14, 0x29), uint8_t(23));
	m.insert_or_assign(sys::pack_color(0xEB, 0xB3, 0xE9), uint8_t(24));
	m.insert_or_assign(sys::pack_color(0xD5, 0x90, 0xC7), uint8_t(25));
	m.insert_or_assign(sys::pack_color(0xB5, 0x6F, 0xB1), uint8_t(26));
	m.insert_or_assign(sys::pack_color(0xB4, 0x56, 0xB3), uint8_t(27));
	m.insert_or_assign(sys::pack_color(0xC0, 0x5A, 0x75), uint8_t(28));
	m.insert_or_assign(sys::pack_color(0xAD, 0x3B, 0x53), uint8_t(29));
	m.insert_or_assign(sys::pack_color(0xA2, 0x27, 0x53), uint8_t(30));
	m.insert_or_assign(sys::pack_color(0x7F, 0x18, 0x3C), uint8_t(31));
	m.insert_or_assign(sys::pack_color(0xE7, 0x20, 0x37), uint8_t(32));
	m.insert_or_assign(sys::pack_color(0xB3, 0x0B, 0x1B), uint8_t(33));
	m.insert_or_assign(sys::pack_color(0x8A, 0x0B, 0x1A), uint8_t(34));
	m.insert_or_assign(sys::pack_color(0x45, 0x0B, 0x10), uint8_t(35));
	m.insert_or_assign(sys::pack_color(0x63, 0x07, 0x0B), uint8_t(36));
	m.insert_or_assign(sys::pack_color(0x52, 0x04, 0x08), uint8_t(37));
	m.insert_or_assign(sys::pack_color(0x3E, 0x02, 0x05), uint8_t(38));
	m.insert_or_assign(sys::pack_color(0x27, 0x00, 0x02), uint8_t(39));
	m.insert_or_assign(sys::pack_color(0x76, 0xF5, 0xD9), uint8_t(40));
	m.insert_or_assign(sys::pack_color(0x61, 0xDC, 0xC1), uint8_t(41));
	m.insert_or_assign(sys::pack_color(0x38, 0xC7, 0xA7), uint8_t(42));
	m.insert_or_assign(sys::pack_color(0x30, 0xAF, 0x93), uint8_t(43));
	m.insert_or_assign(sys::pack_color(0x1F, 0x9A, 0x7F), uint8_t(44));
	m.insert_or_assign(sys::pack_color(0x10, 0x7A, 0x63), uint8_t(45));
	m.insert_or_assign(sys::pack_color(0x02, 0x5E, 0x4A), uint8_t(46));
	m.insert_or_assign(sys::pack_color(0x00, 0x49, 0x39), uint8_t(47));
	m.insert_or_assign(sys::pack_color(0xF1, 0xD2, 0x97), uint8_t(48));
	m.insert_or_assign(sys::pack_color(0xE1, 0xC0, 0x82), uint8_t(49));
	m.insert_or_assign(sys::pack_color(0xCE, 0xA9, 0x63), uint8_t(50));
	m.insert_or_assign(sys::pack_color(0xAC, 0x88, 0x43), uint8_t(51));
	m.insert_or_assign(sys::pack_color(0x96, 0x71, 0x29), uint8_t(52));
	m.insert_or_assign(sys::pack_color(0x7B, 0x5A, 0x1B), uint8_t(53));
	m.insert_or_assign(sys::pack_color(0x65, 0x47, 0x0F), uint8_t(54));
	m.insert_or_assign(sys::pack_color(0x49, 0x32, 0x06), uint8_t(55));
	m.insert_or_assign(sys::pack_color(0x9C, 0x8B, 0x54), uint8_t(56));
	m.insert_or_assign(sys::pack_color(0x88, 0x77, 0xD2), uint8_t(57));
	m.insert_or_assign(sys::pack_color(0x75, 0x63, 0xC2), uint8_t(58));
	m.insert_or_assign(sys::pack_color(0x50, 0x41, 0x92), uint8_t(59));
	m.insert_or_assign(sys::pack_color(0x41, 0x34, 0x79), uint8_t(60));
	m.insert_or_assign(sys::pack_color(0x2D, 0x22, 0x5F), uint8_t(61));
	m.insert_or_assign(sys::pack_color(0x1A, 0x11, 0x43), uint8_t(62));
	m.insert_or_assign(sys::pack_color(0x10, 0x0B, 0x29), uint8_t(63));

	m.insert_or_assign(sys::pack_color(0xFF, 0xFF, 0xFF), uint8_t(255));

	return m;
}

void display_data::load_terrain_data(parsers::scenario_building_context& context) {
	if(!context.new_maps) {
		terrain_id_map = load_bmp(context, NATIVE("terrain.bmp"), glm::ivec2(size_x, size_y), 255);
	} else {
		auto root = simple_fs::get_root(context.state.common_fs);
		auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
		auto terrain_file = open_file(map_dir, NATIVE("alice_terrain.png"));
		terrain_id_map.resize(size_x * size_y, uint8_t(255));

		image terrain_data;
		if(terrain_file)
			terrain_data = load_stb_image(*terrain_file);

		auto terrain_resolution = internal_make_index_map();

		if(terrain_data.size_x == int32_t(size_x) && terrain_data.size_y == int32_t(size_y)) {
			for(uint32_t ty = 0; ty < size_y; ++ty) {
				uint32_t y = size_y - ty - 1;
				for(uint32_t x = 0; x < size_x; ++x) {
					

					uint8_t* ptr = terrain_data.data + (x + size_x * y) * 4;
					auto color = sys::pack_color(ptr[0], ptr[1], ptr[2]);

					if(auto it = terrain_resolution.find(color); it != terrain_resolution.end()) {
						terrain_id_map[ty * size_x + x] = it->second;
					} else {
						uint8_t resolved_index = 255;
						int32_t min_distance = std::numeric_limits<int32_t>::max();
						for(auto& p : terrain_resolution) {
							if(p.second == 255)
								continue;
							auto c = p.first;
							auto r = sys::int_red_from_int(c);
							auto g = sys::int_green_from_int(c);
							auto b = sys::int_blue_from_int(c);
							auto dist = (r - ptr[0]) * (r - ptr[0]) + (b - ptr[1]) * (b - ptr[1]) + (g - ptr[2]) * (g - ptr[2]);
							if(dist < min_distance) {
								min_distance = dist;
								resolved_index = p.second;
							}
						}
						terrain_id_map[ty * size_x + x] = resolved_index;
					}
				}
			}
		}
	}

	// Gets rid of any stray land terrain that has been painted outside the borders
	for(uint32_t y = 0; y < size_y; ++y) {
		for(uint32_t x = 0; x < size_x; ++x) {
			if(province_id_map[y * size_x + x] == 0) { // If there is no province define at that location
				terrain_id_map[y * size_x + x] = uint8_t(255);
			} else if(province_id_map[y * size_x + x] >= province::to_map_id(context.state.province_definitions.first_sea_province)) { // sea province
				terrain_id_map[y * size_x + x] = uint8_t(255);
			} else { // land province
				if(terrain_id_map[y * size_x + x] >= 64) {
					terrain_id_map[y * size_x + x] = uint8_t(5); //  ocean terrain -> plains
				}
			}
		}
	}

	// Load the terrain
	load_median_terrain_type(context);
}


void display_data::load_median_terrain_type(parsers::scenario_building_context& context) {
	median_terrain_type.resize(context.state.world.province_size() + 1);
	province_area.resize(context.state.world.province_size() + 1);
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
		province_area[i] = 0;
		for(int j = max_index; j-- > 0;) {
			province_area[i] += terrain_histogram[i][j];
			if(terrain_histogram[i][j] > max) {
				max_index = j;
				max = terrain_histogram[i][j];
			}
		}
		median_terrain_type[i] = uint8_t(max_index);
		province_area[i] = std::max(province_area[i], uint32_t(1));
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

		// OK, so some mods do in fact define provinces that aren't on the map.
		//assert(tiles_number[i] > 0); // yeah but a province without tiles is no bueno

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
	if(!context.new_maps) {
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
	} else {
		province_id_map.resize(imsz);
		for(uint32_t i = 0; i < imsz; ++i) {
			auto map_x = i % size_x;
			auto map_y = i / size_x;
			uint8_t* ptr = image.data + (map_x + size_x * (size_y - map_y - 1)) * 4;
			auto color = sys::pack_color(ptr[0], ptr[1], ptr[2]);
			if(auto it = context.map_color_to_province_id.find(color); it != context.map_color_to_province_id.end()) {
				assert(it->second);
				province_id_map[i] = province::to_map_id(it->second);
			} else {
				province_id_map[i] = 0;
			}
		}
	}

	load_provinces_mid_point(context);
}

void display_data::load_map_data(parsers::scenario_building_context& context) {
	auto root = simple_fs::get_root(context.state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));

	// Load the province map
	auto provinces_png = open_file(map_dir, NATIVE("alice_provinces.png"));
	map::image provinces_image;
	if(provinces_png) {
		provinces_image = load_stb_image(*provinces_png);
		size_x = uint32_t(provinces_image.size_x);
		size_y = uint32_t(provinces_image.size_y);
		context.new_maps = true;
	} else {
		auto provinces_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
		if(provinces_bmp) {
			provinces_image = load_stb_image(*provinces_bmp);
			size_x = uint32_t(provinces_image.size_x);
			size_y = uint32_t(provinces_image.size_y * 1.3); // schombert: force the correct map size
		} else {
			return; // no map
		}
	}

	load_province_data(context, provinces_image);
	load_terrain_data(context);
	load_border_data(context);

	std::vector<uint8_t> river_data;
	if(!context.new_maps) {
		auto size = glm::ivec2(size_x, size_y);
		river_data = load_bmp(context, NATIVE("rivers.bmp"), size, 255);
	} else {
		auto river_file = open_file(map_dir, NATIVE("alice_rivers.png"));
		river_data.resize(size_x * size_y, uint8_t(255));

		image river_image_data;
		if(river_file)
			river_image_data = load_stb_image(*river_file);

		auto terrain_resolution = internal_make_index_map();

		if(river_image_data.size_x == int32_t(size_x) && river_image_data.size_y == int32_t(size_y)) {
			for(uint32_t ty = 0; ty < size_y; ++ty) {
				uint32_t y = size_y - ty - 1;

				for(uint32_t x = 0; x < size_x; ++x) {

					uint8_t* ptr = river_image_data.data + (x + size_x * y) * 4;
					if(ptr[0] + ptr[1] + ptr[2] < 128 * 3 && terrain_id_map[x + size_x * y] != uint8_t(255))
						river_data[ty * size_x + x] = 0;
					else
						river_data[ty * size_x + x] = 255;
					
				}
			}
		}
	}

	river_vertices = create_river_vertices(*this, context, river_data);
}

// Called to load the terrain and province map data
void map_state::load_map_data(parsers::scenario_building_context& context) {
	map_data.load_map_data(context);
}


}
