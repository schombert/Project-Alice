#include "map.hpp"

#include "province.hpp"
#include "system_state.hpp"
#include "parsers_declarations.hpp"

namespace map {
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
	BorderDirection() {
	};
	struct Information {
		Information() {
		};
		Information(int32_t index_, int32_t id_) : index{ index_ }, id{ id_ } {
		};
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

// Get the index of the border from the province ids and create a new one if one doesn't exist
int32_t get_border_index(uint16_t map_province_id1, uint16_t map_province_id2, parsers::scenario_building_context& context) {
	auto province_id1 = province::from_map_id(map_province_id1);
	auto province_id2 = province::from_map_id(map_province_id2);
	auto border_index = context.state.world.get_province_adjacency_by_province_pair(province_id1, province_id2);
	if(!border_index)
		border_index = context.state.world.force_create_province_adjacency(province_id1, province_id2);
	if(!province_id1 || !province_id2) {
		context.state.world.province_adjacency_set_type(border_index, province::border::impassible_bit);
	}
	return border_index.index();
}

void add_border(
	const uint32_t& x0,
	const uint32_t& y0,
	const uint16_t& id_ul,
	const uint16_t& id_ur,
	const uint16_t& id_dl,
	const uint16_t& id_dr,
	std::vector<std::vector<border_vertex>>& borders_list_vertices,
	std::vector<BorderDirection>& current_row,
	std::vector<BorderDirection>& last_row,
	parsers::scenario_building_context& context,
	glm::vec2& map_size)
{
	uint8_t diff_u = id_ul != id_ur;
	uint8_t diff_d = id_dl != id_dr;
	uint8_t diff_l = id_ul != id_dl;
	uint8_t diff_r = id_ur != id_dr;

	glm::vec2 map_pos(x0, y0);

	auto add_line_helper = [&](glm::vec2 pos1, glm::vec2 pos2, uint16_t id1, uint16_t id2, direction dir) {
		auto border_index = get_border_index(id1, id2, context);
		if(uint32_t(border_index) >= borders_list_vertices.size())
			borders_list_vertices.resize(border_index + 1);
		auto& current_border_vertices = borders_list_vertices[border_index];
		if(!extend_if_possible(x0, border_index, dir, last_row, current_row, map_size, current_border_vertices))
			add_line(map_pos, map_size, pos1, pos2, border_index, x0, dir, current_border_vertices, current_row, 0.5f);
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
}

void display_data::load_border_data(parsers::scenario_building_context& context) {
	border_vertices.clear();

	glm::vec2 map_size(size_x, size_y);

	std::vector<std::vector<border_vertex>> borders_list_vertices;

	// The borders of the current row and last row
	std::vector<BorderDirection> current_row(size_x);
	std::vector<BorderDirection> last_row(size_x);


	for(uint32_t y = 0; y < size_y - 1; y++) {
		for(uint32_t x = 0; x < size_x - 1; x++) {
			auto prov_id_ul = province_id_map[(x + 0) + (y + 0) * size_x];
			auto prov_id_ur = province_id_map[(x + 1) + (y + 0) * size_x];
			auto prov_id_dl = province_id_map[(x + 0) + (y + 1) * size_x];
			auto prov_id_dr = province_id_map[(x + 1) + (y + 1) * size_x];
			if(prov_id_ul != prov_id_ur || prov_id_ul != prov_id_dl || prov_id_ul != prov_id_dr) {
				add_border(x, y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr, borders_list_vertices, current_row, last_row, context, map_size);
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
				add_border((size_x - 1), y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr, borders_list_vertices, current_row, last_row, context, map_size);

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

	/*
	// identify and filter out lakes
	// this has been superseded by a more general connectivity algorithm where only the connected set of water provinces of the greatest size counts as the ocean

	for(auto k = uint32_t(context.state.province_definitions.first_sea_province.index()); k < context.state.world.province_size(); ++k) {
		dcon::province_id p{ dcon::province_id::value_base_t(k) };
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
	*/

	for(auto p : context.state.world.in_province) {
		auto rng = p.get_province_adjacency();
		int32_t num_adj = int32_t(rng.end() - rng.begin());
		auto original_province = context.prov_id_to_original_id_map[p].id;
		// assert(num_adj < 30);
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
				current_row, 0.5f);
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

bool is_river(uint8_t river_data) {
	return river_data < 16;
}

// Set the river crossing bit for the province adjencencies
// Will march a line between each adjecent province centroid. If it hits a river it will set the bit
void load_river_crossings(parsers::scenario_building_context& context, std::vector<uint8_t> const& river_data, glm::ivec2 map_size) {
	auto& world = context.state.world;
	world.for_each_province_adjacency([&](dcon::province_adjacency_id id) {
		auto frel = fatten(world, id);
		auto prov_a = frel.get_connected_provinces(0);
		auto prov_b = frel.get_connected_provinces(1);

		if(!prov_a || !prov_b)
			return; // goto next

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
					if(x < 0 || y < 0)
						continue;
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
					if(x < 0 || y < 0)
						continue;
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
std::vector<border_vertex> create_river_vertices(display_data const& data, parsers::scenario_building_context& context, std::vector<uint8_t> const& river_data) {
	auto size = glm::ivec2(data.size_x, data.size_y);
	load_river_crossings(context, river_data, size);

	std::vector<border_vertex> river_vertices;

	std::vector<BorderDirection> current_row(size.x);
	std::vector<BorderDirection> last_row(size.x);
	auto map_size = glm::vec2(data.size_x, data.size_y);

	auto add_river = [&](uint32_t x0, uint32_t y0, bool river_u, bool river_d, bool river_r, bool river_l) {
		glm::vec2 map_pos(x0, y0);

		auto add_line_helper = [&](glm::vec2 pos1, glm::vec2 pos2, direction dir) {
			// if(!extend_if_possible(x0, 0, dir, last_row, current_row, size, river_vertices))
			add_line(map_pos, map_size, pos1, pos2, 0, x0, dir, river_vertices, current_row, 0.0f);
			};

		if(river_l && river_u && !river_r && !river_d) { // Upper left
			add_line_helper(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 0.0f), direction::UP_LEFT);
		} else if(river_l && river_d && !river_r && !river_u) { // Lower left
			add_line_helper(glm::vec2(0.0f, 0.5f), glm::vec2(0.5f, 1.0f), direction::DOWN_LEFT);
		} else if(river_r && river_u && !river_l && !river_d) { // Upper right
			add_line_helper(glm::vec2(1.0f, 0.5f), glm::vec2(0.5f, 0.0f), direction::UP_RIGHT);
		} else if(river_r && river_d && !river_l && !river_u) { // Lower right
			add_line_helper(glm::vec2(1.0f, 0.5f), glm::vec2(0.5f, 1.0f), direction::DOWN_RIGHT);
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
}
