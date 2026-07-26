#include "economy/economy_pops.hpp"
#include "economy/price.hpp"

#include <array>
#include <limits>

TEST_CASE("commodity price ceiling is tied to the base price", "[economy][price]") {
	REQUIRE(economy::price_properties::commodity::maximum(2.5f) == Approx(250.f));
	REQUIRE(economy::price_properties::commodity::maximum(0.f) == Approx(economy::price_properties::commodity::min));
}

TEST_CASE("safe spending ratio returns fallback when required is zero", "[economy][pops]") {
	auto scalar = economy::pops::safe_spending_ratio(true, 0.0f, 0.0f, 1.0f);
	REQUIRE(scalar == 1.0f);

	auto vector = economy::pops::safe_spending_ratio(
		ve::mask_vector(true, false, true, false),
		ve::fp_vector(0.0f, 2.0f, 0.0f, 6.0f),
		ve::fp_vector(0.0f, 4.0f, 0.0f, 3.0f),
		ve::fp_vector(1.0f)
	);
	REQUIRE(vector[0] == 1.0f);
	REQUIRE(vector[1] == 0.5f);
	REQUIRE(vector[2] == 1.0f);
	REQUIRE(vector[3] == 2.0f);
}

TEST_CASE("safe ratio returns zero when denominator is zero", "[economy][pops]") {
	auto scalar = economy::pops::safe_ratio_or_zero(true, 0.0f, 0.0f);
	REQUIRE(scalar == 0.0f);

	auto vector = economy::pops::safe_ratio_or_zero(
		ve::mask_vector(true, false, true, false),
		ve::fp_vector(0.0f, 2.0f, 0.0f, 6.0f),
		ve::fp_vector(0.0f, 4.0f, 0.0f, 3.0f)
	);
	REQUIRE(vector[0] == 0.0f);
	REQUIRE(vector[1] == 0.5f);
	REQUIRE(vector[2] == 0.0f);
	REQUIRE(vector[3] == 2.0f);
}

TEST_CASE("migration opportunity multiplier is neutral for equal coverage", "[economy][migration]") {
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(1.f, 1.f) == Approx(1.f));
}

TEST_CASE("migration opportunity multiplier has a bounded square root response", "[economy][migration]") {
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(1.f, 4.f) == Approx(2.f));
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(4.f, 1.f) == Approx(0.5f));
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(0.f, 100.f) == Approx(2.f));
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(100.f, 0.f) == Approx(0.5f));
}

TEST_CASE("expected life needs coverage uses sold labor exactly once", "[economy][migration]") {
	auto full_employment = demographics::life_needs_coverage_from_components(2.f, 1.f, 400000.f, 400000.f, 0.f);
	auto half_employment = demographics::life_needs_coverage_from_components(2.f, 0.5f, 400000.f, 400000.f, 0.f);
	REQUIRE(full_employment == Approx(2.f));
	REQUIRE(half_employment == Approx(1.f));
}

TEST_CASE("expensive life needs reduce migration opportunity", "[economy][migration]") {
	auto cheap = demographics::life_needs_coverage_from_components(1.f, 1.f, 400000.f, 400000.f, 0.f);
	auto expensive = demographics::life_needs_coverage_from_components(1.f, 1.f, 800000.f, 400000.f, 0.f);
	REQUIRE(cheap == Approx(1.f));
	REQUIRE(expensive == Approx(0.5f));
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(expensive, cheap) > 1.f);
}

TEST_CASE("subsistence is added as free needs coverage rather than money", "[economy][migration]") {
	auto without_subsistence = demographics::life_needs_coverage_from_components(0.f, 0.f, 400000.f, 400000.f, 0.f);
	auto with_subsistence = demographics::life_needs_coverage_from_components(0.f, 0.f, 400000.f, 400000.f, 0.4f);
	REQUIRE(without_subsistence == Approx(0.f));
	REQUIRE(with_subsistence == Approx(0.4f));
}

TEST_CASE("zero life needs cost remains finite and bounded", "[economy][migration]") {
	auto coverage = demographics::life_needs_coverage_from_components(1.f, 1.f, 0.f, 400000.f, 0.f);
	REQUIRE(std::isfinite(coverage));
	auto multiplier = demographics::migration_opportunity_multiplier_from_coverage(1.f, coverage);
	REQUIRE(std::isfinite(multiplier));
	REQUIRE(multiplier >= 0.5f);
	REQUIRE(multiplier <= 2.f);
}

