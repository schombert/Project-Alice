#include "rebels.hpp"
#include "system_state.hpp"
#include "triggers.hpp"
#include "effects.hpp"
#include "politics.hpp"

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

	state.world.execute_serial_over_movement([&](auto ids) { state.world.movement_set_pop_support(ids, ve::fp_vector()); });
	state.world.for_each_pop([&](dcon::pop_id p) {
		if(auto m = state.world.pop_get_movement_from_pop_movement_membership(p); m) {
			auto i = state.world.movement_get_associated_issue_option(m);
			state.world.movement_get_pop_support(m) += state.world.pop_get_size(p) * (i ? state.world.pop_get_demographics(p, pop_demographics::to_key(state, i)) : 1.0f);
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
		- Movements can accumulate radicalism, but only in civ nations. Internally we may represent radicalism as two values, a
		radicalism value and transient radicalism. Every day the radicalism value is computed as: define:MOVEMENT_RADICALISM_BASE
		+ the movements current transient radicalism + number-of-political-reforms-passed-in-the-country-over-base x
		define:MOVEMENT_RADICALISM_PASSED_REFORM_EFFECT + radicalism-of-the-nation's-primary-culture +
		maximum-nationalism-value-in-any-province x define:MOVEMENT_RADICALISM_NATIONALISM_FACTOR +
		define:POPULATION_MOVEMENT_RADICAL_FACTOR x movement-support / nation's-non-colonial-population.
		*/
		auto host_nations = state.world.movement_get_nation_from_movement_within(ids);

		auto host_max_nationalism = ve::apply(
				[&](dcon::nation_id n, dcon::movement_id m) {
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
				},
				host_nations, ids);
		auto host_pculture = state.world.nation_get_primary_culture(host_nations);
		auto host_cradicalism = ve::to_float(state.world.culture_get_radicalism(host_pculture));

		auto ref_effect = nation_reform_count.get(host_nations) * state.defines.movement_radicalism_passed_reform_effect;
		auto nat_effect = host_max_nationalism * state.defines.movement_radicalism_nationalism_factor;
		auto mov_support = state.world.movement_get_pop_support(ids);
		auto non_c_pop = state.world.nation_get_non_colonial_population(host_nations);
		auto support_effect = state.defines.population_movement_radical_factor * mov_support / non_c_pop;

		auto new_radicalism = state.world.movement_get_transient_radicalism(ids) + state.defines.movement_radicalism_base
			+ ref_effect + host_cradicalism + nat_effect + support_effect;
		state.world.movement_set_radicalism(ids, new_radicalism);
	});
}

void add_pop_to_movement(sys::state& state, dcon::pop_id p, dcon::movement_id m) {
	remove_pop_from_movement(state, p);
	auto i = state.world.movement_get_associated_issue_option(m);
	state.world.movement_get_pop_support(m) += state.world.pop_get_size(p) * (i ? state.world.pop_get_demographics(p, pop_demographics::to_key(state, i)) : 1.0f);
	state.world.try_create_pop_movement_membership(p, m);
}
void remove_pop_from_movement(sys::state& state, dcon::pop_id p) {
	auto prior_movement = state.world.pop_get_movement_from_pop_movement_membership(p);
	if(prior_movement) {
		auto i = state.world.movement_get_associated_issue_option(prior_movement);
		state.world.movement_get_pop_support(prior_movement) -= state.world.pop_get_size(p) * (i ? state.world.pop_get_demographics(p, pop_demographics::to_key(state, i)) : 1.0f);
		state.world.delete_pop_movement_membership(state.world.pop_get_pop_movement_membership(p));
	}
}

void suppress_movement(sys::state& state, dcon::nation_id n, dcon::movement_id m) {
	/*
	- When a movement is suppressed:
	Increase the transient radicalism of the movement by: define:SUPPRESSION_RADICALISM_HIT
	Set the consciousness of all pops that were in the movement to 1 and remove them from it.
	*/
	state.world.movement_get_transient_radicalism(m) += state.defines.suppression_radicalisation_hit;
	for(auto p : state.world.movement_get_pop_movement_membership(m)) {
		auto old_con = p.get_pop().get_consciousness();
		p.get_pop().set_consciousness(1.0f);
	}
	state.world.movement_remove_all_pop_movement_membership(m);
}

void turn_movement_into_rebels(sys::state& state, dcon::movement_id m) {
	/*
	- When the radicalism value for a movement reaches 100, pops get removed from the movement and added to a rebel faction. Those
	pops have their militancy increased to a minimum of define:MIL_ON_REB_MOVE. See below for determining which rebel faction the
	pop joins.
	*/

	for(auto p : state.world.movement_get_pop_movement_membership(m)) {
		p.get_pop().set_militancy(std::max(p.get_pop().get_militancy(), state.defines.mil_on_reb_move));
	}
	// and then they will automatically be picked up by updating rebels

	state.world.delete_movement(m);
}

bool issue_is_valid_for_movement(sys::state& state, dcon::nation_id nation_within, dcon::issue_option_id i) {
	auto parent_issue = state.world.issue_option_get_parent_issue(i);
	auto current_setting = state.world.nation_get_issues(nation_within, parent_issue);
	if(i == current_setting)
		return false;
	if(state.world.issue_get_is_next_step_only(parent_issue)) {
		if(i.index() != current_setting.id.index() - 1 && i.index() != current_setting.id.index() + 1)
			return false;
	}
	auto allow = state.world.issue_option_get_allow(i);
	if(allow && !trigger::evaluate(state, allow, trigger::to_generic(nation_within), trigger::to_generic(nation_within), 0))
		return false;

	return true;
}

