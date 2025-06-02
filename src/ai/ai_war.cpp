#include "ai_war.hpp"
#include "ai_campaign_values.hpp"
#include "prng.hpp"
#include "demographics.hpp"
#include "triggers.hpp"

namespace ai {

// Desired properties of utility:
// 1) utility represents our desirability of an event
// 2) event "nothing happens" has 0 utility
// 3) never choose action with negative utility when there is a better alternative
// 4) utility of actions without intersection is usually additive
// 
// Corollaries:
// 1) 0 utility means that X is useless
// but also is not really harmful and equivalent to nothing
// in most cases we have an option of doing nothing,
// so we will usually filter out actions with negative utility
// 2) usually we will be able to add up utilities
// of independent parts to obtain total utility
// but sometimes we will need to account for synergy effects

inline constexpr float BENEFIT_YOURSELF = 1.f;
inline constexpr float BENEFIT_YOUR_ENEMY = -0.5f;
inline constexpr float BENEFIT_YOUR_ALLY = 0.5f;

inline constexpr float HARM_YOURSELF = -1.f;
inline constexpr float HARM_YOUR_ENEMY = 0.5f;
inline constexpr float HARM_YOUR_ALLY = -1.f;

float utility_multiplier(
	sys::state& state,
	dcon::nation_id actor,
	dcon::nation_id victim,
	dcon::nation_id beneficiary
) {
	assert(victim != beneficiary);
	assert(beneficiary);

	auto enemy = state.world.nation_get_ai_rival(actor);
	auto lord_of_actor = state.world.nation_get_in_sphere_of(actor);
	auto lord_of_victim = state.world.nation_get_in_sphere_of(victim);
	auto lord_of_beneficiary = state.world.nation_get_in_sphere_of(beneficiary);

	// we do not care
	float multiplier = 0.f;

	// but..

	// love yourself
	if(actor == beneficiary) {
		multiplier += BENEFIT_YOURSELF;
	}
	if(actor == victim) {
		multiplier += HARM_YOURSELF;
	}

	// don't help your enemy
	if(enemy == beneficiary) {
		multiplier += BENEFIT_YOUR_ENEMY;
	}

	// but bring misfortune to your enemy
	if(enemy == victim) {
		multiplier += HARM_YOUR_ENEMY;
	}
	if(lord_of_victim && enemy == lord_of_victim) {
		multiplier += HARM_YOUR_ENEMY;
	}

	// do not betray your allies and your lord
	if(lord_of_actor == victim) {
		multiplier += HARM_YOUR_ALLY;
	}
	if(nations::are_allied(state, actor, victim)) {
		multiplier += HARM_YOUR_ALLY;
	}

	// share with your allies and your vassals
	if(lord_of_beneficiary == actor	|| nations::are_allied(state, actor, beneficiary)) {
		multiplier += BENEFIT_YOUR_ALLY;
	}

	return multiplier;
}

// no chance to win a war against equal enemy

inline constexpr float STRENGTH_RATIO_UNLIKELY_VICTORY = 0.5f;
inline constexpr float STRENGTH_RATIO_TOTAL_VICTORY = 3.f;
// probability to naval invade given a total naval superiority 
inline constexpr float AI_NAVAL_INVASION_STUPIDITY = 0.8f;
static_assert(STRENGTH_RATIO_TOTAL_VICTORY > STRENGTH_RATIO_UNLIKELY_VICTORY);

float supremacy_ratio_to_probability(float ratio) {
	return std::clamp(
		(ratio - STRENGTH_RATIO_UNLIKELY_VICTORY)
		/ (STRENGTH_RATIO_TOTAL_VICTORY - STRENGTH_RATIO_UNLIKELY_VICTORY)
		, 0.f, 1.f
	);
}

float probability_of_conquering_state(
	sys::state& state,
	dcon::nation_id conqueror,
	float conqueror_strength,
	dcon::state_instance_id target,
	float target_strength
) {
	auto victim = state.world.state_instance_get_nation_from_state_ownership(target);

	float probability_to_conquer = 1.f;
	bool target_is_close = province::state_borders_nation(state, conqueror, target);

	if(!target_is_close) {
		if(province::state_is_coastal(state, target) && state.world.nation_get_total_ports(conqueror) > 0) {
			float conqueror_fleet = ai::estimate_naval_strength(state, conqueror);

			// we have to conquer the sea itself (transport the troops)
			float victim_fleet = 10.f + 0.1f * province::direct_distance(
				state,
				state.world.nation_get_capital(conqueror),
				state.world.state_instance_get_capital(target)
			);
			if(victim) {
				victim_fleet += ai::estimate_naval_strength(state, victim);
			}
			float naval_ratio = conqueror_fleet / victim_fleet;

			probability_to_conquer *= supremacy_ratio_to_probability(naval_ratio) * AI_NAVAL_INVASION_STUPIDITY;
		} else {
			return 0.f;
		}
	}

	probability_to_conquer *= supremacy_ratio_to_probability(conqueror_strength / (target_strength + 1.f));

	return probability_to_conquer;
}

float probability_of_winning(
	sys::state& state,
	dcon::nation_id conqueror,
	float conqueror_strength,
	dcon::nation_id victim,
	float victim_strength
) {
	float probability_to_conquer = 1.f;
	bool target_is_close = false;
	if(state.world.get_nation_adjacency_by_nation_adjacency_pair(conqueror, victim)) {
		target_is_close = true;
	}

	if(!target_is_close) {
		if(state.world.nation_get_total_ports(conqueror) > 0 && state.world.nation_get_total_ports(victim) > 0) {
			float conqueror_fleet = ai::estimate_naval_strength(state, conqueror);

			// we have to conquer the sea itself (transport the troops)
			float victim_fleet = 10.f + 0.1f * province::direct_distance(
				state,
				state.world.nation_get_capital(conqueror),
				state.world.nation_get_capital(victim)
			);
			if(victim) {
				victim_fleet += ai::estimate_naval_strength(state, victim);
			}
			float naval_ratio = conqueror_fleet / victim_fleet;

			probability_to_conquer *= supremacy_ratio_to_probability(naval_ratio) * AI_NAVAL_INVASION_STUPIDITY;
		} else {
			return 0.f;
		}
	}

	probability_to_conquer *= supremacy_ratio_to_probability(conqueror_strength / victim_strength);

	return probability_to_conquer;
}

// measure utility in effective taxpayers?

inline constexpr float PRIMARY_EFFICIENCY = 0.6f;
inline constexpr float BASE_EFFICIENCY = 0.4f;

// tries to estimate utility of starting a war
// negative as currently there is no way to profit from wars directly
float utility_of_offensive_war(
	sys::state& state,
	dcon::nation_id attacker,
	dcon::nation_id defender
) {
	// assume that we lose 0.1% of population during war we can't win
	// assume that we lose nothing during total victory 
	// TODO: make better estimations and assumptions
	// requires gathering more data and statistics

	float our_strength = ai::estimate_strength(state, attacker);
	float victim_strength = 5.f + ai::estimate_defensive_strength(state, defender);

	float chance_to_win = supremacy_ratio_to_probability(our_strength / victim_strength);
	float lost_population_ratio = (1.f - chance_to_win) * 0.001f;

	return -state.world.nation_get_demographics(attacker, demographics::total)
		* lost_population_ratio;
}

float utility_efficiency_of_state(
	sys::state& state,
	dcon::nation_id potential_owner,
	dcon::state_instance_id target
) {
	bool target_is_close = province::state_borders_nation(state, potential_owner, target);
	auto control_mult = 0.f;
	if(province::state_is_coastal(state, target) && state.world.nation_get_total_ports(potential_owner) > 0) {
		control_mult = 2.f;
	} else {
		if(target_is_close) {
			control_mult = 1.f;
		} else
			return 0.f;
	}
	return control_mult;
}

float utility_efficiency_of_nation(
	sys::state& state,
	dcon::nation_id potential_owner,
	dcon::nation_id target
) {
	auto control_mult = 0.f;
	bool target_is_close = false;
	if(state.world.get_nation_adjacency_by_nation_adjacency_pair(potential_owner, target)) {
		target_is_close = true;
	}
	// coastal nations are easier to control
	if(state.world.nation_get_total_ports(target) > 0 && state.world.nation_get_total_ports(potential_owner) > 0) {
		control_mult = 2.f;
	} else {
		if(target_is_close) {
			control_mult = 1.f;
		} else
			return 0.f;
	}
	return control_mult;
}

float utility_of_state(
	sys::state& state,
	dcon::nation_id owner,
	dcon::state_instance_id target
) {
	auto victim = state.world.state_instance_get_nation_from_state_ownership(target);
	auto primary_key = demographics::to_key(state, state.world.nation_get_primary_culture(owner));
	auto total = state.world.state_instance_get_demographics(target, demographics::total);
	auto primary = state.world.state_instance_get_demographics(target, primary_key);
	auto base_utility = total * BASE_EFFICIENCY + primary * PRIMARY_EFFICIENCY;
	// TODO: account for local goods
	return base_utility;
}

float utility_of_nation(
	sys::state& state,
	dcon::nation_id owner,
	dcon::nation_id target
) {
	auto primary_key = demographics::to_key(state, state.world.nation_get_primary_culture(owner));

	auto total = state.world.nation_get_demographics(target, demographics::total);
	auto primary = state.world.nation_get_demographics(target, primary_key);

	auto base_utility = total * BASE_EFFICIENCY + primary * PRIMARY_EFFICIENCY;	

	// TODO: account for local goods
	return base_utility;
}

float utility_of_states_trigger(
	sys::state& state,
	dcon::nation_id target,
	dcon::nation_id liberated,
	dcon::trigger_key trigger
) {
	auto total_utility = 0.f;
	for(auto soid : state.world.nation_get_state_ownership(target)) {
		auto sid = soid.get_state().id;
		if(trigger::evaluate(
			state,
			trigger,
			trigger::to_generic(sid),
			trigger::to_generic(target),
			trigger::to_generic(liberated)
		)) {
			total_utility += utility_of_state(state, target, sid);
		}
	}
	return total_utility;
}

bool will_be_crisis_primary_attacker(sys::state& state, dcon::nation_id n) {

	auto first_wg = state.crisis_attacker_wargoals.at(0);
	if(first_wg.cb == state.military_definitions.crisis_colony) {
		auto colonizers = state.world.state_definition_get_colonization(first_wg.state);
		if(colonizers.end() - colonizers.begin() < 2)
			return false;

		auto defending_colonizer = (*(colonizers.begin() + 1)).get_colonizer();
		if(state.world.nation_get_in_sphere_of(defending_colonizer) == n)
			return false;
		if(state.world.nation_get_ai_rival(n) == defending_colonizer
			|| (defending_colonizer.get_in_sphere_of() && !nations::are_allied(state, n, defending_colonizer) && state.world.nation_get_ai_rival(n) == defending_colonizer.get_in_sphere_of())) {
			return true;
		} else {
			if(state.primary_crisis_defender && state.world.nation_get_ai_rival(n) == state.primary_crisis_defender)
				return true;
			return false;
		}
	} else if(first_wg.cb == state.military_definitions.liberate) {
		auto state_owner = first_wg.target_nation;
		auto liberated = state.world.national_identity_get_nation_from_identity_holder(first_wg.wg_tag);

		if(state_owner == n) //don't shoot ourselves
			return false;
		if(liberated == n) //except when we are shooting someone else
			return true;
		if(state.world.nation_get_in_sphere_of(state_owner) == n || nations::are_allied(state, n, state_owner))
			return false;
		if(state.world.nation_get_ai_rival(n) == state_owner)
			return true;
		if(state.world.nation_get_in_sphere_of(state_owner) && state.world.nation_get_ai_rival(n) == state.world.nation_get_in_sphere_of(state_owner))
			return true;
		if(state.world.nation_get_in_sphere_of(liberated) == n || nations::are_allied(state, n, liberated))
			return true;
		if(state.primary_crisis_defender && state.world.nation_get_ai_rival(n) == state.primary_crisis_defender)
			return true;
		return false;
	} else {
		return false;
	}
}
bool will_be_crisis_primary_defender(sys::state& state, dcon::nation_id n) {
	auto first_wg = state.crisis_attacker_wargoals.at(0);
	if(first_wg.cb == state.military_definitions.crisis_colony) {
		auto colonizers = state.world.state_definition_get_colonization(first_wg.state);
		if(colonizers.end() - colonizers.begin() < 2)
			return false;

		auto attacking_colonizer = (*colonizers.begin()).get_colonizer();

		if(state.world.nation_get_in_sphere_of(attacking_colonizer) == n)
			return false;
		if(state.world.nation_get_ai_rival(n) == attacking_colonizer
			|| (attacking_colonizer.get_in_sphere_of() && !nations::are_allied(state, n, attacking_colonizer) && state.world.nation_get_ai_rival(n) == attacking_colonizer.get_in_sphere_of())
			|| state.world.nation_get_ai_rival(n) == state.primary_crisis_attacker) {
			return true;
		} else {
			if(state.primary_crisis_attacker && state.world.nation_get_ai_rival(n) == state.primary_crisis_attacker)
				return true;
			return false;
		}
	} else if(first_wg.cb == state.military_definitions.liberate) {
		auto state_owner = first_wg.target_nation;
		auto liberated = state.world.national_identity_get_nation_from_identity_holder(first_wg.wg_tag);

		if(state_owner == n) //don't shoot ourselves
			return false;
		if(liberated == n) //except when we are shooting someone else
			return true;
		if(state.world.nation_get_in_sphere_of(liberated) == n || nations::are_allied(state, n, liberated))
			return false;
		if(state.world.nation_get_ai_rival(n) == liberated)
			return true;
		if(state.world.nation_get_in_sphere_of(liberated) && state.world.nation_get_ai_rival(n) == state.world.nation_get_in_sphere_of(liberated))
			return true;
		if(state.world.nation_get_in_sphere_of(state_owner) == n || nations::are_allied(state, n, state_owner))
			return true;
		if(state.primary_crisis_attacker && state.world.nation_get_ai_rival(n) == state.primary_crisis_attacker)
			return true;
		return false;
	} else {
		return false;
	}
}

struct crisis_str {
	float attacker = 0.0f;
	float defender = 0.0f;
	bool defender_win = false;
	bool attacker_win = false;
	bool fast_victory = false;
};

crisis_str estimate_crisis_str(sys::state& state) {
	float atotal = 0.0f;
	float dtotal = 0.0f;

	if(state.crisis_attacker && state.crisis_attacker != state.primary_crisis_attacker) {
		atotal += estimate_strength(state, state.crisis_attacker);
	}
	if(state.crisis_defender && state.crisis_defender != state.primary_crisis_defender) {
		dtotal += estimate_strength(state, state.crisis_defender);
	}
	for(auto& i : state.crisis_participants) {
		if(!i.id)
			break;
		if(!i.merely_interested) {
			if(i.supports_attacker) {
				atotal += estimate_strength(state, i.id);
			} else {
				dtotal += estimate_strength(state, i.id);
			}
		}
	}

	auto necessary_atk_win_ratio = state.defines.alice_crisis_necessary_base_win_ratio;
	auto necessary_def_win_ratio = state.defines.alice_crisis_necessary_base_win_ratio;
	auto necessary_fast_win_ratio = state.defines.alice_crisis_necessary_base_fast_win_ratio;

	if(!state.world.nation_get_is_civilized(state.crisis_defender)) {
		necessary_atk_win_ratio += state.defines.alice_crisis_unciv_stubbornness;
		necessary_fast_win_ratio += state.defines.alice_crisis_unciv_stubbornness;
	}
	for(auto wg : state.crisis_attacker_wargoals) {
		if(!wg.cb) {
			break;
		}

		necessary_atk_win_ratio += state.defines.alice_crisis_per_wg_ratio;
		necessary_fast_win_ratio += state.defines.alice_crisis_per_wg_ratio;
	}
	for(auto wg : state.crisis_defender_wargoals) {
		if(!wg.cb) {
			break;
		}

		necessary_def_win_ratio += state.defines.alice_crisis_per_wg_ratio;
		necessary_fast_win_ratio += state.defines.alice_crisis_per_wg_ratio;
	}

	auto def_victory = dtotal > atotal * necessary_def_win_ratio;
	auto atk_victory = atotal > dtotal * necessary_atk_win_ratio;
	auto fast_victory = (dtotal / atotal > necessary_fast_win_ratio) || (atotal / dtotal > necessary_fast_win_ratio);
	if(fast_victory) {
		atk_victory = atotal > dtotal;
		def_victory = dtotal > atotal;
	}

	return crisis_str{ atotal, dtotal, def_victory, atk_victory, fast_victory };
}

bool will_join_crisis_with_offer(sys::state& state, dcon::nation_id n, sys::full_wg offer) {
	if(offer.target_nation == state.world.nation_get_ai_rival(n))
		return true;
	auto offer_bits = state.world.cb_type_get_type_bits(offer.cb);
	if((offer_bits & (military::cb_flag::po_demand_state | military::cb_flag::po_annex)) != 0)
		return true;

	// GPs accept unequal treaties as crisis offer
	if(state.world.nation_get_is_great_power(n) && (offer_bits & (military::cb_flag::po_unequal_treaty)) != 0)
		return true;
	return false;
}


bool ai_offer_cb(sys::state& state, dcon::cb_type_id t) {
	auto offer_bits = state.world.cb_type_get_type_bits(t);
	if((offer_bits & (military::cb_flag::po_demand_state | military::cb_flag::po_annex)) != 0)
		return false;
	if((offer_bits & military::cb_flag::all_allowed_states) != 0)
		return false;
	if(military::cb_requires_selection_of_a_liberatable_tag(state, t))
		return false;
	if(military::cb_requires_selection_of_a_valid_nation(state, t))
		return false;
	return true;
}

void prepare_and_sort_list_of_desired_states(
	sys::state& state,
	std::vector<ai::weighted_state_instance>& result,
	dcon::nation_id beneficiary,
	dcon::nation_id target
) {
	for(auto soid : state.world.nation_get_state_ownership(target)) {
		auto sid = soid.get_state().id;
		auto utility = utility_of_state(
			state, beneficiary, sid
		);
		auto efficiency = utility_efficiency_of_state(
			state, beneficiary, sid
		);

		auto expected_utility = utility * efficiency;
		if(expected_utility > 0.f) {
			result.push_back(weighted_state_instance{
				sid,
				expected_utility
			});			
		}
	}

	std::sort(result.begin(), result.end(), [&](weighted_state_instance const& a, weighted_state_instance const& b) {
		if (a.weight != b.weight)
			return a.weight > b.weight;
		return a.target.index() < b.target.index();
	});
}

void prepare_list_of_states_for_conquest(
	sys::state& state,
	std::vector<ai::possible_cb>& result,
	dcon::nation_id attacker,
	float attacker_strength,
	dcon::nation_id target,
	float target_strength,
	dcon::cb_type_id cb,
	dcon::trigger_key trigger,
	dcon::war_id war // can be empty!!!
) {
	assert(trigger);

	auto utility_of_war = 0.f;
	if(!war) {
		utility_of_war = utility_of_offensive_war(state, attacker, target);
	}
	auto multiplier = utility_multiplier(
		state, attacker, target, attacker
	);

	for(auto soid : state.world.nation_get_state_ownership(target)) {
		auto sid = soid.get_state().id;
		auto utility = utility_of_state(
			state, attacker, sid
		);
		auto efficiency = utility_efficiency_of_state(
			state, attacker, sid
		);
		auto probability = probability_of_conquering_state(
			state, attacker, attacker_strength, sid, target_strength
		);

		// if we do not want some territorry,
		// conquering it from an ally to help him
		// to lose this bad territorry would be strange
		// so we put a max clamp here

		auto expected_utility = utility * efficiency * probability * std::max(multiplier, 0.f) + utility_of_war;
		if(expected_utility > 0.f) {
			auto trigger_result = trigger::evaluate(
				state,
				trigger,
				trigger::to_generic(sid),
				trigger::to_generic(attacker),
				trigger::to_generic(attacker)
			);
			if(trigger_result) {
				assert(military::cb_conditions_satisfied(state, attacker, target, cb));

				auto duplicate = military::war_goal_would_be_duplicate(
					state,
					attacker,
					war,
					target,
					cb,
					state.world.state_instance_get_definition(sid),
					dcon::national_identity_id{},
					dcon::nation_id{}
				);

				if(duplicate) {
					continue;
				}

				result.push_back(possible_cb{
					target,
					dcon::nation_id{},
					dcon::national_identity_id{},
					state.world.state_instance_get_definition(sid),
					cb,
					expected_utility
				});
			}
		}
	}
}

void sort_prepared_list_of_cb(
	sys::state& state,
	std::vector<ai::possible_cb>& result
) {
	std::sort(result.begin(), result.end(), [&](possible_cb const& a, possible_cb const& b) {
		if (a.utility_expectation != b.utility_expectation)
			return a.utility_expectation > b.utility_expectation;

		// random stuff to avoid equal elements
		if(a.cb != b.cb)
			return a.cb.index() < b.cb.index();
		if(a.target != b.target)
			return a.target.index() < b.target.index();
		if(a.secondary_nation != b.secondary_nation)
			return a.secondary_nation.index() < b.secondary_nation.index();
		if(a.associated_tag != b.associated_tag)
			return a.associated_tag.index() < b.associated_tag.index();
		return a.state_def.index() < b.state_def.index();
	});
}

void prepare_list_of_targets_for_cb(
	sys::state& state,
	std::vector<ai::possible_cb>& result,
	dcon::nation_id attacker,
	dcon::nation_id target,
	dcon::cb_type_id cb,
	dcon::war_id war // can be empty!!!
) {
	auto can_use = state.world.cb_type_get_can_use(cb);

	auto substate_trigger = state.world.cb_type_get_allowed_substate_regions(cb);
	auto nation_trigger = state.world.cb_type_get_allowed_countries(cb);
	auto state_instance_trigger = state.world.cb_type_get_allowed_states(cb);

	auto actual_cb_victim = target;

	if(substate_trigger) {
		if(!state.world.nation_get_is_substate(target))
			return;
		actual_cb_victim = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target));
	}

	if(can_use && !trigger::evaluate(
		state,
		can_use,
		trigger::to_generic(actual_cb_victim),
		trigger::to_generic(attacker),
		trigger::to_generic(actual_cb_victim))
	) {
		return;
	}

	auto attacker_strength = 1.f;
	auto defender_strength = 1.f;

	if(war) {
		auto attacker_role = military::get_role(state, war, attacker);
		if(attacker_role == military::war_role::attacker) {
			state.world.war_for_each_war_participant(war, [&](auto wpid) {
				auto nation = state.world.war_participant_get_nation(wpid);
				if(state.world.war_participant_get_is_attacker(wpid)) {
					attacker_strength += ai::estimate_strength(state, nation);
				} else {
					defender_strength += ai::estimate_strength(state, nation);
				}
			});
		} else {
			state.world.war_for_each_war_participant(war, [&](auto wpid) {
				auto nation = state.world.war_participant_get_nation(wpid);
				if(state.world.war_participant_get_is_attacker(wpid)) {
					defender_strength += ai::estimate_strength(state, nation);
				} else {
					attacker_strength += ai::estimate_strength(state, nation);
				}
			});
		}
	} else {
		attacker_strength = ai::estimate_strength(state, attacker);
		defender_strength = ai::estimate_defensive_strength(state, target);
	}

	auto probability_to_win_war = probability_of_winning(
		state, attacker, attacker_strength, target, defender_strength
	);

	// we don't care about declaration of war if we are already in war
	auto utility_of_war = 0.f;
	if(!war) {
		utility_of_war = utility_of_offensive_war(state, attacker, target);
	}

	if(!nation_trigger && state_instance_trigger) {		
		prepare_list_of_states_for_conquest(
			state, result, attacker, attacker_strength, target, defender_strength, cb, state_instance_trigger, war
		);
		return;
	}

	if(substate_trigger) { // checking for whether the target is a substate is already done above
		prepare_list_of_states_for_conquest(
			state, result, attacker, attacker_strength, target, defender_strength, cb, substate_trigger, war
		);
		return;
	}

	if(nation_trigger) {
		bool liberate = (state.world.cb_type_get_type_bits(cb) & military::cb_flag::po_transfer_provinces) != 0;

		for(auto liberated : state.world.in_nation) {
			if(liberated == attacker) {
				// we can reclaim cores other way
				continue;
			}
			if(liberated == target) {
				continue;
			}
			auto multiplier = utility_multiplier(state, attacker, target, liberated);

			if(multiplier < 0) {
				continue;
			}

			bool is_candidate_valid = trigger::evaluate(
				state,
				nation_trigger,
				trigger::to_generic(target),
				trigger::to_generic(attacker),
				trigger::to_generic(liberated.id)
			);

			if(!is_candidate_valid) {
				continue;
			}

			auto transfer_all_states =
				state.world.cb_type_get_type_bits(cb)
				& military::cb_flag::all_allowed_states;

			if(liberate) {
				if(state_instance_trigger && !transfer_all_states) {
					for(auto soid : state.world.nation_get_state_ownership(target)) {
						auto sid = soid.get_state().id;
						auto probability = probability_of_conquering_state(
							state, attacker, attacker_strength, sid, defender_strength
						);

						auto expectation = utility_of_state(state, target, sid)
							* multiplier
							* probability
							+ utility_of_war;

						if(expectation <= 0) {
							continue;
						}

						assert(military::cb_conditions_satisfied(state, attacker, target, cb));

						auto duplicated = military::war_goal_would_be_duplicate(
							state,
							attacker,
							war,
							target,
							cb,
							state.world.state_instance_get_definition(sid),
							liberated.get_identity_from_identity_holder(),
							dcon::nation_id{}
						);

						if(duplicated) {
							continue;
						}

						result.push_back(possible_cb{
							target,
							dcon::nation_id{},
							liberated.get_identity_from_identity_holder(),
							state.world.state_instance_get_definition(sid),
							cb,
							expectation
						});
					}
				} else {
					assert(military::cb_conditions_satisfied(state, attacker, target, cb));

					auto expectation = utility_of_states_trigger(state, target, liberated, state_instance_trigger)
						* multiplier
						* probability_to_win_war
						+ utility_of_war;

					if(expectation <= 0) {
						continue;
					}

					auto duplicated = military::war_goal_would_be_duplicate(
						state,
						attacker,
						war,
						target,
						cb,
						dcon::state_definition_id{},
						liberated.get_identity_from_identity_holder(),
						dcon::nation_id{}
					);

					if(duplicated) {
						continue;
					}
					
					result.push_back(possible_cb{
						target,
						dcon::nation_id{},
						liberated.get_identity_from_identity_holder(),
						dcon::state_definition_id{},
						cb,
						expectation
					});
					return;
				}
			} else {
				if(state_instance_trigger && !transfer_all_states) {
					for(auto soid : state.world.nation_get_state_ownership(target)) {
						auto sid = soid.get_state().id;
						auto probability = probability_of_conquering_state(
							state, attacker, attacker_strength, sid, defender_strength
						);

						auto expectation = utility_of_state(state, target, sid)
							* multiplier
							* probability
							+ utility_of_war;

						if(expectation <= 0) {
							continue;
						}

						assert(military::cb_conditions_satisfied(state, attacker, target, cb));

						auto duplicated = military::war_goal_would_be_duplicate(
							state,
							attacker,
							war,
							target,
							cb,
							state.world.state_instance_get_definition(sid),
							dcon::national_identity_id{},
							liberated
						);

						if(duplicated) {
							continue;
						}

						result.push_back(possible_cb{
							target,
							liberated,
							dcon::national_identity_id{},
							state.world.state_instance_get_definition(sid),
							cb,
							expectation
						});
					}
				} else {
					assert(military::cb_conditions_satisfied(state, attacker, target, cb));

					auto expectation = utility_of_states_trigger(state, target, liberated, state_instance_trigger)
						* multiplier
						* probability_to_win_war
						+ utility_of_war;

					if(expectation <= 0) {
						continue;
					}

					auto duplicated = military::war_goal_would_be_duplicate(
						state,
						attacker,
						war,
						target,
						cb,
						dcon::state_definition_id{},
						dcon::national_identity_id{},
						liberated
					);

					if(duplicated) {
						continue;
					}

					result.push_back(possible_cb{
						target,
						liberated,
						dcon::national_identity_id{},
						dcon::state_definition_id{},
						cb,
						expectation
					});
					return;
				}
			}
		}
		return;
	}

	// just give up and consider it having 1 utility
	// to prevent not being able to declare wars at all
	// when you really want to declare war

	assert(military::cb_conditions_satisfied(state, attacker, target, cb));
	auto duplicated = military::war_goal_would_be_duplicate(
		state,
		attacker,
		war,
		target,
		cb,
		dcon::state_definition_id{},
		dcon::national_identity_id{},
		dcon::nation_id{}
	);
	
	if (!duplicated && probability_to_win_war > 0.5f)
		result.push_back(possible_cb{ target, dcon::nation_id{}, dcon::national_identity_id{}, dcon::state_definition_id{}, cb, 1.f });
	return;
}


