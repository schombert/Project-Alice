#pragma once
#include "system_state.hpp"
#include "province.hpp"

namespace province {

template<typename T>
auto is_overseas(sys::state const& state, T ids) {
	auto owners = state.world.province_get_nation_from_province_ownership(ids);
	auto owner_cap = state.world.nation_get_capital(owners);
	return (state.world.province_get_continent(ids) != state.world.province_get_continent(owner_cap)) &&
				 (state.world.province_get_connected_region_id(ids) != state.world.province_get_connected_region_id(owner_cap));
}

template<typename F>
void for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	for(int32_t i = 0; i < last; ++i) {
		dcon::province_id pid{dcon::province_id::value_base_t(i)};
		func(pid);
	}
}

template<typename F>
void ve_for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	ve::execute_serial<dcon::province_id>(uint32_t(last), func);
}

template<typename F>
void ve_parallel_for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	ve::execute_parallel<dcon::province_id>(uint32_t(last), func);
}

template<typename F>
void for_each_sea_province(sys::state const& state, F const& func) {
	int32_t first = state.province_definitions.first_sea_province.index();
	for(int32_t i = first; i < int32_t(state.world.province_size()); ++i) {
		dcon::province_id pid{dcon::province_id::value_base_t(i)};
		func(pid);
	}
}

template<typename F>
void parallel_for_each_sea_province(sys::state const& state, F const& func) {
	int32_t first = state.province_definitions.first_sea_province.index();
	concurrency::parallel_for(uint32_t(first), uint32_t(state.world.province_size()), [&](uint32_t i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		func(pid);
	});
}

template<typename F>
void for_each_province_in_state_instance(sys::state const& state, dcon::state_instance_id s, F const& func) {
	auto d = state.world.state_instance_get_definition(s);
	auto o = state.world.state_instance_get_nation_from_state_ownership(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			func(p.get_province().id);
		}
	}
}

template<typename F>
void for_each_province_building(const sys::state& state, F const& function) {
	for(auto t = economy::province_building_type::railroad; t != economy::province_building_type::last; t = economy::province_building_type(uint8_t(t) + 1)) {
		function(t);
	}
}

template<typename F>
void for_each_market_province_parallel_over_market(sys::state const& state, F const& func) {
	concurrency::parallel_for((size_t)(0), (size_t)(state.world.market_size()), [&](auto raw_mid) {
		dcon::market_id mid{ (dcon::market_id::value_base_t) (raw_mid) };
		auto sid = state.world.market_get_zone_from_local_market(mid);
		province::for_each_province_in_state_instance(state, sid, [&](auto pid) {
			func(mid, sid, pid);
		});
	});
}

template<typename F>
void for_each_nation_owned_province_parallel_over_nation(sys::state& state, F const& func) {
	concurrency::parallel_for((size_t)(0), (size_t)(state.world.nation_size()), [&](auto raw) {
		dcon::nation_id nid{ (dcon::nation_id::value_base_t) (raw) };
		state.world.nation_for_each_province_ownership(nid, [&](dcon::province_ownership_id poid){
			func(nid, state.world.province_ownership_get_province(poid));
		});
	});
}

template<typename F>
void for_each_nation_controlled_province_parallel_over_nation(sys::state& state, F const& func) {
	concurrency::parallel_for((size_t)(0), (size_t)(state.world.nation_size()), [&](auto raw) {
		dcon::nation_id nid{ (dcon::nation_id::value_base_t) (raw) };
		state.world.nation_for_each_province_control(nid, [&](dcon::province_control_id pcid){
			func(nid, state.world.province_control_get_province(pcid));
		});
	});
}

// Get movement cost. Makes sure it cannot be zero, ever
template<concepts::dcon_id_ve_type<dcon::province_id> ve_id_type>
ve::value_to_vector_type<float> movement_cost(const sys::state& state, ve_id_type prov) {
	return ve::max(state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::movement_cost), 0.01f);
}