bool movement_is_valid(sys::state& state, dcon::movement_id m) {
	auto i = state.world.movement_get_associated_issue_option(m);

	auto nation_within = state.world.movement_get_nation_from_movement_within(m);
	if(i) {
		return issue_is_valid_for_movement(state, nation_within, i);
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
		auto owner = nations::owner_of_pop(state, p);
		// pops not in a nation can't be in a movement
		if(!owner)
			return;

		// - Slave pops cannot belong to a movement
		if(state.world.pop_get_poptype(p) == state.culture_definitions.slaves)
			return;
		// pops in rebel factions don't join movements
		if(state.world.pop_get_rebel_faction_from_pop_rebellion_membership(p))
			return;

		auto pop_location = state.world.pop_get_province_from_pop_location(p);
		// pops in colonial provinces don't join movements
		if(state.world.province_get_is_colonial(pop_location))
			return;

		auto existing_movement = state.world.pop_get_movement_from_pop_movement_membership(p);
		auto mil = state.world.pop_get_militancy(p);

		// -Pops with define : MIL_TO_JOIN_REBEL or greater militancy cannot join a movement
		if(mil >= state.defines.mil_to_join_rebel) {
			remove_pop_from_movement(state, p);
			return;
		}
		if(existing_movement) {
			auto i =
					state.world.movement_get_associated_issue_option(existing_movement);
			if(i) {
				auto support = state.world.pop_get_demographics(p, pop_demographics::to_key(state, i));
				if(support * 100.0f < state.defines.issue_movement_leave_limit) {
					// If the pop's support of the issue for an issue-based movement drops below define:ISSUE_MOVEMENT_LEAVE_LIMIT
					// the pop will leave the movement.
					remove_pop_from_movement(state, p);
					return;
				}
			} else if(mil < state.defines.nationalist_movement_mil_cap) {
				// If the pop's militancy falls below define:NATIONALIST_MOVEMENT_MIL_CAP, the pop will leave an independence
				// movement.
				remove_pop_from_movement(state, p);
				return;
			}
			// pop still remains in movement, no more work to do
			return;
		}

		auto con = state.world.pop_get_consciousness(p);
		auto lit = state.world.pop_get_literacy(p);

		// a pop with a consciousness of at least 1.5 or a literacy of at least 0.25 may join a movement
		if(con >= 1.5f || lit >= 0.25f) {
			/*
			- If there are one or more issues that the pop supports by at least define:ISSUE_MOVEMENT_JOIN_LIMIT, then the pop has
			a chance to join an issue-based movement at probability: issue-support x 9 x define:MOVEMENT_LIT_FACTOR x pop-literacy
			+ issue-support x 9 x define:MOVEMENT_CON_FACTOR x pop-consciousness
			*/
			dcon::issue_option_id max_option;
			float max_support = 0;
			state.world.for_each_issue_option([&](dcon::issue_option_id io) {
				auto parent = state.world.issue_option_get_parent_issue(io);
				auto co = state.world.nation_get_issues(owner, parent);
				auto allow = state.world.issue_option_get_allow(io);
				if(co != io && (state.world.issue_get_issue_type(parent) == uint8_t(culture::issue_type::social) || state.world.issue_get_issue_type(parent) == uint8_t(culture::issue_type::political))) { // filter out currently active issue
					auto sup = state.world.pop_get_demographics(p, pop_demographics::to_key(state, io));
					if(sup * 100.0f >= state.defines.issue_movement_join_limit && sup > max_support) { // filter out -- above limit thersholds
						/*
						then the pop has a chance to join an issue-based movement at probability: issue-support x 9 x define:MOVEMENT_LIT_FACTOR x pop-literacy + issue-support x 9 x define:MOVEMENT_CON_FACTOR x pop-consciousness
						*/

						// probability test
						auto fp_prob = 9.0f * sup * (state.defines.movement_lit_factor * lit + state.defines.movement_con_factor * con);
						auto rvalue = float(uint32_t(rng::get_random(state, (p.value << 3) ^ io.index()) & 0xFFFF)) / float(0x10000);
						if(rvalue < fp_prob) {

							// is this issue possible to get by law?
							if((state.world.issue_get_is_next_step_only(parent) == false || co.id.index() + 1 == io.index())
								&& (!allow || trigger::evaluate(state, allow, trigger::to_generic(owner), trigger::to_generic(owner), 0))) {

								max_option = io;
								max_support = sup;
							}
						}
					}
				}
			});

			if(max_option) {
				if(auto m = get_movement_by_position(state, owner, max_option); m) {
					add_pop_to_movement(state, p, m);
				} else if(issue_is_valid_for_movement(state, owner, max_option)) {
					auto new_movement = fatten(state.world, state.world.create_movement());
					new_movement.set_associated_issue_option(max_option);
					state.world.try_create_movement_within(new_movement, owner);
					add_pop_to_movement(state, p, new_movement);
				}
			} else if(!state.world.pop_get_is_primary_or_accepted_culture(p) && mil >= state.defines.nationalist_movement_mil_cap) {
				/*
				- If there are no valid issues, the pop has a militancy of at least define:NATIONALIST_MOVEMENT_MIL_CAP, does not
				have the primary culture of the nation it is in, and does have the primary culture of some core in its province,
				then it has a chance (20% ?) of joining an independence movement for such a core.
				*/
				if(rng::reduce(uint32_t(rng::get_random(state, p.value)), 10) != 0) {
					return; // exit out of considering this pop
				}
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
						break;
					}
				}
			}
		}
	});
}

void update_movements(sys::state& state) { // updates cached values and then possibly turns movements into rebels
	update_pop_movement_membership(state);

	// IMPORTANT: we count down here so that we can delete as we go, compacting from the end
	for(auto last = state.world.movement_size(); last-- > 0;) {
		dcon::movement_id m{dcon::movement_id::value_base_t(last)};
		if(!movement_is_valid(state, m)) {
			state.world.delete_movement(m);
		}
	}

	update_movement_values(state);

	for(auto last = state.world.movement_size(); last-- > 0;) {
		dcon::movement_id m{dcon::movement_id::value_base_t(last)};
		if(state.world.movement_get_radicalism(m) >= 100.0f) {
			turn_movement_into_rebels(state, m);
		}
	}
}