TEST_CASE("migration opportunity rejects non-finite market data", "[economy][migration]") {
	auto nan = std::numeric_limits<float>::quiet_NaN();
	auto infinity = std::numeric_limits<float>::infinity();
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(nan, 1.f) == Approx(1.f));
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(1.f, infinity) == Approx(1.f));

	for(auto coverage : {
		 demographics::life_needs_coverage_from_income(nan, 1.f, 1.f, 0.f),
		 demographics::life_needs_coverage_from_income(1.f, infinity, 1.f, 0.25f),
		 demographics::life_needs_coverage_from_income(1.f, 1.f, 0.f, 0.25f),
		 demographics::life_needs_coverage_from_components(infinity, infinity, 1.f, 1.f, 0.f)
	}) {
		REQUIRE(std::isfinite(coverage));
		REQUIRE(coverage >= 0.f);
		REQUIRE(coverage <= 1000.f);
	}
}

TEST_CASE("population transfers conserve population and liquid savings", "[economy][migration]") {
	auto state = std::make_unique<sys::state>();
	auto source = state->world.create_pop();
	auto target = state->world.create_pop();
	state->world.pop_set_size(source, 100.f);
	state->world.pop_set_size(target, 50.f);
	state->world.pop_set_savings(source, 40.f);
	state->world.pop_set_savings(target, 5.f);

	auto const moved = demographics::transfer_pop_amount(*state, source, target, 25.f);
	REQUIRE(moved == Approx(25.f));
	REQUIRE(state->world.pop_get_size(source) == Approx(75.f));
	REQUIRE(state->world.pop_get_size(target) == Approx(75.f));
	REQUIRE(state->world.pop_get_savings(source) == Approx(30.f));
	REQUIRE(state->world.pop_get_savings(target) == Approx(15.f));
	REQUIRE(state->world.pop_get_size(source) + state->world.pop_get_size(target) == Approx(150.f));
	REQUIRE(state->world.pop_get_savings(source) + state->world.pop_get_savings(target) == Approx(45.f));
}

TEST_CASE("population transfers absorb a cleanup-sized remainder", "[economy][migration]") {
	auto state = std::make_unique<sys::state>();
	auto source = state->world.create_pop();
	auto target = state->world.create_pop();
	state->world.pop_set_size(source, 10.f);
	state->world.pop_set_size(target, 20.f);
	state->world.pop_set_savings(source, 7.f);
	state->world.pop_set_savings(target, 3.f);

	auto const moved = demographics::transfer_pop_amount(*state, source, target, 9.5f);
	REQUIRE(moved == Approx(10.f));
	REQUIRE(state->world.pop_get_size(source) == Approx(0.f));
	REQUIRE(state->world.pop_get_size(target) == Approx(30.f));
	REQUIRE(state->world.pop_get_savings(source) == Approx(0.f));
	REQUIRE(state->world.pop_get_savings(target) == Approx(10.f));
}

TEST_CASE("population transfer to the same POP is a no-op", "[economy][migration]") {
	auto state = std::make_unique<sys::state>();
	auto pop = state->world.create_pop();
	state->world.pop_set_size(pop, 10.f);
	state->world.pop_set_savings(pop, 7.f);

	REQUIRE(demographics::transfer_pop_amount(*state, pop, pop, 5.f) == Approx(0.f));
	REQUIRE(state->world.pop_get_size(pop) == Approx(10.f));
	REQUIRE(state->world.pop_get_savings(pop) == Approx(7.f));
}

TEST_CASE("population transfers remain conservative across a chain", "[economy][migration]") {
	auto state = std::make_unique<sys::state>();
	auto source = state->world.create_pop();
	auto middle = state->world.create_pop();
	auto target = state->world.create_pop();
	state->world.pop_set_size(source, 100.f);
	state->world.pop_set_savings(source, 90.f);
	state->world.pop_set_size(middle, 0.f);
	state->world.pop_set_savings(middle, 0.f);
	state->world.pop_set_size(target, 0.f);
	state->world.pop_set_savings(target, 0.f);

	REQUIRE(demographics::transfer_pop_amount(*state, source, middle, 30.f) == Approx(30.f));
	REQUIRE(demographics::transfer_pop_amount(*state, middle, target, 10.f) == Approx(10.f));
	REQUIRE(state->world.pop_get_size(source) == Approx(70.f));
	REQUIRE(state->world.pop_get_size(middle) == Approx(20.f));
	REQUIRE(state->world.pop_get_size(target) == Approx(10.f));
	REQUIRE(state->world.pop_get_savings(source) == Approx(63.f));
	REQUIRE(state->world.pop_get_savings(middle) == Approx(18.f));
	REQUIRE(state->world.pop_get_savings(target) == Approx(9.f));
}

