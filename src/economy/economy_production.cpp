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
	if constexpr(std::is_same_v<T, dcon::nation_id>) {
		return production_throughput_multiplier * std::max(
			0.01f,
			1.0f
			+ state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::artisan_throughput)
		);
	} else {
		return production_throughput_multiplier * ve::max(
			0.01f,
			1.0f
			+ state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::artisan_throughput)
		);
	}
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

float max_rgo_efficiency(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::commodity_id c) {
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
		* state.defines.alice_rgo_boost * 10.f // compensation for efficiency being not free now
		* std::max(0.5f, (1.0f + state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_output) +
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_output) +
			state.world.nation_get_rgo_goods_output(n, c)));

	assert(result >= 0.0f && std::isfinite(result));
	return result;
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

float rgo_expected_worker_norm_profit(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n,
	dcon::commodity_id c
) {
	auto efficiency = state.world.province_get_rgo_efficiency(p, c);
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
	dcon::province_id labor_market,
	float target_workers_no_education, float target_workers_basic_education, float target_workers_high_education, float employment_unit_size
) {
	assert(target_workers_no_education >= 0.f);
	assert(target_workers_basic_education >= 0.f);
	float workers_no_education = target_workers_no_education
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::no_education);
	float workers_basic_education = target_workers_basic_education
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::basic_education);
	assert(workers_no_education >= 0.f);
	assert(workers_basic_education >= 0.f);

	auto effective_employment = workers_no_education * unqualified_throughput_multiplier
		+ workers_basic_education;
	return effective_employment / employment_unit_size;
}

template<typename M>
ve::fp_vector employment_units_guild(
	sys::state& state,
	M labor_market,
	ve::fp_vector target_workers_guild
) {
	ve::fp_vector guild_actual_size =
		target_workers_guild
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::guild_education);
	return guild_actual_size / artisans_per_employment_unit;
}
float employment_units_guild(
	sys::state& state,
	dcon::province_id labor_market,
	float target_workers_guild
) {
	float guild_actual_size =
		target_workers_guild
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::guild_education);
	return guild_actual_size / artisans_per_employment_unit;
}


float output_mulitplier_from_workers_with_high_education(
	sys::state& state,
	dcon::province_id labor_market,
	float target_workers_high_education,
	float max_employment
) {
	assert(target_workers_high_education >= 0.f);
	float workers_high_education = target_workers_high_education
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::high_education);
	assert(workers_high_education >= 0.f);
	if(max_employment < 1.f) {
		return 1.f;
	}
	return (1.f + economy::secondary_employment_output_bonus * workers_high_education / max_employment);
}

// returns employment units
float consume_labor(
	sys::state& state,
	dcon::province_id labor_market,
	float target_workers_no_education,
	float target_workers_basic_education,
	float target_workers_high_education,
	float employment_unit_size
) {
	assert(std::isfinite(target_workers_no_education));
	assert(std::isfinite(target_workers_basic_education));
	assert(std::isfinite(target_workers_high_education));

	// register demand for labor
	state.world.province_get_labor_demand(labor_market, labor::no_education) += target_workers_no_education;
	state.world.province_get_labor_demand(labor_market, labor::basic_education) += target_workers_basic_education;
	state.world.province_get_labor_demand(labor_market, labor::high_education) += target_workers_high_education;

	assert(std::isfinite(state.world.province_get_labor_demand(labor_market, labor::no_education)));
	assert(std::isfinite(state.world.province_get_labor_demand(labor_market, labor::basic_education)));
	assert(std::isfinite(state.world.province_get_labor_demand(labor_market, labor::high_education)));

	return employment_units(
		state, labor_market,
		target_workers_no_education, target_workers_basic_education, target_workers_high_education, employment_unit_size
	);
}

