#include "economy.hpp"
#include "economy_government.hpp"
#include "economy_stats.hpp"
#include "economy_production.hpp"
#include "economy_trade_routes.hpp"
#include "construction.hpp"
#include "demographics.hpp"
#include "dcon_generated.hpp"
#include "ai_economy.hpp"
#include "system_state.hpp"
#include "prng.hpp"
#include "nations_templates.hpp"
#include "province_templates.hpp"
#include "triggers.hpp"
#include "advanced_province_buildings.hpp"
#include "price.hpp"
#include "economy_pops.hpp"
#include "commodities.hpp"

namespace economy {

float pop_min_wage_factor(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::minimum_wage);
}
template<typename T>
ve::fp_vector ve_pop_min_wage_factor(sys::state& state, T n) {
	return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::minimum_wage);
}
template<typename T>
ve::fp_vector ve_farmer_min_wage(sys::state& state, T markets, ve::fp_vector min_wage_factor) {
	auto life = state.world.market_get_life_needs_costs(markets, state.culture_definitions.farmers);
	auto everyday = state.world.market_get_everyday_needs_costs(markets, state.culture_definitions.farmers);
	return min_wage_factor * (life + everyday) * 1.1f;
}

template<typename T>
ve::fp_vector ve_laborer_min_wage(sys::state& state, T markets, ve::fp_vector min_wage_factor) {
	auto life = state.world.market_get_life_needs_costs(markets, state.culture_definitions.laborers);
	auto everyday = state.world.market_get_everyday_needs_costs(markets, state.culture_definitions.laborers);
	return min_wage_factor * (life + everyday) * 1.1f;
}
float farmer_min_wage(sys::state& state, dcon::market_id m, float min_wage_factor) {
	auto life = state.world.market_get_life_needs_costs(m, state.culture_definitions.farmers);
	auto everyday = state.world.market_get_everyday_needs_costs(m, state.culture_definitions.farmers);
	return min_wage_factor * (life + everyday) * 1.1f;
}
float laborer_min_wage(sys::state& state, dcon::market_id m, float min_wage_factor) {
	auto life = state.world.market_get_life_needs_costs(m, state.culture_definitions.laborers);
	auto everyday = state.world.market_get_everyday_needs_costs(m, state.culture_definitions.laborers);
	return min_wage_factor * (life + everyday) * 1.1f;
}

constexpr inline float expected_savings_per_capita = 0.0001f;

void sanity_check([[maybe_unused]] sys::state& state) {
#ifdef NDEBUG
#else
	/*
	ve::fp_vector total{};
	state.world.execute_serial_over_pop([&](auto ids) {
		total = total + state.world.pop_get_savings(ids);
	});
	assert(total.reduce() > 0.f);
	*/
	//assert(state.inflation > 0.1f && state.inflation < 10.f);
#endif
}

bool can_take_loans(sys::state& state, dcon::nation_id n) {
	if(!state.world.nation_get_is_player_controlled(n) || !state.world.nation_get_is_debt_spending(n))
		return false;

	/*
	A country cannot borrow if it is less than define:BANKRUPTCY_EXTERNAL_LOAN_YEARS since their last bankruptcy.
	*/
	auto last_br = state.world.nation_get_bankrupt_until(n);
	if(last_br && state.current_date < last_br)
		return false;

	return true;
}

float interest_payment(sys::state& state, dcon::nation_id n) {
	/*
	Every day, a nation must pay its creditors. It must pay national-modifier-to-loan-interest x debt-amount x interest-to-debt-holder-rate / 30
	When a nation takes a loan, the interest-to-debt-holder-rate is set at nation-taking-the-loan-technology-loan-interest-modifier + define:LOAN_BASE_INTEREST, with a minimum of 0.01.
	*/
	auto debt = state.world.nation_get_local_loan(n);
	return debt * std::max(0.01f, (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::loan_interest) + 1.0f) * state.defines.loan_base_interest) / 30.0f;
}
float max_loan(sys::state& state, dcon::nation_id n) {
	/*
	There is an income cap to how much may be borrowed, namely: define:MAX_LOAN_CAP_FROM_BANKS x (national-modifier-to-max-loan-amount + 1) x national-tax-base.
	*/
	auto mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_loan_modifier) + 1.0f);
	auto total_tax_base = state.world.nation_get_total_rich_income(n) + state.world.nation_get_total_middle_income(n) + state.world.nation_get_total_poor_income(n);
	return std::max(0.0f, (total_tax_base + state.world.nation_get_national_bank(n)) * mod);
}

int32_t most_recent_price_record_index(sys::state& state) {
	return (state.current_date.value >> 4) % price_history_length;
}
int32_t previous_price_record_index(sys::state& state) {
	return ((state.current_date.value >> 4) + price_history_length - 1) % price_history_length;
}

int32_t most_recent_gdp_record_index(sys::state& state) {
	auto date = state.current_date.to_ymd(state.start_date);
	return (date.year * 4 + date.month / 3) % gdp_history_length;
}
int32_t previous_gdp_record_index(sys::state& state) {
	auto date = state.current_date.to_ymd(state.start_date);
	return ((date.year * 4 + date.month / 3) + gdp_history_length - 1) % gdp_history_length;
}

float ideal_pound_to_real_pound(sys::state& state) {
	auto cost_of_needs = 0.f;
	uint32_t total_commodities = state.world.commodity_size();
	auto worker = state.culture_definitions.primary_factory_worker;
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
		auto price = state.world.commodity_get_median_price(c);
		auto life_base = state.world.pop_type_get_life_needs(worker, c);
		auto everyday_base = state.world.pop_type_get_everyday_needs(worker, c);
		cost_of_needs += price * (life_base + 0.1f * everyday_base);
	}
	return cost_of_needs;
}

float gdp(sys::state& state, dcon::market_id n) {
	return state.world.market_get_gdp(n);
}

float gdp(sys::state& state, dcon::nation_id n) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total = total + economy::gdp(state, market);
	});
	return total;
}

float gdp_adjusted(sys::state& state, dcon::nation_id n) {
	auto conversion = ideal_pound_to_real_pound(state);
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total = total + economy::gdp(state, market);
	});
	return total / conversion;
}

struct spending_cost {
	float administration;
	float construction;
	float other;
	float total;
};

spending_cost full_spending_cost(sys::state& state, dcon::nation_id n, float budget);
void populate_army_consumption(sys::state& state);
void populate_navy_consumption(sys::state& state);
void populate_construction_consumption(sys::state& state);

// Returns factory types for which commodity is an output good
std::vector<dcon::factory_type_id> commodity_get_factory_types_as_output(sys::state const& state, dcon::commodity_id output_good) {
	std::vector<dcon::factory_type_id> types;
	for(auto t : state.world.in_factory_type) {
		if(t.get_output() == output_good) {
			types.push_back(t);
		}
	}
	return types;
}

void initialize_artisan_distribution(sys::state& state) {
	state.world.province_resize_artisan_score(state.world.commodity_size());
	state.world.province_resize_artisan_actual_production(state.world.commodity_size());

	auto artisans = state.culture_definitions.artisans;
	auto keys = demographics::to_key(state, artisans);

	state.world.for_each_commodity([&](auto cid) {
		state.world.execute_serial_over_province([&](auto ids) {
			auto local_artisans = state.world.province_get_demographics(ids, keys);
			state.world.province_set_artisan_score(ids, cid, local_artisans);
		});
	});
}

void initialize_needs_weights(sys::state& state, dcon::market_id n) {
	auto zone = state.world.market_get_zone_from_local_market(n);
	auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_life_need(state, nation, c)) {
				state.world.market_set_life_needs_weights(n, c, 0.001f);
			}
		});
	}
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_everyday_need(state, nation, c)) {
				state.world.market_set_everyday_needs_weights(n, c, 0.001f);
			}
		});
	}
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_luxury_need(state, nation, c)) {
				state.world.market_set_luxury_needs_weights(n, c, 0.001f);
			}
		});
	}
}

float need_weight(sys::state& state, dcon::market_id n, dcon::commodity_id c, float base_wage, float base_amount) {
	auto cost_per_person = base_amount * std::max(price(state, n, c), 0.0001f) / state.defines.alice_needs_scaling_factor;
	auto wage_ratio = base_wage / cost_per_person;
	return std::max(0.f, std::min(1.f, wage_ratio * wage_ratio / 2.f - 0.1f));
	//return 1.f;
}

void rebalance_needs_weights(sys::state& state, dcon::market_id n) {
	auto zone = state.world.market_get_zone_from_local_market(n);
	auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
	auto capital = state.world.state_instance_get_capital(zone);

	auto t = state.culture_definitions.secondary_factory_worker;

	{
		auto wage = state.world.province_get_labor_price(capital, labor::no_education);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_life_need(state, nation, c)) {
				auto base_life = std::max(1.f, state.world.pop_type_get_life_needs(t, c));
				auto weight = need_weight(state, n, c, wage, base_life);
				auto& w = state.world.market_get_life_needs_weights(n, c);
				state.world.market_set_life_needs_weights(n, c, weight * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed));

				assert(std::isfinite(w));
				assert(w <= 1.f + 0.01f);
			}
		});
	}

	{
		auto wage = state.world.province_get_labor_price(capital, labor::no_education) * 2.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_everyday_need(state, nation, c)) {
				auto base_everyday = std::max(1.f, state.world.pop_type_get_everyday_needs(t, c));
				auto weight = need_weight(state, n, c, wage, base_everyday);
				auto& w = state.world.market_get_everyday_needs_weights(n, c);
				state.world.market_set_everyday_needs_weights(n, c, weight * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed));

				assert(std::isfinite(w));
				assert(w <= 1.f + 0.01f);
			}
		});
	}

	{
		auto wage = state.world.province_get_labor_price(capital, labor::no_education) * 20.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_luxury_need(state, nation, c)) {
				auto base_luxury = std::max(1.f, state.world.pop_type_get_luxury_needs(t, c));
				auto weight = need_weight(state, n, c, wage, base_luxury);
				auto& w = state.world.market_get_luxury_needs_weights(n, c);
				state.world.market_set_luxury_needs_weights(n, c, weight * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed));

				assert(std::isfinite(w));
				assert(w <= 1.f + 0.01f);
			}
		});
	}

	/*
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		if(valid_luxury_need(state, nation, c)) {
			state.world.market_set_luxury_needs_weights(n, c, 1.f);
		}
		if(valid_everyday_need(state, nation, c)) {
			state.world.market_set_everyday_needs_weights(n, c, 1.f);
		}
		if(valid_life_need(state, nation, c)) {
			state.world.market_set_life_needs_weights(n, c, 1.f);
		}
	});
	*/
}


void convert_commodities_into_ingredients(
	sys::state& state,
	std::vector<float>& buffer_commodities,
	std::vector<float>& buffer_ingredients,
	std::vector<float>& buffer_weights
) {
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		float amount = buffer_commodities[c.index()];

		if(state.world.commodity_get_rgo_amount(c) > 0.f) {
			buffer_ingredients[c.index()] += amount;
		} else {
			//calculate input vectors weights:
			std::vector<float> weights;
			float total_weight = 0.f;
			float non_zero_count = 0.f;

			state.world.for_each_factory_type([&](dcon::factory_type_id t) {
				auto o = state.world.factory_type_get_output(t);
				if(o == c) {
					auto& inputs = state.world.factory_type_get_inputs(t);

					float weight_current = 0;

					for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
						if(inputs.commodity_type[i]) {
							float weight_input = buffer_weights[inputs.commodity_type[i].index()];
							total_weight += weight_input;
							weight_current += weight_input;
						} else {
							break;
						}
					}

					if(weight_current > 0.f)
						non_zero_count++;

					weights.push_back(weight_current);
				}
			});

			if(total_weight == 0) {
				for(size_t i = 0; i < weights.size(); i++) {
					weights[i] = 1.f;
					total_weight++;
				}
			} else {
				float average_weight = total_weight / non_zero_count;
				for(size_t i = 0; i < weights.size(); i++) {
					if(weights[i] == 0.f) {
						weights[i] = average_weight;
						total_weight += average_weight;
					}
				}
			}

			//now we have weights and can use them for transformation of output into ingredients:
			size_t index = 0;

			state.world.for_each_factory_type([&](dcon::factory_type_id t) {
				auto o = state.world.factory_type_get_output(t);
				if(o == c) {
					auto& inputs = state.world.factory_type_get_inputs(t);
					float output_power = state.world.factory_type_get_output_amount(t);

					float weight_current = weights[index] / total_weight;
					index++;

					for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
						if(inputs.commodity_type[i]) {

							buffer_ingredients[inputs.commodity_type[i].index()] += inputs.commodity_amounts[i] * amount / output_power * weight_current;

							float weight_input = buffer_weights[inputs.commodity_type[i].index()];
							total_weight += weight_input;
							weight_current += weight_input;
						} else {
							break;
						}
					}
				}
			});
		}
	});
}

void presimulate(sys::state& state) {
	// economic updates without construction
#ifdef NDEBUG
	uint32_t steps = 10;
#else
	uint32_t steps = 2;
#endif
	for(uint32_t i = 0; i < steps; i++) {
		update_employment(state);
		daily_update(state, true, (float)i / (float)steps);
		ai::update_budget(state);
	}
}

bool has_building(sys::state const& state, dcon::state_instance_id si, dcon::factory_type_id fac) {
	auto sdef = state.world.state_instance_get_definition(si);
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			for(auto b : p.get_province().get_factory_location()) {
				if(b.get_factory().get_building_type() == fac)
					return true;
			}
		}
	}
	return false;
}

bool is_bankrupt_debtor_to(sys::state& state, dcon::nation_id debt_holder, dcon::nation_id debtor) {
	return state.world.nation_get_is_bankrupt(debt_holder) &&
		state.world.unilateral_relationship_get_owns_debt_of(
				state.world.get_unilateral_relationship_by_unilateral_pair(debtor, debt_holder)) > 0.1f;
}

bool nation_is_constructing_factories(sys::state& state, dcon::nation_id n) {
	auto rng = state.world.nation_get_factory_construction(n);
	return rng.begin() != rng.end();
}
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n) { // TODO - should be "good" now
	auto nation_fat = dcon::fatten(state.world, n);
	for(auto prov_owner : nation_fat.get_province_ownership()) {
		auto prov = prov_owner.get_province();
		for(auto factloc : prov.get_factory_location()) {
			auto scale = factloc.get_factory().get_primary_employment();
			if(scale < factory_closed_threshold) {
				return true;
			}
		}
	}
	return false;
}

// Check if source gives trade rights to target
dcon::unilateral_relationship_id nation_gives_free_trade_rights(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	auto sphere_A = state.world.nation_get_in_sphere_of(target);
	auto sphere_B = state.world.nation_get_in_sphere_of(source);

	auto overlord_A = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(target)
	);
	auto overlord_B = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(source)
	);

	auto market_leader_target = (overlord_A) ? overlord_A : ((sphere_A) ? sphere_A : target);
	auto market_leader_source = (overlord_B) ? overlord_B : ((sphere_B) ? sphere_B : source);

	auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(market_leader_target, market_leader_source);
	if(source_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
		// Enddt empty signalises revoken agreement
		// Enddt > cur_date signalises that the agreement can't be broken
		if(enddt) {
			return source_tariffs_rel;
		}
	}
	return dcon::unilateral_relationship_id{};
}