void update_crisis_leaders(sys::state& state) {
	if(state.current_crisis_state == sys::crisis_state::inactive) {
		return;
	}

	auto str_est = estimate_crisis_str(state);

	if(state.crisis_temperature > 75.f || str_est.fast_victory) { // make peace offer
		auto any_victory = str_est.attacker_win || str_est.defender_win;
		auto defender_victory = str_est.defender_win;

		auto will_propose_peace = state.world.nation_get_is_player_controlled(state.primary_crisis_attacker) == false && any_victory;

		auto primary_wg = state.crisis_attacker_wargoals[0];

		// Defender should never agree to full annex
		if(state.crisis_defender == state.primary_crisis_defender) {
			auto bits = state.world.cb_type_get_type_bits(primary_wg.cb);
			if((bits & military::cb_flag::po_annex) != 0)
				will_propose_peace = false;
		}

		if(state.crisis_temperature <= 20.f) {
			will_propose_peace = false;
		}

		if(will_propose_peace) {
			assert(command::can_start_crisis_peace_offer(state, state.primary_crisis_attacker, defender_victory));
			command::execute_start_crisis_peace_offer(state, state.primary_crisis_attacker, defender_victory);
			auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(state.primary_crisis_attacker);

			auto wargoalslist = (defender_victory) ? state.crisis_defender_wargoals : state.crisis_attacker_wargoals;
			for(auto swg : wargoalslist) {
				if(!swg.cb) {
					break;
				}
				auto wg = fatten(state.world, state.world.create_wargoal());
				wg.set_peace_offer_from_peace_offer_item(pending);
				wg.set_added_by(swg.added_by);
				wg.set_associated_state(swg.state);
				wg.set_associated_tag(swg.wg_tag);
				wg.set_secondary_nation(swg.secondary_nation);
				wg.set_target_nation(swg.target_nation);
				wg.set_type(swg.cb);
				assert(command::can_add_to_crisis_peace_offer(state, state.primary_crisis_attacker, swg.added_by, swg.target_nation,
					swg.cb, swg.state, swg.wg_tag, swg.secondary_nation));
			}
			assert(command::can_send_crisis_peace_offer(state, state.primary_crisis_attacker));
			command::execute_send_crisis_peace_offer(state, state.primary_crisis_attacker);
		} else if(state.world.nation_get_is_player_controlled(state.primary_crisis_defender) == false && any_victory) {
			assert(command::can_start_crisis_peace_offer(state, state.primary_crisis_defender, !defender_victory));
			command::execute_start_crisis_peace_offer(state, state.primary_crisis_defender, !defender_victory);
			auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(state.primary_crisis_defender);

			auto wargoalslist = (defender_victory) ? state.crisis_defender_wargoals : state.crisis_attacker_wargoals;
			for(auto swg : wargoalslist) {
				if(!swg.cb) {
					break;
				}
				assert(command::can_add_to_crisis_peace_offer(state, state.primary_crisis_defender, swg.added_by, swg.target_nation,
					swg.cb, swg.state, swg.wg_tag, swg.secondary_nation));
				auto wg = fatten(state.world, state.world.create_wargoal());
				wg.set_peace_offer_from_peace_offer_item(pending);
				wg.set_added_by(swg.added_by);
				wg.set_associated_state(swg.state);
				wg.set_associated_tag(swg.wg_tag);
				wg.set_secondary_nation(swg.secondary_nation);
				wg.set_target_nation(swg.target_nation);
				wg.set_type(swg.cb);

			}
			assert(command::can_send_crisis_peace_offer(state, state.primary_crisis_defender));
			command::execute_send_crisis_peace_offer(state, state.primary_crisis_defender);
		}
	} else if(state.crisis_temperature > 20.f) { // recruit nations
		if(str_est.attacker < str_est.defender && state.world.nation_get_is_player_controlled(state.primary_crisis_attacker) == false) {
			for(auto& par : state.crisis_participants) {
				if(!par.id)
					break;
				if(par.merely_interested) {
					auto other_cbs = state.world.nation_get_available_cbs(par.id);
					dcon::cb_type_id offer_cb;
					dcon::nation_id target;

					[&]() {
						for(auto& op_par : state.crisis_participants) {
							if(!op_par.id)
								break;
							if(!op_par.merely_interested && op_par.supports_attacker == false) {
								for(auto& cb : other_cbs) {
									if(cb.target == op_par.id && ai_offer_cb(state, cb.cb_type) && military::cb_conditions_satisfied(state, par.id, op_par.id, cb.cb_type)) {
										offer_cb = cb.cb_type;
										target = op_par.id;
										return;
									}
								}
								for(auto cb : state.world.in_cb_type) {
									if((cb.get_type_bits() & military::cb_flag::always) != 0) {
										if(ai_offer_cb(state, cb) && military::cb_conditions_satisfied(state, par.id, op_par.id, cb)) {
											offer_cb = cb;
											target = op_par.id;
											return;
										}
									}
								}
							}
						}
						}();

					if(offer_cb) {
						if(military::cb_requires_selection_of_a_state(state, offer_cb)) {
							std::vector <ai::possible_cb> potential_states;
							prepare_list_of_targets_for_cb(state, potential_states, par.id, target, offer_cb, dcon::war_id{ });
							for(auto s : potential_states) {
								if(military::cb_instance_conditions_satisfied(state, par.id, target, offer_cb, s.state_def, dcon::national_identity_id{}, dcon::nation_id{})) {
									diplomatic_message::message m;
									memset(&m, 0, sizeof(diplomatic_message::message));
									m.to = par.id;
									m.from = state.primary_crisis_attacker;
									m.data.crisis_offer.added_by = m.to;
									m.data.crisis_offer.target_nation = target;
									m.data.crisis_offer.secondary_nation = dcon::nation_id{};
									m.data.crisis_offer.state = s.state_def;
									m.data.crisis_offer.wg_tag = dcon::national_identity_id{};
									m.data.crisis_offer.cb = offer_cb;
									m.type = diplomatic_message::type::take_crisis_side_offer;
									diplomatic_message::post(state, m);

									break;
								}
							}
						} else {
							diplomatic_message::message m;
							memset(&m, 0, sizeof(diplomatic_message::message));
							m.to = par.id;
							m.from = state.primary_crisis_attacker;
							m.data.crisis_offer.added_by = m.to;
							m.data.crisis_offer.target_nation = target;
							m.data.crisis_offer.secondary_nation = dcon::nation_id{};
							m.data.crisis_offer.state = dcon::state_definition_id{};
							m.data.crisis_offer.wg_tag = dcon::national_identity_id{};
							m.data.crisis_offer.cb = offer_cb;
							m.type = diplomatic_message::type::take_crisis_side_offer;
							diplomatic_message::post(state, m);
						}
					}
				}
			}
		} else if(str_est.attacker < str_est.defender && state.world.nation_get_is_player_controlled(state.primary_crisis_defender) == false) {
			for(auto& par : state.crisis_participants) {
				if(!par.id)
					break;
				if(par.merely_interested) {
					auto other_cbs = state.world.nation_get_available_cbs(par.id);
					dcon::cb_type_id offer_cb;
					dcon::nation_id target;

					[&]() {
						for(auto& op_par : state.crisis_participants) {
							if(!op_par.id)
								break;
							if(!op_par.merely_interested && op_par.supports_attacker == true) {
								for(auto& cb : other_cbs) {
									if(cb.target == op_par.id && ai_offer_cb(state, cb.cb_type) && military::cb_conditions_satisfied(state, par.id, op_par.id, cb.cb_type)) {
										offer_cb = cb.cb_type;
										target = op_par.id;
										return;
									}
								}
								for(auto cb : state.world.in_cb_type) {
									if((cb.get_type_bits() & military::cb_flag::always) != 0) {
										if(ai_offer_cb(state, cb) && military::cb_conditions_satisfied(state, par.id, op_par.id, cb)) {
											offer_cb = cb;
											target = op_par.id;
											return;
										}
									}
								}
							}
						}
						}();

					if(offer_cb) {
						if(military::cb_requires_selection_of_a_state(state, offer_cb)) {
							std::vector <ai::possible_cb> potential_states;
							prepare_list_of_targets_for_cb(state, potential_states, par.id, target, offer_cb, dcon::war_id{ });
							for(auto s : potential_states) {
								if(military::cb_instance_conditions_satisfied(state, par.id, target, offer_cb, s.state_def, dcon::national_identity_id{}, dcon::nation_id{})) {

									diplomatic_message::message m;
									memset(&m, 0, sizeof(diplomatic_message::message));
									m.to = par.id;
									m.from = state.primary_crisis_defender;
									m.data.crisis_offer.added_by = m.to;
									m.data.crisis_offer.target_nation = target;
									m.data.crisis_offer.secondary_nation = dcon::nation_id{};
									m.data.crisis_offer.state = s.state_def;
									m.data.crisis_offer.wg_tag = dcon::national_identity_id{};
									m.data.crisis_offer.cb = offer_cb;
									m.type = diplomatic_message::type::take_crisis_side_offer;
									diplomatic_message::post(state, m);

									break;
								}
							}
						} else {
							diplomatic_message::message m;
							memset(&m, 0, sizeof(diplomatic_message::message));
							m.to = par.id;
							m.from = state.primary_crisis_defender;
							m.data.crisis_offer.added_by = m.to;
							m.data.crisis_offer.target_nation = target;
							m.data.crisis_offer.secondary_nation = dcon::nation_id{};
							m.data.crisis_offer.state = dcon::state_definition_id{};
							m.data.crisis_offer.wg_tag = dcon::national_identity_id{};
							m.data.crisis_offer.cb = offer_cb;
							m.type = diplomatic_message::type::take_crisis_side_offer;
							diplomatic_message::post(state, m);
						}
					}
				}
			}
		}
	}
}

