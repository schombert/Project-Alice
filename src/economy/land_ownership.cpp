#include "land_ownership.hpp"

#include "economy_pops_constants.hpp"
#include "demographics.hpp"
#include "gamerule.hpp"
#include "money.hpp"
#include "nations.hpp"
#include "province_templates.hpp"
#include "system_state.hpp"

#include <array>
#include <cmath>
#include <vector>

namespace economy::land_ownership {
namespace {

constexpr std::size_t index(owner_group group) {
	return std::size_t(group);
}

std::array<float, owner_group_count> shares(distribution value) {
	return {
		value.smallholders,
		value.landed_elites,
		value.capitalists,
		value.state,
		value.foreign,
	};
}

distribution from_shares(std::array<float, owner_group_count> const& value) {
	return normalize({
		value[index(owner_group::landed_elites)],
		value[index(owner_group::capitalists)],
		value[index(owner_group::smallholders)],
		value[index(owner_group::state)],
		value[index(owner_group::foreign)],
	});
}

owner_group group_for_pop(sys::state const& state, dcon::pop_id pop) {
	auto const type = state.world.pop_get_poptype(pop);
	if(type == state.culture_definitions.aristocrat)
		return owner_group::landed_elites;
	if(type == state.culture_definitions.capitalists)
		return owner_group::capitalists;
	return owner_group::smallholders;
}

bool participates_as_smallholder(sys::state const& state, dcon::pop_id pop) {
	auto const type = state.world.pop_get_poptype(pop);
	return type == state.culture_definitions.farmers
		|| type == state.culture_definitions.laborers;
}

bool participates(sys::state const& state, dcon::pop_id pop) {
	auto const group = group_for_pop(state, pop);
	return group != owner_group::smallholders
		|| participates_as_smallholder(state, pop);
}

void apply_cash_delta(sys::state& state, dcon::province_id province,
		owner_group group, float cash_delta, float group_savings,
		float group_population) {
	if(!std::isfinite(cash_delta) || std::abs(cash_delta) <= 0.000001f)
		return;

	std::vector<dcon::pop_id> recipients;
	for(auto location : state.world.province_get_pop_location(province)) {
		auto const pop = location.get_pop().id;
		if(participates(state, pop) && group_for_pop(state, pop) == group)
			recipients.push_back(pop);
	}

	if(recipients.empty()) {
		if(cash_delta > 0.f) {
			auto const nation =
				state.world.province_get_nation_from_province_ownership(province);
			if(nation) {
				state.world.nation_set_national_bank(nation,
					finite_nonnegative(state.world.nation_get_national_bank(nation))
						+ cash_delta);
			}
		}
		return;
	}

	auto remaining = std::abs(cash_delta);
	for(std::size_t i = 0; i < recipients.size(); ++i) {
		auto const pop = recipients[i];
		auto const current =
			finite_nonnegative(state.world.pop_get_savings(pop));
		float weight = 0.f;
		if(cash_delta < 0.f && group_savings > 0.f)
			weight = current / group_savings;
		else if(cash_delta > 0.f && group_savings > 0.f)
			// Existing savings are the best already-persisted proxy for each
			// POP's share of its class holding. Seller proceeds therefore do
			// not get redistributed merely because a POP is numerous.
			weight = current / group_savings;
		else if(cash_delta > 0.f)
			weight = 1.f / float(recipients.size());
		auto amount = i + 1 == recipients.size()
			? remaining
			: std::min(remaining, std::abs(cash_delta) * weight);
		if(cash_delta < 0.f)
			amount = std::min(amount, current);
		state.world.pop_set_savings(pop,
			cash_delta < 0.f ? current - amount : current + amount);
		remaining = std::max(0.f, remaining - amount);
	}
}

void apply_state_cash_delta(sys::state& state, dcon::nation_id nation,
		float cash_delta) {
	if(!nation || !std::isfinite(cash_delta))
		return;
	auto const current =
		finite_nonnegative(state.world.nation_get_national_bank(nation));
	state.world.nation_set_national_bank(nation,
		std::max(0.f, current + cash_delta));
}

void apply_treasury_cost(sys::state& state, dcon::nation_id nation,
		float cost) {
	if(!nation || state.world.commodity_size() == 0
			|| !std::isfinite(cost) || cost <= 0.f)
		return;
	auto const treasury = finite_nonnegative(
		state.world.nation_get_stockpiles(nation, economy::money));
	state.world.nation_set_stockpiles(nation, economy::money,
		std::max(0.f, treasury - cost));
}

void apply_foreign_cash_delta(sys::state& state, dcon::nation_id target,
		float cash_delta) {
	if(!target || !std::isfinite(cash_delta))
		return;
	float total_investment = 0.f;
	for(auto relation :
			state.world.nation_get_unilateral_relationship_as_target(target)) {
		total_investment +=
			finite_nonnegative(relation.get_foreign_investment());
	}
	if(total_investment <= 0.f) {
		apply_state_cash_delta(state, target, cash_delta);
		return;
	}
	for(auto relation :
			state.world.nation_get_unilateral_relationship_as_target(target)) {
		auto const weight =
			finite_nonnegative(relation.get_foreign_investment())
			/ total_investment;
		auto const investor = relation.get_source().id;
		auto const current =
			finite_nonnegative(state.world.nation_get_national_bank(investor));
		state.world.nation_set_national_bank(investor,
			std::max(0.f, current + cash_delta * weight));
	}
}

} // namespace

distribution normalize(distribution value) {
	std::array<float, owner_group_count> values{
		finite_nonnegative(value.smallholders),
		finite_nonnegative(value.landed_elites),
		finite_nonnegative(value.capitalists),
		finite_nonnegative(value.state),
		finite_nonnegative(value.foreign),
	};
	float total = 0.f;
	for(auto const item : values)
		total += item;
	if(total <= 0.f)
		return {};
	for(auto& item : values)
		item /= total;
	distribution result;
	result.smallholders = values[index(owner_group::smallholders)];
	result.landed_elites = values[index(owner_group::landed_elites)];
	result.capitalists = values[index(owner_group::capitalists)];
	result.state = values[index(owner_group::state)];
	result.foreign = values[index(owner_group::foreign)];
	return result;
}

namespace {

constexpr uint32_t country_tag(char first, char second, char third) {
	return (uint32_t(uint8_t(first)) << 16)
		| (uint32_t(uint8_t(second)) << 8)
		| uint32_t(uint8_t(third));
}

constexpr bool is_latin_latifundia_tag(uint32_t tag) {
	constexpr std::array<uint32_t, 16> tags{
		country_tag('A', 'R', 'G'), country_tag('B', 'O', 'L'),
		country_tag('B', 'R', 'A'), country_tag('C', 'H', 'L'),
		country_tag('C', 'O', 'L'), country_tag('C', 'O', 'S'),
		country_tag('E', 'C', 'U'), country_tag('G', 'U', 'A'),
		country_tag('H', 'O', 'N'), country_tag('M', 'E', 'X'),
		country_tag('N', 'I', 'C'), country_tag('P', 'A', 'N'),
		country_tag('P', 'A', 'R'), country_tag('P', 'R', 'U'),
		country_tag('U', 'R', 'U'), country_tag('V', 'E', 'N'),
	};
	return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

distribution blend(distribution historical, distribution demographic,
		float historical_weight) {
	auto const weight = std::clamp(historical_weight, 0.f, 1.f);
	historical = normalize(historical);
	demographic = normalize(demographic);
	return normalize({
		historical.landed_elites * weight
			+ demographic.landed_elites * (1.f - weight),
		historical.capitalists * weight
			+ demographic.capitalists * (1.f - weight),
		historical.smallholders * weight
			+ demographic.smallholders * (1.f - weight),
		historical.state * weight
			+ demographic.state * (1.f - weight),
		historical.foreign * weight
			+ demographic.foreign * (1.f - weight),
	});
}

} // namespace

historical_profile profile_for_tag(uint32_t identifying_int) {
	switch(identifying_int) {
	case country_tag('P', 'E', 'R'):
		return historical_profile::persian_estates;
	case country_tag('R', 'U', 'S'):
		return historical_profile::russian_communal;
	case country_tag('T', 'U', 'R'):
		return historical_profile::ottoman_state_tenure;
	case country_tag('U', 'S', 'A'):
		return historical_profile::american_family_farms;
	default:
		return is_latin_latifundia_tag(identifying_int)
			? historical_profile::latin_latifundia
			: historical_profile::demographic;
	}
}

historical_profile profile_for(sys::state const& state,
		dcon::province_id province) {
	auto const stored = state.world.province_get_land_profile(province);
	if(stored >= uint8_t(historical_profile::demographic)
			&& stored <= uint8_t(historical_profile::latin_latifundia))
		return historical_profile(stored);
	auto const nation =
		state.world.province_get_nation_from_province_ownership(province);
	if(!nation)
		return historical_profile::demographic;
	auto const identity =
		state.world.nation_get_identity_from_identity_holder(nation);
	return identity
		? profile_for_tag(
			state.world.national_identity_get_identifying_int(identity))
		: historical_profile::demographic;
}

std::string_view profile_localization_key(historical_profile profile) {
	switch(profile) {
	case historical_profile::persian_estates:
		return "alice_land_profile_persian";
	case historical_profile::russian_communal:
		return "alice_land_profile_russian";
	case historical_profile::ottoman_state_tenure:
		return "alice_land_profile_ottoman";
	case historical_profile::american_family_farms:
		return "alice_land_profile_american";
	case historical_profile::latin_latifundia:
		return "alice_land_profile_latifundia";
	default:
		return "alice_land_profile_demographic";
	}
}

std::string_view estate_law_localization_key(estate_law law) {
	switch(law) {
	case estate_law::concentration_limit:
		return "alice_land_law_estate_limit";
	default:
		return "alice_land_law_estate_unrestricted";
	}
}

std::string_view tenant_law_localization_key(tenant_law law) {
	switch(law) {
	case tenant_law::regulated_rent:
		return "alice_land_law_tenant_regulated";
	case tenant_law::secure_tenure:
		return "alice_land_law_tenant_secure";
	case tenant_law::right_to_buy:
		return "alice_land_law_tenant_buy";
	default:
		return "alice_land_law_tenant_free";
	}
}

distribution historical_initial_distribution(historical_profile profile,
		distribution demographic_claims, float plantation_intensity) {
	auto const demographic = normalize(demographic_claims);
	distribution historical;
	switch(profile) {
	case historical_profile::persian_estates:
		historical = {0.62f, 0.03f, 0.18f, 0.15f, 0.02f};
		break;
	case historical_profile::russian_communal:
		historical = {0.42f, 0.02f, 0.45f, 0.10f, 0.01f};
		break;
	case historical_profile::ottoman_state_tenure:
		historical = {0.33f, 0.03f, 0.32f, 0.30f, 0.02f};
		break;
	case historical_profile::american_family_farms:
		historical = {0.16f, 0.13f, 0.68f, 0.02f, 0.01f};
		break;
	case historical_profile::latin_latifundia:
		historical = {0.62f, 0.07f, 0.22f, 0.05f, 0.04f};
		break;
	default:
		return demographic;
	}

	auto result = blend(historical, demographic, 0.85f);
	if(profile == historical_profile::american_family_farms) {
		auto const plantation = std::clamp(
			finite_nonnegative(plantation_intensity), 0.f, 1.f);
		auto const transfer =
			std::min(result.smallholders, 0.35f * plantation);
		result.smallholders -= transfer;
		result.landed_elites += transfer;
	}
	return normalize(result);
}

void initialize_historical_profiles(sys::state& state) {
	if(!gamerule::age_of_transformation_enabled(state))
		return;

	province::for_each_land_province(state, [&](dcon::province_id province) {
		if(state.world.province_get_land_profile(province) != 0)
			return;
		auto const current = distribution{
			state.world.province_get_landowners_share(province),
			state.world.province_get_capitalists_share(province),
			std::max(0.f, 1.f
				- state.world.province_get_landowners_share(province)
				- state.world.province_get_capitalists_share(province)
				- state.world.province_get_state_land_share(province)
				- state.world.province_get_foreign_land_share(province)),
			state.world.province_get_state_land_share(province),
			state.world.province_get_foreign_land_share(province),
		};
		auto const profile = profile_for(state, province);
		state.world.province_set_land_profile(
			province, uint8_t(profile));
		auto const explicitly_owned =
			finite_nonnegative(current.landed_elites)
			+ finite_nonnegative(current.capitalists)
			+ finite_nonnegative(current.state)
			+ finite_nonnegative(current.foreign);
		// Scenario binaries already contain the old demographic approximation.
		// Replace it during the opening month so existing installs receive the
		// historical baseline, but never rewrite a progressed legacy save.
		if(explicitly_owned > 0.000001f && state.current_date.value > 31)
			return;

		auto const farmers = state.world.province_get_demographics(
			province,
			demographics::to_key(state, state.culture_definitions.farmers));
		auto const laborers = state.world.province_get_demographics(
			province,
			demographics::to_key(state, state.culture_definitions.laborers));
		auto const aristocrats = state.world.province_get_demographics(
			province,
			demographics::to_key(state, state.culture_definitions.aristocrat));
		auto const capitalists = state.world.province_get_demographics(
			province,
			demographics::to_key(state, state.culture_definitions.capitalists));
		auto const slaves = state.world.province_get_demographics(
			province,
			demographics::to_key(state, state.culture_definitions.slaves));
		auto const rural = finite_nonnegative(farmers + laborers);
		auto const demographic = target_from_claims(
			rural + 1.f,
			rural / 50.f + finite_nonnegative(aristocrats) * 200.f
				+ finite_nonnegative(slaves),
			rural / 50.f + finite_nonnegative(capitalists) * 200.f);
		auto const plantation = finite_nonnegative(slaves)
			/ std::max(1.f, rural + finite_nonnegative(slaves));
		auto const initial = historical_initial_distribution(
			profile, demographic, plantation);
		state.world.province_set_landowners_share(
			province, initial.landed_elites);
		state.world.province_set_capitalists_share(
			province, initial.capitalists);
		state.world.province_set_state_land_share(
			province, initial.state);
		state.world.province_set_foreign_land_share(
			province, initial.foreign);
	});
}

float update_smoothed_rent(float previous_daily_rent,
		float current_daily_rent, float window_days) {
	auto const current = finite_nonnegative(current_daily_rent);
	auto const previous = finite_nonnegative(previous_daily_rent);
	auto const window = std::isfinite(window_days)
		? std::max(1.f, window_days) : 270.f;
	if(previous <= 0.f)
		return current;
	return previous + (current - previous) / window;
}

land_use_distribution classify_land_use(float rural_population,
		float privately_worked_land, float tenant_protection) {
	land_use_distribution result;
	auto const population = finite_nonnegative(rural_population);
	if(population <= 0.f)
		return result;
	auto const operated = std::clamp(
		finite_nonnegative(privately_worked_land), 0.f, 1.f);
	auto const protection = std::clamp(
		finite_nonnegative(tenant_protection), 0.f, 1.f);
	result.smallholders = operated;
	result.tenants = (1.f - operated) * (0.25f + 0.5f * protection);
	result.landless_laborers =
		std::max(0.f, 1.f - result.smallholders - result.tenants);
	return result;
}

market_config configuration_for(sys::state const& state,
		dcon::province_id province) {
	market_config config;
	config.enabled = gamerule::age_of_transformation_enabled(state);
	auto const nation =
		state.world.province_get_nation_from_province_ownership(province);
	auto const rules = nation
		? state.world.nation_get_combined_issue_rules(nation) : 0u;
	auto const allows_private_building =
		(rules & issue_rule::pop_build_factory) != 0;
	auto const allows_state_building =
		(rules & issue_rule::build_factory) != 0;
	config.foreign_investment_allowed =
		(rules & issue_rule::allow_foreign_investment) != 0;
	auto const social_protection = nation
		? std::clamp(
			state.world.nation_get_modifier_values(nation,
				sys::national_mod_offsets::unemployment_benefit)
			+ state.world.nation_get_modifier_values(nation,
				sys::national_mod_offsets::pension_level), 0.f, 1.f)
		: 0.f;
	config.tenant_protection = social_protection;
	if(social_protection >= 0.5f) {
		config.tenant_regime = tenant_law::secure_tenure;
	} else if(social_protection > 0.f) {
		config.tenant_regime = tenant_law::regulated_rent;
	}
	config.annual_land_tax_rate = nation
		? 0.05f * nations::tax_efficiency(state, nation)
			* float(state.world.nation_get_rich_tax(nation)) / 100.f
		: 0.f;
	if((rules & issue_rule::all_voting) != 0) {
		config.estate_regime = estate_law::concentration_limit;
		config.tenant_regime = tenant_law::right_to_buy;
		config.large_estate_limit = 0.35f;
		config.right_to_buy_rate = 0.001f;
		config.reform_compensation_rate = 0.75f;
		config.tenant_protection =
			std::max(config.tenant_protection, 0.75f);
	}
	config.implementation_efficiency = nation
		? std::clamp(0.25f + 0.75f * nations::tax_efficiency(state, nation),
			0.25f, 1.f)
		: 0.25f;
	config.available_public_funds = nation && state.world.commodity_size() > 0
		? 0.02f * finite_nonnegative(
			state.world.nation_get_stockpiles(nation, economy::money))
		: 0.f;
	config.nationalization_rate =
		allows_state_building && !allows_private_building ? 0.002f : 0.f;
	config.privatization_rate =
		allows_private_building && !allows_state_building ? 0.001f : 0.f;
	return config;
}

market_result clear_market(distribution current,
		std::array<group_finance, owner_group_count> finances,
		float land_value, market_config const& config) {
	market_result result;
	result.before = normalize(current);
	result.target = result.before;
	result.after = result.before;
	if(!config.enabled)
		return result;
	result.enabled = true;

	result.land_value = std::isfinite(land_value)
		? std::max(1.f, land_value) : 1.f;
	auto const maximum_turnover = std::isfinite(config.maximum_monthly_turnover)
		? std::clamp(config.maximum_monthly_turnover, 0.f, 1.f) : 0.f;
	auto const reserve_months = std::isfinite(config.reserve_months)
		? std::max(0.f, config.reserve_months) : 6.f;
	auto const voluntary_rate = std::isfinite(config.voluntary_ask_rate)
		? std::clamp(config.voluntary_ask_rate, 0.f, 1.f) : 0.f;
	auto const protection = std::isfinite(config.tenant_protection)
		? std::clamp(config.tenant_protection, 0.f, 1.f) : 0.f;
	auto const estate_limit = std::isfinite(config.large_estate_limit)
		? std::clamp(config.large_estate_limit, 0.f, 1.f) : 1.f;

	auto const smallholders = index(owner_group::smallholders);
	auto const landed = index(owner_group::landed_elites);
	auto const capitalists = index(owner_group::capitalists);
	auto const state = index(owner_group::state);
	auto const foreign = index(owner_group::foreign);
	auto next_shares = shares(result.before);

	// Rights-based transfers are executed before the ordinary market. This is
	// what makes an estate ceiling and a tenant right-to-buy legal entitlements
	// rather than mere changes to private demand.
	auto const reform_rate = std::clamp(
		finite_nonnegative(config.agrarian_reform_rate)
			+ finite_nonnegative(config.right_to_buy_rate), 0.f, 1.f);
	auto const implementation = std::clamp(
		finite_nonnegative(config.implementation_efficiency), 0.f, 1.f);
	auto const compensation_rate = std::clamp(
		finite_nonnegative(config.reform_compensation_rate), 0.f, 1.f);
	auto const concentrated =
		next_shares[landed] + next_shares[capitalists];
	result.elite_resistance = std::clamp(
		concentrated * (0.25f + 0.75f * (1.f - implementation)),
		0.f, 0.95f);
	auto desired_landed =
		std::max(0.f, next_shares[landed] - estate_limit)
		+ next_shares[landed] * reform_rate;
	auto desired_capitalist =
		std::max(0.f, next_shares[capitalists] - estate_limit)
		+ next_shares[capitalists] * reform_rate;
	auto const desired_reform = desired_landed + desired_capitalist;
	float reform_capacity = maximum_turnover * implementation
		* (1.f - result.elite_resistance);
	if(compensation_rate > 0.f) {
		auto const funds = finite_nonnegative(config.available_public_funds);
		reform_capacity = std::min(reform_capacity,
			funds / (result.land_value * compensation_rate));
	}
	result.reform_turnover =
		std::min(desired_reform, reform_capacity);
	float reform_from_landed = 0.f;
	float reform_from_capitalist = 0.f;
	if(result.reform_turnover > 0.0000001f && desired_reform > 0.f) {
		reform_from_landed =
			result.reform_turnover * desired_landed / desired_reform;
		reform_from_capitalist =
			result.reform_turnover - reform_from_landed;
		next_shares[landed] -= reform_from_landed;
		next_shares[capitalists] -= reform_from_capitalist;
		next_shares[smallholders] += result.reform_turnover;
		result.reform_compensation =
			result.reform_turnover * result.land_value * compensation_rate;
		result.public_cost = result.reform_compensation;
		result.cash_delta[landed] +=
			reform_from_landed * result.land_value * compensation_rate;
		result.cash_delta[capitalists] +=
			reform_from_capitalist * result.land_value * compensation_rate;
	}

	float total_bids = 0.f;
	float total_asks = 0.f;
	for(std::size_t i = 0; i < owner_group_count; ++i) {
		auto const savings =
			finite_nonnegative(finances[i].liquid_savings);
		auto const reserve =
			reserve_months * finite_nonnegative(
				finances[i].monthly_essential_needs);
		auto const excess_cash = std::max(0.f, savings - reserve);
		auto const hardship = std::clamp(
			finite_nonnegative(finances[i].hardship), 0.f, 1.f);
		auto const distress_ask = next_shares[i]
			* 0.03f * hardship * (1.f - protection);
		result.bids[i] = excess_cash / result.land_value;
		result.asks[i] = next_shares[i]
			* voluntary_rate + distress_ask;
		result.distress_asks += distress_ask;
	}

	if(next_shares[landed] >= estate_limit)
		result.bids[landed] = 0.f;
	if(next_shares[capitalists] >= estate_limit)
		result.bids[capitalists] = 0.f;
	if(!config.foreign_investment_allowed) {
		result.bids[foreign] = 0.f;
		result.asks[foreign] += next_shares[foreign] * 0.02f;
	}
	auto const nationalization = std::clamp(
		finite_nonnegative(config.nationalization_rate), 0.f, 1.f);
	auto const privatization = std::clamp(
		finite_nonnegative(config.privatization_rate), 0.f, 1.f);
	result.asks[capitalists] += next_shares[capitalists] * nationalization;
	result.asks[landed] += next_shares[landed] * nationalization;
	result.asks[foreign] += next_shares[foreign] * nationalization;
	result.asks[state] += next_shares[state] * privatization;
	if(nationalization > 0.f) {
		for(std::size_t i = 0; i < owner_group_count; ++i)
			if(i != state)
				result.bids[i] = 0.f;
	}
	if(privatization > 0.f)
		result.bids[state] = 0.f;

	for(std::size_t i = 0; i < owner_group_count; ++i) {
		result.asks[i] = std::min(result.asks[i], next_shares[i]);
		total_bids += result.bids[i];
		total_asks += result.asks[i];
	}
	result.land_tax = result.land_value
		* std::clamp(finite_nonnegative(config.annual_land_tax_rate), 0.f, 1.f)
		/ 12.f;
	auto const private_turnover = std::min({
		std::max(0.f, maximum_turnover - result.reform_turnover),
		total_bids, total_asks});
	result.turnover = result.reform_turnover + private_turnover;
	if(private_turnover <= 0.0000001f) {
		result.bids[smallholders] += result.reform_turnover;
		result.asks[landed] += reform_from_landed;
		result.asks[capitalists] += reform_from_capitalist;
		result.after = from_shares(next_shares);
		result.target = result.after;
		return result;
	}

	float cash_balance = 0.f;
	std::array<float, owner_group_count> private_cash_delta{};
	for(std::size_t i = 0; i < owner_group_count; ++i) {
		auto const bought = private_turnover * result.bids[i] / total_bids;
		auto const sold = private_turnover * result.asks[i] / total_asks;
		next_shares[i] += bought - sold;
		private_cash_delta[i] = (sold - bought) * result.land_value;
		cash_balance += private_cash_delta[i];
	}
	private_cash_delta[smallholders] -= cash_balance;
	for(std::size_t i = 0; i < owner_group_count; ++i)
		result.cash_delta[i] += private_cash_delta[i];
	result.bids[smallholders] += result.reform_turnover;
	result.asks[landed] += reform_from_landed;
	result.asks[capitalists] += reform_from_capitalist;
	result.after = from_shares(next_shares);
	result.target = result.after;
	return result;
}

market_result clear_market(distribution current,
		std::array<float, owner_group_count> savings,
		float land_value, market_config const& config) {
	std::array<group_finance, owner_group_count> finances{};
	for(std::size_t i = 0; i < owner_group_count; ++i)
		finances[i].liquid_savings = savings[i];
	return clear_market(current, finances, land_value, config);
}

void update_markets(sys::state& state) {
	if(!gamerule::age_of_transformation_enabled(state))
		return;
	auto const date = state.current_date.to_ymd(state.start_date);

	province::for_each_land_province(state, [&](dcon::province_id province) {
		auto const current_daily_rent =
			finite_nonnegative(state.world.province_get_rgo_profit(province));
		auto const smoothed_rent = update_smoothed_rent(
			state.world.province_get_smoothed_land_rent(province),
			current_daily_rent);
		state.world.province_set_smoothed_land_rent(
			province, smoothed_rent);
		if(date.day != 1)
			return;

		auto const nation =
			state.world.province_get_nation_from_province_ownership(province);
		auto const config = configuration_for(state, province);

		std::array<group_finance, owner_group_count> finances{};
		std::array<float, owner_group_count> population{};
		auto const state_instance =
			state.world.province_get_state_membership(province);
		auto const market = state_instance
			? state.world.state_instance_get_market_from_local_market(
				state_instance) : dcon::market_id{};
		for(auto location : state.world.province_get_pop_location(province)) {
			auto const pop = location.get_pop().id;
			if(!participates(state, pop))
				continue;
			auto const group = group_for_pop(state, pop);
			auto const i = index(group);
			auto const size =
				finite_nonnegative(state.world.pop_get_size(pop));
			auto const savings =
				finite_nonnegative(state.world.pop_get_savings(pop));
			auto const life_needs = market
				? finite_nonnegative(state.world.market_get_life_needs_costs(
					market, state.world.pop_get_poptype(pop))) * size * 30.f
				: 0.f;
			auto const need_shortfall = std::clamp(
				1.f - pop_demographics::get_life_needs(state, pop),
				0.f, 1.f);
			auto const employment = size > 0.f
				? std::clamp(
					pop_demographics::get_employment(state, pop) / size,
					0.f, 1.f) : 1.f;
			auto const debt_stress = life_needs > 0.f
				? std::clamp((life_needs - savings) / life_needs,
					0.f, 1.f) : 0.f;
			finances[i].liquid_savings += savings;
			finances[i].monthly_essential_needs += life_needs;
			finances[i].hardship += size
				* (0.5f * need_shortfall
					+ 0.3f * (1.f - employment)
					+ 0.2f * debt_stress);
			population[i] += size;
		}
		for(std::size_t i = 0; i < 3; ++i) {
			if(population[i] > 0.f)
				finances[i].hardship /= population[i];
		}

		auto const current = distribution{
			state.world.province_get_landowners_share(province),
			state.world.province_get_capitalists_share(province),
			std::max(0.f, 1.f
				- state.world.province_get_landowners_share(province)
				- state.world.province_get_capitalists_share(province)
				- state.world.province_get_state_land_share(province)
				- state.world.province_get_foreign_land_share(province)),
			state.world.province_get_state_land_share(province),
			state.world.province_get_foreign_land_share(province),
		};
		auto const foreign_investment = nation
			? nations::get_foreign_investment(state, nation) : 0.f;
		finances[index(owner_group::state)].liquid_savings = nation
			? finite_nonnegative(
				state.world.nation_get_national_bank(nation)) : 0.f;
		finances[index(owner_group::foreign)].liquid_savings =
			config.foreign_investment_allowed
				? finite_nonnegative(foreign_investment) : 0.f;
		auto const daily_rgo_income = std::max(
			smoothed_rent,
			finite_nonnegative(state.world.province_get_rgo_bank(province))
				* economy::pops::trade_dividents_rate);
		auto const land_value = std::max(1.f,
			daily_rgo_income * 365.f * 10.f);
		auto const result = clear_market(
			current, finances, land_value, config);
		float total_bids = 0.f;
		float total_asks = 0.f;
		for(std::size_t i = 0; i < owner_group_count; ++i) {
			total_bids += result.bids[i];
			total_asks += result.asks[i];
		}
		state.world.province_set_land_market_turnover(
			province, result.turnover);
		state.world.province_set_land_market_value(
			province, result.land_value);
		state.world.province_set_land_market_bids(
			province, total_bids);
		state.world.province_set_land_market_asks(
			province, total_asks);
		state.world.province_set_land_market_distress_asks(
			province, result.distress_asks);
		state.world.province_set_land_market_tax(
			province, result.land_tax);
		state.world.province_set_land_reform_turnover(
			province, result.reform_turnover);
		state.world.province_set_land_reform_compensation(
			province, result.reform_compensation);
		state.world.province_set_land_elite_resistance(
			province, result.elite_resistance);
		auto const land_use = classify_land_use(
			population[index(owner_group::smallholders)],
			result.after.smallholders, config.tenant_protection);
		state.world.province_set_land_use_tenant_share(
			province, land_use.tenants);
		state.world.province_set_land_use_landless_share(
			province, land_use.landless_laborers);
		state.world.province_set_smallholder_land_change(
			province,
			result.after.smallholders - result.before.smallholders);
		state.world.province_set_landowner_land_change(
			province,
			result.after.landed_elites - result.before.landed_elites);
		state.world.province_set_capitalist_land_change(
			province,
			result.after.capitalists - result.before.capitalists);
		state.world.province_set_state_land_change(
			province, result.after.state - result.before.state);
		state.world.province_set_foreign_land_change(
			province, result.after.foreign - result.before.foreign);
		for(std::size_t i = 0; i < 3; ++i) {
			apply_cash_delta(state, province, owner_group(i),
				result.cash_delta[i],
				finances[i].liquid_savings, population[i]);
		}
		apply_state_cash_delta(state, nation,
			result.cash_delta[index(owner_group::state)]);
		apply_foreign_cash_delta(state, nation,
			result.cash_delta[index(owner_group::foreign)]);
		apply_treasury_cost(state, nation, result.public_cost);
		float collected_land_tax = 0.f;
		auto const post_market_shares = shares(result.after);
		for(std::size_t i = 0; i < 3; ++i) {
			auto const due = std::min(
				std::max(0.f, finances[i].liquid_savings
					+ result.cash_delta[i]),
				result.land_tax * post_market_shares[i]);
			apply_cash_delta(state, province, owner_group(i), -due,
				finances[i].liquid_savings, population[i]);
			collected_land_tax += due;
		}
		auto const foreign_due = std::min(
			std::max(0.f,
				finances[index(owner_group::foreign)].liquid_savings
				+ result.cash_delta[index(owner_group::foreign)]),
			result.land_tax * post_market_shares[
				index(owner_group::foreign)]);
		apply_foreign_cash_delta(state, nation, -foreign_due);
		collected_land_tax += foreign_due;
		apply_state_cash_delta(state, nation, collected_land_tax);
		state.world.province_set_landowners_share(
			province, result.after.landed_elites);
		state.world.province_set_capitalists_share(
			province, result.after.capitalists);
		state.world.province_set_state_land_share(
			province, result.after.state);
		state.world.province_set_foreign_land_share(
			province, result.after.foreign);
	});
}

} // namespace economy::land_ownership
