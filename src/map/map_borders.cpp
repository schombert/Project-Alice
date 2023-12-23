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
	return river_data < 16;
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

	auto add_next = [&](int32_t i, int32_t j, bool& next_found) {
		if(next_found)
			return glm::ivec2(0, 0);
		if(visited[i + j * dat.size_x])
			return glm::ivec2(0, 0);
		if(j % 2 == 0) {
			if(order_indifferent_compare(prov_prim, prov_sec, dat.safe_get_province(glm::ivec2(i, j / 2)), dat.safe_get_province(glm::ivec2(i - 1, j / 2)))) {
				visited[i + j * dat.size_x] = true;

				points.push_back(glm::vec2(float(i), 0.5f + float(j) / 2.0f));
				next_found = true;
				return glm::ivec2(i, j);
			}
		} else {
			if(order_indifferent_compare(prov_prim, prov_sec, dat.safe_get_province(glm::ivec2(i, j / 2)), dat.safe_get_province(glm::ivec2(i, j / 2 + 1)))) {
				visited[i + j * dat.size_x] = true;

				points.push_back(glm::vec2(float(i) + 0.5f, 0.5f + float(j) / 2.0f));
				next_found = true;
				return glm::ivec2(i, j);
			}
		}

		return glm::ivec2(0, 0);
	};


	points.push_back(glm::vec2(float(start_x) + (start_y % 2 == 0 ? 0.0f : 0.5f), 0.5f + float(start_y) / 2.0f));
	visited[start_x + start_y * dat.size_x] = true;

	int32_t cur_x = start_x;
	int32_t cur_y = start_y;

	bool progress = false;
	// clockwise
	do {
		progress = false;
		glm::ivec2 temp{ 0, 0 };

		if(cur_y % 2 == 0) {
			bool left_is_s = dat.safe_get_province(glm::ivec2(cur_x - 1, cur_y / 2)) == prov_sec;
			if(left_is_s) {
				temp += add_next(cur_x, cur_y + 1, progress);
				temp += add_next(cur_x, cur_y + 2, progress);
				temp += add_next(cur_x - 1, cur_y + 1, progress);
			} else {
				temp += add_next(cur_x - 1, cur_y - 1, progress);
				temp += add_next(cur_x, cur_y - 2, progress);
				temp += add_next(cur_x, cur_y - 1, progress);
			}
		} else {
			bool top_is_s = dat.safe_get_province(glm::ivec2(cur_x, cur_y / 2)) == prov_sec;
			if(top_is_s) {
				temp += add_next(cur_x, cur_y + 1, progress);
				temp += add_next(cur_x - 1, cur_y, progress);
				temp += add_next(cur_x, cur_y - 1, progress);
			} else {
				temp += add_next(cur_x + 1, cur_y - 1, progress);
				temp += add_next(cur_x + 1, cur_y, progress);
				temp += add_next(cur_x + 1, cur_y + 1, progress);
			}
		}
		if(progress) {
			cur_x = temp.x;
			cur_y = temp.y;
		}
	} while(progress);

	//terminal point
	if(cur_y % 2 == 0) {
		bool left_is_s = dat.safe_get_province(glm::ivec2(cur_x - 1, cur_y / 2)) == prov_sec;
		if(left_is_s) {
			points.push_back(glm::vec2(float(cur_x), 1.0f + float(cur_y) / 2.0f));
		} else {
			points.push_back(glm::vec2(float(cur_x), 0.0f + float(cur_y) / 2.0f));
		}
	} else {
		bool top_is_s = dat.safe_get_province(glm::ivec2(cur_x, cur_y / 2)) == prov_sec;
		if(top_is_s) {
			points.push_back(glm::vec2(float(cur_x), 0.5f + float(cur_y) / 2.0f));
		} else {
			points.push_back(glm::vec2(1.0f + float(cur_x), 0.5f + float(cur_y) / 2.0f));
		}
	}

	cur_x = start_x;
	cur_y = start_y;

	std::reverse(points.begin(), points.end());
	//counter clockwise
	progress = false;
	do {
		progress = false;
		glm::ivec2 temp{ 0, 0 };

		if(cur_y % 2 == 0) {
			bool left_is_s = dat.safe_get_province(glm::ivec2(cur_x - 1, cur_y / 2)) == prov_sec;
			if(!left_is_s) {
				temp += add_next(cur_x, cur_y + 1, progress);
				temp += add_next(cur_x, cur_y + 2, progress);
				temp += add_next(cur_x - 1, cur_y + 1, progress);
			} else {
				temp += add_next(cur_x - 1, cur_y - 1, progress);
				temp += add_next(cur_x, cur_y - 2, progress);
				temp += add_next(cur_x, cur_y - 1, progress);
			}
		} else {
			bool top_is_s = dat.safe_get_province(glm::ivec2(cur_x, cur_y / 2)) == prov_sec;
			if(!top_is_s) {
				temp += add_next(cur_x, cur_y + 1, progress);
				temp += add_next(cur_x - 1, cur_y, progress);
				temp += add_next(cur_x, cur_y - 1, progress);
			} else {
				temp += add_next(cur_x + 1, cur_y - 1, progress);
				temp += add_next(cur_x + 1, cur_y, progress);
				temp += add_next(cur_x + 1, cur_y + 1, progress);
			}
		}
		if(progress) {
			cur_x = temp.x;
			cur_y = temp.y;
		}
	} while(progress);

	//terminal point
	if(cur_y % 2 == 0) {
		bool left_is_s = dat.safe_get_province(glm::ivec2(cur_x - 1, cur_y / 2)) == prov_sec;
		if(!left_is_s) {
			points.push_back(glm::vec2(float(cur_x), 1.0f + float(cur_y) / 2.0f));
		} else {
			points.push_back(glm::vec2(float(cur_x), 0.0f + float(cur_y) / 2.0f));
		}
	} else {
		bool top_is_s = dat.safe_get_province(glm::ivec2(cur_x, cur_y / 2)) == prov_sec;
		if(!top_is_s) {
			points.push_back(glm::vec2(float(cur_x), 0.5f + float(cur_y) / 2.0f));
		} else {
			points.push_back(glm::vec2(1.0f + float(cur_x), 0.5f + float(cur_y) / 2.0f));
		}
	}

	return points;
}

