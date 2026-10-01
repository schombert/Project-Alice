#include "flow_map.hpp"
#include "province.hpp"
#include "province_templates.hpp"
#include "economy_stats.hpp"
#include "nations.hpp"

namespace map {
glm::vec2 get_army_location(sys::state const& state, dcon::province_id prov_id);
glm::vec2 put_in_local(glm::vec2 new_point, glm::vec2 base_point, float size_x);
float smootherstep(float x);
}

namespace nations {
float trade_route_control_propagation(
	sys::state const& state,
	ve::vectorizable_buffer<float, dcon::province_id> const& control_buffer,
	ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id> const& coastal_capital_buffer,
	dcon::trade_route_id trid
);
}

namespace flow_map {

void reserve_graph_edge(
	std::vector<ankerl::unordered_dense::map<int32_t, std::vector<float>>>& graph,
	int A, int B, int edge_index, int edges_mult
) {
	if(graph[A].contains(B)) {
		return;
	} else {
		graph[A][B] = { };
		graph[A][B].resize(edges_mult);
	}
}

void perform_negation(flow_map_data& data, int node1, int node2, int edge_index, int edges_mult) {
	reserve_graph_edge(data.flow_graph, node1, node2, edge_index, edges_mult);
	reserve_graph_edge(data.flow_graph, node2, node1, edge_index, edges_mult);
	float forward = data.flow_graph[node1][node2][edge_index];
	float backward = data.flow_graph[node2][node1][edge_index];

	if(forward >= backward) {
		data.flow_graph[node1][node2][edge_index] = forward - backward;
		data.flow_graph[node2][node1][edge_index] = 0.f;
	} else {
		data.flow_graph[node1][node2][edge_index] = 0.f;
		data.flow_graph[node2][node1][edge_index] = backward - forward;
	}
}

void register_trade_flow(flow_map_data& data, int node1, int node2, float volume, int edge_index, int edges_mult) {
	assert(node1 >= 0);
	assert(node2 >= 0);
	assert(volume >= 0.f);
	reserve_graph_edge(data.flow_graph, node1, node2, edge_index, edges_mult);
	data.flow_graph[node1][node2][edge_index] += volume;
	data.node_total_in[node2][edge_index] = data.node_total_in[node2][edge_index] + volume;
	data.node_total_out[node1][edge_index] = data.node_total_out[node1][edge_index] + volume;
}

int node_index(dcon::province_id prov) {
	return prov.index();
}

void add_path(flow_map_data& data, dcon::province_id start, std::vector<dcon::province_id>& path, float volume, int edge_index, int edges_mult) {
	if(path.size() > 0) {
		register_trade_flow(data, node_index(start), node_index(path.back()), volume, edge_index, edges_mult);
	}
	for(int i = int(path.size()) - 1; i >= 0; i--) {
		auto end = path[i];		
		auto start_index = node_index(start);
		auto end_index = node_index(end);
		register_trade_flow(data,  start_index, end_index, volume, edge_index, edges_mult);
		start = end;
	}
}
void add_path_with_negation(flow_map_data& data, dcon::province_id start, std::vector<dcon::province_id>& path, float volume, int edge_index, int edges_mult) {
	if(path.size() > 0) {
		register_trade_flow(data, node_index(start), node_index(path.back()), volume, edge_index, edges_mult);
		perform_negation(data, node_index(start), node_index(path.back()), edge_index, edges_mult);
	}
	for(int i = int(path.size()) - 1; i >= 0; i--) {
		auto end = path[i];
		auto start_index = node_index(start);
		auto end_index = node_index(end);
		register_trade_flow(data, start_index, end_index, volume, edge_index, edges_mult);
		perform_negation(data, start_index, end_index, edge_index, edges_mult);
		start = end;
	}
}

void build_graph_commodity(sys::state const& state, flow_map_data& data, dcon::commodity_id cid) {
	//flow_map_data& data = state.flow_map;

	auto edge = cid.index();
	auto count = state.world.commodity_size();

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
			add_path(data, p_origin, origin_path, absolute_volume, edge, count);

			auto sea_path = province::make_sea_trade_route_path(state, coast_origin, coast_target);
			add_path(data, coast_origin, sea_path, absolute_volume, edge, count);

			auto target_path = province::make_land_trade_path(state, coast_target, p_target);
			add_path(data, coast_target, target_path, absolute_volume, edge, count);
		} else {
			auto path = province::make_land_trade_path(state, p_origin, p_target);
			add_path(data, p_origin, path, absolute_volume, edge, count);
		}
	});
}

