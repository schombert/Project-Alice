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
	if(glm::distance(vertices.back(), vertices.front()) < 0.001f && vertices.size() > 2) {
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
	province_border_vertices.clear();

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
				// We don't create adjacencies for diagnoal-only connections, to keep it consistent with Vic2 behavoir. Some mods rely on this (eg GFM)
				/*if(prov_id_ul != prov_id_dr && prov_id_dr != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));

					auto aval = context.state.world.get_province_adjacency_by_province_pair(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));
					if((context.state.world.province_adjacency_get_type(aval) & province::border::non_adjacent_bit) != 0)
						context.state.world.province_adjacency_get_type(aval) &= ~(province::border::non_adjacent_bit | province::border::impassible_bit);
					
				}*/
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
				// We don't create adjacencies for diagnoal-only connections, to keep it consistent with Vic2 behavoir. Some mods rely on this (eg GFM)
				/*if(prov_id_ul != prov_id_dr && prov_id_dr != 0 && prov_id_ul != 0) {
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));
				}*/
			}
		}
	}
}

bool is_river(uint8_t river_data) {
	return river_data < 250;
}
int32_t river_width(uint8_t river_data) {
	auto result = (255 - (int32_t)(river_data) + 5) / 2;
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

int32_t display_data::safe_get_province_handle_invalid_coords(glm::ivec2 pt) {
	while(pt.x < 0) {
		pt.x += int32_t(size_x);
	}
	while(pt.x >= int32_t(size_x)) {
		pt.x -= int32_t(size_x);
	}
	if(pt.y < 0) {
		return -1;
	}
	if(pt.y >= int32_t(size_y)) {
		return -1;
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

uint64_t constexpr encode_direction(int dx, int dy) {
	return uint64_t((dx + 2* dy) + 2);
}

void display_data::make_borders(sys::state& state, std::vector<uint8_t>& visited) {

	adj_index_to_border_edge.resize(state.world.province_adjacency_size());
	border_nodes.clear();
	border_edges.clear();
	province_to_edges.clear();
	province_border_starts.clear();
	province_border_counts.clear();
	province_border_vertices.clear();

	ankerl::unordered_dense::map<uint64_t, size_t> coord_to_border_node_index;
	ankerl::unordered_dense::map<uint64_t, uint8_t> use_subindex;

	auto border_node_Eq = [&](border_node& A, border_node& B) {
		return A.x == B.x && A.y == B.y;
	};

	auto get_local_node_index = [&](int x, int y, int dx, int dy) {
		uint64_t out_x = x + size_x;
		uint64_t out_y = y;
		if(dx == 1) { out_x = x + 1; out_y = y; }
		if(dx == -1) { out_x = x; out_y = y + 1; }
		if(dy == 1) { out_x = x + 1; out_y = y + 1; }
		if(dy == -1) { out_x = x; out_y = y; }
		while(out_x >= size_x) { out_x -= size_x; }
		uint64_t shift = 0;
		uint64_t base = out_x + out_y * size_x;
		if(use_subindex.contains(base)) {
			shift = encode_direction(dx, dy) * size_x * size_y;
		}
		assert(coord_to_border_node_index.contains(base + shift));
		return coord_to_border_node_index[base + shift];
	};
	auto get_local_node_index_2 = [&](int x, int y, int direction_encoding) {
		uint64_t shift = 0;
		uint64_t base = x + y * size_x;
		if(use_subindex.contains(base)) {
			shift = direction_encoding * size_x * size_y;
		}
		assert(coord_to_border_node_index.contains(base + shift));
		return coord_to_border_node_index[base + shift];
	};
	auto compare_local_node_index = [&](int x, int y, int dx, int dy, uint64_t value) {
		uint64_t out_x = x + size_x;
		uint64_t out_y = y;
		if(dx == 1) {
			out_x = x + 1;
			out_y = y;
		}
		if(dx == -1) {
			out_x = x;
			out_y = y + 1;
		}
		if(dy == 1) {
			out_x = x + 1;
			out_y = y + 1;
		}
		if(dy == -1) {
			out_x = x;
			out_y = y;
		}
		while(out_x >= size_x) {
			out_x -= size_x;
		}
		uint64_t shift = 0;
		uint64_t base = out_x + out_y * size_x;
		if(use_subindex.contains(base)) {
			shift = encode_direction(dx, dy) * size_x * size_y;
		}
		if (!coord_to_border_node_index.contains(base + shift)) {
			return false;
		}
		return coord_to_border_node_index[base + shift] == value;
	};
	auto validate_local_node_index = [&](int x, int y, int dx, int dy, uint64_t value) {
		uint64_t out_x = x + size_x;
		uint64_t out_y = y;
		if(dx == 1) {
			out_x = x + 1;
			out_y = y;
		}
		if(dx == -1) {
			out_x = x;
			out_y = y + 1;
		}
		if(dy == 1) {
			out_x = x + 1;
			out_y = y + 1;
		}
		if(dy == -1) {
			out_x = x;
			out_y = y;
		}
		while(out_x >= size_x) {
			out_x -= size_x;
		}
		uint64_t shift = 0;
		uint64_t base = out_x + out_y * size_x;
		if(use_subindex.contains(base)) {
			shift = encode_direction(dx, dy) * size_x * size_y;
		}
		auto coord = base + shift;
		if(!coord_to_border_node_index.contains(coord)) {
			assert(false);
		}
		auto actual_idx = coord_to_border_node_index[base + shift];
		assert (actual_idx == value);
	};

	auto register_node_index = [&](int x, int y, int dx, int dy, bool use_subindex_flag) {
		uint64_t shift = 0;
		uint64_t base = x + y * size_x;
		border_node result{ x, y };
		result.use_subindex = false;
		result.subindex = 0;
		if(use_subindex_flag) {
			use_subindex[base] = true;
			result.use_subindex = true;
			result.subindex = (uint8_t)encode_direction(dx, dy);
			shift = result.subindex * size_x * size_y;
		}
		auto idx = border_nodes.size();
		auto coord = base + shift;
		coord_to_border_node_index[coord] = idx;
		border_nodes.emplace_back(result);
		return idx;
	};

	auto contains_node = [&](int x, int y, int dx, int dy) {
		uint64_t out_x = x + size_x;
		uint64_t out_y = y;
		if(dx == 1) {
			out_x = x + 1;
			out_y = y;
		}
		if(dx == -1) {
			out_x = x;
			out_y = y + 1;
		}
		if(dy == 1) {
			out_x = x + 1;
			out_y = y + 1;
		}
		if(dy == -1) {
			out_x = x;
			out_y = y;
		}
		while(out_x >= size_x) {
			out_x -= size_x;
		}
		uint64_t shift = 0;
		uint64_t base = out_x + out_y * size_x;
		if(use_subindex.contains(base)) {
			shift = encode_direction(dx, dy) * size_x * size_y;
		}
		return coord_to_border_node_index.contains(base + shift);
	};

	/*
	Build the loops connecting nodes

	Up direction is toward higher y
	Right direction is toward higher x

	Orientation of a pixel's border is
	Top border : left
	Left border: down
	Bottom border: right
	Right border: up

	We move along the loop:
	x, y - current coordinate
	dx, dy - direction we are currently testing (right hand direction)

	If tested direction is available, we rotate toward this direction and move forward
	Otherwise we move forward until we hit the wall (in which case we rotate into minus tested direction

	*/

	auto validate_border_vertex = [&](textured_line_vertex_b_enriched_with_province_index& item) {
		if(item.position.x < 1.f && item.position.x > 0.f) {
			if(item.previous_point.x == 0.f && item.position.x > 0.5f) {
				item.previous_point.x = 1.f;
			}
			if(item.previous_point.x == 1.f && item.position.x < 0.5f) {
				item.previous_point.x = 0.f;
			}

			auto d_prev = glm::distance(item.position * glm::vec2{ size_x, size_y }, item.previous_point * glm::vec2{ size_x, size_y });
			auto d_next = glm::distance(item.position * glm::vec2{ size_x, size_y }, item.next_point * glm::vec2{ size_x, size_y });			

			if(d_prev >= 1.5f || d_next >= 1.5f) {
				assert(false);
				return false;
			}
		}
		return true;
	};

	int deg_shift = 2;

	auto fix_loop = [&](border_edge& item) {
		assert(item.count >= 4);

		item.true_loop = true;

		auto o = item.offset;

		{
			auto& start_vertex = province_border_vertices[o + 0];
			auto& end_vertex = province_border_vertices[o + item.count - 2];
			auto local_dist = glm::distance(start_vertex.position * glm::vec2(size_x, size_y), end_vertex.position * glm::vec2(size_x, size_y));
			assert(start_vertex.position == end_vertex.position);

			auto& start_vertex2 = province_border_vertices[o + 1];
			auto& end_vertex2 = province_border_vertices[o + item.count - 1];
			auto local_dist2 = glm::distance(start_vertex2.position * glm::vec2(size_x, size_y), end_vertex2.position * glm::vec2(size_x, size_y));
			assert(start_vertex2.position == end_vertex2.position);

			end_vertex.next_point = start_vertex2.next_point;
			end_vertex2.next_point = start_vertex2.next_point;

			start_vertex.previous_point = end_vertex.previous_point;
			start_vertex2.previous_point = end_vertex2.previous_point;
		}
	};

	auto validate_true_loop = [&](border_edge const& item) {
		auto o = item.offset;

		auto& start_vertex = province_border_vertices[o + 0];
		auto& end_vertex = province_border_vertices[o + item.count - 2];
		auto local_dist = glm::distance(start_vertex.position * glm::vec2(size_x, size_y), end_vertex.position * glm::vec2(size_x, size_y));
		assert(start_vertex.position == end_vertex.position);

		auto& start_vertex2 = province_border_vertices[o + 1];
		auto& end_vertex2 = province_border_vertices[o + item.count - 1];
		auto local_dist2 = glm::distance(start_vertex2.position * glm::vec2(size_x, size_y), end_vertex2.position * glm::vec2(size_x, size_y));
		assert(start_vertex2.position == end_vertex2.position);

		assert(end_vertex.next_point == start_vertex2.next_point);
		assert(end_vertex2.next_point == start_vertex2.next_point);

		assert(start_vertex.previous_point == end_vertex.previous_point);
		assert(start_vertex2.previous_point == end_vertex2.previous_point);
	};

	auto last_vertex_idx = -1;
	auto build_loop = [&](uint16_t province_map_index, int32_t x, int32_t y, int32_t dx, int32_t dy, bool true_loop) {
		if(y < 0) {
			return;
		}
		if(y > (int32_t)size_y) {
			return;
		}
		if(x >= (int32_t)size_x) {
			x -= size_x;
		}
		if(x < 0) {
			x += size_x;
		}
		if(visited[x + y * size_x + encode_direction(dx, dy) * size_x * size_y] == 1) {
			return;
		}
		//if(!province_map_index) {
			//return;
		//}
		province_border_starts.push_back((GLsizei)province_border_vertices.size() + 2);
		int32_t start_x = x;
		int32_t start_y = y;
		int32_t start_dx = dx;
		int32_t start_dy = dy;

		auto current_border_node_index = get_local_node_index(x, y, dx, dy);
		auto initial_node = current_border_node_index;

		assert(safe_get_province_handle_invalid_coords(glm::ivec2(x , y)) == province_map_index);

		glm::vec2 sr = glm::vec2(dx, dy) / 2.f;
		glm::vec2 sf = glm::vec2(-dy, dx) / 2.f;
		auto prev_pos = (glm::vec2(x, y) + 0.5f + sr - sf) / glm::vec2(size_x, size_y);
		auto current_pos = (glm::vec2(x, y) + 0.5f + sr - sf) / glm::vec2(size_x, size_y);
		glm::vec2 next_pos = (glm::vec2(x, y) + 0.5f + sr - sf) / glm::vec2(size_x, size_y);
		float dist = 0.f;

		auto local_prov = province::from_map_id(province_map_index);
		if(!province_to_edges.contains(local_prov.value)) {
			province_to_edges[local_prov.value] = { };
		}

		auto& container = province_to_edges[local_prov.value];

		auto adjacent_province = safe_get_province(glm::ivec2(x + dx, y + dy));
		border_edge current_edge = { };
		current_edge.offset = (int)province_border_vertices.size() + deg_shift;
		current_edge.adj = state.world.get_province_adjacency_by_province_pair(local_prov, province::from_map_id(adjacent_province));
		auto prev_adj = current_edge.adj;
		auto current_adj = current_edge.adj;
		current_edge.associated_province = local_prov;

		//current_edge.true_loop = true_loop;

		bool at_least_one_step = false;
		uint8_t stage = 2;

		bool last_edge_handled = false;
		bool grace_period = false;
		bool loop_is_fixed = false;

		int edge_to_fix = -1;

		bool first_vertex = true;

		auto lay_border = [&](glm::vec2 next) {
			if(!grace_period) {
				last_edge_handled = false;
			}
			grace_period = false;

			prev_pos = current_pos;
			current_pos = next_pos;
			next_pos = next;

			if(prev_pos.x == 1.f && current_pos.x < 0.5f) {
				prev_pos.x = 0.f;
			}

			if(prev_pos.x == 0.f && current_pos.x > 0.5f) {
				prev_pos.x = 1.f; 
			}

			dist += glm::length(prev_pos - current_pos);
			province_border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				current_pos, prev_pos, next_pos,
				province_map_index,
				0.0f, dist
			});
			assert(validate_border_vertex(province_border_vertices.back()));

			if(first_vertex) {
				auto current = province_border_vertices.back();
				province_border_vertices.push_back(current);
				assert(validate_border_vertex(province_border_vertices.back()));
				province_border_vertices.push_back(current);
				assert(validate_border_vertex(province_border_vertices.back()));
				first_vertex = false;
			}
			province_border_vertices.emplace_back(textured_line_vertex_b_enriched_with_province_index{
				current_pos, prev_pos, next_pos,
				province_map_index,
				1.0f, dist
			});
			assert(validate_border_vertex(province_border_vertices.back()));
		};

		//lay_border((glm::vec2(x, y) + 0.5f + sr - sf) / glm::vec2(size_x, size_y));

		/*
		0 -> lay to the right
		1 -> we have moved away from the border
		2 -> we are in the corner
		*/
		uint8_t border_laying_state = 0;
		uint8_t prev_border_laying_state = 0;

		int major_steps = 0;

		bool stop_pathing = false;
		bool avoid_blocking_path = false;

		auto step_forward = [&](bool allow_new_edges) {
			glm::vec2 right_hand = glm::vec2(dx, dy) / 2.f;
			glm::vec2 forward = glm::vec2(-dy, dx) / 2.f;
			auto right_hand_province = safe_get_province_handle_invalid_coords(glm::ivec2(x + dx, y + dy));
			auto forward_province = safe_get_province_handle_invalid_coords(glm::ivec2(x - dy, y + dx));
			auto forward_right_province = safe_get_province_handle_invalid_coords(glm::ivec2(x + dx - dy, y + dy + dx));


			prev_border_laying_state = border_laying_state;

			bool dominating = forward_province != right_hand_province || province_map_index > forward_province;

			if(province_map_index == right_hand_province) {
				border_laying_state = 1;
			} else if(province_map_index == forward_right_province && dominating /* && province_map_index > forward_province && forward_province == right_hand_province*/) {
				border_laying_state = 0;
				avoid_blocking_path = true;
			} else if(province_map_index != forward_province) {
				border_laying_state = 2;
			} else {
				border_laying_state = 0;
			}

			if(border_laying_state != 1 && allow_new_edges && (int)province_border_vertices.size() > current_edge.offset) {
				if(right_hand_province > 0) {
					auto right_hand_province_id = province::from_map_id((uint16_t)right_hand_province);
					prev_adj = current_adj;
					current_adj = state.world.get_province_adjacency_by_province_pair(local_prov, right_hand_province_id);

					auto node_discovered = contains_node(x, y, dx, dy);

					//if(right_hand_province_id != local_prov && current_adj && current_adj != current_edge.adj) {
					if (right_hand_province_id != local_prov && node_discovered) {
						current_edge.count = (int)(province_border_vertices.size() - current_edge.offset + 2);
						if(current_edge.adj)
							adj_index_to_border_edge[current_edge.adj.index()] = border_edges.size();
						container.push_back(border_edges.size());

						current_edge.node_start = (int)current_border_node_index;
						auto& node_object = border_nodes[current_border_node_index];
						assert(node_object.out_count < 4);
						node_object.edges_out[node_object.out_count] = (int)border_edges.size();
						node_object.out_count++;

						current_border_node_index = get_local_node_index(x, y, dx, dy);
						current_edge.node_end = (int)current_border_node_index;
						auto& final_node_object = border_nodes[current_border_node_index];
						assert(final_node_object.in_count < 4);
						final_node_object.edges_in[final_node_object.in_count] = (int)border_edges.size();
						final_node_object.in_count++;

						assert(current_edge.count > 2);

						border_edges.push_back(current_edge);

						if(
							current_edge.node_end == current_edge.node_start
							&& current_border_node_index == initial_node
							&& major_steps == 0
						) {
							//current_edge.true_loop = true;
							//true_loop = true;
							loop_is_fixed = false;
							edge_to_fix = (int)border_edges.size() - 1;


							auto& prev_prev = border_edges[edge_to_fix  - 1];

							assert(border_edges[edge_to_fix].offset > last_vertex_idx);
						} else {
							edge_to_fix = -1;
						}

						last_vertex_idx = current_edge.offset + current_edge.count - 2;

						current_edge.adj = current_adj;
						current_edge.offset = (int)province_border_vertices.size();
						last_edge_handled = true;
						grace_period = true;

						major_steps++;
					}
				} else {
					auto node_discovered = contains_node(x, y, dx, dy);
					//if(current_edge.adj) {
					if(node_discovered) {
						current_edge.count = (int)province_border_vertices.size() - current_edge.offset + 2;
						if(current_edge.adj) {
							adj_index_to_border_edge[current_edge.adj.index()] = border_edges.size();
						}
						container.push_back(border_edges.size());

						current_edge.node_start = (int)current_border_node_index;

						auto& node_object = border_nodes[current_border_node_index];
						assert(node_object.out_count < 4);
						node_object.edges_out[node_object.out_count] = (int)border_edges.size();
						node_object.out_count++;

						current_border_node_index = get_local_node_index(x, y, dx, dy);
						current_edge.node_end = int(current_border_node_index);

						auto& final_node_object = border_nodes[current_border_node_index];
						assert(final_node_object.in_count < 4);
						final_node_object.edges_in[final_node_object.in_count] = int(border_edges.size());
						final_node_object.in_count++;

						assert(current_edge.count > 2);


						border_edges.push_back(current_edge);
						if(
							current_edge.node_end == current_edge.node_start
							&& current_border_node_index == initial_node
							&& major_steps == 0
						) {
							//current_edge.true_loop = true;
							//true_loop = true;
							loop_is_fixed = false;
							edge_to_fix = int(border_edges.size() - 1);
							assert(border_edges[edge_to_fix].offset > last_vertex_idx);
						} else {
							edge_to_fix = -1;
						}

						last_vertex_idx = current_edge.offset + current_edge.count - 2;
						current_edge.offset = int(province_border_vertices.size());

						last_edge_handled = true;
						grace_period = true;

						major_steps++;
					}
					current_edge.adj = {};
				}
			}

			if(border_laying_state == 0) {
				lay_border((glm::vec2(x, y) + right_hand + forward + 0.5f) / glm::vec2(size_x, size_y));
				x += -dy;
				y += dx;
			} else if(border_laying_state == 1) {
				//lay_border((glm::vec2(x, y) + 3.f * right_hand - forward + 0.5f) / glm::vec2(size_x, size_y));
				x += dx;
				y += dy;
				auto next_dx = dy;
				auto next_dy = -dx;
				dx = next_dx;
				dy = next_dy;
			} else if(border_laying_state == 2) {
				lay_border((glm::vec2(x, y) + right_hand + forward + 0.5f) / glm::vec2(size_x, size_y));
				auto next_dx = -dy;
				auto next_dy = dx;
				dx = next_dx;
				dy = next_dy;
			}

			if(compare_local_node_index(x, y, dx, dy, current_border_node_index)) {
				//current_edge.true_loop = true;
				//true_loop = true;
			}

			if(x >= (int)size_x) {
				x -= size_x;
			}
			if(x < 0) {
				x += size_x;
			}
		};

		{
			glm::vec2 right_hand = glm::vec2(dx, dy) / 2.f;
			glm::vec2 forward = glm::vec2(-dy, dx) / 2.f;
			//lay_border((glm::vec2(x, y) + right_hand - forward + 0.5f) / glm::vec2(size_x, size_y));
		}

		do {
			if (!avoid_blocking_path)
				visited[x + y * size_x + encode_direction(dx, dy) * size_x * size_y] = 1;
			avoid_blocking_path = false;
			step_forward(true);
		} while(!(start_x == x && start_y == y && start_dx == dx && start_dy == dy));

		{
			glm::vec2 right_hand = glm::vec2(dx, dy) / 2.f;
			glm::vec2 forward = glm::vec2(-dy, dx) / 2.f;
			//lay_border((glm::vec2(x, y) + right_hand + forward + 0.5f) / glm::vec2(size_x, size_y));
		}


		visited[x + y * size_x + encode_direction(dx, dy) * size_x * size_y] = 1;

		//step_forward();
		//if(!true_loop) {
			step_forward(true);
		//}

		/* {
			size_t first = province_border_starts.back();
			size_t last = province_border_vertices.size() - 2;
			province_border_vertices[first].previous_point = province_border_vertices[last].previous_point;
			province_border_vertices[first + 1].previous_point = province_border_vertices[last + 1].previous_point;
			province_border_vertices[last].next_point = province_border_vertices[first].next_point;
			province_border_vertices[last + 1].next_point = province_border_vertices[first + 1].next_point;

			assert(validate_border_vertex(province_border_vertices[first]));
			assert(validate_border_vertex(province_border_vertices[first + 1]));
			assert(validate_border_vertex(province_border_vertices[last]));
			assert(validate_border_vertex(province_border_vertices[last + 1]));
		}*/

		{
			size_t first = province_border_starts.back();
			size_t last = province_border_vertices.size() - 2;
			province_border_vertices[first].previous_point = province_border_vertices[last].previous_point;
			province_border_vertices[first + 1].previous_point = province_border_vertices[last + 1].previous_point;
			province_border_vertices[last].next_point = province_border_vertices[first].next_point;
			province_border_vertices[last + 1].next_point = province_border_vertices[first + 1].next_point;

			assert(validate_border_vertex(province_border_vertices[first]));
			assert(validate_border_vertex(province_border_vertices[first + 1]));
			assert(validate_border_vertex(province_border_vertices[last]));
			assert(validate_border_vertex(province_border_vertices[last + 1]));
		}

		province_border_counts.push_back(GLsizei(province_border_vertices.size() - province_border_starts.back()));


		assert(initial_node == current_border_node_index);

		if(edge_to_fix != -1) {

			auto current = province_border_vertices.back();
			province_border_vertices.push_back(current);
			province_border_vertices.push_back(current);

			fix_loop(border_edges[edge_to_fix]);
			validate_true_loop(border_edges[edge_to_fix]);

			assert(border_edges.back().offset + (size_t)border_edges.back().count == province_border_vertices.size() - 2);

			return;
		}

		if(border_node_Eq(border_nodes[current_edge.node_end], border_nodes[current_edge.node_start]) && !last_edge_handled) {
			assert(current_edge.offset > last_vertex_idx);

			lay_border(next_pos);

			auto current = province_border_vertices.back();
			current_edge.count = int(province_border_vertices.size() - current_edge.offset);

			province_border_vertices.push_back(current);
			province_border_vertices.push_back(current);

			auto& edge = current_edge;
			auto o = edge.offset;


			assert(edge.count >= 4);

			auto current_count = province_border_vertices.size();
			auto sampled_idx = o + edge.count - 2;
			//while (province_border_vertices.size() == current_count) step_forward(false);

			

			fix_loop(edge);
			validate_true_loop(edge);

			if (current_edge.adj)
				adj_index_to_border_edge[current_edge.adj.index()] = border_edges.size();
			container.push_back(border_edges.size());
			//current_edge.node_start = current_border_node_index;
			//current_edge.node_end = current_border_node_index;

			//current_border_node_index = get_local_node_index(x, y, dx, dy);
			auto& final_node_object = border_nodes[current_border_node_index];
			assert(final_node_object.in_count < 4);
			assert(final_node_object.out_count < 4);
			final_node_object.edges_in[final_node_object.in_count] = (int)border_edges.size();
			final_node_object.in_count++;
			final_node_object.edges_out[final_node_object.out_count] = (int)border_edges.size();
			final_node_object.out_count++;

			assert(current_edge.count > 2);
			last_vertex_idx = current_edge.offset + current_edge.count - 2;

			assert(current_edge.offset + (size_t)current_edge.count == province_border_vertices.size() - 2);
			border_edges.push_back(current_edge);
			return;
		}

		auto current = province_border_vertices.back();
		province_border_vertices.push_back(current);
		province_border_vertices.push_back(current);

		assert(border_edges.back().offset + (size_t)border_edges.back().count == province_border_vertices.size() - 2);
	};

	for(int32_t j = 0; j < int32_t(size_y); ++j) {
		for(int32_t i = 0; i < int32_t(size_x); ++i) {
			auto p00 = safe_get_province(glm::ivec2(i - 1, j - 1));
			auto p01 = safe_get_province(glm::ivec2(i - 1, j));
			auto p10 = safe_get_province(glm::ivec2(i, j - 1));
			auto p11 = safe_get_province(glm::ivec2(i, j));
			auto diff =
				(p00 == p01 ? 0 : 1)
				+ (p00 == p10 ? 0 : 1)
				+ (p11 == p10 ? 0 : 1)
				+ (p11 == p01 ? 0 : 1);

			bool special_case = p00 == p11 || p10 == p01;

			if (diff > 2 && !special_case) {
				register_node_index(i, j, 0, 0, false);
				//border_node new_node { i, j };
				//coord_to_border_node_index[i + j * size_x] = border_nodes.size();
				//border_nodes.emplace_back(new_node);
			}

			if(diff > 2 && special_case) {
				size_t idx;

				idx = register_node_index(i, j, 0, 1, true);
				auto view = border_nodes[idx];
				validate_local_node_index(i - 1, j - 1, 0, 1, idx);
				idx = register_node_index(i, j, 1, 0, true);
				validate_local_node_index(i - 1, j, 1, 0, idx);
				idx = register_node_index(i, j, 0, -1, true);
				validate_local_node_index(i, j, 0, -1, idx);
				idx = register_node_index(i, j, -1, 0, true);
				validate_local_node_index(i, j - 1, -1, 0, idx);
			}
		}
	}

	auto& value_to_watch = border_nodes[0].out_count;

	for(size_t idx = 0; idx < border_nodes.size(); idx++) {
		auto& item = border_nodes[idx];
		auto i = item.x;
		auto j = item.y;
		auto p00 = safe_get_province(glm::ivec2(i - 1, j - 1));
		auto p01 = safe_get_province(glm::ivec2(i - 1, j));
		auto p10 = safe_get_province(glm::ivec2(i, j - 1));
		auto p11 = safe_get_province(glm::ivec2(i, j));

		if(p00 != p01 && (!item.use_subindex || (item.use_subindex && item.subindex == encode_direction(0, 1)))) {
			validate_local_node_index(i - 1, j - 1, 0, 1, idx);
			//assert((int)idx == get_local_node_index(i - 1, j - 1, 0, 1));
			build_loop(p00, i - 1, j - 1, 0, 1, false);
		}
		if(p01 != p11 && (!item.use_subindex || (item.use_subindex && item.subindex == encode_direction(1, 0)))) {
			validate_local_node_index(i - 1, j, 1, 0, idx);
			//assert((int)idx == get_local_node_index(i - 1, j, 1, 0));
			build_loop(p01, i - 1, j, 1, 0, false);
		}
		if(p11 != p10 && (!item.use_subindex || (item.use_subindex && item.subindex == encode_direction(0, -1)))) {
			validate_local_node_index(i , j , 0, -1, idx);
			//assert((int)idx == get_local_node_index(i, j, 0, -1));
			build_loop(p11, i, j, 0, -1, false);
		}
		if(p10 != p00 && (!item.use_subindex || (item.use_subindex && item.subindex == encode_direction(-1, 0)))) {
			validate_local_node_index(i, j - 1, -1, 0, idx);
			//assert((int)idx == get_local_node_index(i, j - 1, -1, 0));
			build_loop(p10, i, j - 1, -1, 0, false);
		}
	}

	for(auto& item : province_border_vertices) {
		validate_border_vertex(item);
	}


	for(auto& edge : border_edges) {
		if(edge.true_loop) {
			auto o = edge.offset;
			assert(province_border_vertices[o + 0 ].position == province_border_vertices[o + edge.count - 2 ].position);
			assert(province_border_vertices[o + 1 ].position == province_border_vertices[o + edge.count - 1 ].position);

			assert(province_border_vertices[o + 0 ].next_point == province_border_vertices[o + edge.count - 2 ].next_point);
			assert(province_border_vertices[o + 1 ].next_point == province_border_vertices[o + edge.count - 1 ].next_point);

			assert(province_border_vertices[o + 0 ].previous_point == province_border_vertices[o + edge.count - 2 ].previous_point);
			assert(province_border_vertices[o + 1 ].previous_point == province_border_vertices[o + edge.count - 1 ].previous_point);
		}
	}


	for(int32_t j = 1; j < int32_t(size_y); ++j) {
		for(int32_t i = 1; i < int32_t(size_x); ++i) {
			//if(visited[(i - 1) + (j - 1) * size_x]) continue;
			//if(visited[(i) + (j - 1) * size_x]) continue;
			//if(visited[(i - 1) + (j) * size_x]) continue;
			//if(visited[(i) + (j) * size_x]) continue;

			auto p00 = safe_get_province(glm::ivec2(i - 1, j - 1));
			auto p01 = safe_get_province(glm::ivec2(i - 1, j));
			auto p10 = safe_get_province(glm::ivec2(i, j - 1));
			auto p11 = safe_get_province(glm::ivec2(i, j));

			if(
				p11 != p01
				&& p11 != p10
				&& visited[i + j * size_x + encode_direction(0, -1) * size_x * size_y] == 0
			) {
				border_node new_node{ i, j, { }, { }, 0, 0 };
				coord_to_border_node_index[i + j * size_x] = border_nodes.size();
				border_nodes.emplace_back(new_node);
				build_loop(p01, i - 1, j, 1, 0, true);
				build_loop(p11, i, j, 0, -1, true);

				border_node temp = border_nodes.back();

				//assert(temp.in_count == temp.out_count);
				//assert(temp.in_count >= 1);
			}

			/*
			if(
				p00 != p01
				&& visited[i - 1 + j * size_x + encode_direction(0, -1) * size_x * size_y] == 0
			) {
				border_node new_node{ i, j, { }, { }, 0, 0 };
				coord_to_border_node_index[i + j * size_x] = border_nodes.size();
				border_nodes.emplace_back(new_node);
				build_loop(p00, i - 1, j - 1, 0, 1, true);
				build_loop(p01, i - 1, j, 0, -1, true);
			} else if(p01 != p11 && visited[i + j * size_x + encode_direction(-1, 0) * size_x * size_y] == 0) {
				border_node new_node{ i, j, { }, { }, 0, 0 };
				coord_to_border_node_index[i + j * size_x] = border_nodes.size();
				border_nodes.emplace_back(new_node);
				build_loop(p01, i - 1, j, 1, 0, true);
				build_loop(p11, i, j, -1, 0, true);
			} else if(p11 != p10 && visited[i + j * size_x + encode_direction(0, -1) * size_x * size_y] == 0) {
				border_node new_node{ i, j, { }, { }, 0, 0 };
				coord_to_border_node_index[i + j * size_x] = border_nodes.size();
				border_nodes.emplace_back(new_node);
				build_loop(p11, i, j, 0, -1, true);
				build_loop(p10, i, j - 1, 0, 1, true);
			} else if(p10 != p00 && visited[i - 1 + (j - 1)*size_x + encode_direction(1, 0) * size_x * size_y] == 0) {
				border_node new_node{ i, j, { }, { }, 0, 0 };
				coord_to_border_node_index[i + j * size_x] = border_nodes.size();
				border_nodes.emplace_back(new_node);
				build_loop(p10, i, j - 1, -1, 0, true);
				build_loop(p00, i - 1, j - 1, 1, 0, true);
			}
			*/
		}
	}

	for(auto& item : province_border_vertices) {
		assert(validate_border_vertex(item));
	}

	auto last_vertex = -1;
	for(auto& edge : border_edges) {
		if(edge.true_loop) {
			validate_true_loop(edge);

			assert(edge.offset > last_vertex);

			auto o = edge.offset;
			assert(province_border_vertices[o + 0].position == province_border_vertices[o + edge.count - 2].position);
			assert(province_border_vertices[o + 1].position == province_border_vertices[o + edge.count - 1].position);

			assert(province_border_vertices[o + 0].next_point == province_border_vertices[o + edge.count - 2].next_point);
			assert(province_border_vertices[o + 1].next_point == province_border_vertices[o + edge.count - 1].next_point);

			assert(province_border_vertices[o + 0].previous_point == province_border_vertices[o + edge.count - 2].previous_point);
			assert(province_border_vertices[o + 1].previous_point == province_border_vertices[o + edge.count - 1].previous_point);
		}
		last_vertex = edge.count  + edge.offset - 2;
	}

	auto smooth_point = [&](textured_line_vertex_b_enriched_with_province_index& item, float t) {
		if(item.next_point.x >= 0.999f) {
			return;
		}
		if(item.previous_point.x >= 0.999f) {
			return;
		}
		if(item.next_point.x <= 0.001f) {
			return;
		}
		if(item.previous_point.x <= 0.001f) {
			return;
		}
		assert(validate_border_vertex(item));
		item.position = t * (item.position) + (1.f - t) / 2.f * (item.previous_point + item.next_point);
		assert(validate_border_vertex(item));
	};

	for(int count = 0; count < 6; count++) {
		for(auto& edge : border_edges) {
			auto start = border_nodes[edge.node_start];
			bool allow_moving_node = start.use_subindex && edge.node_start == edge.node_end;
			if(allow_moving_node) {
				validate_true_loop(edge);

				auto o = edge.offset;
				assert(province_border_vertices[o + 0].position == province_border_vertices[o + edge.count - 2].position);
				assert(province_border_vertices[o + 1].position == province_border_vertices[o + edge.count - 1].position);

				assert(province_border_vertices[o + 0].next_point == province_border_vertices[o + edge.count - 2].next_point);
				assert(province_border_vertices[o + 1].next_point == province_border_vertices[o + edge.count - 1].next_point);

				assert(province_border_vertices[o + 0].previous_point == province_border_vertices[o + edge.count - 2].previous_point);
				assert(province_border_vertices[o + 1].previous_point == province_border_vertices[o + edge.count - 1].previous_point);
			}
		}

		for(auto& edge : border_edges) {
			auto o = edge.offset;
			auto smoothing = 1.f - std::min(0.25f, std::max(0.f, 0.05f * (float)(edge.count - 4)));
			auto comp = (1.f - smoothing) / 2.f;

			auto start = border_nodes[edge.node_start] ;
			auto end = border_nodes[edge.node_end];
			//bool allow_moving_node = edge.true_loop && start.in_count == 2 && start.out_count == 2 && !start.use_subindex;

			bool allow_moving_node = (start.use_subindex && edge.node_start == edge.node_end) || edge.true_loop;

			if(allow_moving_node) {
				validate_true_loop(edge);

				assert(province_border_vertices[o + 0].position == province_border_vertices[o + edge.count - 2].position);
				assert(province_border_vertices[o + 1].position == province_border_vertices[o + edge.count - 1].position);

				for(int i = o; i < o + edge.count; i++) {
					smooth_point(province_border_vertices[i], smoothing);
				}

				validate_true_loop(edge);

				assert(province_border_vertices[o + 0].position == province_border_vertices[o + edge.count - 2].position);
				assert(province_border_vertices[o + 1].position == province_border_vertices[o + edge.count - 1].position);

			} else {
				for(int i = o + 2; i < o + edge.count - 2; i++) {
					smooth_point(province_border_vertices[i], smoothing);
				}
			}
			for(int i = o + 2; i < o + edge.count; i++) {
				assert(validate_border_vertex(province_border_vertices[i]));
				province_border_vertices[i].previous_point = province_border_vertices[i - 2].position;
				assert(validate_border_vertex(province_border_vertices[i]));
			}
			for(int i = o + 0; i < o + edge.count - 2; i++) {
				assert(validate_border_vertex(province_border_vertices[i]));
				province_border_vertices[i].next_point = province_border_vertices[i + 2].position;
				assert(validate_border_vertex(province_border_vertices[i]));
			}

			if(allow_moving_node) {
				fix_loop(edge);
				/*
				assert(validate_border_vertex(province_border_vertices[o + 0]));
				assert(validate_border_vertex(province_border_vertices[o + 1]));
				assert(validate_border_vertex(province_border_vertices[o + edge.count - 2]));
				assert(validate_border_vertex(province_border_vertices[o + edge.count - 1]));

				assert(province_border_vertices[o + 0].position == province_border_vertices[o + edge.count - 2].position);
				assert(province_border_vertices[o + 1].position == province_border_vertices[o + edge.count - 1].position);

				province_border_vertices[o + 0].previous_point = province_border_vertices[o + edge.count - 2].previous_point;
				province_border_vertices[o + 1].previous_point = province_border_vertices[o + edge.count - 1].previous_point;
				province_border_vertices[o + edge.count - 2].next_point = province_border_vertices[o + 0].next_point;
				province_border_vertices[o + edge.count - 1].next_point = province_border_vertices[o + 1].next_point;

				assert(validate_border_vertex(province_border_vertices[o + 0]));
				assert(validate_border_vertex(province_border_vertices[o + 1]));
				assert(validate_border_vertex(province_border_vertices[o + edge.count - 2]));
				assert(validate_border_vertex(province_border_vertices[o + edge.count - 1]));
				*/
			}
		}
	}

	// restore degenerate positions

	for(size_t i = 0; i < province_border_counts.size(); i++) {
		auto o = province_border_starts[i] - 2;
		auto c = province_border_counts[i] + 2;

		province_border_vertices[o].position = province_border_vertices[o + 2].position;
		province_border_vertices[o + 1].position = province_border_vertices[o + 3].position;

		province_border_vertices[o + c].position = province_border_vertices[o + c - 2].position;
		province_border_vertices[o + 1 + c].position = province_border_vertices[o + 1 + c - 2].position;
	}

	for(auto& edge : border_edges) {
		auto o = edge.offset;
		float dist = 0.f;
		for(int i = o + 0; i < o + edge.count - 2; i +=2 ) {
			province_border_vertices[i].distance = dist;
			province_border_vertices[i + 1].distance = dist;
			dist = dist + glm::distance(province_border_vertices[i + 2].position * glm::vec2(1.f, (float)size_y / (float)size_x), province_border_vertices[i].position * glm::vec2(1.f, (float)size_y / (float)size_x));
		}
		province_border_vertices[o + edge.count - 2].distance = dist;
		province_border_vertices[o + edge.count - 1].distance = dist;
	}

	for(size_t idx = 0; idx < state.map_state.map_data.province_border_starts.size(); idx++) {
		auto boffset = state.map_state.map_data.province_border_starts[idx];
		auto bcount = state.map_state.map_data.province_border_counts[idx];

		float dist = 0.f;
		for(int i = boffset + 0; i < boffset + bcount - 2; i += 2) {
			province_border_vertices[i].distance = dist;
			province_border_vertices[i + 1].distance = dist;
			dist = dist + glm::distance(province_border_vertices[i + 2].position * glm::vec2(1.f, (float) size_y / (float) size_x), province_border_vertices[i].position * glm::vec2(1.f, (float)size_y / (float)size_x));
		}
		province_border_vertices[boffset + bcount - 2].distance = dist;
		province_border_vertices[boffset + bcount - 1].distance = dist;
	}


	// generate siblings
	for(size_t edge_index = 0; edge_index < border_edges.size(); edge_index++) {
		auto& edge = border_edges[edge_index];
		auto& origin = border_nodes[edge.node_start];

		if(!origin.use_subindex) {
			for(uint8_t i = 0; i < origin.in_count; i++) {
				auto& potential_sibling = border_edges[origin.edges_in[i]];
				auto& our_final = border_nodes[edge.node_end];
				auto& their_beginning = border_nodes[potential_sibling.node_start];
				if(border_node_Eq(our_final, their_beginning) && potential_sibling.associated_province != edge.associated_province) {
					//potential_sibling.sibling = edge_index;
					auto& sample_candidate = province_border_vertices[potential_sibling.offset];
					auto& sample_us = province_border_vertices[edge.offset + edge.count - 1];
					auto dist = glm::length((sample_candidate.next_point - sample_candidate.position - sample_us.previous_point + sample_us.position) * glm::vec2(size_x, size_y));
					if(dist < 0.5f) {
						edge.sibling = origin.edges_in[i];
					}
				}
			}
		} else {
			for(int encoding = 0; encoding < 5; encoding += 1) {
				if (encoding == 2) continue;
				auto& potential_origin = border_nodes[get_local_node_index_2(origin.x, origin.y, encoding)];
				for(uint8_t i = 0; i < potential_origin.in_count; i++) {
					auto& potential_sibling = border_edges[potential_origin.edges_in[i]];
					auto& our_final = border_nodes[edge.node_end];
					auto& their_beginning = border_nodes[potential_sibling.node_start];
					if(border_node_Eq(our_final, their_beginning) && potential_sibling.associated_province != edge.associated_province) {
						//potential_sibling.sibling = edge_index
						auto& sample_candidate = province_border_vertices[potential_sibling.offset];
						auto& sample_us = province_border_vertices[edge.offset + edge.count - 1];

						auto dist = glm::length(
							(
								sample_candidate.next_point - sample_candidate.position
								- sample_us.previous_point + sample_us.position
							) * glm::vec2(size_x, size_y)
						);

						if(dist < 0.5f) {
							edge.sibling = potential_origin.edges_in[i];
						}
					}

				}
			}
			assert(edge.sibling > -1);
		}


		/*
		if(edge.sibling > -1) {
			//province_border_vertices[edge.offset + 0].distance = 0.f;
			//province_border_vertices[edge.offset + 1].distance = 0.f;
			province_border_vertices[edge.offset + edge.count - 2].distance = province_border_vertices[edge.offset + 0].distance;
			province_border_vertices[edge.offset + edge.count - 1].distance = province_border_vertices[edge.offset + 1].distance;

			auto length = province_border_vertices[edge.offset + edge.count - 1].distance;
			for(int i = 0; i < edge.count; i++) {
				auto& sibling = border_edges[edge.sibling];
				province_border_vertices[sibling.offset + i].distance = length - province_border_vertices[edge.offset + i].distance;
			}
		}
		*/
	}

	// weave state definition borders:

	auto make_state_definition_loop = [&](border_node& start) {
		if(start.out_count == 0) {
			return;
		}
		
		auto p_sample = border_edges[start.edges_out[0]].associated_province;
		auto sd_sample = state.world.province_get_state_from_abstract_state_membership(p_sample);

		bool state_corner = false;

		for(uint8_t i = 0; i < start.out_count; i++) {
			auto p  = border_edges[start.edges_out[i]].associated_province;
			auto sd = state.world.province_get_state_from_abstract_state_membership(p);

			if(sd != sd_sample) {
				state_corner = true;
			}
		}

		if(state_corner) {
			start.state_definition_corner = true;
		} else {
			return;
		}
	};


	for(auto& item : border_nodes) {
		make_state_definition_loop(item);
	}

	auto good_state_border = [&](border_edge& edge) {
		auto p_left = edge.associated_province;
		auto sd_left = state.world.province_get_state_from_abstract_state_membership(p_left);

		if(edge.sibling == -1) {
			return false;
		}

		auto& sibling = border_edges[edge.sibling];
		auto p_right = sibling.associated_province;
		auto sd_right = state.world.province_get_state_from_abstract_state_membership(p_right);

		if(sd_left == sd_right) {
			return false;
		}
		return true;
		};

	auto state_border = [&](border_edge& edge, dcon::state_definition_id sdid) {
		auto p_left = edge.associated_province;
		auto sd_left = state.world.province_get_state_from_abstract_state_membership(p_left);
		return sd_left == sdid;
		};
	auto weave_state_border = [&](border_edge& edge, bool& first) {
		if(!good_state_border(edge)) {
			return;
		}
		auto p_left = edge.associated_province;
		auto sd_left = state.world.province_get_state_from_abstract_state_membership(p_left);

		// weave previous edge into it

		for(auto k = edge.offset; k < edge.offset + edge.count; k++) {
			state_border_vertices.push_back(province_border_vertices[k]);
			if(first) {
				state_border_vertices.push_back(province_border_vertices[k]);
				state_border_vertices.push_back(province_border_vertices[k]);
				first = false;
			}
		}

		auto& origin_node = border_nodes[edge.node_start];
		for(uint8_t j = 0; j < origin_node.in_count; j++) {
			auto& previous_edge = border_edges[origin_node.edges_in[j]];
			if(!good_state_border(previous_edge)) continue;
			if(!state_border(previous_edge, sd_left)) continue;
			state_border_vertices[state_border_starts.back()].previous_point = province_border_vertices[previous_edge.offset + previous_edge.count - 4].position;
			state_border_vertices[state_border_starts.back() + 1].previous_point = province_border_vertices[previous_edge.offset + previous_edge.count - 3].position;
		}

		auto& final_node = border_nodes[edge.node_end];
		for(uint8_t j = 0; j < final_node.out_count; j++) {
			auto& next_edge = border_edges[final_node.edges_out[j]];
			if(!good_state_border(next_edge)) continue;
			if(!state_border(next_edge, sd_left)) continue;
			state_border_vertices[state_border_vertices.size() - 1].next_point = province_border_vertices[next_edge.offset + 3].position;
			state_border_vertices[state_border_vertices.size() - 2].next_point = province_border_vertices[next_edge.offset + 2].position;
		}
	};

	// start with filling the vertices buffer with long border "snakes"

	std::vector<uint8_t> visited_edges;
	visited_edges.resize(border_edges.size());

	for(size_t idx = 0; idx < border_edges.size(); idx++) {
		if(visited_edges[idx]) {
			continue;
		}
		if(!good_state_border(border_edges[idx])) {
			visited_edges[idx] = 1;
			continue;
		}


		auto& edge = border_edges[idx];
		auto p_left = edge.associated_province;
		auto state_left = state.world.province_get_state_from_abstract_state_membership(p_left);

		size_t current_idx = idx;

		int timeout = 100;

		// go back to the start of the chain
		do {
			bool path_found = false;
			auto& origin = border_nodes[edge.node_start];
			for(uint8_t in_idx = 0; in_idx < origin.in_count; in_idx++) {
				auto& previous_edge = border_edges[origin.edges_in[in_idx]];
				if(!good_state_border(previous_edge)) {
					visited_edges[origin.edges_in[in_idx]] = 1;
					continue;
				}
				if(state_border(previous_edge, state_left) && (size_t)origin.edges_in[in_idx] != current_idx) {
					current_idx = origin.edges_in[in_idx];
					path_found = true;
					break;
				}
			}
			if(!path_found) {
				break;
			}
			timeout--;
		} while(current_idx != idx && timeout > 0);

		/*
		Now we are either
			1) At the start of the loop
			2) At the start of the border chain
		So we can just go forward and push vertices to the buffer
		*/

		timeout = 200;

		state_border_starts.push_back((GLsizei)state_border_vertices.size());
		size_t marked_idx = current_idx;
		bool first = true;
		do {
			bool path_found = false;
			// weave
			visited_edges[current_idx] = 1;
			auto& edge_to_add = border_edges[current_idx];
			weave_state_border(edge_to_add, first);

			// find the next edge

			auto& end = border_nodes[edge_to_add.node_end];
			for(uint8_t out_idx = 0; out_idx < end.out_count; out_idx++) {
				auto& next_edge = border_edges[end.edges_out[out_idx]];
				if(!good_state_border(next_edge)) {
					visited_edges[end.edges_out[out_idx]] = 1;
					continue;
				}
				if(state_border(next_edge, state_left) && (size_t)end.edges_out[out_idx] != current_idx) {
					current_idx = end.edges_out[out_idx];
					path_found = true;
					break;
				}
			}
			if(!path_found) {
				break;
			}
			timeout--;
		} while(current_idx != marked_idx && timeout > 0);
		auto back = state_border_vertices.back();
		state_border_vertices.push_back(back);
		state_border_vertices.push_back(back);

		state_border_counts.push_back(GLsizei(state_border_vertices.size() - state_border_starts.back()));
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

			// runner connects provinces around it:
			if (!runner.waiting_for_merge) {
				auto i = runner.x;
				auto j = runner.y;
				std::vector<dcon::province_id> provinces = {
					province::from_map_id(safe_get_province(glm::ivec2(i - 1, j))),
					province::from_map_id(safe_get_province(glm::ivec2(i + 1, j))),
					province::from_map_id(safe_get_province(glm::ivec2(i, j - 1))),
					province::from_map_id(safe_get_province(glm::ivec2(i, j + 1))),
					province::from_map_id(safe_get_province(glm::ivec2(i, j)))
				};
				for(auto& origin : provinces) {
					// The base paradox map has undefined pxiels which do not belong to a province. To prevent invalid writes these should be filtered out
					if(!bool(origin)) {
						continue;
					}
					if(runner.position->width > 125) {
						context.state.world.province_set_has_major_river(origin, true);
					} else {
						context.state.world.province_set_has_minor_river(origin, true);
					}

					for(auto& target : provinces) {
						auto adj = context.state.world.get_province_adjacency_by_province_pair(origin, target);
						if(adj) {
							uint8_t& buffer = context.state.world.province_adjacency_get_type(adj);
							buffer |= province::border::river_connection_bit;
						}
					}
				}
			}

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
				if(is_river_merge(river_data[runner.x + runner.y * size_x])) {
					exists_running_runner = true;
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

						exists_running_runner = true;

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
						exists_running_runner = true;
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
				for(int steps = 0; steps < 0; steps++) {
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
			float rotation_distance = 0.0f;
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
					next_point = next_point + current_tangent * 1.f;
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

				vertex = next_point;

				//vertex = t * next_point + (1.f - t) * current_point;
				auto width = runner->width;

				auto current_weight = weight * width;

				auto base_direction = vertex - vertex_stabilized;
				auto direction_norm = glm::length(base_direction);
				auto max_steps = 20;
				float meander_length = 0.1f;
				//float speed = 40.f;
				auto step = 0.2f;
				auto speed = 1.f;
				float steps_per_second = speed / step;
				auto t = 0.65f;

				while(direction_norm > step * 8.f && max_steps > 0) {
					base_direction = vertex - vertex_stabilized;
					direction_norm = glm::length(base_direction);

					// rotate:
					auto rotation = sin(rotation_distance / 2.f / width * 150.f / width * 150.f) * 2.f;
					glm::mat2x2 rotation_matrix = glm::mat2x2{ cos(rotation), sin(rotation), -sin(rotation), cos(rotation) };

					auto acceleration = (rotation_matrix * base_direction * t + base_direction * (1.f - t)) / direction_norm; // / current_weight;

					vertex_stabilized_speed =
						vertex_stabilized_speed * 0.5f
						+ acceleration * step;

					rotation_distance += glm::length(vertex_stabilized_speed * steps_per_second * step) * (1.f + 2.f * abs(sin(vertex_stabilized.x / 10.f)));

					//fprintf(pf, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n", vertex_stabilized.x, vertex_stabilized.y, vertex_stabilized_speed.x, vertex_stabilized_speed.y, vertex.x, vertex.y);

					auto vertex_old_opengl_coords = vertex_stabilized / glm::vec2(size_x, size_y);
					vertex_stabilized = vertex_stabilized + vertex_stabilized_speed * steps_per_second * step;

					auto vertex_opengl_coords = vertex_stabilized / glm::vec2(size_x, size_y);
					auto speed_opengl_coords = vertex_opengl_coords - vertex_old_opengl_coords;
					auto normal_opengl_coords = glm::normalize(glm::vec2{ -speed_opengl_coords.y, speed_opengl_coords.x });
					distance += glm::length(speed_opengl_coords);
					river_vertices.emplace_back(textured_line_with_width_vertex{ vertex_opengl_coords, +normal_opengl_coords, 0.0f, distance, width });//C
					river_vertices.emplace_back(textured_line_with_width_vertex{ vertex_opengl_coords, -normal_opengl_coords, 1.0f, distance, width });//D
					max_steps--;
				}


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
