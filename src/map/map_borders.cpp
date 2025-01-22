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

enum class diagonal_border : uint8_t {
	UP_LEFT = 0x01,
	UP_RIGHT = uint8_t(0x01 << 2),
	DOWN_LEFT = uint8_t(0x01 << 4),
	DOWN_RIGHT = uint8_t(0x01 << 6),
	NOTHING = 0,
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

void smooth_points(std::vector<glm::vec2>& vertices) {
	std::vector<glm::vec2> vertices_copy = vertices;

	bool wrap_around = false;
	if(glm::distance(vertices.back(), vertices.front()) < 0.001f) {
		wrap_around = true;
	}

	auto start = int(0);
	auto end = start + int(vertices.size());

	auto start_iteration = start + 1;
	auto end_iteration = end - 1;

	if(wrap_around) {
		start_iteration = start;
		end_iteration = end;
	}

	for(int i = start_iteration; i < end_iteration; i++) {
		glm::vec2 new_position{ 0.f, 0.f };
		float count = 0.f;
		bool smooth = true;
		for(int shift = -2; shift <= 2; shift++) {
			auto shifted_index = i + shift;
			if(wrap_around) {
				if(shifted_index < start) {
					shifted_index = end + shifted_index - start;
				}
				if(shifted_index >= end) {
					shifted_index = start + shifted_index - end;
				}
			} else {
				if(shifted_index < start) {
					continue;
				};
				if(shifted_index >= end) {
					continue;
				};
			}
			auto weight = 1.f / (float(std::abs(shift)) + 1.f);

			if(shift == 0) {
				weight += 10.f / vertices.size() / vertices.size();
			}

			count += weight;
			new_position += (vertices_copy[shifted_index]) * weight;
		}
		if((count > 0) && smooth) {
			vertices[i] = new_position / count;
		}		
	}

	if(wrap_around) {
		vertices[0] = vertices.back();
	}
}

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

void display_data::load_border_data(parsers::scenario_building_context& context) {
	border_vertices.clear();

	glm::vec2 map_size(size_x, size_y);

	diagonal_borders = std::vector<uint8_t>(size_x * size_y, 0);

	// The borders of the current row and last row
	for(uint32_t y = 0; y < size_y - 1; y++) {
		for(uint32_t x = 0; x < size_x - 1; x++) {
			auto prov_id_ul = province_id_map[(x + 0) + (y + 0) * size_x];
			auto prov_id_ur = province_id_map[(x + 1) + (y + 0) * size_x];
			auto prov_id_dl = province_id_map[(x + 0) + (y + 1) * size_x];
			auto prov_id_dr = province_id_map[(x + 1) + (y + 1) * size_x];

			if(prov_id_ur == prov_id_ul && prov_id_dl == prov_id_ul && prov_id_dr != prov_id_ur) { // Upper left
				diagonal_borders[(x + 1) + (y + 1) * uint32_t(map_size.x)] |= uint8_t(diagonal_border::UP_LEFT);
			}
			if(prov_id_ul == prov_id_dl && prov_id_dl == prov_id_dr && prov_id_ur != prov_id_dr) { // Lower left
				diagonal_borders[(x + 1) + y * uint32_t(map_size.x)] |= uint8_t(diagonal_border::DOWN_LEFT);
			}
			if(prov_id_ul == prov_id_ur && prov_id_ur == prov_id_dr && prov_id_dl != prov_id_ul) { // Upper right
				diagonal_borders[x + (y + 1) * uint32_t(map_size.x)] |= uint8_t(diagonal_border::UP_RIGHT);
			}
			if(prov_id_dl == prov_id_dr && prov_id_ur == prov_id_dr && prov_id_ul != prov_id_dl) { // Lower right
				diagonal_borders[x + y * uint32_t(map_size.x)] |= uint8_t(diagonal_border::DOWN_RIGHT);
			}
			if(prov_id_ul == prov_id_dr && prov_id_ur == prov_id_dl && prov_id_ul != prov_id_ur) {
				if((prov_id_ul >= province::to_map_id(context.state.province_definitions.first_sea_province) || prov_id_ul == 0)
					&& (prov_id_ur < province::to_map_id(context.state.province_definitions.first_sea_province) && prov_id_ur != 0)) {

					diagonal_borders[x + (y + 1) * uint32_t(map_size.x)] |= uint8_t(diagonal_border::UP_RIGHT);
					diagonal_borders[(x + 1) + y * uint32_t(map_size.x)] |= uint8_t(diagonal_border::DOWN_LEFT);

				} else if((prov_id_ur >= province::to_map_id(context.state.province_definitions.first_sea_province) || prov_id_ur == 0)
					&& (prov_id_ul < province::to_map_id(context.state.province_definitions.first_sea_province) && prov_id_ul != 0)) {

					diagonal_borders[(x + 1) + (y + 1) * uint32_t(map_size.x)] |= uint8_t(diagonal_border::UP_LEFT);
					diagonal_borders[x + y * uint32_t(map_size.x)] |= uint8_t(diagonal_border::DOWN_RIGHT);
				}
			}

			if(prov_id_ul != prov_id_ur || prov_id_ul != prov_id_dl || prov_id_ul != prov_id_dr) {
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

			if(prov_id_ur == prov_id_ul && prov_id_dl == prov_id_ul && prov_id_dr != prov_id_ur) { // Upper left
				diagonal_borders[0 + (y + 1) * uint32_t(map_size.x)] |= uint8_t(diagonal_border::UP_LEFT);
			}
			if(prov_id_ul == prov_id_dl && prov_id_dl == prov_id_dr && prov_id_ur != prov_id_dr) { // Lower left
				diagonal_borders[0 + y * uint32_t(map_size.x)] |= uint8_t(diagonal_border::DOWN_LEFT);
			}
			if(prov_id_ul == prov_id_ur && prov_id_ur == prov_id_dr && prov_id_dl != prov_id_ul) { // Upper right
				diagonal_borders[(size_x - 1) + (y + 1) * uint32_t(map_size.x)] |= uint8_t(diagonal_border::UP_RIGHT);
			}
			if(prov_id_dl == prov_id_dr && prov_id_ur == prov_id_dr && prov_id_ul != prov_id_dl) { // Lower right
				diagonal_borders[(size_x - 1) + y * uint32_t(map_size.x)] |= uint8_t(diagonal_border::DOWN_RIGHT);
			}
			if(prov_id_ul == prov_id_dr && prov_id_ur == prov_id_dl && prov_id_ul != prov_id_ur) {
				if((prov_id_ul >= province::to_map_id(context.state.province_definitions.first_sea_province) || prov_id_ul == 0)
					&& (prov_id_ur < province::to_map_id(context.state.province_definitions.first_sea_province) && prov_id_ur != 0)) {

					diagonal_borders[(size_x - 1) + (y + 1) * uint32_t(map_size.x)] |= uint8_t(diagonal_border::UP_RIGHT);
					diagonal_borders[0 + y * uint32_t(map_size.x)] |= uint8_t(diagonal_border::DOWN_LEFT);

				} else if((prov_id_ur >= province::to_map_id(context.state.province_definitions.first_sea_province) || prov_id_ur == 0)
					&& (prov_id_ul < province::to_map_id(context.state.province_definitions.first_sea_province) && prov_id_ul != 0)) {

					diagonal_borders[0 + (y + 1) * uint32_t(map_size.x)] |= uint8_t(diagonal_border::UP_LEFT);
					diagonal_borders[(size_x - 1) + y * uint32_t(map_size.x)] |= uint8_t(diagonal_border::DOWN_RIGHT);
				}
			}

			if(prov_id_ul != prov_id_ur || prov_id_ul != prov_id_dl || prov_id_ul != prov_id_dr) {

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
	}
}

bool is_river(uint8_t river_data) {
	return river_data < 250;
}
int32_t river_width(uint8_t river_data) {
	auto result = 255 - (int32_t)(river_data) + 40;
	assert(result >= 0);
	return result;
}
bool is_river_source(uint8_t river_data) {
	return river_data == 0;
}
bool is_river_merge(uint8_t river_data) {
	return river_data == 1;
}

uint16_t display_data::safe_get_province(glm::ivec2 pt) {
	while(pt.x < 0) {
		pt.x += int32_t(size_x);
	}
	while(pt.x >= int32_t(size_x)) {
		pt.x -= int32_t(size_x);
	}
	if(pt.y < 0) {
		pt.y = 0;
	}
	if(pt.y >= int32_t(size_y)) {
		pt.y = int32_t(size_y - 1);
	}
	return province_id_map[pt.x + pt.y * size_x];
}

bool coastal_point(sys::state& state, uint16_t a, uint16_t b) {
	bool a_sea = a == 0 || province::from_map_id(a).index() >= state.province_definitions.first_sea_province.index();
	bool b_sea = b == 0 || province::from_map_id(b).index() >= state.province_definitions.first_sea_province.index();
	return a_sea != b_sea;
}

bool order_indifferent_compare(uint16_t a, uint16_t b, uint16_t c, uint16_t d) {
	return (a == c && b == d) || (a == d && b == c);
}

std::vector<glm::vec2> make_border_section(display_data& dat, sys::state& state, std::vector<bool>& visited, uint16_t prov_prim, uint16_t prov_sec, int32_t start_x, int32_t start_y) {
	std::vector<glm::vec2> points;

	auto add_next = [&](glm::ivec2 prev, glm::ivec2 shift, glm::ivec2 prev_shift, bool& next_found, bool corner_candidate) {
		if(next_found)
			return glm::ivec2(0, 0);

		auto i = prev.x + shift.x;
		auto j = prev.y + shift.y;
		auto prev_i = prev.x;
		auto prev_j = prev.y;

		bool corner = corner_candidate && (shift.x != prev_shift.x) && (shift.y != prev_shift.y) && !(prev_shift.y == 0 && prev_shift.x == 0);

		if(visited[i + j * dat.size_x])
			return glm::ivec2(0, 0);
		if(j % 2 == 0) {
			if(order_indifferent_compare(prov_prim, prov_sec, dat.safe_get_province(glm::ivec2(i, j / 2)), dat.safe_get_province(glm::ivec2(i - 1, j / 2)))) {
				visited[i + j * dat.size_x] = true;

				if(corner) {
					float prev_x = float(prev_i) + 0.5f;
					float prev_y = 0.5f + float(prev_j) / 2.0f;

					float next_x = float(i);
					float next_y = 0.5f + float(j) / 2.0f;

					points.push_back(glm::vec2((prev_x + next_x) / 2.f, prev_y));
					points.push_back(glm::vec2(next_x, (prev_y + next_y) / 2.f));
				}

				points.push_back(glm::vec2(float(i), 0.5f + float(j) / 2.0f));
				next_found = true;
				return glm::ivec2(i, j);
			}
		} else {
			if(order_indifferent_compare(prov_prim, prov_sec, dat.safe_get_province(glm::ivec2(i, j / 2)), dat.safe_get_province(glm::ivec2(i, j / 2 + 1)))) {
				visited[i + j * dat.size_x] = true;

				if(corner) {
					float prev_x = float(prev_i);
					float prev_y = 0.5f + float(prev_j) / 2.0f;

					float next_x = float(i) + 0.5f;
					float next_y = 0.5f + float(j) / 2.0f;

					points.push_back(glm::vec2(prev_x, (prev_y + next_y) / 2.f));
					points.push_back(glm::vec2((prev_x + next_x) / 2.f, next_y));
				}

				points.push_back(glm::vec2(float(i) + 0.5f, 0.5f + float(j) / 2.0f));
				next_found = true;
				return glm::ivec2(i, j);
			}
		}

		return glm::ivec2(0, 0);
	};


	points.push_back(glm::vec2(float(start_x) + (start_y % 2 == 0 ? 0.0f : 0.5f), 0.5f + float(start_y) / 2.0f));
	visited[start_x + start_y * dat.size_x] = true;

	glm::ivec2 current{ start_x, start_y };
	glm::ivec2 prev_direction{ 0, 0 };

	bool progress = false;
	// clockwise
	do {
		progress = false;
		glm::ivec2 temp{ 0, 0 };

		if(current.y % 2 == 0) {
			bool left_is_s = dat.safe_get_province(glm::ivec2(current.x - 1, current.y / 2)) == prov_sec;
			if(left_is_s) {
				temp += add_next(current, { 0, 1 }, prev_direction, progress, true);
				temp += add_next(current, { 0, 2 }, prev_direction, progress, false);
				temp += add_next(current, { -1, 1 }, prev_direction, progress, true);
			} else {
				temp += add_next(current, { -1, -1}, prev_direction, progress, true);
				temp += add_next(current, { 0, -2}, prev_direction, progress, false);
				temp += add_next(current, { 0, -1}, prev_direction, progress, true);
			}
		} else {
			bool top_is_s = dat.safe_get_province(glm::ivec2(current.x, current.y / 2)) == prov_sec;
			if(top_is_s) {
				temp += add_next(current, { 0, 1 }, prev_direction, progress, true);
				temp += add_next(current, { -1, 0 }, prev_direction, progress, false);
				temp += add_next(current, { 0, -1}, prev_direction, progress, true);
			} else {
				temp += add_next(current, { +1, -1}, prev_direction, progress, true);
				temp += add_next(current, { +1, 0}, prev_direction, progress, false);
				temp += add_next(current, { +1, +1}, prev_direction, progress, true);
			}
		}
		if(progress) {
			prev_direction = temp - current;
			current = temp;
		}
	} while(progress);

	//terminal point
	if(current.y % 2 == 0) {
		bool left_is_s = dat.safe_get_province(glm::ivec2(current.x - 1, current.y / 2)) == prov_sec;
		if(left_is_s) {
			points.push_back(glm::vec2(float(current.x), 1.0f + float(current.y) / 2.0f));
		} else {
			points.push_back(glm::vec2(float(current.x), 0.0f + float(current.y) / 2.0f));
		}
	} else {
		bool top_is_s = dat.safe_get_province(glm::ivec2(current.x, current.y / 2)) == prov_sec;
		if(top_is_s) {
			points.push_back(glm::vec2(float(current.x), 0.5f + float(current.y) / 2.0f));
		} else {
			points.push_back(glm::vec2(1.0f + float(current.x), 0.5f + float(current.y) / 2.0f));
		}
	}

	current.x = start_x;
	current.y = start_y;
	prev_direction.x = 0;
	prev_direction.y = 0;

	std::reverse(points.begin(), points.end());
	//counter clockwise
	progress = false;
	do {
		progress = false;
		glm::ivec2 temp{ 0, 0 };

		if(current.y % 2 == 0) {
			bool left_is_s = dat.safe_get_province(glm::ivec2(current.x - 1, current.y / 2)) == prov_sec;
			if(!left_is_s) {
				temp += add_next(current, { 0, 1 }, prev_direction, progress, true);
				temp += add_next(current, { 0, 2 }, prev_direction, progress, false);
				temp += add_next(current, { -1, 1 }, prev_direction, progress, true);
			} else {
				temp += add_next(current, { -1, -1 }, prev_direction, progress, true);
				temp += add_next(current, { 0, -2 }, prev_direction, progress, false);
				temp += add_next(current, { 0, -1 }, prev_direction, progress, true);
			}
		} else {
			bool top_is_s = dat.safe_get_province(glm::ivec2(current.x, current.y / 2)) == prov_sec;
			if(!top_is_s) {
				temp += add_next(current, { 0, 1 }, prev_direction, progress, true);
				temp += add_next(current, { -1, 0 }, prev_direction, progress, false);
				temp += add_next(current, { 0, -1 }, prev_direction, progress, true);
			} else {
				temp += add_next(current, { +1, -1 }, prev_direction, progress, true);
				temp += add_next(current, { +1, 0 }, prev_direction, progress, false);
				temp += add_next(current, { +1, +1 }, prev_direction, progress, true);
			}
		}
		if(progress) {
			prev_direction = temp - current;
			current = temp;
		}
	} while(progress);

	//terminal point
	if(current.y % 2 == 0) {
		bool left_is_s = dat.safe_get_province(glm::ivec2(current.x - 1, current.y / 2)) == prov_sec;
		if(!left_is_s) {
			points.push_back(glm::vec2(float(current.x), 1.0f + float(current.y) / 2.0f));
		} else {
			points.push_back(glm::vec2(float(current.x), 0.0f + float(current.y) / 2.0f));
		}
	} else {
		bool top_is_s = dat.safe_get_province(glm::ivec2(current.x, current.y / 2)) == prov_sec;
		if(!top_is_s) {
			points.push_back(glm::vec2(float(current.x), 0.5f + float(current.y) / 2.0f));
		} else {
			points.push_back(glm::vec2(1.0f + float(current.x), 0.5f + float(current.y) / 2.0f));
		}
	}

	return points;
}

void add_border_segment_vertices(display_data& dat, std::vector<glm::vec2> const& points, uint16_t province_A, uint16_t province_B) {
	if(points.size() < 3)
		return;

	bool wrap_around = false;
	if(glm::length(points.back() - points[0]) < 0.1f) {
		wrap_around = true;
	}

	{
		auto first = dat.border_vertices.size();

		glm::vec2 current_pos = glm::vec2(points.back().x, points.back().y);
		glm::vec2 next_pos = put_in_local(glm::vec2(points[points.size() - 2].x, points[points.size() - 2].y), current_pos, float(dat.size_x));

		float distance = 0.0f;
		glm::vec2 old_pos;
		glm::vec2 raw_dist;
		auto norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

		{
			if(wrap_around) {
				old_pos = put_in_local(glm::vec2(points[1].x, points[1].y), current_pos, float(dat.size_x));
			} else {
				old_pos = 2.0f * current_pos - next_pos;
			}

			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y),
				province_A,
				0.0f, distance
			});
			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y),
				province_A,
				1.0f, distance
			});

			raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
			raw_dist.x *= 2.0f;
			distance += 0.5f * glm::length(raw_dist);
		}

		for(auto i = points.size() - 1; i-- > 1; ) {
			old_pos = current_pos;
			current_pos = glm::vec2(points[i].x, points[i].y);
			old_pos = put_in_local(old_pos, current_pos, float(dat.size_x));
			next_pos = put_in_local(glm::vec2(points[i - 1].x, points[i - 1].y), current_pos, float(dat.size_x));
			auto next_direction = glm::normalize(next_pos - current_pos);

			norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y),
				province_A,
				0.0f, distance
			});
			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y),
				province_A,
				1.0f, distance
			});

			raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
			raw_dist.x *= 2.0f;
			distance += 0.5f * glm::length(raw_dist);
		}

		{ // case i == 0
			old_pos = current_pos;
			current_pos = glm::vec2(points[0].x, points[0].y);

			if(wrap_around) {
				next_pos = put_in_local(glm::vec2(points[points.size() - 2].x, points[points.size() - 2].y), current_pos, float(dat.size_x));
			} else {
				next_pos = 2.0f * current_pos - old_pos;
			}

			auto next_direction = glm::normalize(next_pos - current_pos);

			norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y), province_A, 0.0f, distance
			});
			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y), province_A, 1.0f, distance
			});

			raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
			raw_dist.x *= 2.0f;
			distance += 0.5f * glm::length(raw_dist);
		}
	}

	{
		auto first = dat.border_vertices.size();

		glm::vec2 current_pos = glm::vec2(points.back().x, points.back().y);
		glm::vec2 next_pos = put_in_local(glm::vec2(points[points.size() - 2].x, points[points.size() - 2].y), current_pos, float(dat.size_x));

		float distance = 0.0f;
		glm::vec2 old_pos;
		glm::vec2 raw_dist;
		auto norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

		{
			if(wrap_around) {
				old_pos = put_in_local(glm::vec2(points[1].x, points[1].y), current_pos, float(dat.size_x));
			} else {
				old_pos = 2.0f * current_pos - next_pos;
			}

			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y),
				province_B,
				1.0f, distance
			});
			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y),
				province_B,
				0.0f, distance
			});

			raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
			raw_dist.x *= 2.0f;
			distance += 0.5f * glm::length(raw_dist);
		}

		for(auto i = points.size() - 1; i-- > 1; ) {
			old_pos = current_pos;
			current_pos = glm::vec2(points[i].x, points[i].y);
			old_pos = put_in_local(old_pos, current_pos, float(dat.size_x));
			next_pos = put_in_local(glm::vec2(points[i - 1].x, points[i - 1].y), current_pos, float(dat.size_x));
			auto next_direction = glm::normalize(next_pos - current_pos);

			norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y),
				province_B,
				1.0f, distance
			});
			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y),
				province_B,
				0.0f, distance
			});

			raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
			raw_dist.x *= 2.0f;
			distance += 0.5f * glm::length(raw_dist);
		}

		{ // case i == 0
			old_pos = current_pos;
			current_pos = glm::vec2(points[0].x, points[0].y);

			if(wrap_around) {
				next_pos = put_in_local(glm::vec2(points[points.size() - 2].x, points[points.size() - 2].y), current_pos, float(dat.size_x));
			} else {
				next_pos = 2.0f * current_pos - old_pos;
			}

			auto next_direction = glm::normalize(next_pos - current_pos);

			norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y),
				province_B, 1.0f, distance
			});
			dat.border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y),
				province_B, 0.0f, distance
			});

			raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
			raw_dist.x *= 2.0f;
			distance += 0.5f * glm::length(raw_dist);

		}
	}
}

