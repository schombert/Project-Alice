#include "catch.hpp"
#include "commands.hpp"
#include "province_templates.hpp"
#include "serialization.hpp"

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

TEST_CASE("army merge preserves demand-weighted supply reserves", "[pathfinding][military][logistics]") {
	auto state = std::make_unique<sys::state>();
	state->current_scene.game_in_progress = true;
	auto owner = state->world.create_nation();
	auto province = state->world.create_province();
	auto light_type = state->military_definitions.unit_base_definitions.emplace_back();
	auto heavy_type = state->military_definitions.unit_base_definitions.emplace_back();
	state->military_definitions.unit_base_definitions[light_type].supply_consumption = 1.f;
	state->military_definitions.unit_base_definitions[heavy_type].supply_consumption = 3.f;

	auto supplied = state->world.create_army();
	auto depleted = state->world.create_army();
	for(auto army : { supplied, depleted }) {
		state->world.army_set_controller_from_army_control(army, owner);
		state->world.army_set_location_from_army_location(army, province);
	}
	auto light = state->world.create_regiment();
	auto heavy = state->world.create_regiment();
	state->world.regiment_set_type(light, light_type);
	state->world.regiment_set_type(heavy, heavy_type);
	state->world.try_create_army_membership(light, supplied);
	state->world.try_create_army_membership(heavy, depleted);
	state->world.army_set_supply_reserve(supplied, 1.f);
	state->world.army_set_supply_reserve(depleted, 0.f);
	state->world.army_set_supply_priority(supplied, 0);
	state->world.army_set_supply_priority(depleted, 2);

	command::execute_merge_armies(*state, owner, supplied, depleted);

	REQUIRE(state->world.army_is_valid(supplied));
	REQUIRE_FALSE(state->world.army_is_valid(depleted));
	REQUIRE(state->world.army_get_supply_reserve(supplied) == Approx(0.25f));
	REQUIRE(state->world.army_get_supply_priority(supplied) == 2);
}

TEST_CASE("legacy saves initialize missing army logistics fields", "[pathfinding][military][logistics][serialization]") {
	auto state = std::make_unique<sys::state>();
	auto army = state->world.create_army();
	state->world.army_set_supply_reserve(army, 0.35f);
	state->world.army_set_supply_priority(army, 2);

	auto legacy_record = state->world.make_serialize_record_store_save();
	legacy_record.army_supply_reserve = false;
	legacy_record.army_supply_priority = false;
	std::vector<std::byte> buffer(state->world.serialize_size(legacy_record));
	auto write = buffer.data();
	state->world.serialize(write, legacy_record);

	auto loaded_state = std::make_unique<sys::state>();
	dcon::load_record loaded_record;
	std::byte const* read = buffer.data();
	loaded_state->world.deserialize(read, buffer.data() + buffer.size(), loaded_record);
	REQUIRE_FALSE(loaded_record.army_supply_reserve);
	REQUIRE_FALSE(loaded_record.army_supply_priority);

	sys::migrate_legacy_army_supply_fields(*loaded_state, loaded_record);
	REQUIRE(loaded_state->world.army_get_supply_reserve(army) == Approx(1.0f));
	REQUIRE(loaded_state->world.army_get_supply_priority(army) == 1);

	loaded_state->world.army_set_supply_reserve(army, 0.4f);
	loaded_state->world.army_set_supply_priority(army, 2);
	loaded_record.army_supply_reserve = true;
	loaded_record.army_supply_priority = true;
	sys::migrate_legacy_army_supply_fields(*loaded_state, loaded_record);
	REQUIRE(loaded_state->world.army_get_supply_reserve(army) == Approx(0.4f));
	REQUIRE(loaded_state->world.army_get_supply_priority(army) == 2);

	loaded_state->army_supply_access_cache.emplace_back();
	loaded_state->army_supply_capacity_factor_cache.push_back(0.5f);
	loaded_state->supply_depot_connected_cache.push_back(1);
	loaded_state->army_supply_cache_valid = true;
	loaded_state->clear_army_supply_derived_data();
	REQUIRE(loaded_state->army_supply_access_cache.empty());
	REQUIRE(loaded_state->army_supply_capacity_factor_cache.empty());
	REQUIRE(loaded_state->supply_depot_connected_cache.empty());
	REQUIRE_FALSE(loaded_state->army_supply_cache_valid);
}

TEST_CASE("Dijkstra restores heap ordering after decrease-key", "[pathfinding][military][logistics]") {
	auto state = std::make_unique<sys::state>();
	auto start = state->world.create_province();
	auto expensive = state->world.create_province();
	auto shortcut = state->world.create_province();
	auto target = state->world.create_province();
	auto connect = [&](dcon::province_id a, dcon::province_id b, float cost) {
		auto adjacency = state->world.force_create_province_adjacency(a, b);
		state->world.province_adjacency_set_distance(adjacency, cost);
		state->world.province_adjacency_set_type(adjacency, 0);
	};
	// Initial queue: expensive=10, shortcut=1, target=5. Visiting shortcut
	// lowers expensive to 2; the heap must be repaired before target is closed.
	connect(start, expensive, 10.f);
	connect(start, shortcut, 1.f);
	connect(start, target, 5.f);
	connect(shortcut, expensive, 1.f);
	connect(expensive, target, 1.f);

	auto path = province::make_path_to_expression(
		*state,
		start,
		[](auto, auto, auto) { return true; },
		[](auto) { return true; },
		[](auto, auto, auto, float distance) { return distance; },
		[target](auto province) { return province == target; });

	REQUIRE(path == std::vector<dcon::province_id>{ target, expensive, shortcut });
}



