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

dcon::rebel_faction_id get_faction_by_type(sys::state& state, dcon::nation_id n, dcon::rebel_type_id r) {
	for(auto f : state.world.nation_get_rebellion_within(n)) {
		if(f.get_rebels().get_type() == r)
			return f.get_rebels().id;
	}
	return dcon::rebel_faction_id{};
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

bool movement_is_valid(sys::state& state, dcon::movement_id m) {
	auto i = state.world.movement_get_associated_issue_option(m);

	auto nation_within = state.world.movement_get_nation_from_movement_within(m);
	if(i) {
		auto parent_issue = state.world.issue_option_get_parent_issue(i);
		auto current_setting = state.world.nation_get_issues(nation_within, parent_issue);
		if(i == current_setting)
			return false;
		if(state.world.issue_get_is_next_step_only(parent_issue)) {
			if(i.id.index() != current_setting.id.index() - 1 && i.id.index() != current_setting.id.index() + 1)
				return false;
		}
		auto allow = state.world.issue_option_get_allow(i);
		if(allow && !trigger::evaluate_trigger(state, allow, trigger::to_generic(nation_within), trigger::to_generic(nation_within), 0))
			return false;

		return true;
	}

	auto t = state.world.movement_get_associated_independence(m);
	if(t) {
		for(auto p : state.world.nation_get_province_ownership(nation_within)) {
			if(state.world.get_core_by_prov_tag_key(p.get_province(), t))
				return true;
		}
		return false;
	}

	return false;
}

void update_pop_movement_membership(sys::state& state) {
	state.world.for_each_pop([&](dcon::pop_id p) {
		// - Slave pops cannot belong to a movement
		if(state.world.pop_get_poptype(p) == state.culture_definitions.slaves)
			return;
		// pops in rebel factions don't join movements
		if(state.world.pop_get_pop_rebellion_membership(p))
			return;
		
		auto pop_location = state.world.pop_get_province_from_pop_location(p);
		//pops in colonial provinces don't join movements
		if(state.world.province_get_is_colonial(pop_location))
			return;

		auto existing_movement = state.world.pop_get_pop_movement_membership(p);
		auto mil = state.world.pop_get_militancy(p);

		// -Pops with define : MIL_TO_JOIN_REBEL or greater militancy cannot join a movement
		if(mil >= state.defines.mil_to_join_rebel) {
			if(existing_movement)
				state.world.delete_pop_movement_membership(existing_movement);
			return;
		}
		if(existing_movement) {
			auto i = state.world.movement_get_associated_issue_option(state.world.pop_movement_membership_get_movement(existing_movement));
			if(i) {
				auto support = state.world.pop_get_demographics(p, pop_demographics::to_key(state, i)) / state.world.pop_get_size(p);
				if(support * 100.0f < state.defines.issue_movement_leave_limit) {
					// If the pop's support of the issue for an issue-based movement drops below define:ISSUE_MOVEMENT_LEAVE_LIMIT the pop will leave the movement. 
					state.world.delete_pop_movement_membership(existing_movement);
					return;
				}
			} else if(mil < state.defines.nationalist_movement_mil_cap) {
				// If the pop's militancy falls below define:NATIONALIST_MOVEMENT_MIL_CAP, the pop will leave an independence movement.
				state.world.delete_pop_movement_membership(existing_movement);
				return;
			}
			// pop still remains in movement, no more work to do
			return;
		}

		auto owner = nations::owner_of_pop(state, p);

		auto con = state.world.pop_get_consciousness(p);
		auto lit = state.world.pop_get_literacy(p);

		// a pop with a consciousness of at least 1.5 or a literacy of at least 0.25 may join a movement
		if(con >= 1.5 || lit >= 0.25) {
			/*
			- If there are one or more issues that the pop supports by at least define:ISSUE_MOVEMENT_JOIN_LIMIT, then the pop has a chance to join an issue-based movement at probability: issue-support x 9 x define:MOVEMENT_LIT_FACTOR x pop-literacy + issue-support x 9 x define:MOVEMENT_CON_FACTOR x pop-consciousness
			*/
			dcon::issue_option_id max_option;
			float max_support = 0;
			state.world.for_each_issue_option([&](dcon::issue_option_id io) {
				auto parent = state.world.issue_option_get_parent_issue(io);
				auto co = state.world.nation_get_issues(owner, parent);
				auto allow = state.world.issue_option_get_allow(io);
				if(co != io
					&& (state.world.issue_get_is_next_step_only(parent) == false || co.id.index() + 1 == io.index() || co.id.index() -1 == io.index())
					&& (!allow || trigger::evaluate_trigger(state, allow, trigger::to_generic(owner), trigger::to_generic(owner), 0))) {
					auto sup = state.world.pop_get_demographics(p, pop_demographics::to_key(state, io)) / state.world.pop_get_size(p);
					if(sup * 100.0f >= state.defines.issue_movement_join_limit && sup > max_support) {
						max_option = io;
						max_support = sup;
					}
				}
			});

			if(max_option) {

				return;
			}

			if(!state.world.pop_get_is_primary_or_accepted_culture(p) && mil >= state.defines.nationalist_movement_mil_cap) {
				/*
				- If there are no valid issues, the pop has a militancy of at least define:NATIONALIST_MOVEMENT_MIL_CAP, does not have the primary culture of the nation it is in, and does have the primary culture of some core in its province, then it has a chance (20% ?) of joining an independence movement for such a core.
				*/
				auto pop_culture = state.world.pop_get_culture(p);
				for(auto c : state.world.province_get_core(pop_location)) {
					if(c.get_identity().get_primary_culture() == pop_culture) {
						auto existing_mov = get_movement_by_independence(state, owner, c.get_identity());
						if(existing_mov) {
							state.world.try_create_pop_movement_membership(p, existing_mov);
						} else {
							auto new_mov = fatten(state.world, state.world.create_movement());
							new_mov.set_associated_independence(c.get_identity());
							state.world.try_create_movement_within(new_mov, owner);
							state.world.try_create_pop_movement_membership(p, new_mov);
						}
						return;
					}
				}
			}
		}
	});
}

void update_movements(sys::state& state) { // updates cached values and then possibly turns movements into rebels
	// IMPORTANT: we count down here so that we can delete as we go, compacting from the end
	for(auto last = state.world.movement_size(); last-- > 0; ) {
		dcon::movement_id m{ dcon::movement_id::value_base_t(last) };
		if(!movement_is_valid(state, m)) {
			state.world.delete_movement(m);
		}
	}

	update_movement_values(state);
	
	for(auto last = state.world.movement_size(); last-- > 0; ) {
		dcon::movement_id m{ dcon::movement_id::value_base_t(last) };
		if(state.world.movement_get_radicalism(m) >= 100.0f) {
			turn_movement_into_rebels(state, m);
		}
	}
}

bool sphere_member_has_ongoing_revolt(sys::state& state, dcon::nation_id n) {
	// TODO
	return false;
}

}