bool will_accept_crisis_peace_offer(sys::state& state, dcon::nation_id to, bool is_concession, bool missing_wg) {

	auto primary_wg = state.crisis_attacker_wargoals[0];

	// Defender should never agree to full annex
	if(state.crisis_defender == state.primary_crisis_defender && to == state.crisis_defender) {
		auto bits = state.world.cb_type_get_type_bits(primary_wg.cb);
		if((bits & military::cb_flag::po_annex) != 0)
			return false;
	}

	if(state.crisis_temperature <= 20.f) {
		return false;
	}

	auto str_est = estimate_crisis_str(state);

	if(state.crisis_temperature > 75.f || str_est.fast_victory) {
		if(to == state.primary_crisis_attacker) {
			if(is_concession) {
				return true;
			}
			if(str_est.defender_win) {
				return true;
			}

			return false;
		} else if(to == state.primary_crisis_defender) {
			if(is_concession)
				return true;

			if(str_est.attacker_win)
				return true;

			return false;
		}
	}
	return false;
}

bool will_accept_crisis_peace_offer(sys::state& state, dcon::nation_id to, dcon::peace_offer_id peace) {
	auto primary_wg = state.crisis_attacker_wargoals[0];

	// Defender should never agree to full annex
	if(state.crisis_defender == state.primary_crisis_defender && to == state.crisis_defender) {
		auto bits = state.world.cb_type_get_type_bits(primary_wg.cb);
		if((bits & military::cb_flag::po_annex) != 0)
			return false;
	}

	if(state.crisis_temperature <= 20.f) {
		return false;
	}

	auto str_est = estimate_crisis_str(state);

	if(state.crisis_temperature > 75.f || str_est.fast_victory) {
		if(to == state.primary_crisis_attacker) {
			if(str_est.defender_win)
				return true;

			if(!state.world.peace_offer_get_is_concession(peace))
				return false;

			bool missing_wg = false;
			for(auto swg : state.crisis_attacker_wargoals) {
				bool found_wg = false;
				for(auto item : state.world.peace_offer_get_peace_offer_item(peace)) {
					auto wg = item.get_wargoal();
					if(wg.get_type() == swg.cb && wg.get_associated_state() == swg.state && wg.get_added_by() == swg.added_by && wg.get_target_nation() == swg.target_nation &&
						wg.get_associated_tag() == swg.wg_tag) {
						found_wg = true; continue;
					}
				}

				if(!found_wg) {
					return false;
				}
			}
			return true;

		} else if(to == state.primary_crisis_defender) {
			if(str_est.attacker_win)
				return true;

			if(!state.world.peace_offer_get_is_concession(peace))
				return false;

			bool missing_wg = false;
			for(auto swg : state.crisis_defender_wargoals) {
				bool found_wg = false;
				for(auto item : state.world.peace_offer_get_peace_offer_item(peace)) {
					auto wg = item.get_wargoal();
					if(wg.get_type() == swg.cb && wg.get_associated_state() == swg.state && wg.get_added_by() == swg.added_by && wg.get_target_nation() == swg.target_nation &&
						wg.get_associated_tag() == swg.wg_tag) {
						found_wg = true; continue;
					}
				}

				if(!found_wg) {
					return false;
				}
			}

			return true;
		}
	}
	return false;
}

