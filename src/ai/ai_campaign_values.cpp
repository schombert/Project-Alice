#include "ai_campaign_values.hpp"

namespace ai {

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
