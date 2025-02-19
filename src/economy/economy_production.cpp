#include "dcon_generated.hpp"
#include "system_state.hpp"

#include "economy_production.hpp"
#include "economy_stats.hpp"
#include "demographics.hpp"

#include "province_templates.hpp"

namespace economy {
template<typename T>
auto artisan_input_multiplier(
	sys::state& state,
	T nations
) {
	auto alice_input_base = state.defines.alice_inputs_base_factor_artisans;
	auto nation_input_mod = 1.f + state.world.nation_get_modifier_values(
		nations, sys::national_mod_offsets::artisan_input);

	if constexpr(std::is_same_v<T, dcon::nation_id>) {
		return std::max(alice_input_base * nation_input_mod, 0.01f);
	} else {
		return ve::max(alice_input_base * nation_input_mod, 0.01f);
	}
}
template<typename T>
ve::fp_vector ve_artisan_min_wage(sys::state& state, T markets) {
	auto life = state.world.market_get_life_needs_costs(markets, state.culture_definitions.artisans);
	auto everyday = state.world.market_get_everyday_needs_costs(markets, state.culture_definitions.artisans);
	return (life + everyday) * artisans_greed;
}
template<typename T>
auto artisan_output_multiplier(
	sys::state& state,
	T nations
) {
	auto alice_output_base = state.defines.alice_output_base_factor_artisans;
	auto nation_output_mod = 1.f + state.world.nation_get_modifier_values(
		nations, sys::national_mod_offsets::artisan_output);

	if constexpr(std::is_same_v<T, dcon::nation_id>) {
		return std::max(alice_output_base * nation_output_mod, 0.01f);
	} else {
		return ve::max(alice_output_base * nation_output_mod, 0.01f);
	}
}
template<typename T>
auto artisan_throughput_multiplier(
	sys::state& state,
	T nations
) {
	return production_throughput_multiplier * ve::max(
		0.01f,
		1.0f
		+ state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::artisan_throughput)
	);
}
template<typename T, typename S>
ve::fp_vector base_artisan_profit(
	sys::state& state,
	T markets,
	S nations,
	dcon::commodity_id c,
	ve::fp_vector predicted_price
) {
	auto const& inputs = state.world.commodity_get_artisan_inputs(c);
	ve::fp_vector input_total = 0.0f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total = input_total + inputs.commodity_amounts[i] * ve_price(state, markets, inputs.commodity_type[i]);
		} else {
			break;
		}
	}

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * predicted_price * state.world.market_get_supply_sold_ratio(markets, c);

	auto input_multiplier = artisan_input_multiplier(state, nations);
	auto output_multiplier = artisan_output_multiplier(state, nations);

	return output_total * output_multiplier - input_multiplier * input_total;
}
float base_artisan_profit(
	sys::state& state,
	dcon::market_id market,
	dcon::commodity_id c,
	float predicted_price
) {
	auto sid = state.world.market_get_zone_from_local_market(market);
	auto nid = state.world.state_instance_get_nation_from_state_ownership(sid);

	auto const& inputs = state.world.commodity_get_artisan_inputs(c);
	auto input_total = 0.0f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total = input_total + inputs.commodity_amounts[i] * price(state, market, inputs.commodity_type[i]);
		} else {
			break;
		}
	}

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * predicted_price * state.world.market_get_supply_sold_ratio(market, c);

	auto input_multiplier = artisan_input_multiplier<dcon::nation_id>(state, nid);
	auto output_multiplier = artisan_output_multiplier<dcon::nation_id>(state, nid);

	return output_total * output_multiplier - input_multiplier * input_total;
}

float rgo_efficiency(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::commodity_id c) {
	bool is_mine = state.world.commodity_get_is_mine(c);

	float main_rgo = 1.f;
	auto rgo = state.world.province_get_rgo(p);
	if(rgo == c) {
		main_rgo = state.defines.alice_base_rgo_efficiency_bonus;
	}

	float base_amount = state.world.commodity_get_rgo_amount(c);
	float throughput =
		1.0f
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_throughput)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_throughput)
		+ state.world.province_get_modifier_values(p,
			is_mine ?
			sys::provincial_mod_offsets::mine_rgo_eff
			:
			sys::provincial_mod_offsets::farm_rgo_eff)
		+ state.world.nation_get_modifier_values(n,
			is_mine ?
			sys::national_mod_offsets::mine_rgo_eff
			:
			sys::national_mod_offsets::farm_rgo_eff);

	float result = base_amount
		* main_rgo
		* std::max(0.5f, throughput)
		* state.defines.alice_rgo_boost
		* std::max(0.5f, (1.0f + state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_output) +
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_output) +
			state.world.nation_get_rgo_goods_output(n, c)));

	assert(result >= 0.0f && std::isfinite(result));
	return result;
}

float rgo_full_production_quantity(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::commodity_id c) {
	/*
	- We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size +
	technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
	- We add its production to domestic supply, calculating that amount basically in the same way we do for factories, by
	computing RGO-throughput x RGO-output x RGO-size x base-commodity-production-quantity, except that it is affected by different
	modifiers.
	*/
	auto eff_size = rgo_effective_size(state, n, p, c);
	auto val = eff_size * rgo_efficiency(state, n, p, c);
	assert(val >= 0.0f && std::isfinite(val));
	return val;
}

rgo_workers_breakdown rgo_relevant_population(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	auto relevant_paid_population = 0.f;
	for(auto wt : state.culture_definitions.rgo_workers) {
		relevant_paid_population += state.world.province_get_demographics(p, demographics::to_key(state, wt));
	}
	auto slaves = state.world.province_get_demographics(p, demographics::to_employment_key(state, state.culture_definitions.slaves));

	rgo_workers_breakdown result = {
		.paid_workers = relevant_paid_population,
		.slaves = slaves,
		.total = relevant_paid_population + slaves
	};

	return result;
}