void update_war_intervention(sys::state& state) {
	for(auto& gp : state.great_nations) {
		if(state.world.nation_get_is_player_controlled(gp.nation) == false && state.world.nation_get_is_at_war(gp.nation) == false) {
			bool as_attacker = false;
			dcon::war_id intervention_target;
			[&]() {
				for(auto w : state.world.in_war) {
					//GPs will try to intervene in wars to protect smaller nations in the same cultural union
					if(command::can_intervene_in_war(state, gp.nation, w, false)) {
						auto par = state.world.war_get_primary_defender(w);
						if(state.world.nation_get_primary_culture(gp.nation).get_group_from_culture_group_membership() == state.world.nation_get_primary_culture(par).get_group_from_culture_group_membership()
							&& !nations::is_great_power(state, par)
						) {
							intervention_target = w;
							return;
						}
					}
					if(w.get_is_great()) {
						if(command::can_intervene_in_war(state, gp.nation, w, false)) {
							for(auto par : w.get_war_participant()) {
								if(par.get_is_attacker() && military::can_use_cb_against(state, gp.nation, par.get_nation())) {
									intervention_target = w;
									return;
								}
							}
						}
						if(command::can_intervene_in_war(state, gp.nation, w, true)) {
							for(auto par : w.get_war_participant()) {
								if(!par.get_is_attacker() && military::can_use_cb_against(state, gp.nation, par.get_nation())) {
									intervention_target = w;
									as_attacker = true;
									return;
								}
							}
						}
					} else if(military::get_role(state, w, state.world.nation_get_ai_rival(gp.nation)) == military::war_role::attacker) {
						if(command::can_intervene_in_war(state, gp.nation, w, false)) {
							intervention_target = w;
							return;
						}
					}
				}
				}();
			if(intervention_target) {
				assert(command::can_intervene_in_war(state, gp.nation, intervention_target, as_attacker));
				command::execute_intervene_in_war(state, gp.nation, intervention_target, as_attacker);
			}
		}
	}
}