template<typename M>
ve::fp_vector consume_labor_guild(
	sys::state& state,
	M market,
	ve::fp_vector target_guild_size
) {

#ifndef NDEBUG
	ve::apply([&](float value) {assert(std::isfinite(value)); }, target_guild_size);
#endif // !NDEBUG

	state.world.province_set_labor_demand(
		market,
		labor::guild_education,
		state.world.province_get_labor_demand(market, labor::guild_education) + target_guild_size
	);

#ifndef NDEBUG
	ve::apply([&](float value) {assert(std::isfinite(value)); }, state.world.province_get_labor_demand(market, labor::guild_education));
#endif // !NDEBUG

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

struct preconsumption_data_guild {
	inputs_data direct_inputs_data{ };

	float output_price = 0.f;
	float output_amount_per_production_unit = 0.f;
	float direct_inputs_cost_per_production_unit = 0.f;
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

preconsumption_data_guild prepare_data_for_consumption(
	sys::state& state,
	dcon::market_id market,
	economy::commodity_set const& inputs,
	dcon::commodity_id output,
	float output_amount,
	float input_multiplier, float output_multiplier
) {
	auto direct_inputs_data = get_inputs_data(state, market, inputs);
	auto output_price = price(state, market, output);
	auto output_per_production_unit =
		output_amount
		* output_multiplier;
	auto output_cost_per_production_unit =
		output_per_production_unit
		* output_price;
	preconsumption_data_guild result = {
		.direct_inputs_data = direct_inputs_data,
		.output_price = output_price,
		.output_amount_per_production_unit = output_per_production_unit,
		.direct_inputs_cost_per_production_unit = direct_inputs_data.total_cost * input_multiplier,
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

	float direct_inputs_scale;
	float efficiency_inputs_scale;

	float input_cost_per_employment_unit;
	float output_per_employment_unit;
};
struct ve_consumption_data_guild {
	ve::fp_vector direct_inputs_cost;
	ve::fp_vector output;

	ve::fp_vector direct_inputs_scale;

	ve::fp_vector input_cost_per_employment_unit;
	ve::fp_vector output_per_employment_unit;
};
struct consumption_data_guild {
	float direct_inputs_cost;
	float output;

	float direct_inputs_scale;

	float input_cost_per_employment_unit;
	float output_per_employment_unit;
};

consumption_data imitate_consume(
	sys::state& state,
	economy::commodity_set const& inputs,
	economy::small_commodity_set const& efficiency_inputs,
	preconsumption_data& additional_data,

	float input_multiplier, float efficiency_inputs_multiplier, float throughput_multiplier, float output_multiplier,
	float employment_units, float output_mulitplier_from_workers_with_high_education,
	float max_employment
) {
	float production_units = employment_units * throughput_multiplier;
	float input_scale =
		input_multiplier
		* production_units
		* inputs_demand_reduction(
			additional_data.direct_inputs_cost_per_production_unit,
			additional_data.output_amount_per_production_unit * additional_data.output_price,
			additional_data.direct_inputs_data.min_available
		);
	float e_input_scale =
		input_multiplier
		* efficiency_inputs_multiplier
		* production_units
		* inputs_demand_reduction(
			additional_data.efficiency_inputs_cost_per_production_unit,
			additional_data.output_amount_per_production_unit * additional_data.output_price,
			additional_data.efficiency_inputs_data.min_available
		);

	consumption_data result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,
		.efficiency_inputs_cost = additional_data.efficiency_inputs_cost_per_production_unit * production_units * additional_data.efficiency_inputs_data.min_available,
		.output = additional_data.output_amount_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,

		.direct_inputs_scale = input_scale,
		.efficiency_inputs_scale = e_input_scale,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit / throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit / throughput_multiplier
	};

	return result;
}

consumption_data_guild imitate_consume(
	sys::state& state,
	economy::commodity_set const& inputs,
	preconsumption_data_guild& additional_data,
	float input_multiplier, float throughput_multiplier, float output_multiplier,
	float employment_units
) {
	float production_units = employment_units * throughput_multiplier;
	float input_scale =
		input_multiplier
		* production_units
		* inputs_demand_reduction(
			additional_data.direct_inputs_cost_per_production_unit,
			additional_data.output_amount_per_production_unit * additional_data.output_price,
			additional_data.direct_inputs_data.min_available
		);

	consumption_data_guild result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,
		.output = additional_data.output_amount_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,

		.direct_inputs_scale = input_scale,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit / throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit / throughput_multiplier
	};

	return result;
}

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

		.direct_inputs_scale = input_scale,
		.efficiency_inputs_scale = e_input_scale,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit * throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit * throughput_multiplier
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

	ve::apply([&](auto m, auto scale) {
		register_inputs_demand(state, m, inputs, scale, reason);
	}, market, input_scale);

	ve_consumption_data_guild result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,
		.output = additional_data.output_amount_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,

		.direct_inputs_scale = input_scale,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit / throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit / throughput_multiplier
	};

	return result;
}

template<typename P>
void update_artisan_consumption(
	sys::state& state,
	P provinces,
	ve::fp_vector mobilization_impact
) {
	auto const csize = state.world.commodity_size();
	ve::fp_vector total_profit = 0.0f;

	auto nations = state.world.province_get_nation_from_province_ownership(provinces);
	auto states = state.world.province_get_state_membership(provinces);
	auto markets = state.world.state_instance_get_market_from_local_market(states);

	ve::fp_vector input_multiplier = artisan_input_multiplier(state, nations);
	ve::fp_vector throughput_multiplier = artisan_throughput_multiplier(state, nations);
	ve::fp_vector output_multiplier = artisan_output_multiplier(state, nations);

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto output_amount = state.world.commodity_get_artisan_output_amount(cid);
		if(output_amount <= 0.f) {
			continue;
		}

		auto const& inputs = state.world.commodity_get_artisan_inputs(cid);
		state.world.province_set_artisan_actual_production(provinces, cid, 0.0f);
		auto valid_mask = ve_valid_artisan_good(state, nations, cid);
		ve::fp_vector target_workers = state.world.province_get_artisan_score(provinces, cid);
		auto actual_workers =
			target_workers
			* state.world.province_get_labor_demand_satisfaction(provinces, labor::guild_education)
			* mobilization_impact;
		auto employment_units = ve::select(valid_mask, consume_labor_guild(state, provinces, actual_workers), 0.f);

		ve_preconsumption_data_guild prepared_data = prepare_data_for_consumption(
			state, markets, inputs, cid, output_amount, input_multiplier, output_multiplier
		);
		ve_consumption_data_guild consumption_data = consume(
			state, markets,
			inputs, prepared_data,
			input_multiplier, throughput_multiplier, output_multiplier,
			employment_units, economy_reason::artisan
		);
		state.world.province_set_artisan_actual_production(
			provinces,
			cid,
			consumption_data.output
		);
		total_profit = total_profit + ve::max(0.f,
			consumption_data.output * prepared_data.output_price - consumption_data.direct_inputs_cost
		);
	}
	state.world.province_set_artisan_profit(provinces, total_profit);
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

float factory_throughput_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s, float size) {
	auto output = state.world.factory_type_get_output(fac_type);
	auto national_t = state.world.nation_get_factory_goods_throughput(n, output);
	auto provincial_fac_t = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput);
	auto nationnal_fac_t = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_throughput);

	auto result = 1.f
		* std::max(0.f, 1.f + national_t)
		* std::max(0.f, 1.f + provincial_fac_t)
		* std::max(0.f, 1.f + nationnal_fac_t)
		* (1.f + size / state.world.factory_type_get_base_workforce(fac_type) / 100.f);

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