float rgo_desired_worker_norm_profit(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n,
	float total_relevant_population
) {
	auto current_employment = rgo_total_employment(state, n, p); // maximal amount of workers which rgo could potentially employ

	//we assume a "perfect ratio" of 1 aristo per N pops
	float perfect_aristos_amount = total_relevant_population / 1000.f;
	float perfect_aristos_amount_adjusted = perfect_aristos_amount / state.defines.alice_needs_scaling_factor;
	float aristos_desired_cut = aristocrats_greed * perfect_aristos_amount_adjusted * (
		state.world.market_get_everyday_needs_costs(m, state.culture_definitions.aristocrat)
	);
	float aristo_burden_per_worker = aristos_desired_cut / (total_relevant_population + 1);

	float desired_profit_per_worker =
		aristo_burden_per_worker
		+ state.world.market_get_labor_price(m, labor::no_education);

	assert(std::isfinite(desired_profit_per_worker));
	return desired_profit_per_worker;
}

float rgo_expected_worker_norm_profit(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n,
	dcon::commodity_id c
) {
	auto efficiency = rgo_efficiency(state, n, p, c);
	auto current_price = price(state, m, c);

	return
		efficiency
		* current_price
		/ state.defines.alice_rgo_per_size_employment;
}

float base_artisan_profit(
	sys::state& state,
	dcon::market_id market,
	dcon::commodity_id c
) {
	auto sid = state.world.market_get_zone_from_local_market(market);
	auto nid = state.world.state_instance_get_nation_from_state_ownership(sid);

	auto const& inputs = state.world.commodity_get_artisan_inputs(c);
	auto input_total = 0.0f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total = input_total + inputs.commodity_amounts[i] * price(state, market, inputs.commodity_type[i]);
		} else {
			break;
		}
	}

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * price(state, market, c) * state.world.market_get_supply_sold_ratio(market, c);

	auto input_multiplier = artisan_input_multiplier<dcon::nation_id>(state, nid);
	auto output_multiplier = artisan_output_multiplier<dcon::nation_id>(state, nid);

	return output_total * output_multiplier - input_multiplier * input_total;
}
template<typename T>
ve::mask_vector ve_valid_artisan_good(
	sys::state& state,
	T nations,
	dcon::commodity_id cid
) {
	ve::mask_vector from_the_start = state.world.commodity_get_is_available_from_start(cid);
	ve::mask_vector active = state.world.nation_get_unlocked_commodities(nations, cid);
	auto can_produce = state.world.commodity_get_artisan_output_amount(cid) > 0.0f;

	return can_produce && (from_the_start || active);
}
bool valid_artisan_good(
	sys::state& state,
	dcon::nation_id nations,
	dcon::commodity_id cid
) {
	auto from_the_start = state.world.commodity_get_is_available_from_start(cid);
	auto active = state.world.nation_get_unlocked_commodities(nations, cid);
	auto can_produce = state.world.commodity_get_artisan_output_amount(cid) > 0.0f;

	return can_produce && (from_the_start || active);
}

template<typename M, typename SET>
ve_inputs_data get_inputs_data(sys::state& state, M markets, SET const& inputs) {
	ve::fp_vector input_total = 0.0f;
	ve::fp_vector min_available = 1.0f;
	for(uint32_t j = 0; j < SET::set_size; ++j) {
		if(inputs.commodity_type[j]) {
			input_total =
				input_total
				+ inputs.commodity_amounts[j]
				* ve_price(state, markets, inputs.commodity_type[j]);
			min_available = ve::min(
				min_available,
				state.world.market_get_demand_satisfaction(markets, inputs.commodity_type[j]));
		} else {
			break;
		}
	}
	return { min_available, input_total };
}
template<typename SET>
inputs_data get_inputs_data(sys::state& state, dcon::market_id markets, SET const& inputs) {
	float input_total = 0.0f;
	float min_available = 1.0f;
	for(uint32_t j = 0; j < SET::set_size; ++j) {
		if(inputs.commodity_type[j]) {
			input_total =
				input_total
				+ inputs.commodity_amounts[j]
				* price(state, markets, inputs.commodity_type[j]);
			min_available = std::min(
				min_available,
				state.world.market_get_demand_satisfaction(markets, inputs.commodity_type[j]));
		} else {
			break;
		}
	}
	return { min_available, input_total };
}

template<typename SET>
void register_inputs_demand(
	sys::state& state,
	dcon::market_id market,
	SET const& inputs,
	float scale,
	economy_reason reason
) {
	for(uint32_t i = 0; i < SET::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			register_intermediate_demand(state,
				market,
				inputs.commodity_type[i],
				scale * inputs.commodity_amounts[i],
				reason
			);
		} else {
			break;
		}
	}
}

template<typename M, typename SET>
void register_inputs_demand(
	sys::state& state,
	M market,
	SET const& inputs,
	ve::fp_vector scale,
	economy_reason reason
) {
	for(uint32_t i = 0; i < SET::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			register_intermediate_demand(state,
				market,
				inputs.commodity_type[i],
				scale * inputs.commodity_amounts[i],
				reason
			);
		} else {
			break;
		}
	}
}

inline constexpr float lack_of_efficiency_inputs_penalty = 0.25f;

float employment_units(
	sys::state& state,
	dcon::market_id market,
	float target_workers_no_education, float target_workers_basic_education, float target_workers_high_education, float employment_unit_size
) {
	assert(target_workers_no_education >= 0.f);
	assert(target_workers_basic_education >= 0.f);
	float workers_no_education = target_workers_no_education * state.world.market_get_labor_demand_satisfaction(market, labor::no_education);
	float workers_basic_education = target_workers_basic_education * state.world.market_get_labor_demand_satisfaction(market, labor::basic_education);
	assert(workers_no_education >= 0.f);
	assert(workers_basic_education >= 0.f);

	auto effective_employment = workers_no_education * unqualified_throughput_multiplier
		+ workers_basic_education;
	return effective_employment / employment_unit_size;
}