bool common_administration(sys::state const& state, dcon::nation_id selected_nation, dcon::nation_id candidate) {
	auto sphere_leader = state.world.nation_get_in_sphere_of(candidate);
	auto overlord = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(candidate));
	auto selected_is_subject = false;
	for(auto subject : state.world.nation_get_overlord_as_ruler(candidate)) {
		if(subject.get_subject() == selected_nation)
			selected_is_subject = true;
	}
	return !selected_nation
		|| candidate == selected_nation
		|| sphere_leader == selected_nation
		|| overlord == selected_nation
		|| selected_is_subject;
}

bool common_administration(sys::state const& state, dcon::nation_id selected_nation, dcon::province_id candidate) {
	return common_administration(state, selected_nation, state.world.province_get_nation_from_province_ownership(candidate));
}

void build_graph_administration(sys::state const& state, flow_map_data& data) {
	auto target_province = state.map_state.selected_province;
	auto target_nation = state.world.province_get_nation_from_province_ownership(target_province);

	auto control_buffer = state.world.province_make_vectorizable_float_buffer();
	state.world.execute_serial_over_province([&](auto ids) {
		control_buffer.set(ids, state.world.province_get_control_scale(ids));
	});
	auto control_buffer_out = state.world.province_make_vectorizable_float_buffer();
	state.world.execute_serial_over_province([&](auto ids) {
		control_buffer_out.set(ids, state.world.province_get_control_scale(ids));
	});

	auto coastal_capital_buffer = ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>(state.world.state_instance_size());
	state.world.execute_parallel_over_state_instance([&](auto ids) {
		ve::apply([&](auto sid) {
			coastal_capital_buffer.set(sid, province::state_get_coastal_capital(state, sid));
		}, ids);
	});

	state.world.for_each_trade_route([&](dcon::trade_route_id trid) {
		auto distance = state.world.trade_route_get_distance_km(trid);
		auto A_market = state.world.trade_route_get_origin(trid);
		auto B_market = state.world.trade_route_get_target(trid);
		auto A_state_instance = state.world.market_get_zone_from_local_market(A_market);
		auto B_state_instance = state.world.market_get_zone_from_local_market(B_market);
		auto capital_A = state.world.state_instance_get_capital(A_state_instance);
		auto capital_B = state.world.state_instance_get_capital(B_state_instance);
		if(!common_administration(state, target_nation, capital_A) || !common_administration(state, target_nation, capital_B)) {
			return;
		}

		auto shift = nations::trade_route_control_propagation(state, control_buffer, coastal_capital_buffer, trid);
		bool is_sea = state.world.trade_route_get_is_sea_route(trid);

		if(is_sea) {
			auto coast_origin = coastal_capital_buffer.get(A_state_instance);
			auto coast_target = coastal_capital_buffer.get(B_state_instance);
			if(shift < 0) {
				auto origin_path = province::make_land_trade_path(state, capital_A, coast_origin);
				add_path(data, capital_A, origin_path, -shift, 0, 1);

				auto sea_path = province::make_sea_trade_route_path(state, coast_origin, coast_target);
				add_path(data, coast_origin, sea_path, -shift, 0, 1);

				auto target_path = province::make_land_trade_path(state, coast_target, capital_B);
				add_path(data, coast_target, target_path, -shift, 0, 1);
			} else {
				auto origin_path = province::make_land_trade_path(state, capital_B, coast_target);
				add_path(data, capital_B, origin_path, shift, 0, 1);

				auto sea_path = province::make_sea_trade_route_path(state, coast_target, coast_origin);
				add_path(data, coast_target, sea_path, shift, 0, 1);

				auto target_path = province::make_land_trade_path(state, coast_origin, capital_A);
				add_path(data, coast_origin, target_path, shift, 0, 1);
			}
		} else {
			if(shift < 0) {
				auto path = province::make_land_trade_path(state, capital_A, capital_B);
				add_path(data, capital_A, path, -shift, 0, 1);
			} else {
				auto path = province::make_land_trade_path(state, capital_B, capital_A);
				add_path(data, capital_B, path, shift, 0, 1);
			}
		}

		control_buffer_out.set(capital_A, control_buffer_out.get(capital_A) + shift);
		control_buffer_out.set(capital_B, control_buffer_out.get(capital_B) - shift);
	});

	state.world.for_each_state_instance([&](auto sid) {
		auto capital = state.world.state_instance_get_capital(sid);
		if(!common_administration(state, target_nation, capital)) {
			return;
		}
		province::for_each_province_in_state_instance(state, sid, [&](auto pid) {
			auto change = (control_buffer.get(capital) - control_buffer.get(pid)) * 0.01f;

			if(change < 0) {
				auto path = province::make_land_trade_path(state, pid, capital);
				add_path(data, pid, path, -change, 0, 1);
			} else {
				auto path = province::make_land_trade_path(state, capital, pid);
				add_path(data, capital, path, change, 0, 1);
			}

			control_buffer_out.set(pid, control_buffer_out.get(pid) + change);
			control_buffer_out.set(capital, control_buffer_out.get(capital) - change);
		});
	});

	state.world.execute_serial_over_province([&](auto ids) {
		control_buffer.set(ids, control_buffer_out.get(ids));
	});

	auto total_adjacency_weight = state.world.province_make_vectorizable_float_buffer();

	state.world.for_each_province_adjacency([&](auto paid) {
		auto A = state.world.province_adjacency_get_connected_provinces(paid, 0);
		auto B = state.world.province_adjacency_get_connected_provinces(paid, 1);
		if(!common_administration(state, target_nation, A) || !common_administration(state, target_nation, B)) {
			return;
		}
		auto mult = nations::control_shift_weight_mult(state, paid);
		auto weight_A = nations::desire_score_province(state, A) * mult;
		auto weight_B = nations::desire_score_province(state, B) * mult;
		auto old_A = total_adjacency_weight.get(A);
		auto old_B = total_adjacency_weight.get(B);
		total_adjacency_weight.set(B, old_B + weight_A);
		total_adjacency_weight.set(A, old_A + weight_B);
	});

	state.world.for_each_province([&](auto pid) {
		if(!common_administration(state, target_nation, pid)) {
			return;
		}
		auto total_weight = total_adjacency_weight.get(pid) + 0.00001f;
		auto control_to_transfer = control_buffer.get(pid) * 0.05f;
		state.world.province_for_each_province_adjacency(pid, [&](auto adj) {
			auto other = state.world.province_adjacency_get_connected_provinces(adj, 0);
			if(other == pid) {
				other = state.world.province_adjacency_get_connected_provinces(adj, 1);
			}
			if(!common_administration(state, target_nation, other)) {
				return;
			}
			auto score = nations::desire_score_province(state, other);
			auto mult = nations::control_shift_weight_mult(state, adj);

			auto shift = control_to_transfer * score * mult / total_weight;
			std::vector<dcon::province_id> path = { other };
			add_path_with_negation(data, pid, path, shift, 0, 1);
		});
	});
}