void sort_possible_justification_cbs(std::vector<possible_cb>& result, sys::state& state, dcon::nation_id n, dcon::nation_id target) {
	result.clear();

	// AI can go after po_demand_state and po_annex and po_transfer_provinces
	for(auto cb : state.world.in_cb_type) {
		auto bits = state.world.cb_type_get_type_bits(cb);
		if((bits & (military::cb_flag::po_demand_state | military::cb_flag::po_annex | military::cb_flag::po_transfer_provinces)) == 0)
			continue;

		if(!military::cb_conditions_satisfied(state, n, target, cb))
			continue;
		auto sl = state.world.nation_get_in_sphere_of(target);
		if(sl == n)
			continue;

		prepare_list_of_targets_for_cb(state, result, n, target, cb, dcon::war_id{ });
	}

	// Prioritize CB types
	sort_prepared_list_of_cb(state, result);
}

possible_cb pick_fabrication_type(sys::state& state, dcon::nation_id from, dcon::nation_id target) {
	static std::vector<possible_cb> possibilities;

	sort_possible_justification_cbs(possibilities, state, from, target);
	// Uncivilized nations are more aggressive to westernize faster
	float infamy_limit = state.world.nation_get_is_civilized(from) ? state.defines.badboy_limit / 2.f : state.defines.badboy_limit;

	if(!possibilities.empty()) {
		// AI only goes for top priority choice
		auto possibility = possibilities[0];

		// AI doesn't go over infamy limit;
		if(military::cb_requires_selection_of_a_state(state, possibility.cb)) {
			if(state.world.nation_get_infamy(from) + military::cb_infamy(state, possibility.cb, target, possibility.state_def) > infamy_limit)
				return possible_cb{};
		} else {
			if(state.world.nation_get_infamy(from) + military::cb_infamy(state, possibility.cb, target) > infamy_limit)
				return possible_cb{};
		}

		// Make sure no country higher than originator on rank is justifying on that target
		for(auto n : state.nations_by_rank) {
			if(n == from) {
				break;
			}

			auto is_discovered = state.world.nation_get_constructing_cb_is_discovered(n);

			if(!is_discovered) {
				continue;
			}

			auto target_nation = state.world.nation_get_constructing_cb_target(n);
			auto target_state = state.world.nation_get_constructing_cb_target_state(n);

			if(possibility.target == target_nation && possibility.state_def == target_state) {
				return possible_cb{};
			}
		}

		return possibility;
	} else {
		return possible_cb{};
	}
}

bool valid_construction_target(sys::state& state, dcon::nation_id from, dcon::nation_id target) {
	// Copied from commands.cpp:can_fabricate_cb()
	if(from == target)
		return false;
	if(state.world.nation_get_constructing_cb_type(from))
		return false;
	auto ol = state.world.nation_get_overlord_as_subject(from);
	if(state.world.overlord_get_ruler(ol) && state.world.overlord_get_ruler(ol) != target)
		return false;
	if(state.world.nation_get_in_sphere_of(target) == from)
		return false;
	if(military::are_at_war(state, target, from))
		return false;
	if(military::has_truce_with(state, target, from))
		return false;
	if(state.world.nation_get_owned_province_count(target) == 0)
		return false;
	auto sl = state.world.nation_get_in_sphere_of(target);
	if(sl == from)
		return false;
	return true;
}

void update_cb_fabrication(sys::state& state) {
	for(auto nid : state.nations_by_rank) {
		auto n = dcon::fatten(state.world, nid);

		if(!n.get_is_player_controlled() && n.get_owned_province_count() > 0) {
			if(n.get_is_at_war())
				continue;
			// Uncivilized nations are more aggressive to westernize faster
			float infamy_limit = state.world.nation_get_is_civilized(n) ? state.defines.badboy_limit / 2.f : state.defines.badboy_limit;
			if(n.get_infamy() > infamy_limit)
				continue;
			if(n.get_constructing_cb_type())
				continue;
			auto ol = n.get_overlord_as_subject().get_ruler().id;
			if(n.get_ai_rival()
				&& n.get_ai_rival().get_in_sphere_of() != n
				&& (!ol || ol == n.get_ai_rival())
				&& !military::are_at_war(state, n, n.get_ai_rival())
				&& !military::can_use_cb_against(state, n, n.get_ai_rival())
				&& !military::has_truce_with(state, n, n.get_ai_rival())) {

				auto cb = pick_fabrication_type(state, n, n.get_ai_rival());
				if(cb.cb) {
					n.set_constructing_cb_target(n.get_ai_rival());
					n.set_constructing_cb_type(cb.cb);
					n.set_constructing_cb_target_state(cb.state_def);
					continue;
				}
			}

			static std::vector<std::pair<dcon::nation_id, float>> possible_targets;
			possible_targets.clear();
			float total_weight = 0.f;
			for(auto i : state.world.in_nation) {
				if(valid_construction_target(state, n, i)
				&& !military::has_truce_with(state, n, i)) {
					auto weight = 1.f / (province::direct_distance(
						state,
						state.world.nation_get_capital(n),
						state.world.nation_get_capital(i)
					) + 1.f);

					// if nations are neighbors, then it's much easier to prepare a successful invasion:
					if(state.world.get_nation_adjacency_by_nation_adjacency_pair(n, i)) {
						weight *= 100.f;
					} else {
						// never fabricate on targets you are unable to conquer:
						if(state.world.nation_get_central_ports(n) == 0 || state.world.nation_get_central_ports(i) == 0)
							weight = 0.f;
					}

					// uncivs have higher priority
					if(!i.get_is_civilized()) {
						weight *= 2.f;
					}

					// nations with strong military have lower weight
					weight = weight / (estimate_strength(state, i) + 1.f);

					total_weight += weight;
					possible_targets.push_back({ i.id, weight });
				}
			}
			if(!possible_targets.empty()) {
				auto random_value = uint32_t(rng::get_random(state, uint32_t(n.id.index())) >> 2);
				auto limited_value = rng::reduce(random_value, 1000);
				auto random_float = float(limited_value) / 1000.f * total_weight;
				auto acc = 0.f;
				dcon::nation_id target{ };
				for(auto& weighted_nation : possible_targets) {
					acc += weighted_nation.second;
					if(acc > random_float) {
						target = weighted_nation.first;
						break;
					}
				}
				if(target) {
					if(auto pcb = pick_fabrication_type(state, n, target); pcb.cb) {
						n.set_constructing_cb_target(target);
						n.set_constructing_cb_type(pcb.cb);
						n.set_constructing_cb_target_state(pcb.state_def);
						continue;
					}
				}
			}
		}
	}
}

