#include "map_state.hpp"

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

#include "system_state.hpp"
#include "military.hpp"
#include "parsers_declarations.hpp"
#include "gui_graphics.hpp"
#include "gui_element_base.hpp"

#include <set>

namespace map {

dcon::province_id map_state::get_selected_province() {
	return selected_province;
}

// Called to load the map. Will load the texture and shaders from disk
void map_state::load_map(sys::state& state) {
	map_data.load_map(state);
}

map_view map_state::current_view(sys::state& state) {
	auto current_view = map::map_view::globe;
	if(state.user_settings.map_is_globe == sys::projection_mode::flat) {
		current_view = map::map_view::flat;
	} else if(state.user_settings.map_is_globe == sys::projection_mode::globe_perpect) {
		current_view = map::map_view::globe_perspect;
	}
	return current_view;
}

void map_state::set_selected_province(dcon::province_id prov_id) {
	unhandled_province_selection = selected_province != prov_id;
	selected_province = prov_id;
}

void map_state::render(sys::state& state, uint32_t screen_x, uint32_t screen_y) {
	update(state);
	glm::vec2 offset = glm::vec2(glm::mod(pos.x, 1.f) - 0.5f, pos.y - 0.5f);
	map_data.render(state, glm::vec2(screen_x, screen_y), offset, zoom,
			current_view(state), active_map_mode, globe_rotation, time_counter);
}

glm::vec2 get_port_location(sys::state& state, dcon::province_id p) {
	auto pt = state.world.province_get_port_to(p);
	if(!pt)
		return glm::vec2{};

	auto adj = state.world.get_province_adjacency_by_province_pair(p, pt);
	assert(adj);
	auto id = adj.index();
	auto& map_data = state.map_state.map_data;
	auto& border = map_data.borders[id];
	auto& vertex = map_data.border_vertices[border.start_index + border.count / 2];
	glm::vec2 map_size = glm::vec2(map_data.size_x, map_data.size_y);

	return vertex.position * map_size;
}

bool is_sea_province(sys::state& state, dcon::province_id prov_id) {
	return prov_id.index() >= state.province_definitions.first_sea_province.index();
}

glm::vec2 get_navy_location(sys::state& state, dcon::province_id prov_id) {
	if(is_sea_province(state, prov_id))
		return state.world.province_get_mid_point(prov_id);
	else
		return get_port_location(state, prov_id);
}

glm::vec2 get_army_location(sys::state& state, dcon::province_id prov_id) {
	return state.world.province_get_mid_point(prov_id);
}

void update_unit_arrows(sys::state& state, display_data& map_data) {
	map_data.unit_arrow_vertices.clear();
	map_data.unit_arrow_counts.clear();
	map_data.unit_arrow_starts.clear();

	map_data.attack_unit_arrow_vertices.clear();
	map_data.attack_unit_arrow_counts.clear();
	map_data.attack_unit_arrow_starts.clear();

	map_data.retreat_unit_arrow_vertices.clear();
	map_data.retreat_unit_arrow_counts.clear();
	map_data.retreat_unit_arrow_starts.clear();

	map_data.strategy_unit_arrow_vertices.clear();
	map_data.strategy_unit_arrow_counts.clear();
	map_data.strategy_unit_arrow_starts.clear();

	map_data.objective_unit_arrow_vertices.clear();
	map_data.objective_unit_arrow_counts.clear();
	map_data.objective_unit_arrow_starts.clear();

	map_data.other_objective_unit_arrow_vertices.clear();
	map_data.other_objective_unit_arrow_counts.clear();
	map_data.other_objective_unit_arrow_starts.clear();

	for(auto selected_army : state.selected_armies) {
		if(auto ps = state.world.army_get_path(selected_army); ps.size() > 0) {
			auto dest_controller = state.world.province_get_nation_from_province_control(ps[0]);
			if(state.world.army_get_black_flag(selected_army) || state.world.army_get_is_retreating(selected_army)) {
				auto old_size = map_data.retreat_unit_arrow_vertices.size();
				map_data.retreat_unit_arrow_starts.push_back(GLint(old_size));
				map::make_army_path(state, map_data.retreat_unit_arrow_vertices, selected_army, float(map_data.size_x), float(map_data.size_y));
				map_data.retreat_unit_arrow_counts.push_back(GLsizei(map_data.retreat_unit_arrow_vertices.size() - old_size));
			} else if(state.world.army_get_is_ai_controlled(selected_army)) {
				if(state.local_player_nation && dest_controller && military::are_at_war(state, dest_controller, state.local_player_nation)) {
					auto old_size = map_data.other_objective_unit_arrow_vertices.size();
					map_data.other_objective_unit_arrow_starts.push_back(GLint(old_size));
					map::make_army_path(state, map_data.other_objective_unit_arrow_vertices, selected_army, float(map_data.size_x), float(map_data.size_y));
					map_data.other_objective_unit_arrow_counts.push_back(GLsizei(map_data.other_objective_unit_arrow_vertices.size() - old_size));
				} else {
					auto old_size = map_data.objective_unit_arrow_vertices.size();
					map_data.objective_unit_arrow_starts.push_back(GLint(old_size));
					map::make_army_path(state, map_data.objective_unit_arrow_vertices, selected_army, float(map_data.size_x), float(map_data.size_y));
					map_data.objective_unit_arrow_counts.push_back(GLsizei(map_data.objective_unit_arrow_vertices.size() - old_size));
				}
			} else if(state.local_player_nation && dest_controller && military::are_at_war(state, dest_controller, state.local_player_nation)) {
				auto old_size = map_data.attack_unit_arrow_vertices.size();
				map_data.attack_unit_arrow_starts.push_back(GLint(old_size));
				map::make_army_path(state, map_data.attack_unit_arrow_vertices, selected_army, float(map_data.size_x), float(map_data.size_y));
				map_data.attack_unit_arrow_counts.push_back(GLsizei(map_data.attack_unit_arrow_vertices.size() - old_size));
			} else {
				auto old_size = map_data.unit_arrow_vertices.size();
				map_data.unit_arrow_starts.push_back(GLint(old_size));
				map::make_army_path(state, map_data.unit_arrow_vertices, selected_army, float(map_data.size_x), float(map_data.size_y));
				map_data.unit_arrow_counts.push_back(GLsizei(map_data.unit_arrow_vertices.size() - old_size));
			}
		}
	}
	for(auto selected_navy : state.selected_navies) {
		if(state.world.navy_get_is_retreating(selected_navy)) {
			auto old_size = map_data.retreat_unit_arrow_vertices.size();
			map_data.retreat_unit_arrow_starts.push_back(GLint(old_size));
			map::make_navy_path(state, map_data.retreat_unit_arrow_vertices, selected_navy, float(map_data.size_x), float(map_data.size_y));
			map_data.retreat_unit_arrow_counts.push_back(GLsizei(map_data.retreat_unit_arrow_vertices.size() - old_size));
		} else {
			auto old_size = map_data.unit_arrow_vertices.size();
			map_data.unit_arrow_starts.push_back(GLint(old_size));
			map::make_navy_path(state, map_data.unit_arrow_vertices, selected_navy, float(map_data.size_x), float(map_data.size_y));
			map_data.unit_arrow_counts.push_back(GLsizei(map_data.unit_arrow_vertices.size() - old_size));
		}
	}

	if(!map_data.unit_arrow_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, map_data.vbo_array[map_data.vo_unit_arrow]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(curved_line_vertex) * map_data.unit_arrow_vertices.size(), map_data.unit_arrow_vertices.data(), GL_STATIC_DRAW);
	}
	if(!map_data.attack_unit_arrow_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, map_data.vbo_array[map_data.vo_attack_unit_arrow]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(curved_line_vertex) * map_data.attack_unit_arrow_vertices.size(), map_data.attack_unit_arrow_vertices.data(), GL_STATIC_DRAW);
	}
	if(!map_data.retreat_unit_arrow_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, map_data.vbo_array[map_data.vo_retreat_unit_arrow]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(curved_line_vertex) * map_data.retreat_unit_arrow_vertices.size(), map_data.retreat_unit_arrow_vertices.data(), GL_STATIC_DRAW);
	}
	if(!map_data.strategy_unit_arrow_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, map_data.vbo_array[map_data.vo_strategy_unit_arrow]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(curved_line_vertex) * map_data.strategy_unit_arrow_vertices.size(), map_data.strategy_unit_arrow_vertices.data(), GL_STATIC_DRAW);
	}
	if(!map_data.objective_unit_arrow_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, map_data.vbo_array[map_data.vo_objective_unit_arrow]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(curved_line_vertex) * map_data.objective_unit_arrow_vertices.size(), map_data.objective_unit_arrow_vertices.data(), GL_STATIC_DRAW);
	}
	if(!map_data.other_objective_unit_arrow_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, map_data.vbo_array[map_data.vo_other_objective_unit_arrow]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(curved_line_vertex) * map_data.other_objective_unit_arrow_vertices.size(), map_data.other_objective_unit_arrow_vertices.data(), GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void update_bbox(std::array<glm::vec2, 5>& bbox, glm::vec2 p) {
	if(p.x <= bbox[1].x) {
		bbox[1] = p;
	} if(p.y <= bbox[2].y) {
		bbox[2] = p;
	} if(p.x >= bbox[3].x) {
		bbox[3] = p;
	} if(p.y >= bbox[4].y) {
		bbox[4] = p;
	}
}

bool is_inside_bbox(std::array<glm::vec2, 5>& bbox, glm::vec2 p) {
	if(p.x <= bbox[1].x) {
		return false;
	} if(p.y <= bbox[2].y) {
		return false;
	} if(p.x >= bbox[3].x) {
		return false;
	} if(p.y >= bbox[4].y) {
		return false;
	}

	return true;
}

void update_bbox_negative(std::array<glm::vec2, 5>& bbox, glm::vec2 p) {
	if(!is_inside_bbox(bbox, p))
		return;

	//auto mp = p.get_mid_point();
	//if(mp.x <= bbox[0].x)
	//	bbox[1].x = mp.x * 0.1f + bbox[1].x * 0.9f;
	//if(mp.x >= bbox[0].x)
	//	bbox[3].x = mp.x * 0.1f + bbox[3].x * 0.9f;
}

dcon::nation_id get_top_overlord(sys::state& state, dcon::nation_id n) {
	auto olr = state.world.nation_get_overlord_as_subject(n);
	auto ol = state.world.overlord_get_ruler(olr);
	auto ol_temp = n;

	while(ol && state.world.nation_get_name(ol)) {
		olr = state.world.nation_get_overlord_as_subject(ol);
		ol_temp = ol;
		ol = state.world.overlord_get_ruler(olr);
	}

	return ol_temp;
}

void update_text_lines(sys::state& state, display_data& map_data) {
	auto& f = state.font_collection.fonts[text::font_index_from_font_id(state, 0x80)];
	assert(f.loaded);

	// retroscipt
	std::vector<text_line_generator_data> text_data;
	std::vector<bool> visited(65536, false);
	std::vector<bool> visited_sea_provinces(65536, false);
	std::vector<bool> friendly_sea_provinces(65536, false);
	std::vector<uint16_t> group_of_regions;

	std::unordered_map<uint16_t, std::set<uint16_t>> regions_graph;

	int samples_N = 200;
	int samples_M = 100;
	float step_x = float(map_data.size_x) / float(samples_N);
	float step_y = float(map_data.size_y) / float(samples_M);

	// generate additional points
	/*std::vector<uint16_t> samples_regions;
	for(int i = 0; i < samples_N; i++)
		for(int j = 0; j < samples_M; j++) {
			float x = float(i) * step_x;
			float y = float(map_data.size_y) - float(j) * step_y;
			auto idx = int32_t(y) * int32_t(map_data.size_x) + int32_t(x);

			if(0 <= idx && size_t(idx) < map_data.province_id_map.size()) {
				auto fat_id = dcon::fatten(state.world, province::from_map_id(map_data.province_id_map[idx]));
				samples_regions.push_back(fat_id.get_connected_region_id());
			} else {
				samples_regions.push_back(0);
			}
		}*/

	// generate graph of regions:
	for(auto candidate : state.world.in_province) {
		auto rid = candidate.get_connected_region_id();

		auto nation = get_top_overlord(state, state.world.province_get_nation_from_province_ownership(candidate));

		for(auto adj : candidate.get_province_adjacency()) {
			auto indx = adj.get_connected_provinces(0) != candidate.id ? 0 : 1;
			auto neighbor = adj.get_connected_provinces(indx);

			// if sea, try to jump to the next province
			if(neighbor.id.index() >= state.province_definitions.first_sea_province.index()) {
				for(auto adj_of_neighbor : neighbor.get_province_adjacency()) {
					auto indx2 = adj_of_neighbor.get_connected_provinces(0) != neighbor.id ? 0 : 1;
					auto neighbor_of_neighbor = adj_of_neighbor.get_connected_provinces(indx2);

					// check for a "cut line"
					glm::vec2 point_candidate = candidate.get_mid_point();
					glm::vec2 point_potential_friend = neighbor_of_neighbor.get_mid_point();

					if(glm::distance(point_candidate, point_potential_friend) > map_data.size_x * 0.5f) {
						// do nothing
					} else if(neighbor_of_neighbor.id.index() < state.province_definitions.first_sea_province.index()) {
						auto nation_2 = get_top_overlord(state, state.world.province_get_nation_from_province_ownership(neighbor_of_neighbor));
						if(nation == nation_2)
							regions_graph[rid].insert(neighbor_of_neighbor.get_connected_region_id());
					}
				}
			} else {
				auto nation_2 = get_top_overlord(state, state.world.province_get_nation_from_province_ownership(neighbor));
				if(nation == nation_2)
					regions_graph[rid].insert(neighbor.get_connected_region_id());
			}
		}
	}
	for(auto p : state.world.in_province) {
		if(p.id.index() >= state.province_definitions.first_sea_province.index())
			break;
		auto rid = p.get_connected_region_id();
		if(visited[uint16_t(rid)])
			continue;
		visited[uint16_t(rid)] = true;

		auto n = p.get_nation_from_province_ownership();
		n = get_top_overlord(state, n.id);

		//flood fill regions
		group_of_regions.clear();
		group_of_regions.push_back(rid);
		int first_index = 0;
		int vacant_index = 1;
		while(first_index < vacant_index) {
			auto current_region = group_of_regions[first_index];
			first_index++;
			for(auto neighbour_region : regions_graph[current_region]) {
				if(!visited[neighbour_region]) {
					group_of_regions.push_back(neighbour_region);
					visited[neighbour_region] = true;
					vacant_index++;
				}
			}
		}
		if(!n || !n.get_name())
			continue;

		auto nation_name = text::produce_simple_string(state, n.get_name());
		auto prefix_remove = text::produce_simple_string(state, "map_remove_prefix");
		if(nation_name.starts_with(prefix_remove)) {
			nation_name.erase(0, prefix_remove.size());
		}
		auto acronym_expand = text::produce_simple_string(state, "map_expand_acronym");
		if(acronym_expand.size() > 0 && nation_name.starts_with(acronym_expand)) {
			nation_name.erase(0, acronym_expand.size());
			auto acronym_expand_to = text::produce_simple_string(state, "map_expand_acronym_to");
			nation_name.insert(0, acronym_expand_to.data(), acronym_expand_to.size());
		}

		std::string name = nation_name;
		bool connected_to_capital = false;
		for(auto visited_region : group_of_regions) {
			if(n.get_capital().get_connected_region_id() == visited_region) {
				connected_to_capital = true;
			}
		}
		text::substitution_map sub{};
		text::add_to_substitution_map(sub, text::variable_type::adj, n.get_adjective());
		text::add_to_substitution_map(sub, text::variable_type::country, std::string_view(nation_name));
		text::add_to_substitution_map(sub, text::variable_type::province, p);
		text::add_to_substitution_map(sub, text::variable_type::state, p.get_state_membership());
		text::add_to_substitution_map(sub, text::variable_type::continentname, p.get_continent().get_name());
		if(!connected_to_capital) {
			// Adjective + " " + Continent
			name = text::resolve_string_substitution(state, "map_label_adj_continent", sub);
			// 66% of the provinces correspond to a single national identity
			// then it gets named after that identity
			ankerl::unordered_dense::map<int32_t, uint32_t> map;
			uint32_t total_provinces = 0;
			dcon::province_id last_province;
			bool in_same_state = true;
			for(auto visited_region : group_of_regions) {
				for(auto candidate : state.world.in_province) {
					if(candidate.get_connected_region_id() == visited_region) {
						if(candidate.get_state_membership() != p.get_state_membership())
							in_same_state = false;
						++total_provinces;
						for(const auto core : candidate.get_core_as_province()) {
							uint32_t v = 1;
							if(auto const it = map.find(core.get_identity().id.index()); it != map.end()) {
								v += it->second;
							}
							map.insert_or_assign(core.get_identity().id.index(), v);
						}
					}
				}
			}
			if(in_same_state == true) {
				name = text::resolve_string_substitution(state, "map_label_adj_state", sub);
			}
			if(total_provinces <= 2) {
				// Adjective + Province name
				name = text::resolve_string_substitution(state, "map_label_adj_province", sub);
			} else {
				for(const auto& e : map) {
					if(float(e.second) / float(total_provinces) >= 0.75f) {
						// Adjective + " " + National identity
						auto const nid = dcon::national_identity_id(dcon::national_identity_id::value_base_t(e.first));
						if(state.world.national_identity_get_name(nid)) {
							if(nid == n.get_primary_culture().get_group_from_culture_group_membership().get_identity_from_cultural_union_of()
							|| nid == n.get_identity_from_identity_holder()) {
								if(n.get_capital().get_continent() == state.world.province_get_continent(last_province)) {
									//cultural union tag -> use our name
									name = text::produce_simple_string(state, n.get_name());
									//Get cardinality
									auto p1 = n.get_capital().get_mid_point();
									auto p2 = p.get_mid_point();
									auto radians = glm::atan(p1.y - p2.y, p2.x - p1.x);
									auto degrees = std::fmod(glm::degrees(radians) + 45.f, 360.f);
									if(degrees < 0.f) {
										degrees = 360.f + degrees;
									}
									assert(degrees >= 0.f && degrees <= 360.f);
									if(degrees >= 0.f && degrees < 90.f) {
										name = text::resolve_string_substitution(state, "map_label_east_country", sub);
									} else if(degrees >= 90.f && degrees < 180.f) {
										name = text::resolve_string_substitution(state, "map_label_south_country", sub);
									} else if(degrees >= 180.f && degrees < 270.f) {
										name = text::resolve_string_substitution(state, "map_label_west_country", sub);
									} else if(degrees >= 270.f && degrees < 360.f) {
										name = text::resolve_string_substitution(state, "map_label_north_country", sub);
									}
								}
							} else {
								text::add_to_substitution_map(sub, text::variable_type::tag, state.world.national_identity_get_name(nid));
								name = text::resolve_string_substitution(state, "map_label_adj_tag", sub);
							}
							break;
						}
					}
				}
			}
		}
		//name = "جُمْهُورِيَّة ٱلْعِرَاق";
		//name = "在标准状况下";
		if(name.empty())
			continue;

		float rough_box_left = std::numeric_limits<float>::max();
		float rough_box_right = 0;
		float rough_box_bottom = std::numeric_limits<float>::max();
		float rough_box_top = 0;

		for(auto visited_region : group_of_regions) {
			for(auto candidate : state.world.in_province) {
				if(candidate.get_connected_region_id() == visited_region) {
					glm::vec2 mid_point = candidate.get_mid_point();

					if(mid_point.x < rough_box_left) {
						rough_box_left = mid_point.x;
					}
					if(mid_point.x > rough_box_right) {
						rough_box_right = mid_point.x;
					}
					if(mid_point.y < rough_box_bottom) {
						rough_box_bottom = mid_point.y;
					}
					if(mid_point.y > rough_box_top) {
						rough_box_top = mid_point.y;
					}
				}
			}
		}

		if(rough_box_right - rough_box_left > map_data.size_x * 0.9f) {
			continue;
		}


		std::vector<glm::vec2> points;
		std::vector<glm::vec2> bad_points;

		rough_box_bottom = std::max(0.f, rough_box_bottom - step_y);
		rough_box_top = std::min(float(map_data.size_y), rough_box_top + step_y);
		rough_box_left = std::max(0.f, rough_box_left - step_x);
		rough_box_right = std::min(float(map_data.size_x), rough_box_right + step_x);

		float rough_box_width = rough_box_right - rough_box_left;
		float rough_box_height = rough_box_top - rough_box_bottom;

		float rough_box_ratio = rough_box_width / rough_box_height;
		float height_steps = 15.f;
		float width_steps = std::max(10.f, height_steps * rough_box_ratio);

		glm::vec2 local_step = glm::vec2(rough_box_width, rough_box_height) / glm::vec2(width_steps, height_steps);

		float best_y = 0.f;
		//float best_y_length = 0.f;
		float counter_from_the_bottom = 0.f;
		float best_y_length_real = 0.f;
		float best_y_left_x = 0.f;

		// prepare points for a local grid
		for(int j = 0; j < height_steps; j++) {
			float y = rough_box_bottom + j * local_step.y;

			for(int i = 0; i < width_steps; i++) {
				float x = rough_box_left + float(i) * local_step.x;
				glm::vec2 candidate = { x, y };
				auto idx = int32_t(y) * int32_t(map_data.size_x) + int32_t(x);
				if(0 <= idx && size_t(idx) < map_data.province_id_map.size()) {
					auto fat_id = dcon::fatten(state.world, province::from_map_id(map_data.province_id_map[idx]));
					for(auto visited_region : group_of_regions) {
						if(fat_id.get_connected_region_id() == visited_region) {
							points.push_back(candidate);
						}
					}
				}
			}
		}

		float points_above = 0.f;

		for(int j = 0; j < height_steps; j++) {
			float y = rough_box_bottom + j * local_step.y;

			float current_length = 0.f;
			float left_x = (float)(map_data.size_x);

			for(int i = 0; i < width_steps; i++) {
				float x = rough_box_left + float(i) * local_step.x;

				glm::vec2 candidate = { x, y };

				auto idx = int32_t(y) * int32_t(map_data.size_x) + int32_t(x);
				if(0 <= idx && size_t(idx) < map_data.province_id_map.size()) {
					auto fat_id = dcon::fatten(state.world, province::from_map_id(map_data.province_id_map[idx]));
					for(auto visited_region : group_of_regions)
						if(fat_id.get_connected_region_id() == visited_region) {
							points_above++;
							current_length += local_step.x;
							if(x < left_x) {
								left_x = x;
							}
						}
				}
			}

			if(points_above * 2.f > points.size()) {
				//best_y_length = current_length_adjusted;
				best_y_length_real = current_length;
				best_y = y;
				best_y_left_x = left_x;
				break;
			}
		}

		if(points.size() < 2) {
			continue;
		}

		// clustering points into num_of_clusters parts
		size_t min_amount = 2;
		if(state.user_settings.map_label == sys::map_label_mode::cubic) {
			min_amount = 4;
		}
		if(state.user_settings.map_label == sys::map_label_mode::quadratic) {
			min_amount = 3;
		}
		size_t num_of_clusters = std::max(min_amount, (size_t)(points.size() / 40));
		size_t neighbours_requirement = std::clamp(int(std::log(num_of_clusters + 1)), 1, 3);

		if(points.size() < num_of_clusters) {
			num_of_clusters = points.size();
		}

		std::vector<glm::vec2> centroids;

		for(size_t i = 0; i < num_of_clusters; i++) {
			centroids.push_back(points[i]);
		}

		for(int step = 0; step < 100; step++) {
			std::vector<glm::vec2> new_centroids;
			std::vector<int> counters;
			for(size_t i = 0; i < num_of_clusters; i++) {
				new_centroids.push_back(glm::vec2(0, 0));
				counters.push_back(0);
			}


			for(size_t i = 0; i < points.size(); i++) {
				size_t closest = 0;
				float best_dist = std::numeric_limits<float>::max();

				//finding the closest centroid
				for(size_t cluster = 0; cluster < num_of_clusters; cluster++) {
					if(best_dist > glm::distance(centroids[cluster], points[i])) {
						closest = cluster;
						best_dist = glm::distance(centroids[cluster], points[i]);
					}
				}

				new_centroids[closest] += points[i];
				counters[closest] += 1;
			}

			for(size_t i = 0; i < num_of_clusters; i++) {
				new_centroids[i] /= counters[i];
			}

			centroids = new_centroids;
		}

		std::vector<size_t> good_centroids;
		float min_cross = 1;

		std::vector<glm::vec2> final_points;

		for(size_t i = 0; i < num_of_clusters; i++) {
			float locally_good_distance = std::numeric_limits<float>::max();
			for(size_t j = 0; j < num_of_clusters; j++) {
				if(i == j) continue;
				if(locally_good_distance > glm::distance(centroids[i], centroids[j]))
					locally_good_distance = glm::distance(centroids[i], centroids[j]);
			}

			size_t counter_of_neighbors = 0;
			for(size_t j = 0; j < num_of_clusters; j++) {
				if(i == j) {
					continue;
				}
				if(glm::distance(centroids[i], centroids[j]) < locally_good_distance * 1.2f) {
					counter_of_neighbors++;
				}
			}
			if(counter_of_neighbors >= neighbours_requirement) {
				good_centroids.push_back(i);
				final_points.push_back(centroids[i]);
			}
		}


		if(good_centroids.size() <= 1) {
			good_centroids.clear();
			final_points.clear();
			for(size_t i = 0; i < num_of_clusters; i++) {
				good_centroids.push_back(i);
				final_points.push_back(centroids[i]);
			}
		}

		//throwing away bad cluster

		std::vector<glm::vec2> good_points;

		glm::vec2 sum_points = { 0.f, 0.f };

		//OutputDebugStringA("\n\n");

		for(auto point : points) {
			size_t closest = 0;
			float best_dist = std::numeric_limits<float>::max();

			//finding the closest centroid
			for(size_t cluster = 0; cluster < num_of_clusters; cluster++) {
				if(best_dist > glm::distance(centroids[cluster], point)) {
					closest = cluster;
					best_dist = glm::distance(centroids[cluster], point);
				}
			}


			bool is_good = false;
			for(size_t i = 0; i < good_centroids.size(); i++) {
				if(closest == good_centroids[i])
					is_good = true;
			}

			if (is_good) {
				good_points.push_back(point);
				sum_points += point;
			}
		}

		points = good_points;
		

		//initial center:
		glm::vec2 center = sum_points / (float)(points.size());

		//calculate deviation
		float total_sum = 0;

		for(auto point : points) {
			auto dif_v = point - center;
			total_sum += dif_v.x * dif_v.x;
		}

		float mse = total_sum / points.size();
		//ignore points beyond 3 std
		float limit = mse * 3;

		//calculate radius
		//OutputDebugStringA("\n");
		//OutputDebugStringA("\n");
		float right = 0.f;
		float left = 0.f;
		float top = 0.f;
		float bottom = 0.f;
		for(auto point: points) {
			//OutputDebugStringA((std::to_string(point.x) + ", " + std::to_string(point.y) + ", \n").c_str());
			glm::vec2 current = point - center;
			if((current.x > right) && (current.x * current.x < limit)) {
				right = current.x;
			}
			if(current.y > top) {
				top = current.y;
			}
			if((current.x < left) && (current.x * current.x < limit)) {
				left = current.x;
			}
			if(current.y < bottom) {
				bottom = current.y;
			}
		}

		std::vector<glm::vec2> key_points;

		key_points.push_back(center + glm::vec2(left, 0));
		key_points.push_back(center + glm::vec2(0, bottom + local_step.y));
		key_points.push_back(center + glm::vec2(right, 0));
		key_points.push_back(center + glm::vec2(0, top - local_step.y));

		std::array<glm::vec2, 5> key_provs{
			center, //capital
			center, //min x
			center, //min y
			center, //max x
			center //max y
		};

		for(auto key_point : key_points) {
			//if (glm::length(key_point - center) < 100.f * glm::length(eigenvector_1)) 
			update_bbox(key_provs, key_point);
		}


		glm::vec2 map_size{ float(state.map_state.map_data.size_x), float(state.map_state.map_data.size_y) };
		glm::vec2 basis{ key_provs[1].x, key_provs[2].y };
		glm::vec2 ratio{ key_provs[3].x - key_provs[1].x, key_provs[4].y - key_provs[2].y };

		if(ratio.x < 0.001f || ratio.y < 0.001f)
			continue;

		points = final_points;

		//regularisation parameters
		float lambda = 0.00001f;

		float l_0 = 1.f;
		float l_1 = 1.f;
		float l_2 = 1 / 4.f;
		float l_3 = 1 / 8.f;

		// Populate common dataset points
		std::vector<float> out_y;
		std::vector<float> out_x;
		std::vector<float> w;
		std::vector<std::array<float, 4>> in_x;
		std::vector<std::array<float, 4>> in_y;

		for (auto point : points) {
			auto e = point;

			if(e.x < basis.x) {
				continue;
			}
			if(e.x > basis.x + ratio.x) {
				continue;
			}

			w.push_back(1);

			e -= basis;
			e /= ratio;
			out_y.push_back(e.y);
			out_x.push_back(e.x);
			//w.push_back(10 * float(map_data.province_area[province::to_map_id(p2)]));
					
			in_x.push_back(std::array<float, 4>{ l_0 * 1.f, l_1* e.x, l_1* e.x* e.x, l_3* e.x* e.x* e.x});
			in_y.push_back(std::array<float, 4>{ l_0 * 1.f, l_1* e.y, l_1* e.y* e.y, l_3* e.y* e.y* e.y});
		}


		auto prepared_name = text::stored_glyphs(name, f);
		float name_extent = f.text_extent(state, prepared_name, 0, prepared_name.glyph_count, 1);

		bool use_quadratic = false;
		// We will try cubic regression first, if that results in very
		// weird lines, for example, lines that go to the infinite
		// we will "fallback" to using a quadratic instead
		if(state.user_settings.map_label == sys::map_label_mode::cubic) {
			// Columns -> n
			// Rows -> fixed size of 4
			// [ x0^0 x0^1 x0^2 x0^3 ]
			// [ x1^0 x1^1 x1^2 x1^3 ]
			// [ ...  ...  ...  ...  ]
			// [ xn^0 xn^1 xn^2 xn^3 ]
			// [AB]i,j = sum(n, r=1, a_(i,r) * b(r,j))
			// [ x0^0 x0^1 x0^2 x0^3 ] * [ x0^0 x1^0 ... xn^0 ] = [ a0 a1 a2 ... an ]
			// [ x1^0 x1^1 x1^2 x1^3 ] * [ x0^1 x1^1 ... xn^1 ] = [ b0 b1 b2 ... bn ]
			// [ ...  ...  ...  ...  ] * [ x0^2 x1^2 ... xn^2 ] = [ c0 c1 c2 ... cn ]
			// [ xn^0 xn^1 xn^2 xn^3 ] * [ x0^3 x1^3 ... xn^3 ] = [ d0 d1 d2 ... dn ]
			glm::mat4x4 m0(0.f);
			for(glm::length_t i = 0; i < m0.length(); i++)
				for(glm::length_t j = 0; j < m0.length(); j++)
					for(glm::length_t r = 0; r < glm::length_t(in_x.size()); r++)
						m0[i][j] += in_x[r][j] * w[r] * in_x[r][i] / in_x.size();
			for(glm::length_t i = 0; i < m0.length(); i++)
				m0[i][i] += lambda;
			m0 = glm::inverse(m0); // m0 = (T(X)*X/n + I*lambda)^-1
			glm::vec4 m1(0.f); // m1 = T(X)*Y / n
			for(glm::length_t i = 0; i < m1.length(); i++)
				for(glm::length_t r = 0; r < glm::length_t(in_x.size()); r++)
					m1[i] += in_x[r][i] * w[r] * out_y[r] / in_x.size();
			glm::vec4 mo(0.f); // mo = m1 * m0
			for(glm::length_t i = 0; i < mo.length(); i++)
				for(glm::length_t j = 0; j < mo.length(); j++)
					mo[i] += m0[i][j] * m1[j];
			// y = a + bx + cx^2 + dx^3
			// y = mo[0] + mo[1] * x + mo[2] * x * x + mo[3] * x * x * x
			auto poly_fn = [&](float x) {
				return mo[0] * l_0 + mo[1] * x * l_1 + mo[2] * x * x * l_2 + mo[3] * x * x * x * l_3;
			};
			auto dx_fn = [&](float x) {
				return mo[1] * l_1 + 2.f * mo[2] * x * l_2 + 3.f * mo[3] * x * x * l_3;
			};
			auto error_grad = [&](float x, float y) {
				float error_linear = poly_fn(x) - y;				
				return glm::vec4(error_linear * error_linear * error_linear * error_linear * error_linear * mo);
			};

			auto regularisation_grad = [&]() {
				return glm::vec4(0, 0, mo[2] / 4.f, mo[3] / 6.f);
			};

			float xstep = (1.f / float(name_extent * 2.f));
			for(float x = 0.f; x <= 1.f; x += xstep) {
				float y = poly_fn(x);
				if(y < 0.f || y > 1.f) {
					use_quadratic = true;
					break;
				}
				// Steep change in curve => use cuadratic
				float dx = glm::abs(dx_fn(x) - dx_fn(x - xstep));
				if(dx / xstep >= 0.45f) {
					use_quadratic = true;
					break;
				}
			}			

			if(!use_quadratic)
				text_data.emplace_back(std::move(prepared_name), mo, basis, ratio);
		}

		bool use_linear = false;
		if(state.user_settings.map_label == sys::map_label_mode::quadratic || use_quadratic) {
			// Now lets try quadratic
			glm::mat3x3 m0(0.f);
			for(glm::length_t i = 0; i < m0.length(); i++)
				for(glm::length_t j = 0; j < m0.length(); j++)
					for(glm::length_t r = 0; r < glm::length_t(in_x.size()); r++)
						m0[i][j] += in_x[r][j] * w[r] * in_x[r][i] / in_x.size();
			for(glm::length_t i = 0; i < m0.length(); i++)
				m0[i][i] += lambda;
			m0 = glm::inverse(m0); // m0 = (T(X)*X)^-1
			glm::vec3 m1(0.f); // m1 = T(X)*Y
			for(glm::length_t i = 0; i < m1.length(); i++)
				for(glm::length_t r = 0; r < glm::length_t(in_x.size()); r++)
					m1[i] += in_x[r][i] * w[r] * out_y[r] / in_x.size();
			glm::vec3 mo(0.f); // mo = m1 * m0
			for(glm::length_t i = 0; i < mo.length(); i++)
				for(glm::length_t j = 0; j < mo.length(); j++)
					mo[i] += m0[i][j] * m1[j];
			// y = a + bx + cx^2
			// y = mo[0] + mo[1] * x + mo[2] * x * x
			auto poly_fn = [&](float x) {
				return mo[0] * l_0 + mo[1] * x * l_1 + mo[2] * x * x * l_2;
			};
			auto dx_fn = [&](float x) {
				return mo[1] * l_1 + 2.f * mo[2] * x * l_2;
			};
			float xstep = (1.f / float(name_extent * 2.f));
			for(float x = 0.f; x <= 1.f; x += xstep) {
				float y = poly_fn(x);
				if(y < 0.f || y > 1.f) {
					use_linear = true;
					break;
				}
				// Steep change in curve => use cuadratic
				float dx = glm::abs(dx_fn(x) - dx_fn(x - xstep));
				if(dx / xstep >= 0.45f) {
					use_linear = true;
					break;
				}
			}
			if(!use_linear)
				text_data.emplace_back(std::move(prepared_name), glm::vec4(mo, 0.f), basis, ratio);
		}

		if(state.user_settings.map_label == sys::map_label_mode::linear || use_linear) {
			// Now lets try linear
			glm::mat2x2 m0(0.f);
			for(glm::length_t i = 0; i < m0.length(); i++)
				for(glm::length_t j = 0; j < m0.length(); j++)
					for(glm::length_t r = 0; r < glm::length_t(in_x.size()); r++)
						m0[i][j] += in_x[r][j] * w[r] * in_x[r][i];
			for(glm::length_t i = 0; i < m0.length(); i++)
				m0[i][i] += lambda;
			m0 = glm::inverse(m0); // m0 = (T(X)*X)^-1
			glm::vec2 m1(0.f); // m1 = T(X)*Y
			for(glm::length_t i = 0; i < m1.length(); i++)
				for(glm::length_t r = 0; r < glm::length_t(in_x.size()); r++)
					m1[i] += in_x[r][i] * w[r] * out_y[r];
			glm::vec2 mo(0.f); // mo = m1 * m0
			for(glm::length_t i = 0; i < mo.length(); i++)
				for(glm::length_t j = 0; j < mo.length(); j++)
					mo[i] += m0[i][j] * m1[j];

			// y = a + bx
			// y = mo[0] + mo[1] * x
			auto poly_fn = [&](float x) {
				return mo[0] * l_0 + mo[1] * x * l_1;
			};


			// check if this is really better than taking the longest horizontal

			// firstly check if we are already horizontal
			if(abs(mo[1]) > 0.05) {
				// calculate where our line will start and end:
				float left_side = 0.f;
				float right_side = 1.f;

				if(mo[1] > 0.01f) {
					left_side = -mo[0] / mo[1];
					right_side = (1.f - mo[0]) / mo[1];
				} else if(mo[1] < -0.01f) {
					left_side = (1.f - mo[0]) / mo[1];
					right_side = -mo[0] / mo[1];
				}

				left_side = std::clamp(left_side, 0.f, 1.f);
				right_side = std::clamp(right_side, 0.f, 1.f);

				float length_in_box_units = glm::length(ratio * glm::vec2(poly_fn(left_side), poly_fn(right_side)));

				if(best_y_length_real * 1.05f >= length_in_box_units) {
					basis.x = best_y_left_x;
					ratio.x = best_y_length_real;
					mo[0] = (best_y - basis.y) / ratio.y;
					mo[1] = 0;
				}
			}


			if(ratio.x <= map_size.x * 0.75f && ratio.y <= map_size.y * 0.75f)
				text_data.emplace_back(std::move(prepared_name), glm::vec4(mo, 0.f, 0.f), basis, ratio);
		}
	}
	map_data.set_text_lines(state, text_data);

	if(state.cheat_data.province_names) {
		std::vector<text_line_generator_data> p_text_data;
		for(auto p : state.world.in_province) {
			if(p.get_name()) {
				std::string name = text::produce_simple_string(state, p.get_name());
				p_text_data.emplace_back(text::stored_glyphs(name, f), glm::vec4(0.f, 0.f, 0.f, 0.f), p.get_mid_point() - glm::vec2(5.f, 0.f), glm::vec2(10.f, 10.f));
			}
		}
		map_data.set_province_text_lines(state, p_text_data);
	}
}

void map_state::update(sys::state& state) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	// Set the last_update_time if it hasn't been set yet
	if(last_update_time == std::chrono::time_point<std::chrono::steady_clock>{})
		last_update_time = now;

	update_unit_arrows(state, map_data);

	// Update railroads, only if railroads are being built and we have 'em enabled
	if(state.user_settings.railroads_enabled && state.railroad_built.load(std::memory_order::acquire)) {
		state.map_state.map_data.update_railroad_paths(state);
		state.railroad_built.store(false, std::memory_order::release);
	}

	auto microseconds_since_last_update = std::chrono::duration_cast<std::chrono::microseconds>(now - last_update_time);
	float seconds_since_last_update = (float)(microseconds_since_last_update.count() / 1e6);
	last_update_time = now;

	time_counter += seconds_since_last_update;
	time_counter = (float)std::fmod(time_counter, 600.f); // Reset it after every 10 minutes

	if((left_arrow_key_down xor right_arrow_key_down) or (up_arrow_key_down xor down_arrow_key_down)) {
		glm::vec2 arrow_key_velocity_vector{};
		if (left_arrow_key_down) {
			arrow_key_velocity_vector.x -= 1.f;
		}
		if (right_arrow_key_down) {
			arrow_key_velocity_vector.x += 1.f;
		}
		if (up_arrow_key_down) {
			arrow_key_velocity_vector.y -= 1.f;
		}
		if (down_arrow_key_down) {
			arrow_key_velocity_vector.y += 1.f;
		}
		arrow_key_velocity_vector = glm::normalize(arrow_key_velocity_vector);
		arrow_key_velocity_vector *= 0.175f;
		if(shift_key_down)
			arrow_key_velocity_vector *= glm::e<float>();
		pos_velocity += arrow_key_velocity_vector;
	}

	if(state.user_settings.mouse_edge_scrolling) {
		glm::vec2 mouse_pos_percent{ state.mouse_x_position / float(state.x_size), state.mouse_y_position / float(state.y_size) };
		glm::vec2 cursor_velocity_vector{ 0.0f, 0.0f };

		//check if mouse is at edge of screen, in order to move the map
		if(mouse_pos_percent.x < 0.02f) {
			cursor_velocity_vector.x -= 1.f;
		} else if(mouse_pos_percent.x > 0.98f) {
			cursor_velocity_vector.x += 1.f;
		}
		if(mouse_pos_percent.y < 0.02f) {
			cursor_velocity_vector.y -= 1.f;
		} else if(mouse_pos_percent.y > 0.98f) {
			cursor_velocity_vector.y += 1.f;
		}
	
		// check if the vector length is not zero before normalizing
		if(glm::length(cursor_velocity_vector) != 0.0f) {
			cursor_velocity_vector = glm::normalize(cursor_velocity_vector);
			cursor_velocity_vector *= 0.175f;
			pos_velocity += cursor_velocity_vector;
		}
	}

	pos_velocity /= 1.125;

	glm::vec2 velocity = pos_velocity * (seconds_since_last_update / zoom);
	velocity.x *= float(map_data.size_y) / float(map_data.size_x);
	pos += velocity;

	pos.x = glm::mod(pos.x, 1.f);
	pos.y = glm::clamp(pos.y, 0.f, 1.f);

	glm::vec2 mouse_pos{ state.mouse_x_position, state.mouse_y_position };
	glm::vec2 screen_size{ state.x_size, state.y_size };
	glm::vec2 screen_center = screen_size / 2.f;
	auto view_mode = current_view(state);
	glm::vec2 pos_before_zoom;
	bool valid_pos = screen_to_map(mouse_pos, screen_size, view_mode, pos_before_zoom);

	auto zoom_diff = (zoom_change * seconds_since_last_update) / (1 / zoom);
	zoom += zoom_diff;
	zoom_change *= std::exp(-seconds_since_last_update * state.user_settings.zoom_speed);
	zoom = glm::clamp(zoom, min_zoom, max_zoom);

	glm::vec2 pos_after_zoom;
	if(valid_pos && screen_to_map(mouse_pos, screen_size, view_mode, pos_after_zoom)) {
		switch(state.user_settings.zoom_mode) {
		case sys::map_zoom_mode::panning:
			pos += pos_before_zoom - pos_after_zoom;
			break;
		case sys::map_zoom_mode::inverted:
			pos -= pos_before_zoom - pos_after_zoom;
			break;
		case sys::map_zoom_mode::to_cursor:
			if(zoom_change < 0.f) {
				pos -= pos_before_zoom - pos_after_zoom;
			} else {
				pos += pos_before_zoom - pos_after_zoom;
			}
			break;
		case sys::map_zoom_mode::away_from_cursor:
			if(zoom_change < 0.f) {
				pos += pos_before_zoom - pos_after_zoom;
			} else {
				pos -= pos_before_zoom - pos_after_zoom;
			}
			break;
		case sys::map_zoom_mode::centered:
			//no pos change
			break;
		}
	}

	static float keyboard_zoom_change = 0.f;
	if(pgup_key_down) {
		keyboard_zoom_change += 0.1f;
	}
	if(pgdn_key_down) {
		keyboard_zoom_change -= 0.1f;
	}

	glm::vec2 pos_before_keyboard_zoom;
	valid_pos = screen_to_map(screen_center, screen_size, view_mode, pos_before_keyboard_zoom);

	auto keyboard_zoom_diff = (keyboard_zoom_change * seconds_since_last_update) / (1 / zoom);
	zoom += keyboard_zoom_diff;
	keyboard_zoom_change *= std::exp(-seconds_since_last_update * state.user_settings.zoom_speed);
	zoom = glm::clamp(zoom, min_zoom, max_zoom);

	glm::vec2 pos_after_keyboard_zoom;
	if(valid_pos && screen_to_map(screen_center, screen_size, view_mode, pos_after_keyboard_zoom)) {
		pos += pos_before_keyboard_zoom - pos_after_keyboard_zoom;
	}


	globe_rotation = glm::rotate(glm::mat4(1.f), (0.25f - pos.x) * 2 * glm::pi<float>(), glm::vec3(0, 0, 1));
	// Rotation axis
	glm::vec3 axis = glm::vec3(globe_rotation * glm::vec4(1, 0, 0, 0));
	axis.z = 0;
	axis = glm::normalize(axis);
	axis.y *= -1;
	globe_rotation = glm::rotate(globe_rotation, (-pos.y + 0.5f) * glm::pi<float>(), axis);


	if(unhandled_province_selection) {
		map_mode::update_map_mode(state);
		map_data.set_selected_province(state, selected_province);
		unhandled_province_selection = false;
	}
}