void display_data::make_borders(sys::state& state, std::vector<bool>& visited) {

	borders.resize(state.world.province_adjacency_size());
	for(auto adj : state.world.in_province_adjacency) {
		borders[adj.id.index()].adj = adj;
	}

	for(int32_t j = 0; j < int32_t(size_y); ++j) {
		for(int32_t i = 0; i < int32_t(size_x); ++i) {
			// left verticals
			{
				bool was_visited = visited[i + (j * 2) * size_x];

				auto prim = province::from_map_id(safe_get_province(glm::ivec2(i, j)));
				auto sec = province::from_map_id(safe_get_province(glm::ivec2(i - 1, j)));

				bool fake_border = false;
				if(!prim || !sec) {
					fake_border = true;
				}

				if(!was_visited && prim != sec) {
					int32_t border_index;
					if(!fake_border) {
						auto adj = state.world.get_province_adjacency_by_province_pair(prim, sec);
						assert(adj);
						border_index = adj.index();
						if(borders[border_index].count != 0) {
							border_index = int32_t(borders.size());
							borders.emplace_back();
							borders.back().adj = adj;
						}
						
					} else {
						border_index = int32_t(borders.size());
						borders.emplace_back();
						borders.back().adj = { };
					}

					borders[border_index].start_index = int32_t(border_vertices.size());				
					auto res = make_border_section(*this, state, visited, province::to_map_id(prim), province::to_map_id(sec), i, j * 2);
					smooth_points(res);
					add_border_segment_vertices(*this, res, safe_get_province(glm::ivec2(i, j)), safe_get_province(glm::ivec2(i - 1, j)));
					borders[border_index].count = int32_t(border_vertices.size() - borders[border_index].start_index);
				}
			}

			// horizontals
			if(j < int32_t(size_y) - 1) {
				bool was_visited = visited[i + (j * 2 + 1) * size_x];
				auto prim = province::from_map_id(safe_get_province(glm::ivec2(i, j)));
				auto sec = province::from_map_id(safe_get_province(glm::ivec2(i, j + 1)));

				bool fake_border = false;
				if(!prim || !sec) {
					fake_border = true;
				}

				if(!was_visited && prim != sec && prim && sec) {
					int32_t border_index;

					if(!fake_border) {
						auto adj = state.world.get_province_adjacency_by_province_pair(prim, sec);
						assert(adj);
						border_index = adj.index();
						if(borders[border_index].count != 0) {
							border_index = int32_t(borders.size());
							borders.emplace_back();
							borders.back().adj = adj;
						}
					} else {
						border_index = int32_t(borders.size());
						borders.emplace_back();
						borders.back().adj = { };
					}

					borders[border_index].start_index = int32_t(border_vertices.size());
					auto res = make_border_section(*this, state, visited, province::to_map_id(prim), province::to_map_id(sec), i, j * 2 + 1);
					smooth_points(res);
					add_border_segment_vertices(*this, res, safe_get_province(glm::ivec2(i, j)), safe_get_province(glm::ivec2(i, j + 1)));
					borders[border_index].count = int32_t(border_vertices.size() - borders[border_index].start_index);
				}
			}
		}
	}
}

