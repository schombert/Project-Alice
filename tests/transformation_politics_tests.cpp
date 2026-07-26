#include "culture/transformation_politics.hpp"
#include "demographics.hpp"
#include "system_state.hpp"

#include "catch2/catch.hpp"

#include <cmath>
#include <limits>
#include <numeric>

namespace transformation = politics::transformation;

TEST_CASE("transformation interest-group affinities are bounded distributions", "[politics][transformation]") {
	for(uint8_t raw_role = uint8_t(transformation::population_role::landowner);
		raw_role <= uint8_t(transformation::population_role::other_rich);
		++raw_role) {
		auto const role = transformation::population_role(raw_role);
		auto const affinity = transformation::affinity_for_role(role);
		float total = 0.0f;
		for(auto const value : affinity) {
			REQUIRE(std::isfinite(value));
			REQUIRE(value >= 0.0f);
			REQUIRE(value <= 1.0f);
			total += value;
		}
		REQUIRE(total == Approx(1.0f));
		REQUIRE(transformation::default_property_proxy(role) >= 0.0f);
		REQUIRE(transformation::default_property_proxy(role) <= 1.0f);
	}
}

TEST_CASE("transformation politics is an explicit opt-in no-op", "[politics][transformation]") {
	transformation::ruleset_config config;
	std::vector<transformation::population_sample> samples{{
		transformation::population_role::capital_owner,
		1000.0f,
		100.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
	}};

	auto const result = transformation::evaluate_population(samples, config);
	REQUIRE_FALSE(result.enabled);
	REQUIRE_FALSE(result.interest_groups.enabled);
	REQUIRE(result.interest_groups.represented_population == 0.0f);
	REQUIRE(result.coalition.groups == 0);
	REQUIRE(result.legitimacy.total == 0.0f);
}

TEST_CASE("transformation support and political power normalize independently", "[politics][transformation]") {
	transformation::ruleset_config config;
	config.enabled = true;
	config.wealth_reference = 10.0f;
	std::vector<transformation::population_sample> samples{
		{transformation::population_role::landowner, 100.0f, 30.0f, 0.9f, 1.0f, 0.4f, 0.3f},
		{transformation::population_role::capital_owner, 200.0f, 50.0f, 1.0f, 1.0f, 0.8f, 0.8f},
		{transformation::population_role::intellectual, 300.0f, 2.0f, 0.8f, 0.1f, 1.0f, 0.9f},
		{transformation::population_role::industrial_worker, 800.0f, 0.5f, 0.6f, 0.05f, 0.7f, 0.6f},
		{transformation::population_role::farmer, 1000.0f, 1.0f, 0.7f, 0.35f, 0.4f, 0.3f},
		{transformation::population_role::administrator, 250.0f, 3.0f, 0.9f, 0.1f, 0.9f, 0.7f},
	};

	auto const snapshot = transformation::aggregate_interest_groups(samples, config);
	REQUIRE(snapshot.enabled);
	REQUIRE(snapshot.represented_population == Approx(2650.0f));
	REQUIRE(snapshot.active_groups == transformation::all_group_bits);

	float support_total = 0.0f;
	float power_total = 0.0f;
	float raw_support_total = 0.0f;
	for(auto const& group : snapshot.groups) {
		REQUIRE(std::isfinite(group.support_share));
		REQUIRE(std::isfinite(group.political_power_share));
		REQUIRE(group.support_share >= 0.0f);
		REQUIRE(group.support_share <= 1.0f);
		REQUIRE(group.political_power_share >= 0.0f);
		REQUIRE(group.political_power_share <= 1.0f);
		REQUIRE(group.mean_power_per_capita >= 0.0f);
		REQUIRE(group.mean_power_per_capita <= config.maximum_power_per_capita);
		support_total += group.support_share;
		power_total += group.political_power_share;
		raw_support_total += group.population_support;
	}
	REQUIRE(support_total == Approx(1.0f));
	REQUIRE(power_total == Approx(1.0f));
	REQUIRE(raw_support_total == Approx(snapshot.represented_population));
}

