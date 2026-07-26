#include "gamestate/simulation_runner.hpp"

#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace {

struct simulation_diagnostics_fixture {
	std::unique_ptr<sys::state> state;
	dcon::pop_id pop;
	dcon::province_id province;
	dcon::market_id market;
	dcon::commodity_id commodity;
	dcon::factory_id factory;
	dcon::army_id army;
	dcon::nation_id nation;
};

simulation_diagnostics_fixture make_simulation_diagnostics_fixture() {
	simulation_diagnostics_fixture fixture{};
	fixture.state = std::make_unique<sys::state>();
	fixture.pop = fixture.state->world.create_pop();
	fixture.province = fixture.state->world.create_province();
	fixture.market = fixture.state->world.create_market();
	fixture.commodity = fixture.state->world.create_commodity();
	fixture.factory = fixture.state->world.create_factory();
	fixture.army = fixture.state->world.create_army();
	fixture.nation = fixture.state->world.create_nation();

	fixture.state->world.market_resize_price(fixture.state->world.commodity_size());
	fixture.state->world.market_resize_supply(fixture.state->world.commodity_size());
	fixture.state->world.market_resize_demand(fixture.state->world.commodity_size());
	fixture.state->world.province_resize_labor_price(economy::labor::total);
	fixture.state->world.province_resize_labor_supply(economy::labor::total);
	fixture.state->world.province_resize_labor_demand(economy::labor::total);
	fixture.state->world.province_resize_labor_demand_satisfaction(economy::labor::total);
	fixture.state->world.province_resize_labor_supply_sold(economy::labor::total);
	fixture.state->world.nation_resize_stockpiles(fixture.state->world.commodity_size());
	fixture.state->world.nation_resize_modifier_values(sys::national_mod_offsets::count);

	fixture.state->inflation = 1.25f;
	fixture.state->world.pop_set_size(fixture.pop, 150.0f);
	fixture.state->world.pop_set_savings(fixture.pop, 25.0f);
	fixture.state->world.pop_set_satisfaction(fixture.pop, 0.5f);
	pop_demographics::set_employment(*fixture.state, fixture.pop, 120.0f);
	fixture.state->world.market_set_price(fixture.market, fixture.commodity, 3.0f);
	fixture.state->world.market_set_supply(fixture.market, fixture.commodity, 20.0f);
	fixture.state->world.market_set_demand(fixture.market, fixture.commodity, 15.0f);
	fixture.state->world.market_set_gdp(fixture.market, 35.0f);
	fixture.state->world.factory_set_profit(fixture.factory, -4.0f);
	fixture.state->world.factory_set_unprofitable(fixture.factory, true);
	fixture.state->world.province_set_labor_price(
		fixture.province, economy::labor::no_education, 2.0f);
	fixture.state->world.province_set_labor_supply(
		fixture.province, economy::labor::no_education, 100.0f);
	fixture.state->world.province_set_labor_demand(
		fixture.province, economy::labor::no_education, 80.0f);
	fixture.state->world.province_set_labor_demand_satisfaction(
		fixture.province, economy::labor::no_education, 0.75f);
	fixture.state->world.province_set_labor_supply_sold(
		fixture.province, economy::labor::no_education, 0.5f);
	fixture.state->world.army_set_supply_reserve(fixture.army, 0.6f);
	fixture.state->world.province_set_is_supply_depot(fixture.province, true);
	fixture.state->world.province_set_supply_depot_stockpile(fixture.province, 9.0f);
	fixture.state->world.province_set_nation_from_province_ownership(fixture.province, fixture.nation);
	fixture.state->world.province_set_control_ratio(fixture.province, 0.7f);
	fixture.state->world.nation_set_stockpiles(fixture.nation, economy::money, 10.0f);
	fixture.state->world.nation_set_local_loan(fixture.nation, 5.0f);
	fixture.state->world.nation_set_national_bank(fixture.nation, 8.0f);
	fixture.state->world.nation_set_private_investment(fixture.nation, 3.0f);
	fixture.state->defines.loan_base_interest = 0.03f;
	fixture.state->force_age_of_transformation_ruleset = true;
	fixture.state->transformation_politics_cache.assign(
		fixture.state->world.nation_size(), politics::transformation::nation_result{});
	auto& political = fixture.state->transformation_politics_cache[fixture.nation.index()];
	political.enabled = true;
	political.legitimacy.total = 60.0f;
	political.coalition.power_share = 0.55f;
	fixture.state->transformation_politics_cache_valid = true;

	return fixture;
}

} // namespace