float get_total_wage(sys::state& state, dcon::factory_id f) {
	auto labor_market = state.world.factory_get_province_from_factory_location(f);
	return state.world.province_get_labor_price(labor_market, labor::no_education)
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::no_education)
		* state.world.factory_get_unqualified_employment(f)
		+
		state.world.province_get_labor_price(labor_market, labor::basic_education)
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::basic_education)
		* state.world.factory_get_primary_employment(f)
		+
		state.world.province_get_labor_price(labor_market, labor::high_education)
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::high_education)
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
	
	auto current_size = state.world.factory_get_size(f);
	auto base_size = state.world.factory_type_get_base_workforce(ftid);
	auto construction_units = current_size / base_size;
	auto construction_units_per_day = construction_units / state.world.factory_type_get_construction_time(ftid);
	auto maintenance_scale = construction_units_per_day * construction_units_to_maintenance_units;
	auto& costs = state.world.factory_type_get_construction_costs(ftid);
	auto costs_data = get_inputs_data(state, market, costs);
	auto maintenance_cost = costs_data.total_cost * costs_data.min_available * maintenance_scale;

	auto profit = last_output - wages - last_inputs - maintenance_cost;
	auto expansion_cost = 0.f;
	auto total_employment = state.world.factory_get_unqualified_employment(f)
		+ state.world.factory_get_primary_employment(f)
		+ state.world.factory_get_secondary_employment(f);
	if(profit > 0 && total_employment >= current_size * expansion_trigger) {
		auto expansion_scale = 1.f / state.world.factory_type_get_construction_time(ftid);
		expansion_cost = std::min(profit * 0.1f, expansion_scale * costs_data.total_cost);
	}

	return {
		.inputs = last_inputs,
		.wages = wages,
		.maintenance = maintenance_cost,
		.expansion = expansion_cost,
		.output = last_output,
		.profit = last_output - wages - last_inputs
	};
}

struct factory_update_data {
	preconsumption_data base;
	consumption_data consumption;
};

struct guild_update_data {
	preconsumption_data_guild base;
	consumption_data_guild consumption;
};

guild_update_data imitate_artisan_consumption(
	sys::state& state,
	dcon::province_id p,
	dcon::commodity_id cid,
	float mobilization_impact
) {
	auto const csize = state.world.commodity_size();
	auto total_profit = 0.0f;

	auto nations = state.world.province_get_nation_from_province_ownership(p);
	auto states = state.world.province_get_state_membership(p);
	auto markets = state.world.state_instance_get_market_from_local_market(states);

	float input_multiplier = artisan_input_multiplier(state, nations);
	float throughput_multiplier = artisan_throughput_multiplier(state, nations);
	float output_multiplier = artisan_output_multiplier(state, nations);

	auto const& inputs = state.world.commodity_get_artisan_inputs(cid);
	auto output_amount = state.world.commodity_get_artisan_output_amount(cid);
	float target_workers = state.world.province_get_artisan_score(p, cid);

	float actual_workers =
		target_workers
		* state.world.province_get_labor_demand_satisfaction(p, labor::guild_education)
		* mobilization_impact;

	float employment_units = employment_units_guild(state, p, actual_workers);

	preconsumption_data_guild prepared_data = prepare_data_for_consumption(
		state, markets, inputs, cid, output_amount, input_multiplier, output_multiplier
	);
	consumption_data_guild consumption_data = imitate_consume(
		state,
		inputs,
		prepared_data,
		input_multiplier,
		throughput_multiplier,
		output_multiplier,
		employment_units
	);

	return {
		.base = prepared_data,
		.consumption = consumption_data
	};
}

