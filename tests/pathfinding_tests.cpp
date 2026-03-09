#include "catch.hpp"
#include "province_templates.hpp"

static std::unique_ptr<sys::state> gamestate = nullptr;

template<typename AdjFunc, typename ProvFunc, typename ModifierFunc>
struct make_path_to_prov_params {
	dcon::province_id start;
	dcon::province_id end;
	AdjFunc adj_func;
	ProvFunc prov_func;
	ModifierFunc mod_func;
	std::vector<dcon::province_id> expected_path;

	void assert_test_result() {
		auto actual_path = province::make_path_to_prov<0.0f>(*gamestate, start, end, adj_func, prov_func, mod_func); // test with 0 heuristic, as otherwise we cannot know for sure if it will find the optimal path
		REQUIRE(expected_path == actual_path);
	}
	make_path_to_prov_params(dcon::province_id _start, dcon::province_id _end, AdjFunc _adj_func, ProvFunc _prov_func, ModifierFunc _mod_func, std::vector<dcon::province_id> _expected_path) : start(_start), end(_end), adj_func(_adj_func), prov_func(_prov_func), mod_func(_mod_func), expected_path(_expected_path)
	{

	}
};



template<typename AdjFunc, typename ProvFunc, typename ModifierFunc, typename EndFunc>
struct make_path_to_expression_params {
	dcon::province_id start;
	AdjFunc adj_func;
	ProvFunc prov_func;
	ModifierFunc mod_func;
	EndFunc end_expression;
	std::vector<dcon::province_id> expected_path;

	void assert_test_result() {
		auto actual_path = province::make_path_to_expression(*gamestate, start, adj_func, prov_func, mod_func, end_expression);
		REQUIRE(expected_path == actual_path);
	}
	make_path_to_expression_params(dcon::province_id _start, AdjFunc _adj_func, ProvFunc _prov_func, ModifierFunc _mod_func, EndFunc _end_expression, std::vector<dcon::province_id> _expected_path) : start(_start), adj_func(_adj_func), prov_func(_prov_func), mod_func(_mod_func), end_expression(_end_expression), expected_path(_expected_path) {

	}
};



// these tests expect a vanilla scenario
TEST_CASE("make_path_to_prov_valid_tests", "[pathfinding]") {


	gamestate = load_testing_scenario_file_with_save(sys::network_mode_type::host);


	auto params = std::make_tuple(
		make_path_to_prov_params{ // expect a empty path if start == end on land prov
			dcon::province_id{ 0 }, // start
			dcon::province_id{ 0 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{  }, // expected path
		},
		make_path_to_prov_params{ // expect a empty path if start == end on sea prov
			dcon::province_id{ 3000 }, // start
			dcon::province_id{ 3000 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{  } // expected path
		},
		make_path_to_prov_params{ // expect a empty path if two land provinces are not reachable from eachother ( Tries to go from Europe to USA) while sea is not allowed to be crossed
			dcon::province_id{ 20 }, // start
			dcon::province_id{ 300 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{  } // expected path
		},
		make_path_to_prov_params{ // expect a empty path if two sea provinces are not reachable from eachother ( Tries to go from red sea to the Yang Tse Delta) while land is not allowed to be crossed
			dcon::province_id{ 2779 }, // start
			dcon::province_id{ 2826 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return false; } else {return true; } }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{  } // expected path
		},
		make_path_to_prov_params{ // expect a 1-tile path to the end province when pathfinding from London to Chelmsford
			dcon::province_id{ 299 }, // start
			dcon::province_id{ 292 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 292 } }// expected path
		},
		make_path_to_prov_params{ // expect a 3-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression. It is fastest to go Nitra -> Gyor -> Szejesfegervar -> Budapest
			dcon::province_id{ 634}, // start
			dcon::province_id{ 640 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 642 },  dcon::province_id{ 641 }}// expected path
		},



		make_path_to_prov_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest when Gyor is impassabble. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			dcon::province_id{ 640 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return to_prov != dcon::province_id{ 641 }; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 }}// expected path
		},

		make_path_to_prov_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest when river adjacencies are impassable. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			dcon::province_id{ 640 }, //end
			[](auto adj) { return (gamestate->world.province_adjacency_get_type(adj) & province::border::river_crossing_bit) == 0; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 }}// expected path
		},

		make_path_to_prov_params{ // expect a 6-tile fastest path to the end province when pathfinding from Rostov to Kiev. It is fastest to go Yuzovka-> Kramatorsk -> Ekaterinoslav -> Krivoyrog -> Cherkassy -> Kiev
			dcon::province_id{ 978}, // start
			dcon::province_id{ 957 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 957 }, dcon::province_id{ 960 },dcon::province_id{ 970 }, dcon::province_id{ 971 }, dcon::province_id{ 973 },  dcon::province_id{ 974 }}// expected path
		}

	);

	std::apply([](auto&&... args) {(( args.assert_test_result() ), ...); }, params);
	
}




