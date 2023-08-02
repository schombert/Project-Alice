#include "map.hpp"
#include "glm/fwd.hpp"
#include "texture.hpp"
#include "province.hpp"
#include <cmath>
#include <numbers>
#include <glm/glm.hpp>
#include <glm/mat3x3.hpp>
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "system_state.hpp"
#include "parsers_declarations.hpp"
#include "math_fns.hpp"

namespace map {

image load_stb_image(simple_fs::file& file) {
	int32_t file_channels = 4;
	int32_t size_x = 0;
	int32_t size_y = 0;
	auto content = simple_fs::view_contents(file);
	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size), &size_x, &size_y, &file_channels, 4);
	return image(data, size_x, size_y, 4);
}

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

GLuint make_gl_texture(uint8_t* data, uint32_t size_x, uint32_t size_y, uint32_t channels) {
	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	const GLuint internalformats[] = {GL_R8, GL_RG8, GL_RGB8, GL_RGBA8};
	const GLuint formats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);
		glTexStorage2D(GL_TEXTURE_2D, 1, internalformats[channels - 1], size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, formats[channels - 1], GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return texture_handle;
}
GLuint make_gl_texture(image& image) { return make_gl_texture(image.data, image.size_x, image.size_y, image.channels); }

void set_gltex_parameters(GLuint texture_handle, GLuint texture_type, GLuint filter, GLuint wrap) {
	glBindTexture(texture_type, texture_handle);
	if(filter == GL_LINEAR_MIPMAP_NEAREST) {
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(texture_type);
	} else {
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, filter);
	}
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, wrap);
	glBindTexture(texture_type, 0);
}

GLuint load_texture_array_from_file(simple_fs::file& file, int32_t tiles_x, int32_t tiles_y) {
	auto image = load_stb_image(file);

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);

	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);

		size_t p_dx = image.size_x / tiles_x; // Pixels of each tile in x
		size_t p_dy = image.size_y / tiles_y; // Pixels of each tile in y
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GLsizei(p_dx), GLsizei(p_dy), GLsizei(tiles_x * tiles_y), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, image.size_x);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, image.size_y);

		for(int32_t x = 0; x < tiles_x; x++)
			for(int32_t y = 0; y < tiles_y; y++)
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, GLint(x * tiles_x + y), GLsizei(p_dx), GLsizei(p_dy), 1, GL_RGBA, GL_UNSIGNED_BYTE, ((uint32_t const*)image.data) + (x * p_dy * image.size_x + y * p_dx));

		set_gltex_parameters(texture_handle, GL_TEXTURE_2D_ARRAY, GL_LINEAR_MIPMAP_NEAREST, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
	}

	return texture_handle;
}

enum direction : uint8_t {
	UP_LEFT = 1 << 7,
	UP_RIGHT = 1 << 6,
	DOWN_LEFT = 1 << 5,
	DOWN_RIGHT = 1 << 4,
	UP = 1 << 3,
	DOWN = 1 << 2,
	LEFT = 1 << 1,
	RIGHT = 1 << 0,
};

struct BorderDirection {
	BorderDirection(){};
	struct Information {
		Information(){};
		Information(int32_t index_, int32_t id_) : index{index_}, id{id_} {};
		int32_t index = -1;
		int32_t id = -1;
	};
	Information up;
	Information down;
	Information left;
	Information right;
};

// Create a new vertices to make a line segment
void add_line(glm::vec2 map_pos, glm::vec2 map_size, glm::vec2 offset1, glm::vec2 offset2, int32_t border_id, uint32_t x, direction dir, std::vector<border_vertex>& line_vertices, std::vector<BorderDirection>& current_row, float offset) {

	glm::vec2 direction = normalize(offset2 - offset1);
	glm::vec2 normal_direction = glm::vec2(-direction.y, direction.x);

	// Offset the map position
	map_pos += glm::vec2(offset);
	// Get the map coordinates
	glm::vec2 pos1 = offset1 + map_pos;
	glm::vec2 pos2 = offset2 + map_pos;

	// Rescale the coordinate to 0-1
	pos1 /= map_size;
	pos2 /= map_size;

	int32_t border_index = int32_t(line_vertices.size());
	// First vertex of the line segment
	line_vertices.emplace_back(pos1, normal_direction, direction, border_id);
	line_vertices.emplace_back(pos1, -normal_direction, direction, border_id);
	line_vertices.emplace_back(pos2, -normal_direction, -direction, border_id);
	// Second vertex of the line segment
	line_vertices.emplace_back(pos2, -normal_direction, -direction, border_id);
	line_vertices.emplace_back(pos2, normal_direction, -direction, border_id);
	line_vertices.emplace_back(pos1, normal_direction, direction, border_id);

	BorderDirection::Information direction_information(border_index, border_id);
	switch(dir) {
	case direction::UP:
		current_row[x].up = direction_information;
		break;
	case direction::DOWN:
		current_row[x].down = direction_information;
		break;
	case direction::LEFT:
		current_row[x].left = direction_information;
		break;
	case direction::RIGHT:
		current_row[x].right = direction_information;
		break;
	default:
		break;
	}
};

