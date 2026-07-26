#include "diplomatic_crisis_dynamics.hpp"

#include "gamerule.hpp"
#include "system_state.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace nations::diplomatic_crisis_dynamics {
namespace {

float unit(float value) {
	return std::isfinite(value) ? std::clamp(value, 0.f, 1.f) : 0.f;
}

float nonnegative(float value) {
	return std::isfinite(value) ? std::max(value, 0.f) : 0.f;
}

float ratio(uint64_t numerator, uint64_t denominator) {
	if(denominator == 0)
		return 0.f;
	return unit(float(double(numerator) / double(denominator)));
}

float power_score(sys::state const& state, dcon::nation_id nation) {
	if(!nation || !state.world.nation_is_valid(nation))
		return 0.f;
	return nonnegative(float(state.world.nation_get_military_score(nation)))
		+ nonnegative(float(state.world.nation_get_industrial_score(nation)));
}

float normalized_exhaustion(sys::state const& state, dcon::nation_id nation) {
	if(!nation || !state.world.nation_is_valid(nation))
		return 0.f;
	return unit(state.world.nation_get_war_exhaustion(nation) / 100.f);
}

uint64_t active_wargoals(std::vector<sys::full_wg> const& wargoals) {
	uint64_t count = 0;
	for(auto const& wargoal : wargoals) {
		if(!wargoal.cb)
			break;
		++count;
	}
	return count;
}

} // namespace

std::string_view stage_name(escalation_stage stage) {
	switch(stage) {
	case escalation_stage::inactive: return "inactive";
	case escalation_stage::diplomatic_opening: return "diplomatic_opening";
	case escalation_stage::bargaining: return "bargaining";
	case escalation_stage::confrontation: return "confrontation";
	case escalation_stage::brinkmanship: return "brinkmanship";
	case escalation_stage::war_imminent: return "war_imminent";
	}
	return "inactive";
}

escalation_stage stage_for(bool active, float temperature, bool missing_committed_side) {
	if(!active)
		return escalation_stage::inactive;
	// A crisis still looking for a backer cannot become a war, regardless of a
	// malformed or externally modified temperature value.
	if(missing_committed_side)
		return escalation_stage::diplomatic_opening;
	auto const normalized_temperature = unit(temperature / 100.f);
	if(normalized_temperature < 0.20f)
		return escalation_stage::diplomatic_opening;
	if(normalized_temperature < 0.50f)
		return escalation_stage::bargaining;
	if(normalized_temperature < 0.75f)
		return escalation_stage::confrontation;
	if(normalized_temperature < 0.90f)
		return escalation_stage::brinkmanship;
	return escalation_stage::war_imminent;
}