void initialize(sys::state& state) {
	initialize_artisan_distribution(state);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.execute_serial_over_market([&](auto markets) {
			state.world.market_set_price(markets, c, state.world.commodity_get_cost(c));
			state.world.market_set_demand(markets, c, ve::fp_vector{});
			state.world.market_set_supply(markets, c, ve::fp_vector{});
			state.world.market_set_consumption(markets, c, ve::fp_vector{});
		});

		auto fc = fatten(state.world, c);

		for(uint32_t i = 0; i < price_history_length; ++i) {
			fc.set_price_record(i, fc.get_cost());
		}
		// fc.set_global_market_pool();
	});

	services::reset_demand(state);
	services::reset_supply(state);
	services::reset_price(state);

	/*
	auto savings_buffer = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		auto ft = fatten(state.world, t);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			savings_buffer.get(t) +=
				state.world.commodity_get_is_available_from_start(c)
				?
				state.world.commodity_get_cost(c) * ft.get_life_needs(c)
				+ 0.5f * state.world.commodity_get_cost(c) * ft.get_everyday_needs(c)
				: 0.0f;
		});
		auto strata = (ft.get_strata() * 2) + 1;
		savings_buffer.get(t) *= strata;
	});
	*/


	auto expected_savings = expected_savings_per_capita;

	state.world.for_each_pop([&](dcon::pop_id p) {
		auto fp = fatten(state.world, p);
		pop_demographics::set_life_needs(state, p, 1.0f);
		pop_demographics::set_everyday_needs(state, p, 0.1f);
		pop_demographics::set_luxury_needs(state, p, 0.0f);
		fp.set_savings(fp.get_size() * expected_savings);
	});

	sanity_check(state);

	auto csize = state.world.commodity_size();

	std::vector<float> rgo_efficiency_inputs_amount;
	rgo_efficiency_inputs_amount.resize(csize + 1);
	std::vector<int> rgo_efficiency_inputs_count;
	rgo_efficiency_inputs_count.resize(csize + 1);

	state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
		auto& e_inputs = state.world.factory_type_get_efficiency_inputs(ftid);
		for(uint32_t i = 0; i < e_inputs.set_size; i++) {
			if(e_inputs.commodity_type[i]) {
				rgo_efficiency_inputs_amount[e_inputs.commodity_type[i].value] +=
					e_inputs.commodity_amounts[i]
					/ state.world.factory_type_get_base_workforce(ftid);
				rgo_efficiency_inputs_count[e_inputs.commodity_type[i].value] += 1;
			}
		}
	});

	// find the two most common efficiency inputs (usually, cement and machine parts)
	// TODO: add modding support to overwrite this generation

	int most_common_value = -1;
	int second_most_common_value = -1;
	int most_common_count = -1;
	int second_most_common_count = -1;

	for(size_t i = 0; i < rgo_efficiency_inputs_amount.size(); i++) {
		if(rgo_efficiency_inputs_count[i] >= most_common_count) {
			second_most_common_value = most_common_value;
			second_most_common_count = most_common_count;

			most_common_value = int(i);
			most_common_count = rgo_efficiency_inputs_count[i];
		}
	}

	// generate commodity set:
	economy::commodity_set base_rgo_e_inputs { };

	if(most_common_count > 0) {
		base_rgo_e_inputs.commodity_type[0] = dcon::commodity_id{ uint8_t(most_common_value - 1) };
		base_rgo_e_inputs.commodity_amounts[0] =
			rgo_efficiency_inputs_amount[most_common_value]
			/ (float)most_common_count * 0.0001f;
	}
	if(second_most_common_count > 0) {
		base_rgo_e_inputs.commodity_type[1] = dcon::commodity_id{ uint8_t(second_most_common_value - 1) };
		base_rgo_e_inputs.commodity_amounts[1] =
			rgo_efficiency_inputs_amount[second_most_common_value]
			/ (float)second_most_common_count * 0.0001f;
	}

	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		auto & data = state.world.commodity_get_rgo_efficiency_inputs(cid);
		for(uint32_t i = 0; i < base_rgo_e_inputs.set_size; i++) {
			data.commodity_amounts[i] = base_rgo_e_inputs.commodity_amounts[i];
			data.commodity_type[i] = base_rgo_e_inputs.commodity_type[i];
		}
	});

	state.world.for_each_factory([&](dcon::factory_id f) {
		auto ff = fatten(state.world, f);
		ff.set_unqualified_employment(ff.get_size() * 0.2f);
		ff.set_primary_employment(ff.get_size() * 0.2f);
	});

	// learn some weights for rgo from initial territories:
	std::vector<std::vector<float>> per_climate_distribution_buffer(state.world.modifier_size() + 1, std::vector<float>(csize + 1, 0.f));
	std::vector<std::vector<float>> per_terrain_distribution_buffer(state.world.modifier_size() + 1, std::vector<float>(csize + 1, 0.f));
	std::vector<std::vector<float>> per_continent_distribution_buffer(state.world.modifier_size() + 1, std::vector<float>(csize + 1, 0.f));

	// init the map for climates
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto fp = fatten(state.world, p);
		dcon::commodity_id main_trade_good = state.world.province_get_rgo(p);
		if(state.world.commodity_get_money_rgo(main_trade_good)) {
			return;
		}
		dcon::modifier_id climate = fp.get_climate();
		dcon::modifier_id terrain = fp.get_terrain();
		dcon::modifier_id continent = fp.get_continent();
		per_climate_distribution_buffer[climate.value][main_trade_good.value] += 1.f;
		per_terrain_distribution_buffer[terrain.value][main_trade_good.value] += 1.f;
		per_continent_distribution_buffer[continent.value][main_trade_good.value] += 1.f;
	});

	// normalisation
	for(uint32_t i = 0; i < uint32_t(state.world.modifier_size()); i++) {
		float climate_sum = 0.f;
		float terrain_sum = 0.f;
		float continent_sum = 0.f;
		for(uint32_t j = 0; j < csize; j++) {
			climate_sum += per_climate_distribution_buffer[i][j];
			terrain_sum += per_terrain_distribution_buffer[i][j];
			continent_sum += per_continent_distribution_buffer[i][j];
		}
		for(uint32_t j = 0; j < csize; j++) {
			per_climate_distribution_buffer[i][j] *= climate_sum == 0.f ? 1.f : 1.f / climate_sum;
			per_terrain_distribution_buffer[i][j] *= terrain_sum == 0.f ? 1.f : 1.f / terrain_sum;
			per_continent_distribution_buffer[i][j] *= continent_sum == 0.f ? 1.f : 1.f / continent_sum;
		}
	}

	if(state.defines.alice_rgo_generate_distribution > 0.0f) {
		province::for_each_land_province(state, [&](dcon::province_id p) {
			auto fp = fatten(state.world, p);
			//max size of exploitable land:
			auto max_rgo_size = std::ceil(100.f * state.map_state.map_data.province_area_km2[province::to_map_id(p)]);
			// currently exploited land
			float pop_amount = 0.0f;
			for(auto pt : state.world.in_pop_type) {
				if(pt == state.culture_definitions.slaves) {
					pop_amount += state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves));
				} else if(pt.get_is_paid_rgo_worker()) {
					pop_amount += state.world.province_get_demographics(p, demographics::to_key(state, pt));
				}
			}
			fp.set_rgo_base_size(max_rgo_size);

			if(state.world.province_get_rgo_was_set_during_scenario_creation(p)) {
				return;
			}

			dcon::modifier_id climate = fp.get_climate();
			dcon::modifier_id terrain = fp.get_terrain();
			dcon::modifier_id continent = fp.get_continent();

			state.world.for_each_commodity([&](dcon::commodity_id c) {
				fp.set_rgo_target_employment(c, 0.f);
			});

			static std::vector<float> true_distribution;
			true_distribution.resize(state.world.commodity_size());

			float total = 0.f;
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				float climate_d = per_climate_distribution_buffer[climate.value][c.value];
				float terrain_d = per_terrain_distribution_buffer[terrain.value][c.value];
				float continent_d = per_continent_distribution_buffer[continent.value][c.value];
				float current = (climate_d + terrain_d) * (climate_d + terrain_d) * continent_d;
				true_distribution[c.index()] = current;
				total += current;
			});

			// remove continental restriction if failed:
			if(total == 0.f) {
				state.world.for_each_commodity([&](dcon::commodity_id c) {
					float climate_d = per_climate_distribution_buffer[climate.value][c.value];
					float terrain_d = per_terrain_distribution_buffer[terrain.value][c.value];
					float current = (climate_d + terrain_d) * (climate_d + terrain_d);
					true_distribution[c.index()] = current;
					total += current;
				});
			}

			// make it into uniform distrubution on available goods then...
			if(total == 0.f) {
				state.world.for_each_commodity([&](dcon::commodity_id c) {
					if(state.world.commodity_get_money_rgo(c)) {
						return;
					}
					if(!state.world.commodity_get_is_available_from_start(c)) {
						return;
					}
					float current = 1.f;
					true_distribution[c.index()] = current;
					total += current;
				});
			}

			auto main_rgo = state.world.province_get_rgo(p);
			if(main_rgo) {
				auto main_rgo_score = total * 0.5f;
				if(total <= 0.f)
					main_rgo_score = 1.f;
				true_distribution[main_rgo.id.index()] += main_rgo_score;
				total = total + main_rgo_score;
			}

			assert(total > 0.f);

			state.world.for_each_commodity([&](dcon::commodity_id c) {
				assert(std::isfinite(total));
				// if everything had failed for some reason, then assume 0 distribution: main rgo is still active
				if(total == 0.f) {
					true_distribution[c.index()] = 0.f;
				} else {
					true_distribution[c.index()] /= total;
				}
			});

			// distribution of rgo land per good
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				assert(std::isfinite(true_distribution[c.index()]));
				auto proposed_size = (pop_amount * 20.f + state.defines.alice_base_rgo_employment_bonus) * true_distribution[c.index()];
				if(proposed_size > state.defines.alice_secondary_rgos_min_employment) {
					state.world.province_set_rgo_size(p, c,
						state.world.province_get_rgo_size(p, c) + proposed_size
					);
					state.world.province_set_rgo_potential(p, c,
						state.world.province_get_rgo_potential(p, c) + max_rgo_size * true_distribution[c.index()]
					);
					state.world.province_set_rgo_efficiency(p, c, 1.f);
					state.world.province_set_rgo_target_employment(p, c, state.world.province_get_rgo_size(p, c));
				}
			});
		});
	}

	state.world.for_each_nation([&](dcon::nation_id n) {
		auto fn = fatten(state.world, n);
		fn.set_administrative_spending(int8_t(35));
		fn.set_military_spending(int8_t(60));
		fn.set_education_spending(int8_t(100));
		fn.set_social_spending(int8_t(100));
		fn.set_land_spending(int8_t(100));
		fn.set_naval_spending(int8_t(100));
		fn.set_construction_spending(int8_t(100));
		fn.set_overseas_spending(int8_t(100));

		fn.set_poor_tax(int8_t(75));
		fn.set_middle_tax(int8_t(75));
		fn.set_rich_tax(int8_t(75));

		fn.set_spending_level(1.0f);
	});

	state.world.for_each_market([&](dcon::market_id n) {
		initialize_needs_weights(state, n);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			state.world.market_set_demand_satisfaction(n, c, 0.0f);
			state.world.market_set_supply_sold_ratio(n, c, 0.0f);
			state.world.market_set_direct_demand_satisfaction(n, c, 0.0f);
			// set domestic market pool
		});
	});

	state.world.execute_serial_over_province([&](auto ids) {
		for(int32_t i = 0; i < labor::total; i++) {
			state.world.province_set_labor_price(ids, i, 0.0001f);
		}
	});

	update_employment(state);

	populate_army_consumption(state);
	populate_navy_consumption(state);
	populate_construction_consumption(state);

	state.world.for_each_nation([&](dcon::nation_id n) {
		state.world.nation_set_stockpiles(n, money, 1000.f);
	});
}

float sphere_leader_share_factor(sys::state& state, dcon::nation_id sphere_leader, dcon::nation_id sphere_member) {
	/*
	Share factor : If the nation is a civ and is a secondary power start with define : SECOND_RANK_BASE_SHARE_FACTOR, and
	otherwise start with define : CIV_BASE_SHARE_FACTOR.Also calculate the sphere owner's foreign investment in the nation as a
	fraction of the total foreign investment in the nation (I believe that this is treated as zero if there is no foreign
	investment at all). The share factor is (1 - base share factor) x sphere owner investment fraction + base share factor. For
	uncivs, the share factor is simply equal to define:UNCIV_BASE_SHARE_FACTOR (so 1, by default). If a nation isn't in a sphere,
	we let the share factor be 0 if it needs to be used in any other calculation.
	*/
	if(state.world.nation_get_is_civilized(sphere_member)) {
		float base = state.world.nation_get_rank(sphere_member) <= state.defines.colonial_rank
			? state.defines.second_rank_base_share_factor
			: state.defines.civ_base_share_factor;
		auto const ul = state.world.get_unilateral_relationship_by_unilateral_pair(sphere_member, sphere_leader);
		float sl_investment = state.world.unilateral_relationship_get_foreign_investment(ul);
		float total_investment = nations::get_foreign_investment(state, sphere_member);
		float investment_fraction = total_investment > 0.0001f ? sl_investment / total_investment : 0.0f;
		return base + (1.0f - base) * investment_fraction;
	} else {
		return state.defines.unciv_base_share_factor;
	}
}


void update_factory_triggered_modifiers(sys::state& state) {
	state.world.for_each_factory([&](dcon::factory_id f) {
		auto fac_type = fatten(state.world, state.world.factory_get_building_type(f));
		float sum = 1.0f;
		auto prov = state.world.factory_get_province_from_factory_location(f);
		auto pstate = state.world.province_get_state_membership(prov);
		auto powner = state.world.province_get_nation_from_province_ownership(prov);

		if(powner && pstate) {
			if(auto mod_a = fac_type.get_bonus_1_trigger();
					mod_a && trigger::evaluate(state, mod_a, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum -= fac_type.get_bonus_1_amount();
			}
			if(auto mod_b = fac_type.get_bonus_2_trigger();
					mod_b && trigger::evaluate(state, mod_b, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum -= fac_type.get_bonus_2_amount();
			}
			if(auto mod_c = fac_type.get_bonus_3_trigger();
					mod_c && trigger::evaluate(state, mod_c, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum -= fac_type.get_bonus_3_amount();
			}
		}

		state.world.factory_set_triggered_modifiers(f, sum);
	});
}

float subsistence_size(sys::state const& state, dcon::province_id p) {
	auto rgo_ownership = state.world.province_get_landowners_share(p) + state.world.province_get_capitalists_share(p);
	return state.world.province_get_rgo_base_size(p) * (1.f - rgo_ownership);
}

float subsistence_max_pseudoemployment(sys::state& state, dcon::province_id p) {
	return subsistence_size(state, p) * 1.1f;
}

void update_local_subsistence_factor(sys::state& state) {
	state.world.execute_serial_over_province([&](auto ids) {
		auto quality = (ve::to_float(state.world.province_get_life_rating(ids)) - 10.f) / 10.f;
		quality = ve::max(quality, 0.f) + 0.01f;
		auto score = (subsistence_factor * quality) + subsistence_score_life * 0.9f;
		state.world.province_set_subsistence_score(ids, score);
	});
}


void update_land_ownership(sys::state& state) {
	state.world.execute_serial_over_province([&](auto ids) {
		auto local_states = state.world.province_get_state_membership(ids);
		auto weight_population =
			state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.farmers))
			+ state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.laborers));
		auto weight_capitalists = weight_population / 50.f
			+ state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.capitalists)) * 200.f;
		auto weight_aristocracy = weight_population / 50.f
			+ state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.aristocrat)) * 200.f
			+ state.world.state_instance_get_demographics(local_states, demographics::to_key(state, state.culture_definitions.slaves));
		auto total = weight_aristocracy + weight_capitalists + weight_population + 1.0f;
		state.world.province_set_landowners_share(ids, weight_aristocracy / total);
		state.world.province_set_capitalists_share(ids, weight_capitalists / total);
	});
}

int32_t factory_priority(sys::state const& state, dcon::factory_id f) {
	return (state.world.factory_get_priority_low(f) ? 1 : 0) + (state.world.factory_get_priority_high(f) ? 2 : 0);
}
void set_factory_priority(sys::state& state, dcon::factory_id f, int32_t priority) {
	state.world.factory_set_priority_high(f, priority >= 2);
	state.world.factory_set_priority_low(f, (priority & 1) != 0);
}
bool factory_is_profitable(sys::state const& state, dcon::factory_id f) {
	return state.world.factory_get_unprofitable(f) == false || state.world.factory_get_subsidized(f);
}

struct commodity_profit_holder {
	float profit = 0.0f;
	dcon::commodity_id c;
};

float factory_unqualified_employment(sys::state const& state, dcon::factory_id f) {
	auto pid = state.world.factory_get_province_from_factory_location(f);
	auto employment = state.world.factory_get_unqualified_employment(f)
		* state.world.province_get_labor_demand_satisfaction(pid, labor::no_education);
	return employment;
}
float factory_primary_employment(sys::state const& state, dcon::factory_id f) {
	auto pid = state.world.factory_get_province_from_factory_location(f);
	auto primary_employment = state.world.factory_get_primary_employment(f)
		* state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education);
	return primary_employment;
}
float factory_secondary_employment(sys::state const& state, dcon::factory_id f) {
	auto pid = state.world.factory_get_province_from_factory_location(f);
	auto secondary_employment = state.world.factory_get_secondary_employment(f)
		* state.world.province_get_labor_demand_satisfaction(pid, labor::high_education);
	return secondary_employment;
}

float factory_total_employment(sys::state const& state, dcon::factory_id f) {
	auto pid = state.world.factory_get_province_from_factory_location(f);
	return (
		state.world.factory_get_unqualified_employment(f)
		* state.world.province_get_labor_demand_satisfaction(pid, labor::no_education)
		+
		state.world.factory_get_primary_employment(f)
		* state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education)
		+
		state.world.factory_get_secondary_employment(f)
		* state.world.province_get_labor_demand_satisfaction(pid, labor::high_education)
	);
}





void update_pops_employment(sys::state& state) {
	state.world.execute_serial_over_province([&](auto pids) {
		auto nation = state.world.province_get_nation_from_province_ownership(pids);
		// calculate total pops employment:

		auto no_education = state.world.province_get_labor_supply_sold(pids, labor::no_education);
		auto basic_education = state.world.province_get_labor_supply_sold(pids, labor::basic_education); // craftsmen
		auto high_education = state.world.province_get_labor_supply_sold(pids, labor::high_education); // clerks, clergy and bureaucrats
		auto guild_education = state.world.province_get_labor_supply_sold(pids, labor::guild_education); // artisans
		auto high_education_and_accepted = state.world.province_get_labor_supply_sold(pids, labor::high_education_and_accepted); // clerks, clergy and bureaucrats of accepted culture

		auto rgo_workers_employment =
			state.world.province_get_pop_labor_distribution(pids, pop_labor::rgo_worker_no_education)
			* no_education;

		auto primary_workers_employment =
			state.world.province_get_pop_labor_distribution(pids, pop_labor::primary_no_education)
			* no_education
			+
			state.world.province_get_pop_labor_distribution(pids, pop_labor::primary_basic_education)
			* basic_education;

		auto high_not_accepted_workers_employment =
			state.world.province_get_pop_labor_distribution(pids, pop_labor::high_education_not_accepted_no_education)
			* no_education
			+
			state.world.province_get_pop_labor_distribution(pids, pop_labor::high_education_not_accepted_basic_education)
			* basic_education
			+
			state.world.province_get_pop_labor_distribution(pids, pop_labor::high_education_not_accepted_high_education)
			* high_education;

		auto high_accepted_workers_employment =
			state.world.province_get_pop_labor_distribution(pids, pop_labor::high_education_accepted_no_education)
			* no_education
			+
			state.world.province_get_pop_labor_distribution(pids, pop_labor::high_education_accepted_basic_education)
			* basic_education
			+
			state.world.province_get_pop_labor_distribution(pids, pop_labor::high_education_accepted_high_education)
			* high_education
			+
			state.world.province_get_pop_labor_distribution(pids, pop_labor::high_education_accepted_high_education_accepted)
			* high_education_and_accepted;


		ve::apply([&](dcon::province_id p, dcon::nation_id n, float rgo, float primary, float high, float high_accepted) {
			auto subsistence_employment = 0.f;
			for(auto pop_location : state.world.province_get_pop_location(p)) {
				auto pop = pop_location.get_pop();

				auto accepted = state.world.nation_get_accepted_cultures(n, pop.get_culture())
					|| state.world.nation_get_primary_culture(n) == pop.get_culture();

				if(pop.get_poptype() == state.culture_definitions.slaves) {
					pop_demographics::set_raw_employment(state, pop, rgo);
					subsistence_employment += pop.get_size() * (1.f - rgo);
				} else if(pop.get_poptype().get_is_paid_rgo_worker()) {
					pop_demographics::set_raw_employment(state, pop, rgo);
					subsistence_employment += pop.get_size() * (1.f - rgo);
				} else if(pop.get_poptype() == state.culture_definitions.primary_factory_worker) {
					pop_demographics::set_raw_employment(state, pop, primary);
					subsistence_employment += pop.get_size() * (1.f - primary);
				} else if(pop.get_poptype() == state.culture_definitions.secondary_factory_worker) {
					if(accepted) {
						pop_demographics::set_raw_employment(state, pop, high_accepted);
						subsistence_employment += pop.get_size() * (1.f - high_accepted);
					} else {
						pop_demographics::set_raw_employment(state, pop, high);
						subsistence_employment += pop.get_size() * (1.f - high);
					}
				} else if(pop.get_poptype() == state.culture_definitions.bureaucrat) {
					if(accepted) {
						pop_demographics::set_raw_employment(state, pop, high_accepted);
						subsistence_employment += pop.get_size() * (1.f - high_accepted);
					} else {
						pop_demographics::set_raw_employment(state, pop, high);
						subsistence_employment += pop.get_size() * (1.f - high);
					}
				} else if(pop.get_poptype() == state.culture_definitions.clergy) {
					if(accepted) {
						pop_demographics::set_raw_employment(state, pop, high_accepted);
						subsistence_employment += pop.get_size() * (1.f - high_accepted);
					} else {
						pop_demographics::set_raw_employment(state, pop, high);
						subsistence_employment += pop.get_size() * (1.f - high);
					}
				}
			}
			state.world.province_set_subsistence_employment(
				p,
				std::max(
					0.f,
					std::min(
						subsistence_employment,
						subsistence_max_pseudoemployment(state, p)
					)
				)
			);
		}, pids, nation, rgo_workers_employment, primary_workers_employment, high_not_accepted_workers_employment, high_accepted_workers_employment);
	});
}

float convex_function(float x) {
	return 1.f - (1.f - x) * (1.f - x);
}