// Will check if there is an border there already and extend if it can
bool extend_if_possible(uint32_t x, int32_t border_id, direction dir, std::vector<BorderDirection>& last_row, std::vector<BorderDirection>& current_row, glm::vec2 map_size, std::vector<border_vertex>& border_vertices) {
	if(dir & direction::LEFT)
		if(x == 0)
			return false;

	BorderDirection::Information direction_information;
	switch(dir) {
	case direction::UP:
		direction_information = last_row[x].down;
		break;
	case direction::DOWN:
		direction_information = current_row[x].up;
		break;
	case direction::LEFT:
		direction_information = current_row[x - 1].right;
		break;
	case direction::RIGHT:
		direction_information = current_row[x].left;
		break;
	default:
		return false;
	}
	if(direction_information.id != border_id)
		return false;

	auto border_index = direction_information.index;
	if(border_index == -1)
		return false;

	switch(dir) {
	case direction::UP:
	case direction::DOWN:
		border_vertices[border_index + 2].position_.y += 0.5f / map_size.y;
		border_vertices[border_index + 3].position_.y += 0.5f / map_size.y;
		border_vertices[border_index + 4].position_.y += 0.5f / map_size.y;
		break;
	case direction::LEFT:
	case direction::RIGHT:
		border_vertices[border_index + 2].position_.x += 0.5f / map_size.x;
		border_vertices[border_index + 3].position_.x += 0.5f / map_size.x;
		border_vertices[border_index + 4].position_.x += 0.5f / map_size.x;
		break;
	default:
		break;
	}
	switch(dir) {
	case direction::UP:
		current_row[x].up = direction_information;
		break;
	case direction::DOWN:
		current_row[x].down = direction_information;
		break;
	case direction::LEFT:
		current_row[x].left = direction_information;
		break;
	case direction::RIGHT:
		current_row[x].right = direction_information;
		break;
	default:
		break;
	}
	return true;
};