void map_state::set_province_color(std::vector<uint32_t> const& prov_color, map_mode::mode new_map_mode) {
	active_map_mode = new_map_mode;
	map_data.set_province_color(prov_color);
}

void map_state::set_terrain_map_mode() {
	active_map_mode = map_mode::mode::terrain;
}

void map_state::on_key_down(sys::virtual_key keycode, sys::key_modifiers mod) {
	switch (keycode) {
	case sys::virtual_key::LEFT:
		left_arrow_key_down = true;
		break;
	case sys::virtual_key::RIGHT:
		right_arrow_key_down = true;
		break;
	case sys::virtual_key::UP:
		up_arrow_key_down = true;
		break;
	case sys::virtual_key::DOWN:
		down_arrow_key_down = true;
		break;
	case sys::virtual_key::PRIOR:
		pgup_key_down = true;
		break;
	case sys::virtual_key::NEXT:
		pgdn_key_down = true;
		break;
	case sys::virtual_key::SHIFT:
		shift_key_down = true;
		break;
	default:
		break;
	}
}

void map_state::on_key_up(sys::virtual_key keycode, sys::key_modifiers mod) {
	switch(keycode) {
	case sys::virtual_key::LEFT:
		left_arrow_key_down = false;
		break;
	case sys::virtual_key::RIGHT:
		right_arrow_key_down = false;
		break;
	case sys::virtual_key::UP:
		up_arrow_key_down = false;
		break;
	case sys::virtual_key::DOWN:
		down_arrow_key_down = false;
		break;
	case sys::virtual_key::PRIOR:
		pgup_key_down = false;
		break;
	case sys::virtual_key::NEXT:
		pgdn_key_down = false;
		break;
	case sys::virtual_key::SHIFT:
		shift_key_down = false;
		break;
	default:
		break;
	}
}