void convert_balance_to_probabilities(flow_map_data& data, int sea_node_index, int edges_mult) {
	//std::vector<float> total_out {};
	//total_out.resize(edges_mult);
	float total_out_all_edges = 0.f;
	data.edge_layer_probability.resize(edges_mult);

	for(int edge = 0; edge < edges_mult; ++edge) {
		float total_out = 0.f;
		for(size_t i = 0; i < data.node_total_out.size(); ++i) {
			auto local_balance = data.node_total_out[i][edge] - data.node_total_in[i][edge];
			if(local_balance > 0) {
				total_out += local_balance;
			}
		}
		if(total_out == 0.f) {
			for(size_t i = 0; i < data.node_total_out.size(); ++i) {
				data.node_probability_create[i][edge] = 0.f;
			}
		} else {
			for(size_t i = 0; i < data.node_total_out.size(); ++i) {
				auto local_balance = data.node_total_out[i][edge] - data.node_total_in[i][edge];
				data.node_probability_create[i][edge] = local_balance / total_out;
			}
		}

		data.edge_layer_probability[edge] = total_out;
		total_out_all_edges += total_out;

		for(size_t i = 0; i < data.node_total_out.size(); ++i) {
			float total_volume_out = std::max(0.f, data.node_total_in[i][edge] - data.node_total_out[i][edge] - 0.01f);
			if(i >= (size_t)sea_node_index) {
				total_volume_out = 0.f;
			}

			for(auto const& [target_index, volume] : data.flow_graph[i]) {
				total_volume_out += volume[edge];
			}

			if(total_volume_out > 0.f) {
				for(auto const& [target_index, volume] : data.flow_graph[i]) {
					reserve_graph_edge(data.particle_next_node_probability, i, target_index, edge, edges_mult);
					data.particle_next_node_probability[i][target_index][edge] = volume[edge] / total_volume_out;
				}
			} else {
				for(auto const& [target_index, volume] : data.flow_graph[i]) {
					reserve_graph_edge(data.particle_next_node_probability, i, target_index, edge, edges_mult);
					data.particle_next_node_probability[i][target_index][edge] = 0.f;
				}
			}
		}
	}

	if(total_out_all_edges > 0.f) {
		for(int edge = 0; edge < edges_mult; ++edge) {
			data.edge_layer_probability[edge] /= total_out_all_edges;
		}
	}
}