void display_data::load_border_data(parsers::scenario_building_context& context) {
	border_vertices.clear();

	glm::vec2 map_size(size_x, size_y);

	std::vector<std::vector<border_vertex>> borders_list_vertices;

	// The borders of the current row and last row
	std::vector<BorderDirection> current_row(size_x);
	std::vector<BorderDirection> last_row(size_x);

	// Get the index of the border from the province ids and create a new one if one doesn't exist
	auto get_border_index = [&](uint16_t map_province_id1, uint16_t map_province_id2) -> int32_t {
		auto province_id1 = province::from_map_id(map_province_id1);
		auto province_id2 = province::from_map_id(map_province_id2);
		auto border_index = context.state.world.get_province_adjacency_by_province_pair(province_id1, province_id2);
		if(!border_index )
			border_index = context.state.world.force_create_province_adjacency(province_id1, province_id2);
		if(!province_id1 || !province_id2) {
			context.state.world.province_adjacency_set_type(border_index, province::border::impassible_bit);
		}
		return border_index.index();
	};

	auto add_border = [&](uint32_t x0, uint32_t y0, uint16_t id_ul, uint16_t id_ur, uint16_t id_dl, uint16_t id_dr) {
		uint8_t diff_u = id_ul != id_ur;
		uint8_t diff_d = id_dl != id_dr;
		uint8_t diff_l = id_ul != id_dl;
		uint8_t diff_r = id_ur != id_dr;

		glm::vec2 map_pos(x0, y0);

		auto add_line_helper = [&](glm::vec2 pos1, glm::vec2 pos2, uint16_t id1, uint16_t id2, direction dir) {
			auto border_index = get_border_index(id1, id2);
			if(uint32_t(border_index) >= borders_list_vertices.size())
				borders_list_vertices.resize(border_index + 1);
			auto& current_border_vertices = borders_list_vertices[border_index];
			if(!extend_if_possible(x0, border_index, dir, last_row, current_row, map_size, current_border_vertices))
				add_line(map_pos, map_size, pos1, pos2, border_index, x0, dir, current_border_vertices, current_row, 0.5);
		};

		if(diff_l && diff_u && !diff_r && !diff_d) { // Upper left
			add_line_helper(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 0.0f), id_ul, id_dl, direction::UP_LEFT);
		} else if(diff_l && diff_d && !diff_r && !diff_u) { // Lower left
			add_line_helper(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 1.0f), id_ul, id_dl, direction::DOWN_LEFT);
		} else if(diff_r && diff_u && !diff_l && !diff_d) { // Upper right
			add_line_helper(glm::vec2(1.0f, 0.5f), glm::vec2(0.5f, 0.0f), id_ur, id_dr, direction::UP_RIGHT);
		} else if(diff_r && diff_d && !diff_l && !diff_u) { // Lower right
			add_line_helper(glm::vec2(1.0f, 0.5f), glm::vec2(0.5f, 1.0f), id_ur, id_dr, direction::DOWN_LEFT);
		} else {
			if(diff_u) {
				add_line_helper(glm::vec2(0.5f, 0.0f), glm::vec2(0.5f, 0.5f), id_ul, id_ur, direction::UP);
			}
			if(diff_d) {
				add_line_helper(glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 1.0f), id_dl, id_dr, direction::DOWN);
			}
			if(diff_l) {
				add_line_helper(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 0.5f), id_ul, id_dl, direction::LEFT);
			}
			if(diff_r) {
				add_line_helper(glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 0.5f), id_ur, id_dr, direction::RIGHT);
			}
		}
	};
	for(uint32_t y = 0; y < size_y - 1; y++) {
		for(uint32_t x = 0; x < size_x - 1; x++) {
			auto prov_id_ul = province_id_map[(x + 0) + (y + 0) * size_x];
			auto prov_id_ur = province_id_map[(x + 1) + (y + 0) * size_x];
			auto prov_id_dl = province_id_map[(x + 0) + (y + 1) * size_x];
			auto prov_id_dr = province_id_map[(x + 1) + (y + 1) * size_x];
			if(prov_id_ul != prov_id_ur || prov_id_ul != prov_id_dl || prov_id_ul != prov_id_dr) {
				add_border(x, y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr);
				if(prov_id_ul != prov_id_ur && prov_id_ur != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_ur));
				}
				if(prov_id_ul != prov_id_dl && prov_id_dl != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dl));
				}
				if(prov_id_ul != prov_id_dr && prov_id_dr != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));
				}
			}
		}

		{
			// handle the international date line
			auto prov_id_ul = province_id_map[((size_x - 1) + 0) + (y + 0) * size_x];
			auto prov_id_ur = province_id_map[0 + (y + 0) * size_x];
			auto prov_id_dl = province_id_map[((size_x - 1) + 0) + (y + 1) * size_x];
			auto prov_id_dr = province_id_map[0 + (y + 1) * size_x];

			if(prov_id_ul != prov_id_ur || prov_id_ul != prov_id_dl || prov_id_ul != prov_id_dr) {
				add_border((size_x - 1), y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr);

				if(prov_id_ul != prov_id_ur && prov_id_ur != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_ur));
				}
				if(prov_id_ul != prov_id_dl && prov_id_dl != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dl));
				}
				if(prov_id_ul != prov_id_dr && prov_id_dr != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));
				}
			}
		}
		// Move the border_direction rows a step down
		std::swap(last_row, current_row);
		std::fill(current_row.begin(), current_row.end(), BorderDirection{});
	}

	// identify and filter out lakes
	// TODO: unfortunately, this isn't enough to remove the Michigan lakes, since there are two touching lake tiles there

	for(auto k = uint32_t(context.state.province_definitions.first_sea_province.index()); k < context.state.world.province_size(); ++k) {
		dcon::province_id p{dcon::province_id::value_base_t(k)};
		bool any_other_sea = false;
		for(auto adj : context.state.world.province_get_province_adjacency(p)) {
			auto other = adj.get_connected_provinces(0) != p ? adj.get_connected_provinces(0) : adj.get_connected_provinces(1);
			if(other.id.index() >= context.state.province_definitions.first_sea_province.index()) {
				any_other_sea = true;
				break;
			}
		}
		if(!any_other_sea) {
			for(auto adj : context.state.world.province_get_province_adjacency(p)) {
				auto other = adj.get_connected_provinces(0) != p ? adj.get_connected_provinces(0) : adj.get_connected_provinces(1);
				other.set_is_coast(false);
				adj.get_type() |= province::border::impassible_bit;
			}
		}
	}

	for(auto p : context.state.world.in_province) {
		auto rng = p.get_province_adjacency();
		int32_t num_adj = int32_t(rng.end() - rng.begin());
		auto original_province = context.prov_id_to_original_id_map[p].id;
		assert(num_adj < 30);
	}

	/*
	//  This creates a new special border containing the province to province adjacencies
	//  It omits any adjacencies that are marked as impassible

	auto& world = context.state.world;
	auto last = borders_list_vertices.size();
	borders_list_vertices.emplace_back();
	world.for_each_province_adjacency([&](dcon::province_adjacency_id id) {
		auto frel = fatten(world, id);
		if((frel.get_type() & province::border::impassible_bit) != 0)
			return;
		auto prov_a = frel.get_connected_provinces(0);
		auto prov_b = frel.get_connected_provinces(1);
		if(!prov_a || !prov_b)
			return;
		auto mid_point_a = world.province_get_mid_point(prov_a.id);
		auto mid_point_b = world.province_get_mid_point(prov_b.id);
		add_line(glm::vec2(0), map_size, mid_point_a, mid_point_b, 0, 0, direction::UP_RIGHT, borders_list_vertices[last],
				current_row, 0.5);
	});
	*/

	borders.resize(borders_list_vertices.size());
	for(uint32_t border_id = 0; border_id < borders.size(); border_id++) {
		auto& border = borders[border_id];
		auto& current_border_vertices = borders_list_vertices[border_id];
		border.start_index = int32_t(border_vertices.size());
		border.count = int32_t(current_border_vertices.size());
		border.type_flag = context.state.world.province_adjacency_get_type(dcon::province_adjacency_id(dcon::province_adjacency_id::value_base_t(border_id)));

		border_vertices.insert(border_vertices.end(), std::make_move_iterator(current_border_vertices.begin()), std::make_move_iterator(current_border_vertices.end()));
	}
}