void populate_army_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_army_demand(ids, cid, 0.0f);
		});
	}

	state.world.for_each_regiment([&](dcon::regiment_id r) {
		auto reg = fatten(state.world, r);
		auto type = state.world.regiment_get_type(r);
		auto owner = reg.get_army_from_army_membership().get_controller_from_army_control();
		auto pop = reg.get_pop_from_regiment_source();
		auto location = pop.get_pop_location().get_province().get_state_membership();
		auto market = location.get_market_from_local_market();
		auto strength = reg.get_strength();

		if(owner && type) {
			auto o_sc_mod = std::max(
				0.01f,
				state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption)
				+ 1.0f
			);
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					// Day-to-day consumption
					// Strength under 100% reduces unit supply consumption
					auto& curr_demand = state.world.market_get_army_demand(market, supply_cost.commodity_type[i]);
					state.world.market_set_army_demand(market, supply_cost.commodity_type[i], curr_demand + supply_cost.commodity_amounts[i]
						* state.world.nation_get_unit_stats(owner, type).supply_consumption
						* o_sc_mod * strength);

				} else {
					break;
				}
			}
			auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto reinforcement = military::unit_calculate_reinforcement<military::reinforcement_estimation_type::full_supplies>(state, reg);
					if(reinforcement > 0) {
						// Regiment needs reinforcement - add extra consumption. Every 1% of reinforcement demands 1% of unit cost. Divide to spread the demand out over the month
						auto& curr_demand = state.world.market_get_army_demand(market, build_cost.commodity_type[i]);
						state.world.market_set_army_demand(market, build_cost.commodity_type[i], curr_demand + (build_cost.commodity_amounts[i] * reinforcement) / unit_reinforcement_demand_divisor);
					}
				} else {
					break;
				}
			}
		}
	});
}

void populate_navy_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_navy_demand(ids, cid, 0.0f);
		});
	}

	state.world.for_each_ship([&](dcon::ship_id r) {
		auto shp = fatten(state.world, r);
		auto type = state.world.ship_get_type(r);
		auto owner = shp.get_navy_from_navy_membership().get_controller_from_navy_control();
		auto market = owner.get_capital().get_state_membership().get_market_from_local_market();

		if(owner && type) {
			auto o_sc_mod = std::max(
				0.01f,
				state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption)
				+ 1.0f
			);

			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					auto& curr_demand = state.world.market_get_navy_demand(market, supply_cost.commodity_type[i]);
					state.world.market_set_navy_demand(market, supply_cost.commodity_type[i], curr_demand + supply_cost.commodity_amounts[i]
						* state.world.nation_get_unit_stats(owner, type).supply_consumption
						* o_sc_mod);

				} else {
					break;
				}
			}

			auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto reinforcement = military::unit_calculate_reinforcement<military::reinforcement_estimation_type::full_supplies>(state, shp);
					if(reinforcement > 0) {
						// Ship needs repair - add extra consumption. Every 1% of reinforcement demands 1% of unit cost
						// add only a fraction of the build cost per day, to spread it out over the month
						auto& curr_demand = state.world.market_get_navy_demand(market, build_cost.commodity_type[i]);
						state.world.market_set_navy_demand(market, build_cost.commodity_type[i], curr_demand + (build_cost.commodity_amounts[i] * reinforcement) / unit_reinforcement_demand_divisor);
					}
				} else {
					break;
				}
			}
		}
	});
}


spending_cost full_spending_cost(sys::state& state, dcon::nation_id n, float base_budget) {
	spending_cost costs = {
		.administration = 0.f,
		.construction = 0.f,
		.other = 0.f,
		.total = 0.f,
	};

	float total = 0.0f;
	float military_total = 0.f;
	uint32_t total_commodities = state.world.commodity_size();

	float construction_budget =
		std::max(
			0.f,
			float(state.world.nation_get_construction_spending(n))
			/ 100.0f
			* base_budget
		);

	float l_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float n_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float o_spending = float(state.world.nation_get_overseas_spending(n)) / 100.f;

	float total_construction_costs = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto v = state.world.market_get_army_demand(local_market, cid)
				* l_spending
				* price(state, local_market, cid);
			assert(std::isfinite(v) && v >= 0.0f);
			total += v;
			military_total += v;
		}
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto v = state.world.market_get_navy_demand(local_market, cid)
				* n_spending * price(state, local_market, cid);
			assert(std::isfinite(v) && v >= 0.0f);
			total += v;
			military_total += v;
		}
		assert(std::isfinite(total) && total >= 0.0f);
		state.world.nation_set_maximum_military_costs(n, military_total);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto demand_const = state.world.market_get_construction_demand(local_market, cid);
			auto c_price = price(state, local_market, cid);

			total_construction_costs += demand_const * c_price;
		}
	});

	costs.construction = std::min(construction_budget, total_construction_costs);
	total += costs.construction;


	auto capital_state = state.world.province_get_state_membership(state.world.nation_get_capital(n));
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			total += difference * price(state, capital_market, cid);
		}
	}
	assert(std::isfinite(total) && total >= 0.0f);

	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || state.world.nation_get_unlocked_commodities(n, cid))) {
				total += overseas_factor * price(state, capital_market, cid) * o_spending;
			}
		}
	}


	assert(std::isfinite(total) && total >= 0.0f);


	// wages to employed:
	auto const admin_spending = full_spendings_administration(state, n, base_budget);
	costs.administration = admin_spending;
	total += admin_spending;

	assert(std::isfinite(total) && total >= 0.0f);

	// direct payments to pops
	auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
	auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);
	auto capitalists = state.world.nation_get_demographics(n, capitalists_key);
	auto aristocrats = state.world.nation_get_demographics(n, aristocracy_key);

	if(capitalists + aristocrats > 0) {
		total += base_budget * float(state.world.nation_get_domestic_investment_spending(n)) / 100.f;
	}

	auto const m_spending = float(state.world.nation_get_military_spending(n)) * float(state.world.nation_get_military_spending(n)) / 100.0f / 100.f;

	auto const p_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level));
	auto const unemp_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit));

	if(p_level + unemp_level > 0.f) {
		total += base_budget * float(state.world.nation_get_social_spending(n)) / 100.0f;
	}

	total += base_budget * float(state.world.nation_get_education_spending(n)) / 100.0f;

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			auto key = demographics::to_key(state, pt);
			auto adj_pop_of_type =
				state.world.state_instance_get_demographics(local_state, key)
				/ state.defines.alice_needs_scaling_factor;

			if(adj_pop_of_type <= 0)
				return;

			auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
			if(ln_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt) * payouts_spending_multiplier;
			}

			auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
			if(en_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt) * payouts_spending_multiplier;
			}

			auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
			if(lx_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt) * payouts_spending_multiplier;
			}

			assert(std::isfinite(total) && total >= 0.0f);
		});
	});

	assert(std::isfinite(total) && total >= 0.0f);

	costs.total = total;
	costs.other = total - costs.construction - costs.administration;

	return costs;
}

float estimate_stockpile_filling_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();

	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto market = state.world.state_instance_get_market_from_local_market(capital_state);

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference =
			state.world.nation_get_stockpile_targets(n, cid)
			- state.world.nation_get_stockpiles(n, cid);

		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			total +=
				difference
				* price(state, market, cid)
				* state.world.market_get_demand_satisfaction(market, cid);
		}
	}

	return total;
}

float estimate_overseas_penalty_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto market = state.world.state_instance_get_market_from_local_market(capital_state);

	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	uint32_t total_commodities = state.world.commodity_size();

	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || state.world.nation_get_unlocked_commodities(n, cid))) {
				total +=
					overseas_factor
					* price(state, market, cid)
					* state.world.market_get_demand_satisfaction(market, cid);
			}
		}
	}

	return total;
}

float full_private_investment_cost(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		uint32_t total_commodities = state.world.commodity_size();
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			total +=
				state.world.market_get_private_construction_demand(market, cid)
				* price(state, market, cid);
		}
	});
	return total;
}

void update_private_consumption(sys::state& state, dcon::nation_id n, float private_investment_scale) {
	uint32_t total_commodities = state.world.commodity_size();
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			register_demand(
				state,
				market,
				cid,
				state.world.market_get_private_construction_demand(market, cid)
				* private_investment_scale, economy_reason::construction);
		}
	});
}

void update_national_consumption(sys::state& state, dcon::nation_id n, float spending_scale, float base_budget) {
	uint32_t total_commodities = state.world.commodity_size();
	float l_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float n_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float o_spending = float(state.world.nation_get_overseas_spending(n)) / 100.0f;

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto sat = state.world.market_get_demand_satisfaction(market, cid);
			auto sat_importance = std::min(1.f, 1.f / (price(state, market, cid) + 0.001f));
			auto sat_coefficient = (sat_importance + (1.f - sat_importance) * sat);

			register_demand(
				state,
				market,
				cid,
				state.world.market_get_army_demand(market, cid)
				* l_spending
				* spending_scale
				* sat_coefficient
				, economy_reason::nation
			);
			register_demand(
				state,
				market,
				cid,
				state.world.market_get_navy_demand(market, cid)
				* n_spending
				* spending_scale
				* sat_coefficient
				, economy_reason::nation
			);
		}
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			register_demand(
				state,
				market,
				cid,
				state.world.market_get_construction_demand(market, cid)
				* spending_scale,
				economy_reason::construction
			);
		}
	});

	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto market = state.world.state_instance_get_market_from_local_market(capital_state);

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			auto sat = state.world.market_get_demand_satisfaction(market, cid);
			auto sat_importance = std::min(1.f, 1.f / (price(state, market, cid) + 0.001f));
			auto sat_coefficient = (sat_importance + (1.f - sat_importance) * sat);
			register_demand(
				state,
				market,
				cid,
				difference
				* spending_scale
				* sat_coefficient,
				economy_reason::stockpile
			);
		}
	}
	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	if(overseas_factor > 0.f) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || state.world.nation_get_unlocked_commodities(n, cid))) {
				auto sat = state.world.market_get_demand_satisfaction(market, cid);
				auto sat_importance = std::min(1.f, 1.f / (price(state, market, cid) + 0.001f));
				auto sat_coefficient = (sat_importance + (1.f - sat_importance) * sat);

				register_demand(
					state,
					market,
					cid,
					overseas_factor
					* spending_scale
					* o_spending
					* sat_coefficient,
					economy_reason::overseas_penalty
				);
			}
		}
	}
}

// ### Private Investment ###

const inline float courage = 1.0f;
const inline float days_prepaid = 100.f;

/* Returns number of initiated projects */
std::vector<full_construction_factory> estimate_private_investment_upgrade(sys::state& state, dcon::nation_id nid, float est_private_const_spending) {
	std::vector<full_construction_factory> res;
	auto n = dcon::fatten(state.world, nid);
	auto nation_rules = n.get_combined_issue_rules();

	if(!n.get_is_civilized()) {
		return res;
	}
	if((nation_rules & issue_rule::pop_expand_factory) == 0) {
		return res;
	}

	// check if current projects are already too expensive for capitalists to manage
	float total_cost = est_private_const_spending * days_prepaid * 40.f;
	float total_cost_added = 0.f;
	float current_inv = n.get_private_investment();

	if(current_inv * courage < total_cost + total_cost_added) {
		return res;
	}
	if(current_inv <= total_cost) {
		return res;
	}

	static std::vector<dcon::factory_type_id> desired_types;
	desired_types.clear();

	static std::vector<dcon::province_id> provinces_in_order;
	provinces_in_order.clear();
	for(auto si : n.get_province_ownership()) {
		if(si.get_province().get_state_membership().get_capital().get_is_colonial() == false) {
			provinces_in_order.push_back(si.get_province().id);
		}
	}

	std::sort(
		provinces_in_order.begin(),
		provinces_in_order.end(),
		[&](dcon::province_id a, dcon::province_id b
			) {
				auto a_pop = state.world.province_get_demographics(a, demographics::total);
				auto b_pop = state.world.province_get_demographics(b, demographics::total);
				if(a_pop != b_pop)
					return a_pop > b_pop;
				return a.index() < b.index(); // force total ordering
	});

	//upgrade all good targets!!!
	//upgrading only one per run is too slow and leads to massive unemployment!!!

	for(auto s : provinces_in_order) {
		auto sid = state.world.province_get_state_membership(s);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		float profit = 0.0f;
		dcon::factory_id selected_factory;

		// is there an upgrade target ?
		for(auto f : state.world.province_get_factory_location(s)) {
			if(
				factory_total_employment_score(state, f.get_factory()) >= 0.9f) {
				auto type = f.get_factory().get_building_type();
				auto ug_in_progress = false;
				for(auto c : state.world.province_get_factory_construction(s)) {
					if(c.get_type() == type) {
						ug_in_progress = true;
						break;
					}
				}

				if(ug_in_progress) {
					continue;
				}

				if(auto new_p = explain_last_factory_profit(state, f.get_factory()).profit / (f.get_factory().get_size() + 1);
					new_p > profit
				) {
					profit = new_p;
					selected_factory = f.get_factory();
				}
			}
		}
		if(selected_factory && profit > 0.f) {
			auto ft = state.world.factory_get_building_type(selected_factory);
			auto time = factory_building_construction_time(state, ft, true);
			auto cm = factory_build_cost_multiplier(state, nid, state.world.factory_get_province_from_factory_location(selected_factory), true);
			auto& costs = state.world.factory_type_get_construction_costs(ft);

			float added_cost = 0.0f;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(costs.commodity_type[i]) {
					added_cost +=
						costs.commodity_amounts[i]
						* price(state, market, costs.commodity_type[i])
						* cm
						/ float(time) ;
				} else {
					break;
				}
			}
			total_cost_added += added_cost * days_prepaid;

			if(current_inv * courage < total_cost + total_cost_added) {
				break;
			}
			res.push_back({ added_cost, n, s, true, true, state.world.factory_get_building_type(selected_factory) });
		}
	}

	return res;
}

/* Returns number of initiated projects */
std::vector<full_construction_factory> estimate_private_investment_construct(sys::state& state, dcon::nation_id nid, bool craved, float est_private_const_spending, bool& potential_target_exists) {
	std::vector<full_construction_factory> res;

	auto n = dcon::fatten(state.world, nid);
	auto nation_rules = n.get_combined_issue_rules();

	if(!n.get_is_civilized()) {
		return res;
	}
	if((nation_rules & issue_rule::pop_build_factory) == 0) {
		return res;
	}

	// check if current projects are already too expensive for capitalists to manage
	float total_cost = est_private_const_spending * days_prepaid * 40.f;
	float total_cost_added = 0.f;
	float current_inv = n.get_private_investment();

	if(current_inv * courage < total_cost + total_cost_added) {
		return res;
	}

	static std::vector<dcon::factory_type_id> desired_types;
	desired_types.clear();

	static std::vector<dcon::province_id> provinces_in_order;
	provinces_in_order.clear();
	for(auto si : n.get_province_ownership()) {
		if(si.get_province().get_state_membership().get_capital().get_is_colonial() == false) {
			provinces_in_order.push_back(si.get_province().id);
		}
	}

	std::sort(
		provinces_in_order.begin(),
		provinces_in_order.end(),
		[&](dcon::province_id a, dcon::province_id b
			) {
				auto a_pop = state.world.province_get_demographics(a, demographics::total);
				auto b_pop = state.world.province_get_demographics(b, demographics::total);
				if(a_pop != b_pop)
					return a_pop > b_pop;
				return a.index() < b.index(); // force total ordering
	});

	for(auto s : provinces_in_order) {
		auto existing_constructions = state.world.province_get_factory_construction(s);
		if(existing_constructions.begin() != existing_constructions.end())
			continue; // already building

		int32_t num_factories = province_factory_count(state, s);

		if(num_factories >= int32_t(state.defines.factories_per_state)) {
			continue;
		}

		auto sid = state.world.province_get_state_membership(s);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		// randomly try a valid (check coastal, unlocked, non existing) factory
		desired_types.clear();
		if(craved) {
			ai::get_craved_factory_types(state, n, market, s, desired_types, true);
		} else {
			ai::get_desired_factory_types(state, n, market, s, desired_types, true);
		}

		if(desired_types.empty()) {
			continue;
		}

		auto selected = desired_types[
			rng::reduce(uint32_t(rng::get_random(state, uint32_t((n.id.index() << 6) ^ s.index()))), uint32_t(desired_types.size()))
		];

		if(
			state.world.factory_type_get_is_coastal(selected)
			&& !state.world.province_get_port_to(s)
		)
			continue;

		bool already_in_progress = [&]() {
			for(auto p : state.world.province_get_factory_construction(s)) {
				if(p.get_type() == selected)
					return true;
			}
			return false;
			}();

		if(already_in_progress)
			continue;


		bool present_in_location = false;
		for(auto fac : state.world.province_get_factory_location(s)) {
			auto type = fac.get_factory().get_building_type();
			if(selected == type) {
				present_in_location = true;
				break;
			}
		}

		if(present_in_location) {
			continue;
		}

		potential_target_exists = true;

		auto& costs = state.world.factory_type_get_construction_costs(selected);
		auto time = factory_building_construction_time(state, selected, false);

		float added_cost = 0.0f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(costs.commodity_type[i]) {
				added_cost +=
					costs.commodity_amounts[i]
					* price(state, market, costs.commodity_type[i])
					/ float(time);
			} else {
				break;
			}
		}
		if((total_cost + total_cost_added > 0.f) && (current_inv * courage < total_cost + total_cost_added + added_cost * days_prepaid)) {
			break;
		}
		total_cost_added += added_cost * days_prepaid;
		res.push_back(economy::full_construction_factory{ added_cost, n, s, true, false, selected });
	}

	return res;
}

std::vector<full_construction_province> estimate_private_investment_province(sys::state& state, dcon::nation_id nid, float est_private_const_spending) {
	auto n = dcon::fatten(state.world, nid);
	auto nation_rules = n.get_combined_issue_rules();

	// check if current projects are already too expensive for capitalists to manage
	float total_cost = est_private_const_spending * days_prepaid * 40.f;
	float total_cost_added = 0.f;
	float current_inv = n.get_private_investment();

	std::vector<full_construction_province> res;

	if(current_inv <= total_cost) {
		return res;
	}

	if(!n.get_is_civilized()) {
		return res;
	}

	if((nation_rules & issue_rule::pop_build_factory) == 0) {
		return res;
	}

	static std::vector<std::pair<dcon::province_id, float>> provinces_in_order;
	provinces_in_order.clear();
	for(auto si : n.get_state_ownership()) {
		if(si.get_state().get_capital().get_is_colonial() == false) {
			auto s = si.get_state().id;
			auto d = state.world.state_instance_get_definition(s);
			float num_factories = 0;
			for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
				if(province::generic_can_build_railroads(state, p.get_province(), n) &&
						p.get_province().get_nation_from_province_ownership() == n) {
					for(auto f : p.get_province().get_factory_location())
						num_factories += get_factory_level(state, f.get_factory());
					provinces_in_order.emplace_back(p.get_province().id, num_factories);
				}
			}
			// The state's number of factories is intentionally given
			// to all the provinces within the state so the
			// railroads aren't just built on a single province within a state
			for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
				if(province::generic_can_build_railroads(state, p.get_province(), n) &&
						p.get_province().get_nation_from_province_ownership() == n)
					provinces_in_order.emplace_back(p.get_province().id, num_factories);
			}
		}
	}
	if(!provinces_in_order.empty()) {
		std::pair<dcon::province_id, float> best_p = provinces_in_order[0];
		for(auto e : provinces_in_order)
			if(e.second > best_p.second)
				best_p = e;

		auto sid = state.world.province_get_state_membership(best_p.first);
		auto market = state.world.state_instance_get_market_from_local_market(sid);

		auto costs = state.economy_definitions.building_definitions[int32_t(province_building_type::railroad)].cost;
		auto time = state.economy_definitions.building_definitions[int32_t(province_building_type::railroad)].time;
		float added_cost = 0.0f;

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(costs.commodity_type[i]) {
				added_cost +=
					costs.commodity_amounts[i]
					* price(state, market, costs.commodity_type[i])
					/ float(time);
			} else {
				break;
			}
		}

		total_cost_added += added_cost * days_prepaid;
		if(n.get_private_investment() * courage < total_cost + total_cost_added) {
			return res;
		}

		res.push_back({ added_cost, n, best_p.first , true, province_building_type::railroad });
	}

	return res;
}