factory_update_data imitate_single_factory_consumption(
	sys::state& state,
	dcon::factory_id f
) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();
	auto p = fac.get_province_from_factory_location();
	auto s = p.get_state_membership();
	auto m = s.get_market_from_local_market();
	auto n = p.get_nation_from_province_ownership();

	//modifiers
	float input_multiplier = factory_input_multiplier(state, fac, n, p, s);
	auto const mfactor = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	float throughput_multiplier = factory_throughput_multiplier(state, fac_type, n, p, s, fac.get_size());
	float output_multiplier = factory_output_multiplier_no_secondary_workers(state, fac, n, p);

	auto& direct_inputs = fac_type.get_inputs();
	auto& efficiency_inputs = fac_type.get_efficiency_inputs();
	auto max_employment = fac.get_size();

	auto base_data = prepare_data_for_consumption(
		state, m,
		direct_inputs, efficiency_inputs,
		fac_type.get_output(), fac_type.get_output_amount(),
		input_multiplier, mfactor, output_multiplier
	);

	auto employment = employment_units(
		state, p,
		fac.get_unqualified_employment(), fac.get_primary_employment(), fac.get_secondary_employment(),
		float(fac_type.get_base_workforce())
	);

	auto consumption_data = imitate_consume(
		state, direct_inputs, efficiency_inputs, base_data,
		input_multiplier, mfactor, throughput_multiplier, output_multiplier,
		employment,
		output_mulitplier_from_workers_with_high_education(
			state, fac.get_province_from_factory_location(), fac.get_secondary_employment(), max_employment
		), max_employment
	);

	return {
		.base = base_data, .consumption = consumption_data
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
	float throughput_multiplier = factory_throughput_multiplier(state, fac_type, n, p, s, fac.get_size());
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
		state, fac.get_province_from_factory_location(),
		fac.get_unqualified_employment(), fac.get_primary_employment(), fac.get_secondary_employment(),
		float(fac_type.get_base_workforce())
	) * std::max(0.f, mobilization_impact);
	auto total_employment = fac.get_unqualified_employment() + fac.get_primary_employment() + fac.get_secondary_employment();

	auto data = consume(
		state, m,
		direct_inputs, efficiency_inputs,
		base_data,
		input_multiplier, mfactor, throughput_multiplier, output_multiplier,
		employment_units,
		output_mulitplier_from_workers_with_high_education(
			state, fac.get_province_from_factory_location(), fac.get_secondary_employment(), max_employment
		), max_employment, economy_reason::factory
	);

	auto current_size = state.world.factory_get_size(f);
	auto ftid = state.world.factory_get_building_type(f);
	auto base_size = state.world.factory_type_get_base_workforce(ftid);
	auto construction_units = current_size / base_size;
	auto construction_units_per_day = construction_units / state.world.factory_type_get_construction_time(ftid);
	auto maintenance_scale = construction_units_per_day * construction_units_to_maintenance_units;
	auto& costs = state.world.factory_type_get_construction_costs(ftid);
	auto costs_data = get_inputs_data(state, m, costs);
	register_inputs_demand(state, m, costs, maintenance_scale, economy_reason::construction);
	auto maintenance_cost = costs_data.total_cost * costs_data.min_available * maintenance_scale;
	float actual_wages = get_total_wage(state, f);
	float actual_profit = data.output * base_data.output_price - data.direct_inputs_cost - data.efficiency_inputs_cost - actual_wages - maintenance_cost;

	// if we are at max amount of workers and we are profitable, spend 10% of the profit on actual expansion
	auto expansion_scale = 1.f / state.world.factory_type_get_construction_time(ftid);
	if(actual_profit > 0 && total_employment >= current_size * expansion_trigger) {
		auto base_expansion_cost = expansion_scale * costs_data.total_cost;
		expansion_scale = expansion_scale * std::min(1.f, actual_profit * 0.1f / base_expansion_cost);
		register_inputs_demand(state, m, costs, expansion_scale, economy_reason::construction);
		state.world.factory_set_size(fac, current_size + base_size * expansion_scale * costs_data.min_available);
	} else if(actual_profit < 0) {
		state.world.factory_set_size(fac, std::max(500.f, current_size - base_size * expansion_scale));
	}

	if(state.world.commodity_get_uses_potentials(fac_type.get_output())) {
		auto new_size = state.world.factory_get_size(fac);
		state.world.factory_set_size(fac, std::min(new_size, state.world.province_get_factory_max_size(p, fac_type.get_output())));
	}

	fac.set_unprofitable(actual_profit <= 0.0f);

	fac.set_output(data.output);
	fac.set_output_per_worker(data.output_per_employment_unit / float(fac_type.get_base_workforce()));
	fac.set_input_cost_per_worker(data.input_cost_per_employment_unit / float(fac_type.get_base_workforce()));
	fac.set_input_cost(data.direct_inputs_cost + data.efficiency_inputs_cost);
}


void update_artisan_production(sys::state& state) {

	auto const csize = state.world.commodity_size();
	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		if(state.world.commodity_get_artisan_output_amount(cid) == 0.f)
			continue;
		state.world.for_each_province([&](auto ids) {
			auto production = state.world.province_get_artisan_actual_production(ids, cid);
			auto sid = state.world.province_get_state_membership(ids);
			auto mid = state.world.state_instance_get_market_from_local_market(sid);
			register_domestic_supply(state, mid, cid, production, economy_reason::artisan);
		});
	}
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


// currently rgos consume only labor and efficiency goods
void update_rgo_consumption(
	sys::state& state,
	dcon::province_id p,
	float mobilization_impact
) {
	auto n = state.world.province_get_nation_from_province_ownership(p);
	auto sid = state.world.province_get_state_membership(p);
	auto m = state.world.state_instance_get_market_from_local_market(sid);

	state.world.province_set_rgo_profit(p, 0.f);

	// update efficiency and consume efficiency_inputs:
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto size = state.world.province_get_rgo_size(p, c);
		if(size <= 0.f) {
			return;
		}

		auto max_efficiency = max_rgo_efficiency(state, n, p, c);
		auto free_efficiency = max_efficiency * 0.1f;
		auto current_efficiency = state.world.province_get_rgo_efficiency(p, c);		
		auto& e_inputs = state.world.commodity_get_rgo_efficiency_inputs(c);
		auto e_inputs_data = get_inputs_data(state, m, e_inputs);
		// we try to update our efficiency according to current profit derivative
		// if higher efficiency brings profit, we increase it
		// 10% of max efficiency is free
		auto cost_derivative = 0.f;
		if(current_efficiency > free_efficiency) {
			cost_derivative = e_inputs_data.total_cost;
		}
		auto profit_derivative =
			state.world.commodity_get_rgo_amount(c)
			/ state.defines.alice_rgo_per_size_employment
			* state.world.market_get_price(m, c)
			* state.world.market_get_supply_sold_ratio(m, c);
		auto efficiency_growth = 100.f * (profit_derivative - cost_derivative);
		// we assume that we can maintain efficiency even when there is not enough goods on the market:
		// efficiency goods are "preserved" (but still demanded to "update")
		// it's very risky to decrease efficiency depending on available efficiency goods directly
		// because it might lead to instability cycles
		// there is some natural decay of efficiency
		// we decrease efficiency outside of decay only if decreasing efficiency is profitable
		// but to increase efficiency, there should be enough of goods on the local market unless it's a free efficiency
		if(efficiency_growth > 0.f && current_efficiency > free_efficiency) {
			efficiency_growth = efficiency_growth * e_inputs_data.min_available;
		}
		auto new_efficiency = std::min(max_efficiency, std::max(0.f, current_efficiency * 0.99f + efficiency_growth));
		state.world.province_set_rgo_efficiency(p, c, new_efficiency);

		auto workers = state.world.province_get_rgo_target_employment(p, c)
			* state.world.province_get_labor_demand_satisfaction(p, labor::no_education)
			* mobilization_impact;
		auto demand_scale = workers * std::max(new_efficiency - free_efficiency, 0.f);

		register_inputs_demand(state, m, e_inputs, demand_scale, economy_reason::rgo);
		
		auto target = state.world.province_get_rgo_target_employment(p, c);
		state.world.province_get_labor_demand(p, labor::no_education) += std::min(target, 100.f + workers * 1.1f);
		assert(std::isfinite(target));
		assert(std::isfinite(state.world.province_get_labor_demand(p, labor::no_education)));

		auto per_worker = state.world.commodity_get_rgo_amount(c)
			* state.world.province_get_rgo_efficiency(p, c)
			/ state.defines.alice_rgo_per_size_employment;
		auto amount = workers * per_worker;

		float profit = amount * state.world.market_get_price(m, c) * state.world.market_get_supply_sold_ratio(m, c);
		if(state.world.commodity_get_money_rgo(c)) {
			profit = amount * state.world.market_get_price(m, c);
		}
		assert(profit >= 0);
		state.world.province_get_rgo_profit(p) += profit;
		auto wages = workers
			* state.world.province_get_labor_price(p, labor::no_education);
		auto spent_on_efficiency = demand_scale * e_inputs_data.total_cost * e_inputs_data.min_available;

		state.world.province_get_rgo_profit(p) -= wages + spent_on_efficiency;
		state.world.province_set_rgo_output(p, c, amount);
		state.world.province_set_rgo_output_per_worker(p, c, per_worker);
	});
}