void remove_pop_from_rebel_faction(sys::state& state, dcon::pop_id p) {
	if(auto m = state.world.pop_get_pop_rebellion_membership(p); m) {
		auto fac = state.world.pop_rebellion_membership_get_rebel_faction(m);
		state.world.rebel_faction_get_possible_regiments(fac) -=
				int32_t(state.world.pop_get_size(p) / state.defines.pop_size_per_regiment);
		state.world.delete_pop_rebellion_membership(m);
	}
}
void add_pop_to_rebel_faction(sys::state& state, dcon::pop_id p, dcon::rebel_faction_id m) {
	remove_pop_from_rebel_faction(state, p);
	state.world.try_create_pop_rebellion_membership(p, m);
	state.world.rebel_faction_get_possible_regiments(m) +=
			int32_t(state.world.pop_get_size(p) / state.defines.pop_size_per_regiment);
}

bool rebel_faction_is_valid(sys::state& state, dcon::rebel_faction_id m) {
	/*
	- A rebel faction with no pops in it will disband
	- Rebel factions whose independence country exists will disband (different for defection rebels?)
	- Pan nationalists inside their union country will disband
	- Any pops belonging to a rebel faction that disbands have their militancy reduced to 0
	*/

	auto pops = state.world.rebel_faction_get_pop_rebellion_membership(m);
	if(pops.begin() == pops.end())
		return false;

	auto type = fatten(state.world, state.world.rebel_faction_get_type(m));

	auto tag = state.world.rebel_faction_get_defection_target(m);
	auto within = fatten(state.world, m).get_rebellion_within().get_ruler();
	if(within.get_identity_from_identity_holder() == tag)
		return false;

	return true;
}

bool pop_is_compatible_with_rebel_faction(sys::state& state, dcon::pop_id p, dcon::rebel_faction_id t) {
	/*
	- Faction compatibility: a pop will not join a faction that it is excluded from based on its culture, culture group, religion,
	or ideology (here it is the dominant ideology of the pop that matters). There is also some logic for determining if a pop is
	compatible with a national identity for independence. I don't think it is worth trying to imitate the logic of the base game
	here. Instead I will go with: pop is not an accepted culture and either its primary culture is associated with that identity
	*or* there is no core in the province associated with its primary identity.
	*/
	auto fac = fatten(state.world, t);
	auto pop = fatten(state.world, p);

	if(fac.get_primary_culture() && fac.get_primary_culture() != pop.get_culture())
		return false;
	if(fac.get_religion() && fac.get_religion() != pop.get_religion())
		return false;
	if(fac.get_primary_culture_group() &&
			fac.get_primary_culture_group() != pop.get_culture().get_group_from_culture_group_membership())
		return false;
	if(fac.get_type().get_ideology() && fac.get_type().get_ideology_restriction() && fac.get_type().get_ideology() != pop.get_dominant_ideology())
		return false;
	if(fac.get_defection_target()) {
		if(pop.get_is_primary_or_accepted_culture())
			return false;
		if(pop.get_culture() == fac.get_defection_target().get_primary_culture())
			return true;
		for(auto core : pop.get_province_from_pop_location().get_core()) {
			if(core.get_identity().get_primary_culture() == pop.get_culture())
				return false;
		}
		return true;
	}
	return true;
}

bool pop_is_compatible_with_rebel_type(sys::state& state, dcon::pop_id p, dcon::rebel_type_id t) {
	auto fac = fatten(state.world, t);
	auto pop = fatten(state.world, p);

	if(fac.get_ideology() && fac.get_ideology() != pop.get_dominant_ideology())
		return false;
	if(fac.get_independence() != 0 || fac.get_defection() != 0) {
		if(fac.get_independence() == uint8_t(culture::rebel_independence::pan_nationalist) ||
				fac.get_defection() == uint8_t(culture::rebel_defection::pan_nationalist)) {
			if(pop.get_is_primary_or_accepted_culture())
				return true;
		} else {
			if(pop.get_is_primary_or_accepted_culture())
				return false;
		}
		for(auto core : pop.get_province_from_pop_location().get_core()) {
			if(core.get_identity().get_primary_culture() == pop.get_culture())
				return true;
		}
		return false;
	}
	return true;
}