void run_private_investment(sys::state& state) {
	// make new investments
	for(auto n : state.world.in_nation) {
		auto nation_rules = n.get_combined_issue_rules();

		if(n.get_owned_province_count() > 0 && n.get_is_civilized() && ((nation_rules & issue_rule::pop_build_factory) != 0 || (nation_rules & issue_rule::pop_expand_factory) != 0)) {
			float est_private_const_spending = estimate_private_construction_spendings(state, n);
			bool stop = false;
			auto craved_constructions = estimate_private_investment_construct(state, n, true, est_private_const_spending, stop);
			for(auto const& r : craved_constructions) {
				if(economy::do_resource_potentials_allow_construction(state, r.nation, r.province, r.type)) {
					auto new_up = fatten(
					state.world,
					state.world.force_create_factory_construction(r.province, r.nation)
					);

					new_up.set_is_pop_project(r.is_pop_project);
					new_up.set_is_upgrade(r.is_upgrade);
					new_up.set_type(r.type);
					est_private_const_spending += r.cost;
				}
			}

			if(stop) {
				return;
			}

			/*
			auto upgrades = estimate_private_investment_upgrade(state, n, est_private_const_spending);

			for(auto const& r : upgrades) {
				auto new_up = fatten(
				state.world,
				state.world.force_create_factory_construction(r.province, r.nation)
				);

				new_up.set_is_pop_project(r.is_pop_project);
				new_up.set_is_upgrade(r.is_upgrade);
				new_up.set_type(r.type);
				est_private_const_spending += r.cost;
			}
			*/

			auto constructions = estimate_private_investment_construct(state, n, false, est_private_const_spending, stop);

			for(auto const& r : constructions) {
				if(economy::do_resource_potentials_allow_construction(state, r.nation, r.province, r.type)) {
					auto new_up = fatten(
					state.world,
					state.world.force_create_factory_construction(r.province, r.nation)
					);

					new_up.set_is_pop_project(r.is_pop_project);
					new_up.set_is_upgrade(r.is_upgrade);
					new_up.set_type(r.type);
					est_private_const_spending += r.cost;
				}
			}

			if(stop) {
				return;
			}

			auto province_constr = estimate_private_investment_province(state, n, est_private_const_spending);

			for(auto const& r : province_constr) {
				auto new_rr = fatten(
					state.world,
					state.world.force_create_province_building_construction(r.province, r.nation)
				);
				new_rr.set_is_pop_project(r.is_pop_project);
				new_rr.set_type(uint8_t(r.type));
				est_private_const_spending += r.cost;
			}

			// If nowhere to invest
			if(est_private_const_spending < 1.f && craved_constructions.size() == 0 && constructions.size() == 0 && province_constr.size() == 0) {
				// If it's an overlord - prioritize distributing some private invesmtent to subjects
				// If it's a subject - transfer private investment to overlord
				auto rel = state.world.nation_get_overlord_as_subject(n);
				auto overlord = state.world.overlord_get_ruler(rel);

				auto amt = state.world.nation_get_private_investment(n) * state.defines.alice_privateinvestment_subject_transfer / 100.f;
				state.world.nation_set_private_investment(n, state.world.nation_get_private_investment(n) - amt);

				auto subjects = nations::nation_get_subjects(state, n);
				if(subjects.size() > 0) {
					auto part = amt / subjects.size();
					for(auto s : subjects) {
						state.world.nation_set_private_investment(s, state.world.nation_get_private_investment(s) + part);
					}
				} else if(overlord) {
					state.world.nation_set_private_investment(overlord, state.world.nation_get_private_investment(overlord) + amt);
				}
			}
		} else { // private investment not allowed
			state.world.nation_set_private_investment(n, 0.0f);
		}
	}


}

void daily_update(sys::state& state, bool presimulation, float presimulation_stage) {
	sanity_check(state);

	/* initialization parallel block */

	concurrency::parallel_for(0, 8, [&](int32_t index) {
		switch(index) {
		case 0:
			populate_navy_consumption(state);
			break;
		case 1:
			populate_private_construction_consumption(state);
			break;
		case 2:
			update_factory_triggered_modifiers(state);
			break;
		case 3:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_everyday_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 4:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_luxury_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 5:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_life_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 6:
			state.world.execute_serial_over_nation([&](auto ids) {
				state.world.nation_set_subsidies_spending(ids, 0.0f);
			});
			break;
		case 7:
			state.world.execute_serial_over_nation([&](auto ids) {
				auto treasury = state.world.nation_get_stockpiles(ids, economy::money);
				state.world.nation_set_last_treasury(ids, treasury);
			});
			break;
		}
	});

	populate_army_consumption(state);

	populate_construction_consumption(state);

	sanity_check(state);

	/* end initialization parallel block */

	uint32_t total_commodities = state.world.commodity_size();

	/*
		update scoring for provinces
	*/

	update_land_ownership(state);
	update_local_subsistence_factor(state);

	sanity_check(state);

	// update trade volume based on potential profits right at the start
	// we can't put it between demand and supply generation!
	update_trade_routes_volume(state);

	sanity_check(state);

	static ve::vectorizable_buffer<float, dcon::nation_id> invention_count = state.world.nation_make_vectorizable_float_buffer();
	state.world.execute_serial_over_nation([&](auto nations) {
		invention_count.set(nations, 0.f);
	});

	sanity_check(state);

	state.world.for_each_invention([&](auto iid) {
		state.world.execute_serial_over_nation([&](auto nations) {
			auto count =
				invention_count.get(nations)
				+ ve::select(state.world.nation_get_active_inventions(nations, iid), ve::fp_vector(1.0f), ve::fp_vector(0.0f));
			invention_count.set(nations, count);
		});
	});

	state.world.execute_parallel_over_market([&](auto markets) {
		// reset gdp
		state.world.market_set_gdp(markets, 0.f);

		auto states = state.world.market_get_zone_from_local_market(markets);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);

		auto invention_factor = invention_count.get(nations) * state.defines.invention_impact_on_demand + 1.0f;

		// STEP 0: gather total demand costs:
		/*
		- Each pop strata and needs type has its own demand modifier, calculated as follows:
		- (national-modifier-to-goods-demand + define:BASE_GOODS_DEMAND) x (national-modifier-to-specific-strata-and-needs-type + 1) x
		(define:INVENTION_IMPACT_ON_DEMAND x number-of-unlocked-inventions + 1, but for non-life-needs only)
		- Each needs demand is also multiplied by  2 - the nation's administrative efficiency if the pop has education / admin /
		military income for that need category
		- We calculate an adjusted pop-size as (0.5 + pop-consciousness / define:PDEF_BASE_CON) x (for non-colonial pops: 1 +
		national-plurality (as a fraction of 100)) x pop-size
		*/

		uint32_t total_commodities = state.world.commodity_size();

		// poor strata update

		{
			uint8_t strata_filter = 0;
			auto offset_life = sys::national_mod_offsets::poor_life_needs;
			auto offset_everyday = sys::national_mod_offsets::poor_everyday_needs;
			auto offset_luxury = sys::national_mod_offsets::poor_luxury_needs;

			auto life_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_life) + 1.0f)
				* state.defines.alice_lf_needs_scale;
			auto everyday_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_everyday) + 1.0f)
				* state.defines.alice_ev_needs_scale;
			auto luxury_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_luxury) + 1.0f)
				* state.defines.alice_lx_needs_scale;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

				auto prices = state.world.market_get_price(markets, c);

				auto is_active = state.world.nation_get_unlocked_commodities(nations, c);
				auto life_weights = state.world.market_get_life_needs_weights(markets, c);
				auto everyday_weights = state.world.market_get_everyday_needs_weights(markets, c);
				auto luxury_weights = state.world.market_get_luxury_needs_weights(markets, c);

				state.world.for_each_pop_type([&](dcon::pop_type_id pop_type) {
					if(state.world.pop_type_get_strata(pop_type) != strata_filter) {
						return;
					}

					auto life_base = state.world.pop_type_get_life_needs(pop_type, c);
					auto everyday_base = state.world.pop_type_get_everyday_needs(pop_type, c);
					auto luxury_base = state.world.pop_type_get_luxury_needs(pop_type, c);

					auto life_costs = prices * life_base * life_needs_mult * life_weights;
					auto everyday_costs = prices * everyday_base * everyday_needs_mult * everyday_weights * invention_factor;
					auto luxury_costs = prices * luxury_base * luxury_needs_mult * luxury_weights * invention_factor;

					state.world.market_set_life_needs_costs(
						markets,
						pop_type,
						state.world.market_get_life_needs_costs(markets, pop_type) + life_costs
					);
					state.world.market_set_everyday_needs_costs(
						markets,
						pop_type,
						state.world.market_get_everyday_needs_costs(markets, pop_type) + everyday_costs
					);
					state.world.market_set_luxury_needs_costs(
						markets,
						pop_type,
						state.world.market_get_luxury_needs_costs(markets, pop_type) + luxury_costs
					);

#ifndef NDEBUG
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_life_needs_costs(markets, pop_type)
					);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_everyday_needs_costs(markets, pop_type)
					);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_luxury_needs_costs(markets, pop_type)
					);
#endif
				});
			}
		}

		// middle strata update

		{
			uint8_t strata_filter = 1;
			auto offset_life = sys::national_mod_offsets::middle_life_needs;
			auto offset_everyday = sys::national_mod_offsets::middle_everyday_needs;
			auto offset_luxury = sys::national_mod_offsets::middle_luxury_needs;

			auto life_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_life) + 1.0f)
				* state.defines.alice_lf_needs_scale;
			auto everyday_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_everyday) + 1.0f)
				* state.defines.alice_ev_needs_scale;
			auto luxury_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_luxury) + 1.0f)
				* state.defines.alice_lx_needs_scale;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

				auto prices = state.world.market_get_price(markets, c);

				auto is_active = state.world.nation_get_unlocked_commodities(nations, c);
				auto life_weights = state.world.market_get_life_needs_weights(markets, c);
				auto everyday_weights = state.world.market_get_everyday_needs_weights(markets, c);
				auto luxury_weights = state.world.market_get_luxury_needs_weights(markets, c);

				state.world.for_each_pop_type([&](dcon::pop_type_id pop_type) {
					if(state.world.pop_type_get_strata(pop_type) != strata_filter) {
						return;
					}

					auto life_base = state.world.pop_type_get_life_needs(pop_type, c);
					auto everyday_base = state.world.pop_type_get_everyday_needs(pop_type, c);
					auto luxury_base = state.world.pop_type_get_luxury_needs(pop_type, c);

					auto life_costs = prices * life_base * life_needs_mult * life_weights;
					auto everyday_costs = prices * everyday_base * everyday_needs_mult * everyday_weights * invention_factor;
					auto luxury_costs = prices * luxury_base * luxury_needs_mult * luxury_weights * invention_factor;

					state.world.market_set_life_needs_costs(
						markets,
						pop_type,
						state.world.market_get_life_needs_costs(markets, pop_type) + life_costs
					);
					state.world.market_set_everyday_needs_costs(
						markets,
						pop_type,
						state.world.market_get_everyday_needs_costs(markets, pop_type) + everyday_costs
					);
					state.world.market_set_luxury_needs_costs(
						markets,
						pop_type,
						state.world.market_get_luxury_needs_costs(markets, pop_type) + luxury_costs
					);
#ifndef NDEBUG
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_life_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_everyday_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_luxury_needs_costs(markets, pop_type)
							);
#endif
				});
			}
		}

		// rich strata update

		{
			uint8_t strata_filter = 2;
			auto offset_life = sys::national_mod_offsets::rich_life_needs;
			auto offset_everyday = sys::national_mod_offsets::rich_everyday_needs;
			auto offset_luxury = sys::national_mod_offsets::rich_luxury_needs;

			auto life_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_life) + 1.0f)
				* state.defines.alice_lf_needs_scale;
			auto everyday_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_everyday) + 1.0f)
				* state.defines.alice_ev_needs_scale;
			auto luxury_needs_mult =
				(state.world.nation_get_modifier_values(nations, offset_luxury) + 1.0f)
				* state.defines.alice_lx_needs_scale;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

				auto prices = state.world.market_get_price(markets, c);

				auto is_active = state.world.nation_get_unlocked_commodities(nations, c);
				auto life_weights = state.world.market_get_life_needs_weights(markets, c);
				auto everyday_weights = state.world.market_get_everyday_needs_weights(markets, c);
				auto luxury_weights = state.world.market_get_luxury_needs_weights(markets, c);

				state.world.for_each_pop_type([&](dcon::pop_type_id pop_type) {
					if(state.world.pop_type_get_strata(pop_type) != strata_filter) {
						return;
					}

					auto life_base = state.world.pop_type_get_life_needs(pop_type, c);
					auto everyday_base = state.world.pop_type_get_everyday_needs(pop_type, c);
					auto luxury_base = state.world.pop_type_get_luxury_needs(pop_type, c);

					auto life_costs = prices * life_base * life_needs_mult * life_weights;
					auto everyday_costs = prices * everyday_base * everyday_needs_mult * everyday_weights * invention_factor;
					auto luxury_costs = prices * luxury_base * luxury_needs_mult * luxury_weights * invention_factor;

					state.world.market_set_life_needs_costs(
						markets,
						pop_type,
						state.world.market_get_life_needs_costs(markets, pop_type) + life_costs
					);
					state.world.market_set_everyday_needs_costs(
						markets,
						pop_type,
						state.world.market_get_everyday_needs_costs(markets, pop_type) + everyday_costs
					);
					state.world.market_set_luxury_needs_costs(
						markets,
						pop_type,
						state.world.market_get_luxury_needs_costs(markets, pop_type) + luxury_costs
					);

#ifndef NDEBUG
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_life_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_everyday_needs_costs(markets, pop_type)
							);
					ve::apply(
						[](float x) {
							assert(std::isfinite(x));
						}, state.world.market_get_luxury_needs_costs(markets, pop_type)
							);
#endif
				});
			}
		}

		// clear real demand
		state.world.for_each_commodity([&](dcon::commodity_id c) {

			state.world.market_set_demand(markets, c, ve::fp_vector{});
			state.world.market_set_intermediate_demand(markets, c, ve::fp_vector{});
		});
	});

	for(int32_t i = 0; i < labor::total; i++) {
		state.world.execute_serial_over_province([&](auto ids) {
			state.world.province_set_labor_demand(ids, i, 0.f);
		});
	}

	services::reset_demand(state);

	// rgo/factories/artisans consumption
	update_production_consumption(state);

	state.world.for_each_commodity([&](auto cid) {
		bool is_potential_rgo = state.world.commodity_get_rgo_amount(cid) > 0.f;
		bool already_known_to_exist = state.world.commodity_get_actually_exists_in_nature(cid);
		if(is_potential_rgo && !already_known_to_exist) {
			for(auto pid : state.world.in_province) {
				auto potential = state.world.province_get_rgo_size(pid, cid);
				if(potential > 0) {
					state.world.commodity_set_actually_exists_in_nature(cid, true);
					break;
				}
			}
		}
	});

	sanity_check(state);

	// STEP 3 update pops consumption:

	// reset data first:
	state.world.for_each_pop_type([&](auto pt) {
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_life_needs_scale(ids, pt, 0.f);
			state.world.market_set_everyday_needs_scale(ids, pt, 0.f);
			state.world.market_set_luxury_needs_scale(ids, pt, 0.f);
		});
	});

	sanity_check(state);

	pops::update_consumption(state, invention_count);

	sanity_check(state);

	// national income is handled at the end,
	// so we have money stockpile from previous day
	// and can safely use it to calculate spendings and generate demand

	// national spendings ideally should follow the priority:
	// 1) loans interest (otherwise interest gets diluted and can be abused with high admin spending)
	// 2) international treaties (currently it is handled somewhere else, requires investigation)
	// 3) admin spending (can't pay for stuff without taxes)
	// 4) everything else

	// if nation is unable to pay interest it is considered bankrupt
	// money stockpiles after interest are considered BASEBUDGET
	// admin spendings take a ratio of BASEBUDGET to pay for bureaucracy
	// denote them as ADMIN
	// the rest of spending is divided into two groups:
	// 1) LEGACY: these expenses depend on some predefined value
	// 2) RELATIVE: these spendings are equal to ratio of BASEBUDGET
	// if LEGACY and RELATIVE - ADMIN expenses are larger than BASEBUDGET - ADMIN
	// then scale them down

	// AI is considered bankrupt when money stockpiles are lower than 0,
	// which should happen basically never

	// we have to recalculate loan related variables every new round, because they depend on themselves

	static auto spent_on_construction_buffer = state.world.nation_make_vectorizable_float_buffer();

	for(auto n : state.nations_by_rank) {
		if(!n) {
			continue;
		}
		spent_on_construction_buffer.set(n, 0.f);
		
		// handle loans
		bool is_bankrupt = false;
		{
			auto current_money = state.world.nation_get_stockpiles(n, economy::money);
			if(state.world.nation_get_is_player_controlled(n)) {
				auto max_loan_amount = max_loan(state, n);
				auto current_loan = state.world.nation_get_local_loan(n);
				auto current_interest = interest_payment(state, n);
				auto required_additional_loan = 0.f;
				auto current_bank_money = state.world.nation_get_national_bank(n);

				if(current_money < current_interest) {
					required_additional_loan = required_additional_loan - current_money;
				}

				if(current_money < current_interest && current_loan + required_additional_loan > max_loan_amount) {
					is_bankrupt = true;
				} else if(current_money > current_interest) {
					// can pay interest without new loans
					state.world.nation_set_stockpiles(n, economy::money, current_money - current_interest);
					state.world.nation_set_national_bank(n, current_bank_money + current_interest);
				} else {
					// we have to take additional loan to pay interest and we are able to do it
					state.world.nation_set_local_loan(n, current_loan + required_additional_loan);
					state.world.nation_set_stockpiles(n, economy::money, 0);
				}
			} else {
				if(current_money < 0) {
					is_bankrupt = true;
				}
			}
		}

		if(is_bankrupt) {
			go_bankrupt(state, n);
			state.world.nation_set_spending_level(n, 0.f);
			state.world.nation_set_last_base_budget(n, 0.f);
			update_national_consumption(state, n, 0.f, 0.f);
			update_consumption_administration(state, n, 0.f);
		} else {
			float spending_scale = 1.0f;

			// interest is paid and we are not bankrupt,
			// now we can assume that money stockpile is equal to BASE_BUDGET
			auto base_budget = state.world.nation_get_stockpiles(n, economy::money);
			auto additional_funding = 0.f;
			auto costs = full_spending_cost(state, n, base_budget);
			auto admin = costs.administration;
			auto required_additional_funding = std::max(0.f, costs.total - base_budget);
			auto current_loan = state.world.nation_get_local_loan(n);

			// if loan is required, then take as much as you can

			if(can_take_loans(state, n)) {
				auto available_loan = max_loan(state, n) - current_loan;
				additional_funding = std::min(required_additional_funding, available_loan);
			}

			// by definition, ADMIN must be lower or equal than BASE_BUDGET, so we can always pay for admin budget

			if(base_budget + additional_funding >= costs.total) {
				spending_scale = 1.f;
			} else {
				spending_scale =
					(costs.total - admin < 0.001f)
					? 1.f
					: (base_budget + additional_funding - admin) / (costs.total - admin);
			}

			assert(spending_scale >= 0);
			assert(std::isfinite(spending_scale));

			// spend money
			state.world.nation_set_stockpiles(
				n, economy::money, base_budget - std::min(base_budget, (costs.total - admin) * spending_scale + admin)
			);
			state.world.nation_set_spending_level(n, spending_scale);
			state.world.nation_set_last_base_budget(n, base_budget);

			if (additional_funding > 0.f) {
				// take the loan
				state.world.nation_set_local_loan(n, current_loan + additional_funding);
				state.world.nation_set_stockpiles(n, economy::money, 0);
			} else {
				// repay the loan
				auto money_before = state.world.nation_get_stockpiles(n, economy::money);
				auto paid_loan = std::min(money_before, current_loan);
				auto remaining_loan_after = std::max(0.f, current_loan - paid_loan);
				auto money_after = std::max(0.f, money_before - paid_loan);

				state.world.nation_set_local_loan(n, remaining_loan_after);
				state.world.nation_set_stockpiles(n, economy::money, money_after);
				// we do not increase national bank
				// because it stores the sum of loaned money and money available for a loan
			}

			spent_on_construction_buffer.set(n, spending_scale * costs.construction);

			// use calculated values to perform actual consumption
			update_national_consumption(state, n, spending_scale, base_budget);
			update_consumption_administration(state, n, base_budget);
		}

		// private budget
		{
			float private_spending_scale = 1.0f;
			float pi_total = full_private_investment_cost(state, n);
			float perceived_spending = pi_total;
			float pi_budget = state.world.nation_get_private_investment(n);
			private_spending_scale = perceived_spending <= pi_budget ? 1.0f : pi_budget / perceived_spending;
			state.world.nation_set_private_investment_effective_fraction(n, private_spending_scale);
			state.world.nation_set_private_investment(
				n,
				std::max(0.0f, pi_budget - pi_total * private_spending_scale)
			);

			update_private_consumption(state, n, private_spending_scale);
		}
	}

	sanity_check(state);

	advanced_province_buildings::update_national_size(state);
	advanced_province_buildings::update_private_size(state);
	advanced_province_buildings::update_consumption(state);

	sanity_check(state);

	update_trade_routes_consumption(state);

	sanity_check(state);

	// ###################
	// # MARKET CLEARING #
	// ###################

	/*
	perform actual consumption / purchasing subject to availability at markets:
	*/