template<typename M>
ve::fp_vector employment_units_guild(
	sys::state& state,
	M market,
	ve::fp_vector target_workers_guild
) {
	ve::fp_vector guild_actual_size =
		target_workers_guild
		* state.world.market_get_labor_demand_satisfaction(market, labor::guild_education);
	return guild_actual_size / artisans_per_employment_unit;
}

float output_mulitplier_from_workers_with_high_education(
	sys::state& state,
	dcon::market_id market,
	float target_workers_high_education,
	float max_employment
) {
	assert(target_workers_high_education >= 0.f);
	float workers_high_education = target_workers_high_education * state.world.market_get_labor_demand_satisfaction(market, labor::high_education);
	assert(workers_high_education >= 0.f);
	if(max_employment < 1.f) {
		return 1.f;
	}
	return (1.f + economy::secondary_employment_output_bonus * workers_high_education / max_employment);
}

// returns employment units
float consume_labor(
	sys::state& state,
	dcon::market_id market,
	float target_workers_no_education,
	float target_workers_basic_education,
	float target_workers_high_education,
	float employment_unit_size
) {
	// register demand for labor
	state.world.market_get_labor_demand(market, labor::no_education) += target_workers_no_education;
	state.world.market_get_labor_demand(market, labor::basic_education) += target_workers_basic_education;
	state.world.market_get_labor_demand(market, labor::high_education) += target_workers_high_education;

	return employment_units(
		state, market,
		target_workers_no_education, target_workers_basic_education, target_workers_high_education, employment_unit_size
	);
}

template<typename M>
ve::fp_vector consume_labor_guild(
	sys::state& state,
	M market,
	ve::fp_vector target_guild_size
) {
	state.world.market_get_labor_demand(market, labor::guild_education) =
		state.world.market_get_labor_demand(market, labor::guild_education)
		+ target_guild_size;
	return employment_units_guild(state, market, target_guild_size);
}

struct preconsumption_data {
	inputs_data direct_inputs_data{ };
	inputs_data efficiency_inputs_data{ };

	float output_price = 0.f;
	float output_amount_per_production_unit = 0.f;
	float direct_inputs_cost_per_production_unit = 0.f;
	float efficiency_inputs_cost_per_production_unit = 0.f;
};

struct ve_preconsumption_data_guild {
	ve_inputs_data direct_inputs_data{ };

	ve::fp_vector output_price = 0.f;
	ve::fp_vector output_amount_per_production_unit = 0.f;
	ve::fp_vector direct_inputs_cost_per_production_unit = 0.f;
};

// if inputs are very costly, we demand less of goods
// if inputs are not that costly, we demand full amount
template<typename F>
F inputs_demand_reduction(F cost_of_inputs, F cost_of_output, F min_available) {
	auto min_input_importance =	cost_of_output / (cost_of_inputs + 0.00001f);
	if constexpr(std::is_same<F, float>::value)
		min_input_importance = std::min(std::max(min_input_importance, 0.f), 1.f);
	else
		min_input_importance = ve::min(ve::max(min_input_importance, 0.f), 1.f);
	return (min_input_importance + (1.f - min_input_importance) * min_available);
}

preconsumption_data prepare_data_for_consumption(
	sys::state& state,
	dcon::market_id market,
	economy::commodity_set const& inputs,
	economy::small_commodity_set const& efficiency_inputs,
	dcon::commodity_id output,
	float output_amount,

	float input_multiplier, float efficiency_inputs_multiplier, float output_multiplier
) {
	auto direct_inputs_data = get_inputs_data(state, market, inputs);
	auto efficiency_inputs_data = get_inputs_data(state, market, efficiency_inputs);
	assert(direct_inputs_data.min_available >= 0.f);
	assert(efficiency_inputs_data.min_available >= 0.f);

	auto output_price = price(state, market, output);

	auto output_per_production_unit =
		output_amount
		* output_multiplier;

	auto max_efficiency_penalty =
		output_per_production_unit
		* output_price
		* lack_of_efficiency_inputs_penalty;

	if(max_efficiency_penalty < efficiency_inputs_data.total_cost * efficiency_inputs_multiplier)
		efficiency_inputs_data.min_available = 0.f;

	output_per_production_unit =
		output_per_production_unit
		* (
			(1.f - lack_of_efficiency_inputs_penalty)
			+ lack_of_efficiency_inputs_penalty * efficiency_inputs_data.min_available
		);

	float output_cost_per_production_unit =
		output_per_production_unit
		* output_price;

	preconsumption_data result = {
		.direct_inputs_data = direct_inputs_data,
		.efficiency_inputs_data = efficiency_inputs_data,

		.output_price = output_price,
		.output_amount_per_production_unit = output_per_production_unit,
		.direct_inputs_cost_per_production_unit = direct_inputs_data.total_cost * input_multiplier,
		.efficiency_inputs_cost_per_production_unit = efficiency_inputs_data.total_cost * input_multiplier * efficiency_inputs_multiplier
	};

	return result;
}

template<typename M>
ve_preconsumption_data_guild prepare_data_for_consumption(
	sys::state& state,
	M market,
	economy::commodity_set const& inputs,
	dcon::commodity_id output,
	ve::fp_vector output_amount,
	ve::fp_vector input_multiplier, ve::fp_vector output_multiplier
) {
	auto direct_inputs_data = get_inputs_data(state, market, inputs);
	auto output_price = ve_price(state, market, output);
	auto output_per_production_unit =
		output_amount
		* output_multiplier;
	auto output_cost_per_production_unit =
		output_per_production_unit
		* output_price;
	ve_preconsumption_data_guild result = {
		.direct_inputs_data = direct_inputs_data,
		.output_price = output_price,
		.output_amount_per_production_unit = output_per_production_unit,
		.direct_inputs_cost_per_production_unit = direct_inputs_data.total_cost * input_multiplier,
	};
	return result;
}

struct consumption_data {
	float direct_inputs_cost;
	float efficiency_inputs_cost;
	float output;