std::vector<glm::vec2> make_coastal_loop(display_data& dat, sys::state& state, std::vector<bool>& visited, int32_t start_x, int32_t start_y) {
	std::vector<glm::vec2> points;

	int32_t dropped_points_counter = 0;
	constexpr int32_t dropped_points_max = 64;

	auto add_next = [&](int32_t i, int32_t j, bool& next_found, int32_t prev_i, int32_t prev_j, bool corner) {
		if(next_found)
			return glm::ivec2(0, 0);
		if(visited[i + j  * dat.size_x])
			return glm::ivec2(0, 0);
		if(j % 2 == 0) {
			if(coastal_point(state, dat.safe_get_province(glm::ivec2(i, j / 2)), dat.safe_get_province(glm::ivec2(i - 1, j / 2)))) {
				visited[i + j * dat.size_x] = true;
				
				// test for colinearity
				// this works, but it can result in the border textures being "slanted" because the normals are carried over between two corners
	
				if(points.size() > 2 && !corner) {
					auto l = points[points.size() - 1];
					auto n = points[points.size() - 2];
					if(dropped_points_counter < dropped_points_max &&
						std::sqrt((l.x - n.x) * (l.x - n.x) + (l.y - n.y) * (l.y - n.y)) + std::sqrt((l.x - float(i)) * (l.x - float(i)) + (l.y - 0.5f - float(j) / 2.0f) * (l.y - 0.5f - float(j) / 2.0f))
						== std::sqrt((n.x - float(i)) * (n.x - float(i)) + (n.y - 0.5f - float(j) / 2.0f) * (n.y - 0.5f - float(j) / 2.0f))) {
						++dropped_points_counter;
						points.pop_back();
					} else {
						dropped_points_counter = 0;
					}
				}

				if(corner) {
					float prev_x = float(prev_i) + 0.5f;
					float prev_y = 0.5f + float(prev_j) / 2.0f;

					float next_x = float(i);
					float next_y = 0.5f + float(j) / 2.0f;

					points.push_back(glm::vec2((prev_x + next_x) / 2.f, prev_y));
					points.push_back(glm::vec2(next_x, (prev_y + next_y) / 2.f));
				}
				
				points.push_back(glm::vec2(float(i), 0.5f + float(j) / 2.0f));
				next_found = true;
				return glm::ivec2(i, j);
			}
		} else {
			if(coastal_point(state, dat.safe_get_province(glm::ivec2(i, j / 2)), dat.safe_get_province(glm::ivec2(i, j / 2 + 1)))) {
				visited[i + j * dat.size_x] = true;

				// test for colinearity
				// this works, but it can result in the border textures being "slanted" because the normals are carried over between two corners
				
				if(points.size() > 2 && !corner) {
					auto l = points[points.size() - 1];
					auto n = points[points.size() - 2];
					if(dropped_points_counter < dropped_points_max &&
						std::sqrt((l.x - n.x) * (l.x - n.x) + (l.y - n.y) * (l.y - n.y)) + std::sqrt((l.x - 0.5f - float(i)) * (l.x - 0.5f - float(i)) + (l.y - 0.5f - float(j) / 2.0f) * (l.y - 0.5f - float(j) / 2.0f))
						== std::sqrt((n.x - 0.5f - float(i)) * (n.x - 0.5f - float(i)) + (n.y - 0.5f - float(j) / 2.0f) * (n.y - 0.5f - float(j) / 2.0f))) {
						++dropped_points_counter;
						points.pop_back();
					} else {
						dropped_points_counter = 0;
					}
				}

				if(corner) {
					float prev_x = float(prev_i);
					float prev_y = 0.5f + float(prev_j) / 2.0f;

					float next_x = float(i) + 0.5f;
					float next_y = 0.5f + float(j) / 2.0f;

					points.push_back(glm::vec2(prev_x, (prev_y + next_y) / 2.f));
					points.push_back(glm::vec2((prev_x + next_x) / 2.f, next_y));
				}
				
				points.push_back(glm::vec2(float(i) + 0.5f, 0.5f + float(j) / 2.0f));
				next_found = true;
				return glm::ivec2(i, j);
			}
		}

		return glm::ivec2(0, 0);
	};

	points.push_back(glm::vec2(float(start_x) + (start_y % 2 == 0 ? 0.0f : 0.5f), 0.5f + float(start_y) / 2.0f));
	visited[start_x + start_y * dat.size_x] = true;

	bool progress = false;
	do {
		progress = false;
		glm::ivec2 temp{ 0, 0 };

		if(start_y % 2 == 0) {
			bool left_is_sea = dat.safe_get_province(glm::ivec2(start_x - 1, start_y / 2)) == 0 || province::from_map_id(dat.safe_get_province(glm::ivec2(start_x - 1, start_y / 2))).index() >= state.province_definitions.first_sea_province.index();
			if(left_is_sea) {
				temp += add_next(start_x, start_y + 1, progress, start_x, start_y, true);
				temp += add_next(start_x, start_y + 2, progress, start_x, start_y, false);
				temp += add_next(start_x - 1, start_y + 1, progress, start_x, start_y, true);
			} else {
				temp += add_next(start_x - 1, start_y - 1, progress, start_x, start_y, true);
				temp += add_next(start_x, start_y - 2, progress, start_x, start_y, false);
				temp += add_next(start_x, start_y - 1, progress, start_x, start_y, true);
			}
		} else {
			bool top_is_sea = dat.safe_get_province(glm::ivec2(start_x, start_y / 2)) == 0 || province::from_map_id(dat.safe_get_province(glm::ivec2(start_x, start_y / 2))).index() >= state.province_definitions.first_sea_province.index();
			if(top_is_sea) {
				temp += add_next(start_x, start_y + 1, progress, start_x, start_y, true);
				temp += add_next(start_x - 1, start_y, progress, start_x, start_y, false);
				temp += add_next(start_x, start_y - 1, progress, start_x, start_y, true);
			} else {
				temp += add_next(start_x + 1, start_y - 1, progress, start_x, start_y, true);
				temp += add_next(start_x + 1, start_y, progress, start_x, start_y, false);
				temp += add_next(start_x + 1, start_y + 1, progress, start_x, start_y, true);
			}
		}
		if(progress) {
			start_x = temp.x;
			start_y = temp.y;
		}
	} while(progress);

	return points;
}