bool is_river(uint8_t river_data) { return river_data < 16; }

// Set the river crossing bit for the province adjencencies
// Will march a line between each adjecent province centroid. If it hits a river it will set the bit
void load_river_crossings(parsers::scenario_building_context& context, std::vector<uint8_t> river_data, glm::ivec2 map_size) {
	auto& world = context.state.world;
	world.for_each_province_adjacency([&](dcon::province_adjacency_id id) {
		auto frel = fatten(world, id);
		auto prov_a = frel.get_connected_provinces(0);
		auto prov_b = frel.get_connected_provinces(1);
		auto mid_point_a = world.province_get_mid_point(prov_a.id);
		auto mid_point_b = world.province_get_mid_point(prov_b.id);
		glm::ivec2 tile_pos_a = glm::round(mid_point_a);
		glm::ivec2 tile_pos_b = glm::round(mid_point_b);
		glm::ivec2 diff = glm::abs(tile_pos_a - tile_pos_b);

		bool is_river_crossing = false;
		if(diff.x > diff.y) {
			if(tile_pos_a.x > tile_pos_b.x)
				std::swap(tile_pos_a, tile_pos_b);
			int x_difference = std::max(tile_pos_b.x - tile_pos_a.x, 1);
			int y_difference = tile_pos_a.y - tile_pos_b.y;
			int min_y = std::min(tile_pos_a.y, tile_pos_b.y);
			int max_y = std::max(tile_pos_a.y, tile_pos_b.y);
			for(int x = tile_pos_a.x; x <= tile_pos_b.x; x++) {
				float t = float(x - tile_pos_a.x) / x_difference;
				for(int k = -2; k <= 2; k++) {
					int y = tile_pos_b.y + int(y_difference * t) + k;
					y = std::clamp(y, min_y, max_y);
					is_river_crossing |= is_river(river_data[x + y * map_size.x]);
				}
				if(is_river_crossing)
					break;
			}
		} else {
			if(tile_pos_a.y > tile_pos_b.y)
				std::swap(tile_pos_a, tile_pos_b);
			int y_difference = std::max(tile_pos_b.y - tile_pos_a.y, 1);
			int x_difference = tile_pos_a.x - tile_pos_b.x;
			int min_x = std::min(tile_pos_a.x, tile_pos_b.x);
			int max_x = std::max(tile_pos_a.x, tile_pos_b.x);
			for(int y = tile_pos_a.y; y <= tile_pos_b.y; y++) {
				float t = float(y - tile_pos_a.y) / y_difference;
				for(int k = -2; k <= 2; k++) {
					int x = tile_pos_b.x + int(x_difference * t) + k;
					x = std::clamp(x, min_x, max_x);
					is_river_crossing |= is_river(river_data[x + y * map_size.x]);
				}
				if(is_river_crossing)
					break;
			}
		}

		uint8_t& buffer = world.province_adjacency_get_type(id);
		if(is_river_crossing)
			buffer |= province::border::river_crossing_bit;
	});
}

