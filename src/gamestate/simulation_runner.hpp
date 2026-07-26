#pragma once

#include "culture/transformation_politics.hpp"
#include "economy/banking_stability.hpp"
#include "economy/demographics.hpp"
#include "economy/economy_stats.hpp"
#include "economy/human_development.hpp"
#include "economy/price.hpp"
#include "economy/world_trade_capacity.hpp"
#include "gamerule/gamerule.hpp"
#include "gamestate/system_state.hpp"
#include "military/military.hpp"
#include "nations/diplomatic_crisis_dynamics.hpp"

#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace sys::simulation {

// The snapshot deliberately contains only aggregates. Iterating entities in their
// stable dcon order makes reports reproducible without exposing a very large or
// privacy-sensitive per-entity dump.
enum class invariant_field : uint8_t {
	none,
	inflation,
	pop_size,
	pop_savings,
	pop_satisfaction,
	pop_employment,
	commodity_price,
	commodity_supply,
	commodity_demand,
	labor_price,
	labor_supply,
	labor_demand,
	labor_demand_satisfaction,
	labor_supply_sold,
	army_supply_reserve,
	supply_depot_stockpile,
	nation_treasury,
	nation_debt,
	national_bank,
	private_investment,
	market_gdp,
	factory_profit,
	control_ratio,
	legitimacy,
	coalition_power,
	banking_health,
	trade_route_volume,
	trade_route_capacity,
	trade_congestion,
	crisis_metric,
	aggregate
};

inline constexpr std::string_view invariant_field_name(invariant_field field) noexcept {
	switch(field) {
	case invariant_field::none: return "none";
	case invariant_field::inflation: return "inflation";
	case invariant_field::pop_size: return "pop_size";
	case invariant_field::pop_savings: return "pop_savings";
	case invariant_field::pop_satisfaction: return "pop_satisfaction";
	case invariant_field::pop_employment: return "pop_employment";
	case invariant_field::commodity_price: return "commodity_price";
	case invariant_field::commodity_supply: return "commodity_supply";
	case invariant_field::commodity_demand: return "commodity_demand";
	case invariant_field::labor_price: return "labor_price";
	case invariant_field::labor_supply: return "labor_supply";
	case invariant_field::labor_demand: return "labor_demand";
	case invariant_field::labor_demand_satisfaction: return "labor_demand_satisfaction";
	case invariant_field::labor_supply_sold: return "labor_supply_sold";
	case invariant_field::army_supply_reserve: return "army_supply_reserve";
	case invariant_field::supply_depot_stockpile: return "supply_depot_stockpile";
	case invariant_field::nation_treasury: return "nation_treasury";
	case invariant_field::nation_debt: return "nation_debt";
	case invariant_field::national_bank: return "national_bank";
	case invariant_field::private_investment: return "private_investment";
	case invariant_field::market_gdp: return "market_gdp";
	case invariant_field::factory_profit: return "factory_profit";
	case invariant_field::control_ratio: return "control_ratio";
	case invariant_field::legitimacy: return "legitimacy";
	case invariant_field::coalition_power: return "coalition_power";
	case invariant_field::banking_health: return "banking_health";
	case invariant_field::trade_route_volume: return "trade_route_volume";
	case invariant_field::trade_route_capacity: return "trade_route_capacity";
	case invariant_field::trade_congestion: return "trade_congestion";
	case invariant_field::crisis_metric: return "crisis_metric";
	case invariant_field::aggregate: return "aggregate";
	}
	return "unknown";
}

struct first_violation {
	invariant_field field = invariant_field::none;
	int32_t entity_index = -1;
	int32_t subindex = -1;
	float value = 0.0f;
};

struct invariant_counts {
	uint64_t nonfinite = 0;
	uint64_t negative = 0;
	uint64_t out_of_range = 0;
	first_violation first{};

	[[nodiscard]] constexpr uint64_t total() const noexcept {
		return nonfinite + negative + out_of_range;
	}

	[[nodiscard]] constexpr bool ok() const noexcept {
		return total() == 0;
	}
};

struct aggregate_snapshot {
	uint64_t tick = 0;
	int32_t date_raw = 0;
	bool age_of_transformation = false;
	sys::checksum_key save_checksum{};

	uint64_t pop_count = 0;
	uint64_t province_count = 0;
	uint64_t market_count = 0;
	uint64_t commodity_market_cells = 0;
	uint64_t labor_market_cells = 0;
	uint64_t army_count = 0;
	uint64_t depot_count = 0;
	uint64_t nation_count = 0;
	uint64_t factory_count = 0;
	uint64_t unprofitable_factory_count = 0;
	uint64_t owned_province_count = 0;
	uint64_t owner_controlled_province_count = 0;
	uint64_t foreign_controlled_province_count = 0;
	uint64_t rebel_controlled_province_count = 0;
	uint64_t uncontrolled_owned_province_count = 0;
	uint64_t transformed_nation_count = 0;
	uint64_t trade_route_count = 0;

