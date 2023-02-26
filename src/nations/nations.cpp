#include "nations.hpp"
#include "system_state.hpp"

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