TEST_CASE("population transfer rejects target overflow atomically", "[economy][migration]") {
	auto state = std::make_unique<sys::state>();
	auto source = state->world.create_pop();
	auto target = state->world.create_pop();
	state->world.pop_set_size(source, 1.f);
	state->world.pop_set_savings(source, 1.f);
	state->world.pop_set_size(target, std::numeric_limits<float>::max());
	state->world.pop_set_savings(target, 0.f);

	REQUIRE(demographics::transfer_pop_amount(*state, source, target, 1.f) == Approx(0.f));
	REQUIRE(state->world.pop_get_size(source) == Approx(1.f));
	REQUIRE(state->world.pop_get_savings(source) == Approx(1.f));
	REQUIRE(state->world.pop_get_size(target) == std::numeric_limits<float>::max());

	state->world.pop_set_size(target, 1.f);
	state->world.pop_set_savings(target, std::numeric_limits<float>::max());
	REQUIRE(demographics::transfer_pop_amount(*state, source, target, 1.f) == Approx(0.f));
	REQUIRE(state->world.pop_get_size(source) == Approx(1.f));
	REQUIRE(state->world.pop_get_savings(source) == Approx(1.f));
	REQUIRE(state->world.pop_get_savings(target) == std::numeric_limits<float>::max());
}

TEST_CASE("migration creates a zero-wealth target before transferring a proportional share",
		"[economy][migration][integration]") {
	auto state = std::make_unique<sys::state>();
	auto culture = state->world.create_culture();
	auto religion = state->world.create_religion();
	auto pop_type = state->world.create_pop_type();
	auto origin = state->world.create_province();
	auto destination = state->world.create_province();
	auto source = state->world.create_pop();
	state->world.pop_set_culture(source, culture);
	state->world.pop_set_religion(source, religion);
	state->world.pop_set_poptype(source, pop_type);
	state->world.pop_set_size(source, 100.f);
	state->world.pop_set_savings(source, 40.f);
	state->world.force_create_pop_location(source, origin);

	demographics::migration_buffer buffer;
	buffer.update(state->world.pop_size());
	buffer.amounts.set(source, 25.f);
	buffer.destinations.set(source, destination);
	demographics::apply_internal_migration(*state, 0, 31, buffer);

	dcon::pop_id migrated{};
	for(auto location : state->world.province_get_pop_location(destination)) {
		migrated = location.get_pop();
	}
	REQUIRE(migrated);
	REQUIRE(state->world.pop_get_size(source) == Approx(75.f));
	REQUIRE(state->world.pop_get_savings(source) == Approx(30.f));
	REQUIRE(state->world.pop_get_size(migrated) == Approx(25.f));
	REQUIRE(state->world.pop_get_savings(migrated) == Approx(10.f));
}

TEST_CASE("small POP cleanup merges population and savings instead of destroying them",
		"[economy][migration][cleanup]") {
	auto state = std::make_unique<sys::state>();
	auto province = state->world.create_province();
	auto small = state->world.create_pop();
	auto large = state->world.create_pop();
	state->world.pop_set_size(small, 10.f);
	state->world.pop_set_savings(small, 7.f);
	state->world.pop_set_size(large, 100.f);
	state->world.pop_set_savings(large, 3.f);
	state->world.force_create_pop_location(small, province);
	state->world.force_create_pop_location(large, province);

	demographics::remove_small_pops(*state);

	REQUIRE(state->world.pop_size() == 1);
	float total_size = 0.f;
	float total_savings = 0.f;
	state->world.for_each_pop([&](dcon::pop_id pop) {
		total_size += state->world.pop_get_size(pop);
		total_savings += state->world.pop_get_savings(pop);
	});
	REQUIRE(total_size == Approx(110.f));
	REQUIRE(total_savings == Approx(10.f));
}