TEST_CASE("simulation diagnostics collect stable economy labor and logistics aggregates",
		"[simulation][diagnostics]") {
	auto fixture = make_simulation_diagnostics_fixture();
	auto snapshot = sys::simulation::collect_snapshot(*fixture.state, 42);
	auto validation = sys::simulation::validate_snapshot(snapshot);
	auto line = sys::simulation::serialize_jsonl(snapshot, validation);
	auto repeated_snapshot = sys::simulation::collect_snapshot(*fixture.state, 42);

	REQUIRE(validation.valid);
	REQUIRE(validation.violations.total() == 0);
	REQUIRE(snapshot.tick == 42);
	REQUIRE(snapshot.pop_count == 1);
	REQUIRE(snapshot.province_count == 1);
	REQUIRE(snapshot.market_count == 1);
	REQUIRE(snapshot.commodity_market_cells == 1);
	REQUIRE(snapshot.labor_market_cells == economy::labor::total);
	REQUIRE(snapshot.army_count == 1);
	REQUIRE(snapshot.depot_count == 1);
	REQUIRE(snapshot.nation_count == 1);
	REQUIRE(snapshot.owned_province_count == 1);
	REQUIRE(snapshot.transformed_nation_count == 1);
	REQUIRE(snapshot.age_of_transformation);
	REQUIRE(snapshot.inflation == Approx(1.25));
	REQUIRE(snapshot.population == Approx(150.0));
	REQUIRE(snapshot.pop_savings == Approx(25.0));
	REQUIRE(snapshot.population_weighted_life_needs == Approx(150.0));
	REQUIRE(snapshot.population_weighted_everyday_needs == Approx(75.0));
	REQUIRE(snapshot.population_weighted_luxury_needs == Approx(0.0));
	REQUIRE(snapshot.unemployed_population == Approx(30.0));
	REQUIRE(snapshot.market_gdp == Approx(35.0));
	REQUIRE(snapshot.factory_count == 1);
	REQUIRE(snapshot.unprofitable_factory_count == 1);
	REQUIRE(snapshot.factory_profit == Approx(-4.0));
	REQUIRE(snapshot.commodity_price_sum == Approx(3.0));
	REQUIRE(snapshot.commodity_supply == Approx(20.0));
	REQUIRE(snapshot.commodity_demand == Approx(15.0));
	REQUIRE(snapshot.labor_price_sum == Approx(2.0));
	REQUIRE(snapshot.labor_supply == Approx(100.0));
	REQUIRE(snapshot.labor_demand == Approx(80.0));
	REQUIRE(snapshot.employed_labor == Approx(50.0));
	REQUIRE(snapshot.army_supply_reserve_sum == Approx(0.6));
	REQUIRE(snapshot.minimum_army_supply_reserve == Approx(0.6));
	REQUIRE(snapshot.depot_stockpile == Approx(9.0));
	REQUIRE(snapshot.treasury == Approx(10.0));
	REQUIRE(snapshot.government_debt == Approx(5.0));
	REQUIRE(snapshot.national_bank == Approx(8.0));
	REQUIRE(snapshot.private_investment == Approx(3.0));
	REQUIRE(snapshot.control_ratio_sum == Approx(0.7));
	REQUIRE(snapshot.minimum_control_ratio == Approx(0.7));
	REQUIRE(snapshot.legitimacy_sum == Approx(60.0));
	REQUIRE(snapshot.minimum_legitimacy == Approx(60.0));
	REQUIRE(snapshot.coalition_power_sum == Approx(0.55));
	REQUIRE(snapshot.banking_health_sum >= 0.0);
	REQUIRE(snapshot.banking_health_sum <= 1.0);
	for(uint32_t index = 0; index < sys::checksum_key::key_size; ++index) {
		REQUIRE(snapshot.save_checksum.key[index] == repeated_snapshot.save_checksum.key[index]);
	}
	auto const checksum_field = line.find("\"save_checksum\":\"");
	REQUIRE(checksum_field != std::string::npos);
	REQUIRE(line.find('"', checksum_field + 17) == checksum_field + 17 + 64);
	REQUIRE(line.find("\"age_of_transformation\":true") != std::string::npos);
	REQUIRE(line.find("\"finance\":") != std::string::npos);
	REQUIRE(line.find("\"administration\":") != std::string::npos);
	REQUIRE(line.find("\"politics\":") != std::string::npos);
	REQUIRE(line.find("\"banking\":") != std::string::npos);
	REQUIRE(line.find("\"demography\":") != std::string::npos);
	REQUIRE(line.find("\"living_standards\":") != std::string::npos);
	REQUIRE(line.find("\"trade\":") != std::string::npos);
	REQUIRE(line.find("\"crisis\":") != std::string::npos);
}