void update_province_rgo_production(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n
) {
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		if(state.world.commodity_get_rgo_amount(c) < 0.f) {
			return;
		}
		auto amount = state.world.province_get_rgo_output(p, c);
		register_domestic_supply(state, m, c, amount, economy_reason::rgo);
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
	return expected_profit_per_perfect_worker * power * (1.f + secondary_employment * secondary_power) - wage;
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

	state.world.execute_parallel_over_commodity([&](auto cids) {
		ve::apply([&](dcon::commodity_id c) {
			auto rgo_output = state.world.commodity_get_rgo_amount(c);
			if(rgo_output <= 0.f) {
				return;
			}

			auto& e_inputs = state.world.commodity_get_rgo_efficiency_inputs(c);

			state.world.execute_serial_over_province([&](auto pids) {
				auto state_instance = state.world.province_get_state_membership(pids);
				auto m = state.world.state_instance_get_market_from_local_market(state_instance);
				auto n = state.world.province_get_nation_from_province_ownership(pids);

				auto wage_per_worker = state.world.province_get_labor_price(pids, labor::no_education);

				auto current_size = state.world.province_get_rgo_size(pids, c);
				auto efficiency = state.world.province_get_rgo_efficiency(pids, c);
				auto current_employment_target = state.world.province_get_rgo_target_employment(pids, c);
				auto current_employment = current_employment_target					
					* state.world.province_get_labor_demand_satisfaction(pids, labor::no_education);
				auto output_per_worker = rgo_output * efficiency / state.defines.alice_rgo_per_size_employment;
				auto current_price = ve_price(state, m, c);
				auto supply = state.world.market_get_supply(m, c) + price_rigging;
				auto demand = state.world.market_get_demand(m, c) + price_rigging;

				auto e_inputs_data = get_inputs_data(state, m, e_inputs);

				auto price_speed_change =
					state.world.commodity_get_money_rgo(c)
					? 0.f
					: price_speed_mod * (demand / supply - supply / demand);
				auto predicted_price = current_price + ve::min(ve::max(price_speed_change, -0.025f), 0.025f) * current_price * 0.5f;

				auto gradient = gradient_employment_i(
					output_per_worker * predicted_price,
					1.f,
					(
						state.world.province_get_labor_price(pids, labor::no_education)
						+ e_inputs_data.total_cost * efficiency * 0.9f // good enough approximation because i don't want to recalculate free efficiency again
					) * (1.f + aristocrats_greed)
				);

				auto new_employment = ve::max((current_employment_target + 100.f * gradient), 0.0f);

				// we don't want wages to rise way too high relatively to profits
				// as we do not have actual budgets, we  consider that our workers budget is as follows
				new_employment = ve::min(rgo_profit_to_wage_bound * output_per_worker * predicted_price * current_size / wage_per_worker, new_employment);
				new_employment = ve::min(new_employment, current_size);
				state.world.province_set_rgo_target_employment(pids, c, new_employment);
				state.world.province_set_rgo_output(pids, c, output_per_worker * current_employment);
			});
		}, cids);
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
		auto profit_push = output_per_worker * price_output;
		auto spendings_push = state.world.factory_get_input_cost_per_worker(facids);
		auto wage_no_education = state.world.province_get_labor_price(pid, labor::no_education);
		auto wage_basic_education = state.world.province_get_labor_price(pid, labor::basic_education);
		auto wage_high_education = state.world.province_get_labor_price(pid, labor::high_education);

		employment_data<2> primary_employment{
			{ unqualified, primary },
			{
				unqualified * state.world.province_get_labor_demand_satisfaction(pid, labor::no_education),
				primary * state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education)
			},
			{ unqualified_throughput_multiplier, 1.f },
			{ wage_no_education * 1.01f, wage_basic_education * 1.01f }
		};

		// this time we are content with very simple first order approximation
		auto price_speed_gradient_time =
			ve::select(
				state.world.commodity_get_money_rgo(output),
				0.f, 
				price_speed_mod * (demand / supply - supply / demand)
			);

		auto price_prediction = (price_output + 0.5f * price_speed_gradient_time);
		auto size = state.world.factory_get_size(facids);

		auto profit_per_worker = output_per_worker * price_prediction - state.world.factory_get_input_cost_per_worker(facids);

		auto gradient = get_profit_gradient(
			profit_per_worker,
			secondary,
			secondary * state.world.province_get_labor_demand_satisfaction(pid, labor::high_education),
			1.f / size * economy::secondary_employment_output_bonus,
			wage_high_education * 1.01f,
			primary_employment
		);

		auto unqualified_next = unqualified
			+ ve::min(0.01f * state.world.factory_get_size(facids),
				ve::max(-0.04f * state.world.factory_get_size(facids),
					1'000.f * gradient.primary[0]
				)
			);
		auto primary_next = primary
			+ ve::min(0.01f * state.world.factory_get_size(facids),
				ve::max(-0.04f * state.world.factory_get_size(facids),
					1'000.f * gradient.primary[1]
				)
			);
		auto secondary_next = secondary
			+ ve::min(0.01f * state.world.factory_get_size(facids),
				ve::max(-0.04f * state.world.factory_get_size(facids),
					1'000.f * gradient.secondary
				)
			);

		// do not hire too expensive workers:
		// ideally decided by factory budget but it is what it is

		auto budget = factory_profit_to_wage_bound * state.world.factory_get_size(facids) * profit_per_worker;

		unqualified_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(budget / wage_no_education, unqualified_next)));
		primary_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(budget / wage_basic_education, primary_next)));
		secondary_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(budget / wage_high_education, secondary_next)));

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

	// artisans do not have natural max production size, so we use total population as a limit
	// they also don't have secondary workers

	state.world.execute_parallel_over_commodity([&](auto cids) { 
		ve::apply([&](dcon::commodity_id cid) {
			if(state.world.commodity_get_artisan_output_amount(cid) == 0.f)
				return;
			state.world.execute_serial_over_province([&](auto ids) {
				auto local_states = state.world.province_get_state_membership(ids);
				auto nations = state.world.province_get_nation_from_province_ownership(ids);
				auto markets = state.world.state_instance_get_market_from_local_market(local_states);

				//safeguard against runaway artisans target employment
				auto local_population = state.world.province_get_demographics(ids, demographics::total);

				auto min_wage = ve_artisan_min_wage(state, markets) / state.defines.alice_needs_scaling_factor;
				auto actual_wage = state.world.province_get_labor_price(ids, labor::guild_education);				
				auto mask = ve_valid_artisan_good(state, nations, cid);

				auto price_today = ve_price(state, markets, cid);
				auto supply = state.world.market_get_supply(markets, cid) + price_rigging;
				auto demand = state.world.market_get_demand(markets, cid) + price_rigging;
				auto price_speed_change = price_speed_mod * (demand / supply - supply / demand);
				auto predicted_price = price_today + price_speed_change * 10.f;

				auto base_profit = base_artisan_profit(state, markets, nations, cid, predicted_price);
				auto base_profit_per_worker = base_profit / artisans_per_employment_unit;
				auto current_employment_target = state.world.province_get_artisan_score(ids, cid);
				auto current_employment_actual = current_employment_target
					* state.world.province_get_labor_demand_satisfaction(ids, labor::guild_education);
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
				auto decay = ve::select(base_profit < 0.f, 0.9f, 1.f);
				auto new_employment = ve::select(mask, ve::max(current_employment_target * decay + 10.f * gradient / state.world.commodity_get_artisan_output_amount(cid), 0.0f), 0.f);
				state.world.province_set_artisan_score(ids, cid, ve::min(local_population, new_employment));
				ve::apply(
					[](float x) {
							assert(std::isfinite(x));
					}, new_employment
				);
			});
		}, cids);
	});
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
	state.world.execute_serial_over_province([&](auto ids) {
		auto nations = state.world.province_get_nation_from_province_ownership(ids);
		auto mobilization_impact =
			ve::select(
				state.world.nation_get_is_mobilized(nations),
				military::ve_mobilization_impact(state, nations), 1.f
			);
		update_artisan_consumption(state, ids, mobilization_impact);
	});

	state.world.execute_parallel_over_market([&](auto markets) {
		auto states = state.world.market_get_zone_from_local_market(markets);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
		auto mobilization_impact =
			ve::select(
				state.world.nation_get_is_mobilized(nations),
				military::ve_mobilization_impact(state, nations), 1.f
			);
		ve::apply(
			[&](
				dcon::state_instance_id s,
				dcon::market_id m,
				dcon::nation_id n,
				float mob_impact
				) {
					auto capital = state.world.state_instance_get_capital(s);
					province::for_each_province_in_state_instance(state, s, [&](auto p) {
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
						update_rgo_consumption(state, p, mob_impact);
					});
				}, states, markets, nations, mobilization_impact
		);
	});
}