	float input_cost_per_employment_unit;
	float output_per_employment_unit;
};
struct ve_consumption_data_guild {
	ve::fp_vector direct_inputs_cost;
	ve::fp_vector output;

	ve::fp_vector input_cost_per_employment_unit;
	ve::fp_vector output_per_employment_unit;
};

consumption_data consume(
	sys::state& state,
	dcon::market_id market,
	economy::commodity_set const& inputs,
	economy::small_commodity_set const& efficiency_inputs,
	preconsumption_data& additional_data,

	float input_multiplier, float efficiency_inputs_multiplier, float throughput_multiplier, float output_multiplier,
	float employment_units, float output_mulitplier_from_workers_with_high_education,
	float max_employment,
	economy_reason reason
) {
	assert(input_multiplier >= 0.f);
	assert(throughput_multiplier > 0.f);
	assert(output_multiplier >= 0.f);
	assert(efficiency_inputs_multiplier >= 0.f);
	
	float production_units = employment_units * throughput_multiplier;
	assert(production_units >= 0.f);

	float input_scale =
		input_multiplier
		* production_units
		* inputs_demand_reduction(
			additional_data.direct_inputs_cost_per_production_unit,
			additional_data.output_amount_per_production_unit * additional_data.output_price,
			additional_data.direct_inputs_data.min_available
		);
	assert(input_scale >= 0.f);

	float e_input_scale =
		input_multiplier
		* efficiency_inputs_multiplier
		* production_units
		* inputs_demand_reduction(
			additional_data.efficiency_inputs_cost_per_production_unit,
			additional_data.output_amount_per_production_unit * additional_data.output_price,
			additional_data.efficiency_inputs_data.min_available
		);
	assert(e_input_scale >= 0.f);

	register_inputs_demand(state, market, inputs, input_scale, reason);
	register_inputs_demand(state, market, efficiency_inputs, e_input_scale * efficiency_inputs_multiplier, reason);

	consumption_data result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,
		.efficiency_inputs_cost = additional_data.efficiency_inputs_cost_per_production_unit * production_units * additional_data.efficiency_inputs_data.min_available,
		.output = additional_data.output_amount_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit / throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit / throughput_multiplier
	};

	return result;
}

template<typename M>
ve_consumption_data_guild consume(
	sys::state& state,
	M market,
	economy::commodity_set const& inputs,
	ve_preconsumption_data_guild& additional_data,
	ve::fp_vector input_multiplier, ve::fp_vector throughput_multiplier, ve::fp_vector output_multiplier,
	ve::fp_vector employment_units,
	economy_reason reason
) {
	ve::fp_vector production_units = employment_units * throughput_multiplier;
	ve::fp_vector input_scale =
		input_multiplier
		* production_units
		* inputs_demand_reduction(
			additional_data.direct_inputs_cost_per_production_unit,
			additional_data.output_amount_per_production_unit * additional_data.output_price,
			additional_data.direct_inputs_data.min_available
		);
	register_inputs_demand(state, market, inputs, input_scale, reason);
	ve_consumption_data_guild result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,
		.output = additional_data.output_amount_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit / throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit / throughput_multiplier
	};

	return result;
}

template<typename T, typename S, typename U>
void update_artisan_consumption(
	sys::state& state,
	T markets,
	S nations,
	U states,
	ve::fp_vector mobilization_impact
) {
	auto const csize = state.world.commodity_size();
	ve::fp_vector total_profit = 0.0f;

	ve::fp_vector input_multiplier = artisan_input_multiplier(state, nations);
	ve::fp_vector throughput_multiplier = artisan_throughput_multiplier(state, nations);
	ve::fp_vector output_multiplier = artisan_output_multiplier(state, nations);

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto const& inputs = state.world.commodity_get_artisan_inputs(cid);
		auto output_amount = state.world.commodity_get_artisan_output_amount(cid);
		state.world.market_set_artisan_actual_production(markets, cid, 0.0f);
		auto valid_mask = ve_valid_artisan_good(state, nations, cid);
		ve::fp_vector target_workers = state.world.market_get_artisan_score(markets, cid);
		auto actual_workers =
			target_workers
			* state.world.market_get_labor_demand_satisfaction(markets, labor::guild_education)
			* mobilization_impact;
		consume_labor_guild(state, markets, actual_workers);
		ve_preconsumption_data_guild prepared_data = prepare_data_for_consumption(
			state, markets, inputs, cid, output_amount, input_multiplier, output_multiplier
		);
		ve_consumption_data_guild consumption_data = consume(
			state, markets,
			inputs, prepared_data,
			input_multiplier, throughput_multiplier, output_multiplier,
			actual_workers / economy::artisans_per_employment_unit, economy_reason::artisan
		);
		state.world.market_set_artisan_actual_production(
			markets,
			cid,
			consumption_data.output
		);
		total_profit = total_profit + ve::max(0.f,
			consumption_data.output * prepared_data.output_price - consumption_data.direct_inputs_cost
		);
	}
	state.world.market_set_artisan_profit(markets, total_profit);
}


float factory_min_input_available(
	sys::state const& state,
	dcon::market_id m,
	dcon::factory_type_id fac_type
) {
	float min_input_available = 1.0f;
	auto& inputs = state.world.factory_type_get_inputs(fac_type);
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			min_input_available =
				std::min(
					min_input_available,
					state.world.market_get_demand_satisfaction(m, inputs.commodity_type[i])
				);
		} else {
			break;
		}
	}
	return min_input_available;
}

float priority_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto exp = state.world.nation_get_factory_type_experience(n, fac_type);
	return 100000.f / (100000.f + exp);
}

float nation_factory_input_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto mult = priority_multiplier(state, fac_type, n);

	return mult * std::max(
		0.1f,
		state.defines.alice_inputs_base_factor
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input)
	);
}
float nation_factory_output_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto output = state.world.factory_type_get_output(fac_type);
	return state.world.nation_get_factory_goods_output(n, output)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_output)
		+ 1.0f;
}

