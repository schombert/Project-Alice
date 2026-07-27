#include "economy/land_ownership.hpp"
#include "demographics.hpp"

#include <limits>

TEST_CASE("land ownership claims form a bounded distribution",
		"[economy][ownership]") {
	auto const result =
		economy::land_ownership::target_from_claims(60.f, 30.f, 10.f);
	CHECK(result.smallholders == Approx(0.60f));
	CHECK(result.landed_elites == Approx(0.30f));
	CHECK(result.capitalists == Approx(0.10f));
	CHECK(result.smallholders + result.landed_elites + result.capitalists
		== Approx(1.f));
}

TEST_CASE("existing land ownership changes gradually",
		"[economy][ownership]") {
	economy::land_ownership::distribution current{0.80f, 0.10f};
	economy::land_ownership::distribution target{0.20f, 0.50f};
	auto const next =
		economy::land_ownership::advance(current, target, 0.01f);

	CHECK(next.landed_elites == Approx(0.794f));
	CHECK(next.capitalists == Approx(0.104f));
	CHECK(next.smallholders == Approx(0.102f));
	CHECK(next.landed_elites > target.landed_elites);
	CHECK(next.capitalists < target.capitalists);
}

TEST_CASE("empty legacy ownership initializes on its first observation",
		"[economy][ownership]") {
	auto const initialized = economy::land_ownership::advance(
		{}, {0.35f, 0.15f});
	CHECK(initialized.landed_elites == Approx(0.35f));
	CHECK(initialized.capitalists == Approx(0.15f));
	CHECK(initialized.smallholders == Approx(0.50f));
}

TEST_CASE("country tags select distinct historical land regimes",
		"[economy][ownership][history]") {
	auto const tag = [](char a, char b, char c) {
		return (uint32_t(uint8_t(a)) << 16)
			| (uint32_t(uint8_t(b)) << 8) | uint32_t(uint8_t(c));
	};
	using profile = economy::land_ownership::historical_profile;
	CHECK(economy::land_ownership::profile_for_tag(tag('P', 'E', 'R'))
		== profile::persian_estates);
	CHECK(economy::land_ownership::profile_for_tag(tag('R', 'U', 'S'))
		== profile::russian_communal);
	CHECK(economy::land_ownership::profile_for_tag(tag('T', 'U', 'R'))
		== profile::ottoman_state_tenure);
	CHECK(economy::land_ownership::profile_for_tag(tag('U', 'S', 'A'))
		== profile::american_family_farms);
	CHECK(economy::land_ownership::profile_for_tag(tag('B', 'R', 'A'))
		== profile::latin_latifundia);
	CHECK(economy::land_ownership::profile_for_tag(tag('J', 'A', 'P'))
		== profile::demographic);
}

TEST_CASE("historical profiles create different normalized starting stocks",
		"[economy][ownership][history]") {
	using namespace economy::land_ownership;
	distribution const local_claims{0.20f, 0.10f, 0.70f, 0.f, 0.f};
	auto const persian = historical_initial_distribution(
		historical_profile::persian_estates, local_claims);
	auto const russian = historical_initial_distribution(
		historical_profile::russian_communal, local_claims);
	auto const ottoman = historical_initial_distribution(
		historical_profile::ottoman_state_tenure, local_claims);
	auto const american = historical_initial_distribution(
		historical_profile::american_family_farms, local_claims);
	auto const latin = historical_initial_distribution(
		historical_profile::latin_latifundia, local_claims);

	auto const total = [](distribution const& value) {
		return value.landed_elites + value.capitalists
			+ value.smallholders + value.state + value.foreign;
	};
	CHECK(total(persian) == Approx(1.f));
	CHECK(total(russian) == Approx(1.f));
	CHECK(total(ottoman) == Approx(1.f));
	CHECK(total(american) == Approx(1.f));
	CHECK(total(latin) == Approx(1.f));
	CHECK(persian.landed_elites > persian.smallholders);
	CHECK(russian.smallholders > russian.landed_elites);
	CHECK(ottoman.state > 0.20f);
	CHECK(american.smallholders > 0.60f);
	CHECK(latin.landed_elites > 0.50f);
}

