#include "economy/human_development.hpp"

#include "economy/advanced_province_buildings.hpp"
#include "economy/demographics.hpp"
#include "economy/demographics_templates.hpp"
#include "system_state.hpp"

#include "catch2/catch.hpp"

#include <cmath>
#include <limits>
#include <memory>

namespace human_development = economy::human_development;

TEST_CASE("human development is an exact classic-rules no-op",
		"[economy][demographics][human-development]") {
	human_development::config rules{};
	auto const result = human_development::calculate(rules, {
		.housing_access = 0.f,
		.urbanization = 1.f,
		.education_access = 1.f,
		.literacy = 1.f,
	});
	REQUIRE_FALSE(result.enabled);
	REQUIRE(result.monthly_growth_adjustment == Approx(0.f));
	REQUIRE(result.monthly_militancy_adjustment == Approx(0.f));
	REQUIRE(result.migration_quality == Approx(1.f));
}

TEST_CASE("urban housing shortage produces bounded demographic and political pressure",
		"[economy][demographics][human-development]") {
	human_development::config rules{};
	rules.enabled = true;
	auto const housed = human_development::calculate(rules, {
		.housing_access = 1.f,
		.urbanization = 1.f,
	});
	auto const overcrowded = human_development::calculate(rules, {
		.housing_access = 0.f,
		.urbanization = 1.f,
	});
	REQUIRE(overcrowded.enabled);
	REQUIRE(overcrowded.overcrowding == Approx(1.f));
	REQUIRE(overcrowded.monthly_growth_adjustment
		< housed.monthly_growth_adjustment);
	REQUIRE(overcrowded.monthly_overcrowding_growth_penalty
		== Approx(rules.maximum_overcrowding_growth_penalty));
	REQUIRE(overcrowded.monthly_militancy_adjustment
		== Approx(rules.maximum_overcrowding_militancy));
	REQUIRE(overcrowded.migration_quality < housed.migration_quality);
}

TEST_CASE("education literacy and urbanization drive the demographic transition",
		"[economy][demographics][human-development]") {
	human_development::config rules{};
	rules.enabled = true;
	auto const rural = human_development::calculate(rules, {
		.housing_access = 1.f,
		.urbanization = 0.f,
		.education_access = 1.f,
		.literacy = 1.f,
	});
	auto const developed_city = human_development::calculate(rules, {
		.housing_access = 1.f,
		.urbanization = 1.f,
		.education_access = 1.f,
		.literacy = 1.f,
	});
	REQUIRE(rural.demographic_transition == Approx(0.f));
	REQUIRE(developed_city.demographic_transition == Approx(1.f));
	REQUIRE(developed_city.monthly_transition_growth_reduction
		== Approx(rules.maximum_transition_growth_reduction));
	REQUIRE(developed_city.human_development_index == Approx(1.f));
}

TEST_CASE("housing availability redirects migration without runaway weights",
		"[economy][demographics][human-development]") {
	human_development::config rules{};
	rules.enabled = true;
	auto const shortage = human_development::calculate(rules, {
		.housing_access = 0.f,
		.urbanization = 1.f,
	});
	auto const available = human_development::calculate(rules, {
		.housing_access = 1.f,
		.urbanization = 1.f,
	});
	auto const toward_available = human_development::migration_multiplier(
		rules, shortage, available);
	auto const toward_shortage = human_development::migration_multiplier(
		rules, available, shortage);
	REQUIRE(toward_available > 1.f);
	REQUIRE(toward_shortage < 1.f);
	REQUIRE(toward_available <= rules.maximum_migration_multiplier);
	REQUIRE(toward_shortage >= rules.minimum_migration_multiplier);
}

TEST_CASE("demographic account reconciles all natural-change components",
		"[economy][demographics][human-development]") {
	human_development::config rules{};
	rules.enabled = true;
	auto const development = human_development::calculate(rules, {
		.housing_access = 0.5f,
		.urbanization = 0.8f,
		.education_access = 0.75f,
		.literacy = 0.60f,
	});
	auto const account = human_development::calculate_account(
		100000.f, 0.001f, -0.0002f, development);
	REQUIRE(account.baseline_natural_growth == Approx(100.f));
	REQUIRE(account.starvation_loss == Approx(20.f));
	REQUIRE(account.housing_loss > 0.f);
	REQUIRE(account.transition_reduction > 0.f);
	REQUIRE(account.net_natural_change == Approx(
		account.baseline_natural_growth - account.starvation_loss
		- account.housing_loss - account.transition_reduction));
}

TEST_CASE("human development sanitizes malformed simulation inputs",
		"[economy][demographics][human-development]") {
	human_development::config rules{};
	rules.enabled = true;
	auto const result = human_development::calculate(rules, {
		.housing_access = std::numeric_limits<float>::quiet_NaN(),
		.urbanization = std::numeric_limits<float>::infinity(),
		.education_access = -4.f,
		.literacy = 9.f,
	});
	REQUIRE(std::isfinite(result.human_development_index));
	REQUIRE(std::isfinite(result.monthly_growth_adjustment));
	REQUIRE(std::isfinite(result.monthly_militancy_adjustment));
	REQUIRE(result.human_development_index >= 0.f);
	REQUIRE(result.human_development_index <= 1.f);
}