void update_pop_rebel_membership(sys::state& state) {
	state.world.for_each_pop([&](dcon::pop_id p) {
		auto owner = nations::owner_of_pop(state, p);
		// pops not in a nation can't be in a rebel faction
		if(!owner)
			return;

		auto mil = state.world.pop_get_militancy(p);
		auto existing_faction = state.world.pop_get_rebel_faction_from_pop_rebellion_membership(p);

		// -Pops with define : MIL_TO_JOIN_REBEL will join a rebel_faction
		if(mil >= state.defines.mil_to_join_rebel) {
			if(existing_faction && !pop_is_compatible_with_rebel_faction(state, p, existing_faction)) {
				remove_pop_from_rebel_faction(state, p);
			} else {
				auto prov = state.world.pop_get_province_from_pop_location(p);
				/*
				- A pop in a province sieged or controlled by rebels will join that faction, if the pop is compatible with the
				faction.
				*/

				auto occupying_faction = state.world.province_get_rebel_faction_from_province_rebel_control(prov);
				if(occupying_faction && pop_is_compatible_with_rebel_faction(state, p, occupying_faction)) {
					assert(!bool(state.world.province_get_nation_from_province_control(prov)));
					add_pop_to_rebel_faction(state, p, occupying_faction);
				} else {
					/*
					- Otherwise take all the compatible and possible rebel types. Determine the spawn chance for each of them, by
					taking the *product* of the modifiers. The pop then joins the type with the greatest chance (that's right, it
					isn't really a *chance* at all). If that type has a defection type, it joins the faction with the national
					identity most compatible with it and that type (pan-nationalist go to the union tag, everyone else uses the
					logic I outline below)
					*/
					float greatest_chance = 0.0f;
					dcon::rebel_faction_id f;
					for(auto rf : state.world.nation_get_rebellion_within(owner)) {
						if(pop_is_compatible_with_rebel_faction(state, p, rf.get_rebels())) {
							auto chance = rf.get_rebels().get_type().get_spawn_chance();
							auto eval = trigger::evaluate_multiplicative_modifier(state, chance, trigger::to_generic(p),
									trigger::to_generic(owner), trigger::to_generic(rf.get_rebels().id));
							if(eval > greatest_chance) {
								f = rf.get_rebels();
								greatest_chance = eval;
							}
						}
					}

					dcon::rebel_faction_id temp = state.world.create_rebel_faction();
					dcon::national_identity_id ind_tag = [&]() {
						auto prov = state.world.pop_get_province_from_pop_location(p);
						for(auto core : state.world.province_get_core(prov)) {
							if(core.get_identity().get_primary_culture() == state.world.pop_get_culture(p))
								return core.get_identity().id;
						}
						return dcon::national_identity_id{};
					}();

					dcon::rebel_type_id max_type;

					state.world.for_each_rebel_type([&](dcon::rebel_type_id rt) {
						if(pop_is_compatible_with_rebel_type(state, p, rt)) {
							state.world.rebel_faction_set_type(temp, rt);
							state.world.rebel_faction_set_defection_target(temp, dcon::national_identity_id{});
							state.world.rebel_faction_set_primary_culture(temp, dcon::culture_id{});
							state.world.rebel_faction_set_primary_culture_group(temp, dcon::culture_group_id{});
							state.world.rebel_faction_set_religion(temp, dcon::religion_id{});

							switch(culture::rebel_defection(state.world.rebel_type_get_defection(rt))) {
							case culture::rebel_defection::culture:
								state.world.rebel_faction_set_primary_culture(temp, state.world.pop_get_culture(p));
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							case culture::rebel_defection::culture_group:
								state.world.rebel_faction_set_primary_culture_group(temp,
										state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p)));
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							case culture::rebel_defection::religion:
								state.world.rebel_faction_set_religion(temp, state.world.pop_get_religion(p));
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							case culture::rebel_defection::pan_nationalist: {
								auto cg = state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p));
								auto u = state.world.culture_group_get_identity_from_cultural_union_of(cg);
								if(!u)
									return; // skip -- no pan nationalist possible
								state.world.rebel_faction_set_defection_target(temp, u);
								break;
							}
							case culture::rebel_defection::any:
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							default:
								break;
							}

							switch(culture::rebel_independence(state.world.rebel_type_get_independence(rt))) {
							case culture::rebel_independence::culture:
								state.world.rebel_faction_set_primary_culture(temp, state.world.pop_get_culture(p));
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							case culture::rebel_independence::culture_group:
								state.world.rebel_faction_set_primary_culture_group(temp,
										state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p)));
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							case culture::rebel_independence::religion:
								state.world.rebel_faction_set_religion(temp, state.world.pop_get_religion(p));
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							case culture::rebel_independence::pan_nationalist: {
								auto cg = state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p));
								auto u = state.world.culture_group_get_identity_from_cultural_union_of(cg);
								if(!u)
									return; // skip -- no pan nationalist possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								state.world.rebel_faction_set_defection_target(temp, u);
								break;
							}
							case culture::rebel_independence::any:
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							case culture::rebel_independence::colonial:
								state.world.rebel_faction_set_defection_target(temp, ind_tag);
								if(!ind_tag)
									return; // skip -- no defection possible
								if(state.world.pop_get_is_primary_or_accepted_culture(p))
									return; // skip -- can't defect
								break;
							default:
								break;
							}

							if(state.world.rebel_type_get_culture_restriction(rt) && !state.world.rebel_faction_get_primary_culture(temp)) {
								state.world.rebel_faction_set_primary_culture(temp, state.world.pop_get_culture(p));
							}
							if(state.world.rebel_type_get_culture_group_restriction(rt) && !state.world.rebel_faction_get_primary_culture_group(temp)) {
								state.world.rebel_faction_set_primary_culture_group(temp, state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p)));
							}

							auto chance = state.world.rebel_type_get_spawn_chance(rt);
							auto eval = trigger::evaluate_multiplicative_modifier(state, chance, trigger::to_generic(p),
									trigger::to_generic(owner), trigger::to_generic(temp));
							if(eval > greatest_chance) {
								f = temp;
								max_type = rt;
								greatest_chance = eval;
							}
						}
					});

					if(f != temp) {
						state.world.delete_rebel_faction(temp);
					} else {
						auto rt = max_type;
						state.world.rebel_faction_set_type(temp, rt);
						state.world.rebel_faction_set_defection_target(temp, dcon::national_identity_id{});
						state.world.rebel_faction_set_primary_culture(temp, dcon::culture_id{});
						state.world.rebel_faction_set_primary_culture_group(temp, dcon::culture_group_id{});
						state.world.rebel_faction_set_religion(temp, dcon::religion_id{});

						switch(culture::rebel_defection(state.world.rebel_type_get_defection(rt))) {
						case culture::rebel_defection::culture:
							state.world.rebel_faction_set_primary_culture(temp, state.world.pop_get_culture(p));
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						case culture::rebel_defection::culture_group:
							state.world.rebel_faction_set_primary_culture_group(temp,
									state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p)));
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						case culture::rebel_defection::religion:
							state.world.rebel_faction_set_religion(temp, state.world.pop_get_religion(p));
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						case culture::rebel_defection::pan_nationalist: {
							auto cg = state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p));
							auto u = state.world.culture_group_get_identity_from_cultural_union_of(cg);
							state.world.rebel_faction_set_defection_target(temp, u);
							break;
						}
						case culture::rebel_defection::any:
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						default:
							break;
						}

						switch(culture::rebel_independence(state.world.rebel_type_get_independence(rt))) {
						case culture::rebel_independence::culture:
							state.world.rebel_faction_set_primary_culture(temp, state.world.pop_get_culture(p));
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						case culture::rebel_independence::culture_group:
							state.world.rebel_faction_set_primary_culture_group(temp,
									state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p)));
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						case culture::rebel_independence::religion:
							state.world.rebel_faction_set_religion(temp, state.world.pop_get_religion(p));
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						case culture::rebel_independence::pan_nationalist: {
							auto cg = state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(p));
							auto u = state.world.culture_group_get_identity_from_cultural_union_of(cg);
							state.world.rebel_faction_set_defection_target(temp, u);
							break;
						}
						case culture::rebel_independence::any:
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						case culture::rebel_independence::colonial:
							state.world.rebel_faction_set_defection_target(temp, ind_tag);
							break;
						default:
							break;
						}

						state.world.try_create_rebellion_within(temp, owner);
					}

					if(greatest_chance > 0) {
						add_pop_to_rebel_faction(state, p, f);
					}
				}
			}
		} else { // less than: MIL_TO_JOIN_REBEL will join a rebel_faction -- leave faction
			if(existing_faction) {
				remove_pop_from_rebel_faction(state, p);
			}
		}
	});
}

