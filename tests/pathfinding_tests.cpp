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
		auto actual_path = province::make_path_to_prov(*gamestate, start, end, adj_func, prov_func, mod_func);
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
	dcon::province_id nitra = dcon::province_id{ 634 };
	dcon::province_id miskolc = dcon::province_id{ 645 };
	dcon::province_id budapest = dcon::province_id{ 640 };
	dcon::province_id gyor = dcon::province_id{ 641 };

	



	gamestate = load_testing_scenario_file_with_save(sys::network_mode_type::host);

	float miskolcDist = gamestate->world.province_adjacency_get_distance( gamestate->world.get_province_adjacency_by_province_pair(nitra, miskolc)) + gamestate->world.province_adjacency_get_distance(gamestate->world.get_province_adjacency_by_province_pair(miskolc, budapest));
	float gyorDist = gamestate->world.province_adjacency_get_distance(gamestate->world.get_province_adjacency_by_province_pair(nitra, gyor)) + gamestate->world.province_adjacency_get_distance(gamestate->world.get_province_adjacency_by_province_pair(gyor, budapest));

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
		make_path_to_prov_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			dcon::province_id{ 640 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 } }// expected path
		},
		make_path_to_prov_params{ // expect a 5-tile fastest path to the end province when pathfinding from Rostov to Kiev. It is fastest to go Yuzovka-> Kramatorsk -> Poltava -> Cherkassy -> Kiev
			dcon::province_id{ 978}, // start
			dcon::province_id{ 957 }, // end
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 957 }, dcon::province_id{ 960 },dcon::province_id{ 962 }, dcon::province_id{ 973 }, dcon::province_id{ 974 } }// expected path
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
		make_path_to_expression_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 640 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 } }// expected path
		},
		make_path_to_expression_params{ // expect a 5-tile fastest path to the end province when pathfinding from Rostov to Kiev as expression. It is fastest to go Yuzovka-> Kramatorsk -> Poltava -> Cherkassy -> Kiev
			dcon::province_id{ 978}, // start
			[](auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov) { return 1.0f; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 957 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 957 }, dcon::province_id{ 960 },dcon::province_id{ 962 }, dcon::province_id{ 973 }, dcon::province_id{ 974 } }// expected path
		}

	);

	std::apply([](auto&&... args) {(( args.assert_test_result() ), ...); }, params);
	
}