// these tests expect a vanilla scenario
TEST_CASE("make_path_to_prov_valid_tests", "[pathfinding]") {


	gamestate = load_testing_scenario_file_with_save(sys::network_mode_type::host);


	auto params = std::make_tuple(
		make_path_to_prov_params{ // expect a empty path if start == end on land prov
			dcon::province_id{ 0 }, // start
			dcon::province_id{ 0 }, // end
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{  }, // expected path
		},
		make_path_to_prov_params{ // expect a empty path if start == end on sea prov
			dcon::province_id{ 3000 }, // start
			dcon::province_id{ 3000 }, // end
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{  } // expected path
		},
		make_path_to_prov_params{ // expect a empty path if two land provinces are not reachable from eachother ( Tries to go from Europe to USA) while sea is not allowed to be crossed
			dcon::province_id{ 20 }, // start
			dcon::province_id{ 300 }, // end
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{  } // expected path
		},
		make_path_to_prov_params{ // expect a empty path if two sea provinces are not reachable from eachother ( Tries to go from red sea to the Yang Tse Delta) while land is not allowed to be crossed
			dcon::province_id{ 2779 }, // start
			dcon::province_id{ 2826 }, // end
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return false; } else {return true; } }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{  } // expected path
		},
		make_path_to_prov_params{ // expect a 1-tile path to the end province when pathfinding from London to Chelmsford
			dcon::province_id{ 299 }, // start
			dcon::province_id{ 292 }, // end
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 292 } }// expected path
		},
		make_path_to_prov_params{ // expect a 3-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression. It is fastest to go Nitra -> Gyor -> Szejesfegervar -> Budapest
			dcon::province_id{ 634}, // start
			dcon::province_id{ 640 }, // end
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 642 },  dcon::province_id{ 641 }}// expected path
		},



		make_path_to_prov_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest when Gyor is impassabble. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			dcon::province_id{ 640 }, // end
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return to_prov != dcon::province_id{ 641 }; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 }}// expected path
		},

		make_path_to_prov_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest when river adjacencies are impassable. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			dcon::province_id{ 640 }, //end
			[](auto to, auto from, auto adj) { return (gamestate->world.province_adjacency_get_type(adj) & province::border::river_crossing_bit) == 0; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 }}// expected path
		},

		make_path_to_prov_params{ // expect a 6-tile fastest path to the end province when pathfinding from Rostov to Kiev. It is fastest to go Yuzovka-> Kramatorsk -> Ekaterinoslav -> Krivoyrog -> Cherkassy -> Kiev
			dcon::province_id{ 978}, // start
			dcon::province_id{ 957 }, // end
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			std::vector<dcon::province_id>{ dcon::province_id{ 957 }, dcon::province_id{ 960 },dcon::province_id{ 970 }, dcon::province_id{ 971 }, dcon::province_id{ 973 },  dcon::province_id{ 974 }}// expected path
		}

	);

	std::apply([](auto&&... args) {(( args.assert_test_result() ), ...); }, params);
	
}




TEST_CASE("make_path_to_expression_valid_tests", "[pathfinding]") {

	gamestate = load_testing_scenario_file_with_save(sys::network_mode_type::host);

	auto params = std::make_tuple(
		make_path_to_expression_params{ // expect a empty path if start passes the end expression
			dcon::province_id{ 414 }, // start
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			[](auto end_prov) { return gamestate->world.province_get_is_coast(end_prov); }, // end expression
			std::vector<dcon::province_id>{  }, // expected path
		},
		make_path_to_expression_params{ // expect a empty path if no reachable province passes the expression check
			dcon::province_id{ 20 }, // start
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 300 }; }, // end expression
			std::vector<dcon::province_id>{  } // expected path
		},
		make_path_to_expression_params{ // expect a 1-tile path to the end province when pathfinding from London to Chelmsford as expression
			dcon::province_id{ 299 }, // start
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 292 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 292 } }// expected path
		},
		make_path_to_expression_params{ // expect a 3-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression. It is fastest to go Nitra -> Gyor -> Szejesfegervar -> Budapest
			dcon::province_id{ 634}, // start
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 640 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 642 },  dcon::province_id{ 641 }}// expected path
		},



		make_path_to_expression_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression when Gyor is impassabble. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { return to_prov != dcon::province_id{ 641 }; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 640 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 }}// expected path
		},

		make_path_to_expression_params{ // expect a 2-tile fastest path to the end province when pathfinding from Nitra to Budapest as expression when river adjacencies are impassable. It is fastest to go Nitra -> Miskolc -> Budapest
			dcon::province_id{ 634}, // start
			[](auto to, auto from, auto adj) { return (gamestate->world.province_adjacency_get_type(adj) & province::border::river_crossing_bit) == 0; }, // adj fn
			[](dcon::province_id to_prov) { return true; }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 640 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 640 }, dcon::province_id{ 645 }}// expected path
		},





		make_path_to_expression_params{ // expect a 6-tile fastest path to the end province when pathfinding from Rostov to Kiev. It is fastest to go Yuzovka-> Kramatorsk -> Ekaterinoslav -> Krivoyrog -> Cherkassy -> Kiev
			dcon::province_id{ 978}, // start
			[](auto to, auto from, auto adj) { return true; }, // adj fn
			[](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } }, // prov fn
			[](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; }, // mod fn
			[](auto end_prov) { return end_prov == dcon::province_id{ 957 }; }, // end expression
			std::vector<dcon::province_id>{ dcon::province_id{ 957 }, dcon::province_id{ 960 },dcon::province_id{ 970 }, dcon::province_id{ 971 }, dcon::province_id{ 973 },  dcon::province_id{ 974 }}// expected path
		}

	);

	std::apply([](auto&&... args) {(( args.assert_test_result() ), ...); }, params);
	
}

