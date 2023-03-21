#include "rebels.hpp"
#include "system_state.hpp"

namespace rebel {

dcon::movement_id get_movement_by_position(sys::state& state, dcon::nation_id n, dcon::issue_option_id o) {
	for(auto m : state.world.nation_get_movement_within(n)) {
		if(m.get_movement().get_associated_issue_option() == o)
			return m.get_movement().id;
	}
	return dcon::movement_id{};
}
dcon::movement_id get_movement_by_independence(sys::state& state, dcon::nation_id n, dcon::national_identity_id i) {
	for(auto m : state.world.nation_get_movement_within(n)) {
		if(m.get_movement().get_associated_independence() == i)
			return m.get_movement().id;
	}
	return dcon::movement_id{};
}

void update_movement_values(sys::state& state) { // simply updates cached values

	state.world.execute_serial_over_movement([&](auto ids) {
		state.world.movement_set_pop_support(ids, ve::fp_vector());
	});
	state.world.for_each_pop([&](dcon::pop_id p) {
		if(auto m = state.world.pop_get_movement_from_pop_movement_membership(p); m) {
			state.world.movement_get_pop_support(m) += state.world.pop_get_size(p);
		}
	});

	static ve::vectorizable_buffer<float, dcon::nation_id> nation_reform_count(uint32_t(1));
	static uint32_t old_count = 1;

	auto new_count = state.world.nation_size();
	if(new_count > old_count) {
		nation_reform_count = state.world.nation_make_vectorizable_float_buffer();
		old_count = new_count;
	}

	state.world.execute_serial_over_nation([&](auto host_nations) {
		ve::fp_vector reform_totals;
		for(auto preform : state.culture_definitions.political_issues) {
			ve::tagged_vector<dcon::issue_option_id> creform = state.world.nation_get_issues(host_nations, preform);
			auto base_reform = state.world.issue_get_options(preform)[0].index() + 1;
			reform_totals = reform_totals + ve::to_float(ve::int_vector(creform.to_original_values()) - base_reform);
		}
		nation_reform_count.set(host_nations, reform_totals);
	});

	state.world.execute_serial_over_movement([&](auto ids) {
		/*
		- Movements can accumulate radicalism, but only in civ nations. Internally we may represent radicalism as two values, a radicalism value and transient radicalism. Every day the radicalism value is computed as: define:MOVEMENT_RADICALISM_BASE + the movements current transient radicalism + number-of-political-reforms-passed-in-the-country-over-base x define:MOVEMENT_RADICALISM_PASSED_REFORM_EFFECT + radicalism-of-the-nation's-primary-culture + maximum-nationalism-value-in-any-province x define:MOVEMENT_RADICALISM_NATIONALISM_FACTOR + define:POPULATION_MOVEMENT_RADICAL_FACTOR x movement-support / nation's-non-colonial-population.
		*/
		auto host_nations = state.world.movement_get_nation_from_movement_within(ids);

		auto host_max_nationalism = ve::apply([&](dcon::nation_id n, dcon::movement_id m) {
			if(auto iid = state.world.movement_get_associated_independence(m); iid) {
				float nmax = 0.0f;
				for(auto c : state.world.national_identity_get_core(iid)) {
					if(c.get_province().get_nation_from_province_ownership() == n) {
						nmax = std::max(nmax, c.get_province().get_nationalism());
					}
				}
				return nmax;
			} else {
				return 0.0f;
			}
		}, host_nations, ids);
		auto host_pculture = state.world.nation_get_primary_culture(host_nations);
		auto host_cradicalism = ve::to_float(state.world.culture_get_radicalism(host_pculture));

		auto new_radicalism =
			state.world.movement_get_transient_radicalism(ids)
			+ state.defines.movement_radicalism_base
			+ nation_reform_count.get(host_nations) * state.defines.movement_radicalism_passed_reform_effect
			+ host_cradicalism
			+ host_max_nationalism * state.defines.movement_radicalism_nationalism_factor
			+ state.defines.population_movement_radical_factor * state.world.movement_get_pop_support(ids) / state.world.nation_get_non_colonial_population(host_nations);
		state.world.movement_set_radicalism(ids, new_radicalism);
	});
}

void add_pop_to_movement(sys::state& state, dcon::pop_id p, dcon::movement_id m) {
	remove_pop_from_movement(state, p);
	state.world.movement_get_pop_support(m) += state.world.pop_get_size(p);
	state.world.try_create_pop_movement_membership(p, m);
}
void remove_pop_from_movement(sys::state& state, dcon::pop_id p) {
	auto prior_movement = state.world.pop_get_movement_from_pop_movement_membership(p);
	if(prior_movement) {
		state.world.movement_get_pop_support(prior_movement) -= state.world.pop_get_size(p);
		state.world.delete_pop_movement_membership(state.world.pop_get_pop_movement_membership(p));
	}
}

void suppress_movement(sys::state& state, dcon::nation_id n, dcon::movement_id m) {
	/*
	- When a movement is suppressed:
	Reduce the suppression points for the nation by: if define:POPULATION_SUPPRESSION_FACTOR is zero, movement radicalism + 1, otherwise the greater of movement-radicalism + 1 and movement-radicalism x movement-support / define:POPULATION_SUPPRESSION_FACTOR, to a minimum of zero
	Increase the transient radicalism of the movement by: define:SUPPRESSION_RADICALISM_HIT
	Increase the consciousness of all pops that were in the movement by 1 and remove them from it.
	*/
	auto radicalism = state.world.movement_get_radicalism(m);
	auto spoints = state.defines.population_suppression_factor > 0
		? radicalism + 1.0f
		: std::max(radicalism + 1.0f, radicalism * state.world.movement_get_pop_support(m) / state.defines.population_suppression_factor);
	state.world.movement_get_transient_radicalism(m) += state.defines.suppression_radicalisation_hit;
	for(auto p : state.world.movement_get_pop_movement_membership(m)) {
		auto old_con = p.get_pop().get_consciousness();
		p.get_pop().set_consciousness(std::min(old_con + 1.0f, 10.0f));
	}
	state.world.movement_remove_all_pop_movement_membership(m);
}

void turn_movement_into_rebels(sys::state& state, dcon::movement_id m) {
	/*
	- When the radicalism value for a movement reaches 100, pops get removed from the movement and added to a rebel faction. Those pops have their militancy increased to a minimum of define:MIL_ON_REB_MOVE. See below for determining which rebel faction the pop joins.
	*/
	// TODO

	state.world.delete_movement(m);
}

void update_movements(sys::state& state) { // updates cached values and then possibly turns movements into rebels
	update_movement_values(state);

	// IMPORTANT: we count down here so that we can delete as we go, compacting from the end
	for(auto last = state.world.movement_size(); last-- > 0; ) {
		dcon::movement_id m{ dcon::movement_id::value_base_t(last) };
		if(state.world.movement_get_radicalism(m) >= 100.0f) {
			turn_movement_into_rebels(state, m);
		}
	}
}

}