void map_state::set_pos(glm::vec2 new_pos) {
	pos.x = glm::mod(new_pos.x, 1.f);
	pos.y = glm::clamp(new_pos.y, 0.f, 1.0f);
}

void map_state::center_map_on_province(sys::state& state, dcon::province_id p) {
	if(!p)
		return;

	auto map_pos = state.world.province_get_mid_point(p);
	map_pos.x /= float(map_data.size_x);
	map_pos.y /= float(map_data.size_y);
	map_pos.y = 1.0f - map_pos.y;
	set_pos(map_pos);
}

void map_state::on_mouse_wheel(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod,
		float amount) {
	constexpr auto zoom_speed_factor = 15.f;

	zoom_change = (amount / 5.f) * zoom_speed_factor;
}

void map_state::on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	if(is_dragging) { // Drag the map with middlemouse
		glm::vec2 map_pos;
		screen_to_map(mouse_pos, screen_size, map_view::flat, map_pos);

		set_pos(pos + last_camera_drag_pos - glm::vec2(map_pos));
	}
	glm::vec2 mouse_diff = glm::abs(last_unit_box_drag_pos - mouse_pos);
	if((mouse_diff.x > std::ceil(screen_size_x * 0.0025f) || mouse_diff.y > std::ceil(screen_size_y * 0.0025f))
		&& left_mouse_down)
	{
		auto pos1 = last_unit_box_drag_pos / screen_size;
		auto pos2 = mouse_pos / screen_size;
		auto pixel_size = glm::vec2(1) / screen_size;
		map_data.set_drag_box(true, pos1, pos2, pixel_size);
	} else {
		map_data.set_drag_box(false, {}, {}, {});
	}	
}