#ifndef NDEBUG
	ve::fp_vector total_markets_income = 0.f;

	state.world.execute_serial_over_market([&](auto markets) {
		total_markets_income = total_markets_income - state.world.market_get_stockpile(markets, economy::money);
	});
#endif

	// labor

	for(int32_t j = 0; j < labor::total; j++) {
		state.world.execute_serial_over_province([&](auto ids) {
			auto supply_labor = state.world.province_get_labor_supply(ids, j);
			auto demand_labor = state.world.province_get_labor_demand(ids, j);

			auto satisfaction = ve::select(demand_labor > 0.f, ve::min(1.f, supply_labor / demand_labor), 1.f);
			auto sold = ve::select(supply_labor > 0.f, ve::min(1.f, demand_labor / supply_labor), 1.f);

#ifndef NDEBUG
			ve::apply([&](auto value) { assert(std::isfinite(value) && value >= 0.f); }, sold);
			ve::apply([&](auto value) { assert(std::isfinite(value) && value >= 0.f); }, satisfaction);
#endif

			state.world.province_set_labor_demand_satisfaction(ids, j, satisfaction);
			state.world.province_set_labor_supply_sold(ids, j, sold);
		});
	}

	services::match_supply_and_demand(state);

	state.world.execute_parallel_over_market([&](auto ids) {
		auto zones = state.world.market_get_zone_from_local_market(ids);
		auto market_capitals = state.world.state_instance_get_capital(zones);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(zones);
		auto capital = state.world.nation_get_capital(nations);
		auto capital_states = state.world.province_get_state_membership(capital);
		auto capital_mask = capital_states == zones;

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

			// we do not actually consume/purchase money rgos
			if(state.world.commodity_get_money_rgo(c)) {
				continue;
			}

			auto draw_from_stockpile = state.world.nation_get_drawing_on_stockpiles(nations, c) == true;

			// thankfully with local economies
			// there is no multiple layers of pools nonsense and
			// we can update markets in a really simple way:

			// local merchants buy something too
			// they have to account for spoilage
			// so they don't want spoiled goods to cost too much
			// so naturally, they don't stockpile expensive goods as much:
			auto stockpiles = state.world.market_get_stockpile(ids, c);
			auto stockpile_target_merchants = ve_stockpile_target_speculation(state, ids, c);

			// when good is expensive, we want to emulate competition between merhants to sell or buy it:
			// wage rating: earnings of population unit during the day
			// price_rating: amount of wages a population unit can receive with price of this good

			auto local_wage_rating = state.defines.alice_needs_scaling_factor * state.world.province_get_labor_price(market_capitals, labor::no_education) + 0.00001f;
			auto price_rating = (ve_price(state, ids, c)) / local_wage_rating;
			auto actual_stockpile_to_supply = ve::min(1.f, stockpile_to_supply + price_rating);

			auto merchants_demand = ve::max(0.f, stockpile_target_merchants - stockpiles) * actual_stockpile_to_supply;
			auto merchants_supply = ve::max(0.f, stockpiles - stockpile_target_merchants) * actual_stockpile_to_supply;

			auto production = state.world.market_get_supply(ids, c);
			// we draw from stockpile in capital
			auto national_stockpile = ve::select(
				capital_mask && draw_from_stockpile,
				state.world.nation_get_stockpiles(nations, c),
				0.f
			);
			auto total_supply = production + national_stockpile;
			auto total_demand = state.world.market_get_demand(ids, c) + merchants_demand;
			auto old_saturation = state.world.market_get_demand_satisfaction(ids, c);
			auto new_saturation = ve::select(total_demand == 0.f, 0.f, total_supply / total_demand);

			auto supply_unsold = ve::select(total_supply > total_demand, total_supply - total_demand, 0.f);
			auto supply_sold = total_supply - supply_unsold;
			auto supply_sold_ratio = ve::select(
				total_supply > 0.f,
				supply_sold / total_supply,
				0.f
			);

			new_saturation = ve::min(new_saturation, 1.f);

			auto delayed_saturation =
				old_saturation * state.defines.alice_sat_delay_factor
				+ new_saturation * (1.f - state.defines.alice_sat_delay_factor);

			state.world.market_set_demand_satisfaction(ids, c, delayed_saturation);
			state.world.market_set_consumption(ids, c, delayed_saturation * total_demand);
			auto old_supply_sold = state.world.market_get_supply_sold_ratio(ids, c);
			state.world.market_set_supply_sold_ratio(ids, c, old_supply_sold * 0.8f + supply_sold_ratio * 0.2f);
			state.world.market_set_direct_demand_satisfaction(ids, c, new_saturation);

			// we bought something locally
			// transaction
			// local money stockpile might go negative:
			// traders can take loans after all
			// our balance goal is to make sure that money trajectory is sane
			// there should be no +- inf in money stockpiles
			// decay stockpiles and ""gift"" the unsold supply to merchants

			state.world.market_set_stockpile(
				ids, c,
				ve::max(0.f, (
					state.world.market_get_stockpile(ids, c) * (1.f - stockpile_spoilage)
					+ total_supply - merchants_supply
					- total_demand * new_saturation
					))
			);

			if(presimulation) {
				state.world.market_set_stockpile(
					ids, c,	stockpile_target_merchants
				);
			}

			state.world.market_set_stockpile(
				ids, economy::money,
				state.world.market_get_stockpile(ids, economy::money)
				+ (
					merchants_supply * supply_sold_ratio
					- merchants_demand * new_saturation
					) * ve_price(state, ids, c)
			);

			// record the transaction
			total_demand = total_demand - new_saturation * total_demand;
			total_supply = total_supply - new_saturation * total_demand;

			// register demand from stockpiles to use in pricing
			state.world.market_set_demand(ids, c, merchants_demand + state.world.market_get_demand(ids, c));
#ifndef NDEBUG
			ve::apply([&](auto value) { assert(std::isfinite(value)); }, state.world.market_get_stockpile(ids, c));
#endif


			// then we siphon from national stockpile:
			// there is only one capital in a country!,
			// which means that we can safely pay back for siphoned stockpile
			// and change national stockpile at the same time
			ve::apply([&](bool do_it, float total_demand_i, float national_stockpile_i, float supply_sold_ratio_i, dcon::nation_id nations_i, dcon::market_id ids_i) {
				if(do_it) {
					auto buy_from_nation = ve::min(national_stockpile_i, total_demand_i * supply_sold_ratio_i);
					auto bought_from_nation_cost =
						buy_from_nation
						* state.world.market_get_price(ids_i, c)
						* state.inflation;
					state.world.nation_set_stockpiles(nations_i, c, national_stockpile_i - buy_from_nation);
					auto treasury = state.world.nation_get_stockpiles(nations_i, economy::money);
					state.world.nation_set_stockpiles(
						nations_i, economy::money, treasury + bought_from_nation_cost);
				}
			}, capital_mask && draw_from_stockpile, total_demand, national_stockpile, supply_sold_ratio, nations, ids);
		}
	});

#ifndef NDEBUG
	state.world.execute_serial_over_market([&](auto markets) {
		total_markets_income = total_markets_income + state.world.market_get_stockpile(markets, economy::money);
	});
	state.console_log("Total markets income: " + std::to_string(total_markets_income.reduce()));
#endif // !NDEBUG


	sanity_check(state);


	// ################################################################################
	// # ADJUST ACTUALLY SATISFIED DEMAND DEPENDING ON THE RESULTS OF MARKET CLEARING #
	// ################################################################################

	// updates of national purchases:
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		auto n = dcon::nation_id{ dcon::nation_id::value_base_t(i) };
		if(state.world.nation_get_owned_province_count(n) == 0)
			return;

		auto capital = state.world.nation_get_capital(n);
		auto capital_market = state.world.state_instance_get_market_from_local_market(
			state.world.province_get_state_membership(capital)
		);

		/*
		determine effective spending levels
		we iterate over all markets to gather contruction data
		*/
		auto nations_commodity_spending = state.world.nation_get_spending_level(n);
		float refund = 0.0f;
		{
			float max_sp = 0.0f;
			float total = 0.0f;
			float spending_level = float(state.world.nation_get_naval_spending(n)) / 100.0f;

			state.world.nation_for_each_state_ownership_as_nation(n, [&](dcon::state_ownership_id soid) {
				auto local_state = state.world.state_ownership_get_state(soid);
				auto local_market = state.world.state_instance_get_market_from_local_market(local_state);
				for(uint32_t k = 1; k < total_commodities; ++k) {
					dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

					auto sat = state.world.market_get_demand_satisfaction(local_market, c);
					auto val = state.world.market_get_navy_demand(local_market, c);
					auto delta =
						val
						* (1.0f - sat)
						* nations_commodity_spending
						* spending_level
						* price(state, local_market, c);
					assert(delta >= 0.f);
					refund += delta;
					total += val;
					max_sp += val * sat;
				}
			});

			if(total > 0.f)
				max_sp /= total;
			state.world.nation_set_effective_naval_spending(
				n, nations_commodity_spending * max_sp * spending_level);
			auto& current_buf = state.world.nation_get_naval_reinforcement_buffer(n);
			state.world.nation_set_naval_reinforcement_buffer(n, current_buf + state.world.nation_get_effective_naval_spending(n));
			assert(current_buf >= 0.0f);
		}
		{
			float max_sp = 0.0f;
			float total = 0.0f;
			float spending_level = float(state.world.nation_get_land_spending(n)) / 100.0f;

			state.world.nation_for_each_state_ownership_as_nation(n, [&](dcon::state_ownership_id soid) {
				auto local_state = state.world.state_ownership_get_state(soid);
				auto local_market = state.world.state_instance_get_market_from_local_market(local_state);
				for(uint32_t k = 1; k < total_commodities; ++k) {
					dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

					auto sat = state.world.market_get_demand_satisfaction(local_market, c);
					auto val = state.world.market_get_army_demand(local_market, c);
					auto delta =
						val
						* (1.0f - sat)
						* nations_commodity_spending
						* spending_level
						* price(state, local_market, c);
					assert(delta >= 0.f);
					refund += delta;
					total += val;
					max_sp += val * sat;
				}
			});
			if(total > 0.f)
				max_sp /= total;
			assert(std::isfinite(nations_commodity_spending* max_sp* spending_level));
			state.world.nation_set_effective_land_spending(
				n, nations_commodity_spending * max_sp * spending_level);
			auto& current_buf = state.world.nation_get_land_reinforcement_buffer(n);
			state.world.nation_set_land_reinforcement_buffer(n, current_buf + state.world.nation_get_effective_land_spending(n));
			assert(current_buf >= 0.0f);
		}
		{
			state.world.nation_set_effective_construction_spending(
				n,
				nations_commodity_spending
			);
		}
		/*
		fill stockpiles from the capital market
		*/

		for(uint32_t k = 1; k < total_commodities; ++k) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };
			auto difference = state.world.nation_get_stockpile_targets(n, c) - state.world.nation_get_stockpiles(n, c);
			if(difference > 0.f && state.world.nation_get_drawing_on_stockpiles(n, c) == false) {
				auto sat = state.world.market_get_direct_demand_satisfaction(capital_market, c);
				auto& curr = state.world.nation_get_stockpiles(n, c);
				state.world.nation_set_stockpiles(n, c, curr + difference * nations_commodity_spending * sat);
				auto delta =
					difference
					* (1.0f - sat)
					* nations_commodity_spending
					* price(state, capital_market, c);
				assert(delta >= 0.f);
				refund += delta;
			}
		}

		/*
		calculate overseas penalty:
		ideally these goods would be bought in colonies
		but limit to capital for now
		*/

		{
			auto overseas_factor = state.defines.province_overseas_penalty
				* float(
					state.world.nation_get_owned_province_count(n)
					- state.world.nation_get_central_province_count(n)
				);
			auto overseas_budget = float(state.world.nation_get_overseas_spending(n)) / 100.f;
			auto overseas_budget_satisfaction = 1.f;

			if(overseas_factor > 0) {
				for(uint32_t k = 1; k < total_commodities; ++k) {
					dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };
					if(state.world.commodity_get_overseas_penalty(c) && valid_need(state, n, c)) {
						auto sat = state.world.market_get_demand_satisfaction(capital_market, c);
						overseas_budget_satisfaction = std::min(sat, overseas_budget_satisfaction);
						auto price_of = price(state, capital_market, c);
						auto delta = overseas_factor
							* (1.0f - sat) * nations_commodity_spending * price_of;
						assert(delta >= 0.f);
						refund += delta;
					}
				}

				state.world.nation_set_overseas_penalty(n, overseas_budget
					* overseas_budget_satisfaction);
			} else {
				state.world.nation_set_overseas_penalty(n, 1.0f);
			}
		}

		// finally, pay back refund:
		assert(std::isfinite(refund) && refund >= 0.0f);
		state.world.nation_set_stockpiles(n, money, state.world.nation_get_stockpiles(n, money) + refund);
	});

	sanity_check(state);

	/* now we know demand satisfaction and can set actual satifaction of pops */

	/* prepare needs satisfaction caps */
	state.world.execute_parallel_over_pop_type([&](auto pts) {
		ve::apply([&](dcon::pop_type_id pt) {
			state.world.execute_serial_over_market([&](auto ids) {
				auto states = state.world.market_get_zone_from_local_market(ids);
				auto nations = state.world.state_instance_get_nation_from_state_ownership(states);

				ve::fp_vector ln_total = 0.0f;
				ve::fp_vector en_total = 0.0f;
				ve::fp_vector lx_total = 0.0f;

				ve::fp_vector ln_max = 0.0f;
				ve::fp_vector en_max = 0.0f;
				ve::fp_vector lx_max = 0.0f;

				for(uint32_t i = 1; i < total_commodities; ++i) {
					dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
					auto sat = state.world.market_get_demand_satisfaction(ids, c);
					auto valid_mask = valid_need(state, nations, c);

					auto ln_val = ve::select(valid_mask, ve::fp_vector{ state.world.pop_type_get_life_needs(pt, c) }, ve::fp_vector{ 0.f });

					ln_total = ln_total + ln_val;
					ln_max = ln_max + ln_val * sat * state.world.market_get_life_needs_weights(ids, c);

					auto en_val = ve::select(valid_mask, ve::fp_vector{ state.world.pop_type_get_everyday_needs(pt, c) }, ve::fp_vector{ 0.f });

					en_total = en_total + en_val;
					en_max = en_max + en_val * sat * state.world.market_get_everyday_needs_weights(ids, c);

					auto lx_val = ve::select(valid_mask, ve::fp_vector{ state.world.pop_type_get_luxury_needs(pt, c) }, ve::fp_vector{ 0.f });

					lx_total = lx_total + lx_val;
					lx_max = lx_max + lx_val * sat * state.world.market_get_luxury_needs_weights(ids, c);
				}

				ln_max = ve::select(ln_total > 0.f, ln_max / ln_total, 1.f);
				en_max = ve::select(en_total > 0.f, en_max / en_total, 1.f);
				lx_max = ve::select(lx_total > 0.f, lx_max / lx_total, 1.f);

#ifndef NDEBUG
				ve::apply([](float life, float everyday, float luxury) {
					assert(life >= 0.f && life <= 1.f);
					assert(everyday >= 0.f && everyday <= 1.f);
					assert(luxury >= 0.f && luxury <= 1.f);
				}, ln_max, en_max, lx_max);
#endif // !NDEBUG


				state.world.market_set_max_life_needs_satisfaction(ids, pt, ln_max);
				state.world.market_set_max_everyday_needs_satisfaction(ids, pt, en_max);
				state.world.market_set_max_luxury_needs_satisfaction(ids, pt, lx_max);
			});
		}, pts);
	});

	sanity_check(state);

	// update needs satisfaction depending on actually available goods and services:

	auto base_shift_satisfaction = ve::fp_vector{ 1.f / 200.f };
	auto base_shift_literacy = pop_demographics::pop_u16_scaling;
	state.world.execute_serial_over_pop([&](auto ids) {
		auto province = state.world.pop_get_province_from_pop_location(ids);
		auto local_state = state.world.province_get_state_membership(province);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);

		auto pop_type = state.world.pop_get_poptype(ids);

		auto ln = pop_demographics::get_life_needs(state, ids);
		auto en = pop_demographics::get_everyday_needs(state, ids);
		auto lx = pop_demographics::get_luxury_needs(state, ids);

		auto ln_satisfaction = ve::apply([&](auto market, auto pt) {
			return state.world.market_get_max_life_needs_satisfaction(market, pt);
		}, local_market, pop_type);
		auto en_satisfaction = ve::apply([&](auto market, auto pt) {
			return state.world.market_get_max_everyday_needs_satisfaction(market, pt);
		}, local_market, pop_type);
		auto lx_satisfaction = ve::apply([&](auto market, auto pt) {
			return state.world.market_get_max_luxury_needs_satisfaction(market, pt);
		}, local_market, pop_type);

		auto ln_not_enough = ln_satisfaction < 0.5f;
		auto en_not_enough = en_satisfaction < 0.5f;
		auto lx_not_enough = lx_satisfaction < 0.5f;

		ln = ve::select(ln_not_enough, ln - base_shift_satisfaction, ln);
		en = ve::select(en_not_enough, en - base_shift_satisfaction, en);
		lx = ve::select(lx_not_enough, lx - base_shift_satisfaction, lx);

		pop_demographics::set_life_needs(state, ids, ln);
		pop_demographics::set_everyday_needs(state, ids, en);
		pop_demographics::set_luxury_needs(state, ids, lx);

		{
			auto literacy = pop_demographics::get_literacy(state, ids);
			auto literacy_satisfaction = state.world.province_get_service_satisfaction(province, services::list::education)
				+ state.world.province_get_service_satisfaction_for_free(province, services::list::education);
			literacy = ve::select(literacy_satisfaction < 0.5f, literacy - base_shift_literacy, literacy);
			pop_demographics::set_literacy(state, ids, literacy);
		}
	});

	sanity_check(state);

	// finally we can move to production:

	// ##########
	// # SUPPLY #
	// ##########

	services::reset_supply(state);
	for(int32_t i = 0; i < labor::total; i++) {
		state.world.execute_serial_over_province([&](auto markets) {
			state.world.province_set_labor_supply(markets, i, 0.f);
		});
	}
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.execute_serial_over_market([&](auto markets) {
			state.world.market_set_supply(markets, c, ve::fp_vector{});
			state.world.market_set_import(markets, c, ve::fp_vector{});
			state.world.market_set_export(markets, c, ve::fp_vector{});
		});
	});

	// at this stage we record supply
	// old supply is invalidated

	// we can't handle each trade good separately: they do influence common markets
	// todo: split the logic so some part of it could be done in parallel
	//
	// register trade supply