void add_coastal_loop_vertices(display_data& dat, std::vector<glm::vec2> const& points) {
	if(points.size() < 3)
		return;

	auto first = dat.coastal_vertices.size();
	dat.coastal_starts.push_back(GLint(first));

	glm::vec2 current_pos = glm::vec2(points.back().x, points.back().y);
	glm::vec2 next_pos = put_in_local(glm::vec2(points[points.size() - 2].x, points[points.size() - 2].y), current_pos, float(dat.size_x));
	glm::vec2 prev_direction = glm::normalize(next_pos - current_pos);
	float distance = 0.0f;

	auto norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);
	auto old_pos = glm::vec2{ 0,0 };

	dat.coastal_vertices.emplace_back(textured_line_vertex_b{ norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y), 0.0f, distance });
	dat.coastal_vertices.emplace_back(textured_line_vertex_b{ norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y), 1.0f, distance });

	auto raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
	raw_dist.x *= 2.0f;
	distance += glm::length(raw_dist);

	for(auto i = points.size() - 1; i-- > 1; ) {
		old_pos = current_pos;
		current_pos = glm::vec2(points[i].x, points[i].y);
		old_pos = put_in_local(old_pos, current_pos, float(dat.size_x));
		next_pos = put_in_local(glm::vec2(points[i - 1].x, points[i - 1].y), current_pos, float(dat.size_x));
		auto next_direction = glm::normalize(next_pos - current_pos);

		norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

		dat.coastal_vertices.emplace_back(textured_line_vertex_b{ norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y), 0.0f, distance });
		dat.coastal_vertices.emplace_back(textured_line_vertex_b{ norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y), 1.0f, distance });

		raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
		raw_dist.x *= 2.0f;
		distance += glm::length(raw_dist);
	}

	// case i == 0
	{
		old_pos = current_pos;
		current_pos = glm::vec2(points[0].x, points[0].y);
		old_pos = put_in_local(old_pos, current_pos, float(dat.size_x));
		next_pos = put_in_local(glm::vec2(points.back().x, points.back().y), current_pos, float(dat.size_x));
		auto next_direction = glm::normalize(next_pos - current_pos);

		norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

		dat.coastal_vertices.emplace_back(textured_line_vertex_b{ norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y), 0.0f, distance });
		dat.coastal_vertices.emplace_back(textured_line_vertex_b{ norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y), 1.0f, distance });

		raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
		raw_dist.x *= 2.0f;
		distance += glm::length(raw_dist);
	}

	// wrap-around
	{
		old_pos = current_pos;
		current_pos = glm::vec2(points.back().x, points.back().y);
		old_pos = put_in_local(old_pos, current_pos, float(dat.size_x));
		next_pos = put_in_local(glm::vec2(points[points.size() - 2].x, points[points.size() - 2].y), current_pos, float(dat.size_x));
		auto next_direction = glm::normalize(next_pos - current_pos);
		auto next_normal = glm::vec2(-next_direction.y, next_direction.x);

		norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

		dat.coastal_vertices.emplace_back(textured_line_vertex_b{ norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y), 0.0f, distance });
		dat.coastal_vertices.emplace_back(textured_line_vertex_b{ norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y), 1.0f, distance });

		dat.coastal_vertices[first].previous_point = old_pos / glm::vec2(dat.size_x, dat.size_y);
		dat.coastal_vertices[first + 1].next_point = old_pos / glm::vec2(dat.size_x, dat.size_y);
	}

	dat.coastal_counts.push_back(GLsizei(dat.coastal_vertices.size() - dat.coastal_starts.back()));
}

void display_data::make_coastal_borders(sys::state& state, std::vector<bool>& visited) {
	for(int32_t j = 0; j < int32_t(size_y); ++j) {
		for(int32_t i = 0; i < int32_t(size_x); ++i) {
			// left verticals
			{
				bool was_visited = visited[i + (j * 2) * size_x];
				if(!was_visited && coastal_point(state, safe_get_province(glm::ivec2(i, j)), safe_get_province(glm::ivec2(i - 1, j)))) {
					auto res = make_coastal_loop(*this, state, visited, i, j * 2);
					smooth_points(res);
					add_coastal_loop_vertices(*this, res);
				}
			}

			// horizontals
			if(j < int32_t(size_y) - 1) {
				bool was_visited = visited[i + (j * 2 + 1) * size_x];
				if(!was_visited && coastal_point(state, safe_get_province(glm::ivec2(i, j)), safe_get_province(glm::ivec2(i, j + 1)))) {
					auto res = make_coastal_loop(*this, state, visited, i, j * 2 + 1);
					smooth_points(res);
					add_coastal_loop_vertices(*this, res);
				}
			}
		}
	}
}


// Set the river crossing bit for the province adjacencies
// Will march a line between each adjacent province centroid. If it hits a river it will set the bit
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
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	std::vector<river_vertex*> parents = { };
	std::vector<river_vertex*> children = { };
	bool visited = false;
	bool skip_visited = false;

	bool operator==(const river_vertex& other) {
		return x == other.x && y == other.y;
	}
};


struct river_runner {
	river_vertex* position;
	uint32_t x;
	uint32_t y;
	bool waiting_for_merge;
	bool done;
};

bool check_for_child(river_vertex* vertex, uint32_t x, uint32_t y) {
	for(uint8_t index = 0; index < vertex->children.size(); index++) {
		auto child = vertex->children[index];
		if((child->x - float(x) - 0.5f) > 0.1f) {
			continue;
		}
		if((child->y - float(y) - 0.5f) > 0.1f) {
			continue;
		}
		return true;
	}
	return false;
}

bool check_for_parent(river_vertex* vertex, uint32_t x, uint32_t y) {
	for(uint8_t index = 0; index < vertex->parents.size(); index++) {
		auto parent = vertex->parents[index];
		if(abs(parent->x - float(x) - 0.5f) > 0.1f) {
			continue;
		}
		if(abs(parent->y - float(y) - 0.5f) > 0.1f) {
			continue;
		}
		return true;
	}
	return false;
}

bool check_for_river(std::vector<uint8_t> const& river_data, river_runner& runner, int32_t x, int32_t y, glm::ivec2 size) {
	if(x < 0) {
		return false;
	}
	if(x >= size.x) {
		return false;
	}
	if(y < 0) {
		return false;
	}
	if(y >= size.y) {
		return false;
	}

	if(is_river(river_data[x + y * size.x])) {
		if(!check_for_parent(runner.position, x, y)) {
			return true;
		}
	}
	return false;
}

std::vector<glm::ivec2> check_for_potential_child(std::vector<uint8_t> const& river_data, river_runner& runner, glm::ivec2 size) {
	std::vector<glm::ivec2> results;
	if(check_for_river(river_data, runner, runner.x + 1, runner.y, size)) {
		results.push_back({ runner.x + 1, runner.y });
	}
	if(check_for_river(river_data, runner, runner.x - 1, runner.y, size)) {
		results.push_back({ runner.x - 1, runner.y });
	}
	if(check_for_river(river_data, runner, runner.x, runner.y + 1, size)) {
		results.push_back({ runner.x, runner.y + 1 });
	}
	if(check_for_river(river_data, runner, runner.x, runner.y - 1, size)) {
		results.push_back({ runner.x, runner.y - 1});
	}

	return results;
}