struct retreat_province_and_distance {
	float distance_covered = 0.0f;
	dcon::province_id province;

	bool operator<(retreat_province_and_distance const& other) const noexcept {
		if(other.distance_covered != distance_covered)
			return distance_covered > other.distance_covered;
		return other.province.index() > province.index();
	}
};



struct province_and_distance {
	float distance_covered = 0.0f;
	float distance_to_target = 0.0f;
	dcon::province_id province;

	bool operator<(province_and_distance const& other) const noexcept {
		if(other.distance_covered + other.distance_to_target != distance_covered + distance_to_target)
			return distance_covered + distance_to_target > other.distance_covered + other.distance_to_target;
		return other.province.index() > province.index();
	}
};



struct path_node_heuristic {
	float distance_covered = 0.0f;
	float distance_to_target = 0.0f;
	dcon::province_id province = dcon::province_id{ };
	dcon::province_id parent = dcon::province_id{ };
	bool is_in_closed_list = false;
	bool is_in_open_list = false;

	bool operator<(path_node_heuristic const& other) const noexcept {
		if(other.distance_covered != distance_covered)
			return distance_covered + distance_to_target < other.distance_covered + other.distance_to_target;
		return province.index() < other.province.index();
	}
	bool operator>(path_node_heuristic const& other) const noexcept {
		if(other.distance_covered != distance_covered)
			return distance_covered + distance_to_target > other.distance_covered + other.distance_to_target;
		return province.index() > other.province.index();
	}
};