	double inflation = 0.0;
	double population = 0.0;
	double pop_savings = 0.0;
	// Population-weighted shares. Divide each by population to obtain the
	// average fulfilment of the corresponding need tier.
	double population_weighted_life_needs = 0.0;
	double population_weighted_everyday_needs = 0.0;
	double population_weighted_luxury_needs = 0.0;
	double unemployed_population = 0.0;
	double commodity_price_sum = 0.0;
	double commodity_supply = 0.0;
	double commodity_demand = 0.0;
	double labor_price_sum = 0.0;
	double labor_supply = 0.0;
	double labor_demand = 0.0;
	double employed_labor = 0.0;
	double army_supply_reserve_sum = 0.0;
	double minimum_army_supply_reserve = 0.0;
	double depot_stockpile = 0.0;
	double treasury = 0.0;
	double government_debt = 0.0;
	double national_bank = 0.0;
	double private_investment = 0.0;
	double market_gdp = 0.0;
	// Profit is deliberately signed: a negative value is an economic signal,
	// not an invariant violation.
	double factory_profit = 0.0;
	double control_ratio_sum = 0.0;
	double minimum_control_ratio = 0.0;
	double legitimacy_sum = 0.0;
	double minimum_legitimacy = 0.0;
	double coalition_power_sum = 0.0;
	double banking_health_sum = 0.0;
	double minimum_banking_health = 0.0;
	double banking_stress_sum = 0.0;
	double trade_route_cargo = 0.0;
	double trade_effective_capacity = 0.0;
	double trade_congestion_sum = 0.0;
	double maximum_trade_congestion = 0.0;
	nations::diplomatic_crisis_dynamics::escalation_stage crisis_stage =
		nations::diplomatic_crisis_dynamics::escalation_stage::inactive;
	double crisis_temperature = 0.0;
	double crisis_escalation_pressure = 0.0;
	double crisis_settlement_pressure = 0.0;
	double crisis_war_risk = 0.0;
	double baseline_natural_growth = 0.0;
	double starvation_loss = 0.0;
	double housing_loss = 0.0;
	double transition_reduction = 0.0;
	double net_natural_change = 0.0;
	double population_weighted_housing_access = 0.0;
	double population_weighted_urbanization = 0.0;
	double population_weighted_overcrowding = 0.0;
	double population_weighted_education_access = 0.0;
	double population_weighted_human_development = 0.0;
	double gross_internal_migration = 0.0;
	double gross_international_migration = 0.0;
	// Optional, single-country probe for deterministic headless regression runs.
	// It is deliberately absent unless the caller supplies a nation, so normal
	// aggregate reports and the game UI remain unaffected.
	int32_t tracked_nation_index = -1;
	double tracked_nation_population = 0.0;
	double tracked_nation_monthly_natural_change = 0.0;
	double tracked_nation_monthly_army_attrition = 0.0;
	double tracked_nation_daily_internal_migration = 0.0;
	double tracked_nation_daily_external_migration = 0.0;

	invariant_counts observed_violations{};
};

struct validation_report {
	bool valid = true;
	invariant_counts violations{};
};

namespace detail {

inline void remember_first(invariant_counts& counts, invariant_field field, int32_t entity_index,
		int32_t subindex, float value) noexcept {
	if(counts.first.field == invariant_field::none) {
		counts.first = first_violation{field, entity_index, subindex, value};
	}
}

inline bool observe_nonnegative(invariant_counts& counts, invariant_field field, int32_t entity_index,
		int32_t subindex, float value, float maximum = std::numeric_limits<float>::infinity()) noexcept {
	if(!std::isfinite(value)) {
		++counts.nonfinite;
		remember_first(counts, field, entity_index, subindex, value);
		return false;
	}
	if(value < 0.0f) {
		++counts.negative;
		remember_first(counts, field, entity_index, subindex, value);
		return false;
	}
	if(value > maximum) {
		++counts.out_of_range;
		remember_first(counts, field, entity_index, subindex, value);
		return false;
	}
	return true;
}

inline bool observe_finite(invariant_counts& counts, invariant_field field, int32_t entity_index,
		int32_t subindex, float value) noexcept {
	if(!std::isfinite(value)) {
		++counts.nonfinite;
		remember_first(counts, field, entity_index, subindex, value);
		return false;
	}
	return true;
}

inline void write_json_number(std::ostringstream& out, float value) {
	if(std::isfinite(value)) {
		out << value;
	} else {
		out << "null";
	}
}

inline void write_checksum_hex(std::ostringstream& out, sys::checksum_key const& checksum) {
	constexpr char digits[] = "0123456789abcdef";
	for(auto byte : checksum.key) {
		out << digits[byte >> 4] << digits[byte & 0x0f];
	}
}

} // namespace detail

