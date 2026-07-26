#include "policy_execution.hpp"

#include "economy_stats.hpp"
#include "demographics.hpp"
#include "gamerule.hpp"
#include "system_state.hpp"
#include "transformation_politics.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

namespace nations::policy_execution {
namespace {

float unit(float value) {
	return std::isfinite(value) ? std::clamp(value, 0.f, 1.f) : 0.f;
}

float budget_fraction(sys::state const& state, dcon::nation_id nation, policy_kind policy) {
	auto slider = 0.f;
	switch(policy) {
	case policy_kind::education:
		slider = float(state.world.nation_get_education_spending(nation)) / 100.f;
		break;
	case policy_kind::social_benefits:
		slider = float(state.world.nation_get_social_spending(nation)) / 100.f;
		break;
	case policy_kind::mobilization_logistics:
		return unit(state.world.nation_get_effective_land_spending(nation));
	case policy_kind::crime_suppression:
	case policy_kind::reform_implementation:
		slider = float(state.world.nation_get_administrative_spending(nation)) / 100.f;
		break;
	}
	return unit(slider * unit(state.world.nation_get_spending_level(nation)));
}

} // namespace

weights weights_for(policy_kind policy) {
	switch(policy) {
	case policy_kind::crime_suppression: return {0.25f, 0.35f, 0.15f, 0.15f, 0.10f};
	case policy_kind::education: return {0.20f, 0.20f, 0.25f, 0.20f, 0.15f};
	case policy_kind::social_benefits: return {0.20f, 0.25f, 0.30f, 0.15f, 0.10f};
	case policy_kind::reform_implementation: return {0.20f, 0.20f, 0.15f, 0.15f, 0.30f};
	case policy_kind::mobilization_logistics: return {0.20f, 0.30f, 0.25f, 0.15f, 0.10f};
	}
	return {};
}

breakdown calculate(policy_kind policy, inputs raw_inputs) {
	breakdown result;
	result.policy = policy;
	if(!raw_inputs.enabled)
		return result;

	result.enabled = true;
	result.factors = raw_inputs;
	result.factors.national_administration = unit(raw_inputs.national_administration);
	result.factors.local_control = unit(raw_inputs.local_control);
	result.factors.funding = unit(raw_inputs.funding);
	result.factors.bureaucratic_labor = unit(raw_inputs.bureaucratic_labor);
	result.factors.political_compliance = unit(raw_inputs.political_compliance);
	result.factor_weights = weights_for(policy);

	using item = std::pair<capacity_factor, float>;
	std::array<item, 5> factors{{
		{capacity_factor::national_administration, result.factors.national_administration},
		{capacity_factor::local_control, result.factors.local_control},
		{capacity_factor::funding, result.factors.funding},
		{capacity_factor::bureaucratic_labor, result.factors.bureaucratic_labor},
		{capacity_factor::political_compliance, result.factors.political_compliance},
	}};
	result.bottleneck = factors.front().first;
	result.bottleneck_value = factors.front().second;
	for(auto const& factor : factors) {
		if(factor.second < result.bottleneck_value) {
			result.bottleneck = factor.first;
			result.bottleneck_value = factor.second;
		}
	}

	result.weighted_capacity = unit(
		result.factors.national_administration * result.factor_weights.national_administration
		+ result.factors.local_control * result.factor_weights.local_control
		+ result.factors.funding * result.factor_weights.funding
		+ result.factors.bureaucratic_labor * result.factor_weights.bureaucratic_labor
		+ result.factors.political_compliance * result.factor_weights.political_compliance);
	// The weighted mean represents substitutable capacity; the bounded
	// bottleneck term prevents money alone from replacing territorial control.
	result.effective_execution = unit(
		result.weighted_capacity * (0.65f + 0.35f * result.bottleneck_value));
	return result;
}

breakdown effective_policy(sys::state const& state, dcon::nation_id nation,
	dcon::province_id province, policy_kind policy) {
	inputs derived;
	if(!gamerule::age_of_transformation_enabled(state))
		return calculate(policy, derived);
	derived.enabled = true;
	if(!nation || !province || !state.world.nation_is_valid(nation)
		|| !state.world.province_is_valid(province)) {
		derived.national_administration = 0.f;
		derived.local_control = 0.f;
		derived.funding = 0.f;
		derived.bureaucratic_labor = 0.f;
		derived.political_compliance = 0.f;
		return calculate(policy, derived);
	}

	derived.national_administration = state.world.nation_get_administrative_efficiency(nation);
	derived.local_control = state.world.province_get_control_ratio(province);
	derived.funding = budget_fraction(state, nation, policy);
	derived.bureaucratic_labor = std::max(
		state.world.province_get_labor_demand_satisfaction(
			province, economy::labor::high_education_and_accepted),
		state.world.province_get_labor_demand_satisfaction(
			province, economy::labor::high_education));
	derived.political_compliance = 0.25f;
	auto const index = nation.index();
	if(state.transformation_politics_cache_valid
		&& index < state.transformation_politics_cache.size()
		&& state.transformation_politics_cache[index].enabled) {
		auto const* political_result = &state.transformation_politics_cache[index];
		derived.political_compliance = 0.25f + 0.75f * unit(political_result->legitimacy.total / 100.f);
	}
	return calculate(policy, derived);
}

float average_effective_policy(sys::state const& state, dcon::nation_id nation,
	policy_kind policy) {
	if(!gamerule::age_of_transformation_enabled(state))
		return 1.f;
	if(!nation || !state.world.nation_is_valid(nation))
		return 0.f;

	double weighted_execution = 0.0;
	double total_weight = 0.0;
	for(auto ownership : state.world.nation_get_province_ownership(nation)) {
		auto const province = ownership.get_province().id;
		auto const population = std::max(
			0.f, state.world.province_get_demographics(province, demographics::total));
		// Empty strategic provinces still consume a small administrative share.
		auto const weight = std::max(1.f, population);
		weighted_execution += double(effective_policy(state, nation, province, policy).effective_execution)
			* double(weight);
		total_weight += double(weight);
	}
	if(total_weight <= 0.0)
		return 0.f;
	return unit(float(weighted_execution / total_weight));
}

} // namespace nations::policy_execution