//static std::vector<std::vector<river_vertex>> pool_of_constructed_rivers;
/*
std::vector<river_vertex> make_directional_river(int32_t x, int32_t y, std::vector<std::vector<river_vertex>>& rivers, std::vector<uint8_t> const& river_data, std::vector<uint8_t> const& terrain_data, std::vector<bool>& marked, glm::ivec2 size, int32_t old_x = -1, int32_t old_y = -1, int32_t width = 1, int32_t old_width = 1) {
	//auto available_index = pool_of_constructed_rivers.size();
	//pool_of_constructed_rivers.push_back({ });
	std::vector<river_vertex> constructed = {};// pool_of_constructed_rivers[available_index];

	if(old_x != -1 && old_y != -1) {
		constructed.push_back(river_vertex{ float(old_x + 0.5f), float(old_y + 0.5f), (float)width, false });
	}

	marked[x + y * size.x] = true;
	auto process_non_merge = [&](int32_t xin, int32_t yin, bool& forward_found) {
		if(xin >= 0 && yin >= 0 && xin < size.x && yin < size.y && (xin != old_x || yin != old_y) && is_river(river_data[xin + yin * size.x]) && !marked[xin + yin * size.x] && !is_river_merge(river_data[xin + yin * size.x]) && !is_river_source(river_data[xin + yin * size.x])) {

			marked[xin + yin * size.x] = true;
			auto local_width = river_width(river_data[xin + yin * size.x]);
			if(!forward_found) {
				forward_found = true;
				return glm::ivec3{xin, yin, local_width };
			} else {
				constructed.back().keep = true;
				rivers.emplace_back(make_directional_river(xin, yin, rivers, river_data, terrain_data, marked, size, x, y, local_width));
			}
		}
		return glm::ivec3{ 0, 0, 0 };
	};
	auto process_merge = [&](int32_t xin, int32_t yin, bool& merge_found) {
		if(merge_found)
			return glm::ivec2{ 0, 0 };

		if(xin >= 0 && yin >= 0 && xin < size.x && yin < size.y && (xin != old_x || yin != old_y) && is_river(river_data[xin + yin * size.x]) && !marked[xin + yin * size.x] && is_river_merge(river_data[xin + yin * size.x]) && !is_river_source(river_data[xin + yin * size.x])) {

			marked[xin + yin * size.x] = true;
			merge_found = true;
			return glm::ivec2{ xin, yin };
		}
		return glm::ivec2{ 0, 0 };
	};
	auto process_post_merge = [&](int32_t xin, int32_t yin, bool& forward_found) {
		if(xin >= 0 && yin >= 0 && xin < size.x && yin < size.y && (xin != old_x || yin != old_y) && is_river(river_data[xin + yin * size.x]) && !is_river_merge(river_data[xin + yin * size.x]) && !is_river_source(river_data[xin + yin * size.x])) {
			if(!forward_found) {
				forward_found = true;
				return glm::ivec3{ xin, yin, river_width(river_data[xin + yin * size.x]) };
			}
		}
		return glm::ivec3{ 0, 0, 0 };
	};
	auto process_sea_extend = [&](int32_t xin, int32_t yin, bool& merge_found) {
		if(merge_found)
			return glm::ivec2{ 0, 0 };

		if(xin >= 0 && yin >= 0 && xin < size.x && yin < size.y && (xin != old_x || yin != old_y) && terrain_data[xin + yin * size.x] == 255) {
			merge_found = true;
			return glm::ivec2{ xin, yin };
		}
		return glm::ivec2{ 0, 0 };
	};

	bool forward_progress = false;
	do {
		forward_progress = false;
		constructed.push_back(river_vertex{ float(x + 0.5f), float(y + 0.5f), (float)width, false });

		auto res = process_non_merge(x - 1, y, forward_progress);
		res += process_non_merge(x + 1, y, forward_progress);
		res += process_non_merge(x, y - 1, forward_progress);
		res += process_non_merge(x, y + 1, forward_progress);

		if(forward_progress) {
			old_x = x;
			old_y = y;
			old_width = width;
			x = res.x;
			y = res.y;
			width = res.z;
		}

	} while(forward_progress);

	bool merge_found = false;
	auto resb = process_merge(x - 1, y, merge_found);
	resb += process_merge(x + 1, y, merge_found);
	resb += process_merge(x, y - 1, merge_found);
	resb += process_merge(x, y + 1, merge_found);

	if(merge_found) {
		constructed.push_back(river_vertex{ float(resb.x + 0.5f), float(resb.y + 0.5f), (float)width, false });

		old_x = x;
		old_y = y;
		x = resb.x;
		y = resb.y;

		bool post_progress = false;
		auto resc = process_post_merge(x - 1, y, post_progress);
		resc += process_post_merge(x + 1, y, post_progress);
		resc += process_post_merge(x, y - 1, post_progress);
		resc += process_post_merge(x, y + 1, post_progress);

		if(post_progress) {
			constructed.push_back(river_vertex{ float(resc.x + 0.5f), float(resc.y + 0.5f), (float)resc.z, false });
		}
	} else if(terrain_data[x + y * size.x] != 255) {
		bool post_progress = false;
		auto resc = process_sea_extend(x - 1, y, post_progress);
		resc += process_sea_extend(x + 1, y, post_progress);
		resc += process_sea_extend(x, y - 1, post_progress);
		resc += process_sea_extend(x, y + 1, post_progress);

		if(post_progress) {
			constructed.push_back(river_vertex{ float(resc.x + 0.5f), float(resc.y + 0.5f), (float)width, false });
		}
	}
	if(!constructed.empty()) {
		constructed.front().keep = true;
		constructed.back().keep = true;
	}
	return constructed;
}
*/