bool will_join_war(sys::state& state, dcon::nation_id n, dcon::war_id w, bool as_attacker) {
	/* Forbid war between uncivs against civs of other continent except if they are a sphereling or substate */
	if(bool(state.defines.alice_unciv_civ_forbid_war) && !state.world.nation_get_is_civilized(n)
	&& !state.world.nation_get_is_substate(n) && !state.world.nation_get_in_sphere_of(n)
	&& !state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(n))) {
		auto pa = state.world.war_get_primary_attacker(w);
		auto pd = state.world.war_get_primary_defender(w);
		auto pa_cap = state.world.nation_get_capital(pa);
		auto pd_cap = state.world.nation_get_capital(pd);
		auto cap = state.world.nation_get_capital(n);
		if(state.world.province_get_continent(pa_cap) != state.world.province_get_continent(cap))
			return false;
		if(state.world.province_get_continent(pd_cap) != state.world.province_get_continent(cap))
			return false;
	}

	if(!as_attacker)
		return true;
	for(auto par : state.world.war_get_war_participant(w)) {
		if(par.get_is_attacker() == !as_attacker) {
			// Could use a CB against this nation?
			if(military::can_use_cb_against(state, n, par.get_nation()))
				return true;
			// Eager to absolutely demolish our rival if possible
			if(state.world.nation_get_ai_rival(n) == par.get_nation())
				return true;
		}
	}
	return false;
}

void sort_available_cbs(std::vector<possible_cb>& result, sys::state& state, dcon::nation_id n, dcon::war_id w) {
	result.clear();
	auto is_attacker = military::get_role(state, w, n) == military::war_role::attacker;
	for(auto par : state.world.war_get_war_participant(w)) {
		if(par.get_is_attacker() != is_attacker) {
			for(auto& cb : state.world.nation_get_available_cbs(n)) {
				if(cb.target == par.get_nation())
					prepare_list_of_targets_for_cb(state, result, n, par.get_nation(), cb.cb_type, w);
			}
			for(auto cb : state.world.in_cb_type) {
				if((cb.get_type_bits() & military::cb_flag::always) != 0) {
					prepare_list_of_targets_for_cb(state, result, n, par.get_nation(), cb, w);
				}
			}
		}
	}

	sort_prepared_list_of_cb(state, result);
}

void sort_available_declaration_cbs(std::vector<possible_cb>& result, sys::state& state, dcon::nation_id n, dcon::nation_id target) {
	result.clear();

	// Justified CBs
	auto other_cbs = state.world.nation_get_available_cbs(n);
	for(auto& cb : other_cbs) {
		if(cb.target == target)
			prepare_list_of_targets_for_cb(state, result, n, target, cb.cb_type, dcon::war_id{ });
	}
	// Always available CBs (incl acquire_core_state and annex_core_country)
	for(auto cb : state.world.in_cb_type) {
		if((cb.get_type_bits() & military::cb_flag::always) != 0) {
			prepare_list_of_targets_for_cb(state, result, n, target, cb, dcon::war_id { });
		}
	}

	sort_prepared_list_of_cb(state, result);
}

void add_free_ai_cbs_to_war(sys::state& state, dcon::nation_id n, dcon::war_id w) {
	bool is_attacker = military::is_attacker(state, w, n);
	if(!is_attacker && military::defenders_have_status_quo_wargoal(state, w))
		return;
	if(is_attacker && military::attackers_have_status_quo_wargoal(state, w))
		return;

	bool added = false;
	do {
		added = false;
		static std::vector<possible_cb> potential;
		sort_available_cbs(potential, state, n, w);
		for(auto& p : potential) {
			if(!military::war_goal_would_be_duplicate(state, n, w, p.target, p.cb, p.state_def, p.associated_tag, p.secondary_nation)) {
				military::add_wargoal(state, w, n, p.target, p.cb, p.state_def, p.associated_tag, p.secondary_nation);
				nations::adjust_relationship(state, n, p.target, state.defines.addwargoal_relation_on_accept);
				added = true;
			}
		}
	} while(added);

}

dcon::cb_type_id pick_wargoal_extra_cb_type(sys::state& state, dcon::nation_id from, dcon::nation_id target, bool is_great) {
	static std::vector<dcon::cb_type_id> possibilities;
	possibilities.clear();

	auto free_infamy = state.defines.badboy_limit - state.world.nation_get_infamy(from);

	auto multiplier = is_great ? state.defines.gw_justify_cb_badboy_impact : 1.f;

	for(auto c : state.world.in_cb_type) {
		auto bits = state.world.cb_type_get_type_bits(c);
		if((bits & (military::cb_flag::always | military::cb_flag::is_not_constructing_cb)) != 0)
			continue;
		if((bits & (military::cb_flag::po_demand_state | military::cb_flag::po_annex | military::cb_flag::po_transfer_provinces)) == 0)
			continue;
		if(military::cb_infamy(state, c, target) * multiplier > free_infamy)
			continue;
		if(!military::cb_conditions_satisfied(state, from, target, c))
			continue;
		possibilities.push_back(c);
	}

	if(!possibilities.empty()) {
		return possibilities[rng::reduce(uint32_t(rng::get_random(state, uint32_t((from.index() << 3) ^ target.index()))), uint32_t(possibilities.size()))];
	} else {
		return dcon::cb_type_id{};
	}
}

dcon::nation_id pick_wargoal_target(sys::state& state, dcon::nation_id from, dcon::war_id w, bool is_attacker) {

	if(is_attacker && military::get_role(state, w, state.world.nation_get_ai_rival(from)) == military::war_role::defender)
		return state.world.nation_get_ai_rival(from);
	if(!is_attacker && military::get_role(state, w, state.world.nation_get_ai_rival(from)) == military::war_role::attacker)
		return state.world.nation_get_ai_rival(from);

	static std::vector<dcon::nation_id> possibilities;
	possibilities.clear();

	for(auto par : state.world.war_get_war_participant(w)) {
		if(par.get_is_attacker() != is_attacker) {
			if(state.world.get_nation_adjacency_by_nation_adjacency_pair(from, par.get_nation()))
				possibilities.push_back(par.get_nation().id);
		}
	}
	if(possibilities.empty()) {
		for(auto par : state.world.war_get_war_participant(w)) {
			if(par.get_is_attacker() != is_attacker) {
				if(nations::is_great_power(state, par.get_nation()))
					possibilities.push_back(par.get_nation().id);
			}
		}
	}
	if(!possibilities.empty()) {
		return possibilities[rng::reduce(uint32_t(rng::get_random(state, uint32_t(from.index() ^ 3))), uint32_t(possibilities.size()))];
	} else {
		return dcon::nation_id{};
	}
}

void add_wargoal_to_war(sys::state& state, dcon::nation_id n, dcon::war_id w) {
	auto rval = rng::get_random(state, n.index() ^ w.index() << 2);
	if((rval & 1) == 0)
		return;

	if(n == state.world.war_get_primary_attacker(w) || n == state.world.war_get_primary_defender(w)) {
		if(((rval >> 1) & 1) == 0) {
			add_free_ai_cbs_to_war(state, n, w);
		}
	}

	auto totalpop = state.world.nation_get_demographics(n, demographics::total);
	auto jingoism_perc = totalpop > 0 ? state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.jingoism)) / totalpop : 0.0f;

	if(jingoism_perc < state.defines.wargoal_jingoism_requirement * state.defines.gw_wargoal_jingoism_requirement_mod)
		return;

	bool attacker = military::get_role(state, w, n) == military::war_role::attacker;
	auto spare_ws = attacker
		? (std::clamp(30.f + military::primary_warscore(state, w) * 2.f, 30.f, 100.f) - military::attacker_peace_cost(state, w))
		: (std::clamp(30.f - military::primary_warscore(state, w) * 2.f, 30.f, 100.f) - military::defender_peace_cost(state, w));

	if(spare_ws < 1.0f)
		return;

	auto target = pick_wargoal_target(state, n, w, attacker);
	if(!target)
		return;

	auto cb = pick_wargoal_extra_cb_type(state, n, target, state.world.war_get_is_great(w));
	if(!cb)
		return;

	auto multiplier = state.world.war_get_is_great(w) ? state.defines.gw_justify_cb_badboy_impact : 1.f;

	static std::vector<possible_cb> result;
	result.clear();
	prepare_list_of_targets_for_cb(state, result, n, target, cb, w);
	sort_prepared_list_of_cb(state, result);

	for(size_t i = 0; i < result.size(); i++) {
		if(!command::can_add_war_goal(
			state,
			n,
			w,
			target,
			result[i].cb,
			result[i].state_def,
			result[i].associated_tag,
			result[i].secondary_nation)
		) {
			continue;
		}		
		auto predicted_cost = military::peace_cost(
			state,
			w,
			result[i].cb,
			n,
			target,
			result[i].secondary_nation,
			result[i].state_def,
			result[i].associated_tag
		);
		if(predicted_cost > spare_ws) {
			continue;
		}

		military::add_wargoal(state, w, n, target, cb, result[0].state_def, result[0].associated_tag, result[0].secondary_nation);
		nations::adjust_relationship(state, n, target, state.defines.addwargoal_relation_on_accept);
		state.world.nation_get_infamy(n) += military::cb_infamy(state, cb, target) * multiplier;
		return;
	}
}

void add_wargoals(sys::state& state) {
	for(auto w : state.world.in_war) {
		for(auto par : w.get_war_participant()) {
			if(par.get_nation().get_is_player_controlled() == false) {
				add_wargoal_to_war(state, par.get_nation(), w);
			}
		}
	}
}

// Every single owned core of the country is occupied
bool has_cores_occupied(sys::state& state, dcon::nation_id n) {
	if(bool(state.defines.alice_surrender_on_cores_lost)) {
		auto i = state.world.nation_get_identity_from_identity_holder(n);
		auto cores = state.world.national_identity_get_core(i);
		bool has_owned_cores = false;
		for(auto c : cores) {
			if(c.get_province().get_nation_from_province_ownership() == n) {
				has_owned_cores = true;
			} if(c.get_province().get_nation_from_province_control() == n) {
				return false;
			}
		}
		auto pc = state.world.nation_get_province_control(n); //no cores or some cores are occupied but some are not
		return has_owned_cores || pc.begin() == pc.end(); //controls anything?
	}
	return false;
}

