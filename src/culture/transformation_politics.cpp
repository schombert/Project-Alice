#include "transformation_politics.hpp"

#include "culture.hpp"
#include "demographics.hpp"
#include "policy_execution.hpp"
#include "system_state.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <limits>

namespace politics::transformation {
namespace {

constexpr float comparison_epsilon = 0.000001f;

float finite_or(float value, float fallback) {
	return std::isfinite(value) ? value : fallback;
}

float nonnegative(float value, float fallback = 0.0f) {
	return std::max(0.0f, finite_or(value, fallback));
}

float unit_interval(float value) {
	return std::clamp(finite_or(value, 0.0f), 0.0f, 1.0f);
}

float bounded(float value, float fallback, float minimum, float maximum) {
	return std::clamp(finite_or(value, fallback), minimum, maximum);
}

float finite_nonnegative_float(double value) {
	if(!std::isfinite(value) || value <= 0.0)
		return 0.0f;
	return float(std::min(value, double(std::numeric_limits<float>::max())));
}

uint8_t maximum_coalition_groups(ruleset_config const& config) {
	return std::clamp<uint8_t>(config.maximum_coalition_groups, 1, uint8_t(interest_group_count));
}

float coalition_target(ruleset_config const& config) {
	return bounded(config.coalition_power_target, 0.50f, 0.05f, 0.95f);
}

float wealth_signal(float savings_per_capita, ruleset_config const& config) {
	auto const wealth = nonnegative(savings_per_capita);
	auto const reference = bounded(config.wealth_reference, 1.0f, 0.000001f, 1000000000.0f);
	auto const ratio = std::min(wealth / reference, 80.0f);
	return unit_interval(1.0f - std::exp(-ratio));
}

float power_per_capita(population_sample const& sample, ruleset_config const& config, float wealth) {
	auto const maximum = bounded(config.maximum_power_per_capita, 4.0f, 0.05f, 10.0f);
	auto result =
		bounded(config.base_power, 0.25f, 0.0f, 10.0f)
		+ bounded(config.wealth_power_weight, 1.25f, 0.0f, 10.0f) * wealth
		+ bounded(config.income_power_weight, 0.35f, 0.0f, 10.0f) * unit_interval(sample.income_security)
		+ bounded(config.property_power_weight, 1.50f, 0.0f, 10.0f) * unit_interval(sample.property_ownership)
		+ bounded(config.literacy_power_weight, 0.35f, 0.0f, 10.0f) * unit_interval(sample.literacy)
		+ bounded(config.consciousness_power_weight, 0.30f, 0.0f, 10.0f) * unit_interval(sample.consciousness);
	return std::clamp(finite_or(result, 0.0f), 0.0f, maximum);
}

struct group_accumulator {
	double population_support = 0.0;
	double political_power = 0.0;
	double wealth = 0.0;
	double income = 0.0;
	double property = 0.0;
	double literacy = 0.0;
	double consciousness = 0.0;
};

constexpr std::array<std::array<float, interest_group_count>, interest_group_count> group_compatibility = {{
	{{1.00f, 0.65f, 0.30f, 0.10f, 0.80f, 0.75f}},
	{{0.65f, 1.00f, 0.70f, 0.25f, 0.45f, 0.75f}},
	{{0.30f, 0.70f, 1.00f, 0.75f, 0.50f, 0.70f}},
	{{0.10f, 0.25f, 0.75f, 1.00f, 0.75f, 0.55f}},
	{{0.80f, 0.45f, 0.50f, 0.75f, 1.00f, 0.70f}},
	{{0.75f, 0.75f, 0.70f, 0.55f, 0.70f, 1.00f}},
}};

float coalition_cohesion(interest_group_mask mask) {
	auto const count = std::popcount(mask);
	if(count == 0)
		return 0.0f;
	if(count == 1)
		return 1.0f;

	float sum = 0.0f;
	uint32_t pairs = 0;
	for(std::size_t first = 0; first < interest_group_count; ++first) {
		if((mask & (interest_group_mask(1u) << first)) == 0)
			continue;
		for(std::size_t second = first + 1; second < interest_group_count; ++second) {
			if((mask & (interest_group_mask(1u) << second)) == 0)
				continue;
			sum += group_compatibility[first][second];
			++pairs;
		}
	}
	return pairs > 0 ? unit_interval(sum / float(pairs)) : 1.0f;
}

struct coalition_candidate {
	bool valid = false;
	interest_group_mask mask = 0;
	uint8_t group_count = 0;
	float power_share = 0.0f;
	float support_share = 0.0f;
	float cohesion = 0.0f;
	float score = 0.0f;
	bool majority = false;
};

bool candidate_is_better(coalition_candidate const& challenger, coalition_candidate const& incumbent) {
	if(!challenger.valid)
		return false;
	if(!incumbent.valid)
		return true;
	if(challenger.score > incumbent.score + comparison_epsilon)
		return true;
	if(incumbent.score > challenger.score + comparison_epsilon)
		return false;
	// The mask is a stable interest-group ID ordering. It is the final and only
	// tie-break so identical inputs never depend on container iteration order.
	return challenger.mask < incumbent.mask;
}

coalition_candidate make_candidate(
	interest_group_mask mask,
	std::array<float, interest_group_count> const& power,
	std::array<float, interest_group_count> const& support,
	ruleset_config const& config) {
	coalition_candidate result;
	result.valid = mask != 0;
	result.mask = mask;
	result.group_count = uint8_t(std::popcount(mask));
	for(std::size_t i = 0; i < interest_group_count; ++i) {
		if((mask & (interest_group_mask(1u) << i)) == 0)
			continue;
		result.power_share += power[i];
		result.support_share += support[i];
	}
	result.power_share = unit_interval(result.power_share);
	result.support_share = unit_interval(result.support_share);
	result.cohesion = coalition_cohesion(mask);

	auto const target = coalition_target(config);
	result.majority = result.power_share + comparison_epsilon >= target;
	auto const viability = unit_interval(result.power_share / target);
	result.score =
		bounded(config.coalition_power_weight, 0.45f, 0.0f, 10.0f) * viability
		+ bounded(config.coalition_support_weight, 0.15f, 0.0f, 10.0f) * result.support_share
		+ bounded(config.coalition_cohesion_weight, 0.20f, 0.0f, 10.0f) * result.cohesion
		+ (result.majority ? bounded(config.coalition_majority_bonus, 0.25f, 0.0f, 10.0f) : 0.0f)
		- bounded(config.coalition_size_penalty, 0.025f, 0.0f, 10.0f) * float(result.group_count - 1);
	result.score = finite_or(result.score, 0.0f);
	return result;
}

void normalize_shares(std::array<float, interest_group_count>& values) {
	float total = 0.0f;
	for(auto& value : values) {
		value = nonnegative(value);
		total += value;
	}
	if(total <= comparison_epsilon) {
		values.fill(0.0f);
		return;
	}
	for(auto& value : values)
		value /= total;
}

void assign_candidate(coalition_result& result, coalition_candidate const& chosen) {
	if(!chosen.valid)
		return;
	result.groups = chosen.mask;
	result.group_count = chosen.group_count;
	result.power_share = chosen.power_share;
	result.support_share = chosen.support_share;
	result.cohesion = chosen.cohesion;
	result.score = chosen.score;
	result.has_working_majority = chosen.majority;
}

} // namespace

std::array<float, interest_group_count> affinity_for_role(population_role role) {
	switch(role) {
	case population_role::landowner:
		return {{0.80f, 0.08f, 0.02f, 0.00f, 0.08f, 0.02f}};
	case population_role::capital_owner:
		return {{0.06f, 0.80f, 0.06f, 0.02f, 0.02f, 0.04f}};
	case population_role::artisan:
		return {{0.06f, 0.48f, 0.22f, 0.14f, 0.06f, 0.04f}};
	case population_role::intellectual:
		return {{0.04f, 0.07f, 0.68f, 0.10f, 0.04f, 0.07f}};
	case population_role::administrator:
		return {{0.08f, 0.10f, 0.30f, 0.05f, 0.04f, 0.43f}};
	case population_role::officer:
		return {{0.12f, 0.10f, 0.18f, 0.04f, 0.06f, 0.50f}};
	case population_role::soldier:
		return {{0.03f, 0.03f, 0.06f, 0.30f, 0.14f, 0.44f}};
	case population_role::industrial_worker:
		return {{0.01f, 0.04f, 0.11f, 0.72f, 0.08f, 0.04f}};
	case population_role::farmer:
		return {{0.07f, 0.03f, 0.06f, 0.14f, 0.66f, 0.04f}};
	case population_role::agricultural_laborer:
		return {{0.02f, 0.02f, 0.05f, 0.35f, 0.52f, 0.04f}};
	case population_role::enslaved:
		return {{0.00f, 0.00f, 0.03f, 0.38f, 0.56f, 0.03f}};
	case population_role::other_middle:
		return {{0.05f, 0.18f, 0.35f, 0.20f, 0.10f, 0.12f}};
	case population_role::other_rich:
		return {{0.36f, 0.38f, 0.15f, 0.03f, 0.03f, 0.05f}};
	case population_role::other_poor:
	default:
		return {{0.02f, 0.03f, 0.10f, 0.48f, 0.31f, 0.06f}};
	}
}

float default_property_proxy(population_role role) {
	switch(role) {
	case population_role::landowner: return 1.00f;
	case population_role::capital_owner: return 1.00f;
	case population_role::artisan: return 0.45f;
	case population_role::intellectual: return 0.12f;
	case population_role::administrator: return 0.10f;
	case population_role::officer: return 0.20f;
	case population_role::soldier: return 0.02f;
	case population_role::industrial_worker: return 0.03f;
	case population_role::farmer: return 0.35f;
	case population_role::agricultural_laborer: return 0.04f;
	case population_role::enslaved: return 0.00f;
	case population_role::other_middle: return 0.20f;
	case population_role::other_rich: return 0.70f;
	case population_role::other_poor:
	default: return 0.03f;
	}
}

interest_group_snapshot aggregate_interest_groups(
	std::vector<population_sample> const& samples,
	ruleset_config const& config) {
	interest_group_snapshot result;
	for(std::size_t i = 0; i < interest_group_count; ++i)
		result.groups[i].id = interest_group_id(i);
	if(!config.enabled)
		return result;

	result.enabled = true;
	std::array<group_accumulator, interest_group_count> accumulated{};
	double represented_population = 0.0;
	double total_support = 0.0;
	double total_power = 0.0;

	for(auto const& sample : samples) {
		auto const population = nonnegative(sample.population);
		if(population <= 0.0f)
			continue;
		auto const affinities = affinity_for_role(sample.role);
		auto const wealth = wealth_signal(sample.savings_per_capita, config);
		auto const income = unit_interval(sample.income_security);
		auto const property = unit_interval(sample.property_ownership);
		auto const literacy = unit_interval(sample.literacy);
		auto const consciousness = unit_interval(sample.consciousness);
		auto const individual_power = power_per_capita(sample, config, wealth);
		represented_population += population;

		for(std::size_t i = 0; i < interest_group_count; ++i) {
			auto const support = double(population) * double(unit_interval(affinities[i]));
			auto const power = support * double(individual_power);
			auto& group = accumulated[i];
			group.population_support += support;
			group.political_power += power;
			group.wealth += support * wealth;
			group.income += support * income;
			group.property += support * property;
			group.literacy += support * literacy;
			group.consciousness += support * consciousness;
			total_support += support;
			total_power += power;
		}
	}

	result.represented_population = finite_nonnegative_float(represented_population);
	for(std::size_t i = 0; i < interest_group_count; ++i) {
		auto const& source = accumulated[i];
		auto& destination = result.groups[i];
		destination.population_support = finite_nonnegative_float(source.population_support);
		destination.political_power = finite_nonnegative_float(source.political_power);
		if(source.population_support > 0.0) {
			auto const inverse_support = 1.0 / source.population_support;
			destination.mean_wealth_signal = float(source.wealth * inverse_support);
			destination.mean_income_security = float(source.income * inverse_support);
			destination.mean_property_ownership = float(source.property * inverse_support);
			destination.mean_literacy = float(source.literacy * inverse_support);
			destination.mean_consciousness = float(source.consciousness * inverse_support);
			destination.mean_power_per_capita = float(source.political_power * inverse_support);
			result.active_groups |= interest_group_mask(1u) << i;
		}
		if(total_support > 0.0)
			destination.support_share = float(source.population_support / total_support);
		if(total_power > 0.0)
			destination.political_power_share = float(source.political_power / total_power);
	}
	return result;
}

coalition_result select_governing_coalition(
	interest_group_snapshot const& snapshot,
	ruleset_config const& config,
	interest_group_mask previous_coalition) {
	coalition_result result;
	if(!config.enabled || !snapshot.enabled)
		return result;

	std::array<float, interest_group_count> power{};
	std::array<float, interest_group_count> support{};
	for(std::size_t i = 0; i < interest_group_count; ++i) {
		power[i] = snapshot.groups[i].political_power_share;
		support[i] = snapshot.groups[i].support_share;
	}
	normalize_shares(power);
	normalize_shares(support);

	interest_group_mask active = 0;
	for(std::size_t i = 0; i < interest_group_count; ++i) {
		if(power[i] > comparison_epsilon)
			active |= interest_group_mask(1u) << i;
	}
	if(active == 0)
		return result;

	std::array<coalition_candidate, all_group_bits + 1u> candidates{};
	coalition_candidate best_overall;
	auto const maximum_groups = maximum_coalition_groups(config);
	for(interest_group_mask mask = 1; mask <= all_group_bits; ++mask) {
		if((mask & ~active) != 0 || std::popcount(mask) > maximum_groups)
			continue;
		candidates[mask] = make_candidate(mask, power, support, config);
		if(candidate_is_better(candidates[mask], best_overall))
			best_overall = candidates[mask];
	}
	if(!best_overall.valid)
		return result;

	auto const previous_has_unknown_groups = (previous_coalition & ~all_group_bits) != 0;
	previous_coalition &= all_group_bits;
	auto const previous_is_valid = !previous_has_unknown_groups
		&& previous_coalition != 0 && candidates[previous_coalition].valid;
	coalition_candidate chosen = best_overall;
	if(previous_is_valid) {
		auto const incumbent = candidates[previous_coalition];
		coalition_candidate challenger;
		for(interest_group_mask mask = 1; mask <= all_group_bits; ++mask) {
			if(mask == previous_coalition)
				continue;
			if(candidate_is_better(candidates[mask], challenger))
				challenger = candidates[mask];
		}

		result.incumbent_score = incumbent.score;
		if(challenger.valid) {
			result.best_challenger = challenger.mask;
			result.best_challenger_score = challenger.score;
			result.challenger_advantage = challenger.score - incumbent.score;
			auto const hysteresis = bounded(config.coalition_hysteresis_margin, 0.03f, 0.0f, 1.0f);
			chosen = challenger.score <= incumbent.score + hysteresis ? incumbent : challenger;
		} else {
			chosen = incumbent;
		}
		result.retained_incumbent = chosen.mask == previous_coalition;
	} else {
		result.best_challenger = best_overall.mask;
		result.best_challenger_score = best_overall.score;
		result.challenger_advantage = best_overall.score;
	}

	assign_candidate(result, chosen);
	return result;
}

legitimacy_breakdown calculate_legitimacy(
	interest_group_snapshot const& snapshot,
	coalition_result const& coalition,
	ruleset_config const& config,
	interest_group_mask previous_coalition) {
	legitimacy_breakdown result;
	if(!config.enabled || !snapshot.enabled || coalition.groups == 0)
		return result;

	auto const power = unit_interval(coalition.power_share);
	auto const support = unit_interval(coalition.support_share);
	auto const cohesion = unit_interval(coalition.cohesion);
	auto const target = coalition_target(config);
	auto const group_count = std::clamp<uint8_t>(coalition.group_count, 1, uint8_t(interest_group_count));
	auto const max_groups = maximum_coalition_groups(config);

	result.power_mandate = 35.0f * unit_interval(power / target);
	result.popular_support = 25.0f * support;
	result.coalition_cohesion = 20.0f * cohesion;
	result.social_breadth = max_groups > 1
		? 10.0f * unit_interval(float(group_count - 1) / float(max_groups - 1))
		: 0.0f;
	result.continuity = previous_coalition != 0
		&& (previous_coalition & ~all_group_bits) == 0
		&& coalition.groups == previous_coalition ? 5.0f : 0.0f;
	result.majority_bonus = power + comparison_epsilon >= target ? 5.0f : 0.0f;
	result.minority_penalty = power < target ? 15.0f * unit_interval((target - power) / target) : 0.0f;
	result.representation_gap_penalty = 15.0f * std::max(0.0f, power - support);
	result.fragmentation_penalty = 2.0f * float(group_count - 1);

	auto const positive =
		result.power_mandate + result.popular_support + result.coalition_cohesion
		+ result.social_breadth + result.continuity + result.majority_bonus;
	auto const penalties =
		result.minority_penalty + result.representation_gap_penalty + result.fragmentation_penalty;
	result.total = std::clamp(finite_or(positive - penalties, 0.0f), 0.0f, 100.0f);
	return result;
}

nation_result evaluate_population(
	std::vector<population_sample> const& samples,
	ruleset_config const& config,
	interest_group_mask previous_coalition) {
	nation_result result;
	if(!config.enabled)
		return result;
	result.enabled = true;
	result.interest_groups = aggregate_interest_groups(samples, config);
	result.coalition = select_governing_coalition(result.interest_groups, config, previous_coalition);
	result.legitimacy = calculate_legitimacy(result.interest_groups, result.coalition, config, previous_coalition);
	return result;
}

population_role population_role_for_pop_type(sys::state const& state, dcon::pop_type_id pop_type) {
	if(pop_type == state.culture_definitions.aristocrat)
		return population_role::landowner;
	if(pop_type == state.culture_definitions.capitalists)
		return population_role::capital_owner;
	if(pop_type == state.culture_definitions.artisans)
		return population_role::artisan;
	if(pop_type == state.culture_definitions.clergy)
		return population_role::intellectual;
	if(pop_type == state.culture_definitions.bureaucrat)
		return population_role::administrator;
	if(pop_type == state.culture_definitions.officers)
		return population_role::officer;
	if(pop_type == state.culture_definitions.soldiers)
		return population_role::soldier;
	if(pop_type == state.culture_definitions.primary_factory_worker
		|| pop_type == state.culture_definitions.secondary_factory_worker)
		return population_role::industrial_worker;
	if(pop_type == state.culture_definitions.farmers)
		return population_role::farmer;
	if(pop_type == state.culture_definitions.laborers)
		return population_role::agricultural_laborer;
	if(pop_type == state.culture_definitions.slaves)
		return population_role::enslaved;

	auto const strata = culture::pop_strata(state.world.pop_type_get_strata(pop_type));
	switch(strata) {
	case culture::pop_strata::rich: return population_role::other_rich;
	case culture::pop_strata::middle: return population_role::other_middle;
	case culture::pop_strata::poor:
	default: return population_role::other_poor;
	}
}

population_sample sample_from_pop(sys::state const& state, dcon::pop_id pop) {
	population_sample result;
	if(!pop)
		return result;

	result.population = nonnegative(state.world.pop_get_size(pop));
	result.role = population_role_for_pop_type(state, state.world.pop_get_poptype(pop));
	if(result.population > 0.0f)
		result.savings_per_capita = nonnegative(state.world.pop_get_savings(pop)) / result.population;
	result.income_security =
		0.55f * unit_interval(pop_demographics::get_life_needs(state, pop))
		+ 0.30f * unit_interval(pop_demographics::get_everyday_needs(state, pop))
		+ 0.15f * unit_interval(pop_demographics::get_luxury_needs(state, pop));
	result.property_ownership = default_property_proxy(result.role);
	if(auto const province = state.world.pop_get_province_from_pop_location(pop); province) {
		auto const landed_share = unit_interval(state.world.province_get_landowners_share(province));
		auto const capitalist_share = unit_interval(state.world.province_get_capitalists_share(province));
		if(result.role == population_role::landowner) {
			result.property_ownership = unit_interval(0.25f + 0.75f * landed_share);
		} else if(result.role == population_role::capital_owner) {
			result.property_ownership = unit_interval(0.25f + 0.75f * capitalist_share);
		} else if(result.role == population_role::farmer) {
			result.property_ownership *= unit_interval(1.f - landed_share - capitalist_share);
		}
	}
	result.literacy = unit_interval(pop_demographics::get_literacy(state, pop));
	result.consciousness = unit_interval(pop_demographics::get_consciousness(state, pop) / 10.0f);
	return result;
}

std::vector<population_sample> collect_nation_population(sys::state const& state, dcon::nation_id nation) {
	std::vector<population_sample> result;
	if(!nation || !state.world.nation_is_valid(nation))
		return result;

	state.world.nation_for_each_province_ownership(nation, [&](dcon::province_ownership_id ownership) {
		auto const province = state.world.province_ownership_get_province(ownership);
		state.world.province_for_each_pop_location(province, [&](dcon::pop_location_id location) {
			result.push_back(sample_from_pop(state, state.world.pop_location_get_pop(location)));
		});
	});
	return result;
}

nation_result evaluate_nation(
	sys::state const& state,
	dcon::nation_id nation,
	ruleset_config const& config,
	interest_group_mask previous_coalition) {
	if(!config.enabled)
		return {};
	return evaluate_population(collect_nation_population(state, nation), config, previous_coalition);
}

issue_support_result evaluate_issue_support(sys::state& state,
	dcon::nation_id nation, dcon::issue_option_id option) {
	issue_support_result result;
	auto const config = ruleset_config_for(state);
	if(!config.enabled || !nation || !option || !state.world.nation_is_valid(nation))
		return result;
	result.enabled = true;

	std::array<double, interest_group_count> group_backing{};
	std::array<double, interest_group_count> group_power{};
	double popular_backing = 0.0;
	double population = 0.0;
	double power_backing = 0.0;
	double total_power = 0.0;
	auto const issue_key = pop_demographics::to_key(state, option);

	state.world.nation_for_each_province_ownership(nation, [&](dcon::province_ownership_id ownership) {
		auto const province = state.world.province_ownership_get_province(ownership);
		state.world.province_for_each_pop_location(province, [&](dcon::pop_location_id location) {
			auto const pop = state.world.pop_location_get_pop(location);
			auto const sample = sample_from_pop(state, pop);
			if(sample.population <= 0.f)
				return;
			auto const support = unit_interval(pop_demographics::get_demo(state, pop, issue_key));
			auto const affinity = affinity_for_role(sample.role);
			auto const per_capita_power = power_per_capita(sample, config, wealth_signal(sample.savings_per_capita, config));
			auto const pop_power = double(sample.population) * double(per_capita_power);
			popular_backing += double(sample.population) * double(support);
			population += double(sample.population);
			power_backing += pop_power * double(support);
			total_power += pop_power;
			for(std::size_t i = 0; i < interest_group_count; ++i) {
				auto const affinity_power = pop_power * double(affinity[i]);
				group_power[i] += affinity_power;
				group_backing[i] += affinity_power * double(support);
			}
		});
	});

	result.popular_support = population > 0.0 ? unit_interval(float(popular_backing / population)) : 0.f;
	result.political_power_support = total_power > 0.0 ? unit_interval(float(power_backing / total_power)) : 0.f;
	for(std::size_t i = 0; i < interest_group_count; ++i) {
		result.group_support[i] = group_power[i] > 0.0
			? unit_interval(float(group_backing[i] / group_power[i])) : 0.f;
	}

	if(auto const* political = cached_nation_result(state, nation); political && political->enabled) {
		float coalition_weight = 0.f;
		float opposition_weight = 0.f;
		for(std::size_t i = 0; i < interest_group_count; ++i) {
			auto const weight = political->interest_groups.groups[i].political_power_share;
			if((political->coalition.groups & (interest_group_mask(1u) << i)) != 0) {
				result.coalition_support += weight * result.group_support[i];
				coalition_weight += weight;
			} else {
				result.opposition_support += weight * result.group_support[i];
				opposition_weight += weight;
			}
		}
		result.coalition_support = coalition_weight > 0.f
			? unit_interval(result.coalition_support / coalition_weight) : 0.f;
		result.opposition_support = opposition_weight > 0.f
			? unit_interval(result.opposition_support / opposition_weight) : 0.f;
	}
	return result;
}

movement_pressure_breakdown calculate_movement_pressure(movement_pressure_inputs inputs) {
	movement_pressure_breakdown result;
	if(!inputs.enabled)
		return result;
	result.enabled = true;
	auto const power = unit_interval(inputs.political_power_support);
	auto const coalition = unit_interval(inputs.coalition_support);
	auto const legitimacy = unit_interval(inputs.legitimacy);
	auto const hardship = unit_interval(inputs.economic_hardship);
	auto const implementation_gap = unit_interval(inputs.implementation_gap);
	result.political_power_pressure = 18.f * power;
	result.coalition_opposition_pressure = 12.f * (1.f - coalition) * power;
	result.legitimacy_pressure = 12.f * (1.f - legitimacy);
	result.hardship_pressure = 25.f * hardship;
	result.implementation_pressure = 25.f * implementation_gap;
	result.legitimacy_relief = 12.f * legitimacy * coalition;
	result.total_adjustment = std::clamp(
		result.political_power_pressure + result.coalition_opposition_pressure
		+ result.legitimacy_pressure + result.hardship_pressure
		+ result.implementation_pressure - result.legitimacy_relief,
		-15.f, 60.f);
	return result;
}

movement_pressure_breakdown movement_pressure_for(sys::state& state,
	dcon::movement_id movement) {
	movement_pressure_inputs inputs;
	if(!ruleset_config_for(state).enabled || !movement || !state.world.movement_is_valid(movement))
		return calculate_movement_pressure(inputs);
	inputs.enabled = true;
	auto const nation = state.world.movement_get_nation_from_movement_within(movement);
	auto const option = state.world.movement_get_associated_issue_option(movement);
	if(option) {
		auto const support = evaluate_issue_support(state, nation, option);
		inputs.political_power_support = support.political_power_support;
		inputs.coalition_support = support.coalition_support;
	}
	if(auto const* political = cached_nation_result(state, nation); political && political->enabled)
		inputs.legitimacy = unit_interval(political->legitimacy.total / 100.f);

	double hardship = 0.0;
	double population = 0.0;
	for(auto membership : state.world.movement_get_pop_movement_membership(movement)) {
		auto const pop = membership.get_pop().id;
		auto const size = nonnegative(state.world.pop_get_size(pop));
		auto const needs =
			0.7f * unit_interval(pop_demographics::get_life_needs(state, pop))
			+ 0.3f * unit_interval(pop_demographics::get_everyday_needs(state, pop));
		hardship += double(size) * double(1.f - needs);
		population += double(size);
	}
	inputs.economic_hardship = population > 0.0 ? unit_interval(float(hardship / population)) : 0.f;
	inputs.implementation_gap = 1.f - nations::policy_execution::average_effective_policy(
		state, nation, nations::policy_execution::policy_kind::reform_implementation);
	return calculate_movement_pressure(inputs);
}

ruleset_config ruleset_config_for(sys::state const& state) {
	ruleset_config result;
	result.enabled = gamerule::age_of_transformation_enabled(state);
	return result;
}

void invalidate_cache(sys::state& state) {
	state.transformation_politics_cache.clear();
	state.transformation_politics_cache_valid = false;
}

void refresh_all_nations(sys::state& state) {
	auto const config = ruleset_config_for(state);
	auto const nation_count = state.world.nation_size();
	state.transformation_politics_cache.assign(nation_count, nation_result{});

	if(!config.enabled) {
		state.transformation_politics_cache_valid = true;
		return;
	}

	for(auto nation : state.world.in_nation) {
		auto const index = nation.id.index();
		// Keep runtime politics a pure function of serialized state. The pure
		// coalition API still supports hysteresis for a future saved incumbent.
		auto result = evaluate_nation(state, nation.id, config);
		state.transformation_politics_cache[index] = std::move(result);
	}
	state.transformation_politics_cache_valid = true;
}

nation_result const* cached_nation_result(sys::state& state, dcon::nation_id nation) {
	if(!nation || !state.world.nation_is_valid(nation))
		return nullptr;
	if(!state.transformation_politics_cache_valid
		|| state.transformation_politics_cache.size() != state.world.nation_size())
		refresh_all_nations(state);
	auto const index = nation.index();
	return index < state.transformation_politics_cache.size()
		? &state.transformation_politics_cache[index] : nullptr;
}

} // namespace politics::transformation
