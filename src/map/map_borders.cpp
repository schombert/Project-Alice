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

struct border_direction {
	border_direction() {}
	struct information {
		information() {}
		information(int32_t index_, int32_t id_) : index{ index_ }, id{ id_ } {}
		int32_t index = -1;
		int32_t id = -1;
	};
	information up;
	information down;
	information left;
	information right;
};

// Create a new vertices to make a line segment
void add_line(glm::vec2 map_pos, glm::vec2 map_size, glm::vec2 offset1, glm::vec2 offset2, int32_t border_id, uint32_t x, direction dir, std::vector<curved_line_vertex>& line_vertices, std::vector<border_direction>& current_row, float offset) {
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
	line_vertices.emplace_back(pos1, normal_direction, direction, glm::vec2(0.f, 0.f), float(border_id));
	line_vertices.emplace_back(pos1, -normal_direction, direction, glm::vec2(0.f, 1.f), float(border_id));
	line_vertices.emplace_back(pos2, -normal_direction, -direction, glm::vec2(1.f, 1.f), float(border_id));
	// Second vertex of the line segment
	line_vertices.emplace_back(pos2, -normal_direction, -direction, glm::vec2(1.f, 1.f), float(border_id));
	line_vertices.emplace_back(pos2, normal_direction, -direction, glm::vec2(1.f, 0.f), float(border_id));
	line_vertices.emplace_back(pos1, normal_direction, direction, glm::vec2(0.f, 0.f), float(border_id));

	border_direction::information direction_information(border_index, border_id);
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
bool extend_if_possible(uint32_t x, int32_t border_id, direction dir, std::vector<border_direction>& last_row, std::vector<border_direction>& current_row, glm::vec2 map_size, std::vector<curved_line_vertex>& border_vertices) {
	if((dir & direction::LEFT) != 0 && x == 0)
		return false;

	border_direction::information direction_information;
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
	uint32_t x0,
	uint32_t y0,
	uint16_t id_ul,
	uint16_t id_ur,
	uint16_t id_dl,
	uint16_t id_dr,
	std::vector<std::vector<curved_line_vertex>>& borders_list_vertices,
	std::vector<border_direction>& current_row,
	std::vector<border_direction>& last_row,
	parsers::scenario_building_context& context,
	glm::vec2 map_size)
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

	std::vector<std::vector<curved_line_vertex>> borders_list_vertices;
	// The borders of the current row and last row
	std::vector<border_direction> current_row(size_x);
	std::vector<border_direction> last_row(size_x);
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


					auto aval = context.state.world.get_province_adjacency_by_province_pair(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_ur));
					if((context.state.world.province_adjacency_get_type(aval) & province::border::non_adjacent_bit) != 0)
						context.state.world.province_adjacency_get_type(aval) &= ~(province::border::non_adjacent_bit | province::border::impassible_bit);
					
				}
				if(prov_id_ul != prov_id_dl && prov_id_dl != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dl));

					auto aval = context.state.world.get_province_adjacency_by_province_pair(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dl));
					if((context.state.world.province_adjacency_get_type(aval) & province::border::non_adjacent_bit) != 0)
							context.state.world.province_adjacency_get_type(aval) &= ~(province::border::non_adjacent_bit | province::border::impassible_bit);
					
				}
				if(prov_id_ul != prov_id_dr && prov_id_dr != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));

					auto aval = context.state.world.get_province_adjacency_by_province_pair(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));
					if((context.state.world.province_adjacency_get_type(aval) & province::border::non_adjacent_bit) != 0)
						context.state.world.province_adjacency_get_type(aval) &= ~(province::border::non_adjacent_bit | province::border::impassible_bit);
					
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
		std::fill(current_row.begin(), current_row.end(), border_direction{});
	}

	for(auto p : context.state.world.in_province) {
		auto rng = p.get_province_adjacency();
		int32_t num_adj = int32_t(rng.end() - rng.begin());
		auto original_province = context.prov_id_to_original_id_map[p].id;
		// assert(num_adj < 30);
	}

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
		auto frel = dcon::fatten(world, id);
		auto prov_a = frel.get_connected_provinces(0);
		auto prov_b = frel.get_connected_provinces(1);

		if(!prov_a || !prov_b)
			return; // goto next

		glm::vec2 mid_point_a = world.province_get_mid_point(prov_a.id);
		glm::vec2 mid_point_b = world.province_get_mid_point(prov_b.id);
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