[[nodiscard]] inline aggregate_snapshot collect_snapshot(sys::state& state, uint64_t tick = 0,
		dcon::nation_id tracked_nation = {}) {
	aggregate_snapshot result{};
	result.tick = tick;
	result.date_raw = state.current_date.to_raw_value();
	result.age_of_transformation = gamerule::age_of_transformation_enabled(state);
	// Hash exactly the serialized save section (excluding local-only fields), so
	// repeated runs and save/load continuations share the same comparison key.
	result.save_checksum = state.get_save_checksum();

	if(detail::observe_nonnegative(result.observed_violations, invariant_field::inflation, -1, -1,
			state.inflation)) {
		result.inflation = state.inflation;
	}

	bool has_banking_result = false;
	state.world.for_each_nation([&](dcon::nation_id nation) {
		++result.nation_count;
		auto const entity = int32_t(nation.index());
		auto const treasury = state.world.nation_get_stockpiles(nation, economy::money);
		auto const debt = state.world.nation_get_local_loan(nation);
		auto const bank = state.world.nation_get_national_bank(nation);
		auto const investment = state.world.nation_get_private_investment(nation);
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::nation_treasury,
				entity, -1, treasury)) {
			result.treasury += double(treasury);
		}
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::nation_debt,
				entity, -1, debt)) {
			result.government_debt += double(debt);
		}
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::national_bank,
				entity, -1, bank)) {
			result.national_bank += double(bank);
		}
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::private_investment,
				entity, -1, investment)) {
			result.private_investment += double(investment);
		}

		auto const banking = economy::banking_stability::evaluate_nation(state, nation);
		if(banking.enabled
			&& detail::observe_nonnegative(result.observed_violations, invariant_field::banking_health,
				entity, -1, banking.credit_health, 1.0f)) {
			result.banking_health_sum += double(banking.credit_health);
			result.banking_stress_sum += double(banking.financial_stress);
			if(!has_banking_result || banking.credit_health < result.minimum_banking_health)
				result.minimum_banking_health = banking.credit_health;
			has_banking_result = true;
		}
	});

	state.world.for_each_pop([&](dcon::pop_id pop) {
		++result.pop_count;
		auto const entity = int32_t(pop.index());
		auto const size = state.world.pop_get_size(pop);
		auto const savings = state.world.pop_get_savings(pop);
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::pop_size, entity, -1, size)) {
			result.population += double(size);
			auto const satisfaction = state.world.pop_get_satisfaction(pop);
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::pop_satisfaction,
					entity, -1, satisfaction, 1.0f)) {
				result.population_weighted_life_needs +=
					double(size) * pop_demographics::get_life_needs(state, pop);
				result.population_weighted_everyday_needs +=
					double(size) * pop_demographics::get_everyday_needs(state, pop);
				result.population_weighted_luxury_needs +=
					double(size) * pop_demographics::get_luxury_needs(state, pop);
			}
			auto const employed = pop_demographics::get_employment(state, pop);
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::pop_employment,
					entity, 1, employed, size)) {
				result.unemployed_population += double(size - employed);
			}
			auto const development = economy::human_development::evaluate_pop(state, pop);
			auto const province = state.world.pop_get_province_from_pop_location(pop);
			auto const owner = province
				? state.world.province_get_nation_from_province_ownership(province)
				: dcon::nation_id{};
			if(development.enabled && owner) {
				auto const legacy_growth = demographics::get_pop_growth_modifiers(state, pop);
				auto const starvation = demographics::get_net_pop_starvation_penalty(
					state, pop, legacy_growth);
				auto const account = economy::human_development::calculate_account(
					size, legacy_growth, starvation, development);
				result.baseline_natural_growth += account.baseline_natural_growth;
				result.starvation_loss += account.starvation_loss;
				result.housing_loss += account.housing_loss;
				result.transition_reduction += account.transition_reduction;
				result.net_natural_change += account.net_natural_change;
				result.population_weighted_housing_access +=
					double(size) * development.factors.housing_access;
				result.population_weighted_urbanization +=
					double(size) * development.factors.urbanization;
				result.population_weighted_overcrowding +=
					double(size) * development.overcrowding;
				result.population_weighted_education_access +=
					double(size) * development.factors.education_access;
				result.population_weighted_human_development +=
					double(size) * development.human_development_index;
			}
		}
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::pop_savings, entity, -1, savings)) {
			result.pop_savings += double(savings);
		}
	});

	if(tracked_nation && state.world.nation_is_valid(tracked_nation)) {
		result.tracked_nation_index = int32_t(tracked_nation.index());
		for(auto province : state.world.nation_get_province_ownership(tracked_nation)) {
			auto const province_id = province.get_province();
			for(auto membership : state.world.province_get_pop_location(province_id)) {
				result.tracked_nation_population += double(state.world.pop_get_size(membership.get_pop()));
			}
			result.tracked_nation_daily_internal_migration +=
				double(state.world.province_get_daily_net_migration(province_id));
			result.tracked_nation_daily_external_migration +=
				double(state.world.province_get_daily_net_immigration(province_id));
		}
		result.tracked_nation_monthly_natural_change =
			double(nations::get_monthly_pop_increase_of_nation(state, tracked_nation));
		result.tracked_nation_monthly_army_attrition =
			double(military::estimated_monthly_army_pop_attrition_loss(state, tracked_nation));
	}

	state.world.for_each_market([&](dcon::market_id market) {
		++result.market_count;
		auto const gdp = state.world.market_get_gdp(market);
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::market_gdp,
				int32_t(market.index()), -1, gdp)) {
			result.market_gdp += double(gdp);
		}
		state.world.for_each_commodity([&](dcon::commodity_id commodity) {
			++result.commodity_market_cells;
			auto const entity = int32_t(market.index());
			auto const subindex = int32_t(commodity.index());
			auto const price = state.world.market_get_price(market, commodity);
			auto const supply = state.world.market_get_supply(market, commodity);
			auto const demand = state.world.market_get_demand(market, commodity);
			auto const maximum_price = economy::price_properties::commodity::maximum(
				float(state.world.commodity_get_cost(commodity)));
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::commodity_price,
					entity, subindex, price, maximum_price)) {
				result.commodity_price_sum += double(price);
			}
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::commodity_supply,
					entity, subindex, supply)) {
				result.commodity_supply += double(supply);
			}
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::commodity_demand,
					entity, subindex, demand)) {
				result.commodity_demand += double(demand);
			}
		});
	});

	state.world.for_each_factory([&](dcon::factory_id factory) {
		++result.factory_count;
		if(state.world.factory_get_unprofitable(factory))
			++result.unprofitable_factory_count;
		auto const profit = state.world.factory_get_profit(factory);
		if(detail::observe_finite(result.observed_violations, invariant_field::factory_profit,
				int32_t(factory.index()), -1, profit)) {
			result.factory_profit += double(profit);
		}
	});

	bool has_owned_control = false;
	state.world.for_each_province([&](dcon::province_id province) {
		++result.province_count;
		auto const entity = int32_t(province.index());
		if(auto const owner = state.world.province_get_nation_from_province_ownership(province); owner) {
			++result.owned_province_count;
			auto const controller =
				state.world.province_get_nation_from_province_control(province);
			auto const rebel_controller =
				state.world.province_get_rebel_faction_from_province_rebel_control(province);
			if(controller == owner) {
				++result.owner_controlled_province_count;
			} else if(controller) {
				++result.foreign_controlled_province_count;
			} else if(rebel_controller) {
				++result.rebel_controlled_province_count;
			} else {
				++result.uncontrolled_owned_province_count;
			}
			auto const control = state.world.province_get_control_ratio(province);
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::control_ratio,
					entity, -1, control, 1.0f)) {
				result.control_ratio_sum += double(control);
				if(!has_owned_control || control < result.minimum_control_ratio)
					result.minimum_control_ratio = control;
				has_owned_control = true;
			}
		}
		for(int32_t labor_type = 0; labor_type < economy::labor::total; ++labor_type) {
			++result.labor_market_cells;
			auto const price = state.world.province_get_labor_price(province, labor_type);
			auto const supply = state.world.province_get_labor_supply(province, labor_type);
			auto const demand = state.world.province_get_labor_demand(province, labor_type);
			auto const demand_satisfaction =
				state.world.province_get_labor_demand_satisfaction(province, labor_type);
			auto const supply_sold = state.world.province_get_labor_supply_sold(province, labor_type);

			if(detail::observe_nonnegative(result.observed_violations, invariant_field::labor_price,
					entity, labor_type, price, economy::price_properties::labor::max)) {
				result.labor_price_sum += double(price);
			}
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::labor_supply,
					entity, labor_type, supply)) {
				result.labor_supply += double(supply);
				if(std::isfinite(supply_sold) && supply_sold >= 0.0f && supply_sold <= 1.0f) {
					result.employed_labor += double(supply) * double(supply_sold);
				}
			}
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::labor_demand,
					entity, labor_type, demand)) {
				result.labor_demand += double(demand);
			}
			detail::observe_nonnegative(result.observed_violations, invariant_field::labor_demand_satisfaction,
				entity, labor_type, demand_satisfaction, 1.0f);
			detail::observe_nonnegative(result.observed_violations, invariant_field::labor_supply_sold,
				entity, labor_type, supply_sold, 1.0f);
		}

		auto const stockpile = state.world.province_get_supply_depot_stockpile(province);
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::supply_depot_stockpile,
				entity, -1, stockpile)) {
			result.depot_stockpile += double(stockpile);
		}
		if(state.world.province_get_is_supply_depot(province) || stockpile != 0.0f
				|| state.world.province_get_supply_depot_owner(province)) {
			++result.depot_count;
		}
		auto const internal_migration = state.world.province_get_daily_net_migration(province);
		if(std::isfinite(internal_migration))
			result.gross_internal_migration += std::abs(double(internal_migration)) * 0.5;
		auto const international_migration = state.world.province_get_daily_net_immigration(province);
		if(std::isfinite(international_migration))
			result.gross_international_migration += std::abs(double(international_migration)) * 0.5;
	});

	bool has_army = false;
	state.world.for_each_army([&](dcon::army_id army) {
		++result.army_count;
		auto const reserve = state.world.army_get_supply_reserve(army);
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::army_supply_reserve,
				int32_t(army.index()), -1, reserve, 1.0f)) {
			result.army_supply_reserve_sum += double(reserve);
			if(!has_army || reserve < result.minimum_army_supply_reserve) {
				result.minimum_army_supply_reserve = reserve;
			}
			has_army = true;
		}
	});

	bool has_legitimacy = false;
	if(state.transformation_politics_cache_valid
		&& state.transformation_politics_cache.size() == state.world.nation_size()) {
		for(std::size_t index = 0; index < state.transformation_politics_cache.size(); ++index) {
			auto const& political = state.transformation_politics_cache[index];
			if(!political.enabled)
				continue;
			++result.transformed_nation_count;
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::legitimacy,
					int32_t(index), -1, political.legitimacy.total, 100.0f)) {
				result.legitimacy_sum += double(political.legitimacy.total);
				if(!has_legitimacy || political.legitimacy.total < result.minimum_legitimacy)
					result.minimum_legitimacy = political.legitimacy.total;
				has_legitimacy = true;
			}
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::coalition_power,
					int32_t(index), -1, political.coalition.power_share, 1.0f)) {
				result.coalition_power_sum += double(political.coalition.power_share);
			}
		}
	}

	state.world.for_each_trade_route([&](dcon::trade_route_id route) {
		++result.trade_route_count;
		auto const trade = economy::world_trade::evaluate_route_capacity(state, route);
		auto const entity = int32_t(route.index());
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::trade_route_volume,
				entity, -1, trade.cargo)) {
			result.trade_route_cargo += double(trade.cargo);
		}
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::trade_route_capacity,
				entity, -1, trade.effective_capacity)) {
			result.trade_effective_capacity += double(trade.effective_capacity);
		}
		if(detail::observe_nonnegative(result.observed_violations, invariant_field::trade_congestion,
				entity, -1, trade.congestion, 1.0f)) {
			result.trade_congestion_sum += double(trade.congestion);
			result.maximum_trade_congestion = std::max(
				result.maximum_trade_congestion, double(trade.congestion));
		}
	});

	auto const crisis = nations::diplomatic_crisis_dynamics::evaluate_current_crisis(state);
	if(crisis.enabled) {
		result.crisis_stage = crisis.stage;
		auto observe_crisis = [&](float value, double& target, int32_t subindex) {
			if(detail::observe_nonnegative(result.observed_violations, invariant_field::crisis_metric,
					-1, subindex, value, 1.0f)) {
				target = double(value);
			}
		};
		observe_crisis(crisis.normalized_temperature, result.crisis_temperature, 0);
		observe_crisis(crisis.escalation_pressure, result.crisis_escalation_pressure, 1);
		observe_crisis(crisis.settlement_pressure, result.crisis_settlement_pressure, 2);
		observe_crisis(crisis.war_risk, result.crisis_war_risk, 3);
	}

	return result;
}