// Needs to be called after load_province_data for the mid points to set
// and load_border_data for the province_adjacencies to be set
std::vector<border_vertex> create_river_vertices(display_data const& data, parsers::scenario_building_context& context) {
	auto size = glm::ivec2(data.size_x, data.size_y);
	auto river_data = load_bmp(context, NATIVE("rivers.bmp"), size, 255);
	load_river_crossings(context, river_data, size);

	std::vector<border_vertex> river_vertices;

	std::vector<BorderDirection> current_row(size.x);
	std::vector<BorderDirection> last_row(size.x);
	auto map_size = glm::vec2(data.size_x, data.size_y);

	auto add_river = [&](uint32_t x0, uint32_t y0, bool river_u, bool river_d, bool river_r, bool river_l) {
		glm::vec2 map_pos(x0, y0);

		auto add_line_helper = [&](glm::vec2 pos1, glm::vec2 pos2, direction dir) {
			// if(!extend_if_possible(x0, 0, dir, last_row, current_row, size, river_vertices))
			add_line(map_pos, map_size, pos1, pos2, 0, x0, dir, river_vertices, current_row, 0.0);
		};

		if(river_l && river_u && !river_r && !river_d) { // Upper left
			add_line_helper(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 0.0f), direction::UP_LEFT);
		} else if(river_l && river_d && !river_r && !river_u) { // Lower left
			add_line_helper(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 1.0f), direction::DOWN_LEFT);
		} else if(river_r && river_u && !river_l && !river_d) { // Upper right
			add_line_helper(glm::vec2(1.0f, 0.5f), glm::vec2(0.5f, 0.0f), direction::UP_RIGHT);
		} else if(river_r && river_d && !river_l && !river_u) { // Lower right
			add_line_helper(glm::vec2(1.0f, 0.5f), glm::vec2(0.5f, 1.0f), direction::DOWN_LEFT);
		} else {
			if(river_u) {
				add_line_helper(glm::vec2(0.5f, 0.0f), glm::vec2(0.5f, 0.5f), direction::UP);
			}
			if(river_d) {
				add_line_helper(glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 1.0f), direction::DOWN);
			}
			if(river_l) {
				add_line_helper(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 0.5f), direction::LEFT);
			}
			if(river_r) {
				add_line_helper(glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 0.5f), direction::RIGHT);
			}
		}
	};

	for(int y = 1; y < size.y - 1; y++) {
		for(int x = 1; x < size.x - 1; x++) {
			auto river_center = is_river(river_data[(x + 0) + (y + 0) * size.x]);
			if(river_center) {
				auto river_u = is_river(river_data[(x + 0) + (y - 1) * size.x]);
				auto river_d = is_river(river_data[(x + 0) + (y + 1) * size.x]);
				auto river_r = is_river(river_data[(x + 1) + (y + 0) * size.x]);
				auto river_l = is_river(river_data[(x - 1) + (y + 0) * size.x]);
				add_river(x, y, river_u, river_d, river_r, river_l);
			}
		}

		// Move the border_direction rows a step down
		std::swap(last_row, current_row);
		std::fill(current_row.begin(), current_row.end(), BorderDirection{});
	}
	return river_vertices;
}

void display_data::update_borders(sys::state& state) {
	uint32_t border_id = 0;
	for(; border_id < state.world.province_adjacency_size(); border_id++) {
		auto& border = borders[border_id];
		border.type_flag = state.world.province_adjacency_get_type(dcon::province_adjacency_id(dcon::province_adjacency_id::value_base_t(border_id)));
	}
	for(; border_id < borders.size(); border_id++) {
		auto& border = borders[border_id];
		border.type_flag = province::border::test_bit;
	}
}

void setupVertexAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void const* offset) {
	glVertexAttribFormat(index, size, type, normalized, stride);
	glEnableVertexAttribArray(index);
	glVertexAttribBinding(index, 0);
}

void create_line_vbo(GLuint& vbo, std::vector<border_vertex>& vertices) {
	// Create and populate the border VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(border_vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(border_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(border_vertex, position_));
	// Set up vertex attribute format for the normal direction
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(border_vertex, normal_direction_));
	// Set up vertex attribute format for the direction
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(border_vertex, direction_));
	// Set up vertex attribute format for the border id
	glVertexAttribFormat(3, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(border_vertex, border_id_));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
	glVertexAttribBinding(3, 0);
}

void display_data::create_border_ogl_objects() {
	// Create and bind the VAO
	glGenVertexArrays(1, &border_vao);
	glBindVertexArray(border_vao);

	if(border_vertices.size() != 0)
		create_line_vbo(border_vbo, border_vertices);

	glGenVertexArrays(1, &river_vao);
	glBindVertexArray(river_vao);

	if(river_vertices.size() != 0)
		create_line_vbo(river_vbo, river_vertices);

	glBindVertexArray(0);
}