void add_border_segment_vertices(display_data& dat, std::vector<glm::vec2> const& points) {
	if(points.size() < 3)
		return;

	auto first = dat.border_vertices.size();

	glm::vec2 current_pos = glm::vec2(points.back().x, points.back().y);
	glm::vec2 next_pos = put_in_local(glm::vec2(points[points.size() - 2].x, points[points.size() - 2].y), current_pos, float(dat.size_x));

	float distance = 0.0f;
	glm::vec2 old_pos;
	glm::vec2 raw_dist;
	auto norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

	{
		old_pos = 2.0f * current_pos - next_pos;

		dat.border_vertices.emplace_back(textured_line_vertex_b{ norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y), 0.0f, distance });
		dat.border_vertices.emplace_back(textured_line_vertex_b{ norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y), 1.0f, distance });

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

		dat.border_vertices.emplace_back(textured_line_vertex_b{ norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y), 0.0f, distance });
		dat.border_vertices.emplace_back(textured_line_vertex_b{ norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y), 1.0f, distance });

		raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
		raw_dist.x *= 2.0f;
		distance += 0.5f * glm::length(raw_dist);
	}

	// case i == 0
	{
		old_pos = current_pos;
		current_pos = glm::vec2(points[0].x, points[0].y);

		next_pos = 2.0f * current_pos - old_pos;

		auto next_direction = glm::normalize(next_pos - current_pos);

		norm_pos = current_pos / glm::vec2(dat.size_x, dat.size_y);

		dat.border_vertices.emplace_back(textured_line_vertex_b{ norm_pos, old_pos / glm::vec2(dat.size_x, dat.size_y), next_pos / glm::vec2(dat.size_x, dat.size_y), 0.0f, distance });
		dat.border_vertices.emplace_back(textured_line_vertex_b{ norm_pos, next_pos / glm::vec2(dat.size_x, dat.size_y), old_pos / glm::vec2(dat.size_x, dat.size_y), 1.0f, distance });

		raw_dist = (current_pos - next_pos) / glm::vec2(dat.size_x, dat.size_y);
		raw_dist.x *= 2.0f;
		distance += 0.5f * glm::length(raw_dist);

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

				if(!was_visited && prim != sec && prim && sec) {
					auto adj = state.world.get_province_adjacency_by_province_pair(prim, sec);
					assert(adj);
					int32_t border_index = adj.index();
					if(borders[border_index].count != 0) {
						border_index = int32_t(borders.size());
						borders.emplace_back();
						borders.back().adj = adj;
					}

					borders[border_index].start_index = int32_t(border_vertices.size());
				
					auto res = make_border_section(*this, state, visited, province::to_map_id(prim), province::to_map_id(sec), i, j * 2);
					add_border_segment_vertices(*this, res);

					borders[border_index].count = int32_t(border_vertices.size() - borders[border_index].start_index);
				}
			}

			// horizontals
			if(j < int32_t(size_y) - 1) {
				bool was_visited = visited[i + (j * 2 + 1) * size_x];
				auto prim = province::from_map_id(safe_get_province(glm::ivec2(i, j)));
				auto sec = province::from_map_id(safe_get_province(glm::ivec2(i, j + 1)));

				if(!was_visited && prim != sec && prim && sec) {
					auto adj = state.world.get_province_adjacency_by_province_pair(prim, sec);
					assert(adj);
					int32_t border_index = adj.index();
					if(borders[border_index].count != 0) {
						border_index = int32_t(borders.size());
						borders.emplace_back();
						borders.back().adj = adj;
					}

					borders[border_index].start_index = int32_t(border_vertices.size());

					auto res = make_border_section(*this, state, visited, province::to_map_id(prim), province::to_map_id(sec), i, j * 2 + 1);
					add_border_segment_vertices(*this, res);

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

	auto add_next = [&](int32_t i, int32_t j, bool& next_found) {
		if(next_found)
			return glm::ivec2(0, 0);
		if(visited[i + j  * dat.size_x])
			return glm::ivec2(0, 0);
		if(j % 2 == 0) {
			if(coastal_point(state, dat.safe_get_province(glm::ivec2(i, j / 2)), dat.safe_get_province(glm::ivec2(i - 1, j / 2)))) {
				visited[i + j * dat.size_x] = true;
				
				// test for colinearity
				// this works, but it can result in the border textures being "slanted" because the normals are carried over between two corners
	
				if(points.size() > 2) {
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
				
				points.push_back(glm::vec2(float(i), 0.5f + float(j) / 2.0f));
				next_found = true;
				return glm::ivec2(i, j);
			}
		} else {
			if(coastal_point(state, dat.safe_get_province(glm::ivec2(i, j / 2)), dat.safe_get_province(glm::ivec2(i, j / 2 + 1)))) {
				visited[i + j * dat.size_x] = true;

				// test for colinearity
				// this works, but it can result in the border textures being "slanted" because the normals are carried over between two corners
				
				if(points.size() > 2) {
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
				temp += add_next(start_x, start_y + 1, progress);
				temp += add_next(start_x, start_y + 2, progress);
				temp += add_next(start_x - 1, start_y + 1, progress);
			} else {
				temp += add_next(start_x - 1, start_y - 1, progress);
				temp += add_next(start_x, start_y - 2, progress);
				temp += add_next(start_x, start_y - 1, progress);
			}
		} else {
			bool top_is_sea = dat.safe_get_province(glm::ivec2(start_x, start_y / 2)) == 0 || province::from_map_id(dat.safe_get_province(glm::ivec2(start_x, start_y / 2))).index() >= state.province_definitions.first_sea_province.index();
			if(top_is_sea) {
				temp += add_next(start_x, start_y + 1, progress);
				temp += add_next(start_x - 1, start_y, progress);
				temp += add_next(start_x, start_y - 1, progress);
			} else {
				temp += add_next(start_x + 1, start_y - 1, progress);
				temp += add_next(start_x + 1, start_y, progress);
				temp += add_next(start_x + 1, start_y + 1, progress);
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
					add_coastal_loop_vertices(*this, res);
				}
			}

			// horizontals
			if(j < int32_t(size_y) - 1) {
				bool was_visited = visited[i + (j * 2 + 1) * size_x];
				if(!was_visited && coastal_point(state, safe_get_province(glm::ivec2(i, j)), safe_get_province(glm::ivec2(i, j + 1)))) {
					auto res = make_coastal_loop(*this, state, visited, i, j * 2 + 1);
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
	bool keep = false;
};

std::vector<river_vertex> make_directional_river(int32_t x, int32_t y, std::vector<std::vector<river_vertex>>& rivers, std::vector<uint8_t> const& river_data, std::vector<uint8_t> const& terrain_data, std::vector<bool>& marked, glm::ivec2 size, int32_t old_x = -1, int32_t old_y = -1) {

	std::vector<river_vertex> constructed;

	if(old_x != -1 && old_y != -1) {
		constructed.push_back(river_vertex{ float(old_x + 0.5f), float(old_y + 0.5f), false });
	}

	marked[x + y * size.x] = true;
	auto process_non_merge = [&](int32_t xin, int32_t yin, bool& forward_found) {
		if(xin >= 0 && yin >= 0 && xin < size.x && yin < size.y && (xin != old_x || yin != old_y) && is_river(river_data[xin + yin * size.x]) && !marked[xin + yin * size.x] && !is_river_merge(river_data[xin + yin * size.x]) && !is_river_source(river_data[xin + yin * size.x])) {

			marked[xin + yin * size.x] = true;
			if(!forward_found) {
				forward_found = true;
				return glm::ivec2{xin, yin};
			} else {
				constructed.back().keep = true;
				rivers.emplace_back(make_directional_river(xin, yin, rivers, river_data, terrain_data, marked, size, x, y));
			}
		}
		return glm::ivec2{ 0, 0 };
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
				return glm::ivec2{ xin, yin };
			}
		}
		return glm::ivec2{ 0, 0 };
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
		constructed.push_back(river_vertex{ float(x + 0.5f), float(y + 0.5f), false });

		auto res = process_non_merge(x - 1, y, forward_progress);
		res += process_non_merge(x + 1, y, forward_progress);
		res += process_non_merge(x, y - 1, forward_progress);
		res += process_non_merge(x, y + 1, forward_progress);

		if(forward_progress) {
			old_x = x;
			old_y = y;
			x = res.x;
			y = res.y;
		}

	} while(forward_progress);

	bool merge_found = false;
	auto resb = process_merge(x - 1, y, merge_found);
	resb += process_merge(x + 1, y, merge_found);
	resb += process_merge(x, y - 1, merge_found);
	resb += process_merge(x, y + 1, merge_found);

	if(merge_found) {
		constructed.push_back(river_vertex{ float(resb.x + 0.5f), float(resb.y + 0.5f), false });

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
			constructed.push_back(river_vertex{ float(resc.x + 0.5f), float(resc.y + 0.5f), false });
		}
	} else if(terrain_data[x + y * size.x] != 255) {
		bool post_progress = false;
		auto resc = process_sea_extend(x - 1, y, post_progress);
		resc += process_sea_extend(x + 1, y, post_progress);
		resc += process_sea_extend(x, y - 1, post_progress);
		resc += process_sea_extend(x, y + 1, post_progress);

		if(post_progress) {
			constructed.push_back(river_vertex{ float(resc.x + 0.5f), float(resc.y + 0.5f), false });
		}
	}
	if(!constructed.empty()) {
		constructed.front().keep = true;
		constructed.back().keep = true;
	}
	return constructed;
}

void display_data::create_curved_river_vertices(parsers::scenario_building_context & context, std::vector<uint8_t> const& river_data, std::vector<uint8_t> const& terrain_data) {

	std::vector<std::vector<river_vertex>> rivers;

	std::vector<bool> marked(size_x * size_y, false);

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
		glm::vec2 prev_perpendicular = glm::normalize(next_pos - current_pos);
		float distance = 0.0f;

		auto start_normal = glm::vec2(-prev_perpendicular.y, prev_perpendicular.x);
		auto norm_pos = current_pos / glm::vec2(size_x, size_y);
		river_vertices.emplace_back(textured_line_vertex{ norm_pos, +start_normal, 0.0f, distance });//C
		river_vertices.emplace_back(textured_line_vertex{ norm_pos, -start_normal, 1.0f, distance });//D



		for(auto i = river.size() - 1; i-- > 0;) {
			if(!river[i].keep && i % 3 != 0) {
				continue; // skip
			}

			glm::vec2 next_perpendicular{ 0.0f, 0.0f };
			next_pos = put_in_local(glm::vec2(river[i].x, river[i].y), current_pos, float(size_x));
			if(i > 0) {
				auto nexti = i - 1;
				while(!river[nexti].keep && nexti % 3 != 0) {
					nexti--;
				}
				glm::vec2 next_next_pos = put_in_local(glm::vec2(river[nexti].x, river[nexti].y), next_pos, float(size_x));
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
			add_tl_bezier_to_buffer(river_vertices, current_pos, next_pos, prev_perpendicular, next_perpendicular, 0.0f, false, float(size_x), float(size_y), 4, distance);
			prev_perpendicular = -1.0f * next_perpendicular;
			current_pos = glm::vec2(river[i].x, river[i].y);
		}

		river_counts.push_back(GLsizei(river_vertices.size() - river_starts.back()));
	}
}

}