[[nodiscard]] inline validation_report validate_snapshot(aggregate_snapshot const& snapshot) noexcept {
	validation_report report{};
	report.violations = snapshot.observed_violations;

	auto validate_aggregate = [&](double value) {
		if(!std::isfinite(value)) {
			++report.violations.nonfinite;
			detail::remember_first(report.violations, invariant_field::aggregate, -1, -1, float(value));
		} else if(value < 0.0) {
			++report.violations.negative;
			detail::remember_first(report.violations, invariant_field::aggregate, -1, -1, float(value));
		}
	};

	validate_aggregate(snapshot.inflation);
	validate_aggregate(snapshot.population);
	validate_aggregate(snapshot.pop_savings);
	validate_aggregate(snapshot.population_weighted_life_needs);
	validate_aggregate(snapshot.population_weighted_everyday_needs);
	validate_aggregate(snapshot.population_weighted_luxury_needs);
	validate_aggregate(snapshot.unemployed_population);
	validate_aggregate(snapshot.commodity_price_sum);
	validate_aggregate(snapshot.commodity_supply);
	validate_aggregate(snapshot.commodity_demand);
	validate_aggregate(snapshot.labor_price_sum);
	validate_aggregate(snapshot.labor_supply);
	validate_aggregate(snapshot.labor_demand);
	validate_aggregate(snapshot.employed_labor);
	validate_aggregate(snapshot.army_supply_reserve_sum);
	validate_aggregate(snapshot.minimum_army_supply_reserve);
	validate_aggregate(snapshot.depot_stockpile);
	validate_aggregate(snapshot.treasury);
	validate_aggregate(snapshot.government_debt);
	validate_aggregate(snapshot.national_bank);
	validate_aggregate(snapshot.private_investment);
	validate_aggregate(snapshot.market_gdp);
	if(!std::isfinite(snapshot.factory_profit)) {
		++report.violations.nonfinite;
		detail::remember_first(report.violations, invariant_field::aggregate, -1, -1,
			float(snapshot.factory_profit));
	}
	validate_aggregate(snapshot.control_ratio_sum);
	validate_aggregate(snapshot.minimum_control_ratio);
	validate_aggregate(snapshot.legitimacy_sum);
	validate_aggregate(snapshot.minimum_legitimacy);
	validate_aggregate(snapshot.coalition_power_sum);
	validate_aggregate(snapshot.banking_health_sum);
	validate_aggregate(snapshot.minimum_banking_health);
	validate_aggregate(snapshot.banking_stress_sum);
	validate_aggregate(snapshot.trade_route_cargo);
	validate_aggregate(snapshot.trade_effective_capacity);
	validate_aggregate(snapshot.trade_congestion_sum);
	validate_aggregate(snapshot.maximum_trade_congestion);
	validate_aggregate(snapshot.crisis_temperature);
	validate_aggregate(snapshot.crisis_escalation_pressure);
	validate_aggregate(snapshot.crisis_settlement_pressure);
	validate_aggregate(snapshot.crisis_war_risk);
	if(!std::isfinite(snapshot.baseline_natural_growth)) {
		++report.violations.nonfinite;
		detail::remember_first(report.violations, invariant_field::aggregate,
			-1, -1, float(snapshot.baseline_natural_growth));
	}
	validate_aggregate(snapshot.starvation_loss);
	validate_aggregate(snapshot.housing_loss);
	validate_aggregate(snapshot.transition_reduction);
	if(!std::isfinite(snapshot.net_natural_change)) {
		++report.violations.nonfinite;
		detail::remember_first(report.violations, invariant_field::aggregate,
			-1, -1, float(snapshot.net_natural_change));
	}
	validate_aggregate(snapshot.population_weighted_housing_access);
	validate_aggregate(snapshot.population_weighted_urbanization);
	validate_aggregate(snapshot.population_weighted_overcrowding);
	validate_aggregate(snapshot.population_weighted_education_access);
	validate_aggregate(snapshot.population_weighted_human_development);
	validate_aggregate(snapshot.gross_internal_migration);
	validate_aggregate(snapshot.gross_international_migration);

	report.valid = report.violations.ok();
	return report;
}