TEST_CASE("state adapter reads existing city housing education and POP literacy",
		"[economy][demographics][human-development][integration]") {
	auto state = std::make_unique<sys::state>();
	state->force_age_of_transformation_ruleset = true;
	auto const province = state->world.create_province();
	auto const pop = state->world.create_pop();
	state->world.force_create_pop_location(pop, province);
	state->world.pop_set_size(pop, 1000.f);
	state->world.pop_set_uliteracy(pop, pop_demographics::to_pu16(0.8f));

	state->world.province_resize_demographics(demographics::size(*state));
	services::initialize_size_of_dcon_arrays(*state);
	advanced_province_buildings::initialize_size_of_dcon_arrays(*state);
	state->world.province_set_demographics(province, demographics::total, 1000.f);
	state->world.province_set_advanced_province_building_max_private_size(
		province, advanced_province_buildings::list::local_cities_and_towns, 800.f);
	state->world.province_set_service_satisfaction(
		province, services::list::urban_housing, 0.5f);
	state->world.province_set_service_demand_forbidden_public_supply(
		province, services::list::urban_housing, 100.f);
	state->world.province_set_service_supply_private(
		province, services::list::urban_housing, 100.f);
	state->world.province_set_service_satisfaction(
		province, services::list::education, 0.5f);
	state->world.province_set_service_satisfaction_for_free(
		province, services::list::education, 0.5f);

	auto const result = human_development::evaluate_pop(*state, pop);
	REQUIRE(result.enabled);
	REQUIRE(result.factors.urbanization == Approx(0.8f));
	REQUIRE(result.factors.housing_access == Approx(0.5f));
	REQUIRE(result.factors.education_access == Approx(0.75f));
	REQUIRE(result.factors.literacy == Approx(0.8f).margin(0.001f));
	REQUIRE(result.overcrowding == Approx(0.4f));
}

TEST_CASE("an inactive housing market is demographically neutral",
		"[economy][demographics][human-development][integration]") {
	auto state = std::make_unique<sys::state>();
	state->force_age_of_transformation_ruleset = true;
	auto const province = state->world.create_province();
	state->world.province_resize_demographics(demographics::size(*state));
	services::initialize_size_of_dcon_arrays(*state);
	advanced_province_buildings::initialize_size_of_dcon_arrays(*state);
	state->world.province_set_demographics(province, demographics::total, 1000.f);
	state->world.province_set_advanced_province_building_max_private_size(
		province, advanced_province_buildings::list::local_cities_and_towns, 1000.f);
	state->world.province_set_service_satisfaction(
		province, services::list::urban_housing, 0.f);
	state->world.province_set_service_demand_forbidden_public_supply(
		province, services::list::urban_housing, 0.f);

	auto const result = human_development::evaluate_province(*state, province, 0.f);
	REQUIRE(result.enabled);
	REQUIRE(result.factors.housing_access == Approx(1.f));
	REQUIRE(result.overcrowding == Approx(0.f));
	REQUIRE(result.monthly_overcrowding_growth_penalty == Approx(0.f));
}

TEST_CASE("overcrowding reaches monthly POP growth while classic growth is unchanged",
		"[economy][demographics][human-development][integration]") {
	auto state = std::make_unique<sys::state>();
	auto const nation = state->world.create_nation();
	auto const province = state->world.create_province();
	auto const pop_type = state->world.create_pop_type();
	auto const pop = state->world.create_pop();
	state->world.province_set_nation_from_province_ownership(province, nation);
	state->world.force_create_pop_location(pop, province);
	state->world.pop_set_poptype(pop, pop_type);
	state->world.pop_set_size(pop, 1000.f);
	state->world.pop_set_uliteracy(pop, pop_demographics::to_pu16(0.f));

	state->world.province_resize_modifier_values(sys::provincial_mod_offsets::count);
	state->world.nation_resize_modifier_values(sys::national_mod_offsets::count);
	state->world.province_resize_demographics(demographics::size(*state));
	services::initialize_size_of_dcon_arrays(*state);
	advanced_province_buildings::initialize_size_of_dcon_arrays(*state);
	state->world.province_set_demographics(province, demographics::total, 1000.f);
	state->world.province_set_advanced_province_building_max_private_size(
		province, advanced_province_buildings::list::local_cities_and_towns, 1000.f);
	state->world.province_set_service_satisfaction(
		province, services::list::urban_housing, 0.f);
	state->world.province_set_service_demand_forbidden_public_supply(
		province, services::list::urban_housing, 100.f);
	state->world.province_set_service_supply_private(
		province, services::list::urban_housing, 100.f);
	state->defines.base_popgrowth = 0.001f;
	state->defines.life_rating_growth_bonus = 0.f;
	state->defines.life_need_starvation_limit = 0.f;

	state->force_age_of_transformation_ruleset = false;
	REQUIRE(demographics::get_monthly_pop_growth_factor(*state, pop)
		== Approx(0.001f));
	state->force_age_of_transformation_ruleset = true;
	REQUIRE(demographics::get_monthly_pop_growth_factor(*state, pop)
		== Approx(0.001f - 0.0012f));

	demographics::update_growth(*state, 0, 31);
	REQUIRE(state->world.pop_get_size(pop) == Approx(999.8f));
}