TEST_CASE("make_path_to_expression_valid_tests", "[pathfinding]") {
	dcon::province_id nitra = dcon::province_id{ 634 };
	dcon::province_id miskolc = dcon::province_id{ 645 };
	dcon::province_id budapest = dcon::province_id{ 640 };
	dcon::province_id gyor = dcon::province_id{ 641 };

	gamestate = load_testing_scenario_file_with_save(sys::network_mode_type::host);

	auto params = std::make_tuple(
		make_path_to_expression_params{ // expect a empty path if start passes the end expression
			dcon::province_id{ 414 }, // start
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return gamestate->world.province_get_is_coast(end_prov); }, // end expression
			std::vector<dcon::province_id>{  }, // expected path
		},
		make_path_to_expression_params{ // expect a empty path if no reachable province passes the expression check
			dcon::province_id{ 20 }, // start
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 300 }; }, // end expression
			std::vector<dcon::province_id>{  } // expected path
		},
		make_path_to_expression_params{ // expect a 1-tile path to the end province when pathfinding from London to Chelmsford as expression
			dcon::province_id{ 299 }, // start
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 292 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 292 } }// expected path
		},
		make_path_to_expression_params{ // expect a 3-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression. It is fastest to go Nitra -> Gyor -> Szejesfegervar -> Budapest
			dcon::province_id{ 634}, // start
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 640 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 642 },  dcon::province_id{ 641 }}// expected path
		},



		make_path_to_expression_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression when Gyor is impassabble. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return to_prov != dcon::province_id{ 641 }; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 640 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 }}// expected path
		},

		make_path_to_expression_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression when river adjacencies are impassable. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			[](auto adj) { return (gamestate->world.province_adjacency_get_type(adj) & province::border::river_crossing_bit) == 0; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 640 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 }}// expected path
		},





		make_path_to_expression_params{ // expect a 6-tile fastest path to the end province when pathfinding from Rostov to Kiev. It is fastest to go Yuzovka-> Kramatorsk -> Ekaterinoslav -> Krivoyrog -> Cherkassy -> Kiev
			dcon::province_id{ 978}, // start
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 957 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 957 }, dcon::province_id{ 960 },dcon::province_id{ 970 }, dcon::province_id{ 971 }, dcon::province_id{ 973 },  dcon::province_id{ 974 }}// expected path
		}

	);

	std::apply([](auto&&... args) {(( args.assert_test_result() ), ...); }, params);
	
}