// Returns exactly one JSON object followed by '\n', suitable for appending to a
// JSONL stream. Invalid floating-point samples are represented by their counters;
// a non-finite first sample is emitted as JSON null rather than invalid NaN text.
[[nodiscard]] inline std::string serialize_jsonl(aggregate_snapshot const& snapshot,
		validation_report const& validation) {
	std::ostringstream out;
	out.imbue(std::locale::classic());
	out << std::setprecision(17);
	out << "{\"tick\":" << snapshot.tick
		<< ",\"date_raw\":" << snapshot.date_raw
		<< ",\"valid\":" << (validation.valid ? "true" : "false")
		<< ",\"save_checksum\":\"";
	detail::write_checksum_hex(out, snapshot.save_checksum);
	out << "\""
		<< ",\"ruleset\":{\"age_of_transformation\":"
		<< (snapshot.age_of_transformation ? "true" : "false") << "}"
		<< ",\"counts\":{\"pops\":" << snapshot.pop_count
		<< ",\"provinces\":" << snapshot.province_count
		<< ",\"owned_provinces\":" << snapshot.owned_province_count
		<< ",\"owner_controlled_provinces\":"
		<< snapshot.owner_controlled_province_count
		<< ",\"foreign_controlled_provinces\":"
		<< snapshot.foreign_controlled_province_count
		<< ",\"rebel_controlled_provinces\":"
		<< snapshot.rebel_controlled_province_count
		<< ",\"uncontrolled_owned_provinces\":"
		<< snapshot.uncontrolled_owned_province_count
		<< ",\"markets\":" << snapshot.market_count
		<< ",\"nations\":" << snapshot.nation_count
		<< ",\"transformed_nations\":" << snapshot.transformed_nation_count
		<< ",\"factories\":" << snapshot.factory_count
		<< ",\"unprofitable_factories\":" << snapshot.unprofitable_factory_count
		<< ",\"trade_routes\":" << snapshot.trade_route_count
		<< ",\"commodity_market_cells\":" << snapshot.commodity_market_cells
		<< ",\"labor_market_cells\":" << snapshot.labor_market_cells
		<< ",\"armies\":" << snapshot.army_count
		<< ",\"depots\":" << snapshot.depot_count << "}"
		<< ",\"economy\":{\"inflation\":" << snapshot.inflation
		<< ",\"population\":" << snapshot.population
		<< ",\"pop_savings\":" << snapshot.pop_savings
		<< ",\"market_gdp\":" << snapshot.market_gdp
		<< ",\"factory_profit\":" << snapshot.factory_profit
		<< ",\"commodity_price_sum\":" << snapshot.commodity_price_sum
		<< ",\"commodity_supply\":" << snapshot.commodity_supply
		<< ",\"commodity_demand\":" << snapshot.commodity_demand << "}"
		<< ",\"demography\":{\"baseline_natural_growth\":"
		<< snapshot.baseline_natural_growth
		<< ",\"starvation_loss\":" << snapshot.starvation_loss
		<< ",\"housing_loss\":" << snapshot.housing_loss
		<< ",\"transition_reduction\":" << snapshot.transition_reduction
		<< ",\"net_natural_change\":" << snapshot.net_natural_change
		<< ",\"housing_access_population_sum\":"
		<< snapshot.population_weighted_housing_access
		<< ",\"urbanization_population_sum\":"
		<< snapshot.population_weighted_urbanization
		<< ",\"overcrowding_population_sum\":"
		<< snapshot.population_weighted_overcrowding
		<< ",\"education_access_population_sum\":"
		<< snapshot.population_weighted_education_access
		<< ",\"human_development_population_sum\":"
		<< snapshot.population_weighted_human_development
		<< ",\"gross_internal_migration\":" << snapshot.gross_internal_migration
		<< ",\"gross_international_migration\":"
		<< snapshot.gross_international_migration << "}"
		<< ",\"tracked_nation\":{\"index\":" << snapshot.tracked_nation_index
		<< ",\"population\":" << snapshot.tracked_nation_population
		<< ",\"monthly_natural_change\":" << snapshot.tracked_nation_monthly_natural_change
		<< ",\"monthly_army_attrition\":" << snapshot.tracked_nation_monthly_army_attrition
		<< ",\"daily_internal_migration\":" << snapshot.tracked_nation_daily_internal_migration
		<< ",\"daily_external_migration\":" << snapshot.tracked_nation_daily_external_migration << "}"
		<< ",\"living_standards\":{\"life_needs_population_sum\":"
		<< snapshot.population_weighted_life_needs
		<< ",\"everyday_needs_population_sum\":"
		<< snapshot.population_weighted_everyday_needs
		<< ",\"luxury_needs_population_sum\":"
		<< snapshot.population_weighted_luxury_needs
		<< ",\"unemployed_population\":" << snapshot.unemployed_population << "}"
		<< ",\"finance\":{\"treasury\":" << snapshot.treasury
		<< ",\"government_debt\":" << snapshot.government_debt
		<< ",\"national_bank\":" << snapshot.national_bank
		<< ",\"private_investment\":" << snapshot.private_investment << "}"
		<< ",\"labor\":{\"price_sum\":" << snapshot.labor_price_sum
		<< ",\"supply\":" << snapshot.labor_supply
		<< ",\"demand\":" << snapshot.labor_demand
		<< ",\"employed\":" << snapshot.employed_labor << "}"
		<< ",\"logistics\":{\"army_supply_reserve_sum\":" << snapshot.army_supply_reserve_sum
		<< ",\"minimum_army_supply_reserve\":" << snapshot.minimum_army_supply_reserve
		<< ",\"depot_stockpile\":" << snapshot.depot_stockpile << "}"
		<< ",\"administration\":{\"control_ratio_sum\":" << snapshot.control_ratio_sum
		<< ",\"minimum_control_ratio\":" << snapshot.minimum_control_ratio << "}"
		<< ",\"politics\":{\"legitimacy_sum\":" << snapshot.legitimacy_sum
		<< ",\"minimum_legitimacy\":" << snapshot.minimum_legitimacy
		<< ",\"coalition_power_sum\":" << snapshot.coalition_power_sum << "}"
		<< ",\"banking\":{\"credit_health_sum\":" << snapshot.banking_health_sum
		<< ",\"minimum_credit_health\":" << snapshot.minimum_banking_health
		<< ",\"financial_stress_sum\":" << snapshot.banking_stress_sum << "}"
		<< ",\"trade\":{\"cargo\":" << snapshot.trade_route_cargo
		<< ",\"effective_capacity\":" << snapshot.trade_effective_capacity
		<< ",\"congestion_sum\":" << snapshot.trade_congestion_sum
		<< ",\"maximum_congestion\":" << snapshot.maximum_trade_congestion << "}"
		<< ",\"crisis\":{\"stage\":\""
		<< nations::diplomatic_crisis_dynamics::stage_name(snapshot.crisis_stage)
		<< "\",\"normalized_temperature\":" << snapshot.crisis_temperature
		<< ",\"escalation_pressure\":" << snapshot.crisis_escalation_pressure
		<< ",\"settlement_pressure\":" << snapshot.crisis_settlement_pressure
		<< ",\"war_risk\":" << snapshot.crisis_war_risk << "}"
		<< ",\"violations\":{\"total\":" << validation.violations.total()
		<< ",\"nonfinite\":" << validation.violations.nonfinite
		<< ",\"negative\":" << validation.violations.negative
		<< ",\"out_of_range\":" << validation.violations.out_of_range
		<< ",\"first\":{\"field\":\"" << invariant_field_name(validation.violations.first.field)
		<< "\",\"entity\":" << validation.violations.first.entity_index
		<< ",\"subindex\":" << validation.violations.first.subindex
		<< ",\"value\":";
	detail::write_json_number(out, validation.violations.first.value);
	out << "}}}\n";
	return out.str();
}