void display_data::create_meshes() {

	std::vector<map_vertex> land_vertices;

	auto add_quad = [map_size = glm::vec2(float(size_x), float(size_y))](std::vector<map_vertex>& vertices, glm::vec2 pos0, glm::vec2 pos1) {
		// Rescale the coordinate to 0-1
		pos0 /= map_size;
		pos1 /= map_size;

		// First vertex of the quad
		vertices.emplace_back(pos0.x, pos0.y);
		vertices.emplace_back(pos1.x, pos0.y);
		vertices.emplace_back(pos1.x, pos1.y);
		// Second vertex of the quad
		vertices.emplace_back(pos1.x, pos1.y);
		vertices.emplace_back(pos0.x, pos1.y);
		vertices.emplace_back(pos0.x, pos0.y);
	};

	glm::vec2 last_pos(0, 0);
	glm::vec2 pos(0, 0);
	glm::vec2 map_size(size_x, size_y);
	glm::vec2 sections(200, 200);
	for(int y = 0; y < sections.y; y++) {
		pos.y = last_pos.y + (map_size.y / sections.y);
		if(y == sections.y - 1)
			pos.y = map_size.y;

		last_pos.x = 0;
		for(int x = 0; x < sections.x; x++) {
			pos.x = last_pos.x + (map_size.x / sections.x);
			if(x == sections.x - 1)
				pos.x = map_size.x;
			add_quad(land_vertices, last_pos, pos);
			last_pos.x = pos.x;
		}
		last_pos.y = pos.y;
	}

	land_vertex_count = ((uint32_t)land_vertices.size());

	// Create and populate the VBO
	glGenBuffers(1, &land_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, land_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(map_vertex) * land_vertices.size(), &land_vertices[0], GL_STATIC_DRAW);

	// Create and bind the VAO
	glGenVertexArrays(1, &land_vao);
	glBindVertexArray(land_vao);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, land_vbo, 0, sizeof(map_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(map_vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribBinding(0, 0);

	glBindVertexArray(0);

	create_border_ogl_objects();
}

display_data::~display_data() {
	if(provinces_texture_handle)
		glDeleteTextures(1, &provinces_texture_handle);
	if(terrain_texture_handle)
		glDeleteTextures(1, &terrain_texture_handle);
	if(terrainsheet_texture_handle)
		glDeleteTextures(1, &terrainsheet_texture_handle);
	if(water_normal)
		glDeleteTextures(1, &water_normal);
	if(colormap_water)
		glDeleteTextures(1, &colormap_water);
	if(colormap_terrain)
		glDeleteTextures(1, &colormap_terrain);
	if(overlay)
		glDeleteTextures(1, &overlay);
	if(province_color)
		glDeleteTextures(1, &province_color);
	if(stripes_texture)
		glDeleteTextures(1, &stripes_texture);
	if(province_highlight)
		glDeleteTextures(1, &province_highlight);

	if(land_vao)
		glDeleteVertexArrays(1, &land_vao);
	if(border_vao)
		glDeleteVertexArrays(1, &border_vao);

	if(land_vbo)
		glDeleteBuffers(1, &land_vbo);
	if(border_vbo)
		glDeleteBuffers(1, &border_vbo);
	if(river_vao)
		glDeleteBuffers(1, &river_vbo);
	if(river_vbo)
		glDeleteBuffers(1, &river_vbo);

	if(terrain_shader)
		glDeleteProgram(terrain_shader);
	if(line_border_shader)
		glDeleteProgram(line_border_shader);
	if(line_river_shader)
		glDeleteProgram(line_river_shader);
}

std::optional<simple_fs::file> try_load_shader(simple_fs::directory& root, native_string_view name) {
	auto shader = simple_fs::open_file(root, name);
	if(!bool(shader))
		ogl::notify_user_of_fatal_opengl_error("Unable to open a necessary shader file");
	return shader;
}

GLuint create_program(simple_fs::file& vshader_file, simple_fs::file& fshader_file) {
	auto vshader_content = simple_fs::view_contents(vshader_file);
	auto vshader_string = std::string_view(vshader_content.data, vshader_content.file_size);
	auto fshader_content = simple_fs::view_contents(fshader_file);
	auto fshader_string = std::string_view(fshader_content.data, fshader_content.file_size);
	return ogl::create_program(vshader_string, fshader_string);
}

void display_data::load_shaders(simple_fs::directory& root) {
	// Map shaders
	auto map_vshader = try_load_shader(root, NATIVE("assets/shaders/map_v.glsl"));
	auto map_fshader = try_load_shader(root, NATIVE("assets/shaders/map_f.glsl"));

	terrain_shader = create_program(*map_vshader, *map_fshader);

	// Line shaders
	auto line_vshader = try_load_shader(root, NATIVE("assets/shaders/line_border_v.glsl"));
	auto line_border_fshader = try_load_shader(root, NATIVE("assets/shaders/line_border_f.glsl"));
	auto line_river_fshader = try_load_shader(root, NATIVE("assets/shaders/line_river_f.glsl"));

	line_border_shader = create_program(*line_vshader, *line_border_fshader);
	line_river_shader = create_program(*line_vshader, *line_river_fshader);
}

void display_data::render(glm::vec2 screen_size, glm::vec2 offset, float zoom, map_view map_view_mode, map_mode::mode active_map_mode, glm::mat3 globe_rotation, float time_counter) {

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, provinces_texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrain_texture_handle);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, terrainsheet_texture_handle);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, water_normal);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, colormap_water);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, colormap_terrain);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, overlay);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D_ARRAY, province_color);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, colormap_political);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, province_highlight);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, stripes_texture);

	// Load general shader stuff, used by both land and borders
	auto load_shader = [&](GLuint program) {
		glUseProgram(program);

		// uniform vec2 offset
		glUniform2f(0, offset.x + 0.f, offset.y);
		// uniform float aspect_ratio
		glUniform1f(1, screen_size.x / screen_size.y);
		// uniform float zoom
		glUniform1f(2, zoom);
		// uniform vec2 map_size
		glUniform2f(3, GLfloat(size_x), GLfloat(size_y));
		glUniformMatrix3fv(5, 1, GL_FALSE, glm::value_ptr(glm::mat3(globe_rotation)));

		GLuint vertex_subroutines;
		// calc_gl_position()
		if(map_view_mode == map_view::globe)
			vertex_subroutines = 0; // globe_coords()
		else
			vertex_subroutines = 1; // flat_coords()
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &vertex_subroutines);
	};

	load_shader(terrain_shader);

	{ // Land specific shader uniform
		glUniform1f(4, time_counter);
		// get_land()
		GLuint fragment_subroutines[2];
		if(active_map_mode == map_mode::mode::terrain)
			fragment_subroutines[0] = 0; // get_land_terrain()
		else if(zoom > 5)
			fragment_subroutines[0] = 1; // get_land_political_close()
		else
			fragment_subroutines[0] = 2; // get_land_political_far()
		// get_water()
		if(active_map_mode == map_mode::mode::terrain || zoom > 5)
			fragment_subroutines[1] = 3; // get_water_terrain()
		else
			fragment_subroutines[1] = 4; // get_water_political()
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, fragment_subroutines);
	}

	glBindVertexArray(land_vao);
	glDrawArrays(GL_TRIANGLES, 0, land_vertex_count);

	// Draw the rivers
	load_shader(line_river_shader);
	glUniform1f(4, 0.001f);
	glBindVertexArray(river_vao);
	glBindBuffer(GL_ARRAY_BUFFER, river_vbo);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)river_vertices.size());

	// Draw the borders
	load_shader(line_border_shader);
	glBindVertexArray(border_vao);

	glBindBuffer(GL_ARRAY_BUFFER, border_vbo);

	if(zoom > 8) {
		glUniform1f(4, 0.0013f);
		uint8_t visible_borders =
				(province::border::national_bit | province::border::coastal_bit | province::border::non_adjacent_bit |
						province::border::impassible_bit | province::border::state_bit | province::border::test_bit);

		std::vector<GLint> first;
		std::vector<GLsizei> count;
		for(auto& border : borders) {
			if((border.type_flag & visible_borders) == 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, &first[0], &count[0], GLsizei(count.size()));
	}

	if(zoom > 3.5f) {
		glUniform1f(4, 0.0018f);
		uint8_t visible_borders = (province::border::national_bit | province::border::coastal_bit |
				province::border::test_bit | province::border::non_adjacent_bit | province::border::impassible_bit);

		std::vector<GLint> first;
		std::vector<GLsizei> count;
		for(auto& border : borders) {
			if((border.type_flag & visible_borders) == 0 && (border.type_flag & province::border::state_bit) != 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, &first[0], &count[0], GLsizei(count.size()));
	}

	{
		glUniform1f(4, 0.0027f);
		uint8_t visible_borders = (province::border::national_bit | province::border::coastal_bit |
															 province::border::non_adjacent_bit | province::border::impassible_bit);

		std::vector<GLint> first;
		std::vector<GLsizei> count;
		for(auto& border : borders) {
			if(border.type_flag & visible_borders) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}

		glMultiDrawArrays(GL_TRIANGLES, &first[0], &count[0], GLsizei(count.size()));
	}

	/*
	// SCHOMBERT: enabling this will render any special borders you make with the test bit set
	{
		glUniform1f(4, 0.0016f);
		uint8_t visible_borders = (province::border::test_bit);

		std::vector<GLint> first;
		std::vector<GLsizei> count;
		for(auto& border : borders) {
			if(border.type_flag & visible_borders) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}

		glMultiDrawArrays(GL_TRIANGLES, &first[0], &count[0], GLsizei(count.size()));
	}
	*/

	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
}

GLuint load_province_map(std::vector<uint16_t>& province_index, uint32_t size_x, uint32_t size_y) {
	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		// Create a texture with only one byte color
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RG, GL_UNSIGNED_BYTE, &province_index[0]);
		glBindTexture(GL_TEXTURE_2D, 0);
		set_gltex_parameters(texture_handle, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	}
	return texture_handle;
}