TEST_CASE("slave intensity creates a plantation belt inside the US profile",
		"[economy][ownership][history]") {
	using namespace economy::land_ownership;
	distribution const local_claims{0.15f, 0.10f, 0.75f, 0.f, 0.f};
	auto const free_farming = historical_initial_distribution(
		historical_profile::american_family_farms, local_claims, 0.f);
	auto const plantation = historical_initial_distribution(
		historical_profile::american_family_farms, local_claims, 0.8f);
	CHECK(plantation.landed_elites > free_farming.landed_elites);
	CHECK(plantation.smallholders < free_farming.smallholders);
	CHECK(plantation.landed_elites + plantation.capitalists
		+ plantation.smallholders + plantation.state + plantation.foreign
		== Approx(1.f));
}

TEST_CASE("historical profile initializes a province exactly once",
		"[economy][ownership][history][integration]") {
	auto state = std::make_unique<sys::state>();
	state->force_age_of_transformation_ruleset = true;
	auto const farmers = state->world.create_pop_type();
	auto const laborers = state->world.create_pop_type();
	auto const aristocrats = state->world.create_pop_type();
	auto const capitalists = state->world.create_pop_type();
	auto const slaves = state->world.create_pop_type();
	state->culture_definitions.farmers = farmers;
	state->culture_definitions.laborers = laborers;
	state->culture_definitions.aristocrat = aristocrats;
	state->culture_definitions.capitalists = capitalists;
	state->culture_definitions.slaves = slaves;

	auto const nation = state->world.create_nation();
	auto const identity = state->world.create_national_identity();
	state->world.national_identity_set_identifying_int(identity,
		(uint32_t('P') << 16) | (uint32_t('E') << 8) | uint32_t('R'));
	state->world.force_create_identity_holder(nation, identity);
	auto const province = state->world.create_province();
	state->world.province_set_nation_from_province_ownership(
		province, nation);
	state->province_definitions.first_sea_province =
		dcon::province_id{dcon::province_id::value_base_t(1)};
	state->world.province_resize_demographics(demographics::size(*state));
	state->world.province_set_demographics(province,
		demographics::to_key(*state, farmers), 10'000.f);
	state->world.province_set_demographics(province,
		demographics::to_key(*state, aristocrats), 100.f);

	economy::land_ownership::initialize_historical_profiles(*state);
	CHECK(state->world.province_get_land_profile(province)
		== uint8_t(economy::land_ownership::historical_profile::persian_estates));
	CHECK(state->world.province_get_landowners_share(province) > 0.50f);
	CHECK(state->world.province_get_state_land_share(province) > 0.10f);
	auto const first_landed =
		state->world.province_get_landowners_share(province);

	state->world.province_set_demographics(province,
		demographics::to_key(*state, aristocrats), 100'000.f);
	economy::land_ownership::initialize_historical_profiles(*state);
	CHECK(state->world.province_get_landowners_share(province)
		== Approx(first_landed));

	state->world.province_set_land_profile(province, 0);
	state->world.province_set_landowners_share(province, 0.30f);
	state->world.province_set_capitalists_share(province, 0.10f);
	state->world.province_set_state_land_share(province, 0.f);
	state->world.province_set_foreign_land_share(province, 0.f);
	state->current_date = sys::date{100};
	economy::land_ownership::initialize_historical_profiles(*state);
	CHECK(state->world.province_get_landowners_share(province)
		== Approx(0.30f));
	CHECK(state->world.province_get_capitalists_share(province)
		== Approx(0.10f));
}

TEST_CASE("land ownership sanitizes invalid inputs",
		"[economy][ownership]") {
	auto const nan = std::numeric_limits<float>::quiet_NaN();
	auto const infinity = std::numeric_limits<float>::infinity();
	auto const result = economy::land_ownership::advance(
		{nan, infinity}, {2.f, 2.f}, nan);

	CHECK(std::isfinite(result.landed_elites));
	CHECK(std::isfinite(result.capitalists));
	CHECK(std::isfinite(result.smallholders));
	CHECK(result.landed_elites >= 0.f);
	CHECK(result.capitalists >= 0.f);
	CHECK(result.smallholders >= 0.f);
	CHECK(result.landed_elites + result.capitalists + result.smallholders
		== Approx(1.f));
}

TEST_CASE("land market purchases conserve cash and ownership",
		"[economy][ownership][market]") {
	economy::land_ownership::market_config config;
	config.enabled = true;
	config.maximum_monthly_turnover = 1.f;
	config.voluntary_ask_rate = 0.10f;
	auto const result = economy::land_ownership::clear_market(
		{0.70f, 0.10f},
		std::array<float,
			economy::land_ownership::owner_group_count>{
				100.f, 50.f, 1'000.f},
		1'000.f,
		config);

	CHECK(result.enabled);
	CHECK(result.turnover > 0.f);
	CHECK(result.after.capitalists > result.before.capitalists);
	CHECK(result.cash_delta[std::size_t(
		economy::land_ownership::owner_group::capitalists)] < 0.f);
	auto const cash_sum = result.cash_delta[0]
		+ result.cash_delta[1] + result.cash_delta[2];
	CHECK(cash_sum == Approx(0.f).margin(0.0001f));
	CHECK(result.after.smallholders + result.after.landed_elites
		+ result.after.capitalists == Approx(1.f));
}

TEST_CASE("land market cannot spend more than committed savings",
		"[economy][ownership][market]") {
	economy::land_ownership::market_config config;
	config.enabled = true;
	config.maximum_monthly_turnover = 1.f;
	config.voluntary_ask_rate = 1.f;
	std::array<economy::land_ownership::group_finance,
		economy::land_ownership::owner_group_count> finances{};
	finances[std::size_t(
		economy::land_ownership::owner_group::capitalists)] =
			{10.f, 1.5f, 0.f};
	auto const result = economy::land_ownership::clear_market(
		{0.90f, 0.f},
		finances,
		10'000.f,
		config);
	auto const capitalist_payment = -result.cash_delta[std::size_t(
		economy::land_ownership::owner_group::capitalists)];

	CHECK(capitalist_payment <= 1.f + 0.0001f);
	CHECK(result.turnover <= 0.0001f + 0.000001f);
}

TEST_CASE("nine month rent signal damps a one day shock",
		"[economy][ownership][price]") {
	auto const next = economy::land_ownership::update_smoothed_rent(
		100.f, 370.f, 270.f);
	CHECK(next == Approx(101.f));
	CHECK(next < 370.f);
}

TEST_CASE("cash below the essential-needs reserve cannot bid",
		"[economy][ownership][reserve]") {
	economy::land_ownership::market_config config;
	config.enabled = true;
	config.maximum_monthly_turnover = 1.f;
	config.voluntary_ask_rate = 1.f;
	std::array<economy::land_ownership::group_finance,
		economy::land_ownership::owner_group_count> finances{};
	finances[2] = {600.f, 100.f, 0.f};
	auto const result = economy::land_ownership::clear_market(
		{0.8f, 0.1f}, finances, 100.f, config);
	CHECK(result.bids[2] == Approx(0.f));
	CHECK(result.turnover == Approx(0.f));
}

TEST_CASE("hardship lists land while tenant protection reduces the sale",
		"[economy][ownership][distress]") {
	std::array<economy::land_ownership::group_finance,
		economy::land_ownership::owner_group_count> finances{};
	finances[0] = {1'000.f, 0.f, 0.f};
	finances[1] = {0.f, 0.f, 1.f};
	economy::land_ownership::market_config exposed;
	exposed.enabled = true;
	exposed.voluntary_ask_rate = 0.f;
	exposed.tenant_protection = 0.f;
	auto protected_config = exposed;
	protected_config.tenant_protection = 0.8f;
	auto const a = economy::land_ownership::clear_market(
		{0.8f, 0.1f}, finances, 100.f, exposed);
	auto const b = economy::land_ownership::clear_market(
		{0.8f, 0.1f}, finances, 100.f, protected_config);
	CHECK(a.asks[1] > b.asks[1]);
	CHECK(a.turnover > b.turnover);
}

TEST_CASE("land use distinguishes owner tenants and landless workers",
		"[economy][ownership][tenancy]") {
	auto const use = economy::land_ownership::classify_land_use(
		1'000.f, 0.4f, 0.5f);
	CHECK(use.smallholders == Approx(0.4f));
	CHECK(use.tenants == Approx(0.3f));
	CHECK(use.landless_laborers == Approx(0.3f));
}

TEST_CASE("nationalization and privatization move the state share",
		"[economy][ownership][law]") {
	std::array<economy::land_ownership::group_finance,
		economy::land_ownership::owner_group_count> finances{};
	finances[std::size_t(
		economy::land_ownership::owner_group::state)].liquid_savings =
			1'000.f;
	economy::land_ownership::market_config nationalize;
	nationalize.enabled = true;
	nationalize.maximum_monthly_turnover = 1.f;
	nationalize.nationalization_rate = 0.1f;
	auto const public_result = economy::land_ownership::clear_market(
		{0.5f, 0.2f}, finances, 100.f, nationalize);
	CHECK(public_result.after.state > 0.f);

	auto privatize = nationalize;
	privatize.nationalization_rate = 0.f;
	privatize.privatization_rate = 0.1f;
	finances[2].liquid_savings = 1'000.f;
	auto const private_result = economy::land_ownership::clear_market(
		public_result.after, finances, 100.f, privatize);
	CHECK(private_result.after.state < public_result.after.state);
	CHECK(private_result.after.capitalists
		> public_result.after.capitalists);
}

TEST_CASE("foreign ownership is liquidated when investment is forbidden",
		"[economy][ownership][foreign][law]") {
	std::array<economy::land_ownership::group_finance,
		economy::land_ownership::owner_group_count> finances{};
	finances[0].liquid_savings = 1'000.f;
	economy::land_ownership::market_config config;
	config.enabled = true;
	config.maximum_monthly_turnover = 1.f;
	config.foreign_investment_allowed = false;
	auto const result = economy::land_ownership::clear_market(
		{0.2f, 0.1f, 0.5f, 0.f, 0.2f},
		finances, 100.f, config);
	CHECK(result.after.foreign < result.before.foreign);
	CHECK(result.bids[4] == Approx(0.f));
}

TEST_CASE("disabled land market is an exact no-op",
		"[economy][ownership][market]") {
	auto const result = economy::land_ownership::clear_market(
		{0.60f, 0.20f},
		std::array<float,
			economy::land_ownership::owner_group_count>{
				1'000.f, 1'000.f, 1'000.f},
		100.f, {});
	CHECK_FALSE(result.enabled);
	CHECK(result.after.landed_elites == Approx(0.60f));
	CHECK(result.after.capitalists == Approx(0.20f));
	CHECK(result.after.smallholders == Approx(0.20f));
	CHECK(result.turnover == 0.f);
	CHECK(result.cash_delta[0] == 0.f);
	CHECK(result.cash_delta[1] == 0.f);
	CHECK(result.cash_delta[2] == 0.f);
}

TEST_CASE("province land market transfers real POP savings",
		"[economy][ownership][market][integration]") {
	auto state = std::make_unique<sys::state>();
	state->force_age_of_transformation_ruleset = true;
	state->start_date =
		sys::absolute_time_point(sys::year_month_day{1836, 1, 1});
	state->current_date = sys::date{0};

	auto const farmers_type = state->world.create_pop_type();
	auto const aristocrat_type = state->world.create_pop_type();
	auto const capitalist_type = state->world.create_pop_type();
	state->culture_definitions.farmers = farmers_type;
	state->culture_definitions.aristocrat = aristocrat_type;
	state->culture_definitions.capitalists = capitalist_type;

	auto const nation = state->world.create_nation();
	state->world.nation_resize_modifier_values(
		sys::national_mod_offsets::count);
	auto const province = state->world.create_province();
	state->province_definitions.first_sea_province =
		dcon::province_id{dcon::province_id::value_base_t(1)};
	state->world.province_set_nation_from_province_ownership(
		province, nation);
	state->world.province_set_landowners_share(province, 0.80f);
	state->world.province_set_capitalists_share(province, 0.10f);
	state->world.province_set_rgo_bank(province, 0.f);

	auto make_pop = [&](dcon::pop_type_id type, float savings) {
		auto const pop = state->world.create_pop();
		state->world.pop_set_poptype(pop, type);
		state->world.pop_set_size(pop, 1000.f);
		state->world.pop_set_savings(pop, savings);
		state->world.force_create_pop_location(pop, province);
		return pop;
	};
	auto const farmers = make_pop(farmers_type, 100.f);
	auto const aristocrats = make_pop(aristocrat_type, 100.f);
	auto const capitalists = make_pop(capitalist_type, 10'000.f);
	auto const money_before =
		state->world.pop_get_savings(farmers)
		+ state->world.pop_get_savings(aristocrats)
		+ state->world.pop_get_savings(capitalists);

	economy::land_ownership::update_markets(*state);

	auto const money_after =
		state->world.pop_get_savings(farmers)
		+ state->world.pop_get_savings(aristocrats)
		+ state->world.pop_get_savings(capitalists);
	CHECK(money_after == Approx(money_before).margin(0.001f));
	CHECK(state->world.pop_get_savings(capitalists) < 10'000.f);
	CHECK(state->world.pop_get_savings(farmers) > 100.f);
	CHECK(state->world.pop_get_savings(aristocrats) > 100.f);
	CHECK(state->world.province_get_capitalists_share(province) > 0.10f);
	CHECK(state->world.province_get_landowners_share(province) < 0.80f);
}