struct run_options {
	uint64_t ticks = 0;
	uint64_t snapshot_cadence = 30;
	bool include_initial_snapshot = true;
	bool include_final_snapshot = true;
	bool fail_on_invariant = true;
	dcon::nation_id tracked_nation{};
};

struct run_result {
	uint64_t ticks_completed = 0;
	uint64_t snapshots_emitted = 0;
	bool completed = true;
	validation_report last_validation{};
};

using line_sink = std::function<void(std::string_view)>;

namespace detail {

template<typename TickFunction, typename SinkFunction>
run_result run_ticks_with(sys::state& state, run_options const& options, TickFunction&& tick_function,
		SinkFunction&& sink_function) {
	run_result result{};
	auto emit_snapshot = [&](uint64_t tick) {
		if(gamerule::age_of_transformation_enabled(state)
			&& (!state.transformation_politics_cache_valid
				|| state.transformation_politics_cache.size() != state.world.nation_size())) {
			politics::transformation::refresh_all_nations(state);
		}
		auto snapshot = collect_snapshot(state, tick, options.tracked_nation);
		result.last_validation = validate_snapshot(snapshot);
		auto line = serialize_jsonl(snapshot, result.last_validation);
		std::invoke(sink_function, std::string_view(line));
		++result.snapshots_emitted;
		if(options.fail_on_invariant && !result.last_validation.valid) {
			result.completed = false;
			return false;
		}
		return true;
	};

	if(options.include_initial_snapshot && !emit_snapshot(0)) {
		return result;
	}

	for(uint64_t tick = 1; tick <= options.ticks; ++tick) {
		std::invoke(tick_function, state);
		result.ticks_completed = tick;

		auto const cadence_due = options.snapshot_cadence != 0 && tick % options.snapshot_cadence == 0;
		auto const final_due = options.include_final_snapshot && tick == options.ticks;
		if((cadence_due || final_due) && !emit_snapshot(tick)) {
			return result;
		}
	}

	// With zero requested ticks, the final state is the initial state. Avoid a
	// duplicate line when the initial snapshot was already requested.
	if(options.ticks == 0 && options.include_final_snapshot && !options.include_initial_snapshot) {
		emit_snapshot(0);
	}
	return result;
}

} // namespace detail

// Synchronously advances exactly options.ticks game ticks unless an observed
// snapshot violates an invariant and fail_on_invariant is enabled. The sink is
// called synchronously; its string_view is valid only for the duration of the call.
[[nodiscard]] inline run_result run_ticks(sys::state& state, run_options const& options,
		line_sink sink = {}) {
	auto tick = [](sys::state& target) { target.single_game_tick(); };
	if(sink) {
		return detail::run_ticks_with(state, options, tick, sink);
	}
	return detail::run_ticks_with(state, options, tick, [](std::string_view) {});
}

} // namespace sys::simulation