void delete_faction(sys::state& state, dcon::rebel_faction_id reb) {

	auto armies = state.world.rebel_faction_get_army_rebel_control(reb);
	while(armies.begin() != armies.end()) {
		military::cleanup_army(state, (*armies.begin()).get_army());
	}
	auto control = state.world.rebel_faction_get_province_rebel_control(reb);
	for(auto p : control) {
		auto prov = p.get_province();
		prov.set_nation_from_province_control(prov.get_nation_from_province_ownership());
		prov.set_last_control_change(state.current_date);
		// we let the deletion of the rebel faction undo the rebel control itself
	}
	state.world.delete_rebel_faction(reb);
}

void update_factions(sys::state& state) {
	update_pop_rebel_membership(state);

	// IMPORTANT: we count down here so that we can delete as we go, compacting from the end
	for(auto last = state.world.rebel_faction_size(); last-- > 0;) {
		dcon::rebel_faction_id m{dcon::rebel_faction_id::value_base_t(last)};
		if(!rebel_faction_is_valid(state, m)) {
			for(auto members : state.world.rebel_faction_get_pop_rebellion_membership(m)) {
				members.get_pop().set_militancy(0.0f);
			}
			delete_faction(state, m);
		}
	}
}

void daily_update_rebel_organization(sys::state& state) {
	// update brigade count
	state.world.for_each_rebel_faction([&](dcon::rebel_faction_id rf) {
		int32_t total = 0;
		for(auto pop : state.world.rebel_faction_get_pop_rebellion_membership(rf)) {
			total += int32_t(state.world.pop_get_size(pop.get_pop()) / state.defines.pop_size_per_regiment);
		}
		state.world.rebel_faction_set_possible_regiments(rf, total);
	});

	// update org
	state.world.for_each_rebel_faction([&](dcon::rebel_faction_id rf) {
		/*
		- Sum for each pop belonging to the faction that has made money in the day: (pop-income + 0.02) x pop-literacy x (10 if
		militancy is > define:MILITANCY_TO_AUTO_RISE and 5 if militancy is less than that but > define:MILITANCY_TO_JOIN_RISING) /
		(1 + national-administration-spending-setting). Take this sum, multiply by 0.001 x (rebel organization from technology +
		1) and divide by the number of regiments those pops could form. If positive, add this to the current organization of the
		rebel faction (to a maximum of 1). This appears to be done daily.
		*/

		float total_change = 0;
		for(auto pop : state.world.rebel_faction_get_pop_rebellion_membership(rf)) {
			auto mil_factor = [&]() {
				auto m = pop.get_pop().get_militancy();
				if(m > state.defines.mil_to_autorise)
					return 10.0f;
				else if(m > state.defines.mil_to_join_rising)
					return 5.0f;
				return 1.0f;
			}();
			total_change += pop.get_pop().get_savings() * pop.get_pop().get_literacy() * mil_factor;
		}
		auto reg_count = state.world.rebel_faction_get_possible_regiments(rf);
		auto within = state.world.rebel_faction_get_ruler_from_rebellion_within(rf);
		auto rebel_org_mod = 1.0f + state.world.nation_get_rebel_org_modifier(within, state.world.rebel_faction_get_type(rf));

		if(reg_count > 0) {
			state.world.rebel_faction_set_organization(rf,
					std::min(1.0f,
							state.world.rebel_faction_get_organization(rf) +
									total_change *
											(1.0f + state.world.nation_get_rebel_org_modifier(within, state.world.rebel_faction_get_type(rf))) *
											0.001f / (1.0f + state.world.nation_get_administrative_efficiency(within)) / float(reg_count)));
		}
	});
}

void damage_pops_on_rebel_victory(sys::state& state, dcon:: province_id p) {
	for(auto pop : state.world.province_get_pop_location(p)) {
		pop.get_pop().get_size() *= 0.995f; // 0.5% reduction in size on rebel victory
	}
}