void display_data::gen_prov_color_texture(GLuint texture_handle, std::vector<uint32_t> const& prov_color, uint8_t layers) {
	if(layers == 1) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);
	} else {
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);
	}
	uint32_t rows = ((uint32_t)prov_color.size()) / 256;
	uint32_t left_on_last_row = ((uint32_t)prov_color.size()) % 256;

	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t width = 256;
	uint32_t height = rows;

	if(layers == 1) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &prov_color[0]);
	} else {
		// Set the texture data for each layer
		for(int i = 0; i < layers; i++) {
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, i, width, height / layers, 1, GL_RGBA, GL_UNSIGNED_BYTE, &prov_color[i * (prov_color.size() / layers)]);
		}
	}

	x = 0;
	y = rows;
	width = left_on_last_row;
	height = 1;

	// SCHOMBERT: added a conditional to block reading from after the end in the case it is evenly divisible by 256
	// SCHOMBERT: that looks right to me, but I don't fully understand the intent
	if(left_on_last_row > 0 && layers == 1)
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &prov_color[rows * 256]);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void display_data::set_selected_province(sys::state& state, dcon::province_id prov_id) {
	std::vector<uint32_t> province_highlights(state.world.province_size() + 1);
	if(prov_id)
		province_highlights[province::to_map_id(prov_id)] = 0x2B2B2B2B;
	gen_prov_color_texture(province_highlight, province_highlights);
}