// Creates a path from start province to end province,with given template functions to decide various factors. Returns the path in the passed-in buffer, and can thusly be used with static buffers. Expects the buffer to be empty
// Returns true if a valid path was found, false if not
// HeuristicModifier: Modifier to the heuristic used (direct distance). The higher this value is, the less accurate but more performant the pathfinding will be. If 0.0f, it will always find the optimal path
// StateType: Type of the local state to be used and passed to other lambdas. If it is std::monostate, it will be ignored and not passed to any function
// AdjPred: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, local_state)  and returns a bool. Decides if the passage between the two provinces is possible.
// ToProvPred: Lambda which takes the following as parameter (to_prov, local_state)  and returns a bool. Decides if the given province is passable from any direction
// MovementCostFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance, local_state) and returns a float. The returned value is used as movement cost in pathfinding
// StateInitToProvFunc: Lambda which takes the following as parameters (to_prov, local_state) and returns void. This will be called once per to-province iteration and can be used to initialize the local state to be used in ToProvFunc
// StateInitAdjFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance, local_state) and returns void. This will be called once per adjacency iteration and can be used to initialize the local state to be used in other functions
// StateInitFromProvFunc: Lambda which takes the following as parameters (from_prov, local_state) and returns void. This will be called once per from-province iteration and can be used to initialize the local state relavent to the from-province 
template<float HeuristicModifier, typename StateType, typename AdjPred, typename ToProvPred, typename MovementCostFunc, typename StateInitToProvFunc, typename StateInitAdjFunc, typename StateInitFromProvFunc>
bool make_path_to_prov(const sys::state& state, dcon::province_id start, dcon::province_id end, std::vector<dcon::province_id>& path_result, AdjPred&& adj_predicate, ToProvPred&& to_prov_predicate, MovementCostFunc&& movementcost_func, StateInitToProvFunc&& stateinit_to_prov_func, StateInitAdjFunc&& stateinit_adj_func, StateInitFromProvFunc&& stateinit_from_prov_func) {

	// uses an A* implementation with direct distance as heuristic
	StateType local_state{ };
	if(start == end) // early exit if start is already at destination
		return true;

	// Initialize to-province state on the last province, and run the check function for the end province to see if it is valid in the first place
	if constexpr(!std::is_same_v<StateType, std::monostate>) {
		stateinit_to_prov_func(end, local_state);
	}
	bool prov_check = [&]() {
		if constexpr(std::is_same_v<StateType, std::monostate>) {
			return to_prov_predicate(end);
		} else {
			return to_prov_predicate(end, local_state);
		}
	}();
	if(!prov_check) { //  or if the end province would fail the province check
		return false;
	}


	static thread_local std::vector<dcon::province_id> open_queue; // prioity queue of nodes to be processed, ordered by computed distance. Smallest distance goes first
	open_queue.clear();
	// contains nodes for all provinces, including if they are in the open queue, are closed and its parent for constructing path later. It is allocated as a static thread local buffer, as it is possible both the UI and update thread will calculate paths
	static thread_local tagged_vector<path_node_heuristic, dcon::province_id> path_node_container;

	path_node_container.resize(state.world.province_size());
	std::memset(path_node_container.data(), 0, sizeof(path_node_heuristic) * path_node_container.size()); // memset incase it was used earlier


	auto province_comparer = [&](dcon::province_id a, dcon::province_id b) {
		return path_node_container[a] > path_node_container[b];
		};

	auto fill_path_result = [&](dcon::province_id i) {
		while(i && i != start) {
			path_result.push_back(i);
			i = path_node_container[i].parent;
		}
		};
	auto assert_path_result = [](std::vector<dcon::province_id>& v) {
		for(auto const e : v)
			assert(bool(e));
		};
	auto& start_node = path_node_container[start];
	start_node.distance_covered = 0.0f;
	start_node.distance_to_target = 0.0f;
	start_node.is_in_closed_list = false;
	start_node.is_in_open_list = true;
	start_node.parent = dcon::province_id{ };
	start_node.province = start;

	open_queue.push_back(start);
	while(!open_queue.empty()) {
		std::pop_heap(open_queue.begin(), open_queue.end(), province_comparer);
		auto current_prov = open_queue.back();
		open_queue.pop_back();
		auto& current_node = path_node_container[current_prov];
		// Its a duplicate we have already been through, skip
		if(current_node.is_in_closed_list) {
			continue;
		}
		// check if we have reached the end
		if(current_prov == end) {
			fill_path_result(current_prov);
			assert_path_result(path_result);
			return true;
		}
		// add current to closed list immediately
		current_node.is_in_open_list = false;
		current_node.is_in_closed_list = true;

		// Init the local state for the from-province only once a new from-province is chosen
		if constexpr(!std::is_same_v<StateType, std::monostate>) {
			stateinit_from_prov_func(current_prov, local_state);
		} 

		for(auto adj : state.world.province_get_province_adjacency(current_prov)) {
			auto other_prov =
				adj.get_connected_provinces(0) == current_prov ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto distance = adj.get_distance();


			auto& neighbor_node = path_node_container[other_prov];

			// check if not present in the closed list
			if(!neighbor_node.is_in_closed_list) {
				if constexpr(std::is_same_v<StateType, std::monostate>) {
					prov_check = to_prov_predicate(other_prov);
				}
				else {
					stateinit_to_prov_func(other_prov, local_state);
					prov_check = to_prov_predicate(other_prov, local_state);
				}
				// check if province check passes (aka province isnt impassable from all directions). If not, add to closed list as this will stay impassable
				if(prov_check) {
					bool adj_check;
					if constexpr(std::is_same_v<StateType, std::monostate>) {
						adj_check = adj_predicate(other_prov, current_prov, adj);
					}
					else {
						stateinit_adj_func(other_prov, current_prov, adj, distance, local_state);
						adj_check = adj_predicate(other_prov, current_prov, adj, local_state);
					}
					// and passes the adjacency check(aka the specific adjacency is passable).It is not added to the closed list if the adj check fails as it may be passable from a diffrent adjacency
					if(adj_check) {
						float actual_dist = [&]() {
							// to and from province
							if constexpr(std::is_same_v<StateType, std::monostate>) {
								return movementcost_func(other_prov, current_prov, adj, distance);
							}
							else {
								return movementcost_func(other_prov, current_prov, adj, distance, local_state); 
							}
						}();

						float distance_to_neighbor = current_node.distance_covered + actual_dist; // computes net distance to neighbor from the start province
						// if not present in the open queue add it to the open queue for processing later
						if(!neighbor_node.is_in_open_list) {
							// set distance and parent, then add it to the open queue
							neighbor_node.distance_covered = distance_to_neighbor;
							neighbor_node.distance_to_target = direct_distance(state, other_prov, end) * HeuristicModifier;
							neighbor_node.parent = current_prov;
							neighbor_node.province = other_prov; // set province now, before it is added to the open queue
							open_queue.push_back(other_prov);
							std::push_heap(open_queue.begin(), open_queue.end(), province_comparer);
							neighbor_node.is_in_open_list = true;
						} else if(distance_to_neighbor < neighbor_node.distance_covered) {
							// this is a better path; update distance covered and parent
							neighbor_node.distance_covered = distance_to_neighbor;
							neighbor_node.parent = current_prov;
							// Add it to the heap again to update it with the new distance information. The old one is kept, but ignored when it is popped
							open_queue.push_back(other_prov);
							std::push_heap(open_queue.begin(), open_queue.end(), province_comparer);
						}


					} else {
						// Nothing, it can be tried again if it fails adjacency check
					}
				}
				else {
					neighbor_node.is_in_closed_list = true;// exclude it from being checked again
				}
			}
		}
	}
	assert_path_result(path_result);
	return false;

}