void rebel_risings_check(sys::state& state) {
	static auto province_damage = state.world.province_make_vectorizable_float_buffer();

	for(auto rf : state.world.in_rebel_faction) {
		auto revolt_chance = get_faction_revolt_risk(state, rf);
		auto rval = rng::get_random(state, uint32_t(rf.id.value));
		float p_val = float(rval & 0xFFFF) / float(0x10000);
		if(p_val < revolt_chance) {
			auto const faction_owner = rf.get_ruler_from_rebellion_within();
			auto const new_to_make = get_faction_brigades_ready(state, rf);
			// auto counter = new_to_make;

			if(new_to_make == 0)
				continue;

			ve::execute_serial_fast<dcon::province_id>(state.world.province_size(), [&](auto index) {
				province_damage.set(index, ve::fp_vector{});
			});

			/*
			- When a rising happens, pops with at least define:MILITANCY_TO_JOIN_RISING will spawn faction-organization x
			max-possible-supported-regiments, to a minimum of 1 (if any more regiments are possible).
			*/

			float total_damage = 0.0f;

			for(auto pop : rf.get_pop_rebellion_membership()) {
				//if(counter == 0)
				//	break;

				if(pop.get_pop().get_militancy() >= state.defines.mil_to_join_rising) {
					auto location = pop.get_pop().get_province_from_pop_location();
					if(location.get_nation_from_province_control() == faction_owner) { // only in a province controlled by the owner
						auto& sz = pop.get_pop().get_size();
						province_damage.get(location) += sz;
						total_damage += sz;
						sz *= 0.98f; // 2% damage to pops that attempt a rising
					}

					/*
					// this is the logic we would use if we were creating rebel regiments
					auto max_count = int32_t(state.world.pop_get_size(pop.get_pop()) / state.defines.pop_size_per_regiment);

					
					auto cregs = pop.get_pop().get_regiment_source();
					auto used_count = int32_t(cregs.end() - cregs.begin());

					if(used_count < max_count) {
						auto pop_location = pop.get_pop().get_province_from_pop_location();

						auto new_reg = military::create_new_regiment(state, dcon::nation_id{}, state.military_definitions.irregular);
						auto a = [&]() {
							for(auto ar : state.world.province_get_army_location(pop_location)) {
								if(ar.get_army().get_controller_from_army_rebel_control() == rf)
									return ar.get_army().id;
							}
							auto new_army = fatten(state.world, state.world.create_army());
							new_army.set_controller_from_army_rebel_control(rf);
							military::army_arrives_in_province(state, new_army, pop_location, military::crossing_type::none);
							return new_army.id;
						}();
						state.world.try_create_army_membership(new_reg, a);
						state.world.try_create_regiment_source(new_reg, pop.get_pop());

						--counter;
					}
					*/
				}
			}

			// this is the new logic to do rebel occupation and damage on a rising
			if(total_damage == 0)
				continue;

			float scale_factor = float(new_to_make) * 0.1f / total_damage; // 0.1 damage per regiment that would have been raised
			for(int32_t i = state.province_definitions.first_sea_province.index(); i-- > 0;) {
				dcon::province_id p{dcon::province_id::value_base_t(i) };
				if(auto dmg = province_damage.get(p) * scale_factor; dmg > 0.05f) { // threshold for flipping a province
					float total_army_strength = 0.0f;

					// first, sum up all strength in province
					for(auto ar : state.world.province_get_army_location(p)) {
						if(ar.get_army().get_black_flag() == false && ar.get_army().get_is_retreating() == false && !bool(ar.get_army().get_navy_from_army_transport())) {

							for(auto r : ar.get_army().get_army_membership()) {
								total_army_strength += r.get_regiment().get_strength();
							}
						}
					}

					auto damage_fraction = total_army_strength > 0 ? dmg / total_army_strength : 1.0f;
					if(damage_fraction >= 1.0f) {
						damage_fraction = 1.0f;

						damage_pops_on_rebel_victory(state, p);

						if(auto swt = rf.get_type().get_siege_won_trigger(); !swt || trigger::evaluate(state, swt, trigger::to_generic(p), trigger::to_generic(faction_owner.id), trigger::to_generic(rf.id))) {
							auto swe = rf.get_type().get_siege_won_effect();
							if(swe) {
								effect::execute(state, swe, trigger::to_generic(p), trigger::to_generic(faction_owner.id), trigger::to_generic(rf.id), uint32_t(state.current_date.value), uint32_t((rf.id.index() << 3) ^ p.index()));
							}
						}
						state.world.province_set_rebel_faction_from_province_rebel_control(p, rf);
						state.world.province_set_nation_from_province_control(p, dcon::nation_id{});
						state.world.province_set_last_control_change(p, state.current_date);

						military::eject_ships(state, p);
					}

					// deal damage to any armies present
					for(auto ar : state.world.province_get_army_location(p)) {
						if(ar.get_army().get_black_flag() == false && ar.get_army().get_is_retreating() == false && !bool(ar.get_army().get_navy_from_army_transport())) {

							for(auto r : ar.get_army().get_army_membership()) {
								auto& str = r.get_regiment().get_strength();
								auto damage = str * damage_fraction;
								str -= damage;
								r.get_regiment().get_pending_damage() += damage;
							}
						}
					}

					if(damage_fraction >= 1.0f) {
						military::update_blackflag_status(state, p);
					}
				}
			}

			// end of new logic
			
			//if(counter != new_to_make) {
				notification::post(state, notification::message{
					[reb = rf.id](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_revolt_1", text::variable_type::x, state.world.rebel_faction_get_type(reb).get_title());
					},
					"msg_revolt_title",
					rf.get_ruler_from_rebellion_within(),
					sys::message_setting_type::revolt });
			//}

			/*
			- Faction organization is reduced to 0 after an initial rising (for later contributory risings, it may instead be reduced by
			a factor of (number-of-additional-regiments x 0.01 + 1))
			*/
			rf.set_organization(0);

		}
	}
}