bool map_state::screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size, map_view view_mode, glm::vec2& map_pos) {
	if(view_mode == map_view::globe) {
		screen_pos -= screen_size * 0.5f;
		screen_pos /= screen_size;
		screen_pos.x *= screen_size.x / screen_size.y;

		float cursor_radius = glm::length(screen_pos);
		glm::vec3 cursor_pos = glm::vec3(screen_pos.x, -10 * zoom, -screen_pos.y);
		glm::vec3 cursor_direction = glm::vec3(0, 1, 0);
		glm::vec3 sphere_center = glm::vec3(0, 0, 0);
		float sphere_radius = zoom / glm::pi<float>();

		glm::vec3 intersection_pos;
		glm::vec3 intersection_normal;

		if(glm::intersectRaySphere(cursor_pos, cursor_direction, sphere_center, sphere_radius, intersection_pos,
			intersection_normal)) {
			intersection_pos = glm::mat3(glm::inverse(globe_rotation)) * intersection_pos;
			float theta = std::acos(std::clamp(intersection_pos.z / glm::length(intersection_pos), -1.f, 1.f));
			float phi = std::atan2(intersection_pos.y, intersection_pos.x);
			float pi = glm::pi<float>();
			map_pos = glm::vec2((phi / (2 * pi)) + 0.5f, theta / pi);
			return true;
		}
		return false;
	} else if (view_mode == map_view::globe_perspect) {
		float aspect_ratio = screen_size.x / screen_size.y;
		float pi = glm::pi<float>();

		//normalize screen
		screen_pos -= screen_size * 0.5f;
		screen_pos /= -screen_size;

		//perspective values
		float near_plane = 0.1f;
		float far_plane = 1.2f;
		float right = near_plane * tan(pi / 6.f) / zoom * aspect_ratio * 2.f;
		float top = near_plane * tan(pi / 6.f) / zoom * 2.f;

		//transform screen plane to near plane
		screen_pos.x *= right;
		screen_pos.y *= top;

		//set up data for glm::intersectRaySphere
		float cursor_radius = glm::length(screen_pos);
		glm::vec3 camera = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 cursor_pos = glm::vec3(screen_pos.x, screen_pos.y, -near_plane);
		glm::vec3 cursor_direction = glm::normalize(cursor_pos);
		glm::vec3 sphere_center = glm::vec3(0.f, 0.f, -1.2f);
		float sphere_radius = 1.f / pi;


		glm::vec3 intersection_pos;
		glm::vec3 intersection_normal;

		if(glm::intersectRaySphere(camera, cursor_direction, sphere_center, sphere_radius, intersection_pos,
			intersection_normal)) {
			intersection_pos -= sphere_center;

			intersection_pos = glm::vec3(-intersection_pos.x, -intersection_pos.z, intersection_pos.y);

			intersection_pos = glm::mat3(glm::inverse(globe_rotation)) * intersection_pos;
			float theta = std::acos(std::clamp(intersection_pos.z / glm::length(intersection_pos), -1.f, 1.f));
			float phi = std::atan2(intersection_pos.y, intersection_pos.x);
			map_pos = glm::vec2((phi / (2.f * pi)) + 0.5f, theta / pi);
			return true;
		}
		return false;
	} else {
		screen_pos -= screen_size * 0.5f;
		screen_pos /= screen_size;
		screen_pos.x *= screen_size.x / screen_size.y;
		screen_pos.x *= float(map_data.size_y) / float(map_data.size_x);

		screen_pos /= zoom;
		screen_pos += pos;
		map_pos = screen_pos;
		return (map_pos.x >= 0 && map_pos.y >= 0 && map_pos.x <= map_data.size_x && map_pos.y <= map_data.size_y);
	}
}