float factory_input_multiplier(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s) {
	float total_workers = state.world.factory_get_size(fac);
	float small_size_effect = 1.f;
	auto factory_type = state.world.factory_get_building_type(fac);
	auto per_level_employment = state.world.factory_type_get_base_workforce(factory_type);
	float small_bound = per_level_employment * 5.f;
	if(total_workers < small_bound) {
		small_size_effect = 0.5f + total_workers / small_bound * 0.5f;
	}

	float total_state_pop = std::max(0.01f, state.world.state_instance_get_demographics(s, demographics::total));
	float capitalists = state.world.state_instance_get_demographics(s, demographics::to_key(state, state.culture_definitions.capitalists));
	float owner_fraction = total_state_pop > 0
		? std::min(
			0.05f,
			capitalists / total_state_pop)
		: 0.0f;

	auto ftid = state.world.factory_get_building_type(fac);
	auto mult = priority_multiplier(state, ftid, n);

	return std::max(0.001f, small_size_effect *
		state.world.factory_get_triggered_modifiers(fac) *
		std::max(
			0.1f,
			mult
			* (
				state.defines.alice_inputs_base_factor
				+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_input)
				+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input)
				+ owner_fraction * -2.5f
				)
		));
}

float factory_throughput_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s) {
	auto output = state.world.factory_type_get_output(fac_type);
	auto national_t = state.world.nation_get_factory_goods_throughput(n, output);
	auto provincial_fac_t = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput);
	auto nationnal_fac_t = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_throughput);

	auto result = 1.f
		* std::max(0.f, 1.f + national_t)
		* std::max(0.f, 1.f + provincial_fac_t)
		* std::max(0.f, 1.f + nationnal_fac_t);

	return production_throughput_multiplier * result;
}

float factory_output_multiplier_no_secondary_workers(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p) {
	auto fac_type = state.world.factory_get_building_type(fac);
	return std::max(0.f,
		state.world.nation_get_factory_goods_output(n, fac_type.get_output())
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_output)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_output)
		+ 1.0f
	);
}

float factory_throughput_additional_multiplier(sys::state const& state, dcon::factory_id fac, float mobilization_impact, bool occupied) {
	return (occupied ? 0.1f : 1.0f) * std::max(0.0f, mobilization_impact);
}

float get_factory_level(sys::state& state, dcon::factory_id f) {
	auto ftid = state.world.factory_get_building_type(f);
	return state.world.factory_get_size(f) / state.world.factory_type_get_base_workforce(ftid);
}


float get_total_wage(sys::state& state, dcon::factory_id f) {
	auto location = state.world.factory_get_province_from_factory_location(f);
	auto zone = state.world.province_get_state_membership(location);
	auto market = state.world.state_instance_get_market_from_local_market(zone);

	return state.world.market_get_labor_price(market, labor::no_education)
		* state.world.market_get_labor_demand_satisfaction(market, labor::no_education)
		* state.world.factory_get_unqualified_employment(f)
		+
		state.world.market_get_labor_price(market, labor::basic_education)
		* state.world.market_get_labor_demand_satisfaction(market, labor::basic_education)
		* state.world.factory_get_primary_employment(f)
		+
		state.world.market_get_labor_price(market, labor::high_education)
		* state.world.market_get_labor_demand_satisfaction(market, labor::high_education)
		* state.world.factory_get_secondary_employment(f);
}


profit_explanation explain_last_factory_profit(sys::state& state, dcon::factory_id f) {
	auto location = state.world.factory_get_province_from_factory_location(f);
	auto zone = state.world.province_get_state_membership(location);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto ftid = state.world.factory_get_building_type(f);
	auto output_commodity = state.world.factory_type_get_output(ftid);
	auto local_price = price(state, market, output_commodity);

	auto last_output = state.world.factory_get_output(f) * local_price;
	auto last_inputs = state.world.factory_get_input_cost(f);

	auto wages = get_total_wage(state, f);

	return {
		.inputs = last_inputs,
		.wages = wages,
		.output = last_output,
		.profit = last_output - wages - last_inputs
	};
}

void update_single_factory_consumption(
	sys::state& state,
	dcon::factory_id f,
	dcon::province_id p,
	dcon::state_instance_id s,
	dcon::market_id m,
	dcon::nation_id n,
	float mobilization_impact,
	bool occupied
) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();
	assert(fac_type);
	assert(fac_type.get_output());
	assert(n);
	assert(p);
	assert(s);

	//modifiers
	float input_multiplier = factory_input_multiplier(state, fac, n, p, s);
	auto const mfactor = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	float throughput_multiplier = factory_throughput_multiplier(state, fac_type, n, p, s);
	float output_multiplier = factory_output_multiplier_no_secondary_workers(state, fac, n, p);

	auto& direct_inputs = fac_type.get_inputs();
	auto& efficiency_inputs = fac_type.get_efficiency_inputs();

	auto base_data = prepare_data_for_consumption(
		state, m,
		direct_inputs, efficiency_inputs,
		fac_type.get_output(), fac_type.get_output_amount(),
		input_multiplier, mfactor, output_multiplier
	);

	auto max_employment = state.world.factory_get_size(fac);

	auto employment_units = consume_labor(
		state, m,
		fac.get_unqualified_employment(), fac.get_primary_employment(), fac.get_secondary_employment(),
		float(fac_type.get_base_workforce())
	) * mobilization_impact;

	auto data = consume(
		state, m,
		direct_inputs, efficiency_inputs,
		base_data,
		input_multiplier, mfactor, throughput_multiplier, output_multiplier,
		employment_units,
		output_mulitplier_from_workers_with_high_education(state, m, fac.get_secondary_employment(), max_employment),
		max_employment, economy_reason::factory
	);

	float actual_wages = get_total_wage(state, f);

	float actual_profit = data.output * base_data.output_price - data.direct_inputs_cost - data.efficiency_inputs_cost - actual_wages;

	fac.set_unprofitable(actual_profit <= 0.0f);

	fac.set_output(data.output);
	fac.set_output_per_worker(data.output_per_employment_unit / float(fac_type.get_base_workforce()));
	fac.set_input_cost_per_worker(data.input_cost_per_employment_unit / float(fac_type.get_base_workforce()));
	fac.set_input_cost(data.direct_inputs_cost + data.efficiency_inputs_cost);
}


