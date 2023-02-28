#include "nations.hpp"
#include "system_state.hpp"
#include "ve_scalar_extensions.hpp"

namespace nations {

// returns whether a culture is on the accepted list OR is the primary culture
template<typename T, typename U>
auto nation_accepts_culture(sys::state const& state, T ids, U cul_ids) {
	auto is_accepted = ve::apply([&state](dcon::nation_id n, dcon::culture_id c) {
		if(n)
			return state.world.nation_get_accepted_cultures(n).contains(c);
		else
			return false;
	}, ids, cul_ids);
	return (state.world.nation_get_primary_culture(ids) == cul_ids) || is_accepted;
}

template<typename T>
auto primary_culture_group(sys::state const& state, T ids) {
	auto cultures = state.world.nation_get_primary_culture(ids);
	return state.world.culture_get_group_from_culture_group_membership(cultures);
}

template<typename T>
auto owner_of_pop(sys::state const& state, T pop_ids) {
	auto location = state.world.pop_get_province_from_pop_location(pop_ids);
	return state.world.province_get_nation_from_province_ownership(location);
}

template<typename T>
auto central_reb_controlled_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_province_count(ids));
	auto reb_count = ve::to_float(state.world.nation_get_central_rebel_controlled(ids));
	return ve::select(cpc != 0.0f, reb_count / cpc, decltype(cpc)());
}

template<typename T>
auto central_blockaded_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_ports(ids));
	auto b_count = ve::to_float(state.world.nation_get_central_blockaded(ids));
	return ve::select(cpc != 0.0f, b_count / cpc, decltype(cpc)());
}

template<typename T>
auto central_has_crime_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_province_count(ids));
	auto crim_count = ve::to_float(state.world.nation_get_central_crime_count(ids));
	return ve::select(cpc != 0.0f, crim_count / cpc, decltype(cpc)());
}

void update_national_rankings(sys::state& state) {
	if(!state.national_rankings_out_of_date)
		return;

	state.national_rankings_out_of_date = false;

	// do something to update the rankings
}

void restore_unsaved_values(sys::state& state) {
	state.world.for_each_gp_relationship([&](dcon::gp_relationship_id rel) {
		if((influence::level_mask & state.world.gp_relationship_get_status(rel)) == influence::level_in_sphere) {
			auto t = state.world.gp_relationship_get_influence_target(rel);
			auto gp = state.world.gp_relationship_get_great_power(rel);
			state.world.nation_set_in_sphere_of(t, gp);
		}
	});

	state.world.for_each_state_instance([&](dcon::state_instance_id sid) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(sid);
		auto base_state = state.world.state_instance_get_definition(sid);
		for(auto mprov : state.world.state_definition_get_abstract_state_membership(base_state)) {
			if(mprov.get_province().get_nation_from_province_ownership() == owner) {
				mprov.get_province().set_state_membership(sid);
			}
		}
	});

	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_allies_count(ids, ve::int_vector());
	});
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_vassals_count(ids, ve::int_vector());
	});

	state.world.for_each_diplomatic_relation([&](dcon::diplomatic_relation_id id) {
		if(state.world.diplomatic_relation_get_are_allied(id)) {
			state.world.nation_get_allies_count(state.world.diplomatic_relation_get_related_nations(id,0)) += uint16_t(1);
			state.world.nation_get_allies_count(state.world.diplomatic_relation_get_related_nations(id, 1)) += uint16_t(1);
		}
	});
	state.world.for_each_nation([&](dcon::nation_id n) {
		int32_t total = 0;
		for(auto v : state.world.nation_get_overlord_as_ruler(n)) {
			++total;
		}
		state.world.nation_set_vassals_count(n, uint16_t(total));
	});
}

void generate_initial_state_instances(sys::state& state) {
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };
		auto owner = state.world.province_get_nation_from_province_ownership(pid);
		if(owner && !(state.world.province_get_state_membership(pid))) {
			auto state_instance = fatten(state.world, state.world.create_state_instance());
			auto abstract_state = state.world.province_get_state_from_abstract_state_membership(pid);

			state_instance.set_definition(abstract_state);
			state_instance.set_capital(pid);
			state.world.force_create_state_ownership(state_instance, owner);

			for(auto mprov : state.world.state_definition_get_abstract_state_membership(abstract_state)) {
				if(mprov.get_province().get_nation_from_province_ownership() == owner) {
					mprov.get_province().set_state_membership(state_instance);
				}
			}
		}
	}
}

}