void map_state::on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);

	glm::vec2 map_pos;
	screen_to_map(mouse_pos, screen_size, map_view::flat, map_pos);

	last_camera_drag_pos = map_pos;
	is_dragging = true;
	pos_velocity = glm::vec2(0);
}

void map_state::on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod) {
	is_dragging = false;
}

void map_state::on_lbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y,
		sys::key_modifiers mod) {
	left_mouse_down = true;
	map_data.set_drag_box(false, {}, {}, {});
	last_unit_box_drag_pos = glm::vec2(x, y);
}

void map_state::on_lbutton_up(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y,
		sys::key_modifiers mod) {
	left_mouse_down = false;
	map_data.set_drag_box(false, {}, {}, {});
	auto mouse_pos = glm::vec2(x, y);
	glm::vec2 mouse_diff = glm::abs(last_unit_box_drag_pos - mouse_pos);
	if(mouse_diff.x <= std::ceil(screen_size_x * 0.0025f) && mouse_diff.y <= std::ceil(screen_size_y * 0.0025f)) {
		auto screen_size = glm::vec2(screen_size_x, screen_size_y);
		glm::vec2 map_pos;
		if(!screen_to_map(mouse_pos, screen_size, current_view(state), map_pos)) {
			return;
		}
		map_pos *= glm::vec2(float(map_data.size_x), float(map_data.size_y));
		auto idx = int32_t(map_data.size_y - map_pos.y) * int32_t(map_data.size_x) + int32_t(map_pos.x);
		if(0 <= idx && size_t(idx) < map_data.province_id_map.size()) {
			sound::play_interface_sound(state, sound::get_random_province_select_sound(state),
				state.user_settings.interface_volume * state.user_settings.master_volume);
			auto fat_id = dcon::fatten(state.world, province::from_map_id(map_data.province_id_map[idx]));
			if(map_data.province_id_map[idx] < province::to_map_id(state.province_definitions.first_sea_province)) {
				set_selected_province(province::from_map_id(map_data.province_id_map[idx]));
			} else {
				set_selected_province(dcon::province_id{});
			}
		} else {
			set_selected_province(dcon::province_id{});
		}
	}
}