bool sphere_member_has_ongoing_revolt(sys::state& state, dcon::nation_id n) {
	auto nation_count = state.world.nation_size();
	for(uint32_t i = 0; i < nation_count; ++i) {
		dcon::nation_id m{dcon::nation_id::value_base_t(i)};
		if(state.world.nation_get_in_sphere_of(m) == n) {
			for(auto fac : state.world.nation_get_rebellion_within(m)) {
				auto control = fac.get_rebels().get_province_rebel_control();
				if(control.begin() != control.end())
					return true;
			}
		}
	}
	return false;
}

int32_t get_faction_brigades_ready(sys::state& state, dcon::rebel_faction_id r) {
	return state.world.rebel_faction_get_possible_regiments(r) - get_faction_brigades_active(state, r);
}

int32_t get_faction_brigades_active(sys::state& state, dcon::rebel_faction_id r) {
	int32_t total = 0;
	for(auto ar : state.world.rebel_faction_get_army_rebel_control(r)) {
		auto regs = ar.get_army().get_army_membership();
		total += int32_t(regs.end() - regs.begin());
	}
	return total;
}

float get_faction_organization(sys::state& state, dcon::rebel_faction_id r) {
	return state.world.rebel_faction_get_organization(r);
}

float get_faction_revolt_risk(sys::state& state, dcon::rebel_faction_id r) {
	/*
	- Rebels have a chance to rise once per month. The probability the rising will happen is: faction-organization x 0.05 + 0.02 +
	faction-organization x number-of-regiments-the-rising-could-form / 1v(number-of-regiments-controlled-by-nation x 20)
	*/
	auto nation_brigades =
			std::max(1, int32_t(state.world.nation_get_active_regiments(state.world.rebel_faction_get_ruler_from_rebellion_within(r))));

	auto num_brigades_ready = get_faction_brigades_ready(state, r);
	auto faction_org = state.world.rebel_faction_get_organization(r);

	if(num_brigades_ready <= 0) {
		return 0.0f;
	}

	float chance = std::clamp(
			faction_org * 0.05f + 0.02f + faction_org * float(num_brigades_ready) / (float(nation_brigades) * 20.0f), 0.0f, 1.0f);
	return chance;
}

void execute_province_defections(sys::state& state) {
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto reb_controller = state.world.province_get_rebel_faction_from_province_rebel_control(p);
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(reb_controller && owner) {
			assert(!bool(state.world.province_get_nation_from_province_control(p)));
			auto reb_type = state.world.rebel_faction_get_type(reb_controller);
			culture::rebel_defection def_type = culture::rebel_defection(reb_type.get_defection());
			if(def_type != culture::rebel_defection::none && state.world.province_get_last_control_change(p) + reb_type.get_defect_delay() * 31 <= state.current_date) {

				// defection time

				dcon::nation_id defection_tag = [&]() {
					switch(def_type) {
					case culture::rebel_defection::ideology: {
						// prefer existing tag of same ideology
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(c.get_identity().get_nation_from_identity_holder().get_ruling_party().get_ideology() ==
											reb_type.get_ideology() &&
									owned.begin() != owned.end()) {
								return c.get_identity().get_nation_from_identity_holder().id;
							}
						}
						// otherwise pick a non-existent tag
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(!c.get_identity().get_is_not_releasable() && owned.begin() == owned.end()) {
								auto t = c.get_identity().get_nation_from_identity_holder().id;
								nations::create_nation_based_on_template(state, t, owner);
								politics::force_nation_ideology(state, t, reb_type.get_ideology());
								return t;
							}
						}
						break;
					}
					case culture::rebel_defection::religion:
						// prefer existing
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(c.get_identity().get_religion() == state.world.rebel_faction_get_religion(reb_controller) &&
									owned.begin() != owned.end()) {
								return c.get_identity().get_nation_from_identity_holder().id;
							}
						}
						// otherwise pick a non-existent tag
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(c.get_identity().get_religion() == state.world.rebel_faction_get_religion(reb_controller) &&
									owned.begin() == owned.end()) {
								auto t = c.get_identity().get_nation_from_identity_holder().id;
								nations::create_nation_based_on_template(state, t, owner);
								return t;
							}
						}
						break;
					case culture::rebel_defection::culture:
						// prefer existing
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(c.get_identity().get_primary_culture() == state.world.rebel_faction_get_primary_culture(reb_controller) &&
									owned.begin() != owned.end()) {
								return c.get_identity().get_nation_from_identity_holder().id;
							}
						}
						// otherwise pick a non-existent tag
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(c.get_identity().get_primary_culture() == state.world.rebel_faction_get_primary_culture(reb_controller) &&
									owned.begin() == owned.end()) {
								auto t = c.get_identity().get_nation_from_identity_holder().id;
								nations::create_nation_based_on_template(state, t, owner);
								return t;
							}
						}
						break;
					case culture::rebel_defection::culture_group:
						// prefer existing
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(c.get_identity().get_primary_culture().get_group_from_culture_group_membership() ==
											state.world.rebel_faction_get_primary_culture_group(reb_controller) &&
									owned.begin() != owned.end()) {
								return c.get_identity().get_nation_from_identity_holder().id;
							}
						}
						// otherwise pick a non-existent tag
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(c.get_identity().get_primary_culture().get_group_from_culture_group_membership() ==
											state.world.rebel_faction_get_primary_culture_group(reb_controller) &&
									owned.begin() == owned.end()) {
								auto t = c.get_identity().get_nation_from_identity_holder().id;
								nations::create_nation_based_on_template(state, t, owner);
								return t;
							}
						}

						// first: existing tag of culture; then existing tag of culture group;
						// then dead tag of culture, then dead tag of cg
						break;
					case culture::rebel_defection::pan_nationalist:
						// union tag or nothing
						for(auto c : state.world.province_get_core(p)) {
							auto owned = c.get_identity().get_nation_from_identity_holder().get_province_ownership();
							if(c.get_identity() == state.world.rebel_faction_get_defection_target(reb_controller)) {
								if(owned.begin() != owned.end())
									return c.get_identity().get_nation_from_identity_holder().id;

								auto t = c.get_identity().get_nation_from_identity_holder().id;
								nations::create_nation_based_on_template(state, t, owner);
								return t;
							}
						}
						break;
					default: // NONE and ANY
						return dcon::nation_id{};
					}
					return dcon::nation_id{};
				}();

				if(defection_tag) {
					// TODO: if rebel armies were still a thing, you would probably want to delete the ones that did
					// the work of causing the province to defect or to send them back to the original country or something
					// but ...
					province::change_province_owner(state, p, defection_tag);
				}
			}
		}
	});
}