void clear_vertices(sys::state& state) {
	map::display_data& map_data = state.map_state.map_data;
	map_data.trade_flow_vertices.clear();
	map_data.trade_flow_arrow_counts.clear();
	map_data.trade_flow_arrow_starts.clear();
}

void convert_graph_to_vertices(sys::state& state, int edge) {
	bool aggregate = edge == -1;

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
		for(auto& [key, value] : graph[origin.index()]) {
			previous[key] = origin.index();
			auto replace = 0.f;
			if(aggregate) {
				for(auto val : value) {
					replace += val;
				}
			} else {
				replace = value[edge];
			}

			graph_incoming[key][origin.index()] = replace;
			the_most_fat_route = std::max(the_most_fat_route, replace);
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
		if(previous.contains(origin.index())) {
			auto total_outgoing = 0.f;
			auto total_incoming = 0.f;

			for(auto const& [index, volume] : graph[origin.index()]) {
				total_outgoing += volume[edge];
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
						* data.particle_next_node_probability[origin.index()][target_index][edge];

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
					auto volume = target_volume[edge] * left_outgoing / total_outgoing;

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
		if(!visited[origin.index()]) {
			to_visit.clear();
			to_visit.push_back(origin.index());
			current_index_to_visit = 0;
			distance_field[origin.index()] = 0.f;

			while(current_index_to_visit < to_visit.size()) {
				auto current = to_visit[current_index_to_visit];
				auto prev_it = previous.find(current);
				if(prev_it != previous.end() && !visited[prev_it->second]) {
					auto edge_volume = graph[prev_it->second][current][edge];
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
						auto width = std::min(std::sqrt(std::abs(edge_volume[edge])) / data.cutoff, 5.f) * 1000.f;
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
		auto middle = (count / 8) * 4 - 2;

		if(middle <= 0) {
			return;
		}

		auto idx = start;
		auto bound_left = idx;
		auto bound_right = idx + middle;
		if(forward && first_half) {
			idx = start;
			bound_left = start;
			bound_right = start + middle;
		} else if(!forward && first_half) {
			idx = start + middle;
			bound_left = start;
			bound_right = start + middle;
		} else if(forward && !first_half) {
			idx = start + middle;
			bound_left = start + middle;
			bound_right = start + count - 2;
		} else if(!forward && !first_half) {
			idx = start + count - 2;
			bound_left = start + middle;
			bound_right = start + count - 2;
		}
		auto sign = 1;
		auto step = 16;
		if(!forward) {
			step = -16;
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
		auto start_middle = (start_count / 8) * 4 - 2;
		auto start_idx = start_start + start_middle;
		int start_step = step_size * 8;
		if(!forward_start) {
			start_step = -step_size * 8;
		}
		if(start_middle <= 0) {
			return;
		}

		auto end_start = state.map_state.map_data.railroad_starts[hint_end.index()];
		auto end_count = state.map_state.map_data.railroad_counts[hint_end.index()];
		auto end_middle = (end_count / 8) * 4 - 2;
		auto end_idx = end_start;
		int end_step = step_size * 8;
		if(!forward_end) {
			end_idx = end_start + end_count - 2;
			end_step = -step_size * 8;
		}
		if(end_middle <= 0) {
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
		if(previous.contains(origin.index())) {
			auto total_outgoing = 0.f;
			auto total_incoming = 0.f;

			for(auto const& [index, volume] : graph[origin.index()]) {
				total_outgoing += volume[edge];
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
						* data.particle_next_node_probability[origin.index()][target_index][edge];

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

					auto volume_start = target_volume[edge] * left_outgoing / total_outgoing;
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
	data.flow_particles_content.clear();
	for(int i = 0; i < data.amount_of_particles; i++) {
		flow_particle p{
			.position_ = { },
			.target_ = { },
			.graph_node_current = -1,
			.graph_node_prev = -1,
			.graph_node_next = -1
		};
		data.flow_particles_positions.push_back(p);
		data.flow_particles_content.push_back(0);
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
			data.cutoff = 0.005f;
			data.flow_graph.resize(state.world.province_size());
			data.particle_next_node_probability.resize(state.world.province_size());
			data.node_position.resize(state.world.province_size());
			data.node_total_in.resize(state.world.province_size());
			data.node_total_out.resize(state.world.province_size());
			data.node_probability_create.resize(state.world.province_size());
			state.world.for_each_province([&](auto pid) {
				data.node_position[pid.index()] = map::get_army_location(state, pid);
				data.node_total_out[pid.index()].resize(state.world.commodity_size());
				data.node_total_in[pid.index()].resize(state.world.commodity_size());
				data.node_probability_create[pid.index()].resize(state.world.commodity_size());
			});

			state.world.for_each_commodity([&](auto item) {
				build_graph_commodity(state, data, item);
				//convert_graph_to_vertices(state);
			});
			convert_balance_to_probabilities(data, state.province_definitions.first_sea_province.index(), state.world.commodity_size());

			if(state.user_settings.trade_particles_count == 0) {
				data.amount_of_particles = 0;
			} else if(state.user_settings.trade_particles_count == 1) {
				data.amount_of_particles = 1000;
			} else if(state.user_settings.trade_particles_count == 2) {
				data.amount_of_particles = 2000;
			} else if(state.user_settings.trade_particles_count == 3) {
				data.amount_of_particles = 4000;
			} else if(state.user_settings.trade_particles_count == 4) {
				data.amount_of_particles = 8000;
			} else if(state.user_settings.trade_particles_count == 5) {
				data.amount_of_particles = 16000;
			}

			if(state.selected_trade_good) {
				convert_graph_to_vertices(state, state.selected_trade_good.index());
			}

			reset_particles(data);
		} else if(data.source == data_source::administration) {
			data.cutoff = 0.005f;
			data.flow_graph.resize(state.world.province_size());
			data.particle_next_node_probability.resize(state.world.province_size());
			data.node_position.resize(state.world.province_size());
			data.node_total_in.resize(state.world.province_size());
			data.node_total_out.resize(state.world.province_size());
			data.node_probability_create.resize(state.world.province_size());
			state.world.for_each_province([&](auto pid) {
				data.node_position[pid.index()] = map::get_army_location(state, pid);
				data.node_total_out[pid.index()].resize(1);
				data.node_total_in[pid.index()].resize(1);
				data.node_probability_create[pid.index()].resize(1);
			});
			build_graph_administration(state, data);
			convert_balance_to_probabilities(data, state.province_definitions.first_sea_province.index(), 1);
			convert_graph_to_vertices(state, 0);

			data.amount_of_particles = 4000;

			reset_particles(data);
		} else if(data.source == data_source::none) {
			data.node_position.clear();
			data.node_total_in.clear();
			data.node_total_out.clear();
			clear_vertices(state);
		}
		data.update_requested.store(false, std::memory_order_release);
	}
}

}