TEST_CASE("wealth income and property change power without inventing popular support", "[politics][transformation]") {
	transformation::ruleset_config config;
	config.enabled = true;
	config.wealth_reference = 10.0f;

	std::vector<transformation::population_sample> baseline{
		{transformation::population_role::capital_owner, 1000.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.2f},
		{transformation::population_role::industrial_worker, 1000.0f, 0.0f, 0.5f, 0.05f, 0.7f, 0.7f},
	};
	auto enriched = baseline;
	enriched[0].savings_per_capita = 100.0f;
	enriched[0].income_security = 1.0f;
	enriched[0].property_ownership = 1.0f;
	enriched[0].literacy = 1.0f;
	enriched[0].consciousness = 1.0f;

	auto const before = transformation::aggregate_interest_groups(baseline, config);
	auto const after = transformation::aggregate_interest_groups(enriched, config);
	auto const industrialists = std::size_t(transformation::interest_group_id::industrialists);
	REQUIRE(after.groups[industrialists].support_share == Approx(before.groups[industrialists].support_share));
	REQUIRE(after.groups[industrialists].political_power_share > before.groups[industrialists].political_power_share);
	REQUIRE(after.groups[industrialists].mean_property_ownership > before.groups[industrialists].mean_property_ownership);
	REQUIRE(after.groups[industrialists].mean_wealth_signal > before.groups[industrialists].mean_wealth_signal);
}