float get_suppression_point_cost(sys::state& state, dcon::movement_id m) {
	/*
	Depends on whether define:POPULATION_SUPPRESSION_FACTOR is non zero. If it is zero, suppression costs their effective
	radicalism + 1. If it is non zero, then the cost is the greater of that value and the movements effective radicalism x the
	movement's support / define:POPULATION_SUPPRESSION_FACTOR
	*/
	if(state.defines.population_suppression_factor > 0.0f) {
		return std::max(
			state.world.movement_get_radicalism(m) + 1.0f,
			state.world.movement_get_radicalism(m) * state.world.movement_get_pop_support(m) / state.defines.population_suppression_factor);
	} else {
		return state.world.movement_get_radicalism(m) + 1.0f;
	}
}

void execute_rebel_victories(sys::state& state) {
	for(uint32_t i = state.world.rebel_faction_size(); i-- > 0;) {
		auto reb = dcon::rebel_faction_id{dcon::rebel_faction_id::value_base_t(i)};
		auto within = state.world.rebel_faction_get_ruler_from_rebellion_within(reb);
		auto control = state.world.rebel_faction_get_province_rebel_control(reb);
		auto enforce_trigger = state.world.rebel_faction_get_type(reb).get_demands_enforced_trigger();
		if(control.begin() != control.end() && enforce_trigger &&
				trigger::evaluate(state, enforce_trigger, trigger::to_generic(within), trigger::to_generic(within),
						trigger::to_generic(reb))) {
			// rebel victory

			/*
			If it wins, it gets its `demands_enforced_effect` executed.
			*/

			if(auto k = state.world.rebel_faction_get_type(reb).get_demands_enforced_effect(); k)
				effect::execute(state, k, trigger::to_generic(within), trigger::to_generic(within), trigger::to_generic(reb),
						uint32_t(state.current_date.value), uint32_t(within.index() ^ (reb.index() << 4)));

			/*
			The government type of the nation will change if the rebel type has an associated government (with the same logic for
			a government type change from a wargoal or other cause).
			*/

			auto old_gov = state.world.nation_get_government_type(within);
			auto new_gov = state.world.rebel_faction_get_type(reb).get_government_change(old_gov);
			if(new_gov) {
				politics::change_government_type(state, within, new_gov);
			}
			if(auto iid = state.world.rebel_faction_get_type(reb).get_ideology(); iid) {
				politics::force_nation_ideology(state, within, iid);
			}

			/*
			If the rebel type has "break alliances on win" then the nation loses all of its alliances, all of its non-substate
			vassals, all of its sphere members, and loses all of its influence and has its influence level set to neutral.
			*/

			if(state.world.rebel_faction_get_type(reb).get_break_alliance_on_win()) {
				nations::destroy_diplomatic_relationships(state, within);
			}

			/*
			The nation loses prestige equal to define:PRESTIGE_HIT_ON_BREAK_COUNTRY x (nation's current prestige + permanent
			prestige), which is multiplied by the nation's prestige modifier from technology + 1 as usual (!).
			*/
			auto ploss = state.defines.prestige_hit_on_break_country * nations::prestige_score(state, within);
			nations::adjust_prestige(state, within, ploss);

			notification::post(state, notification::message{
				[type = state.world.rebel_faction_get_type(reb).id, ploss, new_gov, within, when = state.current_date, i = reb.index()](sys::state& state, text::layout_base& contents) {

					text::add_line(state, contents, "msg_rebels_win_1", text::variable_type::x, state.world.rebel_type_get_title(type));
					text::add_line(state, contents, "msg_rebels_win_2", text::variable_type::x, text::fp_one_place{ploss});
					if(new_gov) {
						text::add_line(state, contents, "msg_rebels_win_3", text::variable_type::x, state.culture_definitions.governments[new_gov].name);
					}
					if(auto iid = state.world.rebel_type_get_ideology(type); iid) {
						text::add_line(state, contents, "msg_rebels_win_4", text::variable_type::x, state.world.ideology_get_name(iid));
					}
					if(state.world.rebel_type_get_break_alliance_on_win(type)) {
						text::add_line(state, contents, "msg_rebels_win_5");
					}
					if(auto k = state.world.rebel_type_get_demands_enforced_effect(type); k) {
						text::add_line(state, contents, "msg_rebels_win_6");
						ui::effect_description(state, contents, k, trigger::to_generic(within), trigger::to_generic(within), -1, uint32_t(when.value), uint32_t(within.index() ^ (i << 4)));
					}
				},
				"msg_rebels_win_title",
				within,
				sys::message_setting_type::breakcountry
			});

			/*
			Any units for the faction that exist are destroyed (or transferred if it is one of the special rebel types).
			*/
			delete_faction(state, reb);
		}
	}
}

void trigger_revolt(sys::state& state, dcon::nation_id n, dcon::rebel_type_id t, dcon::ideology_id i, dcon::culture_id c,
		dcon::religion_id r) {
	// TODO
}

} // namespace rebel
