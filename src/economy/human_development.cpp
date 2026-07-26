#include "human_development.hpp"

#include "advanced_province_buildings.hpp"
#include "demographics.hpp"
#include "demographics_templates.hpp"
#include "economy_stats.hpp"
#include "gamerule.hpp"
#include "system_state.hpp"

#include <algorithm>
#include <cmath>

namespace economy::human_development {
namespace {

float unit(float value, float fallback = 0.f) {
	return std::isfinite(value) ? std::clamp(value, 0.f, 1.f) : fallback;
}

float nonnegative(float value) {
	return std::isfinite(value) ? std::max(0.f, value) : 0.f;
}

float province_literacy(sys::state const& state, dcon::province_id province) {
	auto const population = state.world.province_get_demographics(province, demographics::total);
	if(!std::isfinite(population) || population <= 0.f)
		return 0.f;
	auto const literacy = state.world.province_get_demographics(province, demographics::literacy);
	return unit(literacy / population);
}

inputs inputs_for_province(sys::state const& state, dcon::province_id province,
		float literacy) {
	inputs result{};
	auto const population = nonnegative(
		state.world.province_get_demographics(province, demographics::total));
	auto const urban_capacity = nonnegative(
		state.world.province_get_advanced_province_building_private_size(
			province, advanced_province_buildings::list::local_cities_and_towns));
	result.urbanization = population > 0.f ? unit(urban_capacity / population) : 0.f;
	// The labour market already clears each skill lane and stores the fraction
	// actually bought.  A city cannot be an attractive destination merely
	// because it has flats: residents also need access to paid work.  This is
	// deliberately local until intra-province districts and transport links
	// exist, at which point this field becomes the natural route-access input.
	float sold_total = 0.f;
	for(int32_t lane = 0; lane < economy::labor::total; ++lane) {
		sold_total += unit(state.world.province_get_labor_supply_sold(province, lane));
	}
	result.job_access = economy::labor::total > 0
		? unit(sold_total / float(economy::labor::total)) : 1.f;
	auto const housing_demand = nonnegative(
		state.world.province_get_service_demand_forbidden_public_supply(
			province, services::list::urban_housing));
	auto const housing_supply = nonnegative(
		state.world.province_get_service_supply_private(
			province, services::list::urban_housing));
	// The service matcher represents an empty paid market with satisfaction 0.
	// That is useful for refunds, but it must not mean that a province with no
	// active housing market has literally zero housing. Only measured demand can
	// turn market satisfaction into demographic pressure.
	result.housing_access = housing_demand > 0.001f && housing_supply > 0.001f
		? unit(state.world.province_get_service_satisfaction(
			province, services::list::urban_housing), 1.f)
		: 1.f;
	auto const paid_education = unit(
		state.world.province_get_service_satisfaction(province, services::list::education));
	auto const public_education = unit(
		state.world.province_get_service_satisfaction_for_free(
			province, services::list::education));
	// Private and public access are alternative ways to obtain education. This
	// union avoids double-counting while preserving either complete channel.
	result.education_access = 1.f - (1.f - paid_education) * (1.f - public_education);
	result.literacy = unit(literacy);
	return result;
}

} // namespace

config ruleset_config_for(sys::state const& state) {
	config result{};
	result.enabled = gamerule::age_of_transformation_enabled(state);
	return result;
}

breakdown calculate(config const& rules, inputs raw_inputs) {
	breakdown result{};
	result.factors.housing_access = unit(raw_inputs.housing_access, 1.f);
	result.factors.urbanization = unit(raw_inputs.urbanization);
	result.factors.job_access = unit(raw_inputs.job_access, 1.f);
	result.factors.education_access = unit(raw_inputs.education_access);
	result.factors.literacy = unit(raw_inputs.literacy);
	if(!rules.enabled)
		return result;

	result.enabled = true;
	result.housing_shortage = 1.f - result.factors.housing_access;
	result.overcrowding = unit(
		result.housing_shortage * result.factors.urbanization);
	result.employment_exclusion = 1.f - result.factors.job_access;
	result.demographic_transition = unit(
		result.factors.urbanization
		* (0.60f * result.factors.literacy
			+ 0.40f * result.factors.education_access));
	result.human_development_index = unit(
		0.40f * result.factors.housing_access
		+ 0.35f * result.factors.literacy
		+ 0.25f * result.factors.education_access);
	result.monthly_overcrowding_growth_penalty =
		nonnegative(rules.maximum_overcrowding_growth_penalty) * result.overcrowding;
	result.monthly_transition_growth_reduction =
		nonnegative(rules.maximum_transition_growth_reduction)
			* result.demographic_transition;
	result.monthly_growth_adjustment = -(
		result.monthly_overcrowding_growth_penalty
		+ result.monthly_transition_growth_reduction);
	result.monthly_militancy_adjustment =
		nonnegative(rules.maximum_overcrowding_militancy) * result.overcrowding;
	// Rural provinces without an urban housing market are neutral. Housing only
	// becomes a migration constraint where city capacity and demand overlap.
	// Housing and work are complementary: a cheap city with no jobs must not
	// pull migrants just because its rent is low.
	result.migration_quality = (1.f - 0.65f * result.overcrowding)
		* (0.35f + 0.65f * result.factors.job_access);
	return result;
}

breakdown evaluate_province(sys::state const& state, dcon::province_id province,
		float literacy) {
	auto rules = ruleset_config_for(state);
	// This early return is part of the compatibility contract: classic games
	// must not even read service arrays that older/minimal scenarios may not
	// have initialized.
	if(!rules.enabled)
		return calculate(rules, {});
	if(!province || !state.world.province_is_valid(province)) {
		rules.enabled = false;
		return calculate(rules, {});
	}
	return calculate(rules, inputs_for_province(state, province, literacy));
}

breakdown evaluate_pop(sys::state const& state, dcon::pop_id pop) {
	if(!pop || !state.world.pop_is_valid(pop)) {
		auto rules = ruleset_config_for(state);
		rules.enabled = false;
		return calculate(rules, {});
	}
	auto const province = state.world.pop_get_province_from_pop_location(pop);
	return evaluate_province(state, province, pop_demographics::get_literacy(state, pop));
}

float migration_multiplier(config const& rules, breakdown const& origin,
		breakdown const& target) {
	if(!rules.enabled || !origin.enabled || !target.enabled)
		return 1.f;
	auto const safe_origin = std::max(0.05f, nonnegative(origin.migration_quality));
	auto const safe_target = std::max(0.05f, nonnegative(target.migration_quality));
	auto const ratio = safe_target / safe_origin;
	auto const raw = std::isfinite(ratio) && ratio >= 0.f ? std::sqrt(ratio) : 1.f;
	auto const minimum = std::isfinite(rules.minimum_migration_multiplier)
		? std::max(0.f, rules.minimum_migration_multiplier) : 0.70f;
	auto const requested_maximum = std::isfinite(rules.maximum_migration_multiplier)
		? rules.maximum_migration_multiplier : 1.35f;
	auto const maximum = std::max(minimum, requested_maximum);
	return std::clamp(raw, minimum, maximum);
}

float migration_multiplier(sys::state const& state, dcon::province_id origin,
		dcon::province_id target) {
	auto const rules = ruleset_config_for(state);
	if(!rules.enabled || !origin || !target
		|| !state.world.province_is_valid(origin)
		|| !state.world.province_is_valid(target))
		return 1.f;
	return migration_multiplier(rules,
		evaluate_province(state, origin, province_literacy(state, origin)),
		evaluate_province(state, target, province_literacy(state, target)));
}

demographic_account calculate_account(float population, float legacy_growth_modifier,
		float starvation_penalty, breakdown const& development) {
	demographic_account result{};
	auto const safe_population = nonnegative(population);
	auto const safe_legacy = std::isfinite(legacy_growth_modifier)
		? legacy_growth_modifier : 0.f;
	auto const safe_starvation = std::isfinite(starvation_penalty)
		? std::min(0.f, starvation_penalty) : 0.f;
	result.baseline_natural_growth = safe_population * safe_legacy;
	result.starvation_loss = safe_population * -safe_starvation;
	if(development.enabled) {
		result.housing_loss = safe_population
			* nonnegative(development.monthly_overcrowding_growth_penalty);
		result.transition_reduction = safe_population
			* nonnegative(development.monthly_transition_growth_reduction);
	}
	result.net_natural_change = result.baseline_natural_growth
		- result.starvation_loss - result.housing_loss - result.transition_reduction;
	return result;
}

} // namespace economy::human_development
