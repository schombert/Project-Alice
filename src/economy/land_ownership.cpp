#include "land_ownership.hpp"

#include "economy_pops_constants.hpp"
#include "demographics.hpp"
#include "gamerule.hpp"
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
	config.tenant_protection = nation
		? std::clamp(
			state.world.nation_get_modifier_values(nation,
				sys::national_mod_offsets::unemployment_benefit)
			+ state.world.nation_get_modifier_values(nation,
				sys::national_mod_offsets::pension_level), 0.f, 1.f)
		: 0.f;
	config.annual_land_tax_rate = nation
		? 0.05f * nations::tax_efficiency(state, nation)
			* float(state.world.nation_get_rich_tax(nation)) / 100.f
		: 0.f;
	config.large_estate_limit =
		(rules & issue_rule::all_voting) != 0 ? 0.35f : 1.f;
	config.agrarian_reform_rate =
		(rules & issue_rule::all_voting) != 0 ? 0.001f : 0.f;
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

	auto const current_shares = shares(result.before);
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
		auto const distress_ask = current_shares[i]
			* 0.03f * hardship * (1.f - protection);
		result.bids[i] = excess_cash / result.land_value;
		result.asks[i] = current_shares[i]
			* voluntary_rate + distress_ask;
		result.distress_asks += distress_ask;
	}

	auto const smallholders = index(owner_group::smallholders);
	auto const landed = index(owner_group::landed_elites);
	auto const capitalists = index(owner_group::capitalists);
	auto const state = index(owner_group::state);
	auto const foreign = index(owner_group::foreign);
	if(current_shares[landed] >= estate_limit)
		result.bids[landed] = 0.f;
	if(current_shares[capitalists] >= estate_limit)
		result.bids[capitalists] = 0.f;
	result.asks[landed] +=
		std::max(0.f, current_shares[landed] - estate_limit);
	result.asks[capitalists] +=
		std::max(0.f, current_shares[capitalists] - estate_limit);
	if(!config.foreign_investment_allowed) {
		result.bids[foreign] = 0.f;
		result.asks[foreign] += current_shares[foreign] * 0.02f;
	}
	auto const agrarian = std::clamp(
		finite_nonnegative(config.agrarian_reform_rate), 0.f, 1.f);
	auto const nationalization = std::clamp(
		finite_nonnegative(config.nationalization_rate), 0.f, 1.f);
	auto const privatization = std::clamp(
		finite_nonnegative(config.privatization_rate), 0.f, 1.f);
	result.asks[landed] += current_shares[landed] * agrarian;
	result.asks[capitalists] += current_shares[capitalists]
		* (agrarian + nationalization);
	result.asks[landed] += current_shares[landed] * nationalization;
	result.asks[foreign] += current_shares[foreign] * nationalization;
	result.asks[state] += current_shares[state] * privatization;
	if(agrarian > 0.f) {
		for(std::size_t i = 0; i < owner_group_count; ++i)
			if(i != smallholders)
				result.bids[i] = 0.f;
	}
	if(nationalization > 0.f) {
		for(std::size_t i = 0; i < owner_group_count; ++i)
			if(i != state)
				result.bids[i] = 0.f;
	}
	if(privatization > 0.f)
		result.bids[state] = 0.f;

	for(std::size_t i = 0; i < owner_group_count; ++i) {
		result.asks[i] = std::min(result.asks[i], current_shares[i]);
		total_bids += result.bids[i];
		total_asks += result.asks[i];
	}
	result.land_tax = result.land_value
		* std::clamp(finite_nonnegative(config.annual_land_tax_rate), 0.f, 1.f)
		/ 12.f;
	result.turnover = std::min({maximum_turnover, total_bids, total_asks});
	if(result.turnover <= 0.0000001f)
		return result;

	auto next_shares = current_shares;
	float cash_balance = 0.f;
	for(std::size_t i = 0; i < owner_group_count; ++i) {
		auto const bought = result.turnover * result.bids[i] / total_bids;
		auto const sold = result.turnover * result.asks[i] / total_asks;
		next_shares[i] += bought - sold;
		result.cash_delta[i] = (sold - bought) * result.land_value;
		cash_balance += result.cash_delta[i];
	}
	result.cash_delta[smallholders] -= cash_balance;
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
