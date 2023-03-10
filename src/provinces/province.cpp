#include "province.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include <vector>

namespace province {

template<typename T>
auto is_overseas(sys::state const& state, T ids) {
	auto owners = state.world.province_get_nation_from_province_ownership(ids);
	auto owner_cap = state.world.nation_get_capital(owners);
	return (state.world.province_get_continent(ids) != state.world.province_get_continent(owner_cap))
		&& (state.world.province_get_connected_region_id(ids) != state.world.province_get_connected_region_id(owner_cap));
}

template<typename F>
void for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	for(int32_t i = 0; i < last; ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		func(pid);
	}
}

template<typename F>
void ve_for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	ve::execute_serial<dcon::province_id>(uint32_t(last), func);
}

template<typename F>
void for_each_sea_province(sys::state& state, F const& func) {
	int32_t first = state.province_definitions.first_sea_province.index();
	for(int32_t i = first; i < int32_t(state.world.province_size()); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		func(pid);
	}
}

bool nations_are_adjacent(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	auto it = state.world.get_nation_adjacency_by_nation_adjacency_pair(a, b);
	return bool(it);
}

void update_connected_regions(sys::state& state) {
	if(!state.adjacency_data_out_of_date)
		return;

	state.adjacency_data_out_of_date = false;

	state.world.nation_adjacency_resize(0);

	state.world.for_each_province([&](dcon::province_id id) {
		state.world.province_set_connected_region_id(id, 0);
	});
	// TODO get a better allocator
	static std::vector<dcon::province_id> to_fill_list;
	uint16_t current_fill_id = 0;

	to_fill_list.reserve(state.world.province_size());

	for(int32_t i = state.province_definitions.first_sea_province.index(); i-- > 0; ) {
		dcon::province_id id{ dcon::province_id ::value_base_t(i) };
		if(state.world.province_get_connected_region_id(id) == 0) {
			++current_fill_id;
			
			to_fill_list.push_back(id);

			while(!to_fill_list.empty()) {
				auto current_id = to_fill_list.back();
				to_fill_list.pop_back();

				state.world.province_set_connected_region_id(current_id, current_fill_id);
				for(auto rel : state.world.province_get_province_adjacency(current_id)) {
					if((rel.get_type() & (province::border::coastal_bit | province::border::impassible_bit)) == 0) { // not entering sea, not impassible
						auto owner_a = rel.get_connected_provinces(0).get_nation_from_province_ownership();
						auto owner_b = rel.get_connected_provinces(1).get_nation_from_province_ownership();
						if(owner_a == owner_b) { // both have the same owner
							if(rel.get_connected_provinces(0).get_connected_region_id() == 0)
								to_fill_list.push_back(rel.get_connected_provinces(0));
							if(rel.get_connected_provinces(1).get_connected_region_id() == 0)
								to_fill_list.push_back(rel.get_connected_provinces(1));
						} else {
							state.world.try_create_nation_adjacency(owner_a, owner_b);
						}
					}
				}
			}

			to_fill_list.clear();
		}
	}
}


void restore_unsaved_values(sys::state& state) {
	//clear nation values that cache province information

	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_owned_province_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_province_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_blockaded(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_rebel_controlled(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_rebel_controlled_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_ports(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_central_crime_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_total_ports(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_occupied_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_owned_state_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_is_colonial_nation(ids, ve::mask_vector());
	});
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		[&]() {
			for(auto adj : state.world.province_get_province_adjacency(pid)) {
				if((state.world.province_adjacency_get_type(adj) & province::border::coastal_bit) != 0) {
					state.world.province_set_is_coast(pid, true);
					return;
				}
			}
		}();
		auto owner = state.world.province_get_nation_from_province_ownership(pid);
		if(owner) {
			state.world.nation_get_owned_province_count(owner) += uint16_t(1);

			bool reb_controlled = bool(state.world.province_get_rebel_faction_from_province_rebel_control(pid));

			if(reb_controlled) {
				state.world.nation_get_rebel_controlled_count(owner) += uint16_t(1);
			}
			if(state.world.province_get_is_coast(pid)) {
				state.world.nation_get_total_ports(owner) += uint16_t(1);
			}
			if(auto c = state.world.province_get_nation_from_province_control(pid); bool(c) && c != owner) {
				state.world.nation_get_occupied_count(owner) += uint16_t(1);
			}
			if(state.world.province_get_is_colonial(pid)) {
				state.world.nation_set_is_colonial_nation(owner, true);
			}
			if(!is_overseas(state, pid)) {
				state.world.nation_get_central_province_count(owner) += uint16_t(1);

				if(military::province_is_blockaded(state, pid)) {
					state.world.nation_get_central_blockaded(owner) += uint16_t(1);
				}
				if(state.world.province_get_is_coast(pid)) {
					state.world.nation_get_central_ports(owner) += uint16_t(1);
				}
				if(reb_controlled) {
					state.world.nation_get_central_rebel_controlled(owner) += uint16_t(1);
				}
				if(state.world.province_get_crime(pid)) {
					state.world.nation_get_central_crime_count(owner) += uint16_t(1);
				}
			}
		}
	}
	state.world.for_each_state_instance([&](dcon::state_instance_id s) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(s);
		state.world.nation_get_owned_state_count(owner) += uint16_t(1);
	});
}

float monthly_net_pop_growth(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float monthly_net_pop_promotion_and_demotion(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float monthly_net_pop_internal_migration(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float monthly_net_pop_external_migration(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float rgo_maximum_employment(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float rgo_employment(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float rgo_income(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float rgo_production_quantity(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float crime_fighting_efficiency(sys::state& state, dcon::province_id id) {
	// TODO
	return 0.0f;
}
float state_admin_efficiency(sys::state& state, dcon::state_instance_id id) {
	// TODO
	return 0.0f;
}

}