// Creates a path from start province to end province,with given template functions to decide various factors. Returns the path in the passed-in buffer, and can thusly be used with static buffers. Expects the buffer to be empty
// 
// HeuristicModifier: Modifier to the heuristic used (direct distance). The higher this value is, the less accurate but more performant the pathfinding will be. If 0.0f, it will always find the optimal path
// AdjPred: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, local_state)  and returns a bool. Decides if the passage between the two provinces is possible.
// ToProvPred: Lambda which takes the following as parameter (to_prov, local_state)  and returns a bool. Decides if the given province is passable from any direction
// MovementCostFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance, local_state) and returns a float. The returned value is used as movement cost in pathfinding
template<float HeuristicModifier, typename AdjPred, typename ToProvPred, typename MovementCostFunc>
bool make_path_to_prov(const sys::state& state, dcon::province_id start, dcon::province_id end, std::vector<dcon::province_id>& path_result, AdjPred&& adj_predicate, ToProvPred&& to_prov_predicate, MovementCostFunc&& movementcost_func) {
	return make_path_to_prov<HeuristicModifier, std::monostate>(state, start, end, path_result, adj_predicate, to_prov_predicate, movementcost_func, []() { }, []() { }, []() { });
}


// Creates a path from start province to end province,with given template functions to decide various factors
// 
// HeuristicModifier: Modifier to the heuristic used (direct distance). The higher this value is, the less accurate but more performant the pathfinding will be. If 0.0f, it will always find the optimal path
// StateType: Type of the local state to be used and passed to other lambdas. If it is std::monostate, it will be ignored and not passed to any function
// AdjPred: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, local_state)  and returns a bool. Decides if the passage between the two provinces is possible.
// ToProvPred: Lambda which takes the following as parameter (to_prov, local_state)  and returns a bool. Decides if the given province is passable from any direction
// MovementCostFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance, local_state) and returns a float. The returned value is used as movement cost in pathfinding
// StateInitToProvFunc: Lambda which takes the following as parameters (to_prov, local_state) and returns void. This will be called once per to-province iteration and can be used to initialize the local state to be used in ToProvFunc
// StateInitAdjFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance, local_state) and returns void. This will be called once per adjacency iteration and can be used to initialize the local state to be used in other functions
// StateInitFromProvFunc: Lambda which takes the following as parameters (from_prov, local_state) and returns void. This will be called once per from-province iteration and can be used to initialize the local state relavent to the from-province 
template<float HeuristicModifier, typename StateType, typename AdjPred, typename ToProvPred, typename MovementCostFunc, typename StateInitProvFunc, typename StateInitAdjFunc, typename StateInitFromProvFunc>
std::vector<dcon::province_id> make_path_to_prov(const sys::state& state, dcon::province_id start, dcon::province_id end, AdjPred&& adj_predicate, ToProvPred&& to_prov_predicate, MovementCostFunc&& movementcost_func, StateInitProvFunc&& stateinit_prov_func, StateInitAdjFunc&& stateinit_adj_func, StateInitFromProvFunc&& stateinit_from_prov_func) {

	std::vector<dcon::province_id> path_result;
	make_path_to_prov<HeuristicModifier, StateType>(state, start, end, path_result, adj_predicate, to_prov_predicate, movementcost_func, stateinit_prov_func, stateinit_adj_func, stateinit_from_prov_func);
	return path_result;

}