struct river_vertex {
	float x = 0.f;
	float y = 0.f;
	bool keep = false;
	// TODO: add padding
	river_vertex(float x_, float y_, bool keep_) : x(x_), y(y_), keep(keep_) { }
	bool operator==(river_vertex const& o) const {
		return x == o.x && y == o.y && keep == o.keep;
	}
	glm::vec2 to_vec2() const {
		return glm::vec2(x, y);
	}
};

void river_explore_helper(uint32_t x, uint32_t y, std::vector<std::vector<river_vertex>>& rivers, std::vector<uint8_t> const& river_data, std::vector<bool>& marked, glm::ivec2 size) {
	assert(!rivers.empty());
	uint32_t ic = (x + 0) + (y + 0) * size.x;
	if(!marked[ic] && is_river(river_data[ic])) {
		marked[ic] = true;
		rivers.back().emplace_back(float(x), float(y), rivers.back().empty());
		uint32_t branch_count = 0;
		for(int32_t ty = -1; ty <= 1; ty++) {
			for(int32_t tx = -1; tx <= 1; tx++) {
				// We are checking for the cuadrant of:
				// [ -1 -1 ] [ +0 -1 ] [ +1 -1 ]
				// [ -1 +0 ] [ +0 +0 ] [ +1 +0 ]
				// [ -1 +1 ] [ +0 +1 ] [ +1 +1 ]
				// We do not want (-1, -1), (1, -1), (-1, +1) and (+1, +1)
				// This means that there must be atleast one zero
				// To obtain this we multiply x*y, getting the following:
				// [  1 ] [  0 ] [ -1 ]
				// [  0 ] [  0 ] [  0 ]
				// [ -1 ] [  0 ] [  1 ]
				if(tx * ty != 0 || (tx == 0 && ty == 0))
					continue;
				uint32_t index = (x + tx) + (y + ty) * size.x;
				if(!marked[index] && is_river(river_data[index])) {
					if(branch_count > 0) {
						// this river is a branch
						glm::vec2 prev_vertice{ float(x), float(y) };
						if(!rivers.back().empty()) {
							rivers.back().back().keep = true;
						}
						rivers.push_back(std::vector<river_vertex>());
						rivers.back().emplace_back(float(x), float(y), true);
					}
					river_explore_helper(uint32_t(int32_t(x) + tx), uint32_t(int32_t(y) + ty), rivers, river_data, marked, size);
					branch_count++;
				}
			}
		}
		if(branch_count == 0)
			rivers.push_back(std::vector<river_vertex>()); // No match, but has a center, so make new river
	}
}

void add_arrow(std::vector<curved_line_vertex>& buffer, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 prev_normal_dir, glm::vec2 curr_normal_dir, glm::vec2 curr_dir, float progress);