TEST_CASE("army_supply_access_is_diagnostic_and_factorized", "[pathfinding][military]") {
	auto state = std::make_unique<sys::state>();
	auto owner = state->world.create_nation();
	auto ally = state->world.create_nation();
	auto enemy = state->world.create_nation();
	state->province_definitions.first_sea_province = dcon::province_id{ 100 };
	state->world.province_resize_building_level(economy::max_building_types);
	state->world.nation_resize_modifier_values(sys::national_mod_offsets::count);

	state->world.nation_set_effective_land_spending(owner, 1.0f);
	REQUIRE(military::army_supply_reinforcement_factor(0.0f) == Approx(0.25f));
	REQUIRE(military::army_supply_reinforcement_factor(0.5f) == Approx(0.625f));
	REQUIRE(military::army_supply_reinforcement_factor(1.0f) == Approx(1.0f));
	REQUIRE(military::army_supply_movement_factor(0.0f) == Approx(0.75f));
	REQUIRE(military::army_supply_movement_factor(0.5f) == Approx(0.875f));
	REQUIRE(military::army_supply_movement_factor(1.0f) == Approx(1.0f));

	auto make_province = [&](dcon::nation_id province_owner, dcon::nation_id controller) {
		auto province = state->world.create_province();
		state->world.province_set_nation_from_province_ownership(province, province_owner);
		state->world.province_set_nation_from_province_control(province, controller);
		state->world.province_set_control_ratio(province, 1.0f);
		return province;
	};
	auto make_source = [&](dcon::nation_id source_owner) {
		auto province = make_province(source_owner, source_owner);
		auto source_state = state->world.create_state_instance();
		state->world.province_set_state_membership(province, source_state);
		state->world.state_instance_set_capital(source_state, province);
		return province;
	};
	auto connect = [&](dcon::province_id a, dcon::province_id b, float distance_km) {
		auto adjacency = state->world.force_create_province_adjacency(a, b);
		state->world.province_adjacency_set_distance_km(adjacency, distance_km);
		state->world.province_adjacency_set_type(adjacency, 0);
		return adjacency;
	};

	auto source = make_source(owner);
	state->world.province_set_control_ratio(source, 1.0f);
	state->world.province_set_building_level(source, uint8_t(economy::province_building_type::railroad), 0);

	auto result = military::calculate_army_supply_access(*state, owner, source);
	REQUIRE(result.reachable);
	REQUIRE(result.source == source);
	REQUIRE(result.distance_km == 0.0f);
	REQUIRE(result.distance_factor == 1.0f);
	REQUIRE(result.infrastructure_factor == Approx(0.6f));
	REQUIRE(result.control_factor == 1.0f);
	REQUIRE(result.military_goods_availability == 1.0f);
	REQUIRE(result.spatial_access == Approx(0.6f));
	REQUIRE(result.effective_supply == Approx(0.6f));

	state->world.province_set_control_ratio(source, 0.5f);
	auto reduced_control = military::calculate_army_supply_access(*state, owner, source);
	REQUIRE(reduced_control.spatial_access == Approx(0.3f));
	state->world.nation_set_effective_land_spending(owner, 0.5f);
	auto reduced_goods = military::calculate_army_supply_access(*state, owner, source);
	REQUIRE(reduced_goods.spatial_access == Approx(0.3f));
	REQUIRE(reduced_goods.military_goods_availability == Approx(0.5f));
	REQUIRE(reduced_goods.effective_supply == Approx(0.15f));
	state->world.nation_set_effective_land_spending(owner, 1.0f);

	auto remote = make_province(owner, owner);
	state->world.province_set_control_ratio(remote, 0.5f);
	connect(source, remote, 1000.0f);

	state->world.province_set_control_ratio(source, 1.0f);
	auto remote_result = military::calculate_army_supply_access(*state, owner, remote);
	REQUIRE(remote_result.reachable);
	REQUIRE(remote_result.source == source);
	REQUIRE(remote_result.distance_km == Approx(1000.0f));
	REQUIRE(remote_result.distance_factor == Approx(2.0f / 3.0f));
	REQUIRE(remote_result.infrastructure_factor == Approx(0.6f));
	REQUIRE(remote_result.control_factor == Approx(0.75f));
	REQUIRE(remote_result.spatial_access == Approx(0.3f));
	REQUIRE(remote_result.effective_supply == Approx(0.3f));

	SECTION("an isolated port is not a source before sea routes exist") {
		auto port = make_province(owner, owner);
		state->world.province_set_is_coast(port, true);
		state->world.province_set_port_to(port, dcon::province_id{ 100 });
		state->world.province_set_is_blockaded(port, false);
		auto unblocked = military::calculate_army_supply_access(*state, owner, port);
		REQUIRE_FALSE(unblocked.reachable);
		REQUIRE(unblocked.spatial_access == 0.0f);

		state->world.province_set_is_blockaded(port, true);
		auto blockaded = military::calculate_army_supply_access(*state, owner, port);
		REQUIRE_FALSE(blockaded.reachable);
		REQUIRE(blockaded.effective_supply == 0.0f);
	}

	SECTION("a port supplies only through a real unblocked sea route") {
		auto origin_port = make_province(owner, owner);
		auto destination_port = make_province(owner, owner);
		state->world.province_set_is_coast(origin_port, true);
		state->world.province_set_is_coast(destination_port, true);
		connect(source, origin_port, 100.0f);
		while(state->world.province_size() <= 101) {
			state->world.create_province();
		}
		auto origin_sea = dcon::province_id{ 100 };
		auto destination_sea = dcon::province_id{ 101 };
		state->world.province_set_port_to(origin_port, origin_sea);
		state->world.province_set_port_to(destination_port, destination_sea);
		connect(origin_port, origin_sea, 10.0f);
		auto sea_adjacency = connect(origin_sea, destination_sea, 500.0f);
		connect(destination_sea, destination_port, 10.0f);

		auto connected = military::calculate_army_supply_access(*state, owner, destination_port);
		REQUIRE(connected.reachable);
		REQUIRE(connected.source == source);
		REQUIRE(connected.uses_sea_route);
		REQUIRE(connected.embark_port == origin_port);
		REQUIRE(connected.disembark_port == destination_port);
		REQUIRE(connected.sea_distance_km == Approx(520.0f));

		for(auto province : { source, origin_port, destination_port }) {
			state->world.province_set_building_level(province, uint8_t(economy::province_building_type::railroad), 5);
		}
		dcon::army_id first_port_army;
		for(int32_t index = 0; index < 20; ++index) {
			auto army = state->world.create_army();
			state->world.army_set_controller_from_army_control(army, owner);
			state->world.army_set_location_from_army_location(army, destination_port);
			if(index == 0) {
				first_port_army = army;
			}
		}
		auto congested_port = military::calculate_army_supply_access(*state, first_port_army);
		REQUIRE(congested_port.route_capacity == Approx(10.0f));
		REQUIRE(congested_port.route_demand == Approx(20.0f));
		REQUIRE(congested_port.capacity_factor == Approx(0.5f));
		state->world.province_set_building_level(origin_port, uint8_t(economy::province_building_type::naval_base), 1);
		state->world.province_set_building_level(destination_port, uint8_t(economy::province_building_type::naval_base), 1);
		auto expanded_ports = military::calculate_army_supply_access(*state, first_port_army);
		REQUIRE(expanded_ports.route_capacity == Approx(20.0f));
		REQUIRE(expanded_ports.capacity_factor == Approx(1.0f));

		state->world.province_set_is_blockaded(destination_port, true);
		auto blockaded = military::calculate_army_supply_access(*state, owner, destination_port);
		REQUIRE_FALSE(blockaded.reachable);
		state->world.province_set_is_blockaded(destination_port, false);

		state->world.province_adjacency_set_type(sea_adjacency, province::border::impassible_bit);
		auto closed_strait = military::calculate_army_supply_access(*state, owner, destination_port);
		REQUIRE_FALSE(closed_strait.reachable);
		state->world.province_adjacency_set_type(sea_adjacency, 0);

		state->world.province_set_is_blockaded(origin_port, true);
		auto origin_blockaded = military::calculate_army_supply_access(*state, owner, destination_port);
		REQUIRE_FALSE(origin_blockaded.reachable);
	}

	SECTION("a route may cross allied control but not occupied control") {
		auto allied_source = make_source(owner);
		auto allied_middle = make_province(ally, ally);
		auto allied_remote = make_province(owner, owner);
		connect(allied_source, allied_middle, 100.0f);
		connect(allied_middle, allied_remote, 100.0f);
		auto relation = state->world.force_create_diplomatic_relation(owner, ally);
		state->world.diplomatic_relation_set_are_allied(relation, true);

		auto through_ally = military::calculate_army_supply_access(*state, owner, allied_remote);
		REQUIRE(through_ally.reachable);
		REQUIRE(through_ally.source == allied_source);

		state->world.province_set_nation_from_province_control(allied_middle, enemy);
		auto occupied = military::calculate_army_supply_access(*state, owner, allied_remote);
		REQUIRE_FALSE(occupied.reachable);
		REQUIRE(occupied.spatial_access == 0.0f);
	}

	SECTION("a break in a railroad lowers spatial access") {
		auto rail_source = make_source(owner);
		auto rail_middle = make_province(owner, owner);
		auto rail_remote = make_province(owner, owner);
		connect(rail_source, rail_middle, 200.0f);
		connect(rail_middle, rail_remote, 200.0f);
		for(auto province : { rail_source, rail_middle, rail_remote }) {
			state->world.province_set_building_level(province, uint8_t(economy::province_building_type::railroad), 5);
		}
		auto continuous = military::calculate_army_supply_access(*state, owner, rail_remote);
		state->world.province_set_building_level(rail_middle, uint8_t(economy::province_building_type::railroad), 0);
		auto interrupted = military::calculate_army_supply_access(*state, owner, rail_remote);
		REQUIRE(continuous.reachable);
		REQUIRE(interrupted.reachable);
		REQUIRE(continuous.infrastructure_factor == Approx(1.0f));
		REQUIRE(interrupted.infrastructure_factor == Approx(0.6f));
		REQUIRE(interrupted.spatial_access < continuous.spatial_access);
	}

	SECTION("armies in different provinces compete for a shared route edge") {
		auto shared_source = make_source(owner);
		auto shared_hub = make_province(owner, owner);
		auto first_destination = make_province(owner, owner);
		auto second_destination = make_province(owner, owner);
		connect(shared_source, shared_hub, 100.0f);
		connect(shared_hub, first_destination, 100.0f);
		connect(shared_hub, second_destination, 100.0f);
		dcon::army_id first_army;
		for(int32_t index = 0; index < 20; ++index) {
			auto army = state->world.create_army();
			state->world.army_set_controller_from_army_control(army, owner);
			state->world.army_set_location_from_army_location(army,
				index < 10 ? first_destination : second_destination);
			if(index == 0) first_army = army;
		}
		auto congested = military::calculate_army_supply_access(*state, first_army);
		REQUIRE(congested.route_capacity == Approx(10.0f));
		REQUIRE(congested.route_demand == Approx(20.0f));
		REQUIRE(congested.capacity_factor == Approx(0.5f));

		state->world.province_set_building_level(shared_source, uint8_t(economy::province_building_type::railroad), 1);
		state->world.province_set_building_level(shared_hub, uint8_t(economy::province_building_type::railroad), 1);
		auto expanded = military::calculate_army_supply_access(*state, first_army);
		REQUIRE(expanded.route_capacity == Approx(20.0f));
		REQUIRE(expanded.route_demand == Approx(20.0f));
		REQUIRE(expanded.capacity_factor == Approx(1.0f));
	}

	SECTION("cached army access remains a constant-time snapshot until invalidated") {
		auto cached_source = make_source(owner);
		auto cached_front = make_province(owner, owner);
		auto route = connect(cached_source, cached_front, 100.0f);
		auto unit_type = state->military_definitions.unit_base_definitions.emplace_back();
		state->military_definitions.unit_base_definitions[unit_type].supply_consumption = 1.0f;
		auto army = state->world.create_army();
		state->world.army_set_controller_from_army_control(army, owner);
		state->world.army_set_location_from_army_location(army, cached_front);
		auto regiment = state->world.create_regiment();
		state->world.regiment_set_type(regiment, unit_type);
		state->world.regiment_set_strength(regiment, 1.0f);
		state->world.try_create_army_membership(regiment, army);

		military::update_army_supply_cache(*state);
		auto initial = military::calculate_army_supply_access_cached(*state, army);
		REQUIRE(initial.reachable);
		REQUIRE(initial.replacement_load == Approx(0.0f));

		state->world.province_adjacency_set_type(route, province::border::impassible_bit);
		state->world.regiment_set_strength(regiment, 0.0f);
		auto snapshot = military::calculate_army_supply_access_cached(*state, army);
		REQUIRE(snapshot.reachable);
		REQUIRE(snapshot.replacement_load == Approx(0.0f));
		REQUIRE(state->army_supply_cache_valid);

		military::invalidate_army_supply_cache(*state);
		auto refreshed = military::calculate_army_supply_access_cached(*state, army);
		REQUIRE_FALSE(refreshed.reachable);
		REQUIRE(refreshed.replacement_load == Approx(1.0f));
	}

	SECTION("unit composition determines logistics demand") {
		auto light_type = state->military_definitions.unit_base_definitions.emplace_back();
		auto heavy_type = state->military_definitions.unit_base_definitions.emplace_back();
		state->military_definitions.unit_base_definitions[light_type].supply_consumption = 0.5f;
		state->military_definitions.unit_base_definitions[heavy_type].supply_consumption = 2.0f;
		auto logistics_source = make_source(owner);
		auto army = state->world.create_army();
		state->world.army_set_controller_from_army_control(army, owner);
		state->world.army_set_location_from_army_location(army, logistics_source);
		dcon::regiment_id first_light;
		for(int32_t index = 0; index < 4; ++index) {
			auto light = state->world.create_regiment();
			if(index == 0) first_light = light;
			state->world.regiment_set_type(light, light_type);
			state->world.try_create_army_membership(light, army);
			auto heavy = state->world.create_regiment();
			state->world.regiment_set_type(heavy, heavy_type);
			state->world.try_create_army_membership(heavy, army);
		}
		auto weighted = military::calculate_army_supply_access(*state, army);
		REQUIRE(military::regiment_logistics_weight(*state, first_light) == Approx(0.5f));
		REQUIRE(weighted.army_demand == Approx(10.0f));
		REQUIRE(weighted.route_demand == Approx(10.0f));
		REQUIRE(weighted.capacity_factor == Approx(1.0f));

		auto extra_heavy = state->world.create_regiment();
		state->world.regiment_set_type(extra_heavy, heavy_type);
		state->world.try_create_army_membership(extra_heavy, army);
		auto overloaded = military::calculate_army_supply_access(*state, army);
		REQUIRE(overloaded.army_demand == Approx(12.0f));
		REQUIRE(overloaded.route_demand == Approx(12.0f));
		REQUIRE(overloaded.capacity_factor == Approx(10.0f / 12.0f));

		auto soldiers = state->world.create_pop_type();
		auto civilian_type = state->world.create_pop_type();
		state->culture_definitions.soldiers = soldiers;
		auto mobilized_pop = state->world.create_pop();
		state->world.pop_set_poptype(mobilized_pop, civilian_type);
		state->world.try_create_regiment_source(extra_heavy, mobilized_pop);
		REQUIRE(military::regiment_logistics_weight(*state, extra_heavy) == Approx(1.5f));
		auto mobilized = military::calculate_army_supply_access(*state, army);
		REQUIRE(mobilized.army_demand == Approx(11.5f));
		REQUIRE(mobilized.capacity_factor == Approx(10.0f / 11.5f));
	}

	SECTION("supply reserves drain while isolated and recover through a restored route") {
		auto line_type = state->military_definitions.unit_base_definitions.emplace_back();
		state->military_definitions.unit_base_definitions[line_type].supply_consumption = 1.0f;
		auto reserve_source = make_source(owner);
		auto isolated = make_province(owner, owner);
		state->world.province_set_building_level(reserve_source, uint8_t(economy::province_building_type::railroad), 5);
		state->world.province_set_building_level(isolated, uint8_t(economy::province_building_type::railroad), 5);
		auto army = state->world.create_army();
		state->world.army_set_controller_from_army_control(army, owner);
		state->world.army_set_location_from_army_location(army, isolated);
		state->world.army_set_supply_reserve(army, 1.0f);
		for(int32_t index = 0; index < 10; ++index) {
			auto regiment = state->world.create_regiment();
			state->world.regiment_set_type(regiment, line_type);
			state->world.regiment_set_strength(regiment, 1.0f);
			state->world.try_create_army_membership(regiment, army);
		}
		auto cut_off = military::calculate_army_supply_access(*state, army);
		REQUIRE(cut_off.effective_supply == 0.0f);
		REQUIRE(cut_off.supply_reserve == Approx(1.0f));
		REQUIRE(cut_off.reserve_daily_change == Approx(-0.025f));
		REQUIRE(cut_off.replacement_load == Approx(0.0f));
		military::update_army_supply_reserves(*state);
		REQUIRE(state->world.army_get_supply_reserve(army) == Approx(0.975f));

		state->world.army_get_path(army).push_back(reserve_source);
		auto moving = military::calculate_army_supply_access(*state, army);
		REQUIRE(moving.reserve_daily_change == Approx(-0.03125f));
		state->world.army_get_path(army).clear();

		connect(reserve_source, isolated, 0.0f);
		state->world.army_set_supply_reserve(army, 0.5f);
		auto restored = military::calculate_army_supply_access(*state, army);
		REQUIRE(restored.effective_supply == Approx(1.0f));
		REQUIRE(restored.reserve_daily_change == Approx(0.05f));
		military::update_army_supply_reserves(*state);
		REQUIRE(state->world.army_get_supply_reserve(army) == Approx(0.55f));
		REQUIRE(military::calculate_army_supply_access(*state, army).reinforcement_factor == Approx(0.6625f));
		auto first_regiment = (*state->world.army_get_army_membership(army).begin()).get_regiment();
		state->world.regiment_set_strength(first_regiment, 0.0f);
		auto wounded = military::calculate_army_supply_access(*state, army);
		REQUIRE(wounded.replacement_load == Approx(1.0f));
		REQUIRE(wounded.reserve_daily_change == Approx(0.0485f));
		auto checksum_before = state->get_mp_state_checksum();
		state->world.army_set_supply_reserve(army, 0.56f);
		auto checksum_after = state->get_mp_state_checksum();
		REQUIRE_FALSE(checksum_before.is_equal(checksum_after));
		auto save_record = state->world.make_serialize_record_store_save();
		std::vector<std::byte> save_buffer(state->world.serialize_size(save_record));
		auto write_position = save_buffer.data();
		state->world.serialize(write_position, save_record);
		auto restored_state = std::make_unique<sys::state>();
		dcon::load_record loaded_record;
		std::byte const* read_position = save_buffer.data();
		restored_state->world.deserialize(read_position, save_buffer.data() + save_buffer.size(), loaded_record);
		REQUIRE(restored_state->world.army_get_supply_reserve(army) == Approx(0.56f));
	}

	SECTION("equal sources are selected deterministically by province id") {
		auto first_source = make_source(owner);
		auto second_source = make_source(owner);
		auto equal_remote = make_province(owner, owner);
		connect(first_source, equal_remote, 250.0f);
		connect(second_source, equal_remote, 250.0f);
		REQUIRE(first_source.index() < second_source.index());
		for(int32_t iteration = 0; iteration < 5; ++iteration) {
			auto equal_result = military::calculate_army_supply_access(*state, owner, equal_remote);
			REQUIRE(equal_result.reachable);
			REQUIRE(equal_result.source == first_source);
		}
	}

	SECTION("an encircled army keeps minimum reinforcement and recovers the old rate with its route") {
		auto reinforcement_source = make_source(owner);
		auto encircled_location = make_province(owner, owner);
		state->world.province_set_building_level(reinforcement_source, uint8_t(economy::province_building_type::railroad), 5);
		state->world.province_set_building_level(encircled_location, uint8_t(economy::province_building_type::railroad), 5);
		auto army = state->world.create_army();
		state->world.army_set_controller_from_army_control(army, owner);
		state->world.army_set_location_from_army_location(army, encircled_location);

		auto old_formula = state->defines.reinforce_speed * 2.0f;
		state->army_supply_cache_valid = false;
		auto encircled = military::calculate_army_combined_reinforce<military::reinforcement_estimation_type::monthly>(*state, army);
		REQUIRE_FALSE(state->army_supply_cache_valid);
		REQUIRE(military::calculate_army_supply_access(*state, army).effective_supply == 0.0f);
		REQUIRE(encircled == Approx(old_formula * 0.25f));
		auto encircled_diagnostics = military::calculate_army_supply_diagnostics(*state, owner);
		REQUIRE(encircled_diagnostics.army_count == 1);
		REQUIRE(encircled_diagnostics.disconnected_armies == 1);
		REQUIRE(encircled_diagnostics.low_supply_armies == 1);
		REQUIRE(encircled_diagnostics.average_effective_supply == 0.0f);
		REQUIRE(encircled_diagnostics.calculation_time_us >= 0);

		auto half_supply_source = make_source(owner);
		state->world.province_set_building_level(half_supply_source, uint8_t(economy::province_building_type::railroad), 5);
		state->world.province_set_control_ratio(half_supply_source, 0.5f);
		state->world.army_set_location_from_army_location(army, half_supply_source);
		state->world.army_set_supply_reserve(army, 0.5f);
		auto half_supply = military::calculate_army_supply_access(*state, army);
		auto half_rate = military::calculate_army_combined_reinforce<military::reinforcement_estimation_type::monthly>(*state, army);
		REQUIRE(half_supply.effective_supply == Approx(0.5f));
		REQUIRE(half_supply.reinforcement_factor == Approx(0.625f));
		REQUIRE(half_rate == Approx(old_formula * 0.625f));

		state->world.army_set_location_from_army_location(army, encircled_location);
		connect(reinforcement_source, encircled_location, 0.0f);
		state->world.army_set_supply_reserve(army, 1.0f);
		auto restored_supply = military::calculate_army_supply_access(*state, army);
		auto restored = military::calculate_army_combined_reinforce<military::reinforcement_estimation_type::monthly>(*state, army);
		REQUIRE(restored_supply.effective_supply == Approx(1.0f));
		REQUIRE(restored_supply.reinforcement_factor == Approx(1.0f));
		REQUIRE(restored == Approx(old_formula));
		auto restored_diagnostics = military::calculate_army_supply_diagnostics(*state, owner);
		REQUIRE(restored_diagnostics.army_count == 1);
		REQUIRE(restored_diagnostics.disconnected_armies == 0);
		REQUIRE(restored_diagnostics.low_supply_armies == 0);
		REQUIRE(restored_diagnostics.average_effective_supply == Approx(1.0f));

		state->world.province_set_building_level(reinforcement_source, uint8_t(economy::province_building_type::railroad), 0);
		state->world.province_set_building_level(encircled_location, uint8_t(economy::province_building_type::railroad), 0);
		for(int32_t index = 0; index < 19; ++index) {
			auto competing_army = state->world.create_army();
			state->world.army_set_controller_from_army_control(competing_army, owner);
			state->world.army_set_location_from_army_location(competing_army, encircled_location);
		}
		auto congested_rail = military::calculate_army_supply_access(*state, army);
		REQUIRE(congested_rail.route_capacity == Approx(10.0f));
		REQUIRE(congested_rail.route_demand == Approx(20.0f));
		REQUIRE(congested_rail.capacity_factor == Approx(0.5f));
		REQUIRE(congested_rail.effective_supply == Approx(congested_rail.effective_supply_before_capacity * 0.5f));
		state->world.province_set_building_level(reinforcement_source, uint8_t(economy::province_building_type::railroad), 1);
		state->world.province_set_building_level(encircled_location, uint8_t(economy::province_building_type::railroad), 1);
		auto expanded_rail = military::calculate_army_supply_access(*state, army);
		REQUIRE(expanded_rail.route_capacity == Approx(20.0f));
		REQUIRE(expanded_rail.capacity_factor == Approx(1.0f));

		auto simulate_reinforcement = [](float initial_strength, float monthly_rate, int32_t ticks) {
			for(int32_t tick = 0; tick < ticks; ++tick) {
				initial_strength = std::min(1.0f, initial_strength + monthly_rate);
			}
			return initial_strength;
		};
		REQUIRE(simulate_reinforcement(0.2f, old_formula, 4) == Approx(1.0f));
		REQUIRE(simulate_reinforcement(0.2f, old_formula * 0.25f, 4) == Approx(0.6f));
		REQUIRE(simulate_reinforcement(0.2f, old_formula * 0.625f, 4) == Approx(1.0f));
	}

	SECTION("connected depots buffer a cut and allocate scarce dispatch by priority") {
		auto depot_source = make_source(owner);
		auto depot = make_province(owner, owner);
		auto front = make_province(owner, owner);
		auto rear_edge = connect(depot_source, depot, 100.0f);
		connect(depot, front, 100.0f);
		state->world.province_set_is_supply_depot(depot, true);
		state->world.province_set_supply_depot_owner(depot, owner);
		military::update_supply_depots(*state);
		REQUIRE(state->supply_depot_connected_cache[depot.index()] == 1);
		REQUIRE(state->world.province_get_supply_depot_stockpile(depot) > 0.0f);

		auto unit_type = state->military_definitions.unit_base_definitions.emplace_back();
		state->military_definitions.unit_base_definitions[unit_type].supply_consumption = 1.0f;
		dcon::army_id low;
		dcon::army_id high;
		for(uint8_t priority : { uint8_t(0), uint8_t(2) }) {
			auto army = state->world.create_army();
			state->world.army_set_controller_from_army_control(army, owner);
			state->world.army_set_location_from_army_location(army, front);
			state->world.army_set_supply_priority(army, priority);
			auto regiment = state->world.create_regiment();
			state->world.regiment_set_type(regiment, unit_type);
			state->world.try_create_army_membership(regiment, army);
			(priority == 0 ? low : high) = army;
		}
		state->world.province_set_supply_depot_stockpile(depot, 0.075f);
		military::update_army_supply_cache(*state);
		auto high_access = military::calculate_army_supply_access_cached(*state, high);
		auto low_access = military::calculate_army_supply_access_cached(*state, low);
		REQUIRE(high_access.source == depot);
		REQUIRE(high_access.source_is_depot);
		REQUIRE(high_access.depot_delivery_factor == Approx(1.0f));
		REQUIRE(low_access.depot_delivery_factor == Approx(0.0f));

		// Cutting the rear stops incoming flow, but the depot remains a valid
		// source until its already stored stockpile has been consumed.
		state->world.province_adjacency_set_type(rear_edge, province::border::impassible_bit);
		state->world.province_set_supply_depot_stockpile(depot, 1.0f);
		military::update_supply_depots(*state);
		REQUIRE(state->supply_depot_connected_cache[depot.index()] == 0);
		REQUIRE(state->world.province_get_supply_depot_stockpile(depot) == Approx(1.0f));
		REQUIRE(military::calculate_army_supply_access(*state, high).source == depot);

		// Capture invalidates ownership and destroys the old stockpile.
		state->world.province_set_nation_from_province_control(depot, enemy);
		military::update_supply_depots(*state);
		REQUIRE(state->world.province_get_supply_depot_stockpile(depot) == 0.0f);
		REQUIRE_FALSE(state->world.province_get_supply_depot_owner(depot));
	}

	SECTION("depot state and army priority survive saves and affect the MP checksum") {
		auto depot = make_province(owner, owner);
		state->world.province_set_is_supply_depot(depot, true);
		state->world.province_set_supply_depot_owner(depot, owner);
		state->world.province_set_supply_depot_stockpile(depot, 12.5f);
		auto army = state->world.create_army();
		state->world.army_set_supply_priority(army, 2);
		auto checksum = state->get_mp_state_checksum();
		state->world.province_set_supply_depot_stockpile(depot, 12.0f);
		REQUIRE_FALSE(checksum.is_equal(state->get_mp_state_checksum()));
		state->world.province_set_supply_depot_stockpile(depot, 12.5f);
		auto record = state->world.make_serialize_record_store_save();
		std::vector<std::byte> buffer(state->world.serialize_size(record));
		auto write = buffer.data();
		state->world.serialize(write, record);
		auto loaded = std::make_unique<sys::state>();
		dcon::load_record load_record;
		std::byte const* read = buffer.data();
		loaded->world.deserialize(read, buffer.data() + buffer.size(), load_record);
		REQUIRE(loaded->world.province_get_is_supply_depot(depot));
		REQUIRE(loaded->world.province_get_supply_depot_owner(depot) == owner);
		REQUIRE(loaded->world.province_get_supply_depot_stockpile(depot) == Approx(12.5f));
		REQUIRE(loaded->world.army_get_supply_priority(army) == 2);
	}

	auto invalid = military::calculate_army_supply_access(*state, dcon::nation_id{}, source);
	REQUIRE_FALSE(invalid.reachable);
	REQUIRE(invalid.spatial_access == 0.0f);
	REQUIRE(invalid.effective_supply == 0.0f);
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
			if(!neighbor_node.is_in_closed_list && adj_func(other_prov, current_prov, adj)) {
				// check if province check passes (aka province isnt impassable from all directions). If not, add to closed list
				if(prov_func(other_prov)) {
					float new_dist = mod_func(other_prov, current_prov, adj, distance); // to and from province
					float distance_to_neighbor = current_node.distance_covered + new_dist;
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
			if(!neighbor_node.is_in_closed_list && adj_func(other_prov, current_prov, adj)) {
				// check if province check passes (aka province isnt impassable from all directions). If not, add to closed list
				if(prov_func(other_prov)) {
					float new_dist = mod_func(other_prov, current_prov, adj, distance); // to and from province
					float distance_to_neighbor = current_node.distance_covered + new_dist;
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
		auto adj_func = [&](auto to, auto from, auto adj) { return true; };
		auto prov_func = [&](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } };
		auto mod_func = [&](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; };
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
		auto adj_func = [&](auto to, auto from, auto adj) { return true; };
		auto prov_func = [&](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) { return true; } else {return false; } };
		auto mod_func = [](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; };
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
		auto adj_func = [&](auto to, auto from, auto adj) { return true; };
		auto prov_func = [&](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) {
			return true;
		} else {
			return false;
		} };
		auto mod_func = [&](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; };
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
			auto a = province::make_path_to_expression_fast(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
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
		auto adj_func = [&](auto to, auto from, auto adj) { return true; };
		auto prov_func = [&](dcon::province_id to_prov) { if(to_prov.index() < gamestate->province_definitions.first_sea_province.index()) {
			return true;
		} else {
			return false;
		} };
		auto mod_func = [&](auto to_prov, auto from_prov, auto adj, auto dist) { return dist; };
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
			auto a = province::make_path_to_expression_fast(*gamestate, start, adj_func, prov_func, mod_func, end_exp);
			placeholder.push_back(a.size());
		}
		time_end = std::chrono::steady_clock::now();
		net_time = time_end - time_start;
		gamestate->console_log("long path NOT A* ns: " + std::to_string(net_time.count()));

		std::cout << placeholder.size();

		std::this_thread::sleep_for(std::chrono::seconds{ 1 });
	}

}