TEST_CASE("the final partial SIMD POP receives its staggered update", "[economy][pops][simd]") {
	auto state = std::make_unique<sys::state>();
	auto nation = state->world.create_nation();
	auto province = state->world.create_province();
	auto worker = state->world.create_pop_type();
	state->culture_definitions.slaves = state->world.create_pop_type();
	state->world.province_resize_modifier_values(sys::provincial_mod_offsets::count);
	state->world.nation_resize_modifier_values(sys::national_mod_offsets::count);
	state->world.province_set_nation_from_province_ownership(province, nation);
	state->defines.base_popgrowth = 0.1f;
	state->defines.life_rating_growth_bonus = 0.f;
	state->defines.life_need_starvation_limit = 0.f;
	state->defines.slave_growth_divisor = 1.f;

	std::array<dcon::pop_id, 17> pops{};
	for(auto& pop : pops) {
		pop = state->world.create_pop();
		state->world.pop_set_poptype(pop, worker);
		state->world.pop_set_size(pop, 100.f);
		state->world.force_create_pop_location(pop, province);
	}

	demographics::update_growth(*state, 1, 31);

	REQUIRE(state->world.pop_get_size(pops[15]) == Approx(100.f));
	REQUIRE(state->world.pop_get_size(pops[16]) == Approx(110.f));
}

TEST_CASE("employment and prices can reverse a nominal wage preference", "[economy][migration]") {
	// Old nominal-wage logic prefers the target (2 > 1). Real opportunity does
	// not: only 10% of its labor is sold and its life-needs basket costs twice as much.
	auto origin = demographics::life_needs_coverage_from_components(1.f, 1.f, 400000.f, 400000.f, 0.f);
	auto target = demographics::life_needs_coverage_from_components(2.f, 0.1f, 800000.f, 400000.f, 0.f);
	REQUIRE(target < origin);
	REQUIRE(demographics::migration_opportunity_multiplier_from_coverage(origin, target) < 1.f);
}

TEST_CASE("province migration coverage reads the current labor market and pop basket", "[economy][migration][integration]") {
	auto state = std::make_unique<sys::state>();
	auto pop_type = state->world.create_pop_type();
	auto pop = state->world.create_pop();
	auto origin = state->world.create_province();
	auto target = state->world.create_province();
	auto origin_state = state->world.create_state_instance();
	auto target_state = state->world.create_state_instance();
	auto origin_market = state->world.create_market();
	auto target_market = state->world.create_market();

	state->world.province_set_state_membership(origin, origin_state);
	state->world.province_set_state_membership(target, target_state);
	state->world.state_instance_set_market_from_local_market(origin_state, origin_market);
	state->world.state_instance_set_market_from_local_market(target_state, target_market);
	state->world.pop_set_poptype(pop, pop_type);
	state->world.force_create_pop_location(pop, origin);
	state->culture_definitions.primary_factory_worker = pop_type;

	state->world.market_resize_life_needs_costs(state->world.pop_type_size());
	state->world.province_resize_labor_price(economy::labor::total);
	state->world.province_resize_labor_supply_sold(economy::labor::total);
	state->world.province_resize_pop_labor_distribution(economy::pop_labor::total);
	state->world.province_resize_demographics(demographics::count_special_keys + state->world.pop_type_size() * 2);

	state->world.market_set_life_needs_costs(origin_market, pop_type, 400000.f);
	state->world.market_set_life_needs_costs(target_market, pop_type, 800000.f);
	state->world.province_set_labor_price(origin, economy::labor::no_education, 1.f);
	state->world.province_set_labor_supply_sold(origin, economy::labor::no_education, 1.f);
	state->world.province_set_labor_price(target, economy::labor::no_education, 2.f);
	state->world.province_set_labor_supply_sold(target, economy::labor::no_education, 0.1f);
	state->world.province_set_pop_labor_distribution(origin, economy::pop_labor::primary_no_education, 1.f);
	state->world.province_set_pop_labor_distribution(target, economy::pop_labor::primary_no_education, 1.f);

	auto origin_coverage = demographics::expected_life_needs_coverage(*state, pop, origin);
	auto target_coverage = demographics::expected_life_needs_coverage(*state, pop, target);
	REQUIRE(origin_coverage == Approx(1.f));
	REQUIRE(target_coverage == Approx(0.1f));
	REQUIRE(demographics::real_opportunity_migration_multiplier(
		*state, pop, origin, target) < 1.f);
}