void map_state::on_rbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y,
		sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	glm::vec2 map_pos;
	if(!screen_to_map(mouse_pos, screen_size, current_view(state), map_pos)) {
		return;
	}
	map_pos *= glm::vec2(float(map_data.size_x), float(map_data.size_y));
	auto idx = int32_t(map_data.size_y - map_pos.y) * int32_t(map_data.size_x) + int32_t(map_pos.x);
	if(0 <= idx && size_t(idx) < map_data.province_id_map.size()) {

	} else {
		set_selected_province(dcon::province_id{});
	}
}

dcon::province_id map_state::get_province_under_mouse(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	glm::vec2 map_pos;
	if(!map_state::screen_to_map(mouse_pos, screen_size, current_view(state), map_pos)) {
		return dcon::province_id{};
	}
	map_pos *= glm::vec2(float(map_data.size_x), float(map_data.size_y));
	auto idx = int32_t(map_data.size_y - map_pos.y) * int32_t(map_data.size_x) + int32_t(map_pos.x);
	if(0 <= idx && size_t(idx) < map_data.province_id_map.size()) {
		auto fat_id = dcon::fatten(state.world, province::from_map_id(map_data.province_id_map[idx]));
		//if(map_data.province_id_map[idx] < province::to_map_id(state.province_definitions.first_sea_province)) {
		return province::from_map_id(map_data.province_id_map[idx]);
		/*} else {
			return dcon::province_id{};
		}*/
	} else {
		return dcon::province_id{};
	}
}