TEST_CASE("simulation diagnostics classify invalid values without emitting invalid JSON",
		"[simulation][diagnostics]") {
	auto fixture = make_simulation_diagnostics_fixture();
	fixture.state->world.pop_set_savings(fixture.pop, std::numeric_limits<float>::quiet_NaN());
	fixture.state->world.market_set_price(
		fixture.market, fixture.commodity, std::numeric_limits<float>::infinity());
	fixture.state->world.province_set_labor_supply_sold(
		fixture.province, economy::labor::no_education, 1.25f);
	fixture.state->world.army_set_supply_reserve(fixture.army, -0.1f);

	auto snapshot = sys::simulation::collect_snapshot(*fixture.state, 7);
	auto validation = sys::simulation::validate_snapshot(snapshot);
	auto line = sys::simulation::serialize_jsonl(snapshot, validation);

	REQUIRE_FALSE(validation.valid);
	REQUIRE(validation.violations.nonfinite == 2);
	REQUIRE(validation.violations.negative == 1);
	REQUIRE(validation.violations.out_of_range == 1);
	REQUIRE(validation.violations.total() == 4);
	REQUIRE(validation.violations.first.field == sys::simulation::invariant_field::pop_savings);
	REQUIRE(line.find("\"valid\":false") != std::string::npos);
	REQUIRE(line.find("\"field\":\"pop_savings\"") != std::string::npos);
	REQUIRE(line.find(":nan") == std::string::npos);
	REQUIRE(line.find(":inf") == std::string::npos);
	REQUIRE(line.find("NaN") == std::string::npos);
	REQUIRE(line.find("Infinity") == std::string::npos);
	REQUIRE(line.back() == '\n');
}

TEST_CASE("simulation runner honors cadence final snapshot and synchronous tick count",
		"[simulation][runner]") {
	auto fixture = make_simulation_diagnostics_fixture();
	sys::simulation::run_options options{};
	options.ticks = 5;
	options.snapshot_cadence = 2;
	options.include_initial_snapshot = true;
	options.include_final_snapshot = true;
	options.fail_on_invariant = true;

	uint64_t callback_count = 0;
	std::vector<std::string> lines;
	auto result = sys::simulation::detail::run_ticks_with(
		*fixture.state,
		options,
		[&](sys::state& state) {
			++callback_count;
			state.current_date += 1;
		},
		[&](std::string_view line) { lines.emplace_back(line); }
	);

	REQUIRE(result.completed);
	REQUIRE(result.ticks_completed == 5);
	REQUIRE(result.snapshots_emitted == 4);
	REQUIRE(callback_count == 5);
	REQUIRE(lines.size() == 4);
	REQUIRE(lines[0].find("\"tick\":0") != std::string::npos);
	REQUIRE(lines[1].find("\"tick\":2") != std::string::npos);
	REQUIRE(lines[2].find("\"tick\":4") != std::string::npos);
	REQUIRE(lines[3].find("\"tick\":5") != std::string::npos);
}

TEST_CASE("simulation runner stops on an invariant violation at an observed tick",
		"[simulation][runner]") {
	auto fixture = make_simulation_diagnostics_fixture();
	sys::simulation::run_options options{};
	options.ticks = 10;
	options.snapshot_cadence = 1;
	options.fail_on_invariant = true;

	uint64_t callback_count = 0;
	auto result = sys::simulation::detail::run_ticks_with(
		*fixture.state,
		options,
		[&](sys::state& state) {
			++callback_count;
			if(callback_count == 2) {
				state.world.pop_set_savings(fixture.pop, std::numeric_limits<float>::quiet_NaN());
			}
		},
		[](std::string_view) {}
	);

	REQUIRE_FALSE(result.completed);
	REQUIRE(result.ticks_completed == 2);
	REQUIRE(result.snapshots_emitted == 3);
	REQUIRE(callback_count == 2);
	REQUIRE(result.last_validation.violations.first.field ==
		sys::simulation::invariant_field::pop_savings);
}