#ifndef NDEBUG
	float total_trade_paid = 0.f;
	float total_merchant_cut = 0.f;
#endif // !NDEBUG


	state.world.for_each_trade_route([&](auto trade_route) {
		auto data = explain_trade_route(state, trade_route);

		for(uint32_t k = 0; k < total_commodities; k++) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
			if(state.world.commodity_get_money_rgo(cid) || (state.world.trade_route_get_volume(trade_route, cid) == 0.f)) {
				continue;
			}
			trade_and_tariff route_data = explain_trade_route_commodity(state, trade_route, data, cid);
			auto& cur_export = state.world.market_get_export(route_data.origin, cid);
			state.world.market_set_export(route_data.origin, cid, cur_export + route_data.amount_origin);
			auto& cur_import = state.world.market_get_import(route_data.target, cid);
			state.world.market_set_import(route_data.target, cid, cur_import + route_data.amount_target);
			auto& cur_origin_stockpile = state.world.market_get_stockpile(route_data.origin, economy::money);
			state.world.market_set_stockpile(route_data.origin, economy::money, cur_origin_stockpile + route_data.amount_origin * route_data.payment_received_per_unit);
			auto& cur_target_stockpile = state.world.market_get_stockpile(route_data.target, economy::money);
			state.world.market_set_stockpile(route_data.target, economy::money, cur_target_stockpile - route_data.amount_origin * route_data.payment_per_unit);

#ifndef NDEBUG
			total_trade_paid += route_data.amount_origin * route_data.payment_per_unit;
			total_merchant_cut += route_data.amount_origin * route_data.payment_received_per_unit;
#endif // !NDEBUG
			state.world.market_set_tariff_collected(route_data.origin, state.world.market_get_tariff_collected(route_data.origin) + route_data.tariff_origin);
			state.world.market_set_tariff_collected(route_data.target, state.world.market_get_tariff_collected(route_data.target) + route_data.tariff_target);
			state.world.market_set_stockpile(route_data.target, cid, state.world.market_get_stockpile(route_data.target, cid) + route_data.amount_target);

			assert(std::isfinite(state.world.market_get_export(route_data.origin, cid)));
			assert(std::isfinite(state.world.market_get_import(route_data.target, cid)));
		}
	});

#ifndef NDEBUG
	state.console_log("Total markets trade payments: " + std::to_string(total_trade_paid));
	state.console_log("Total markets merchant cuts: " + std::to_string(total_merchant_cut));
#endif // !NDEBUG

	// we bought something: register supply from stockpiles:

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.execute_serial_over_market([&](auto markets) {
			state.world.market_set_supply(markets, c, ve::fp_vector{});
			if(!state.world.commodity_get_money_rgo(c)) {
				auto stockpiles = state.world.market_get_stockpile(markets, c);
				auto states = state.world.market_get_zone_from_local_market(markets);
				auto capitals = state.world.state_instance_get_capital(states);
				auto price = ve_price(state, markets, c);
				auto stockpile_target_merchants = ve_stockpile_target_speculation(state, markets, c);
				auto local_wage_rating = state.defines.alice_needs_scaling_factor * state.world.province_get_labor_price(capitals, labor::no_education) + 0.00001f;
				auto price_rating = price / local_wage_rating;
				auto actual_stockpile_to_supply = ve::min(1.f, stockpile_to_supply + price_rating);
				auto merchants_supply = ve::max(0.f, stockpiles - stockpile_target_merchants) * actual_stockpile_to_supply;
				state.world.market_set_supply(markets, c, state.world.market_get_supply(markets, c) + merchants_supply);
			}
		});
	});

	sanity_check(state);

	// labor supply
	{
		// very simple labor distribution for now:
		// just follow wages proportionally and reduce supply when wages are small

		state.world.execute_parallel_over_province([&](auto ids) {
			auto states = state.world.province_get_state_membership(ids);
			auto markets = state.world.state_instance_get_market_from_local_market(states);
			auto target_wage =
				(
					state.world.market_get_life_needs_costs(markets, state.culture_definitions.primary_factory_worker) * labor_greed_life
					+ state.world.market_get_everyday_needs_costs(markets, state.culture_definitions.primary_factory_worker) * labor_greed_everyday
				)
				/ state.defines.alice_needs_scaling_factor
				+ 0.0001f;

			auto no_education = state.world.province_get_labor_price(ids, labor::no_education);
			auto basic_education = state.world.province_get_labor_price(ids, labor::basic_education);
			auto high_education = state.world.province_get_labor_price(ids, labor::high_education);
			auto guild_education = state.world.province_get_labor_price(ids, labor::guild_education);
			auto high_education_and_accepted = state.world.province_get_labor_price(ids, labor::high_education_and_accepted);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::rgo_worker_no_education,
				no_education / (no_education + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::rgo_worker_no_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::primary_no_education,
				no_education / (no_education + basic_education + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::primary_no_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::high_education_accepted_no_education,
				no_education / (no_education + basic_education + high_education + high_education_and_accepted + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_accepted_no_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::high_education_not_accepted_no_education,
				no_education / (no_education + basic_education + high_education + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_not_accepted_no_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::primary_basic_education,
				basic_education / (no_education + basic_education + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::primary_basic_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::high_education_accepted_basic_education,
				basic_education / (no_education + basic_education + high_education + high_education_and_accepted + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_accepted_basic_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::high_education_not_accepted_basic_education,
				basic_education / (no_education + basic_education + high_education + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_not_accepted_basic_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::high_education_accepted_high_education,
				high_education / (no_education + basic_education + high_education + high_education_and_accepted + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_accepted_high_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::high_education_not_accepted_high_education,
				high_education / (no_education + basic_education + high_education + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_not_accepted_high_education) * 0.9f
			);

			state.world.province_set_pop_labor_distribution(
				ids,
				pop_labor::high_education_accepted_high_education_accepted,
				high_education_and_accepted / (no_education + basic_education + high_education + high_education_and_accepted + target_wage) * 0.1f
				+ state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_accepted_high_education_accepted) * 0.9f
			);

			auto n = state.world.province_get_nation_from_province_ownership(ids);

			auto total_rgo_workers = ve::fp_vector{ 0.f };
			state.world.for_each_pop_type([&](dcon::pop_type_id ptid) {
				if(
					state.world.pop_type_get_is_paid_rgo_worker(ptid)
					|| ptid == state.culture_definitions.slaves
				) {
					total_rgo_workers = total_rgo_workers + state.world.province_get_demographics(ids, demographics::to_key(state, ptid));
				}
			});

			auto total_primary = state.world.province_get_demographics(ids, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
			auto total_guild_education = state.world.province_get_demographics(ids, demographics::to_key(state, state.culture_definitions.artisans));


			auto total_high_education = ve::apply([&](auto p, auto n) {
				auto total = 0.f;
				for(auto pl : state.world.province_get_pop_location(p)) {
					auto not_accepted = !state.world.nation_get_accepted_cultures(n, pl.get_pop().get_culture())
						&& state.world.nation_get_primary_culture(n) != pl.get_pop().get_culture().id;

					if(state.culture_definitions.secondary_factory_worker == pl.get_pop().get_poptype()) {
						if(not_accepted)
							total += pl.get_pop().get_size();
					} else if(pl.get_pop().get_poptype() == state.culture_definitions.bureaucrat) {
						if(not_accepted)
							total += pl.get_pop().get_size();
					} else if(pl.get_pop().get_poptype() == state.culture_definitions.clergy) {
						if(not_accepted)
							total += pl.get_pop().get_size();
					}
				}
				return total;
			}, ids, n);

			auto total_high_education_and_accepted = ve::apply([&](auto p, auto n) {
				auto total = 0.f;
				for(auto pl : state.world.province_get_pop_location(p)) {
					auto accepted = state.world.nation_get_accepted_cultures(n, pl.get_pop().get_culture())
						|| state.world.nation_get_primary_culture(n) == pl.get_pop().get_culture().id;

					if(state.culture_definitions.secondary_factory_worker == pl.get_pop().get_poptype()) {
						if(accepted)
							total += pl.get_pop().get_size();
					} else if(pl.get_pop().get_poptype() == state.culture_definitions.bureaucrat) {
						if(accepted)
							total += pl.get_pop().get_size();
					} else if(pl.get_pop().get_poptype() == state.culture_definitions.clergy) {
						if(accepted)
							total += pl.get_pop().get_size();
					}
				}
				return total;
			}, ids, n);

			state.world.province_set_labor_supply(
				ids, labor::no_education,
				total_rgo_workers * state.world.province_get_pop_labor_distribution(ids, pop_labor::rgo_worker_no_education)
				+ total_primary * state.world.province_get_pop_labor_distribution(ids, pop_labor::primary_no_education)
				+ total_high_education * state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_not_accepted_no_education)
				+ total_high_education_and_accepted * state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_accepted_no_education)
			);
			state.world.province_set_labor_supply(
				ids, labor::basic_education,
				total_primary * state.world.province_get_pop_labor_distribution(ids, pop_labor::primary_basic_education)
				+ total_high_education * state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_not_accepted_basic_education)
				+ total_high_education_and_accepted * state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_accepted_basic_education)
			);
			state.world.province_set_labor_supply(
				ids, labor::high_education,
				total_high_education * state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_not_accepted_high_education)
				+ total_high_education_and_accepted * state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_accepted_high_education)
			);
			state.world.province_set_labor_supply(
				ids, labor::guild_education,
				total_guild_education
			);
			state.world.province_set_labor_supply(
				ids, labor::high_education_and_accepted,
				total_high_education_and_accepted * state.world.province_get_pop_labor_distribution(ids, pop_labor::high_education_accepted_high_education_accepted)
			);
		});
	}

	update_pops_employment(state);
	sanity_check(state);

	// produce goods and services

	update_artisan_production(state);
	advanced_province_buildings::update_production(state);
	update_factories_production(state);
	update_rgo_production(state);

	// ####################
	// # PAYMENTS TO POPS #
	// ####################

	pops::update_income_national_subsidy(state);
	pops::update_income_trade(state);
	pops::update_income_artisans(state);
	pops::update_income_wages(state);

	// #####################
	// # TAXES AND TARIFFS #
	// #####################

	for(auto n : state.world.in_nation) {
		/* advance construction */
		advance_construction(state, n, spent_on_construction_buffer.get(n));
		if(presimulation) {
			emulate_construction_demand(state, n);
		}

		/*
		collect taxes
		*/

		if(false) {
			// we do not want to accumulate tons of money during presim
			state.world.for_each_nation([&](dcon::nation_id n) {
				state.world.nation_set_stockpiles(n, money, 0.f);
			});
		}

		collect_taxes(state, n);

		{
			/*
			collect tariffs
			*/

			float t_total = 0.0f;

			for(auto si : state.world.nation_get_state_ownership(n)) {
				auto market = si.get_state().get_market_from_local_market();
				auto capital = si.get_state().get_capital();
				if(capital.get_nation_from_province_control() == n) {
					t_total += state.world.market_get_tariff_collected(market);
				}
				state.world.market_set_tariff_collected(market, 0.f);
			}

			assert(std::isfinite(t_total));
			assert(t_total >= 0);
			state.world.nation_set_stockpiles(n, money, state.world.nation_get_stockpiles(n, money) + t_total);
		}

		// shift needs weights
		for(auto si : state.world.nation_get_state_ownership(n)) {
			auto market = si.get_state().get_market_from_local_market();
			rebalance_needs_weights(state, market);
		}
	};

	sanity_check(state);

	// #################
	// # PRICE UPDATES #
	// #################

	// at this point we already know supply and demand
	// so we can update prices

	/*
	adjust prices based on global production & consumption
	*/
	state.world.execute_serial_over_market([&](auto ids) {
		auto costs = state.world.market_get_everyday_needs_costs(ids, state.culture_definitions.laborers);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(!state.world.commodity_get_money_rgo(c))
				return;
			state.world.market_set_price(ids, c, ve::min(costs * 10.f, state.world.commodity_get_cost(c)));
		});
	});

	// price of labor

	for(int32_t i = 0; i < labor::total; i++) {
		state.world.execute_serial_over_province([&](auto ids) {
			ve::fp_vector supply = state.world.province_get_labor_supply(ids, i);
			ve::fp_vector demand = state.world.province_get_labor_demand(ids, i);
			auto current_price = state.world.province_get_labor_price(ids, i);

			current_price = current_price + price_properties::change(current_price, supply, demand);

			auto nids = state.world.province_get_nation_from_province_ownership(ids);
			auto sids = state.world.province_get_state_membership(ids);
			auto mids = state.world.state_instance_get_market_from_local_market(sids);
			auto min_wage_factor = ve_pop_min_wage_factor(state, nids);

			// labor price control

			ve::fp_vector price_control = ve::fp_vector{ 0.f };
			price_control = price_control + state.world.market_get_life_needs_costs(mids, state.culture_definitions.secondary_factory_worker) * 1.2f;
			price_control = price_control + state.world.market_get_everyday_needs_costs(mids, state.culture_definitions.secondary_factory_worker) * 0.5f;

			// we reduce min wage if unemployment is too high
			// base min wage is decided by national multipliers
			// then we apply administrative efficiency

			price_control =
				price_control
				* state.world.province_get_labor_supply_sold(ids, i)
				* state.world.province_get_control_ratio(ids)
				/ state.defines.alice_needs_scaling_factor
				* min_wage_factor;

#ifndef NDEBUG
			ve::apply([&](auto value) { assert(std::isfinite(value)); }, current_price);
#endif
			current_price = ve::min(ve::max(current_price, ve::max(price_properties::labor::min, price_control)), price_properties::labor::max);
			state.world.province_set_labor_price(ids, i, current_price);
		});
	}

	state.world.execute_serial_over_market([&](auto ids) {
		concurrency::parallel_for(uint32_t(1), total_commodities, [&](uint32_t k) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

			//handling gold cost separetely
			if(state.world.commodity_get_money_rgo(cid)) {
				return;
			}

			ve::fp_vector supply = state.world.market_get_supply(ids, cid);
			ve::fp_vector demand = state.world.market_get_demand(ids, cid);
			auto current_price = ve_price(state, ids, cid);
			current_price = current_price + price_properties::change(current_price, supply, demand);

#ifndef NDEBUG
			ve::apply([&](auto value) { assert(std::isfinite(value)); }, current_price);
#endif
			
			current_price = ve::min(ve::max(current_price, price_properties::commodity::min), price_properties::commodity::max);
			state.world.market_set_price(ids, cid, current_price);
		});
	});	

	services::update_price(state);

	// update median prices
	state.world.for_each_commodity([&](auto cid) {
		state.world.commodity_set_median_price(cid, median_price(state, cid));
	});

	sanity_check(state);

	if(state.cheat_data.ecodump) {
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			float states_count = 0.f;
			float total_price = 0.f;
			float total_production = 0.f;
			float total_demand = 0.f;

			state.world.for_each_market([&](auto id) {
				states_count++;
				total_price += state.world.market_get_price(id, c);
				total_production += state.world.market_get_supply(id, c);
				total_demand += state.world.market_get_demand(id, c);
			});

			state.cheat_data.prices_dump_buffer += std::to_string(total_price / states_count) + ",";
			state.cheat_data.supply_dump_buffer += std::to_string(total_production) + ",";
			state.cheat_data.demand_dump_buffer += std::to_string(total_demand) + ",";
		});

		state.cheat_data.prices_dump_buffer += "\n";
		state.cheat_data.supply_dump_buffer += "\n";
		state.cheat_data.demand_dump_buffer += "\n";
	}


	/*
	DIPLOMATIC EXPENSES
	*/

	for(auto n : state.world.in_nation) {
		// Subject money transfers
		auto rel = state.world.nation_get_overlord_as_subject(n);
		auto overlord = state.world.overlord_get_ruler(rel);

		if(overlord) {
			auto transferamt = estimate_subject_payments_paid(state, n);
			state.world.nation_set_stockpiles(n, money, state.world.nation_get_stockpiles(n, money) - transferamt);
			state.world.nation_set_stockpiles(overlord, money, state.world.nation_get_stockpiles(overlord, money) + transferamt);
		}

		for(auto uni : n.get_unilateral_relationship_as_source()) {
			if(uni.get_war_subsidies()) {
				auto sub_size = estimate_war_subsidies(state, uni.get_target(), uni.get_source());

				if(sub_size <= n.get_stockpiles(money)) {
					n.set_stockpiles(money, n.get_stockpiles(money) - sub_size);
					auto& current = uni.get_target().get_stockpiles(money);
					uni.get_target().set_stockpiles(money, current + sub_size);
				} else {
					uni.set_war_subsidies(false);

					notification::post(state, notification::message{
						[source = n.id, target = uni.get_target().id](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_wsub_end_1", text::variable_type::x, source, text::variable_type::y, target);
						},
						"msg_wsub_end_title",
						n.id, uni.get_target().id, dcon::nation_id{},
						sys::message_base_type::war_subsidies_end
					});
				}
			}
			if(uni.get_reparations() && state.current_date < n.get_reparations_until()) {
				auto const tax_eff = nations::tax_efficiency(state, n);
				auto total_tax_base = n.get_total_rich_income() + n.get_total_middle_income() + n.get_total_poor_income();

				auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
				auto capped_payout = std::min(n.get_stockpiles(money), payout);
				assert(capped_payout >= 0.0f);
				n.set_stockpiles(money, n.get_stockpiles(money) - capped_payout);
				auto& current = uni.get_target().get_stockpiles(money);
				uni.get_target().set_stockpiles(money, current + capped_payout);
			}
		}
	}

	// essentially upper bound on wealth in the system
	state.inflation = 0.999f;

	sanity_check(state);

	// make constructions:
	resolve_constructions(state);

	if(!presimulation) {
		run_private_investment(state);
	}

	sanity_check(state);

	// ####################
	// # STATS COLLECTION #
	// ####################

	//write gdp and total savings to file
	if(state.cheat_data.ecodump) {
		float total_savings_pops[20] = { };

		for(int i = 0; i < 20; i++) {
			total_savings_pops[i] = 0.f;
		}
		state.world.for_each_pop([&](auto pop) {
			total_savings_pops[state.world.pop_get_poptype(pop).id.index()] += state.world.pop_get_savings(pop);
		});

		float total_savings_markets = 0.f;
		state.world.for_each_market([&](auto market) {
			total_savings_markets += state.world.market_get_stockpile(market, economy::money);
		});

		float total_savings_nations = 0.f;
		float total_investment_pool = 0.f;
		state.world.for_each_nation([&](auto nation) {
			total_savings_nations += state.world.nation_get_stockpiles(nation, economy::money);
			total_investment_pool += state.world.nation_get_private_investment(nation);
		});

		if(state.cheat_data.savings_buffer.size() == 0) {
			state.world.for_each_pop_type([&](auto pop_type) {
				state.cheat_data.savings_buffer += std::to_string(total_savings_pops[pop_type.index()]);
				state.cheat_data.savings_buffer += ";";
			});
			state.cheat_data.savings_buffer += std::to_string(total_savings_markets);
			state.cheat_data.savings_buffer += ";";
			state.cheat_data.savings_buffer += std::to_string(total_savings_nations);
			state.cheat_data.savings_buffer += ";";
			state.cheat_data.savings_buffer += std::to_string(total_investment_pool);
			state.cheat_data.savings_buffer += "\n";
		}
	}

	sanity_check(state);
}