void make_peace_offers(sys::state& state) {
	auto send_offer_up_to = [&](dcon::nation_id from, dcon::nation_id to, dcon::war_id w, bool attacker, int32_t score_max, bool concession) {
		if(auto off = state.world.nation_get_peace_offer_from_pending_peace_offer(from); off) {
			if(state.world.peace_offer_get_is_crisis_offer(off) == true || state.world.peace_offer_get_war_from_war_settlement(off))
				return; // offer in flight
			state.world.delete_peace_offer(off); // else -- offer has been already resolved and was just pending gc
		}

		assert(command::can_start_peace_offer(state, from, to, w, concession));
		command::execute_start_peace_offer(state, from, to, w, concession);
		auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(from);
		if(!pending)
			return;

		score_max = std::min(score_max, 100);
		int32_t current_value = 0;
		for(auto wg : state.world.war_get_wargoals_attached(w)) {
			if((military::is_attacker(state, w, wg.get_wargoal().get_added_by()) == attacker) == !concession) {
				auto goal_cost = military::peace_cost(state, w, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(), wg.get_wargoal().get_target_nation(), wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(), wg.get_wargoal().get_associated_tag());
				if(current_value + goal_cost <= score_max) {
					current_value += goal_cost;
					state.world.force_create_peace_offer_item(pending, wg.get_wargoal().id);
				}
			}
		}

		assert(command::can_send_peace_offer(state, from));
		command::execute_send_peace_offer(state, from);
		};

	for(auto w : state.world.in_war) {
		if((w.get_primary_attacker().get_is_player_controlled() == false || w.get_primary_defender().get_is_player_controlled() == false)
		&& w.get_primary_attacker().get_owned_province_count() > 0
		&& w.get_primary_defender().get_owned_province_count() > 0) {
			//postpone until military gc does magic
			if(military::get_role(state, w, w.get_primary_attacker()) != military::war_role::attacker)
				continue;
			if(military::get_role(state, w, w.get_primary_defender()) != military::war_role::defender)
				continue;

			auto overall_score = military::primary_warscore(state, w);
			if(overall_score >= 0) { // attacker winning
				bool defender_surrender = has_cores_occupied(state, w.get_primary_defender());
				auto total_po_cost = military::attacker_peace_cost(state, w);
				if(w.get_primary_attacker().get_is_player_controlled() == false) { // attacker makes offer
					if(defender_surrender || (overall_score >= 100 || (overall_score >= 50 && overall_score >= total_po_cost * 2))) {
						send_offer_up_to(w.get_primary_attacker(), w.get_primary_defender(), w, true, int32_t(overall_score), false);
						continue;
					}
					if(w.get_primary_defender().get_is_player_controlled() == false) {
						auto war_duration = state.current_date.value - state.world.war_get_start_date(w).value;
						if(war_duration >= 365) {
							float willingness_factor = float(war_duration - 365) * 10.f / 365.0f;
							if(defender_surrender || (overall_score > (total_po_cost - willingness_factor) && (-overall_score / 2 + total_po_cost - willingness_factor) < 0)) {
								send_offer_up_to(w.get_primary_attacker(), w.get_primary_defender(), w, true, int32_t(total_po_cost), false);
								continue;
							}
						}
					}
				} else if(w.get_primary_defender().get_is_player_controlled() == false) { // defender may surrender
					if(defender_surrender || (overall_score >= 100 || (overall_score >= 50 && overall_score >= total_po_cost * 2))) {
						send_offer_up_to(w.get_primary_defender(), w.get_primary_attacker(), w, false, int32_t(overall_score), true);
						continue;
					}
				}
			} else {
				bool attacker_surrender = has_cores_occupied(state, w.get_primary_attacker());
				auto total_po_cost = military::defender_peace_cost(state, w);
				if(w.get_primary_defender().get_is_player_controlled() == false) { // defender makes offer
					if(attacker_surrender || (overall_score <= -100 || (overall_score <= -50 && overall_score <= -total_po_cost * 2))) {
						send_offer_up_to(w.get_primary_defender(), w.get_primary_attacker(), w, false, int32_t(-overall_score), false);
						continue;
					}
					if(w.get_primary_attacker().get_is_player_controlled() == false) {
						auto war_duration = state.current_date.value - state.world.war_get_start_date(w).value;
						if(war_duration >= 365) {
							float willingness_factor = float(war_duration - 365) * 10.f / 365.0f;
							if(attacker_surrender || (-overall_score > (total_po_cost - willingness_factor) && (overall_score / 2 + total_po_cost - willingness_factor) < 0)) {
								send_offer_up_to(w.get_primary_defender(), w.get_primary_attacker(), w, false, int32_t(total_po_cost), false);
								continue;
							}
						}
					}
				} else if(w.get_primary_attacker().get_is_player_controlled() == false) { // attacker may surrender
					if(attacker_surrender || (overall_score <= -100 || (overall_score <= -50 && overall_score <= -total_po_cost * 2))) {
						send_offer_up_to(w.get_primary_attacker(), w.get_primary_defender(), w, true, int32_t(-overall_score), true);
						continue;
					}
				}
			}
		}
	}
}

bool will_accept_peace_offer_value(sys::state& state,
	dcon::nation_id n, // TO country reviews peace offer
	dcon::nation_id from, // FROM country sends peace offer
	dcon::nation_id prime_attacker, dcon::nation_id prime_defender,
	float primary_warscore,
	float scoreagainst_me, // Score against TO country
	bool offer_from_attacker,
	bool concession, // False if FROM country demands from TO. True if FROM submits goals to TO.
	int32_t overall_po_value, // Score value of the proposed deal
	int32_t my_po_target, // How much warscore worth of wargoals does TO country have against FROM
	int32_t target_personal_po_value, int32_t potential_peace_score_against,
	int32_t my_side_against_target, int32_t my_side_peace_cost,
	int32_t war_duration, // Length of the war in days
	bool contains_sq // Does peace offer contain status quo
) {
	bool is_attacking = !offer_from_attacker;

	auto overall_score = primary_warscore;
	if(concession && overall_score <= -50.0f) {
		return true;
	}

	if(!concession) {
		overall_po_value = -overall_po_value;
	}
	if(overall_po_value < -100)
		return false;

	auto personal_score_saved = target_personal_po_value - potential_peace_score_against;
	auto war_exhaustion = state.world.nation_get_war_exhaustion(n); // War exhaustion between 0 and 100
	// Since we have functional blockades now, it's reasonable to account for war_exhaustion in AI willingness to peace out.
	float willingness_factor = float(war_duration - 365) * 10.f / 365.0f + war_exhaustion;

	// War-ending peace from primary participant to primary participant (concession & demand)
	if((prime_attacker == n || prime_defender == n) && (prime_attacker == from || prime_defender == from)) {
		if((overall_score <= -50 || war_exhaustion > state.defines.alice_ai_war_exhaustion_readiness_limit) && overall_score <= overall_po_value * 2)
			return true;

		// Forced peace when 100 warscore
		if(overall_score <= -100 and overall_po_value <= 100)
			return true;

		if(concession && my_side_peace_cost <= overall_po_value)
			return true; // offer contains everything
		if(war_duration < 365) {
			return false;
		}

		if(overall_score >= 0) {
			if(concession && ((overall_score * 2 - overall_po_value - willingness_factor) < 0))
				return true;
		} else {
			if((overall_score - willingness_factor) <= overall_po_value && (overall_score / 2 - overall_po_value - willingness_factor) < 0)
				return true;
		}
	}
	// Peace offer from secondary participant to primary participant (concession)
	else if((prime_attacker == n || prime_defender == n) && concession) {
		if(scoreagainst_me > 50 || war_exhaustion > state.defines.alice_ai_war_exhaustion_readiness_limit)
			return true;

		if(overall_score < 0.0f) { // we are losing
			if(my_side_against_target - scoreagainst_me <= overall_po_value + personal_score_saved)
				return true;
		} else {
			if(my_side_against_target <= overall_po_value)
				return true;
		}

	}
	// Peace offer to secondary participant (concession & demand)
	else {
		if(contains_sq)
			return false;

		if((scoreagainst_me > 50 || war_exhaustion > state.defines.alice_ai_war_exhaustion_readiness_limit) && scoreagainst_me > -overall_po_value * 2)
			return true;

		// Forced peace when 100 warscore
		if(scoreagainst_me >= 100 and overall_po_value <= 100)
			return true;

		if(overall_score < 0.0f) { // we are losing
			if(personal_score_saved > 0 && scoreagainst_me + personal_score_saved - my_po_target >= -overall_po_value)
				return true;

		} else { // we are winning
			if(my_po_target > 0 && my_po_target >= overall_po_value)
				return true;
		}
	}

	//will accept anything
	if(has_cores_occupied(state, n))
		return true;
	return false;
}