// this function is for profiling
template<typename AdjFunc, typename ProvFunc, typename ModifierFunc, typename EndFunc>
std::vector<dcon::province_id> make_path_to_expression_tagged_vector_heap(sys::state& state, dcon::province_id start, AdjFunc&& adj_func, ProvFunc&& prov_func, ModifierFunc&& mod_func, EndFunc&& end_expression) {

	// uses an A* implementation

	std::vector<dcon::province_id> open_queue;
	tagged_vector<province::path_node, dcon::province_id> path_node_container(state.world.province_size());
	//auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

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
		current_node.is_in_open_list = false;
		// check if we have reached the end
		if(end_expression(current_prov)) {
			fill_path_result(current_prov);
			assert_path_result(path_result);
			return path_result;
		}
		// add current to closed list immediately
		current_node.is_in_closed_list = true;

		for(auto adj : state.world.province_get_province_adjacency(current_prov)) {
			auto other_prov =
				adj.get_connected_provinces(0) == current_prov ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			auto& neighbor_node = path_node_container[other_prov];

			// check if not present in the closed list, and passes the adjacency check (aka the specific adjacency is passable)
			if(!neighbor_node.is_in_closed_list && adj_func(adj.id)) {
				// check if province check passes (aka province isnt impassable from all directions). If not, add to closed list
				if(prov_func(other_prov)) {
					float modifier = mod_func(other_prov, current_node.province); // to and from province
					float distance_to_neighbor = current_node.distance_covered + distance * modifier;
					// if not present in the path heap, add it to the path heap for processing later

					if(!neighbor_node.is_in_open_list || distance_to_neighbor < neighbor_node.distance_covered) {
						neighbor_node.distance_covered = distance_to_neighbor;
						neighbor_node.parent = current_node.province;
						if(!neighbor_node.is_in_open_list) {
							neighbor_node.province = other_prov; // set province now, before it is added to the open queue
							open_queue.push_back(other_prov);
							std::push_heap(open_queue.begin(), open_queue.end(), province_comparer);
							neighbor_node.is_in_open_list = true;
						}

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




// this function is for profiling
template<typename AdjFunc, typename ProvFunc, typename ModifierFunc, typename EndFunc>
std::vector<dcon::province_id> make_path_to_expression_tagged_vector_thread_local(sys::state& state, dcon::province_id start, AdjFunc&& adj_func, ProvFunc&& prov_func, ModifierFunc&& mod_func, EndFunc&& end_expression) {

	// uses an A* implementation

	std::vector<dcon::province_id> open_queue;
	static thread_local tagged_vector<province::path_node, dcon::province_id> path_node_container(state.world.province_size());
	path_node_container.resize(state.world.province_size());
	std::memset(path_node_container.data(), 0, sizeof(province::path_node) * path_node_container.size());
	//auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

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
		current_node.is_in_open_list = false;
		// check if we have reached the end
		if(end_expression(current_prov)) {
			fill_path_result(current_prov);
			assert_path_result(path_result);
			return path_result;
		}
		// add current to closed list immediately
		current_node.is_in_closed_list = true;

		for(auto adj : state.world.province_get_province_adjacency(current_prov)) {
			auto other_prov =
				adj.get_connected_provinces(0) == current_prov ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			auto& neighbor_node = path_node_container[other_prov];

			// check if not present in the closed list, and passes the adjacency check (aka the specific adjacency is passable)
			if(!neighbor_node.is_in_closed_list && adj_func(adj.id)) {
				// check if province check passes (aka province isnt impassable from all directions). If not, add to closed list
				if(prov_func(other_prov)) {
					float modifier = mod_func(other_prov, current_node.province); // to and from province
					float distance_to_neighbor = current_node.distance_covered + distance * modifier;
					// if not present in the path heap, add it to the path heap for processing later

					if(!neighbor_node.is_in_open_list || distance_to_neighbor < neighbor_node.distance_covered) {
						neighbor_node.distance_covered = distance_to_neighbor;
						neighbor_node.parent = current_node.province;
						if(!neighbor_node.is_in_open_list) {
							neighbor_node.province = other_prov; // set province now, before it is added to the open queue
							open_queue.push_back(other_prov);
							std::push_heap(open_queue.begin(), open_queue.end(), province_comparer);
							neighbor_node.is_in_open_list = true;
						}

					}


				} else {
					//origins_vector.set(other_prov, dcon::province_id{ 0 });
					neighbor_node.is_in_closed_list = true;// exclude it from being checked again
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;

}



TEST_CASE("make_path_to_expression_profiling", "[pathfinding_profiling]") {

	gamestate = load_testing_scenario_file_with_save(sys::network_mode_type::host);

	{ 
		auto start = dcon::province_id{ 978};
		auto adj_func = [&](auto adj) { return true; };
		auto prov_func = [&](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } };
		auto mod_func = [&](auto to_prov, auto from_prov) { return 1.0f; };
		auto end_exp = [&](auto end_prov) { return end_prov == dcon::province_id{ 975 }; };



		std::vector<size_t> placeholder;
		placeholder.reserve(2000000);
		std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
		for(int i = 0; i < 1000000; i++) {
			auto a = make_path_to_expression_tagged_vector_heap(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
		auto net_time = time_end - time_start;
		gamestate->console_log("tagged_vector Heap small path ns: "  + std::to_string(net_time.count()));


		time_start = std::chrono::steady_clock::now();
		for(int i = 0; i < 1000000; i++) {
			auto a = make_path_to_expression_tagged_vector_thread_local(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		time_end = std::chrono::steady_clock::now();
		net_time = time_end - time_start;
		gamestate->console_log("tagged_vector thread_local small path ns: " + std::to_string(net_time.count()));

		std::cout << placeholder.size();

		std::this_thread::sleep_for(std::chrono::seconds{1 });
	}
	{ 
		auto start = dcon::province_id{ 978};
		auto adj_func = [&](auto adj) { return true; };
		auto prov_func = [&](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } };
		auto mod_func = [&](auto to_prov, auto from_prov) { return 1.0f; };
		auto end_exp = [&](auto end_prov) { return end_prov == dcon::province_id{ 957 }; };



		std::vector<size_t> placeholder;
		placeholder.reserve(2000000);
		std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
		for(int i = 0; i < 1000000; i++) {
			auto a = make_path_to_expression_tagged_vector_heap(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
		auto net_time = time_end - time_start;
		gamestate->console_log("tagged_vector Heap medium path ns: "  + std::to_string(net_time.count()));


		time_start = std::chrono::steady_clock::now();
		for(int i = 0; i < 1000000; i++) {
			auto a = make_path_to_expression_tagged_vector_thread_local(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		time_end = std::chrono::steady_clock::now();
		net_time = time_end - time_start;
		gamestate->console_log("tagged_vector thread_local medium path ns: " + std::to_string(net_time.count()));

		std::cout << placeholder.size();

		std::this_thread::sleep_for(std::chrono::seconds{1 });
	}




	
}

TEST_CASE("make_path_to_expression_profiling_AStar", "[pathfinding_profiling]") {
	gamestate = load_testing_scenario_file_with_save(sys::network_mode_type::host);
	{
		auto start = dcon::province_id{ 978 };
		auto adj_func = [&](auto adj) { return true; };
		auto prov_func = [&](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) {
			return true;
		} else {
			return false;
		} };
		auto mod_func = [&](auto to_prov, auto from_prov) { return 1.0f; };
		auto end_exp = [&](auto end_prov) { return end_prov == dcon::province_id{ 957 }; };



		std::vector<size_t> placeholder;
		placeholder.reserve(2000000);
		std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
		for(int i = 0; i < 1000000; i++) {
			auto a = province::make_path_to_expression(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
		auto net_time = time_end - time_start;
		gamestate->console_log("tagged_vector thread_local medium path A* ns: " + std::to_string(net_time.count()));


		time_start = std::chrono::steady_clock::now();
		for(int i = 0; i < 1000000; i++) {
			auto a = province::make_path_to_expression_old(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		time_end = std::chrono::steady_clock::now();
		net_time = time_end - time_start;
		gamestate->console_log("medium path NOT A* ns: " + std::to_string(net_time.count()));

		std::cout << placeholder.size();

		std::this_thread::sleep_for(std::chrono::seconds{ 1 });
	}


	{
		auto start = dcon::province_id{ 978 };
		auto adj_func = [&](auto adj) { return true; };
		auto prov_func = [&](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) {
			return true;
		} else {
			return false;
		} };
		auto mod_func = [&](auto to_prov, auto from_prov) { return 1.0f; };
		auto end_exp = [&](auto end_prov) { return end_prov == dcon::province_id{ 1470 }; };



		std::vector<size_t> placeholder;
		placeholder.reserve(2000000);
		std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
		for(int i = 0; i < 1000000; i++) {
			auto a = province::make_path_to_expression(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
		auto net_time = time_end - time_start;
		gamestate->console_log("tagged_vector thread_local long path A* ns: " + std::to_string(net_time.count()));


		time_start = std::chrono::steady_clock::now();
		for(int i = 0; i < 1000000; i++) {
			auto a = province::make_path_to_expression_old(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		time_end = std::chrono::steady_clock::now();
		net_time = time_end - time_start;
		gamestate->console_log("long path NOT A* ns: " + std::to_string(net_time.count()));

		std::cout << placeholder.size();

		std::this_thread::sleep_for(std::chrono::seconds{ 1 });
	}

}