TEST_CASE("prospective income combines every labor tier used by a pop", "[economy][migration][integration]") {
	auto state = std::make_unique<sys::state>();
	auto pop_type = state->world.create_pop_type();
	auto pop = state->world.create_pop();
	auto province = state->world.create_province();
	state->culture_definitions.primary_factory_worker = pop_type;
	state->world.pop_set_poptype(pop, pop_type);

	state->world.province_resize_labor_price(economy::labor::total);
	state->world.province_resize_labor_supply_sold(economy::labor::total);
	state->world.province_resize_pop_labor_distribution(economy::pop_labor::total);
	state->world.province_set_labor_price(province, economy::labor::no_education, 2.f);
	state->world.province_set_labor_supply_sold(province, economy::labor::no_education, 0.5f);
	state->world.province_set_labor_price(province, economy::labor::basic_education, 4.f);
	state->world.province_set_labor_supply_sold(province, economy::labor::basic_education, 0.25f);
	state->world.province_set_pop_labor_distribution(province, economy::pop_labor::primary_no_education, 0.25f);
	state->world.province_set_pop_labor_distribution(province, economy::pop_labor::primary_basic_education, 0.75f);

	// 0.25 * (2 * 0.5) + 0.75 * (4 * 0.25)
	REQUIRE(demographics::expected_labor_income_per_capita(*state, pop, province) == Approx(1.f));
}

TEST_CASE("prospective income uses the target nation's culture acceptance", "[economy][migration][integration]") {
	auto state = std::make_unique<sys::state>();
	auto pop_type = state->world.create_pop_type();
	auto culture = state->world.create_culture();
	auto pop = state->world.create_pop();
	auto origin = state->world.create_province();
	auto target = state->world.create_province();
	auto origin_owner = state->world.create_nation();
	auto target_owner = state->world.create_nation();
	state->culture_definitions.secondary_factory_worker = pop_type;
	state->world.pop_set_poptype(pop, pop_type);
	state->world.pop_set_culture(pop, culture);
	state->world.province_set_nation_from_province_ownership(origin, origin_owner);
	state->world.province_set_nation_from_province_ownership(target, target_owner);
	state->world.nation_resize_accepted_cultures(state->world.culture_size());
	state->world.nation_set_accepted_cultures(target_owner, culture, true);

	state->world.province_resize_labor_price(economy::labor::total);
	state->world.province_resize_labor_supply_sold(economy::labor::total);
	state->world.province_resize_pop_labor_distribution(economy::pop_labor::total);
	for(auto province : { origin, target }) {
		state->world.province_set_labor_price(province, economy::labor::high_education, 1.f);
		state->world.province_set_labor_supply_sold(province, economy::labor::high_education, 1.f);
		state->world.province_set_labor_price(province, economy::labor::high_education_and_accepted, 10.f);
		state->world.province_set_labor_supply_sold(province, economy::labor::high_education_and_accepted, 1.f);
		state->world.province_set_pop_labor_distribution(
			province, economy::pop_labor::high_education_not_accepted_high_education, 1.f);
		state->world.province_set_pop_labor_distribution(
			province, economy::pop_labor::high_education_accepted_high_education_accepted, 1.f);
	}

	REQUIRE(demographics::expected_labor_income_per_capita(*state, pop, origin) == Approx(1.f));
	REQUIRE(demographics::expected_labor_income_per_capita(*state, pop, target) == Approx(10.f));
}

TEST_CASE("non-labor pops get a neutral real-opportunity adjustment", "[economy][migration][integration]") {
	auto state = std::make_unique<sys::state>();
	auto pop_type = state->world.create_pop_type();
	auto pop = state->world.create_pop();
	auto origin = state->world.create_province();
	auto target = state->world.create_province();
	state->world.pop_set_poptype(pop, pop_type);

	REQUIRE_FALSE(demographics::pop_uses_labor_market(*state, pop_type));
	REQUIRE(demographics::real_opportunity_migration_multiplier(
		*state, pop, origin, target) == Approx(1.f));
}

TEST_CASE("tariff execution uses normalized provincial control", "[economy][administration]") {
	auto state = std::make_unique<sys::state>();
	auto nation = state->world.create_nation();
	auto province = state->world.create_province();
	auto state_instance = state->world.create_state_instance();
	auto market = state->world.create_market();
	state->world.nation_resize_modifier_values(sys::national_mod_offsets::count);
	state->world.market_set_zone_from_local_market(market, state_instance);
	state->world.state_instance_set_capital(state_instance, province);
	state->world.province_set_control_scale(province, 1000.f);
	state->world.province_set_control_ratio(province, 0.25f);
	state->defines.base_tariff_efficiency = 0.8f;

	REQUIRE(nations::tariff_efficiency(*state, nation, market) == Approx(0.2f));
}