void display_data::create_curved_river_vertices(parsers::scenario_building_context & context, std::vector<uint8_t> const& river_data, std::vector<uint8_t> const& terrain_data) {

	std::vector<river_vertex> rivers;


	std::vector<bool> marked(size_x * size_y, false);

	std::vector<river_vertex*> sources;
	std::vector<river_vertex*> mouths;
	std::map<uint32_t, river_vertex*> vertex_by_pixel_index = {};
	std::vector<river_runner> runners;
	std::vector<river_runner> new_runners;


	auto size = glm::ivec2(int32_t(size_x), int32_t(size_y));



	for(uint32_t y = 0; y < size_y; y++) {
		for(uint32_t x = 0; x < size_x; x++) {
			auto index = x + y * size_x;
			if(is_river_source(river_data[index])) {
				river_vertex * source = new river_vertex { float(x) + 0.5f, float(y) + 0.5f, 40.f };
				sources.push_back(source);
				vertex_by_pixel_index[index] = source;
				river_runner r = { source, x, y, false, false };
				runners.push_back(r);
			}
		}
	}


	bool exists_running_runner = true;

	

	while(exists_running_runner) {
		exists_running_runner = false;

		for(auto & item : new_runners) {
			runners.push_back(item);
		}
		new_runners.clear();


		for(auto & runner : runners) {
			if(runner.done) {
				continue;
			}

			auto potential_children = check_for_potential_child(river_data, runner, size);

			if(potential_children.size() == 0) {
				// extend to the sea:

				for(int32_t i = -1; i <= 1; i++) {
					for(int32_t j = -1; j <= 1; j++) {
						auto x = (int32_t)runner.x + i;
						auto y = (int32_t)runner.y + j;
						if(x < 0) {
							continue;
						}
						if(y < 0) {
							continue;
						}
						if(x >= size.x) {
							continue;
						}
						if(y >= size.y) {
							continue;
						}
						auto index = uint32_t(x) + uint32_t(y) * size_x;

						bool is_sea = terrain_data[index] == 255;
						if(!is_sea) {
							continue;
						}

						if(!vertex_by_pixel_index.contains(index)) {
							river_vertex* new_river_vertex = new river_vertex{
								float(x) + 0.5f,
								float(y) + 0.5f,
								runner.position->width * 2.f
							};
							vertex_by_pixel_index[index] = new_river_vertex;
							new_river_vertex->parents.push_back(runner.position);
							runner.position->children.push_back(new_river_vertex);

							i = 3;
							j = 3;
							break;
						}
					}
				}

				runner.done = true;
				mouths.push_back(runner.position);
			} else if(runner.waiting_for_merge) {
				for(auto target_coord : potential_children) {
					auto target_index = target_coord.x + target_coord.y * size_x;
					if(vertex_by_pixel_index.contains(target_index)) {
						auto target_vertex = vertex_by_pixel_index[target_index];
						target_vertex->parents.push_back(runner.position);
						runner.position->children.push_back(target_vertex);
						runner.waiting_for_merge = false;
						runner.done = true;
					}
				}
			} else {
				exists_running_runner = true;
				if(is_river_merge(river_data[runner.x + runner.y * size_x])) {
					runner.waiting_for_merge = true;
				} else {
					bool first_child = true;
					uint32_t next_x = 0;
					uint32_t next_y = 0;
					river_vertex* next_position = nullptr;

					for(auto candidate : potential_children) {
						auto x = (uint32_t)candidate.x;
						auto y = (uint32_t)candidate.y;
						auto index = x + y * size_x;
						auto width = float(river_width(river_data[index]));

						if(is_river_merge(river_data[index])) {
							if(potential_children.size() > 1) {
								continue;
							}
							width = runner.position->width;
						}

						river_vertex* new_river_vertex = nullptr;

						bool we_will_move_forward = false;

						if(vertex_by_pixel_index.contains(index)) {
							new_river_vertex = vertex_by_pixel_index[index];
						} else {
							new_river_vertex = new river_vertex{
								float(x) + 0.5f,
								float(y) + 0.5f,
								width
							};
							vertex_by_pixel_index[index] = new_river_vertex;
							we_will_move_forward = true;
						}							

						new_river_vertex->parents.push_back(runner.position);
						runner.position->children.push_back(new_river_vertex);

						if(we_will_move_forward) {
							if(first_child) {
								next_x = x;
								next_y = y;
								next_position = new_river_vertex;
								first_child = false;
							} else {
								river_runner r = { new_river_vertex, x, y, false, false };
								new_runners.push_back(r);
							}
						} else {
							runner.done = true;
						}
					}

					if(next_position != nullptr) {
						runner.x = next_x;
						runner.y = next_y;
						runner.position = next_position;
					}
				}
			}
		}
	}



	/*
	for(uint32_t y = 0; y < size_y; y++) {
		for(uint32_t x = 0; x < size_x; x++) {
			if(is_river_source(river_data[x + y * size_x]))
				rivers.emplace_back(make_directional_river(x, y, rivers, river_data, terrain_data, marked, glm::ivec2(int32_t(size_x), int32_t(size_y))));
		}
	}
	

	// remove empty rivers or rivers with 1 vertex
	for(auto i = rivers.size(); i-- > 0; ) {
		if(rivers[i].size() <= 1) {
			rivers[i] = std::move(rivers.back());
			rivers.pop_back();
		}
	}

	// mark merge points as keep
	// boy, this sure is wildly inefficient ...

	for(const auto& river : rivers) {
		auto back_point = river.back();
		for(auto& other_river : rivers) {
			bool found_merge = false;
			for(auto& pt : other_river) {
				if(pt.x == back_point.x && pt.y == back_point.y) {
					pt.keep = true;
					found_merge = true;
					break;
				}
			}
			if(found_merge)
				break;
		}
	}

	

	for(const auto& river : rivers) {
		river_starts.push_back(GLint(river_vertices.size()));

		glm::vec2 current_pos = glm::vec2(river.back().x, river.back().y);
		glm::vec2 next_pos = put_in_local(glm::vec2(river[river.size() - 2].x, river[river.size() - 2].y), current_pos, float(size_x));

		float current_width = river.back().width;
		float next_width = river[river.size() - 2].width;

		glm::vec2 tangent_prev = glm::normalize(next_pos - current_pos);
		float distance = 0.0f;

		auto start_normal = glm::vec2(-tangent_prev.y, tangent_prev.x);
		auto norm_pos = current_pos / glm::vec2(size_x, size_y);
		river_vertices.emplace_back(textured_line_with_width_vertex{ norm_pos, +start_normal, 0.0f, distance, river.back().width });//C
		river_vertices.emplace_back(textured_line_with_width_vertex{ norm_pos, -start_normal, 1.0f, distance, river.back().width });//D

		for(auto i = river.size() - 1; i-- > 0;) {
			//if(!river[i].keep && i % 3 != 0) {
			//	continue; // skip
			//}
			glm::vec2 tangent_next{ 0.0f, 0.0f };
			next_pos = put_in_local(glm::vec2(river[i].x, river[i].y), current_pos, float(size_x));

			current_width = river[i].width;
			if(i > 0) {
				auto nexti = i - 1;
				//while(!river[nexti].keep && nexti % 3 != 0) {
				//	nexti--;
				//}

				glm::vec2 next_next_pos = put_in_local(glm::vec2(river[nexti].x, river[nexti].y), next_pos, float(size_x));


				glm::vec2 tangent_current_to_next = glm::normalize(next_pos - current_pos);
				glm::vec2 tangent_next_to_next_next = glm::normalize(next_next_pos - next_pos);
				tangent_next = glm::normalize(tangent_current_to_next + tangent_next_to_next_next);
				next_width = river[nexti].width;
			} else {
				tangent_next = glm::normalize(next_pos - current_pos);
				next_width = current_width;
			}
			float save_distance = distance;
			add_tl_bezier_to_buffer(river_vertices, current_pos, next_pos, tangent_prev, tangent_next, 0.0f, false, float(size_x), float(size_y), 4, distance, current_width, next_width);
			assert(abs((distance - save_distance) * size_x) > 0.5);
			assert(abs((distance - save_distance) * size_x) < 4.0);
			tangent_prev = tangent_next;
			current_pos = glm::vec2(river[i].x, river[i].y);
		}
		river_counts.push_back(GLsizei(river_vertices.size() - river_starts.back()));
	}

	*/

	std::vector<river_vertex*> current_path = { };
	std::vector<river_vertex*> nodes_to_skip = { };

	current_path.clear();

	uint32_t skip_nodes_counter = 0;


	// to avoid "squareness" we walk around the graph and "forget" some of nodes:
	// optimally, we want to forget a few nodes before nodes with multiple parents
	// to create smooth "confluence"
	for(auto source : sources) {
		//skip one pixel rivers
		if(source->children.size() == 0) {
			continue;
		}

		current_path.push_back(source);
		

		// walk around to find a confluence
		while(current_path.size() > 0) {
			auto current_node = current_path.back();
			current_node->skip_visited = true;

			// check if one of our child is a confluence:
			bool has_confluence_further = false;

			for(auto potential_confluence : current_node->children) {
				if(
					potential_confluence->parents.size() > 1
					&& potential_confluence->children.size() > 0
				) {
					has_confluence_further = true;
				}
			}

			if(has_confluence_further) {
				// we found a confluence!
				// backtrack and mark a few nodes for removal
				auto backrunner = current_path[current_path.size() - 1];
				for(int steps = 0; steps < 2; steps++) {
					if(current_path.size() - 1 - steps <= 0) {
						break;
					}
					if(backrunner->parents.size() == 0) {
						break;
					}					
					nodes_to_skip.push_back(backrunner);
					backrunner = current_path[current_path.size() - 1 - steps];
				}
			}

			if(current_path.size() > 2) {
				auto prev_node = current_path[current_path.size() - 2];

				if(current_node->parents.size() > 1 && current_node->children.size() > 0) {
					// we are the confluence!
					// skip three nodes
					skip_nodes_counter = 2;
					//nodes_to_skip.push_back(current_node);
				} else {
					// we are not at special node
					bool node_was_skipped = false;
					if(current_node->children.size() == 1)
						if(current_node->parents.size() == 1)
							if(current_node->children[0]->parents.size() == 1)
								if(current_node->parents[0]->children.size() == 1) {
									assert(current_node->children[0]->parents[0] == current_node);
									assert(current_node->parents[0]->children[0] == current_node);
									if(current_path.size() % 3 == 0) {
										nodes_to_skip.push_back(current_node);
										node_was_skipped = true;
										if(skip_nodes_counter > 0) {
											skip_nodes_counter -= 1;
										}
									};
								}
					if(current_node->children.size() > 0)
						if(current_node->parents.size() > 0)
							if(!node_was_skipped && skip_nodes_counter > 0)  {
								//nodes_to_skip.push_back(current_node);
								skip_nodes_counter -= 1;
							}
				}
			}
			bool pop_back = true;
			for(auto candidate : current_node->children) {
				if(!candidate->skip_visited) {
					current_path.push_back(candidate);
					pop_back = false;
				}
			}

			if(pop_back) {
				current_path.pop_back();
			}
		}
	}

	
	std::vector<river_vertex*> parents;
	std::vector<river_vertex*> children;
	std::vector<river_vertex*> temp;
	for(auto item : nodes_to_skip) {
		parents.clear();
		children.clear();

		// collect all children:
		for(auto child : item->children) {
			children.push_back(child);
		}
		// collect all parents:
		for(auto parent : item->parents) {
			parents.push_back(parent);
		}

		// attach children to parents:
		for(auto parent : parents) {
			temp.clear();

			// save old children to temp
			for(auto parent_child : parent->children) {
				if(parent_child->x == item->x)
					if(parent_child->y == item->y)
						continue;
				temp.push_back(parent_child);
			}

			for(auto child_to_attach : children) {
				// check that they don't have this child already
				bool skip_child = false;
				for(auto parent_child : parent->children) {
					if(parent_child->x == child_to_attach->x)
						if(parent_child->y == child_to_attach->y) {
							skip_child = true;
							break;
						}
				}

				if(skip_child) {
					continue;
				} else {
					temp.push_back(child_to_attach);
				}
			}

			parent->children.clear();

			for(auto saved_item : temp) {
				parent->children.push_back(saved_item);
			}
		}

		// attach parents to children
		for(auto child : children) {
			temp.clear();

			// save old parents to temp
			for(auto child_parent : child->parents) {
				if(child_parent->x == item->x)
					if(child_parent->y == item->y)
						continue;
				temp.push_back(child_parent);
			}

			for(auto parent_to_attach : parents) {
				// check that they don't have this child already
				bool skip_parent = false;
				for(auto child_parent : child->parents) {
					if(child_parent->x == parent_to_attach->x)
						if(child_parent->y == parent_to_attach->y) {
							skip_parent = true;
							break;
						}
				}

				if(skip_parent) {
					continue;
				} else {
					temp.push_back(parent_to_attach);
				}
			}

			child->parents.clear();

			for(auto saved_item : temp) {
				//assert(saved_item->children[0] == child);
				child->parents.push_back(saved_item);
			}
		}

		item->children.clear();
		item->parents.clear();
	}
	

	// now we generate quads for rivers:
	// basic idea is to proceed from sources to children
	// every segment is a bezier curve with control points being the tangent vectors of rivers
	// in result, segments should share tangent lines at the merge/split points
	// we assume that "main" child is always at index 0

	current_path.clear();
	std::vector<glm::vec2> vertex_stabilized_history;
	vertex_stabilized_history.clear();

	for(auto source : sources) {
		//skip one pixel rivers
		if(source->children.size() == 0) {
			continue;
		}

		current_path.push_back(source);
		vertex_stabilized_history.push_back({ source->x, source->y });

		// we explore current river and create paths along it
		// until we explore all downstream nodes
		while(current_path.size() > 0) {
			// we start by tracking back along the river until we find
			// a node with not visited child
			bool can_start_here = false;


			river_vertex* runner = current_path.back();
			if(!runner->visited) {
				can_start_here = true;
			}

			while(!can_start_here) {
				if(current_path.size() == 0) {
					break;
				}

				runner = current_path.back();
				current_path.pop_back();
				vertex_stabilized_history.pop_back();

				for(auto child : runner->children) {
					if(child->visited) {
						continue;
					}

					can_start_here = true;
				}
			}

			if(!can_start_here) {
				break;
			}

			// we start running along the river
			// to create a new path and save it in a buffer			

			// save current offset
			river_starts.push_back(GLint(river_vertices.size()));

			//init distance from the source
			float distance = 0.0f;
			// it will store distance local to current source
			// which will lead to incontinuities in distances in river basins
			// these problems should be probably solved in a separate pass ?

			glm::vec2 vertex = { runner->x, runner->y };
			glm::vec2 vertex_stabilized = vertex;
			if(vertex_stabilized_history.size() > 0) {
				vertex_stabilized = vertex_stabilized_history.back();
			}
			glm::vec2 vertex_stabilized_speed = { 0.f, 0.f };
			float friction = 0.932f;
			uint8_t steps = 8;
			float weight = 0.005f;

			uint8_t count_back = 16;

			bool can_continue = true;
			while(can_continue) {
				runner->visited = true;

				can_continue = false;
				bool stop = true;

				river_vertex* next_node = nullptr;

				for(auto candidate : runner->children) {
					if(candidate->visited) {
						can_continue = true;
						next_node = candidate;
						continue;
					} else {
						next_node = candidate;
						can_continue = true;
						stop = false;
						break;
					}
				}

				if(!can_continue) {
					// no children at all: give up
					break;
				}
				// otherwise we have an already visited children
				// which we could use to finish the curve

				// we care about mouths:
				if(next_node->children.size() == 0) {
					can_continue = false;
					next_node->visited = true;

					auto current_point = glm::vec2(runner->x, runner->y);
					auto next_point = glm::vec2(next_node->x, next_node->y);

					auto current_tangent = glm::normalize(next_point - current_point);
					next_point = next_point + current_tangent * 5.f;
					auto current_normal = glm::vec2{ -current_tangent.y, current_tangent.x };
					auto next_tangent = glm::normalize(current_tangent);

					for(uint8_t step = 0; step <= steps; step++) {
						auto t = (float(step) / float(steps));
						vertex =
							t * next_point
							+ (1.f - t) * current_point;
						auto width = t * next_node->width + (1.f - t) * runner->width;

						auto current_weight = weight * width;

						auto acceleration = (vertex - vertex_stabilized) / current_weight;
						vertex_stabilized_speed += acceleration;
						vertex_stabilized_speed *= (1.f - friction * 0.9f);

						auto vertex_old_opengl_coords = vertex_stabilized / glm::vec2(size_x, size_y);
						vertex_stabilized = vertex_stabilized + vertex_stabilized_speed;
						auto vertex_opengl_coords = vertex_stabilized / glm::vec2(size_x, size_y);
						auto speed_opengl_coords = vertex_opengl_coords - vertex_old_opengl_coords;
						auto normal_opengl_coords = glm::normalize(glm::vec2{ -speed_opengl_coords.y, speed_opengl_coords.x });
						distance += glm::length(speed_opengl_coords);

						river_vertices.emplace_back(textured_line_with_width_vertex{ vertex_opengl_coords, +normal_opengl_coords, 0.0f, distance, width });//C
						river_vertices.emplace_back(textured_line_with_width_vertex{ vertex_opengl_coords, -normal_opengl_coords, 1.0f, distance, width });//D
					}

					/*
					add_tl_bezier_to_buffer(
						river_vertices,
						current_point,
						next_point,
						current_tangent,
						next_tangent,
						0.0f,
						false,
						float(size_x),
						float(size_y),
						8,
						distance,
						runner->width,
						next_node->width
					);
					*/

					break;
				}

				// backtrack for a while to connect to previous parent?

				river_vertex* next_next_node = next_node->children[0];

				auto current_point = glm::vec2(runner->x, runner->y);
				auto next_point = glm::vec2(next_node->x, next_node->y);
				auto next_next_point = glm::vec2(next_next_node->x, next_next_node->y);

				auto current_tangent = next_point - current_point;
				auto next_tangent = next_next_point - next_point;

				for(uint8_t step = 0; step <= steps; step++) {
					auto t = (float(step) / float(steps));
					vertex =
						t * next_point
						+ (1.f - t) * current_point;
					auto width = t * next_node->width + (1.f - t) * runner->width;

					auto current_weight = weight * width;

					auto acceleration = (vertex - vertex_stabilized) / current_weight;
					vertex_stabilized_speed += acceleration;
					vertex_stabilized_speed *= (1.f - friction);

					auto vertex_old_opengl_coords = vertex_stabilized / glm::vec2(size_x, size_y);
					vertex_stabilized = vertex_stabilized + vertex_stabilized_speed;
					auto vertex_opengl_coords = vertex_stabilized / glm::vec2(size_x, size_y);
					auto speed_opengl_coords = vertex_opengl_coords - vertex_old_opengl_coords;
					auto normal_opengl_coords = glm::normalize(glm::vec2{ -speed_opengl_coords.y, speed_opengl_coords.x });
					distance += glm::length(speed_opengl_coords);

					river_vertices.emplace_back(textured_line_with_width_vertex{ vertex_opengl_coords, +normal_opengl_coords, 0.0f, distance, width });//C
					river_vertices.emplace_back(textured_line_with_width_vertex{ vertex_opengl_coords, -normal_opengl_coords, 1.0f, distance, width });//D
				}

				/*
				add_tl_bezier_to_buffer(
					river_vertices,
					current_point,
					next_point,
					current_tangent,
					next_tangent,
					0.0f,
					false,
					float(size_x),
					float(size_y),
					8,
					distance,
					runner->width,
					next_node->width
				);
				*/

				if(stop) {
					count_back -= 1;
				}

				if(count_back == 0) {
					// we already visited the next node
					// and used it only to have a smooth merge
					break;
				}

				runner = next_node;
				current_path.push_back(runner);
				vertex_stabilized_history.push_back( vertex_stabilized );
			}

			// save count of vertices in current batch
			river_counts.push_back(GLsizei(river_vertices.size() - river_starts.back()));
		}

		glm::vec2 current_pos = glm::vec2(source->x, source->y);
		glm::vec2 next_pos = glm::vec2(0, 0);



		
	}
}

}