bool will_accept_peace_offer(sys::state& state, dcon::nation_id n, dcon::nation_id from, dcon::peace_offer_id p) {
	auto w = state.world.peace_offer_get_war_from_war_settlement(p);
	auto prime_attacker = state.world.war_get_primary_attacker(w);
	auto prime_defender = state.world.war_get_primary_defender(w);
	bool is_attacking = military::is_attacker(state, w, n);
	bool contains_sq = false;

	auto overall_score = military::primary_warscore(state, w);
	if(!is_attacking)
		overall_score = -overall_score;

	auto concession = state.world.peace_offer_get_is_concession(p);

	if(concession && overall_score <= -50.0f) {
		return true;
	}

	int32_t overall_po_value = 0;
	int32_t personal_po_value = 0;
	int32_t my_po_target = 0;
	for(auto wg : state.world.peace_offer_get_peace_offer_item(p)) {
		auto wg_value = military::peace_cost(state, w, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(), wg.get_wargoal().get_target_nation(), wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(), wg.get_wargoal().get_associated_tag());
		overall_po_value += wg_value;

		if((wg.get_wargoal().get_type().get_type_bits() & military::cb_flag::po_status_quo) != 0)
			contains_sq = true;

		if(wg.get_wargoal().get_target_nation() == n) {
			personal_po_value += wg_value;
		}
	}
	if(!concession) {
		overall_po_value = -overall_po_value;
	}
	if(overall_po_value < -100)
		return false;


	int32_t potential_peace_score_against = 0;
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if(wg.get_wargoal().get_target_nation() == n || wg.get_wargoal().get_added_by() == n) {
			auto wg_value = military::peace_cost(state, w, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(), n, wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(), wg.get_wargoal().get_associated_tag());

			if(wg.get_wargoal().get_target_nation() == n && (wg.get_wargoal().get_added_by() == from || from == prime_attacker || from == prime_defender)) {
				potential_peace_score_against += wg_value;
			}
			if(wg.get_wargoal().get_added_by() == n && (wg.get_wargoal().get_target_nation() == from || from == prime_attacker || from == prime_defender)) {
				my_po_target += wg_value;
			}
		}
	}
	auto personal_score_saved = personal_po_value - potential_peace_score_against;

	auto war_duration = state.current_date.value - state.world.war_get_start_date(w).value;
	auto war_exhaustion = state.world.nation_get_war_exhaustion(n); // War exhaustion between 0 and 100
	// Since we have functional blockades now, it's reasonable to account for war_exhaustion in AI willingness to peace out.
	float willingness_factor = float(war_duration - 365) * 10.f / 365.0f + war_exhaustion;

	// War-ending peace from primary participant to primary participant (concession & demand)
	if((prime_attacker == n || prime_defender == n) && (prime_attacker == from || prime_defender == from)) {
		if((overall_score <= -50 || war_exhaustion > state.defines.alice_ai_war_exhaustion_readiness_limit) && overall_score <= overall_po_value * 2)
			return true;

		if(concession && (is_attacking ? military::attacker_peace_cost(state, w) : military::defender_peace_cost(state, w)) <= overall_po_value)
			return true; // offer contains everything
		if(war_duration < 365) {
			return false;
		}
		if(overall_score >= 0) {
			if(concession && ((overall_score * 2 - overall_po_value - willingness_factor) < 0))
				return true;
		} else {
			if((overall_score - willingness_factor) <= overall_po_value && (overall_score / 2 - overall_po_value - willingness_factor) < 0)
				return true;
		}

	}
	// Peace offer from secondary participant to primary participant (concession)
	else if((prime_attacker == n || prime_defender == n) && concession) {
		auto scoreagainst_me = military::directed_warscore(state, w, from, n);

		if(scoreagainst_me > 50 || war_exhaustion > state.defines.alice_ai_war_exhaustion_readiness_limit)
			return true;

		int32_t my_side_against_target = 0;
		for(auto wg : state.world.war_get_wargoals_attached(w)) {
			if(wg.get_wargoal().get_target_nation() == from) {
				auto wg_value = military::peace_cost(state, w, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(), n, wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(), wg.get_wargoal().get_associated_tag());

				my_side_against_target += wg_value;
			}
		}

		if(overall_score < 0.0f) { // we are losing
			if(my_side_against_target - scoreagainst_me <= overall_po_value + personal_score_saved)
				return true;
		} else {
			if(my_side_against_target <= overall_po_value)
				return true;
		}
	}
	// Peace offer to secondary participant (concession & demand)
	else {
		if(contains_sq)
			return false;

		auto scoreagainst_me = military::directed_warscore(state, w, from, n);
		if((scoreagainst_me > 50 || war_exhaustion > state.defines.alice_ai_war_exhaustion_readiness_limit) && scoreagainst_me > -overall_po_value * 2)
			return true;

		if(overall_score < 0.0f) { // we are losing
			if(personal_score_saved > 0 && scoreagainst_me + personal_score_saved - my_po_target >= -overall_po_value)
				return true;

		} else { // we are winning
			if(my_po_target > 0 && my_po_target >= overall_po_value)
				return true;
		}
	}

	//will accept anything
	if(has_cores_occupied(state, n))
		return true;
	return false;
}


void make_war_decs(sys::state& state) {
	auto targets = ve::vectorizable_buffer<dcon::nation_id, dcon::nation_id>(state.world.nation_size());
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id n{ dcon::nation_id::value_base_t(i) };
		if(state.world.nation_get_owned_province_count(n) == 0)
			return;
		if(state.world.nation_get_is_at_war(n))
			return;
		if(state.world.nation_get_is_player_controlled(n))
			return;
		if(state.world.nation_get_military_score(n) == 0)
			return;
		// Subjects don't declare wars without a define set
		if(auto ol = state.world.nation_get_overlord_as_subject(n); state.world.overlord_get_ruler(ol) && state.defines.alice_allow_subjects_declare_wars < 1)
			return;
		// AI shouldn't declare wars if it has high war exhaustion that would make it want to peace out immediately
		if(state.world.nation_get_war_exhaustion(n) > state.defines.alice_ai_war_exhaustion_readiness_limit)
			return;
		auto base_strength = estimate_strength(state, n);
		float best_difference = 2.0f;
		//Great powers should look for non-neighbor nations to use their existing wargoals on; helpful for forcing unification/repay debts wars to happen
		if(nations::is_great_power(state, n)) {
			for(auto target : state.world.in_nation) {
				auto real_target = target.get_overlord_as_subject().get_ruler() ? target.get_overlord_as_subject().get_ruler() : target;
				if(target == n || real_target == n)
					continue;
				if(state.world.nation_get_owned_province_count(real_target) == 0)
					continue;
				if(nations::are_allied(state, n, real_target))
					continue;
				if(target.get_in_sphere_of() == n)
					continue;
				if(military::has_truce_with(state, n, real_target))
					continue;
				if(!military::can_use_cb_against(state, n, target))
					continue;
				//If it neighbors one of our spheres and we can pathfind to each other's capitals, we don't need naval supremacy to reach this nation
				//Generally here to help Prussia realize it doesn't need a navy to attack Denmark
				for(auto adj : state.world.nation_get_nation_adjacency(target)) {
					auto other = adj.get_connected_nations(0) != n ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
					auto neighbor = other;
					if(neighbor.get_in_sphere_of() == n) {
						auto path = province::make_safe_land_path(state, state.world.nation_get_capital(n), state.world.nation_get_capital(neighbor), n);
						if(path.empty()) {
							continue;
						}
						auto str_difference = base_strength + estimate_additional_offensive_strength(state, n, real_target) - estimate_defensive_strength(state, real_target);
						if(str_difference > best_difference) {
							best_difference = str_difference;
							targets.set(n, target.id);
							break;
						}
					}
				}
				if(!state.world.get_nation_adjacency_by_nation_adjacency_pair(n, target) && !does_have_naval_supremacy(state, n, target))
					continue;
				auto str_difference = base_strength + estimate_additional_offensive_strength(state, n, real_target) - estimate_defensive_strength(state, real_target);
				if(str_difference > best_difference) {
					best_difference = str_difference;
					targets.set(n, target.id);
				}
			}
		}
		for(auto adj : state.world.nation_get_nation_adjacency(n)) {
			auto other = adj.get_connected_nations(0) != n ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
			auto real_target = other.get_overlord_as_subject().get_ruler() ? other.get_overlord_as_subject().get_ruler() : other;
			if(real_target == n)
				continue;
			if(nations::are_allied(state, n, real_target) || nations::are_allied(state, n, other))
				continue;
			if(real_target.get_in_sphere_of() == n)
				continue;
			if(state.world.nation_get_in_sphere_of(other) == n)
				continue;
			if(military::has_truce_with(state, n, other) || military::has_truce_with(state, n, real_target))
				continue;
			if(!military::can_use_cb_against(state, n, other))
				continue;
			if(!state.world.get_nation_adjacency_by_nation_adjacency_pair(n, other) && !does_have_naval_supremacy(state, n, other))
				continue;
			auto str_difference = base_strength + estimate_additional_offensive_strength(state, n, real_target) - estimate_defensive_strength(state, real_target);
			if(str_difference > best_difference) {
				best_difference = str_difference;
				targets.set(n, other.id);
			}
		}
		if(state.world.nation_get_central_ports(n) > 0) {
			// try some random coastal nations
			for(uint32_t j = 0; j < 6; ++j) {
				auto rvalue = rng::get_random(state, uint32_t((n.index() << 3) + j));
				auto reduced_value = rng::reduce(uint32_t(rvalue), state.world.nation_size());
				dcon::nation_id other{ dcon::nation_id::value_base_t(reduced_value) };
				auto real_target = fatten(state.world, other).get_overlord_as_subject().get_ruler() ? fatten(state.world, other).get_overlord_as_subject().get_ruler() : fatten(state.world, other);
				if(other == n || real_target == n)
					continue;
				if(state.world.nation_get_owned_province_count(other) == 0 || state.world.nation_get_owned_province_count(real_target) == 0)
					continue;
				if(state.world.nation_get_central_ports(other) == 0 || state.world.nation_get_central_ports(real_target) == 0)
					continue;
				if(nations::are_allied(state, n, real_target) || nations::are_allied(state, n, other))
					continue;
				if(real_target.get_in_sphere_of() == n)
					continue;
				if(state.world.nation_get_in_sphere_of(other) == n)
					continue;
				if(military::has_truce_with(state, n, other) || military::has_truce_with(state, n, real_target))
					continue;
				if(!military::can_use_cb_against(state, n, other))
					continue;
				if(!state.world.get_nation_adjacency_by_nation_adjacency_pair(n, other) && !does_have_naval_supremacy(state, n, other))
					continue;
				auto str_difference = base_strength + estimate_additional_offensive_strength(state, n, real_target) - estimate_defensive_strength(state, real_target);
				if(str_difference > best_difference) {
					best_difference = str_difference;
					targets.set(n, other);
				}
			}
		}
	});
	for(auto n : state.world.in_nation) {
		if(n.get_is_at_war() == false && targets.get(n)) {
			static std::vector<possible_cb> potential;
			sort_available_declaration_cbs(potential, state, n, targets.get(n));
			if(!potential.empty()) {
				assert(command::can_declare_war(state, n, targets.get(n), potential[0].cb, potential[0].state_def, potential[0].associated_tag, potential[0].secondary_nation));
				command::execute_declare_war(state, n, targets.get(n), potential[0].cb, potential[0].state_def, potential[0].associated_tag, potential[0].secondary_nation, true, false);
			}
		}
	}
}

}