float factory_type_build_cost(sys::state& state, dcon::nation_id n, dcon::market_id m, dcon::factory_type_id factory_type) {
	auto fat = dcon::fatten(state.world, factory_type);
	auto& costs = fat.get_construction_costs();

	float factory_mod = state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::factory_cost) + 1.0f;
	float admin_eff = state.world.nation_get_administrative_efficiency(state.local_player_nation);
	float admin_cost_factor = (2.0f - admin_eff) * factory_mod;

	auto total = 0.0f;
	for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
		auto cid = costs.commodity_type[i];
		if(bool(cid)) {
			total += price(state, m, cid) * costs.commodity_amounts[i] * admin_cost_factor;
		}
	}

	return total;
}

float factory_type_output_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::market_id m,
	dcon::factory_type_id factory_type
) {
	auto fac_type = dcon::fatten(state.world, factory_type);
	float output_multiplier = nation_factory_output_multiplier(state, factory_type, n);
	float total_production = fac_type.get_output_amount() * output_multiplier;

	return total_production * price(state, m, fac_type.get_output());
}

float factory_type_input_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::market_id m,
	dcon::factory_type_id factory_type
) {
	auto fac_type = dcon::fatten(state.world, factory_type);
	auto const& inputs = fac_type.get_inputs();
	auto const& e_inputs = fac_type.get_efficiency_inputs();

	auto inputs_data = get_inputs_data(state, m, inputs);
	auto e_inputs_data = get_inputs_data(state, m, e_inputs);

	//modifiers
	auto const maint_multiplier = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	float input_multiplier = nation_factory_input_multiplier(state, fac_type, n);

	return inputs_data.total_cost * input_multiplier + e_inputs_data.total_cost * input_multiplier * maint_multiplier;
}