TEST_CASE("transformation aggregation sanitizes invalid simulation inputs", "[politics][transformation]") {
	transformation::ruleset_config config;
	config.enabled = true;
	config.wealth_reference = std::numeric_limits<float>::quiet_NaN();
	config.property_power_weight = std::numeric_limits<float>::infinity();
	std::vector<transformation::population_sample> samples{
		{transformation::population_role::landowner,
			std::numeric_limits<float>::infinity(), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
		{transformation::population_role::industrial_worker, 1000.0f,
			std::numeric_limits<float>::quiet_NaN(),
			std::numeric_limits<float>::infinity(), -1.0f,
			std::numeric_limits<float>::quiet_NaN(), 4.0f},
	};

	auto const snapshot = transformation::aggregate_interest_groups(samples, config);
	REQUIRE(snapshot.enabled);
	REQUIRE(snapshot.represented_population == Approx(1000.0f));
	float support_total = 0.0f;
	float power_total = 0.0f;
	for(auto const& group : snapshot.groups) {
		REQUIRE(std::isfinite(group.population_support));
		REQUIRE(std::isfinite(group.political_power));
		REQUIRE(std::isfinite(group.support_share));
		REQUIRE(std::isfinite(group.political_power_share));
		REQUIRE(std::isfinite(group.mean_wealth_signal));
		REQUIRE(std::isfinite(group.mean_income_security));
		REQUIRE(std::isfinite(group.mean_property_ownership));
		REQUIRE(std::isfinite(group.mean_literacy));
		REQUIRE(std::isfinite(group.mean_consciousness));
		support_total += group.support_share;
		power_total += group.political_power_share;
	}
	REQUIRE(support_total == Approx(1.0f));
	REQUIRE(power_total == Approx(1.0f));
}

TEST_CASE("coalition selection uses stable group IDs to break exact ties", "[politics][transformation]") {
	transformation::ruleset_config config;
	config.enabled = true;
	config.maximum_coalition_groups = 1;
	transformation::interest_group_snapshot snapshot;
	snapshot.enabled = true;
	auto const landed = std::size_t(transformation::interest_group_id::landed_elites);
	auto const industrialists = std::size_t(transformation::interest_group_id::industrialists);
	snapshot.groups[landed].support_share = 0.5f;
	snapshot.groups[landed].political_power_share = 0.5f;
	snapshot.groups[industrialists].support_share = 0.5f;
	snapshot.groups[industrialists].political_power_share = 0.5f;

	auto const coalition = transformation::select_governing_coalition(snapshot, config);
	REQUIRE(coalition.groups == transformation::group_bit(transformation::interest_group_id::landed_elites));
	REQUIRE(coalition.has_working_majority);

	auto const malformed_incumbent =
		transformation::group_bit(transformation::interest_group_id::industrialists) | (1u << 20u);
	auto const sanitized = transformation::select_governing_coalition(snapshot, config, malformed_incumbent);
	REQUIRE(sanitized.groups == transformation::group_bit(transformation::interest_group_id::landed_elites));
	REQUIRE_FALSE(sanitized.retained_incumbent);
}

TEST_CASE("coalition selection rewards a cohesive working majority", "[politics][transformation]") {
	transformation::ruleset_config config;
	config.enabled = true;
	config.maximum_coalition_groups = 2;
	config.coalition_power_target = 0.55f;
	transformation::interest_group_snapshot snapshot;
	snapshot.enabled = true;
	auto const landed = std::size_t(transformation::interest_group_id::landed_elites);
	auto const labor = std::size_t(transformation::interest_group_id::organized_labor);
	auto const rural = std::size_t(transformation::interest_group_id::rural_communities);
	snapshot.groups[landed].support_share = snapshot.groups[landed].political_power_share = 0.42f;
	snapshot.groups[labor].support_share = snapshot.groups[labor].political_power_share = 0.28f;
	snapshot.groups[rural].support_share = snapshot.groups[rural].political_power_share = 0.30f;

	auto const coalition = transformation::select_governing_coalition(snapshot, config);
	auto const expected =
		transformation::group_bit(transformation::interest_group_id::landed_elites)
		| transformation::group_bit(transformation::interest_group_id::rural_communities);
	REQUIRE(coalition.groups == expected);
	REQUIRE(coalition.group_count == 2);
	REQUIRE(coalition.has_working_majority);
	REQUIRE(coalition.power_share == Approx(0.72f));
	REQUIRE(coalition.cohesion == Approx(0.80f));
}

TEST_CASE("coalition hysteresis keeps narrow incumbents and permits decisive changes", "[politics][transformation]") {
	transformation::ruleset_config config;
	config.enabled = true;
	config.maximum_coalition_groups = 1;
	config.coalition_majority_bonus = 0.0f;
	config.coalition_hysteresis_margin = 0.02f;
	transformation::interest_group_snapshot snapshot;
	snapshot.enabled = true;
	auto const landed = std::size_t(transformation::interest_group_id::landed_elites);
	auto const industrialists = std::size_t(transformation::interest_group_id::industrialists);
	snapshot.groups[landed].support_share = snapshot.groups[landed].political_power_share = 0.51f;
	snapshot.groups[industrialists].support_share = snapshot.groups[industrialists].political_power_share = 0.49f;
	auto const incumbent = transformation::group_bit(transformation::interest_group_id::industrialists);

	auto const stable = transformation::select_governing_coalition(snapshot, config, incumbent);
	REQUIRE(stable.groups == incumbent);
	REQUIRE(stable.retained_incumbent);
	REQUIRE(stable.best_challenger == transformation::group_bit(transformation::interest_group_id::landed_elites));
	REQUIRE(stable.challenger_advantage > 0.0f);
	REQUIRE(stable.challenger_advantage < config.coalition_hysteresis_margin);

	snapshot.groups[landed].support_share = snapshot.groups[landed].political_power_share = 0.80f;
	snapshot.groups[industrialists].support_share = snapshot.groups[industrialists].political_power_share = 0.20f;
	auto const changed = transformation::select_governing_coalition(snapshot, config, incumbent);
	REQUIRE(changed.groups == transformation::group_bit(transformation::interest_group_id::landed_elites));
	REQUIRE_FALSE(changed.retained_incumbent);
	REQUIRE(changed.challenger_advantage > config.coalition_hysteresis_margin);
}

TEST_CASE("legitimacy exposes an exact explainable balance", "[politics][transformation]") {
	transformation::ruleset_config config;
	config.enabled = true;
	config.maximum_coalition_groups = 3;
	config.coalition_power_target = 0.50f;
	transformation::interest_group_snapshot snapshot;
	snapshot.enabled = true;
	transformation::coalition_result coalition;
	coalition.groups =
		transformation::group_bit(transformation::interest_group_id::intelligentsia)
		| transformation::group_bit(transformation::interest_group_id::organized_labor);
	coalition.group_count = 2;
	coalition.power_share = 0.55f;
	coalition.support_share = 0.55f;
	coalition.cohesion = 0.80f;
	coalition.has_working_majority = true;

	auto const balanced = transformation::calculate_legitimacy(snapshot, coalition, config, coalition.groups);
	auto const positive = balanced.power_mandate + balanced.popular_support + balanced.coalition_cohesion
		+ balanced.social_breadth + balanced.continuity + balanced.majority_bonus;
	auto const penalties = balanced.minority_penalty + balanced.representation_gap_penalty
		+ balanced.fragmentation_penalty;
	REQUIRE(balanced.total == Approx(positive - penalties));
	REQUIRE(balanced.continuity == Approx(5.0f));
	REQUIRE(balanced.minority_penalty == 0.0f);
	REQUIRE(balanced.representation_gap_penalty == 0.0f);
	REQUIRE(balanced.total >= 0.0f);
	REQUIRE(balanced.total <= 100.0f);

	auto overrepresented_coalition = coalition;
	overrepresented_coalition.support_share = 0.25f;
	auto const overrepresented = transformation::calculate_legitimacy(snapshot, overrepresented_coalition, config);
	REQUIRE(overrepresented.representation_gap_penalty > 0.0f);
	REQUIRE(overrepresented.total < balanced.total);

	auto minority_coalition = coalition;
	minority_coalition.power_share = 0.30f;
	minority_coalition.support_share = 0.45f;
	minority_coalition.has_working_majority = false;
	auto const minority = transformation::calculate_legitimacy(snapshot, minority_coalition, config);
	REQUIRE(minority.minority_penalty > 0.0f);
	REQUIRE(minority.majority_bonus == 0.0f);
	REQUIRE(minority.total < balanced.total);
}

TEST_CASE("Project Alice POP adapter derives roles and an opt-in nation result", "[politics][transformation][integration]") {
	auto state = std::make_unique<sys::state>();
	auto const capitalist_type = state->world.create_pop_type();
	auto const worker_type = state->world.create_pop_type();
	state->culture_definitions.capitalists = capitalist_type;
	state->culture_definitions.primary_factory_worker = worker_type;
	auto const nation = state->world.create_nation();
	auto const province = state->world.create_province();
	state->world.province_set_nation_from_province_ownership(province, nation);
	state->world.province_set_capitalists_share(province, 1.0f);
	state->world.province_set_capitalists_share(province, 1.0f);

	auto const capitalist = state->world.create_pop();
	state->world.pop_set_poptype(capitalist, capitalist_type);
	state->world.pop_set_size(capitalist, 1000.0f);
	state->world.pop_set_savings(capitalist, 10000.0f);
	state->world.pop_set_satisfaction(capitalist, 1.0f);
	state->world.pop_set_uliteracy(capitalist, pop_demographics::to_pu16(0.80f));
	state->world.pop_set_uconsciousness(capitalist, pop_demographics::to_pmc(7.0f));
	state->world.force_create_pop_location(capitalist, province);

	auto const worker = state->world.create_pop();
	state->world.pop_set_poptype(worker, worker_type);
	state->world.pop_set_size(worker, 2000.0f);
	state->world.pop_set_savings(worker, 1000.0f);
	state->world.pop_set_satisfaction(worker, 0.20f);
	state->world.pop_set_uliteracy(worker, pop_demographics::to_pu16(0.60f));
	state->world.pop_set_uconsciousness(worker, pop_demographics::to_pmc(5.0f));
	state->world.force_create_pop_location(worker, province);

	REQUIRE(transformation::population_role_for_pop_type(*state, capitalist_type)
		== transformation::population_role::capital_owner);
	REQUIRE(transformation::population_role_for_pop_type(*state, worker_type)
		== transformation::population_role::industrial_worker);
	auto const capitalist_sample = transformation::sample_from_pop(*state, capitalist);
	REQUIRE(capitalist_sample.population == Approx(1000.0f));
	REQUIRE(capitalist_sample.savings_per_capita == Approx(10.0f));
	REQUIRE(capitalist_sample.property_ownership == Approx(1.0f));
	REQUIRE(capitalist_sample.literacy == Approx(0.80f).margin(0.001f));
	REQUIRE(capitalist_sample.consciousness == Approx(0.70f).margin(0.001f));

	transformation::ruleset_config disabled;
	REQUIRE_FALSE(transformation::evaluate_nation(*state, nation, disabled).enabled);
	transformation::ruleset_config enabled;
	enabled.enabled = true;
	auto const result = transformation::evaluate_nation(*state, nation, enabled);
	REQUIRE(result.enabled);
	REQUIRE(result.interest_groups.represented_population == Approx(3000.0f));
	REQUIRE(result.coalition.groups != 0);
	REQUIRE(result.legitimacy.total >= 0.0f);
	REQUIRE(result.legitimacy.total <= 100.0f);
}

TEST_CASE("movement pressure is explainable and legacy compatible", "[politics][transformation][movement]") {
	transformation::movement_pressure_inputs legacy;
	auto const no_op = transformation::calculate_movement_pressure(legacy);
	REQUIRE_FALSE(no_op.enabled);
	REQUIRE(no_op.total_adjustment == Approx(0.f));

	transformation::movement_pressure_inputs stable;
	stable.enabled = true;
	stable.political_power_support = 0.25f;
	stable.coalition_support = 0.80f;
	stable.legitimacy = 0.90f;
	stable.economic_hardship = 0.10f;
	stable.implementation_gap = 0.10f;
	auto const low_pressure = transformation::calculate_movement_pressure(stable);

	auto crisis = stable;
	crisis.political_power_support = 0.80f;
	crisis.coalition_support = 0.10f;
	crisis.legitimacy = 0.20f;
	crisis.economic_hardship = 0.90f;
	crisis.implementation_gap = 0.80f;
	auto const high_pressure = transformation::calculate_movement_pressure(crisis);
	REQUIRE(high_pressure.total_adjustment > low_pressure.total_adjustment);
	REQUIRE(high_pressure.total_adjustment <= 60.f);
	auto const unclamped =
		high_pressure.political_power_pressure
		+ high_pressure.coalition_opposition_pressure
		+ high_pressure.legitimacy_pressure
		+ high_pressure.hardship_pressure
		+ high_pressure.implementation_pressure
		- high_pressure.legitimacy_relief;
	REQUIRE(unclamped > high_pressure.total_adjustment);
	REQUIRE(high_pressure.total_adjustment == Approx(60.f));
}

TEST_CASE("wealth-backed groups can outweigh equal popular reform support",
	"[politics][transformation][reform][integration]") {
	auto state = std::make_unique<sys::state>();
	state->force_age_of_transformation_ruleset = true;
	auto const capitalist_type = state->world.create_pop_type();
	auto const worker_type = state->world.create_pop_type();
	state->culture_definitions.capitalists = capitalist_type;
	state->culture_definitions.primary_factory_worker = worker_type;
	auto const issue = state->world.create_issue();
	auto const option = state->world.create_issue_option();
	state->world.issue_option_set_parent_issue(option, issue);
	auto const nation = state->world.create_nation();
	auto const province = state->world.create_province();
	state->world.province_set_nation_from_province_ownership(province, nation);
	state->world.province_set_capitalists_share(province, 1.f);

	auto const capitalist = state->world.create_pop();
	auto const worker = state->world.create_pop();
	state->world.pop_resize_udemographics(pop_demographics::size(*state));
	state->world.pop_set_poptype(capitalist, capitalist_type);
	state->world.pop_set_size(capitalist, 1000.f);
	state->world.pop_set_savings(capitalist, 100000.f);
	state->world.pop_set_uliteracy(capitalist, pop_demographics::to_pu16(1.f));
	state->world.pop_set_uconsciousness(capitalist, pop_demographics::to_pmc(10.f));
	state->world.force_create_pop_location(capitalist, province);
	state->world.pop_set_poptype(worker, worker_type);
	state->world.pop_set_size(worker, 1000.f);
	state->world.pop_set_savings(worker, 0.f);
	state->world.force_create_pop_location(worker, province);
	pop_demographics::set_demo(*state, capitalist, pop_demographics::to_key(*state, option), 1.f);
	pop_demographics::set_demo(*state, worker, pop_demographics::to_key(*state, option), 0.f);

	auto const support = transformation::evaluate_issue_support(*state, nation, option);
	REQUIRE(support.enabled);
	REQUIRE(support.popular_support == Approx(0.5f));
	REQUIRE(support.political_power_support > support.popular_support);
	REQUIRE(support.group_support[std::size_t(transformation::interest_group_id::industrialists)] > 0.5f);
}