// Creates a path from start province to end province,with given template functions to decide various factors
// 
// HeuristicModifier: Modifier to the heuristic used (direct distance). The higher this value is, the less accurate but more performant the pathfinding will be. If 0.0f, it will always find the optimal path
// AdjPred: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, local_state)  and returns a bool. Decides if the passage between the two provinces is possible.
// ToProvPred: Lambda which takes the following as parameter (to_prov, local_state)  and returns a bool. Decides if the given province is passable from any direction
// MovementCostFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance, local_state) and returns a float. The returned value is used as movement cost in pathfinding
template<float HeuristicModifier, typename AdjPred, typename ToProvPred, typename MovementCostFunc>
std::vector<dcon::province_id> make_path_to_prov(const sys::state& state, dcon::province_id start, dcon::province_id end, AdjPred&& adj_predicate, ToProvPred&& to_prov_predicate, MovementCostFunc&& movementcost_func) {
	return make_path_to_prov<HeuristicModifier, std::monostate>(state, start, end, adj_predicate, to_prov_predicate, movementcost_func, []() { }, []() { }, []() { });
}





// Creates a path from start province to end province,with given template functions to decide various factors. This one is faster, but at the cost of precision when dealing with distance modifiers
// AdjFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency)  and returns a bool. Decides if the passage between the two provinces is possible.
// ProvFunc: Lambda which takes a province_id as parameter and returns a bool. Decides if the given province is passable from any direction
// MovementCostFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance) and returns a float. The returned value is used as movement cost in pathfinding
template<typename AdjFunc, typename ProvFunc, typename MovementCostFunc>
std::vector<dcon::province_id> make_path_to_prov_fast(sys::state& state, dcon::province_id start, dcon::province_id end, AdjFunc&& adj_func, ProvFunc&& prov_func, MovementCostFunc&& movementcost_func) {

	std::vector<province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	std::vector<dcon::province_id> path_result;

	if(start == end)
		return path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		//path_result.push_back(end);
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};
	auto assert_path_result = [](std::vector<dcon::province_id>& v) {
		for(auto const e : v)
			assert(bool(e));
		};

	path_heap.push_back(province_and_distance{ 0.0f, direct_distance(state, start, end), start });
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		if(nearest.province == end) {
			fill_path_result(nearest.province);
			assert_path_result(path_result);
			return path_result;
		}

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
				adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if(adj_func(other_prov, nearest.province, adj) && !origins_vector.get(other_prov)) {

				if(prov_func(other_prov)) {
					/*if(other_prov == end) {
						fill_path_result(nearest.province);
						assert_path_result(path_result);
						return path_result;
					}*/
					float new_dist = movementcost_func(other_prov, nearest.province, adj, distance); // to and from province
					path_heap.push_back(
								province_and_distance{ nearest.distance_covered + new_dist, direct_distance(state, other_prov, end), other_prov });
					std::push_heap(path_heap.begin(), path_heap.end());
					origins_vector.set(other_prov, nearest.province);
				} else {
					origins_vector.set(other_prov, dcon::province_id{ 0 }); // exclude it from being checked again
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;

}




struct path_node {
	float distance_covered = 0.0f;
	dcon::province_id province = dcon::province_id{ };
	dcon::province_id parent = dcon::province_id{ };
	bool is_in_closed_list = false;
	bool is_in_open_list = false;

	bool operator<(path_node const& other) const noexcept {
		if(other.distance_covered != distance_covered)
			return distance_covered < other.distance_covered;
		return province.index() < other.province.index();
	}
	bool operator>(path_node const& other) const noexcept {
		if(other.distance_covered != distance_covered)
			return distance_covered > other.distance_covered;
		return province.index() > other.province.index();
	}
};


// Creates a path from start province to end province,with given template functions to decide various factors
// AdjFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency)  and returns a bool. Decides if the passage between the two provinces is possible.
// ProvFunc: Lambda which takes a province_id as parameter and returns a bool. Decides if the given province is passable from any direction
// MovementCostFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance) and returns a float. The returned value is used as movement cost in pathfinding
// EndFunc: Lambda which takes a province_id and returns a bool. Decides if the given province is the end goal
template<typename AdjFunc, typename ProvFunc, typename MovementCostFunc, typename EndFunc>
std::vector<dcon::province_id> make_path_to_expression(const sys::state& state, dcon::province_id start, AdjFunc&& adj_func, ProvFunc&& prov_func, MovementCostFunc&& movementcost_func, EndFunc&& end_expression) {

	// uses an Dijkstra's algorithm implementation to find the best possible path to the end expression

	static thread_local std::vector<dcon::province_id> open_queue; // prioity queue of nodes to be processed, ordered by computed distance. Smallest distance goes first
	open_queue.clear();
	// contains nodes for all provinces, including if they are in the open queue, are closed and its parent for constructing path later. It is allocated as a static thread local buffer, as it is possible both the UI and update thread will calculate paths
	static thread_local tagged_vector<path_node, dcon::province_id> path_node_container;

	path_node_container.resize(state.world.province_size());
	std::memset(path_node_container.data(), 0, sizeof(path_node) * path_node_container.size()); // memset incase it was used earlier

	std::vector<dcon::province_id> path_result;

	if(end_expression(start)) {
		return path_result;
	}

	auto province_comparer = [&](dcon::province_id a, dcon::province_id b) {
		return path_node_container[a] > path_node_container[b];
	};

	auto fill_path_result = [&](dcon::province_id i) {
		while(i && i != start) {
			path_result.push_back(i);
			i = path_node_container[i].parent;
		}
	};
	auto assert_path_result = [](std::vector<dcon::province_id>& v) {
		for(auto const e : v)
			assert(bool(e));
	};
	auto& start_node = path_node_container[start];
	start_node.distance_covered = 0.0f;
	start_node.is_in_closed_list = false;
	start_node.is_in_open_list = true;
	start_node.parent = dcon::province_id{ };
	start_node.province = start;

	open_queue.push_back(start);
	while(open_queue.size() > 0) {
		std::pop_heap(open_queue.begin(), open_queue.end(), province_comparer);
		auto current_prov = open_queue.back();
		open_queue.pop_back();
		auto& current_node = path_node_container[current_prov];
		// Its a duplicate we have already been through, skip
		if(current_node.is_in_closed_list) {
			continue;
		}
		// check if we have reached the end
		if(end_expression(current_prov)) {
			fill_path_result(current_prov);
			assert_path_result(path_result);
			return path_result;
		}
		// add current to closed list immediately
		current_node.is_in_open_list = false;
		current_node.is_in_closed_list = true;

		for(auto adj : state.world.province_get_province_adjacency(current_prov)) {
			auto other_prov =
				adj.get_connected_provinces(0) == current_prov ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			auto& neighbor_node = path_node_container[other_prov];

			// check if not present in the closed list, and passes the adjacency check (aka the specific adjacency is passable). It is not added to the closed list if the adj check fails as it may be passable from a diffrent adjacency
			if(!neighbor_node.is_in_closed_list && adj_func(other_prov, current_prov, adj)) {
				// check if province check passes (aka province isnt impassable from all directions). If not, add to closed list as this will stay impassable
				if(prov_func(other_prov)) {
					float new_dist = movementcost_func(other_prov, current_prov, adj, distance); // to and from province
					float distance_to_neighbor = current_node.distance_covered + new_dist; // computes net distance to neighbor from the start province
					// if not present in the open queue add it to the open queue for processing later

					if(!neighbor_node.is_in_open_list) {
						// set distance and parent, then add it to the open queue
						neighbor_node.distance_covered = distance_to_neighbor;
						neighbor_node.parent = current_prov;
						neighbor_node.province = other_prov; // set province now, before it is added to the open queue
						open_queue.push_back(other_prov);
						std::push_heap(open_queue.begin(), open_queue.end(), province_comparer);
						neighbor_node.is_in_open_list = true;
					}
					else if(distance_to_neighbor < neighbor_node.distance_covered) {
						// this is a better path; update distance covered and parent
						neighbor_node.distance_covered = distance_to_neighbor;
						neighbor_node.parent = current_prov;
						// Add it to the heap again to update it with the new distance information. The old one is kept, but ignored when it is popped
						open_queue.push_back(other_prov);
						std::push_heap(open_queue.begin(), open_queue.end(), province_comparer);
					}
					

				} else {
					neighbor_node.is_in_closed_list = true;// exclude it from being checked again
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;

}



// Creates a path from start province to end province,with given template functions to decide various factors. This one is faster, but at the cost of precision when dealing with modifiers to distance
// AdjFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency)  and returns a bool. Decides if the passage between the two provinces is possible.
// ProvFunc: Lambda which takes a province_id as parameter and returns a bool. Decides if the given province is passable from any direction
// MovementCostFunc: Lambda which takes the following as parameters (to_prov, from_prov, adjacency, distance) and returns a float. The returned value is used as movement cost in pathfinding
// EndFunc: Lambda which takes a province_id and returns a bool. Decides if the given province is the end goal
template<typename AdjFunc, typename ProvFunc, typename MovementCostFunc, typename EndFunc>
std::vector<dcon::province_id> make_path_to_expression_fast(sys::state& state, dcon::province_id start, AdjFunc&& adj_func, ProvFunc&& prov_func, MovementCostFunc&& movementcost_func, EndFunc&& end_expression) {

	std::vector<retreat_province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());
	origins_vector.set(start, dcon::province_id{ 0 });

	std::vector<dcon::province_id> path_result;

	if(end_expression(start)) {
		return path_result;
	}

	auto fill_path_result = [&](dcon::province_id i) {
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
		};
	auto assert_path_result = [](std::vector<dcon::province_id>& v) {
		for(auto const e : v)
			assert(bool(e));
		};

	path_heap.push_back(retreat_province_and_distance{ 0.0f, start });
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		if(end_expression(nearest.province)) {
			fill_path_result(nearest.province);
			assert_path_result(path_result);
			return path_result;
		}

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
				adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if(adj_func(other_prov, nearest.province, adj) && !origins_vector.get(other_prov)) {

				if(prov_func(other_prov)) {
					float new_dist = movementcost_func(other_prov, nearest.province, adj, distance); // to and from province
					path_heap.push_back(
								retreat_province_and_distance{ nearest.distance_covered + new_dist, other_prov });
					std::push_heap(path_heap.begin(), path_heap.end());
					origins_vector.set(other_prov, nearest.province);
				} else {
					origins_vector.set(other_prov, dcon::province_id{ 0 }); // exclude it from being checked again
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;

}








} // namespace province