float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::factory_id f) {
	auto data = imitate_single_factory_consumption(state, f);
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();
	auto& direct_inputs = fac_type.get_inputs();
	auto& efficiency_inputs = fac_type.get_efficiency_inputs();
	auto result = 0.f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(direct_inputs.commodity_type[i]) {
			if(direct_inputs.commodity_type[i] == c) {
				result += data.consumption.direct_inputs_scale * direct_inputs.commodity_amounts[i];
				break;
			}
		} else {
			break;
		}
	}
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(efficiency_inputs.commodity_type[i]) {
			if(efficiency_inputs.commodity_type[i] == c) {
				result += data.consumption.efficiency_inputs_scale * efficiency_inputs.commodity_amounts[i];
				break;
			}
		} else {
			break;
		}
	}
	return result;
}

float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	auto result = 0.f;
	for(auto location : state.world.province_get_factory_location(p)) {
		result += estimate_factory_consumption(state, c, location.get_factory());
	}
	return result;
}
float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::state_instance_id s) {
	auto result = 0.f;
	province::for_each_province_in_state_instance(state, s, [&](dcon::province_id p) {
		result += estimate_factory_consumption(state, c, p);
	});
	return result;
}
float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::nation_id n) {
	auto result = 0.f;
	for(auto ownership : state.world.nation_get_province_ownership(n)) {
		result += estimate_factory_consumption(state, c, ownership.get_province());
	}
	return result;
}
float estimate_factory_consumption(sys::state& state, dcon::commodity_id c) {
	auto result = 0.f;
	state.world.for_each_factory([&](auto f) {
		result += estimate_factory_consumption(state, c, f);
	});
	return result;
}

float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p, dcon::commodity_id output) {
	auto mob_impact = military::mobilization_impact(state, state.world.province_get_nation_from_province_ownership(p));
	auto data = imitate_artisan_consumption(state, p, output, mob_impact);
	auto& direct_inputs = state.world.commodity_get_artisan_inputs(output);
	auto result = 0.f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(direct_inputs.commodity_type[i]) {
			if(direct_inputs.commodity_type[i] == c) {
				result += data.consumption.direct_inputs_scale * direct_inputs.commodity_amounts[i];
				break;
			}
		} else {
			break;
		}
	}
	return result;
}
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	auto result = 0.f;
	state.world.for_each_commodity([&](auto output) {
		result += estimate_artisan_consumption(state, c, p, output);
	});
	return result;
}
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::state_instance_id s) {
	auto result = 0.f;
	province::for_each_province_in_state_instance(state, s, [&](dcon::province_id p) {
		result += estimate_artisan_consumption(state, c, p);
	});
	return result;
}
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::nation_id n) {
	auto result = 0.f;
	for(auto ownership : state.world.nation_get_province_ownership(n)) {
		result += estimate_artisan_consumption(state, c, ownership.get_province());
	}
	return result;
}
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c) {
	auto result = 0.f;
	state.world.for_each_province([&](auto p) {
		result += estimate_artisan_consumption(state, c, p);
	});
	return result;
}


float rgo_potential_size(sys::state const& state, dcon::nation_id n, dcon::province_id p, dcon::commodity_id c) {
	bool is_mine = state.world.commodity_get_is_mine(c);
	// - We calculate its potential size which is its base size x (technology-bonus-to-specific-rgo-good-size +
	// technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
	auto rgo_ownership = state.world.province_get_landowners_share(p) + state.world.province_get_capitalists_share(p);
	auto sz = state.world.province_get_rgo_potential(p, c) * rgo_ownership;
	auto pmod = state.world.province_get_modifier_values(p, is_mine ? sys::provincial_mod_offsets::mine_rgo_size : sys::provincial_mod_offsets::farm_rgo_size);
	auto nmod = state.world.nation_get_modifier_values(n, is_mine ? sys::national_mod_offsets::mine_rgo_size : sys::national_mod_offsets::farm_rgo_size);
	auto specific_pmod = state.world.nation_get_rgo_size(n, c);
	auto bonus = std::max(0.f, pmod + nmod + specific_pmod + 1.0f);
	return std::max(sz * bonus, 0.00f);
}

float rgo_employment(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	return state.world.province_get_rgo_target_employment(p, c)
		* state.world.province_get_labor_demand_satisfaction(p, labor::no_education);
}
float rgo_employment(sys::state& state, dcon::province_id p) {
	float total = 0.f;
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		total += rgo_employment(state, c, p);
	});
	return total;
}


float rgo_max_employment(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	return state.world.province_get_rgo_size(p, c);
}
float rgo_max_employment(sys::state& state, dcon::province_id p) {
	float total = 0.f;
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		total += rgo_max_employment(state, c, p);
	});
	return total;
}


float land_maximum_employment(sys::state& state, dcon::province_id id) {
	auto owner = state.world.province_get_nation_from_province_ownership(id);
	return rgo_max_employment(state, id) + economy::subsistence_max_pseudoemployment(state, owner, id);
}
float land_employment(sys::state& state, dcon::province_id id) {
	auto owner = state.world.province_get_nation_from_province_ownership(id);
	return rgo_employment(state, id) + state.world.province_get_subsistence_employment(id);
}
float rgo_maximum_employment(sys::state& state, dcon::province_id id) {
	auto owner = state.world.province_get_nation_from_province_ownership(id);
	return rgo_max_employment(state, id);
}