void update_artisan_production(sys::state& state) {
	state.world.execute_serial_over_market([&](auto ids) {
		auto const csize = state.world.commodity_size();
		for(uint32_t i = 1; i < csize; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto production = state.world.market_get_artisan_actual_production(ids, cid);
			register_domestic_supply(state, ids, cid, production, economy_reason::artisan);
		}
	});
}

void update_single_factory_production(
	sys::state& state,
	dcon::factory_id f,
	dcon::market_id m,
	dcon::nation_id n
) {
	auto production = state.world.factory_get_output(f);
	if(production > 0) {
		auto fac = fatten(state.world, f);
		auto fac_type = fac.get_building_type();
		register_domestic_supply(state, m, fac_type.get_output(), production, economy_reason::factory);
	}
}


// currently rgos consume only labor
void update_rgo_consumption(sys::state& state,
	dcon::province_id p,
	dcon::market_id m
) {
	auto n = state.world.province_get_nation_from_province_ownership(p);
	state.world.province_set_rgo_full_labor_cost(p, 0.f);
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto max_production = rgo_full_production_quantity(state, n, p, c);
		if(max_production < 0.001f) {
			return;
		}
		auto target = state.world.province_get_rgo_target_employment_per_good(p, c);
		state.world.market_get_labor_demand(m, labor::no_education) += target;
		state.world.province_get_rgo_full_labor_cost(p) +=
			target
			* state.world.market_get_labor_demand_satisfaction(m, labor::no_education)
			* state.world.market_get_labor_price(m, labor::no_education);
	});
}

void update_province_rgo_production(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n
) {
	state.world.province_set_rgo_full_output_cost(p, 0.f);
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto amount = state.world.province_get_rgo_actual_production_per_good(p, c);
		register_domestic_supply(state, m, c, amount, economy_reason::rgo);
		float profit = amount * state.world.market_get_price(m, c) * state.world.market_get_supply_sold_ratio(m, c);
		if(state.world.commodity_get_money_rgo(c)) {
			profit = amount * state.world.market_get_price(m, c);
		}
		assert(profit >= 0);
		state.world.province_set_rgo_profit_per_good(p, c, profit);
		state.world.province_get_rgo_full_output_cost(p) += profit;

		if(state.world.commodity_get_money_rgo(c)) {
			assert(
				std::isfinite(
					amount
					* state.defines.gold_to_cash_rate
					* state.world.commodity_get_cost(c)
				)
				&& amount * state.defines.gold_to_cash_rate >= 0.0f
			);
			state.world.nation_get_stockpiles(n, money) +=
				amount
				* state.defines.gold_to_cash_rate
				* state.world.commodity_get_cost(c);
		}
	});
}

template<size_t N>
struct employment_data {
	std::array<ve::fp_vector, N> target;
	std::array<ve::fp_vector, N> actual;
	std::array<ve::fp_vector, N> power;
	std::array<ve::fp_vector, N> wage;
};

template<size_t N>
struct employment_vector {
	std::array<ve::fp_vector, N> primary;
	ve::fp_vector secondary;
};

ve::fp_vector gradient_employment_i(
	ve::fp_vector expected_profit_per_perfect_worker,
	ve::fp_vector power,
	ve::fp_vector wage,
	ve::fp_vector secondary_employment,
	ve::fp_vector secondary_power
) {
	return (expected_profit_per_perfect_worker * power - wage) * (1.f + secondary_employment * secondary_power);
}
ve::fp_vector gradient_employment_i(
	ve::fp_vector expected_profit_per_perfect_worker,
	ve::fp_vector power,
	ve::fp_vector wage
) {
	return expected_profit_per_perfect_worker * power - wage;
}

template<size_t N>
ve::fp_vector gradient_employment_secondary(
	ve::fp_vector expected_profit_per_perfect_worker,
	ve::fp_vector secondary_power,
	ve::fp_vector secondary_wage,
	employment_data<N>& primary_employment
) {
	auto gradient = -secondary_wage;
	auto mult = secondary_power;
	for(size_t i = 0; i < N; i++) {
		gradient = gradient
			+ secondary_power
			* primary_employment.actual[i]
			* primary_employment.power[i]
			* expected_profit_per_perfect_worker;
	}
	return gradient;
}

template<size_t N>
employment_vector<N> get_profit_gradient(
	ve::fp_vector expected_profit_per_perfect_primary_worker,
	ve::fp_vector secondary_target,
	ve::fp_vector secondary_actual,
	ve::fp_vector secondary_power,
	ve::fp_vector secondary_wage,
	employment_data<N>& primary_employment
) {
	employment_vector<N> result{ };
	for(size_t i = 0; i < N; i++) {
		result.primary[i] = gradient_employment_i(
			expected_profit_per_perfect_primary_worker,
			primary_employment.power[i],
			primary_employment.wage[i],
			secondary_actual,
			secondary_power
		);
	}
	result.secondary = gradient_employment_secondary(
		expected_profit_per_perfect_primary_worker,
		secondary_power,
		secondary_wage,
		primary_employment
	);
	return result;
}