void regenerate_unsaved_values(sys::state& state) {
	state.culture_definitions.rgo_workers.clear();
	for(auto pt : state.world.in_pop_type) {
		if(pt.get_is_paid_rgo_worker())
			state.culture_definitions.rgo_workers.push_back(pt);
	}

	auto const total_commodities = state.world.commodity_size();
	for(uint32_t k = 1; k < total_commodities; ++k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		for(auto pt : state.world.in_pop_type) {
			if(pt != state.culture_definitions.slaves) {
				if(pt.get_life_needs(cid) > 0.0f)
					state.world.commodity_set_is_life_need(cid, true);
				if(pt.get_everyday_needs(cid) > 0.0f)
					state.world.commodity_set_is_everyday_need(cid, true);
				if(pt.get_luxury_needs(cid) > 0.0f)
					state.world.commodity_set_is_luxury_need(cid, true);
			}
		}
	}

	state.world.market_resize_intermediate_demand(state.world.commodity_size());

	state.world.market_resize_life_needs_costs(state.world.pop_type_size());
	state.world.market_resize_everyday_needs_costs(state.world.pop_type_size());
	state.world.market_resize_luxury_needs_costs(state.world.pop_type_size());

	state.world.market_resize_life_needs_scale(state.world.pop_type_size());
	state.world.market_resize_everyday_needs_scale(state.world.pop_type_size());
	state.world.market_resize_luxury_needs_scale(state.world.pop_type_size());

	state.world.market_resize_max_life_needs_satisfaction(state.world.pop_type_size());
	state.world.market_resize_max_everyday_needs_satisfaction(state.world.pop_type_size());
	state.world.market_resize_max_luxury_needs_satisfaction(state.world.pop_type_size());
}

float government_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto overseas_factor =
		state.defines.province_overseas_penalty *
		float(
			state.world.nation_get_owned_province_count(n)
			- state.world.nation_get_central_province_count(n)
		);
	auto o_adjust = 0.0f;
	if(overseas_factor > 0) {
		if(
			state.world.commodity_get_overseas_penalty(c)
			&& (
				state.world.commodity_get_is_available_from_start(c)
				|| state.world.nation_get_unlocked_commodities(n, c)
				)
		) {
			o_adjust = overseas_factor;
		}
	}

	auto total = 0.f;

	state.world.nation_for_each_state_ownership_as_nation(n, [&](auto soid) {
		auto market =
			state.world.state_instance_get_market_from_local_market(
				state.world.state_ownership_get_state(soid)
			);
		total = total + state.world.market_get_army_demand(market, c);
		total = total + state.world.market_get_navy_demand(market, c);
		total = total + state.world.market_get_construction_demand(market, c);
	});

	return total + o_adjust;
}

float nation_pop_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto amount = 0.f;
	if(state.world.commodity_get_is_available_from_start(c) || state.world.nation_get_unlocked_commodities(n, c)) {
		state.world.nation_for_each_state_ownership(n, [&](auto soid) {
			auto si = state.world.state_ownership_get_state(soid);
			auto m = state.world.state_instance_get_market_from_local_market(si);
			state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
				amount += (
						state.world.pop_type_get_life_needs(pt, c)
							* state.world.market_get_life_needs_weights(m, c)
						+ state.world.pop_type_get_everyday_needs(pt, c)
							* state.world.market_get_everyday_needs_weights(m, c)
						+ state.world.pop_type_get_luxury_needs(pt, c)
							* state.world.market_get_luxury_needs_weights(m, c)
					)
					* state.world.nation_get_demographics(n, demographics::to_key(state, pt))

					/ state.defines.alice_needs_scaling_factor;
			});
		});
	}
	return amount;
}

float nation_total_imports(sys::state& state, dcon::nation_id n) {
	float t_total = 0.0f;

	auto const total_commodities = state.world.commodity_size();

	for(uint32_t k = 1; k < total_commodities; ++k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		state.world.nation_for_each_state_ownership(n, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			t_total += price(state, market, cid) * state.world.market_get_import(market, cid);
		});
	}

	return t_total;
}

float nation_total_exports(sys::state& state, dcon::nation_id n) {
	float t_total = 0.0f;

	auto const total_commodities = state.world.commodity_size();

	for(uint32_t k = 1; k < total_commodities; ++k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		state.world.nation_for_each_state_ownership(n, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			t_total += price(state, market, cid) * state.world.market_get_export(market, cid);
		});
	}

	return t_total;
}

float estimate_gold_income(sys::state& state, dcon::nation_id n) {
	auto amount = 0.f;
	for(auto poid : state.world.nation_get_province_ownership_as_nation(n)) {
		auto prov = poid.get_province();
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(state.world.commodity_get_money_rgo(c)) {
				amount +=
					state.world.province_get_rgo_output(prov, c)
					* state.world.commodity_get_cost(c);
			}
		});
	}
	return amount * state.defines.gold_to_cash_rate;
}

float estimate_tariff_import_income(sys::state& state, dcon::nation_id n) {
	float result = 0.f;
	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		state.world.nation_for_each_state_ownership(n, [&](auto sid) {
			auto mid = state.world.state_instance_get_market_from_local_market(state.world.state_ownership_get_state(sid));
			state.world.market_for_each_trade_route(mid, [&](auto trade_route) {
				auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
				auto target =
					current_volume <= 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				if(target == mid) {
					trade_and_tariff route_data = explain_trade_route_commodity(state, trade_route, cid);
					result += route_data.tariff_target;
				}
			});
		});
	});
	return result;
}

float estimate_tariff_export_income(sys::state& state, dcon::nation_id n) {
	float result = 0.f;
	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		state.world.nation_for_each_state_ownership(n, [&](auto sid) {
			auto mid = state.world.state_instance_get_market_from_local_market(state.world.state_ownership_get_state(sid));
			state.world.market_for_each_trade_route(mid, [&](auto trade_route) {
				auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
				auto origin =
					current_volume > 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				if(origin == mid) {
					trade_and_tariff route_data = explain_trade_route_commodity(state, trade_route, cid);
					result += route_data.tariff_origin;
				}
			});
		});
	});
	return result;
}

float estimate_social_spending(sys::state& state, dcon::nation_id n) {
	auto base_budget = state.world.nation_get_stockpiles(n, economy::money);
	auto const p_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level));
	auto const unemp_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit));

	if(p_level + unemp_level == 0.f) {
		return 0.f;
	}

	auto social_budget = base_budget * float(state.world.nation_get_social_spending(n)) / 100.0f;

	return social_budget;
}

float estimate_education_spending(sys::state& state, dcon::nation_id n) {
	auto base_budget = state.world.nation_get_stockpiles(n, economy::money);
	return float(state.world.nation_get_education_spending(n)) * base_budget / 100.f;
}

float estimate_pop_payouts_by_income_type(sys::state& state, dcon::nation_id n, culture::income_type in) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			auto adj_pop_of_type =
				state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt)) / state.defines.alice_needs_scaling_factor;

			if(adj_pop_of_type <= 0)
				return;

			auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
			if(ln_type == in) {
				total += adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
			}

			auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
			if(en_type == in) {
				total += adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
			}

			auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
			if(lx_type == in) {
				total += adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
			}
		});
	});
	return total * payouts_spending_multiplier;
}

float estimate_subsidy_spending(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_subsidies_spending(n);
}

float estimate_war_subsidies_income(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	for(auto uni : state.world.nation_get_unilateral_relationship_as_target(n)) {
		if(uni.get_war_subsidies()) {
			total += estimate_war_subsidies(state, uni.get_target(), uni.get_source());
		}
	}
	return total;
}
float estimate_reparations_income(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	for(auto uni : state.world.nation_get_unilateral_relationship_as_target(n)) {
		if(uni.get_reparations() && state.current_date < uni.get_source().get_reparations_until()) {
			auto source = uni.get_source();
			auto const tax_eff = nations::tax_efficiency(state, n);
			auto total_tax_base = state.world.nation_get_total_rich_income(source) +
				state.world.nation_get_total_middle_income(source) +
				state.world.nation_get_total_poor_income(source);
			auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
			total += payout;
		}
	}
	return total;
}

float estimate_war_subsidies_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	for(auto uni : state.world.nation_get_unilateral_relationship_as_source(n)) {
		if(uni.get_war_subsidies()) {
			total += estimate_war_subsidies(state, uni.get_target(), uni.get_source());
		}
	}

	return total;
}

float estimate_reparations_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	if(state.current_date < state.world.nation_get_reparations_until(n)) {
		for(auto uni : state.world.nation_get_unilateral_relationship_as_source(n)) {
			if(uni.get_reparations()) {
				auto const tax_eff = nations::tax_efficiency(state, n);
				auto total_tax_base = state.world.nation_get_total_rich_income(n) +
					state.world.nation_get_total_middle_income(n) +
					state.world.nation_get_total_poor_income(n);
				auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
				total += payout;
			}
		}
	}
	return total;
}

float estimate_diplomatic_balance(sys::state& state, dcon::nation_id n) {
	float w_sub = estimate_war_subsidies_income(state, n) - estimate_war_subsidies_spending(state, n);
	float w_reps = estimate_reparations_income(state, n) - estimate_reparations_spending(state, n);
	float subject_payments = estimate_subject_payments_received(state, n) - estimate_subject_payments_paid(state, n);
	return w_sub + w_reps + subject_payments;
}
float estimate_diplomatic_income(sys::state& state, dcon::nation_id n) {
	float w_sub = estimate_war_subsidies_income(state, n);
	float w_reps = estimate_reparations_income(state, n);
	float subject_payments = estimate_subject_payments_received(state, n);
	return w_sub + w_reps + subject_payments;
}
float estimate_diplomatic_expenses(sys::state& state, dcon::nation_id n) {
	float w_sub = estimate_war_subsidies_spending(state, n);
	float w_reps = estimate_reparations_spending(state, n);
	float subject_payments = estimate_subject_payments_paid(state, n);
	return w_sub + w_reps + subject_payments;
}



float estimate_max_domestic_investment(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_stockpiles(n, economy::money);
}

float estimate_current_domestic_investment(sys::state& state, dcon::nation_id n) {
	return estimate_max_domestic_investment(state, n) * float(state.world.nation_get_domestic_investment_spending(n)) / 100.0f;
}

float estimate_land_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			total +=
				state.world.market_get_army_demand(market, cid)
				* price(state, market, cid)
				* state.world.market_get_demand_satisfaction(market, cid);
		}
	});
	return total;
}

float estimate_naval_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			total += state.world.market_get_navy_demand(market, cid)
				* price(state, market, cid)
				* state.world.market_get_demand_satisfaction(market, cid);
		}
	});
	return total;
}

float estimate_war_subsidies(sys::state& state, dcon::nation_fat_id target, dcon::nation_fat_id source) {
	/* total-nation-tax-base x defines:WARSUBSIDIES_PERCENT */

	auto target_m_costs = (target.get_total_rich_income() + target.get_total_middle_income() + target.get_total_poor_income()) * state.defines.warsubsidies_percent;
	auto source_m_costs = (source.get_total_rich_income() + source.get_total_middle_income() + source.get_total_poor_income()) * state.defines.warsubsidies_percent;
	return std::min(target_m_costs, source_m_costs);
}

float estimate_subject_payments_paid(sys::state& state, dcon::nation_id n) {
	auto tax = explain_tax_income(state, n);
	auto collected_tax = tax.mid + tax.poor + tax.rich;

	auto rel = state.world.nation_get_overlord_as_subject(n);
	auto overlord = state.world.overlord_get_ruler(rel);

	if(overlord) {
		auto transferamt = collected_tax;

		if(state.world.nation_get_is_substate(n)) {
			transferamt *= state.defines.alice_substate_subject_money_transfer / 100.f;
		} else {
			transferamt *= state.defines.alice_puppet_subject_money_transfer / 100.f;
		}

		return std::max(0.f, std::min(state.world.nation_get_stockpiles(n, money), transferamt));
	}

	return 0;
}

float estimate_subject_payments_received(sys::state& state, dcon::nation_id o) {
	auto res = 0.0f;
	for(auto n : state.world.in_nation) {
		auto rel = state.world.nation_get_overlord_as_subject(n);
		auto overlord = state.world.overlord_get_ruler(rel);

		if(overlord == o) {
			auto tax = explain_tax_income(state, n);
			auto const collected_tax = tax.poor + tax.mid + tax.rich;
			auto transferamt = collected_tax;

			if(state.world.nation_get_is_substate(n)) {
				transferamt *= state.defines.alice_substate_subject_money_transfer / 100.f;
			} else {
				transferamt *= state.defines.alice_puppet_subject_money_transfer / 100.f;
			}

			res += transferamt;
		}
	}

	return res;
}

construction_status province_building_construction(sys::state& state, dcon::province_id p, province_building_type t) {
	assert(0 <= int32_t(t) && int32_t(t) < int32_t(economy::max_building_types));
	for(auto pb_con : state.world.province_get_province_building_construction(p)) {
		if(pb_con.get_type() == uint8_t(t)) {
			float modifier = build_cost_multiplier(state, p, pb_con.get_is_pop_project());
			float total = 0.0f;
			float purchased = 0.0f;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total +=
					state.economy_definitions.building_definitions[int32_t(t)].cost.commodity_amounts[i]
					* modifier;
				purchased += pb_con.get_purchased_goods().commodity_amounts[i];
			}
			return construction_status{ total > 0.0f ? purchased / total : 0.0f, true };
		}
	}
	return construction_status{ 0.0f, false };
}

construction_status factory_upgrade(sys::state& state, dcon::factory_id f) {
	auto in_prov = state.world.factory_get_province_from_factory_location(f);
	auto fac_type = state.world.factory_get_building_type(f);

	for(auto st_con : state.world.province_get_factory_construction(in_prov)) {
		if(st_con.get_type() == fac_type) {
			float modifier = factory_build_cost_multiplier(state, st_con.get_nation(), st_con.get_province(), st_con.get_is_pop_project());
			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(fac_type);
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i] * modifier;
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}
			return construction_status{ total > 0.0f ? purchased / total : 0.0f, true };
		}
	}

	return construction_status{ 0.0f, false };
}

float unit_construction_progress(sys::state& state, dcon::province_land_construction_id c) {
	auto pop = state.world.province_land_construction_get_pop(c);
	auto province = state.world.pop_get_province_from_pop_location(pop);
	float cost_factor = economy::build_cost_multiplier(state, province, false);

	auto& goods = state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(c)].build_cost;
	auto& cgoods = state.world.province_land_construction_get_purchased_goods(c);

	float total = 0.0f;
	float purchased = 0.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		total += goods.commodity_amounts[i] * cost_factor;
		purchased += cgoods.commodity_amounts[i];
	}

	auto construction_time = state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(c)].build_time;
	auto time_progress = (float) sys::days_difference(state.world.province_land_construction_get_start_date(c).to_ymd(state.start_date), state.current_date.to_ymd(state.start_date)) / (float) construction_time;

	return std::min(time_progress, purchased / total);
}

float unit_construction_progress(sys::state& state, dcon::province_naval_construction_id c) {
	auto province = state.world.province_naval_construction_get_province(c);
	float cost_factor = economy::build_cost_multiplier(state, province, false);

	auto& goods = state.military_definitions.unit_base_definitions[state.world.province_naval_construction_get_type(c)].build_cost;
	auto& cgoods = state.world.province_naval_construction_get_purchased_goods(c);

	float total = 0.0f;
	float purchased = 0.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		total += goods.commodity_amounts[i] * cost_factor;
		purchased += cgoods.commodity_amounts[i];
	}

	return total > 0.0f ? purchased / total : 0.0f;
}

void add_factory_level_to_province(sys::state& state, dcon::province_id p, dcon::factory_type_id t) {

	// Since construction may be queued together with refit, check if there is factory to add level to
	for(auto f : state.world.province_get_factory_location(p)) {
		auto fac = f.get_factory();
		auto fac_type = fac.get_building_type();
		if(f.get_factory().get_building_type() == t) {
			f.get_factory().set_size(f.get_factory().get_size() + fac_type.get_base_workforce());
			return;
		}
	}

	// Only then create new factory
	auto new_fac = fatten(state.world, state.world.create_factory());
	new_fac.set_building_type(t);
	float base_size = float(state.world.factory_type_get_base_workforce(t));
	new_fac.set_size(base_size * 0.1f);
	new_fac.set_unqualified_employment(base_size * 0.1f);
	new_fac.set_primary_employment(base_size * 0.1f);
	new_fac.set_secondary_employment(base_size * 0.1f);
	state.world.try_create_factory_location(new_fac, p);
}

void change_factory_type_in_province(sys::state& state, dcon::province_id p, dcon::factory_type_id t, dcon::factory_type_id refit_target) {
	assert(refit_target);
	dcon::factory_id factory_target;

	// Find factory in question
	for(auto f : state.world.province_get_factory_location(p)) {
		if(f.get_factory().get_building_type() == t) {
			factory_target = f.get_factory();
		}
	}

	// Find existing factories to sum
	for(auto f : state.world.province_get_factory_location(p))
		if(f.get_factory().get_building_type() == refit_target) {
			auto factory_size_1 = f.get_factory().get_size();
			auto factory_size_2 = state.world.factory_get_size(factory_target);
			f.get_factory().set_size(factory_size_1 + factory_size_2);

			state.world.delete_factory(factory_target);
			return;
		}

	// Change type of the given factory
	for(auto f : state.world.province_get_factory_location(p)) {
		if(f.get_factory().get_building_type() == t) {
			f.get_factory().set_building_type(refit_target);
			return;
		}
	}
}