float rgo_output(sys::state& state, dcon::commodity_id c, dcon::province_id id) {
	return state.world.province_get_rgo_output(id, c);
}
float rgo_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id) {
	auto result = 0.f;
	province::for_each_province_in_state_instance(state, id, [&](dcon::province_id p) {
		result += rgo_output(state, c, p);
	});
	return result;
}
float rgo_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id) {
	auto result = 0.f;
	state.world.nation_for_each_province_ownership(id, [&](auto poid) {
		result += rgo_output(state, c, state.world.province_ownership_get_province(poid));
	});
	return result;
}
float rgo_output(sys::state& state, dcon::commodity_id c) {
	auto result = 0.f;
	state.world.for_each_province([&](auto pid) {
		result += rgo_output(state, c, pid);
	});
	return result;
}

float rgo_potential_output(sys::state& state, dcon::commodity_id c, dcon::province_id id) {
	return state.world.commodity_get_rgo_amount(c)
		* state.world.province_get_rgo_efficiency(id, c)
		/ state.defines.alice_rgo_per_size_employment
		* state.world.province_get_rgo_size(id, c);
}

float rgo_income(sys::state& state, dcon::province_id id) {
	return state.world.province_get_rgo_profit(id);
}
float rgo_income(sys::state& state, dcon::commodity_id c, dcon::province_id id) {
	auto sid = state.world.province_get_state_membership(id);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);
	return rgo_potential_output(state, c, id)
		* price(state, mid, c)
		* state.world.market_get_supply_sold_ratio(mid, c);
}



float artisan_employment_target(sys::state& state, dcon::commodity_id c, dcon::province_id id) {
	return state.world.province_get_artisan_score(id, c);
}
float artisan_employment_target(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id) {
	auto result = 0.f;
	province::for_each_province_in_state_instance(state, id, [&](dcon::province_id p) {
		result += artisan_employment_target(state, c, p);
	});
	return result;
}
float artisan_employment_target(sys::state& state, dcon::commodity_id c, dcon::market_id id) {	
	return artisan_employment_target(state, c, state.world.market_get_zone_from_local_market(id));
}

float artisan_output(sys::state& state, dcon::commodity_id c, dcon::province_id id) {
	return state.world.province_get_artisan_actual_production(id, c);
}
float artisan_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id) {
	auto result = 0.f;
	province::for_each_province_in_state_instance(state, id, [&](dcon::province_id p) {
		result += artisan_output(state, c, p);
	});
	return result;
}
float artisan_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id) {
	auto result = 0.f;
	state.world.nation_for_each_province_ownership(id, [&](auto poid) {
		result += artisan_output(state, c, state.world.province_ownership_get_province(poid));
	});
	return result;
}
float artisan_output(sys::state& state, dcon::commodity_id c) {
	auto result = 0.f;
	state.world.for_each_province([&](auto pid) {
		result += artisan_output(state, c, pid);
	});
	return result;
}

float factory_output(sys::state& state, dcon::commodity_id c, dcon::province_id id) {
	auto result = 0.f;
	state.world.province_for_each_factory_location(id, [&](auto flid) {
		auto fid = state.world.factory_location_get_factory(flid);
		auto ftid = state.world.factory_get_building_type(fid);
		if(state.world.factory_type_get_output(ftid) == c) {
			result += state.world.factory_get_output(fid);
		}
	});
	return result;
}
float factory_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id) {
	auto result = 0.f;
	province::for_each_province_in_state_instance(state, id, [&](dcon::province_id p) {
		result += factory_output(state, c, p);
	});
	return result;
}
float factory_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id) {
	auto result = 0.f;
	state.world.nation_for_each_province_ownership(id, [&](auto poid) {
		result += factory_output(state, c, state.world.province_ownership_get_province(poid));
	});
	return result;
}
float factory_output(sys::state& state, dcon::commodity_id c) {
	auto result = 0.f;
	state.world.for_each_province([&](auto pid) {
		result += factory_output(state, c, pid);
	});
	return result;
}

breakdown_commodity explain_output(sys::state& state, dcon::commodity_id c, dcon::province_id id){
	return {
		.rgo = rgo_output(state, c, id),
		.guild = artisan_output(state, c, id),
		.factories = factory_output(state, c, id)
	};
}
breakdown_commodity explain_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id) {
	return {
		.rgo = rgo_output(state, c, id),
		.guild = artisan_output(state, c, id),
		.factories = factory_output(state, c, id)
	};
}
breakdown_commodity explain_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id) {
	return {
		.rgo = rgo_output(state, c, id),
		.guild = artisan_output(state, c, id),
		.factories = factory_output(state, c, id)
	};
}

breakdown_commodity explain_output(sys::state& state, dcon::commodity_id c) {
	return {
		.rgo = rgo_output(state, c),
		.guild = artisan_output(state, c),
		.factories = factory_output(state, c)
	};
}

bool commodity_surplus(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id){
	auto market = state.world.state_instance_get_market_from_local_market(id);
	return state.world.market_get_supply_sold_ratio(market, c) < 1.f;
}
bool commodity_shortage(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id){
	auto market = state.world.state_instance_get_market_from_local_market(id);
	return state.world.market_get_demand_satisfaction(market, c) < 1.f;
}
bool labor_surplus(sys::state& state, int32_t c, dcon::province_id id){
	return state.world.province_get_labor_supply_sold(id, c) < 1.f;
}
bool labor_shortage(sys::state& state, int32_t c, dcon::province_id id){
	return state.world.province_get_labor_demand_satisfaction(id, c) < 1.f;
}

}