void update_employment(sys::state& state) {
	// note: markets are independent, so nations are independent:
	// so we can execute in parallel over nations but not over provinces

	state.world.execute_parallel_over_nation([&](auto nations) {
		ve::apply([&](dcon::nation_id n) {
			// STEP 3 update local rgo employment:
			state.world.nation_for_each_province_ownership_as_nation(n, [&](dcon::province_ownership_id poid) {
				auto p = state.world.province_ownership_get_province(poid);
				bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));
				auto state_instance = state.world.province_get_state_membership(p);
				auto m = state_instance.get_market_from_local_market();
				auto n = state_instance.get_nation_from_state_ownership();
				auto min_wage_factor = pop_min_wage_factor(state, n);
				auto pop_farmer_min_wage = farmer_min_wage(state, m, min_wage_factor);
				auto pop_laborer_min_wage = laborer_min_wage(state, m, min_wage_factor);
				auto rgo_pops = rgo_relevant_population(state, p, n);
				float desired_profit_per_worker = rgo_desired_worker_norm_profit(state, p, m, n, rgo_pops.total);
				float total_employment = 0.f;

				state.world.for_each_commodity([&](dcon::commodity_id c) {
					float max_production = rgo_full_production_quantity(state, n, p, c);
					if(max_production < 0.001f) {
						return;
					}
					// maximal amount of workers which rgo could potentially employ
					float pops_max = rgo_max_employment(state, n, p, c);
					float current_employment =
						state.world.province_get_rgo_target_employment_per_good(p, c)
						* state.world.market_get_labor_demand_satisfaction(m, labor::no_education);
					float efficiency_per_hire = rgo_efficiency(state, n, p, c) / state.defines.alice_rgo_per_size_employment;
					float current_price = price(state, m, c);
					float supply = state.world.market_get_supply(m, c) + price_rigging;
					float demand = state.world.market_get_demand(m, c) + price_rigging;
					if(state.world.commodity_get_money_rgo(c)) {
						supply = 100000.f;
						demand = 100000.f;
					}
					float price_speed_change = price_speed_mod * (demand / supply - supply / demand);
					float predicted_price = current_price + std::min(std::max(price_speed_change, -0.025f), 0.025f) * current_price * 0.5f;
					float predicted_income = predicted_price * efficiency_per_hire;
					float predicted_spending = desired_profit_per_worker;

					float change = 1000.f * (predicted_income - predicted_spending) / std::max(predicted_income, 0.00001f);
					assert(std::isfinite(current_employment + change));
					float new_employment = std::clamp(current_employment + change, 0.f, pops_max);
					state.world.province_set_rgo_target_employment_per_good(p, c, new_employment);
					total_employment += new_employment;

					// rgos produce all the way down
					float employment_ratio = current_employment / pops_max;
					assert(max_production * employment_ratio >= 0);
					state.world.province_set_rgo_actual_production_per_good(p, c, max_production * employment_ratio);
				});

				if(total_employment > rgo_pops.total) {
					state.world.for_each_commodity([&](dcon::commodity_id c) {
						auto current = state.world.province_get_rgo_target_employment_per_good(p, c);
						state.world.province_set_rgo_target_employment_per_good(p, c, current * rgo_pops.total / total_employment);
					});
				}
			});
		}, nations);
	});

	state.world.execute_serial_over_factory([&](auto facids) {
		auto pid = state.world.factory_get_province_from_factory_location(facids);
		auto sid = state.world.province_get_state_membership(pid);
		auto mid = state.world.state_instance_get_market_from_local_market(sid);
		auto factory_type = state.world.factory_get_building_type(facids);
		auto output = state.world.factory_type_get_output(factory_type);

		auto price_output = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_price(market, cid) + price_rigging;
		}, mid, output);

		auto actually_sold = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_supply_sold_ratio(market, cid);
		}, mid, output);

		auto supply = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_supply(market, cid) + price_rigging;
		}, mid, output);

		auto demand = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_demand(market, cid) + price_rigging;
		}, mid, output);

		auto output_per_worker = state.world.factory_get_output_per_worker(facids);
		auto unqualified = state.world.factory_get_unqualified_employment(facids);
		auto primary = state.world.factory_get_primary_employment(facids);
		auto secondary = state.world.factory_get_secondary_employment(facids);
		auto profit_push = output_per_worker * price_output * actually_sold;
		auto spendings_push = state.world.factory_get_input_cost_per_worker(facids);
		auto wage_no_education = state.world.market_get_labor_price(mid, labor::no_education);
		auto wage_basic_education = state.world.market_get_labor_price(mid, labor::basic_education);
		auto wage_high_education = state.world.market_get_labor_price(mid, labor::high_education);

		employment_data<2> primary_employment{
			{ unqualified, primary },
			{
				unqualified * state.world.market_get_labor_demand_satisfaction(mid, labor::no_education),
				primary * state.world.market_get_labor_demand_satisfaction(mid, labor::basic_education)
			},
			{ unqualified_throughput_multiplier, 1.f },
			{ wage_no_education * 1.01f, wage_basic_education * 1.01f }
		};

		// this time we are content with very simple first order approximation
		auto price_speed_gradient_time =
			price_speed_mod * (demand / supply - supply / demand);
		auto price_prediction = actually_sold * (price_output + 0.5f * price_speed_gradient_time);
		auto size = state.world.factory_get_size(facids);

		auto gradient = get_profit_gradient(
			output_per_worker * price_prediction - state.world.factory_get_input_cost_per_worker(facids),
			secondary,
			secondary * state.world.market_get_labor_demand_satisfaction(mid, labor::high_education),
			1.f / size * economy::secondary_employment_output_bonus,
			wage_high_education * 1.01f,
			primary_employment
		);

		auto unqualified_next = unqualified
			+ ve::min(0.01f * state.world.factory_get_size(facids),
				ve::max(-0.05f * state.world.factory_get_size(facids),
					10'000.f * gradient.primary[0]
				)
			);
		auto primary_next = primary
			+ ve::min(0.01f * state.world.factory_get_size(facids),
				ve::max(-0.05f * state.world.factory_get_size(facids),
					10'000.f * gradient.primary[1]
				)
			);
		auto secondary_next = secondary
			+ ve::min(0.01f * state.world.factory_get_size(facids),
				ve::max(-0.05f * state.world.factory_get_size(facids),
					10'000.f * gradient.secondary
				)
			);

		// do not hire too expensive workers:
		// ideally decided by factory budget but it is what it is: we assume that factory budget is 100 pounds per size unit

		unqualified_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(100.f * state.world.factory_get_size(facids) / wage_no_education, unqualified_next)));
		primary_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(100.f * state.world.factory_get_size(facids) / wage_basic_education, primary_next)));
		secondary_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(100.f * state.world.factory_get_size(facids) / wage_high_education, secondary_next)));

		auto total = unqualified_next + primary_next + secondary_next;
		auto scaler = ve::select(total > state.world.factory_get_size(facids), state.world.factory_get_size(facids) / total, 1.f);