void display_data::set_province_color(std::vector<uint32_t> const& prov_color) { gen_prov_color_texture(province_color, prov_color, 2); }

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
	river_vertices = create_river_vertices(*this, context);
}

GLuint load_dds_texture(simple_fs::directory const& dir, native_string_view file_name) {
	auto file = simple_fs::open_file(dir, file_name);
	auto content = simple_fs::view_contents(*file);
	uint32_t size_x, size_y;
	uint8_t const* data = (uint8_t const*)(content.data);
	return ogl::SOIL_direct_load_DDS_from_memory(data, content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS);
}

void display_data::load_map(sys::state& state) {
	auto root = simple_fs::get_root(state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto map_terrain_dir = simple_fs::open_directory(map_dir, NATIVE("terrain"));

	load_shaders(root);

	terrain_texture_handle = make_gl_texture(&terrain_id_map[0], size_x, size_y, 1);
	set_gltex_parameters(terrain_texture_handle, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	create_meshes();

	provinces_texture_handle = load_province_map(province_id_map, size_x, size_y);

	auto texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.tga"));
	terrainsheet_texture_handle = load_texture_array_from_file(*texturesheet, 8, 8);

	water_normal = load_dds_texture(map_terrain_dir, NATIVE("sea_normal.dds"));
	colormap_water = load_dds_texture(map_terrain_dir, NATIVE("colormap_water.dds"));
	colormap_terrain = load_dds_texture(map_terrain_dir, NATIVE("colormap.dds"));
	colormap_political = load_dds_texture(map_terrain_dir, NATIVE("colormap_political.dds"));
	overlay = load_dds_texture(map_terrain_dir, NATIVE("map_overlay_tile.dds"));
	stripes_texture = load_dds_texture(map_terrain_dir, NATIVE("stripes.dds"));
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get the province_color handle
	// province_color is an array of 2 textures, one for province and the other for stripes
	glGenTextures(1, &province_color);
	glBindTexture(GL_TEXTURE_2D_ARRAY, province_color);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 2);
	set_gltex_parameters(province_color, GL_TEXTURE_2D_ARRAY, GL_NEAREST, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get the province_highlight handle
	glGenTextures(1, &province_highlight);
	glBindTexture(GL_TEXTURE_2D, province_highlight);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
	set_gltex_parameters(province_highlight, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	uint32_t province_size = state.world.province_size() + 1;
	province_size += 256 - province_size % 256;

	std::vector<uint32_t> testHighlight(province_size);
	std::vector<uint32_t> testColor(province_size * 4);
	gen_prov_color_texture(province_highlight, testHighlight);

	for(uint32_t i = 0; i < testHighlight.size(); ++i) {
		testHighlight[i] = 255;
	}

	for(uint32_t i = 0; i < testColor.size(); ++i) {
		testColor[i] = 255;
	}

	set_province_color(testColor);
}

} // namespace map