bool map_state::map_to_screen(sys::state& state, glm::vec2 map_pos, glm::vec2 screen_size, glm::vec2& screen_pos) {
	switch(state.user_settings.map_is_globe) {
	case sys::projection_mode::globe_ortho:
		{
			glm::vec3 cartesian_coords;
			float section = 200;
			float pi = glm::pi<float>();
			float angle_x1 = 2 * pi * std::floor(map_pos.x * section) / section;
			float angle_x2 = 2 * pi * std::floor(map_pos.x * section + 1) / section;
			if(!std::isfinite(angle_x1)) {
				assert(false);
				angle_x1 = 0.0f;
			}
			if(!std::isfinite(angle_x2)) {
				assert(false);
				angle_x2 = 0.0f;
			}
			if(!std::isfinite(map_pos.x)) {
				assert(false);
				map_pos.x = 0.0f;
			}
			if(!std::isfinite(map_pos.y)) {
				assert(false);
				map_pos.y = 0.0f;
			}
			cartesian_coords.x = std::lerp(std::cos(angle_x1), std::cos(angle_x2), std::fmod(map_pos.x * section, 1.f));
			cartesian_coords.y = std::lerp(std::sin(angle_x1), std::sin(angle_x2), std::fmod(map_pos.x * section, 1.f));

			float angle_y = (1.f - map_pos.y) * pi;
			cartesian_coords.x *= std::sin(angle_y);
			cartesian_coords.y *= std::sin(angle_y);
			cartesian_coords.z = std::cos(angle_y);
			cartesian_coords = glm::mat3(globe_rotation) * cartesian_coords;
			cartesian_coords /= glm::pi<float>();
			cartesian_coords.x *= -1;
			cartesian_coords.y *= -1;
			if(cartesian_coords.y > 0) {
				return false;
			}
			cartesian_coords += glm::vec3(0.5f);

			screen_pos = glm::vec2(cartesian_coords.x, cartesian_coords.z);
			screen_pos = (2.f * screen_pos - glm::vec2(1.f));
			screen_pos *= zoom;
			screen_pos.x *= screen_size.y / screen_size.x;
			screen_pos = ((screen_pos + glm::vec2(1.f)) * 0.5f);
			screen_pos *= screen_size;
			return true;
		}
	case sys::projection_mode::globe_perpect:
		{
			float aspect_ratio = screen_size.x / screen_size.y;

			glm::vec3 cartesian_coords;
			float section = 200;
			float angle_x1 = 2.f * glm::pi<float>() * std::floor(map_pos.x * section) / section;
			float angle_x2 = 2.f * glm::pi<float>() * std::floor(map_pos.x * section + 1) / section;
			if(!std::isfinite(angle_x1)) {
				assert(false);
				angle_x1 = 0.0f;
			}
			if(!std::isfinite(angle_x2)) {
				assert(false);
				angle_x2 = 0.0f;
			}
			if(!std::isfinite(map_pos.x)) {
				assert(false);
				map_pos.x = 0.0f;
			}
			if(!std::isfinite(map_pos.y)) {
				assert(false);
				map_pos.y = 0.0f;
			}
			cartesian_coords.x = std::lerp(std::cos(angle_x1), std::cos(angle_x2), std::fmod(map_pos.x * section, 1.f));
			cartesian_coords.y = std::lerp(std::sin(angle_x1), std::sin(angle_x2), std::fmod(map_pos.x * section, 1.f));

			float angle_y = (map_pos.y) * glm::pi<float>();
			cartesian_coords.x *= std::sin(angle_y);
			cartesian_coords.y *= std::sin(angle_y);
			cartesian_coords.z = std::cos(angle_y);

			glm::vec3 temp_vector = cartesian_coords;

			// Apply rotation
			cartesian_coords.z *= -1;
			cartesian_coords = glm::mat3(globe_rotation) * cartesian_coords;
			cartesian_coords.z *= -1;

			cartesian_coords /= glm::pi<float>(); // Will make the zoom be the same for the globe and flat map
			cartesian_coords.x *= -1;
			cartesian_coords.z *= -1;

			float temp = cartesian_coords.z;
			cartesian_coords.z = cartesian_coords.y;
			cartesian_coords.y = temp;

			// shift the globe away from camera
			cartesian_coords.z -= 1.2f;
			float near_plane = 0.1f;

			// optimal far plane for culling out invisible part of a planet
			constexpr float tangent_length_square = 1.2f * 1.2f - 1 / glm::pi<float>() / glm::pi<float>();
			float far_plane = tangent_length_square / 1.2f;

			float right = near_plane * tan(glm::pi<float>() / 6.f) / zoom;
			float top = near_plane * tan(glm::pi<float>() / 6.f) / zoom;

			cartesian_coords.x *= near_plane / right;
			cartesian_coords.y *= near_plane / top;

			// depth calculations just for reference
			float w = -cartesian_coords.z;
			cartesian_coords.z = -(far_plane + near_plane) / (far_plane - near_plane) * cartesian_coords.z - 2 * far_plane * near_plane / (far_plane - near_plane);

			if(cartesian_coords.z > far_plane) {
				return false;
			}

			screen_pos = glm::vec2(cartesian_coords.x, cartesian_coords.y) / w;
			//screen_pos = (2.f * screen_pos - glm::vec2(1.f));
			//screen_pos *= zoom;
			screen_pos.x *= screen_size.y / screen_size.x;
			screen_pos = ((screen_pos + glm::vec2(1.f)) * 0.5f);
			screen_pos *= screen_size;
			return true;
		}
	case sys::projection_mode::flat:
		{
			map_pos -= pos;

			if(map_pos.x >= 0.5f)
				map_pos.x -= 1.0f;
			if(map_pos.x < -0.5f)
				map_pos.x += 1.0f;

			map_pos *= zoom;

			map_pos.x *= float(map_data.size_x) / float(map_data.size_y);
			map_pos.x *= screen_size.y / screen_size.x;
			map_pos *= screen_size;
			map_pos += screen_size * 0.5f;
			screen_pos = map_pos;
			if(screen_pos.x >= float(std::numeric_limits<int16_t>::max() / 2))
				return false;
			if(screen_pos.x <= float(std::numeric_limits<int16_t>::min() / 2))
				return false;
			if(screen_pos.y >= float(std::numeric_limits<int16_t>::max() / 2))
				return false;
			if(screen_pos.y <= float(std::numeric_limits<int16_t>::min() / 2))
				return false;
			return true;
		}
	case sys::projection_mode::num_of_modes:
		return false;
	default:
		return false;
	}
}

glm::vec2 map_state::normalize_map_coord(glm::vec2 p) {
	auto new_pos = p / glm::vec2{ float(map_data.size_x), float(map_data.size_y) };
	new_pos.y = 1.f - new_pos.y;
	return new_pos;
}

} // namespace map