breakdown calculate(inputs raw_inputs) {
	breakdown result;
	if(!raw_inputs.enabled)
		return result;

	result.enabled = true;
	result.factors = raw_inputs;
	result.factors.temperature = std::isfinite(raw_inputs.temperature)
		? std::clamp(raw_inputs.temperature, 0.f, 100.f)
		: 0.f;
	result.factors.attacker_power = nonnegative(raw_inputs.attacker_power);
	result.factors.defender_power = nonnegative(raw_inputs.defender_power);
	result.factors.attacker_war_exhaustion = unit(raw_inputs.attacker_war_exhaustion);
	result.factors.defender_war_exhaustion = unit(raw_inputs.defender_war_exhaustion);

	if(!result.factors.active)
		return result;

	auto const committed_total = raw_inputs.committed_attackers
		> std::numeric_limits<uint64_t>::max() - raw_inputs.committed_defenders
		? std::numeric_limits<uint64_t>::max()
		: raw_inputs.committed_attackers + raw_inputs.committed_defenders;
	result.factors.total_participants = std::max(raw_inputs.total_participants, committed_total);
	result.missing_committed_side = raw_inputs.committed_attackers == 0
		|| raw_inputs.committed_defenders == 0;
	result.normalized_temperature = unit(result.factors.temperature / 100.f);
	result.commitment_ratio = ratio(committed_total, result.factors.total_participants);
	if(committed_total != 0) {
		auto const smaller_side = std::min(
			raw_inputs.committed_attackers, raw_inputs.committed_defenders);
		result.bilateral_commitment = unit(float(
			2.0 * double(smaller_side) / double(committed_total)));
	}

	auto const attacker_power = double(result.factors.attacker_power);
	auto const defender_power = double(result.factors.defender_power);
	auto const total_power = attacker_power + defender_power;
	if(total_power > 0.0 && std::isfinite(total_power)) {
		result.attacker_power_share = unit(float(attacker_power / total_power));
		result.mean_war_exhaustion = unit(float(
			(attacker_power * double(result.factors.attacker_war_exhaustion)
				+ defender_power * double(result.factors.defender_war_exhaustion))
			/ total_power));
	} else {
		result.mean_war_exhaustion = unit(
			0.5f * (result.factors.attacker_war_exhaustion
				+ result.factors.defender_war_exhaustion));
	}
	result.power_asymmetry = unit(std::abs(2.f * result.attacker_power_share - 1.f));

	auto const total_wargoals = raw_inputs.attacker_wargoals
		> std::numeric_limits<uint64_t>::max() - raw_inputs.defender_wargoals
		? std::numeric_limits<uint64_t>::max()
		: raw_inputs.attacker_wargoals + raw_inputs.defender_wargoals;
	result.claim_pressure = unit(float(
		double(std::min<uint64_t>(total_wargoals, 6u)) / 6.0));
	auto const readiness = 1.f - result.mean_war_exhaustion;
	result.escalation_pressure = unit(
		0.50f * result.normalized_temperature
		+ 0.20f * result.commitment_ratio
		+ 0.10f * result.bilateral_commitment
		+ 0.10f * readiness
		+ 0.10f * result.claim_pressure);
	result.settlement_pressure = unit(
		0.45f * result.normalized_temperature
		+ 0.25f * result.mean_war_exhaustion
		+ 0.20f * result.power_asymmetry
		+ 0.10f * (1.f - result.bilateral_commitment));

	if(!result.missing_committed_side) {
		result.war_risk = result.normalized_temperature >= 1.f
			? 1.f
			: unit(result.normalized_temperature
					* (0.70f + 0.15f * result.commitment_ratio
						+ 0.10f * result.bilateral_commitment
						+ 0.05f * readiness)
				+ 0.05f * result.claim_pressure * result.commitment_ratio);
	}
	result.stage = stage_for(
		result.factors.active, result.factors.temperature, result.missing_committed_side);
	return result;
}

breakdown evaluate_current_crisis(sys::state const& state) {
	inputs derived;
	derived.enabled = gamerule::age_of_transformation_enabled(state);
	if(!derived.enabled)
		return calculate(derived);
	derived.active = state.current_crisis_state != sys::crisis_state::inactive;
	derived.temperature = state.crisis_temperature;
	derived.attacker_wargoals = active_wargoals(state.crisis_attacker_wargoals);
	derived.defender_wargoals = active_wargoals(state.crisis_defender_wargoals);

	float attacker_exhaustion_sum = 0.f;
	float defender_exhaustion_sum = 0.f;
	for(auto const& participant : state.crisis_participants) {
		if(!participant.id)
			break;
		if(!state.world.nation_is_valid(participant.id))
			continue;
		++derived.total_participants;
		if(participant.merely_interested)
			continue;
		if(participant.supports_attacker) {
			++derived.committed_attackers;
			derived.attacker_power += power_score(state, participant.id);
			attacker_exhaustion_sum += normalized_exhaustion(state, participant.id);
		} else {
			++derived.committed_defenders;
			derived.defender_power += power_score(state, participant.id);
			defender_exhaustion_sum += normalized_exhaustion(state, participant.id);
		}
	}

	// Participants are populated only after both backers have been found. These
	// fallbacks keep the diagnostic meaningful during the two formation states.
	if(derived.committed_attackers == 0 && state.primary_crisis_attacker
		&& state.world.nation_is_valid(state.primary_crisis_attacker)) {
		derived.committed_attackers = 1;
		derived.attacker_power = power_score(state, state.primary_crisis_attacker);
		attacker_exhaustion_sum = normalized_exhaustion(state, state.primary_crisis_attacker);
	}
	if(derived.committed_defenders == 0 && state.primary_crisis_defender
		&& state.world.nation_is_valid(state.primary_crisis_defender)) {
		derived.committed_defenders = 1;
		derived.defender_power = power_score(state, state.primary_crisis_defender);
		defender_exhaustion_sum = normalized_exhaustion(state, state.primary_crisis_defender);
	}
	derived.total_participants = std::max(
		derived.total_participants,
		derived.committed_attackers + derived.committed_defenders);
	derived.attacker_war_exhaustion = derived.committed_attackers == 0
		? 0.f
		: attacker_exhaustion_sum / float(derived.committed_attackers);
	derived.defender_war_exhaustion = derived.committed_defenders == 0
		? 0.f
		: defender_exhaustion_sum / float(derived.committed_defenders);
	return calculate(derived);
}

} // namespace nations::diplomatic_crisis_dynamics