#ifndef NDEBUG
		ve::apply([&](auto factory, auto value) { if(state.world.factory_is_valid(factory)) assert(std::isfinite(value) && (value >= 0.f)); },
			facids, unqualified_next
		);
		ve::apply([&](auto factory, auto value) { if(state.world.factory_is_valid(factory)) assert(std::isfinite(value) && (value >= 0.f)); },
			facids, primary_next
		);
		ve::apply([&](auto factory, auto value) { if(state.world.factory_is_valid(factory)) assert(std::isfinite(value) && (value >= 0.f)); },
			facids, secondary_next
		);
#endif // !NDEBUG

		state.world.factory_set_unqualified_employment(facids, unqualified_next * scaler);
		state.world.factory_set_primary_employment(facids, primary_next * scaler);
		state.world.factory_set_secondary_employment(facids, secondary_next * scaler);
	});

	auto const csize = state.world.commodity_size();

	// artisans do not have max production size, so they don't need any kind of normalisation actually
	// they also don't have secondary workers

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			auto local_states = state.world.market_get_zone_from_local_market(ids);
			auto nations = state.world.state_instance_get_nation_from_state_ownership(local_states);
			auto sids = state.world.market_get_zone_from_local_market(ids);
			auto min_wage = ve_artisan_min_wage(state, ids) / state.defines.alice_needs_scaling_factor;
			auto actual_wage = state.world.market_get_labor_price(ids, labor::guild_education);				
			auto mask = ve_valid_artisan_good(state, nations, cid);
			auto price_today = ve_price(state, ids, cid);
			auto supply = state.world.market_get_supply(ids, cid) + price_rigging;
			auto demand = state.world.market_get_demand(ids, cid) + price_rigging;
			auto price_speed_change = price_speed_mod * (demand / supply - supply / demand);
			auto predicted_price = price_today + price_speed_change * 0.5f;
			auto base_profit = base_artisan_profit(state, ids, nations, cid, predicted_price);
			auto base_profit_per_worker = base_profit / artisans_per_employment_unit;
			auto current_employment_target = state.world.market_get_artisan_score(ids, cid);
			auto current_employment_actual = current_employment_target
				* state.world.market_get_labor_demand_satisfaction(ids, labor::guild_education);
			auto profit_per_worker = ve::select(
				mask,
				base_profit_per_worker,
				std::numeric_limits<float>::lowest()
			);
			auto gradient = gradient_employment_i(
				profit_per_worker,
				1.f,
				min_wage + actual_wage
			);
			auto new_employment = ve::select(mask, ve::max(current_employment_target + 10'000.f * gradient, 0.0f), 0.f);
			state.world.market_set_artisan_score(ids, cid, new_employment);
			ve::apply(
				[](float x) {
						assert(std::isfinite(x));
				}, new_employment
			);
		});
	}
}

/*
*
- Each factory has an input, output, and throughput multipliers.
- These are computed from the employees present. Input and output are 1 + employee effects, throughput starts at 0
- The input multiplier is also multiplied by (1 + sum-of-any-triggered-modifiers-for-the-factory) x
0v(national-mobilization-impact)
- Note: overseas is repurposed to administration of colonies
- Owner fraction is calculated from the fraction of owners in the state to total state population in the state (with some cap --
5%?)
- For each pop type employed, we calculate the ratio of number-of-pop-employed-of-a-type / (base-workforce x level) to the optimal
fraction defined for the production type (capping it at 1). That ratio x the-employee-effect-amount is then added into the
input/output/throughput modifier for the factory.
- Then, for input/output/throughput we sum up national and provincial modifiers to general factory input/output/throughput are
added, plus technology modifiers to its specific output commodity, add one to the sum, and then multiply the
input/output/throughput modifier from the workforce by it.

- The target input consumption scale is: input-multiplier x throughput-multiplier x factory level
- The actual consumption scale is limited by the input commodities sitting in the stockpile (i.e. input-consumption-scale x
input-quantity must be less than the amount in the stockpile)
- A similar process is done for efficiency inputs, except the consumption of efficiency inputs is
(national-factory-maintenance-modifier + 1) x input-multiplier x throughput-multiplier x factory level
- Finally, we get the efficiency-adjusted consumption scale by multiplying the base consumption scale by (0.75 + 0.25 x the
efficiency consumption scale)

*/

void update_production_consumption(sys::state& state) {
	state.world.execute_parallel_over_market([&](auto markets) {
		auto states = state.world.market_get_zone_from_local_market(markets);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
		auto mobilization_impact =
			ve::select(
				state.world.nation_get_is_mobilized(nations),
				military::ve_mobilization_impact(state, nations), 1.f
			);

		// STEP 1: artisans consumption update:
		update_artisan_consumption(state, markets, nations, states, mobilization_impact);

		ve::apply(
			[&](
				dcon::state_instance_id s,
				dcon::market_id m,
				dcon::nation_id n,
				float mob_impact
				) {
					auto capital = state.world.state_instance_get_capital(s);
					province::for_each_province_in_state_instance(state, s, [&](auto p) {
						// STEP 2:
						// update local factories consumption
						// update local rgo consumption

						for(auto f : state.world.province_get_factory_location(p)) {
							update_single_factory_consumption(
								state,
								f.get_factory(),
								p,
								s,
								m,
								n,
								mob_impact,
								state.world.province_get_nation_from_province_control(p) != n // is occupied
							);
						}
						update_rgo_consumption(
							state,
							p, m
						);
					});
				}, states, markets, nations, mobilization_impact
		);
	});
}

}