// Needs to be called after load_province_data for the mid points to set
// and load_border_data for the province_adjacencies to be set
std::vector<curved_line_vertex> create_river_vertices(display_data const& data, parsers::scenario_building_context& context, std::vector<uint8_t> const& river_data) {
	glm::vec2 size{ float(data.size_x), float(data.size_y) };
	assert(size.x >= 1.f && size.y >= 1.f);

	load_river_crossings(context, river_data, size);

	std::vector<curved_line_vertex> river_vertices;
	std::vector<std::vector<river_vertex>> rivers;
	rivers.push_back(std::vector<river_vertex>());
	std::vector<bool> marked(data.size_x * data.size_y, false);
	for(uint32_t y = 1; y < uint32_t(size.y) - 1; y++) {
		for(uint32_t x = 1; x < uint32_t(size.x) - 1; x++) {
			river_explore_helper(x, y, rivers, river_data, marked, size);
		}
	}

	// remove empty rivers or rivers with 1 vertice
	for(uint32_t i = 0; i < rivers.size(); i++) {
		if(rivers[i].size() <= 1) {
			rivers.erase(rivers.begin() + i);
			--i;
		}
	}

	for(auto& river : rivers) {
		if(river.size() == 2)
			continue;
		for(uint32_t i = 1; i < river.size() - 1; i++) {
			if(river[i + 1].keep || river[i].keep)
				continue;
			if(std::abs(river[i].x - river[i + 1].x) <= 4.f && std::abs(river[i].y - river[i + 1].y) <= 4.f)
				river[i + 1] = river[i];
		}
		// Ensure no duplicates
		river.erase(std::unique(river.begin(), river.end()), river.end());
	}

	for(const auto& river : rivers) {
		/*
		if(auto rs = river.size(); rs > 1) { //last back element is used as "initiator"
			glm::vec2 current_pos = river[rs - 1].to_vec2();
			glm::vec2 next_pos = put_in_local(river[rs - 2].to_vec2(), current_pos, size.x);
			glm::vec2 prev_perpendicular = glm::normalize(next_pos - current_pos);
			for(auto i = rs; i-- > 0;) {
				glm::vec2 next_perpendicular{ 0.0f, 0.0f };
				next_pos = put_in_local(river[i].to_vec2(), current_pos, size.x);
				if(i > 0) {
					glm::vec2 next_next_pos = put_in_local(river[i - 1].to_vec2(), next_pos, size.x);
					glm::vec2 a_per = glm::normalize(next_pos - current_pos);
					glm::vec2 b_per = glm::normalize(next_pos - next_next_pos);
					glm::vec2 temp = a_per + b_per;
					if(glm::length(temp) < 0.00001f) {
						next_perpendicular = -a_per;
					} else {
						next_perpendicular = glm::normalize(glm::vec2{ -temp.y, temp.x });
						if(glm::dot(a_per, -next_perpendicular) < glm::dot(a_per, next_perpendicular)) {
							next_perpendicular *= -1.0f;
						}
					}
				} else {
					next_perpendicular = glm::normalize(current_pos - next_pos);
				}
				add_bezier_to_buffer(river_vertices, current_pos, next_pos, prev_perpendicular, next_perpendicular, i == int32_t(rs - 1) ? 1.f : 0.0f, i == 0, size.x, size.y, 2);
				prev_perpendicular = -1.0f * next_perpendicular;
				current_pos = river[i].to_vec2();
			}
		}
		*/
		auto progress = 0.f;
		glm::vec2 prev_normal_dir;
		{
			auto prev_pos = river[0].to_vec2();
			auto next_pos = river[1].to_vec2();
			auto direction1 = glm::normalize(next_pos - prev_pos);
			prev_normal_dir = glm::vec2(-direction1.y, direction1.x);
		}
		for(int i = 0; i < static_cast<int>(river.size()) - 2; i++) {
			auto pos1 = river[i].to_vec2();
			auto pos2 = river[i + 1].to_vec2();
			auto pos3 = river[i + 2].to_vec2();
			glm::vec2 curr_dir = glm::normalize(pos2 - pos1);
			glm::vec2 next_dir = glm::normalize(pos3 - pos2);
			glm::vec2 average_direction = normalize(curr_dir + next_dir);
			glm::vec2 curr_normal_dir = glm::vec2(-average_direction.y, average_direction.x);
			if(pos1 == pos3) {
				prev_normal_dir = -glm::vec2(-curr_dir.y, curr_dir.x);
				continue;
			}

			// Rescale the coordinate to 0-1
			pos1 /= size;
			pos2 /= size;

			int32_t border_index = int32_t(river_vertices.size());

			float current_progress = i == 0 ? progress : 0;
			add_arrow(river_vertices, pos1, pos2, prev_normal_dir, curr_normal_dir, curr_dir, current_progress);

			prev_normal_dir = curr_normal_dir;
		}
		{
			int i = static_cast<int>(river.size()) - 2;
			auto pos1 = river[i].to_vec2();
			auto pos2 = river[i + 1].to_vec2();

			glm::vec2 direction = glm::normalize(pos2 - pos1);
			glm::vec2 curr_normal_dir = glm::vec2(-direction.y, direction.x);

			// Rescale the coordinate to 0-1
			pos1 /= size;
			pos2 /= size;

			int32_t border_index = int32_t(river_vertices.size());

			float current_progress = i == 0 ? progress : 0;
			add_arrow(river_vertices, pos1, pos2, prev_normal_dir, curr_normal_dir, direction, current_progress);

			// Type for arrow
			float type = 1;
			// First vertex of the line segment
			river_vertices.emplace_back(pos2, +curr_normal_dir, +direction, glm::vec2(0.0f, 0.0f), type);
			river_vertices.emplace_back(pos2, -curr_normal_dir, +direction, glm::vec2(0.0f, 1.0f), type);
			river_vertices.emplace_back(pos2, -curr_normal_dir, -direction, glm::vec2(1.0f, 1.0f), type);
			// Second vertex of the line segment
			river_vertices.emplace_back(pos2, -curr_normal_dir, -direction, glm::vec2(1.0f, 1.0f), type);
			river_vertices.emplace_back(pos2, +curr_normal_dir, -direction, glm::vec2(1.0f, 0.0f), type);
			river_vertices.emplace_back(pos2, +curr_normal_dir, +direction, glm::vec2(0.0f, 0.0f), type);
		}
	}
	return river_vertices;
}
}
