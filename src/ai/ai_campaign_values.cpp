#include "ai_types.hpp"
#include "ai_campaign_values.hpp"

namespace ai {

int32_t future_rebels_in_nation(sys::state& state, dcon::nation_id n) {
	auto total = 0;
	for(auto fac : state.world.nation_get_rebellion_within(n)) {
		for(auto ar : state.world.rebel_faction_get_army_rebel_control(fac.get_rebels())) {
			auto regs = ar.get_army().get_army_membership();
			total += int32_t(regs.end() - regs.begin());
		}
	}

	return total;
}

int16_t calculate_desired_army_size(sys::state& state, dcon::nation_id nation) {
	auto fid = dcon::fatten(state.world, nation);

	auto factor = 1.0f;

	if(state.world.nation_get_ai_is_threatened(nation)) {
		factor *= 1.25f;
	}

	if(fid.get_is_at_war()) {
		factor *= 1.5f;
	}

	if(state.world.nation_get_ai_strategy(nation) == ai_strategies::militant) {
		factor *= 1.25f;
	} else {
		factor *= 0.75f;
	}

	if(future_rebels_in_nation(state, nation) == 0) {
		factor *= 0.9f;
	} else {
		factor *= 1.1f;
	}

	auto in_sphere_of = state.world.nation_get_in_sphere_of(nation);

	// Most dangerous neighbor
	dcon::nation_id greatest_neighbor;
	auto greatest_neighbor_strength = 0.0f;

	auto own_str = estimate_strength(state, nation);
	for(auto b : state.world.nation_get_nation_adjacency_as_connected_nations(nation)) {
		auto other = b.get_connected_nations(0) != nation ? b.get_connected_nations(0) : b.get_connected_nations(1);
		if(!nations::are_allied(state, nation, other) && (!in_sphere_of || in_sphere_of != other.get_in_sphere_of())) {

			auto other_str = estimate_strength(state, other);
			if(other_str > greatest_neighbor_strength && other_str < own_str * 10.0f) {
				greatest_neighbor_strength = other_str;
				greatest_neighbor = other;
			}
		}
	}

	// How many regiments it has
	int16_t total = 0;
	for(auto p : state.world.nation_get_army_control(greatest_neighbor)) {
		auto frange = p.get_army().get_army_membership();
		total += int16_t(frange.end() - frange.begin());
	}

	// Debug lines
#ifndef NDEBUG
	auto fid2 = dcon::fatten(state.world, nation);
	auto identity = fid.get_identity_from_identity_holder();
	auto tagname = text::produce_simple_string(state, identity.get_name());
	fid2 = dcon::fatten(state.world, greatest_neighbor);
	identity = fid2.get_identity_from_identity_holder();
	auto greatest_neighbour_tagname = text::produce_simple_string(state, identity.get_name());
#endif

	// Use ceil: we want a bit stronger army than the enemy, at least one regiment.
	return int16_t(std::clamp(ceil(total * double(factor)), ceil(0.1 * fid.get_recruitable_regiments()), 1.0 * fid.get_recruitable_regiments()));
}

// strength is an abstract value measured in arbitrary strength units
// would be nice to measure it in firepower instead
float estimate_strength(sys::state& state, dcon::nation_id n) {
	if(state.cheat_data.disable_ai) {
		return 1.f;
	}

	float value = state.world.nation_get_military_score(n) * state.defines.alice_ai_strength_estimation_military_industrial_balance;
	value += state.world.nation_get_industrial_score(n) * (1.f - state.defines.alice_ai_strength_estimation_military_industrial_balance);
	for(auto subj : state.world.nation_get_overlord_as_ruler(n)) {
		value += subj.get_subject().get_military_score() * state.defines.alice_ai_strength_estimation_military_industrial_balance;
		value += subj.get_subject().get_industrial_score() * (1.f - state.defines.alice_ai_strength_estimation_military_industrial_balance);
	}
	return value;
}

float estimate_defensive_strength(sys::state& state, dcon::nation_id n) {
	if(state.cheat_data.disable_ai) {
		return 1.0f;
	}

	float value = estimate_strength(state, n);
	for(auto dr : state.world.nation_get_diplomatic_relation(n)) {
		if(!dr.get_are_allied())
			continue;

		auto other = dr.get_related_nations(0) != n ? dr.get_related_nations(0) : dr.get_related_nations(1);
		if(other.get_overlord_as_subject().get_ruler() != n)
			value += estimate_strength(state, other);
	}
	if(auto sl = state.world.nation_get_in_sphere_of(n); sl)
		value += estimate_strength(state, sl);
	return value;
}

float estimate_additional_offensive_strength(sys::state& state, dcon::nation_id n, dcon::nation_id target) {
	float value = 0.f;
	for(auto dr : state.world.nation_get_diplomatic_relation(n)) {
		if(!dr.get_are_allied())
			continue;

		auto other = dr.get_related_nations(0) != n ? dr.get_related_nations(0) : dr.get_related_nations(1);
		if(other.get_overlord_as_subject().get_ruler() != n && military::can_use_cb_against(state, other, target) && !military::has_truce_with(state, other, target))
			value += estimate_strength(state, other);
	}
	return value * state.defines.alice_ai_offensive_strength_overestimate;
}

float estimate_naval_strength(sys::state& state, dcon::nation_id n) {
	return (float)state.world.nation_get_used_naval_supply_points(n);
}

bool does_have_naval_supremacy(sys::state& state, dcon::nation_id n, dcon::nation_id target) {
	auto self_sup = state.world.nation_get_used_naval_supply_points(n);

	auto real_target = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target));
	if(!real_target)
		real_target = target;

	if(self_sup <= state.world.nation_get_used_naval_supply_points(real_target))
		return false;

	if(self_sup <= state.world.nation_get_in_sphere_of(real_target).get_used_naval_supply_points())
		return false;

	for(auto a : state.world.nation_get_diplomatic_relation(real_target)) {
		if(!a.get_are_allied())
			continue;
		auto other = a.get_related_nations(0) != real_target ? a.get_related_nations(0) : a.get_related_nations(1);
		if(self_sup <= other.get_used_naval_supply_points())
			return false;
	}

	return true;
}

bool ai_will_accept_free_trade(sys::state& state, dcon::nation_id target, dcon::nation_id from) {
	return false;
}

void explain_ai_trade_agreement_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent) {
	text::add_line_with_condition(state, contents, "never", false, indent + 15);
}

bool ai_will_grant_access(sys::state& state, dcon::nation_id target, dcon::nation_id from) {
	if(!state.world.nation_get_is_at_war(from))
		return false;
	if(state.world.nation_get_ai_rival(target) == from)
		return false;
	if(military::are_at_war(state, from, state.world.nation_get_ai_rival(target)))
		return true;

	for(auto wa : state.world.nation_get_war_participant(target)) {
		auto is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_is_attacker() != is_attacker) {
				if(military::are_at_war(state, o.get_nation(), from))
					return true;
			}
		}
	}
	return false;

}
void explain_ai_access_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent) {
	text::add_line_with_condition(state, contents, "ai_access_1", ai_will_grant_access(state, target, state.local_player_nation), indent);
}

}