void resolve_constructions(sys::state& state) {
	// US1. Regiment construction
	// US1AC7.
	for(auto c : state.world.in_province_land_construction) {
		auto pop = state.world.province_land_construction_get_pop(c);
		auto province = state.world.pop_get_province_from_pop_location(pop);
		float cost_factor = economy::build_cost_multiplier(state, province, false);

		auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
		auto& current_purchased = c.get_purchased_goods();
		auto construction_time = state.military_definitions.unit_base_definitions[c.get_type()].build_time;

		// US1AC4. All goods costs must be built
		bool ready_for_deployment = true;
		if(!(c.get_nation().get_is_player_controlled() && state.cheat_data.instant_army)) {
			for(uint32_t j = 0; j < commodity_set::set_size && ready_for_deployment; ++j) {
				if(base_cost.commodity_type[j]) {
					if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * cost_factor) {
						ready_for_deployment = false;
					}
				} else {
					break;
				}
			}
		}

		// US1AC5. But no faster than construction_time
		if(!state.cheat_data.instant_army) {
			if(state.current_date < c.get_start_date() + construction_time) {
				ready_for_deployment = false;
			}
		}

		if(ready_for_deployment) {
			military::spawn_regiment(state, c.get_nation(), c.get_type(), c.get_pop(), c.get_template_province());

			state.world.delete_province_land_construction(c);
		}
	}

	// US2 Ships construction
	// US2AC7
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() != rng.end()) {
			auto c = *(rng.begin());

			auto province = state.world.province_naval_construction_get_province(c);
			float cost_factor = economy::build_cost_multiplier(state, province, false);

			auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
			auto& current_purchased = c.get_purchased_goods();
			auto construction_time = state.military_definitions.unit_base_definitions[c.get_type()].build_time;

			// US2AC4.
			bool ready_for_deployment = true;
			if(!(c.get_nation().get_is_player_controlled() && state.cheat_data.instant_navy)) {
				for(uint32_t i = 0; i < commodity_set::set_size && ready_for_deployment; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * cost_factor) {
							ready_for_deployment = false;
						}
					} else {
						break;
					}
				}
			}

			// US2AC5. But no faster than construction_time
			if(!state.cheat_data.instant_army) {
				if(state.current_date < c.get_start_date() + construction_time) {
					ready_for_deployment = false;
				}
			}

			if(ready_for_deployment) {
				military::spawn_ship(state, c.get_nation(), c.get_type(), c.get_province(), c.get_template_province());

				state.world.delete_province_naval_construction(c);
			}
		}
	});

	// Construction of province buildings
	for(auto c : state.world.in_province_building_construction) {
		auto for_province = c.get_province();
		float cost_factor = economy::build_cost_multiplier(state, for_province, c.get_is_pop_project());

		auto t = province_building_type(state.world.province_building_construction_get_type(c));
		assert(0 <= int32_t(t) && int32_t(t) < int32_t(economy::max_building_types));
		auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
		auto& current_purchased = state.world.province_building_construction_get_purchased_goods(c);
		bool all_finished = true;

		for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
			if(base_cost.commodity_type[j]) {
				if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * cost_factor) {
					all_finished = false;
				}
			} else {
				break;
			}
		}

		if(all_finished) {
			if(state.world.province_get_building_level(for_province, uint8_t(t)) < state.world.nation_get_max_building_level(state.world.province_get_nation_from_province_ownership(for_province), uint8_t(t))) {
				state.world.province_set_building_level(for_province, uint8_t(t), uint8_t(state.world.province_get_building_level(for_province, uint8_t(t)) + 1));

				if(t == province_building_type::railroad) {
					/* Notify the railroad mesh builder to update the railroads! */
					state.railroad_built.store(true, std::memory_order::release);
				}

				if(state.world.province_building_construction_get_nation(c) == state.local_player_nation) {
					switch(t) {
					case province_building_type::naval_base:
						notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "amsg_naval_base_complete");
							},
							"amsg_naval_base_complete",
							state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
							sys::message_base_type::naval_base_complete
						});
						break;
					case province_building_type::fort:
						notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "amsg_fort_complete");
							},
							"amsg_fort_complete",
							state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
							sys::message_base_type::fort_complete
						});
						break;
					case province_building_type::railroad:
						notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "amsg_rr_complete");
							},
							"amsg_rr_complete",
							state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
							sys::message_base_type::rr_complete
						});
						break;
					default:
						break;
					}
				}
			}
			state.world.delete_province_building_construction(c);
		}
	}

	// Construction of factories
	for(auto c : state.world.in_factory_construction) {
		auto n = state.world.factory_construction_get_nation(c);
		auto type = state.world.factory_construction_get_type(c);
		auto base_cost = (c.get_refit_target()) ? calculate_factory_refit_goods_cost(state, n, c.get_province(), c.get_type(), c.get_refit_target()) : state.world.factory_type_get_construction_costs(type);
		auto& current_purchased = state.world.factory_construction_get_purchased_goods(c);
		float factory_mod = factory_build_cost_multiplier(state, n, c.get_province(), c.get_is_pop_project());

		if(!state.world.factory_construction_get_is_pop_project(c)) {
			bool all_finished = true;
			if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
				for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
					if(base_cost.commodity_type[j]) {
						if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * factory_mod) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
			}
			if(all_finished && c.get_refit_target()) {
				change_factory_type_in_province(
					state,
					c.get_province(),
					type,
					c.get_refit_target()
				);
				state.world.delete_factory_construction(c);
			}
			else if (all_finished) {
				add_factory_level_to_province(
					state, c.get_province(), type
				);
				state.world.delete_factory_construction(c);
			}
		} else {
			bool all_finished = true;
			if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
				for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
					if(base_cost.commodity_type[j]) {
						if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * factory_mod) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
			}
			if(all_finished && c.get_refit_target()) {
				change_factory_type_in_province(state, state.world.factory_construction_get_province(c), type,
						c.get_refit_target());
				state.world.delete_factory_construction(c);
			} else if(all_finished) {
				add_factory_level_to_province(state, state.world.factory_construction_get_province(c), type);

				if(state.world.factory_construction_get_nation(c) == state.local_player_nation) {
					notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "amsg_factory_complete");
						},
						"amsg_factory_complete",
						state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::factory_complete
					});
				}

				state.world.delete_factory_construction(c);
			}
		}
	}
}

/* TODO -
 * This should return what we think the income will be next day, and as a result wont account for any unprecedented actions
 * return value is passed directly into text::fp_currency{} without adulteration.
 */
float estimate_daily_income(sys::state& state, dcon::nation_id n) {
	auto tax = explain_tax_income(state, n);
	return tax.mid + tax.poor + tax.rich;
}

void try_add_factory_to_state(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id t) {
	auto province = state.world.state_instance_get_capital(s);

	if(state.world.factory_type_get_is_coastal(t)) {
		if(!state.world.province_get_port_to(province))
			return; // requires coast to build coastal factory
	}

	auto existing_constructions = state.world.province_get_factory_construction(province);
	int32_t num_factories = 0;
	for(auto prj : existing_constructions) {
		if(!prj.get_is_upgrade())
			++num_factories;
		if(prj.get_type() == t)
			return; // can't duplicate type
	}

	// is there an upgrade target ?
	for(auto f : state.world.province_get_factory_location(province)) {
		++num_factories;
		if(f.get_factory().get_building_type() == t)
			return; // can't build another of this type
	}

	if(num_factories < int32_t(state.defines.factories_per_state)) {
		add_factory_level_to_province(state, province, t);
	}
}

command::budget_settings_data budget_minimums(sys::state& state, dcon::nation_id n) {
	command::budget_settings_data result;
	result.education_spending = 0;
	result.military_spending = 0;
	result.administrative_spending = 0;
	result.social_spending = 0;
	result.land_spending = 0;
	result.naval_spending = 0;
	result.construction_spending = 0;
	result.poor_tax = 0;
	result.middle_tax = 0;
	result.rich_tax = 0;
	result.tariffs_import = 0;
	result.tariffs_export = 0;
	result.domestic_investment = 0;
	result.overseas = 0;

	{
		auto min_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tariff));
		result.tariffs_import = int8_t(std::clamp(min_tariff, 0, 100));
		result.tariffs_export = int8_t(std::clamp(min_tariff, 0, 100));
	}
	{
		auto min_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tax));
		result.poor_tax = int8_t(std::clamp(min_tax, 0, 100));
		result.middle_tax = int8_t(std::clamp(min_tax, 0, 100));
		result.rich_tax = int8_t(std::clamp(min_tax, 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_military_spending));
		result.military_spending = int8_t(std::clamp(min_spend, 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_social_spending));
		result.social_spending = int8_t(std::clamp(min_spend, 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_domestic_investment));
		result.domestic_investment = int8_t(std::clamp(min_spend, 0, 100));
	}
	return result;
}
command::budget_settings_data budget_maximums(sys::state& state, dcon::nation_id n) {
	command::budget_settings_data result;
	result.education_spending = 100;
	result.military_spending = 100;
	result.administrative_spending = 100;
	result.social_spending = 100;
	result.land_spending = 100;
	result.naval_spending = 100;
	result.construction_spending = 100;
	result.poor_tax = 100;
	result.middle_tax = 100;
	result.rich_tax = 100;
	result.tariffs_import = 100;
	result.tariffs_export = 100;
	result.domestic_investment = 100;
	result.overseas = 100;

	{
		auto min_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tariff));
		auto max_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_tariff));
		max_tariff = std::max(min_tariff, max_tariff);

		result.tariffs_import = int8_t(std::clamp(max_tariff, 0, 100));
		result.tariffs_export = int8_t(std::clamp(max_tariff, 0, 100));
	}
	{
		auto min_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tax));
		auto max_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_tax));
		if(max_tax <= 0)
			max_tax = 100;
		max_tax = std::max(min_tax, max_tax);

		result.poor_tax = int8_t(std::clamp(max_tax, 0, 100));
		result.middle_tax = int8_t(std::clamp(max_tax, 0, 100));
		result.rich_tax = int8_t(std::clamp(max_tax, 0, 100));
	}
	{
		auto min_spend =
			int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_military_spending));
		auto max_spend =
			int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_military_spending));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		result.military_spending = int8_t(std::clamp(max_spend, 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_social_spending));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_social_spending));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		result.social_spending = int8_t(std::clamp(max_spend, 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_domestic_investment));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_domestic_investment));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		result.domestic_investment = int8_t(std::clamp(max_spend, 0, 100));
	}
	return result;
}
void bound_budget_settings(sys::state& state, dcon::nation_id n) {
	{
		auto min_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tariff));
		auto max_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_tariff));
		max_tariff = std::max(min_tariff, max_tariff);

		{
			auto& tariff = state.world.nation_get_tariffs_import(n);
			state.world.nation_set_tariffs_import(n, int8_t(std::clamp(std::clamp(int32_t(tariff), min_tariff, max_tariff), 0, 100)));
		}

		{
			auto& tariff = state.world.nation_get_tariffs_export(n);
			state.world.nation_set_tariffs_export(n, int8_t(std::clamp(std::clamp(int32_t(tariff), min_tariff, max_tariff), 0, 100)));
		}
	}
	{
		auto min_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tax));
		auto max_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_tax));
		if(max_tax <= 0)
			max_tax = 100;
		max_tax = std::max(min_tax, max_tax);

		auto& ptax = state.world.nation_get_poor_tax(n);
		state.world.nation_set_poor_tax(n, int8_t(std::clamp(std::clamp(int32_t(ptax), min_tax, max_tax), 0, 100)));
		auto& mtax = state.world.nation_get_middle_tax(n);
		state.world.nation_set_middle_tax(n, int8_t(std::clamp(std::clamp(int32_t(mtax), min_tax, max_tax), 0, 100)));
		auto& rtax = state.world.nation_get_rich_tax(n);
		state.world.nation_set_rich_tax(n, int8_t(std::clamp(std::clamp(int32_t(rtax), min_tax, max_tax), 0, 100)));
	}
	{
		auto min_spend =
			int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_military_spending));
		auto max_spend =
			int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_military_spending));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_military_spending(n);
		state.world.nation_set_military_spending(n, int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100)));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_social_spending));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_social_spending));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_social_spending(n);
		state.world.nation_set_social_spending(n, int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100)));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_domestic_investment));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_domestic_investment));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_domestic_investment_spending(n);
		state.world.nation_set_domestic_investment_spending(n, int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100)));
	}
}

void prune_factories(sys::state& state) {
	for(auto pid : state.world.in_province) {
		auto owner = pid.get_nation_from_province_ownership();
		auto rules = owner.get_combined_issue_rules();

		if(owner.get_is_player_controlled() && (rules & issue_rule::destroy_factory) != 0) // not for players who can manually destroy
			continue;

		dcon::factory_id deletion_choice;
		int32_t factory_count = 0;

		for(auto f : state.world.province_get_factory_location(pid)) {
			++factory_count;
			auto desired_employment = factory_total_desired_employment(state, f.get_factory());
			bool unprofitable = f.get_factory().get_unprofitable();
			if(((desired_employment < 10.f) && unprofitable) && (!deletion_choice || state.world.factory_get_size(deletion_choice) > f.get_factory().get_size())) {
				deletion_choice = f.get_factory();
			}
		}

		// aggressive pruning
		// to help building more healthy economy instead of 1 profitable giant factory with 6 small 0 scale factories
		if(deletion_choice && (4 + factory_count) >= int32_t(state.defines.factories_per_state)) {
			auto production_type = state.world.factory_get_building_type(deletion_choice);
			state.world.delete_factory(deletion_choice);

			for(auto proj : pid.get_factory_construction()) {
				if(proj.get_type() == production_type) {
					state.world.delete_factory_construction(proj);
					break;
				}
			}
		}
	}
}

dcon::modifier_id get_province_selector_modifier(sys::state& state) {
	return state.economy_definitions.selector_modifier;
}

dcon::modifier_id get_province_immigrator_modifier(sys::state& state) {
	return state.economy_definitions.immigrator_modifier;
}

void go_bankrupt(sys::state& state, dcon::nation_id n) {
	auto& debt = state.world.nation_get_local_loan(n);

	/*
	 If a nation cannot pay and the amount it owes is less than define:SMALL_DEBT_LIMIT, the nation it owes money to gets an on_debtor_default_small event (with the nation defaulting in the from slot). Otherwise, the event is pulled from on_debtor_default. The nation then goes bankrupt. It receives the bad_debter modifier for define:BANKRUPCY_EXTERNAL_LOAN_YEARS years (if it goes bankrupt again within this period, creditors receive an on_debtor_default_second event). It receives the in_bankrupcy modifier for define:BANKRUPCY_DURATION days. Its prestige is reduced by a factor of define:BANKRUPCY_FACTOR, and each of its pops has their militancy increase by 2.
	*/
	auto existing_br = state.world.nation_get_bankrupt_until(n);
	if(existing_br && state.current_date < existing_br) {
		for(auto gn : state.great_nations) {
			if(gn.nation && gn.nation != n) {
				event::fire_fixed_event(state, state.national_definitions.on_debtor_default_second, trigger::to_generic(gn.nation), event::slot_type::nation, gn.nation, trigger::to_generic(n), event::slot_type::nation);
			}
		}
	} else if(debt >= -state.defines.small_debt_limit) {
		for(auto gn : state.great_nations) {
			if(gn.nation && gn.nation != n) {
				event::fire_fixed_event(state, state.national_definitions.on_debtor_default_small, trigger::to_generic(gn.nation), event::slot_type::nation, gn.nation, trigger::to_generic(n), event::slot_type::nation);
			}
		}
	} else {
		for(auto gn : state.great_nations) {
			if(gn.nation && gn.nation != n) {
				event::fire_fixed_event(state, state.national_definitions.on_debtor_default, trigger::to_generic(gn.nation), event::slot_type::nation, gn.nation, trigger::to_generic(n), event::slot_type::nation);
			}
		}
	}

	// RESET MONEY: POTENTIAL MERGE CONFLICT WITH SNEAKBUG'S FUTURE CHANGES
	state.world.nation_set_stockpiles(n, economy::money, 0.f);

	sys::add_modifier_to_nation(state, n, state.national_definitions.in_bankrupcy, state.current_date + int32_t(state.defines.bankrupcy_duration * 365));
	sys::add_modifier_to_nation(state, n, state.national_definitions.bad_debter, state.current_date + int32_t(state.defines.bankruptcy_external_loan_years * 365));

	state.world.nation_set_local_loan(n, 0.0f);
	state.world.nation_set_is_debt_spending(n, false);
	state.world.nation_set_bankrupt_until(n, state.current_date + int32_t(state.defines.bankrupcy_duration * 365));

	notification::post(state, notification::message{
		[n](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_bankruptcy_1", text::variable_type::x, n);
		},
		"msg_bankruptcy_title",
		n, dcon::nation_id{}, dcon::nation_id{},
		sys::message_base_type::bankruptcy
	});
}

float estimate_investment_pool_daily_loss(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_private_investment(n) * 0.001f;
}

// Does this commodity has any factory using potentials mechanic
// Since in vanilla there are no such factories, it will return false.
bool get_commodity_uses_potentials(sys::state& state, dcon::commodity_id c) {
	for(auto type : state.world.in_factory_type) {
		auto output = type.get_output();
		if(output == c && output.get_uses_potentials()) {
			return true;
		}
	}
	return false;
}

float calculate_province_factory_limit(sys::state& state, dcon::province_id pid, dcon::commodity_id c) {
	return state.world.province_get_factory_max_size(pid, c);
}

float calculate_state_factory_limit(sys::state& state, dcon::state_instance_id sid, dcon::commodity_id c) {
	float result = 0.f;
	province::for_each_province_in_state_instance(state, sid, [&](dcon::province_id pid) {
		result += calculate_province_factory_limit(state, pid, c);
	});
	return result;
}

float calculate_nation_factory_limit(sys::state& state, dcon::nation_id nid, dcon::commodity_id c) {
	float res = 0;
	for(auto p : state.world.in_province) {
		if(p.get_nation_from_province_ownership() != nid) {
			continue;
		}

		if(p.get_factory_limit_was_set_during_scenario_creation()) {
			res += calculate_province_factory_limit(state, p, c);
		}
	}

	return res;
}

bool do_resource_potentials_allow_construction(sys::state& state, [[maybe_unused]] dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type) {
	/* If mod uses Factory Province limits */
	auto output = state.world.factory_type_get_output(type);
	auto limit = economy::calculate_province_factory_limit(state, location, output);

	if(!output.get_uses_potentials()) {
		return true;
	}

	// Is there a potential for this commodity limit?
	if(limit <= 0) {
		return false;
	}

	return true;
}

bool do_resource_potentials_allow_upgrade(sys::state& state, [[maybe_unused]] dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type) {
	/* If mod uses Factory Province limits */
	auto output = state.world.factory_type_get_output(type);

	if(!output.get_uses_potentials()) {
		return true;
	}

	auto limit = economy::calculate_province_factory_limit(state, location, output);

	// Will upgrade put us over the limit?
	auto existing_levels = 0.f;
	for(auto f : state.world.province_get_factory_location(location)) {
		if(f.get_factory().get_building_type() == type) {
			existing_levels += f.get_factory().get_size();
		}
		if(existing_levels + state.world.factory_type_get_base_workforce(type) > limit) {
			return false;
		}
	}

	return true;
}

bool do_resource_potentials_allow_refit(sys::state& state, [[maybe_unused]] dcon::nation_id source, dcon::province_id location, dcon::factory_type_id from, dcon::factory_type_id refit_target) {
	/* If mod uses Factory Province limits */
	auto output = state.world.factory_type_get_output(from);
	auto limit = economy::calculate_province_factory_limit(state, location, output);

	if(!output.get_uses_potentials()) {
		return true;
	}

	auto refit_levels = 0.f;
	// How many levels changed factory has
	for(auto f : state.world.province_get_factory_location(location)) {
		if(f.get_factory().get_building_type() == from) {
			refit_levels = f.get_factory().get_size();
		}
	}
	// Will that put us over the limit?
	auto existing_levels = 0.f;
	for(auto f : state.world.province_get_factory_location(location)) {
		if(f.get_factory().get_building_type() == refit_target) {
			existing_levels += f.get_factory().get_size();
		}
		if(existing_levels + refit_levels > limit) {
			return false;
		}
	}

	return true;
}

} // namespace economy
