#include "flow_map.hpp"
#include "province.hpp"
#include "economy_stats.hpp"

namespace map {
glm::vec2 get_army_location(sys::state& state, dcon::province_id prov_id);
glm::vec2 put_in_local(glm::vec2 new_point, glm::vec2 base_point, float size_x);
float smootherstep(float x);
}

namespace flow_map {

void register_trade_flow(flow_map_data& data, int node1, int node2, float volume) {
	assert(node1 >= 0);
	assert(node2 >= 0);
	if(data.particle_next_node_probability.contains(node1)) {
		if(data.particle_next_node_probability[node1].contains(node2)) {
			data.particle_next_node_probability[node1][node2] += volume;
		} else {
			data.particle_next_node_probability[node1][node2] = volume;
		}
	} else {
		data.particle_next_node_probability[node1] = { };
		data.particle_next_node_probability[node1][node2] = volume;
	}

	if(data.flow_graph.contains(node1)) {
		if(data.flow_graph[node1].contains(node2)) {
			data.flow_graph[node1][node2] += volume;
		} else {
			data.flow_graph[node1][node2] = volume;
		}
	} else {
		data.flow_graph[node1] = std::map<int32_t, float>{ };
		data.flow_graph[node1][node2] = volume;
	}

	data.node_total_in[node2] = data.node_total_in[node2] + volume;
	data.node_total_out[node1] = data.node_total_out[node1] + volume;
}

int node_index(dcon::province_id prov) {
	return prov.index();
}

void add_path(flow_map_data& data, dcon::province_id start, std::vector<dcon::province_id>& path, float volume) {
	if(path.size() > 0) {
		register_trade_flow(data, node_index(start), node_index(path.back()), volume);
	}
	for(int i = int(path.size()) - 1; i >= 0; i--) {
		auto end = path[i];		
		auto start_index = node_index(start);
		auto end_index = node_index(end);
		register_trade_flow(data,  start_index, end_index, volume);
		start = end;
	}
}

void build_graph_commodity(sys::state& state, dcon::commodity_id cid) {
	flow_map_data& data = state.flow_map;
	map::display_data& map_data = state.map_state.map_data;
	std::vector<float> volume_sample;

	/*
		Node positions are just army location for now
	*/

	//data.node_position.clear();
	state.world.for_each_province([&](auto pid){
		data.node_position[pid.index()] = map::get_army_location(state, pid);
	});

	/*
		Prepare data to filter away non-significant trade routes.
	*/
	{
		float total_volume = 0.f;
		state.world.for_each_trade_route([&](dcon::trade_route_id trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
			auto origin = state.world.trade_route_get_origin(trade_route);
			auto target = state.world.trade_route_get_target(trade_route);
			auto sat = state.world.market_get_actual_probability_to_buy(origin, cid);
			auto absolute_volume = std::abs(sat * current_volume);
			total_volume += absolute_volume;
			volume_sample.push_back(absolute_volume);
		});
		if(total_volume == 0.f) {
			data.amount_of_particles = 0;
			return;
		}

		data.amount_of_particles = std::clamp(int(total_volume), 100, 5000);

		std::sort(volume_sample.begin(), volume_sample.end());
	}
	data.cutoff = std::max(0.005f, volume_sample[9 * volume_sample.size() / 10] * 0.5f);

	/*
		Build the graph
	*/
	state.world.for_each_trade_route([&](dcon::trade_route_id trade_route) {
		auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
		auto origin = state.world.trade_route_get_origin(trade_route);
		auto target = state.world.trade_route_get_target(trade_route);
		auto s_origin = state.world.market_get_zone_from_local_market(origin);
		auto s_target = state.world.market_get_zone_from_local_market(target);
		auto p_origin = state.world.state_instance_get_capital(s_origin);
		auto p_target = state.world.state_instance_get_capital(s_target);
		auto sat = state.world.market_get_actual_probability_to_buy(origin, cid);
		auto absolute_volume = sat * current_volume;
		if(absolute_volume < data.cutoff) {
			return;
		}
		bool is_sea = state.world.trade_route_get_is_sea_route(trade_route);
		if(is_sea) {
			auto coast_origin = province::state_get_coastal_capital(state, s_origin);
			auto coast_target = province::state_get_coastal_capital(state, s_target);

			auto origin_path = province::make_land_trade_path(state, p_origin, coast_origin);
			add_path(data, p_origin, origin_path, absolute_volume);

			auto sea_path = province::make_sea_trade_route_path(state, coast_origin, coast_target);
			add_path(data, coast_origin, sea_path, absolute_volume);

			auto target_path = province::make_land_trade_path(state, coast_target, p_target);
			add_path(data, coast_target, target_path, absolute_volume);
		} else {
			auto path = province::make_land_trade_path(state, p_origin, p_target);
			add_path(data, p_origin, path, absolute_volume);
		}
	});
}

void convert_balance_to_probabilities(flow_map_data& data) {
	float total_out = 0.f;
	for(size_t i = 0; i < data.node_total_out.size(); ++i) {
		auto local_balance = data.node_total_out[i] - data.node_total_in[i];
		if(local_balance > 0) {
			total_out += local_balance;
		}
	}
	if(total_out == 0.f) {
		for(size_t i = 0; i < data.node_total_out.size(); ++i) {
			data.node_probability_create[i] = 0.f;
		}
	} else {
		for(size_t i = 0; i < data.node_total_out.size(); ++i) {
			auto local_balance = data.node_total_out[i] - data.node_total_in[i];
			data.node_probability_create[i] = local_balance / total_out;
		}
	}

	for(size_t i = 0; i < data.node_total_out.size(); ++i) {
		if(data.particle_next_node_probability.contains(i)) {
			float total_volume_out = 0.f;
			for(auto const& [target_index, volume] : data.particle_next_node_probability[i]) {
				total_volume_out += volume;
			}

			for(auto const& [target_index, volume] : data.particle_next_node_probability[i]) {
				data.particle_next_node_probability[i][target_index] = volume / total_volume_out;
			}
		}
	}
}

void convert_graph_to_vertices(sys::state& state) {
	flow_map_data& data = state.flow_map;
	map::display_data& map_data = state.map_state.map_data;
	auto& graph = data.flow_graph;

	map_data.trade_flow_vertices.clear();
	map_data.trade_flow_arrow_counts.clear();
	map_data.trade_flow_arrow_starts.clear();

	auto size_x = float(map_data.size_x);
	auto size_y = float(map_data.size_y);

	std::map<int32_t, bool> visited;
	state.world.for_each_province([&](dcon::province_id origin) { visited[origin.index()] = false; });

	float the_most_fat_route = 0.f;

	std::map<int32_t, std::map<int32_t, float>> graph_incoming;
	state.world.for_each_province([&](dcon::province_id origin) {
		graph_incoming[origin.index()] = { };
	});
	// todo: use the most important node as previous by storing "volume" and updating previous only when volume gets larger
	std::map<int32_t, int32_t> previous;
	state.world.for_each_province([&](dcon::province_id origin) {
		if(graph.contains(origin.index())) {
			for(auto& [key, value] : graph[origin.index()]) {
				previous[key] = origin.index();
				graph_incoming[key][origin.index()] = value;
				the_most_fat_route = std::max(the_most_fat_route, value);
			}
		}
	});

	std::map<int32_t, std::map<int32_t, float>> graph_max_incoming;
	state.world.for_each_province([&](dcon::province_id origin) {
		graph_max_incoming[origin.index()] = { };
	});

	auto is_sea = [&](dcon::province_id x) {
		return x.value >= state.province_definitions.first_sea_province.value;
	};

	state.world.for_each_province([&](dcon::province_id origin) {
		if(graph.contains(origin.index()) || previous.contains(origin.index())) {

			auto total_outgoing = 0.f;
			auto total_incoming = 0.f;

			for(auto const& [index, volume] : graph[origin.index()]) {
				total_outgoing += volume;
			}

			for(auto const& [index, volume] : graph_incoming[origin.index()]) {
				total_incoming += volume;
			}

			auto left_outgoing = total_outgoing;
			auto left_incoming = total_incoming;

			for(auto const& [target_index, target_volume] : graph[origin.index()]) {
				for(auto const& [source_index, source_volume] : graph_incoming[origin.index()]) {
					auto target = dcon::province_id{ dcon::province_id::value_base_t(target_index) };
					auto source = dcon::province_id{ dcon::province_id::value_base_t(source_index) };

					if(source == target) {
						continue;
					}

					// if land->[port]->sea - abort
					// if sea->[port]->land - abort
					if(is_sea(source) && !is_sea(origin) && !is_sea(target))continue;
					if(!is_sea(source) && !is_sea(origin) && is_sea(target))continue;

					auto volume_start =
						source_volume;
					auto volume_end =
						volume_start
						* data.particle_next_node_probability[origin.index()][target_index];

					left_incoming -= volume_end;
					left_outgoing -= volume_end;

					auto found = graph_max_incoming[target_index].find(origin.index());
					if(found == graph_max_incoming[target_index].end())
						graph_max_incoming[target_index][origin.index()] = volume_end;
					else if(graph_max_incoming[target_index][origin.index()] < volume_end)
						graph_max_incoming[target_index][origin.index()] = volume_end;
				}
			}

			if(left_outgoing > 0.001f) {
				for(auto const& [target_index, target_volume] : graph[origin.index()]) {
					auto target = dcon::province_id{ dcon::province_id::value_base_t(target_index) };
					auto volume = target_volume * left_outgoing / total_outgoing;

					auto found = graph_max_incoming[target_index].find(origin.index());
					if(found == graph_max_incoming[target_index].end())
						graph_max_incoming[target_index][origin.index()] = volume;
					else if(graph_max_incoming[target_index][origin.index()] < volume)
						graph_max_incoming[target_index][origin.index()] = volume;
				}
			}
		}
	});


	std::map<int32_t, float> distance_field;
	std::vector<int32_t> to_visit;
	size_t current_index_to_visit = 0;
	state.world.for_each_province([&](dcon::province_id origin) {
		if(graph.contains(origin.index()) && !visited[origin.index()]) {
			to_visit.clear();
			to_visit.push_back(origin.index());
			current_index_to_visit = 0;
			distance_field[origin.index()] = 0.f;

			while(current_index_to_visit < to_visit.size()) {
				auto current = to_visit[current_index_to_visit];
				auto prev_it = previous.find(current);
				if(prev_it != previous.end() && !visited[prev_it->second]) {
					auto edge_volume = graph[prev_it->second][current];
					auto width = std::min(std::sqrt(std::abs(edge_volume)) / data.cutoff, 5.f) * 1000.f;
					distance_field[prev_it->second] = distance_field[current] - province::direct_distance(
						state,
						dcon::province_id{ (dcon::province_id::value_base_t)(current) },
						dcon::province_id{ (dcon::province_id::value_base_t)(prev_it->second) }
					) / width;
					to_visit.push_back(prev_it->second);
					visited[prev_it->second] = true;
				}

				for(auto const& [target_index, volume] : graph[origin.index()]) {
					if(!visited[target_index]) {
						auto edge_volume = graph[current][target_index];
						auto width = std::min(std::sqrt(std::abs(edge_volume)) / data.cutoff, 5.f) * 1000.f;
						distance_field[target_index] = distance_field[current] + province::direct_distance(
							state,
							dcon::province_id{ (dcon::province_id::value_base_t)(current) },
							dcon::province_id{ (dcon::province_id::value_base_t)(target_index) }
						) / width;
						to_visit.push_back(target_index);
						visited[target_index] = true;
					}
				}

				current_index_to_visit++;
			}
		};
	});

	// now we are building vertices

	auto volume_to_width = [&](float volume) {
		return std::abs(volume) / std::max(0.05f, the_most_fat_route) * 40000.f;
		};

	std::map<int32_t, bool> vertices_built;
	state.world.for_each_province([&](dcon::province_id origin) { vertices_built[origin.index()] = false; });

	auto build_bezier = [&](
		glm::vec2 start,
		glm::vec2 start_tangent,
		float start_width,
		glm::vec2 end,
		glm::vec2 end_tangent,
		float end_width,
		float& distance
	) {
		auto s = glm::vec2(size_x, size_y);
		auto old_size = map_data.trade_flow_vertices.size();
		map_data.trade_flow_arrow_starts.push_back(GLint(old_size));
		auto start_normal = glm::vec2(-start_tangent.y, start_tangent.x);
		auto norm_pos = start; // glm::vec2(size_x, size_y);
		map_data.trade_flow_vertices.emplace_back(map::textured_line_with_width_vertex{
			norm_pos, start_normal,
			0.f, distance, start_width
		});
		map_data.trade_flow_vertices.emplace_back(map::textured_line_with_width_vertex{
			norm_pos, -start_normal,
			1.f, distance, start_width
		});
		// final step
		add_bezier_to_buffer_variable_width(
			map_data.trade_flow_vertices,
			start * s, map::put_in_local(end * s, start * s, size_x),
			start_tangent, end_tangent,
			1.0f,
			false,
			size_x, size_y,
			40,
			distance,
			start_width, end_width, end_width
		);

		map_data.trade_flow_arrow_counts.push_back(
			GLsizei(map_data.trade_flow_vertices.size() - old_size)
		);
	};

	auto build_bezier_adjacency = [&](
		dcon::province_adjacency_id hint, bool forward,
		bool first_half,
		float start_width,
		float end_width,
		float& distance
	) {
		auto start = state.map_state.map_data.railroad_starts[hint.index()];
		auto count = state.map_state.map_data.railroad_counts[hint.index()];
		auto middle = count / 2;

		if(count == 0) {
			return;
		}

		auto idx = start;

		if(forward && first_half) {
			idx = start;
		} else if(!forward && first_half) {
			idx = start + middle;
		} else if(forward && !first_half) {
			idx = start + middle;
		} else if(!forward && !first_half) {
			idx = start + count - 2;
		}
		auto bound_left = idx;
		auto bound_right = std::min(idx + middle, start + count - 2);
		auto sign = 1;
		auto step = 16;
		if(!forward) {
			step = -16;
			bound_left = idx - middle;
			bound_right = idx;
			sign = -1;
		}
		auto initial_idx = idx;

		while(bound_left <= idx && idx <= bound_right) {
			glm::vec2 current_position{ };
			glm::vec2 current_normal{ };
			glm::vec2 next_position{ };
			glm::vec2 next_normal{ };

			current_position = state.map_state.map_data.railroad_vertices[idx].position_;
			current_normal = state.map_state.map_data.railroad_vertices[idx].normal_direction_;

			next_position = state.map_state.map_data.railroad_vertices[std::clamp(idx + step, bound_left, bound_right)].position_;
			next_normal = state.map_state.map_data.railroad_vertices[std::clamp(idx + step, bound_left, bound_right)].normal_direction_;

			if(!forward) {
				current_normal = -current_normal;
				next_normal = -next_normal;
			}

			auto current_tangent = glm::vec2(current_normal.y, -current_normal.x);
			auto next_tangent = glm::vec2(next_normal.y, -next_normal.x);


			float t = abs(float(idx - initial_idx)) / float(middle);
			auto local_width = start_width;
			if(t < 0.5f) {
				auto a = map::smootherstep(t * 2.f);
				auto b = 1.f - a;
				local_width = b * start_width + a * end_width;
			} else {
				auto a = map::smootherstep((t - 0.5f) * 2.f);
				auto b = 1.f - a;
				local_width = b * end_width + a * end_width;
			}
			if(current_position != next_position) {
				build_bezier(
					current_position, current_tangent, start_width,
					next_position, next_tangent, local_width,
					distance
				);
			}
			idx = idx + step;
			start_width = local_width;
		}
	};

	auto build_bezier_hint_both = [&](
		dcon::province_adjacency_id hint_start, bool forward_start,
		float start_width,
		dcon::province_adjacency_id hint_end, bool forward_end,
		float end_width,
		float& distance,
		int step_size
	) {
		auto start_start = state.map_state.map_data.railroad_starts[hint_start.index()];
		auto start_count = state.map_state.map_data.railroad_counts[hint_start.index()];
		auto start_middle = start_count / 2;
		auto start_idx = start_start + start_middle;
		int start_step = step_size * 8;
		if(!forward_start) {
			start_step = -step_size * 8;
		}
		if(start_count == 0) {
			return;
		}

		auto end_start = state.map_state.map_data.railroad_starts[hint_end.index()];
		auto end_count = state.map_state.map_data.railroad_counts[hint_end.index()];
		auto end_middle = end_count / 2;
		auto end_idx = end_start;
		int end_step = step_size * 8;
		if(!forward_end) {
			end_idx = end_start + end_count - 2;
			end_step = -step_size * 8;
		}
		if(end_count == 0) {
			return;
		}

		while((forward_end && end_idx < end_start + end_middle) || (!forward_end && end_idx > end_start + end_middle)) {
			glm::vec2 current_position { };
			glm::vec2 current_normal{ };
			glm::vec2 next_position{ };
			glm::vec2 next_normal{ };

			if(start_idx < start_start + start_count && start_idx >= start_start) {
				current_position = state.map_state.map_data.railroad_vertices[start_idx].position_;
				current_normal = state.map_state.map_data.railroad_vertices[start_idx].normal_direction_;
				if(!forward_start) {
					current_normal = -current_normal;
				}
			} else {
				current_position = state.map_state.map_data.railroad_vertices[end_idx].position_;
				current_normal = state.map_state.map_data.railroad_vertices[end_idx].normal_direction_;
				if(!forward_end) {
					current_normal = -current_normal;
				}
			}
			if(start_idx + start_step < start_start + start_count && start_idx + start_step >= start_start) {
				next_position = state.map_state.map_data.railroad_vertices[start_idx + start_step].position_;
				next_normal = state.map_state.map_data.railroad_vertices[start_idx + start_step].normal_direction_;
				if(!forward_start) {
					next_normal = -next_normal;
				}
			} else {
				auto clamp_left = end_start;
				auto clamp_right = end_start + end_middle;
				if(!forward_end) {
					clamp_left = end_start + end_middle;
					clamp_right = end_start + end_count - 2;
				}
				next_position = state.map_state.map_data.railroad_vertices[std::clamp(end_idx + end_step, clamp_left, clamp_right)].position_;
				next_normal = state.map_state.map_data.railroad_vertices[std::clamp(end_idx + end_step, clamp_left, clamp_right)].normal_direction_;

				if(!forward_end) {
					next_normal = -next_normal;
				}
			}

			auto current_tangent = glm::vec2(current_normal.y, -current_normal.x);
			auto next_tangent = glm::vec2(next_normal.y, -next_normal.x);

			float t1 = abs(float(start_idx - start_start)) / float(start_count);
			float t2 = abs(float(end_idx - end_start)) / float(end_count);
			float t = t1 + t2;

			auto local_width = start_width;
			if(t < 0.5f) {
				auto a = map::smootherstep(t * 2.f);
				auto b = 1.f - a;
				local_width = b * start_width + a * end_width;
			} else {
				auto a = map::smootherstep((t - 0.5f) * 2.f);
				auto b = 1.f - a;
				local_width = b * end_width + a * end_width;
			}

			if(start_idx + start_step < start_start + start_count && start_idx + start_step >= start_start) {
				start_idx += start_step;
			} else if(start_idx < start_start + start_count && start_idx >= start_start) {
				start_idx += start_step;
				end_idx += end_step;				
			} else {
				end_idx += end_step;
			}
			if(current_position != next_position) {
				build_bezier(
						current_position, current_tangent, start_width,
						next_position, next_tangent, local_width,
						distance
				);
			}
			start_width = local_width;
		}
	};


	state.world.for_each_province([&](dcon::province_id origin) {
		if(graph.contains(origin.index()) || previous.contains(origin.index())) {
			auto total_outgoing = 0.f;
			auto total_incoming = 0.f;

			for(auto const& [index, volume] : graph[origin.index()]) {
				total_outgoing += volume;
			}

			for(auto const& [index, volume] : graph_incoming[origin.index()]) {
				total_incoming += volume;
			}

			auto left_outgoing = total_outgoing;
			auto left_incoming = total_incoming;

			for(auto const& [target_index, target_volume] : graph[origin.index()]) {
				for(auto const& [source_index, source_volume] : graph_incoming[origin.index()]) {
					glm::vec2 current_pos = data.node_position[origin.index()];

					auto volume_start =
						source_volume;
					auto volume_end =
						volume_start
						* data.particle_next_node_probability[origin.index()][target_index];

					auto target = dcon::province_id{ dcon::province_id::value_base_t(target_index) };
					glm::vec2 next_pos = map::put_in_local(data.node_position[target_index], current_pos, size_x);

					auto source = dcon::province_id{ dcon::province_id::value_base_t(source_index) };
					glm::vec2 prev_pos = map::put_in_local(data.node_position[source_index], current_pos, size_x);

					if(source_index == target_index) {
						// model of probabilistic movement is not perfect, so we have to ignore something when we attempt to move in a loop
						left_incoming -= volume_end;
						left_outgoing -= volume_end;
						continue;
					}

					auto adj = state.world.get_province_adjacency_by_province_pair(source, origin);
					auto adj2 = state.world.get_province_adjacency_by_province_pair(origin, target);

					// by default: connect centers of the segments between midpoints of provinces and use their tangents as start and end tangents

					glm::vec2 tangent_start = glm::normalize(current_pos - prev_pos);
					glm::vec2 tangent_end = glm::normalize(next_pos - current_pos);
					glm::vec2 start = (current_pos + prev_pos) / 2.f;
					glm::vec2 end = (current_pos + next_pos) / 2.f;

					left_incoming -= volume_end;
					left_outgoing -= volume_end;

					if(volume_end == 0.f || volume_start == 0.f) {
						continue;
					}

					auto start_width = volume_to_width(graph_max_incoming[origin.index()][source_index]);
					if(start_width < 50.f) {
						continue;
					}
					if(volume_to_width(volume_end) < 50.f) {
						continue;
					}

					// finally
					float distance = distance_field[source_index];
					auto area = state.map_state.map_data.province_area_km2[province::to_map_id(origin)];

					build_bezier_hint_both(
						adj, state.world.province_adjacency_get_connected_provinces(adj, 0) != source, start_width,
						adj2, state.world.province_adjacency_get_connected_provinces(adj2, 0) != origin, volume_to_width(volume_end),
						distance, 1 + (int)(area / 10000.f)
					);
				}
			}

			if(left_incoming > 0.001f) {
				// here we treat the case when some volume in was not matched with volume out
				// in this case we assume that the remaining volume is "consumed" in the middle of the province
				for(auto const& [source_index, source_volume] : graph_incoming[origin.index()]) {
					auto distance = distance_field[source_index];
					auto source = dcon::province_id{ dcon::province_id::value_base_t(source_index) };

					//source -> origin
					auto adj = state.world.get_province_adjacency_by_province_pair(source, origin);

					auto volume_start = source_volume * left_incoming / total_incoming;
					auto volume_end = volume_start;
					auto width_start = volume_to_width(volume_start);
					auto width_end = volume_to_width(volume_end);
					if(width_start < 50.f || width_end < 50.f) {
						continue;
					}

					/*
					Note: railroads are moving into opposite direction to adjacencies.
					We want to move from source to origin.
					So if the origin of adj. is source, we want to move "backward"
					Also we want to get the half  of adj. closer to origin.
					So if we are moving forward, we want to take later part.
					*/
					auto forward = state.world.province_adjacency_get_connected_provinces(adj, 0) != source;

					build_bezier_adjacency(
						adj, forward, !forward, width_start,
						width_end, distance
					);
				}
			}

			if(left_outgoing > 0.001f) {
				for(auto const& [target_index, target_volume] : graph[origin.index()]) {
					auto distance = distance_field[origin.index()];
					auto target = dcon::province_id{ dcon::province_id::value_base_t(target_index) };

					//origin -> target
					auto adj = state.world.get_province_adjacency_by_province_pair(origin, target);

					auto volume_start = target_volume * left_outgoing / total_outgoing;
					auto volume_end = volume_start;
					auto width_start = volume_to_width(volume_start);
					auto width_end = volume_to_width(volume_end);
					if(width_start < 50.f || width_end < 50.f) {
						continue;
					}

					auto forward = state.world.province_adjacency_get_connected_provinces(adj, 0) != origin;

					build_bezier_adjacency(
						adj, forward, forward, width_start,
						width_end, distance
					);
				}
			}
		}
	});

	if(!map_data.trade_flow_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, map_data.vbo_array[map_data.vo_trade_flow]);
		glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(map::textured_line_with_width_vertex)
			* map_data.trade_flow_vertices.size(),
			map_data.trade_flow_vertices.data(),
			GL_STATIC_DRAW
		);
	}
}

void reset_particles(flow_map_data& data) {
	data.flow_particles_positions.clear();
	for(int i = 0; i < data.amount_of_particles; i++) {
		flow_particle p{
			.position_ = { },
			.target_ = { },
			.graph_node_current = -1,
			.graph_node_prev = -1,
			.graph_node_next = -1
		};
		data.flow_particles_positions.push_back(p);
	}
}

void update(sys::state& state) {
	flow_map_data& data = state.flow_map;
	map::display_data& map_data = state.map_state.map_data;

	if(data.update_requested.load(std::memory_order::acquire)) {
		data.flow_graph.clear();
		data.particle_next_node_probability.clear();
		data.node_probability_create.clear();

		if(data.source == data_source::commodity) {
			if(state.selected_trade_good) {
				build_graph_commodity(state, state.selected_trade_good);
				convert_balance_to_probabilities(data);
				convert_graph_to_vertices(state);
				reset_particles(data);
			}
		}

		data.update_requested.store(false, std::memory_order_release);
	}
}

}
