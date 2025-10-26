#include "dcon_generated.hpp"
#include "system_state.hpp"

#include "adaptive_ve.hpp"

#include "economy_production.hpp"
#include "economy_stats.hpp"
#include "demographics.hpp"
#include "construction.hpp"
#include "price.hpp"

#include "province_templates.hpp"
#include "advanced_province_buildings.hpp"

namespace economy {

constexpr float base_expansion_scale = 10.f;

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
	return (life + everyday) * 0.001f;
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

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * predicted_price;

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

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * predicted_price;

	auto input_multiplier = artisan_input_multiplier<dcon::nation_id>(state, nid);
	auto output_multiplier = artisan_output_multiplier<dcon::nation_id>(state, nid);

	return output_total * output_multiplier - input_multiplier * input_total;
}

template<typename NATIONS, typename PROV>
auto max_rgo_efficiency(sys::state& state, NATIONS n, PROV p, dcon::commodity_id c) {
	using VALUE = adaptive_ve::convert_to_float<NATIONS>;
	using MASK = adaptive_ve::convert_to_bool<NATIONS>;

	auto is_mine = state.world.commodity_get_is_mine(c);

	VALUE base_rgo_mult = state.defines.alice_base_rgo_efficiency_bonus;

	VALUE main_rgo = adaptive_ve::select<MASK, VALUE>(
		state.world.province_get_rgo(p) == c,
		base_rgo_mult,
		1.f
	);

	auto prov_mod_offset = is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff;
	auto nation_mod_offset = is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff;

	VALUE base_amount = state.world.commodity_get_rgo_amount(c);
	VALUE throughput =
		1.0f
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_throughput)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_throughput)
		+ state.world.province_get_modifier_values(p, prov_mod_offset)
		+ state.world.nation_get_modifier_values(n, nation_mod_offset);

	VALUE result = base_amount
		* main_rgo
		*
			(
				(state.world.province_get_demographics(p, demographics::literacy) + 1.f)
				/ (state.world.province_get_demographics(p, demographics::total) + 1.f)
				+ 0.05f
			)
		* adaptive_ve::max<VALUE>(0.5f, throughput)
		* state.defines.alice_rgo_boost // sizable compensation for efficiency being not free
		* adaptive_ve::max<VALUE>(0.5f, (
			1.0f
			+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_output)
			+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_output)
			+ state.world.nation_get_rgo_goods_output(n, c)
		));

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

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * price(state, market, c);

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

template<typename SET>
detailed_commodity_set add_details_to_commodity_set(sys::state const& state, dcon::market_id markets, SET const& set, float multiplier, bool is_input) {
	detailed_commodity_set result{};

	for(uint32_t j = 0; j < SET::set_size; ++j) {
		auto cid = set.commodity_type[j];

		if(!cid) {
			break;
		}

		result.commodity_type[j] = cid;
		result.commodity_base_amount[j] = set.commodity_amounts[j];
		result.commodity_actual_amount[j] = set.commodity_amounts[j] * multiplier;
		if(is_input) {
			result.efficient_ratio[j] = state.world.market_get_demand_satisfaction(markets, cid);
		} else {
			result.efficient_ratio[j] = state.world.market_get_supply_sold_ratio(markets, cid);
		}
		result.commodity_price[j] = state.world.market_get_price(markets, cid);
	}

	return result;
}

template<typename M, typename SET>
ve_inputs_data get_inputs_data(sys::state const& state, M markets, SET const& inputs) {
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
inputs_data get_inputs_data(sys::state const& state, dcon::market_id markets, SET const& inputs) {
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

template<typename PROV, typename SET, typename VALUE>
void save_inputs_to_buffers(
	sys::state& state,
	PROV provs,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_demanded,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_consumed,
	SET const& inputs,
	VALUE scale,
	VALUE min_available
) {
	for(uint32_t i = 0; i < SET::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			auto b_index = inputs.commodity_type[i].index();
			buffer_demanded[b_index].set(provs, buffer_demanded[b_index].get(provs) + scale * inputs.commodity_amounts[i]);
			buffer_consumed[b_index].set(provs, buffer_consumed[b_index].get(provs) + scale * inputs.commodity_amounts[i] * min_available);
		} else {
			break;
		}
	}
}

inline constexpr float lack_of_efficiency_inputs_penalty = 0.25f;

float employment_units(
	sys::state const& state,
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
	sys::state const& state,
	M labor_market,
	ve::fp_vector target_workers_guild
) {
	ve::fp_vector guild_actual_size =
		target_workers_guild
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::guild_education);
	return guild_actual_size / artisans_per_employment_unit;
}
float employment_units_guild(
	sys::state const& state,
	dcon::province_id labor_market,
	float target_workers_guild
) {
	float guild_actual_size =
		target_workers_guild
		* state.world.province_get_labor_demand_satisfaction(labor_market, labor::guild_education);
	return guild_actual_size / artisans_per_employment_unit;
}


float output_multiplier_from_workers_with_high_education(
	sys::state const& state,
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
	auto& no_edu_demand = state.world.province_get_labor_demand(labor_market, labor::no_education);
	state.world.province_set_labor_demand(labor_market, labor::no_education, no_edu_demand + target_workers_no_education);

	auto& basic_edu_demand = state.world.province_get_labor_demand(labor_market, labor::basic_education);
	state.world.province_set_labor_demand(labor_market, labor::basic_education, basic_edu_demand + target_workers_basic_education);

	auto& high_edu_demand = state.world.province_get_labor_demand(labor_market, labor::high_education);
	state.world.province_set_labor_demand(labor_market, labor::high_education, high_edu_demand + target_workers_high_education);

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
	float employment_units, float output_multiplier_from_workers_with_high_education,
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

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit * throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit * throughput_multiplier
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

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit * throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit * throughput_multiplier
	};

	return result;
}

consumption_data consume(
	sys::state& state,
	dcon::province_id province,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_demanded,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_consumed,
	economy::commodity_set const& inputs,
	economy::small_commodity_set const& efficiency_inputs,
	preconsumption_data& additional_data,

	float input_multiplier, float efficiency_inputs_multiplier, float throughput_multiplier, float output_multiplier,
	float employment_units, float output_multiplier_from_workers_with_high_education,
	float max_employment,
	economy_reason reason
) {
	assert(input_multiplier >= 0.f);
	assert(throughput_multiplier >= 0.f);
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

	save_inputs_to_buffers(state, province, buffer_demanded, buffer_consumed, inputs, input_scale, additional_data.direct_inputs_data.min_available);
	save_inputs_to_buffers(state, province, buffer_demanded, buffer_consumed, efficiency_inputs, e_input_scale * efficiency_inputs_multiplier, additional_data.efficiency_inputs_data.min_available);

	consumption_data result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit
		* production_units
		* additional_data.direct_inputs_data.min_available,

		.efficiency_inputs_cost = additional_data.efficiency_inputs_cost_per_production_unit
		* production_units
		* additional_data.efficiency_inputs_data.min_available,

		.output =
			additional_data.output_amount_per_production_unit
			* production_units
			* additional_data.direct_inputs_data.min_available
			* output_multiplier_from_workers_with_high_education,

		.direct_inputs_scale = input_scale,
		.efficiency_inputs_scale = e_input_scale,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit * throughput_multiplier,
		.output_per_employment_unit =
			additional_data.output_amount_per_production_unit
			* throughput_multiplier
			* output_multiplier_from_workers_with_high_education
	};

	return result;
}

template<typename PROV>
ve_consumption_data_guild consume(
	sys::state& state,
	PROV province,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_demanded,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_consumed,
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

	save_inputs_to_buffers(state, province, buffer_demanded, buffer_consumed, inputs, input_scale, additional_data.direct_inputs_data.min_available);

	ve_consumption_data_guild result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,
		.output = additional_data.output_amount_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,

		.direct_inputs_scale = input_scale,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit * throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit * throughput_multiplier
	};

	return result;
}

template<typename P>
void update_artisan_consumption(
	sys::state& state,
	P provinces,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_demanded,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_consumed,
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

		if(target_workers.reduce() <= 0.f) {
			continue;
		}

		auto actual_workers = ve::select(valid_mask, target_workers * mobilization_impact, 0.f);
		auto employment_units = ve::select(valid_mask, consume_labor_guild(state, provinces, actual_workers), 0.f);

		ve_preconsumption_data_guild prepared_data = prepare_data_for_consumption(
			state, markets, inputs, cid, output_amount, input_multiplier, output_multiplier
		);
		ve_consumption_data_guild consumption_data = consume(
			state, provinces, buffer_demanded, buffer_consumed,
			inputs, prepared_data,
			input_multiplier, throughput_multiplier, output_multiplier,
			employment_units, economy_reason::artisan
		);
		state.world.province_set_artisan_actual_production(
			provinces,
			cid,
			consumption_data.output
		);
		total_profit =
			total_profit
			+ consumption_data.output * prepared_data.output_price * state.world.market_get_supply_sold_ratio(markets, cid)
			- consumption_data.direct_inputs_cost;
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

inline constexpr float input_multiplier_per_capitalist_percentage = -2.5f;

namespace factory_operation {
input_multipliers_explanation explain_input_multiplier(sys::state const& state, dcon::factory_id f) {
	input_multipliers_explanation result{ };

	auto fac = fatten(state.world, f);
	auto p = fac.get_province_from_factory_location();
	auto s = p.get_state_membership();
	auto m = s.get_market_from_local_market();
	auto n = p.get_nation_from_province_ownership();
	auto factory_type = state.world.factory_get_building_type(f);

	{
		float size = state.world.factory_get_size(f);
		float small_size_effect = 1.f;
		auto per_level_employment = state.world.factory_type_get_base_workforce(factory_type);
		float small_bound = per_level_employment * 5.f;
		if(size < small_bound) {
			result.from_scale = 0.5f + size / small_bound * 0.5f;
		} else {
			result.from_scale = 1.f;
		}
	}

	{
		float total_state_pop = std::max(0.01f, state.world.state_instance_get_demographics(s, demographics::total));
		float capitalists = state.world.state_instance_get_demographics(s, demographics::to_key(state, state.culture_definitions.capitalists));
		float owner_fraction = total_state_pop > 0
			? std::min(
				0.05f,
				capitalists / total_state_pop)
			: 0.0f;

		result.from_competition = 1.f + input_multiplier_per_capitalist_percentage * owner_fraction;
	}

	{
		result.from_modifiers = state.defines.alice_inputs_base_factor
			+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_input)
			+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input);

		result.from_modifiers = std::max(0.1f, result.from_modifiers);
	}

	{
		result.from_specialisation = priority_multiplier(state, factory_type, n);
	}

	{
		result.from_triggered_modifiers = std::max(0.1f, state.world.factory_get_triggered_modifiers(f));
	}

	{
		result.total = result.from_competition
			* result.from_modifiers
			* result.from_triggered_modifiers
			* result.from_scale
			* result.from_specialisation;
	}

	return result;
}
}

float factory_input_multiplier(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s) {
	float size = state.world.factory_get_size(fac);
	float small_size_effect = 1.f;
	auto factory_type = state.world.factory_get_building_type(fac);
	auto per_level_employment = state.world.factory_type_get_base_workforce(factory_type);
	float small_bound = per_level_employment * 5.f;
	if(size < small_bound) {
		small_size_effect = 0.5f + size / small_bound * 0.5f;
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

	auto modifiers = state.defines.alice_inputs_base_factor
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_input)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input);

	auto competition = 1.f + owner_fraction * input_multiplier_per_capitalist_percentage;

	// sanitise
	modifiers = std::max(0.1f, modifiers);

	return std::max(
		0.001f,
		small_size_effect
		* mult
		* competition
		* modifiers
	);
}


namespace factory_operation {
throughput_multipliers_explanation explain_throughput_multiplier(sys::state const& state, dcon::factory_id f) {
	throughput_multipliers_explanation result{ };

	auto fac = fatten(state.world, f);
	auto factory_type = state.world.factory_get_building_type(f);
	auto p = fac.get_province_from_factory_location();
	auto s = p.get_state_membership();
	auto m = s.get_market_from_local_market();
	auto n = p.get_nation_from_province_ownership();

	{
		result.from_scale = (1.f + fac.get_size() / factory_type.get_base_workforce() / 100.f);
	}

	{
		result.base = production_throughput_multiplier;
	}

	{
		auto output = state.world.factory_type_get_output(factory_type);
		auto national_t = state.world.nation_get_factory_goods_throughput(n, output);
		auto provincial_fac_t = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput);
		auto nationnal_fac_t = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_throughput);
		auto triggered = state.world.factory_get_triggered_modifiers(fac);
		result.from_modifiers = 1.f
			* std::max(0.f, 1.f + national_t)
			* std::max(0.f, 1.f + provincial_fac_t)
			* std::max(0.f, 1.f + nationnal_fac_t)
			* std::max(0.f, 1.f + triggered);
	}

	{
		result.total = result.from_modifiers * result.from_scale * result.base;
	}

	return result;
}
}

float factory_throughput_multiplier(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s, float size) {
	auto fac_type = state.world.factory_get_building_type(fac);
	auto output = state.world.factory_type_get_output(fac_type);
	auto national_t = state.world.nation_get_factory_goods_throughput(n, output);
	auto provincial_fac_t = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput);
	auto nationnal_fac_t = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_throughput);
	auto triggered = state.world.factory_get_triggered_modifiers(fac);

	auto result = 1.f
		* std::max(0.f, 1.f + national_t)
		* std::max(0.f, 1.f + provincial_fac_t)
		* std::max(0.f, 1.f + nationnal_fac_t)
		* (1.f + size / state.world.factory_type_get_base_workforce(fac_type) / 100.f)
		* std::max(0.f, 1.f + triggered);

	return production_throughput_multiplier * result;
}

namespace factory_operation {
output_multipliers_explanation explain_output_multiplier(sys::state const& state, dcon::factory_id f) {
	output_multipliers_explanation result{};

	auto fac = fatten(state.world, f);
	auto factory_type = state.world.factory_get_building_type(f);
	auto p = fac.get_province_from_factory_location();
	auto s = p.get_state_membership();
	auto m = s.get_market_from_local_market();
	auto n = p.get_nation_from_province_ownership();

	{
		result.from_modifiers = factory_output_multiplier_no_secondary_workers(state, f, n, p);
	}

	{
		result.from_secondary_workers = output_multiplier_from_workers_with_high_education(
			state, p,
			fac.get_secondary_employment(), fac.get_size()
		);
	}

	{
		inputs_data efficiency_inputs_data = get_inputs_data<economy::small_commodity_set>(state, m, factory_type.get_efficiency_inputs());
		result.from_efficiency_goods = (1.f - lack_of_efficiency_inputs_penalty)
			+ lack_of_efficiency_inputs_penalty * efficiency_inputs_data.min_available;
	}

	{
		inputs_data inputs_data = get_inputs_data<economy::commodity_set>(state, m, factory_type.get_inputs());
		result.from_inputs_lack = inputs_data.min_available;
	}

	{
		result.total =
			result.from_efficiency_goods
			* result.from_inputs_lack
			* result.from_modifiers
			* result.from_secondary_workers;

		result.total_ignore_inputs =
			result.from_efficiency_goods
			* result.from_modifiers
			* result.from_secondary_workers;
	}

	return result;
}
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

	auto last_output = state.world.factory_get_output(f) * local_price * state.world.market_get_supply_sold_ratio(market, output_commodity);
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
		auto expansion_scale = base_expansion_scale / state.world.factory_type_get_construction_time(ftid);
		expansion_cost = std::min(profit * 0.1f, expansion_scale * costs_data.total_cost)
			* std::max(0.f, costs_data.min_available - 0.5f);
	}

	return {
		.inputs = last_inputs,
		.wages = wages,
		.maintenance = maintenance_cost,
		.expansion = expansion_cost,
		.output = last_output,
		.profit = last_output - wages - last_inputs - expansion_cost
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
	float actual_workers = target_workers * mobilization_impact;
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
	float throughput_multiplier = factory_throughput_multiplier(state, fac, n, p, s, fac.get_size());
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
		output_multiplier_from_workers_with_high_education(
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
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_demanded,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_consumed,
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
	float throughput_multiplier = factory_throughput_multiplier(state, f, n, p, s, fac.get_size());
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
		state, p, buffer_demanded, buffer_consumed,
		direct_inputs, efficiency_inputs,
		base_data,
		input_multiplier, mfactor, throughput_multiplier, output_multiplier,
		employment_units,
		output_multiplier_from_workers_with_high_education(
			state, fac.get_province_from_factory_location(), fac.get_secondary_employment(), max_employment
		), max_employment, economy_reason::factory
	);

	auto current_size = state.world.factory_get_size(f);
	auto ftid = state.world.factory_get_building_type(f);
	auto base_size = state.world.factory_type_get_base_workforce(ftid);
	auto construction_units = 0.1f * current_size / base_size;
	auto construction_units_per_day = construction_units / state.world.factory_type_get_construction_time(ftid);
	auto maintenance_scale = construction_units_per_day * construction_units_to_maintenance_units;
	auto& costs = state.world.factory_type_get_construction_costs(ftid);
	auto costs_data = get_inputs_data(state, m, costs);
	save_inputs_to_buffers(state, p, buffer_demanded, buffer_consumed, costs, maintenance_scale, costs_data.min_available);
	auto maintenance_cost = costs_data.total_cost * costs_data.min_available * maintenance_scale;
	float actual_wages = get_total_wage(state, f);
	float actual_profit =
		data.output
		* base_data.output_price
		* state.world.market_get_supply_sold_ratio(m, fac_type.get_output())
		- data.direct_inputs_cost
		- data.efficiency_inputs_cost
		- actual_wages
		- maintenance_cost;

	auto expansion_scale = base_expansion_scale / state.world.factory_type_get_construction_time(ftid);
	if(actual_profit > 0 && total_employment >= current_size * expansion_trigger) {
		// if we are at max amount of workers and we are profitable, spend 10% of the profit on actual expansion
		expansion_scale = std::min(expansion_scale, actual_profit * 0.1f / costs_data.total_cost);
		// do not expand factories when direct inputs are scarce
		expansion_scale *= std::max(0.f, base_data.direct_inputs_data.min_available - 0.5f);
		save_inputs_to_buffers(state, p, buffer_demanded, buffer_consumed, costs, expansion_scale, costs_data.min_available);
		state.world.factory_set_size(fac, current_size + base_size * expansion_scale * costs_data.min_available);
		assert(std::isfinite(state.world.factory_get_size(fac)));
		assert(state.world.factory_get_size(fac) > 0.f);
	} else if(actual_profit < 0) {
		state.world.factory_set_size(fac, std::max(500.f, current_size - 0.01f * base_size * expansion_scale));
		assert(std::isfinite(state.world.factory_get_size(fac)));
		assert(state.world.factory_get_size(fac) > 0.f);
	}

	if(state.world.commodity_get_uses_potentials(fac_type.get_output())) {
		auto new_size = state.world.factory_get_size(fac);
		// A factory may be created in history but have no potentials for such factory in the province
		// Then we apply min size
		state.world.factory_set_size(fac, std::min(new_size, std::max(500.f, state.world.province_get_factory_max_size(p, fac_type.get_output()))));
		assert(std::isfinite(state.world.factory_get_size(fac)));
		assert(state.world.factory_get_size(fac) > 0.f);
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
			// if the province has no valid state instance (eg uncolonized) then skip
			if(!sid) {
				return;
			}
			auto mid = state.world.state_instance_get_market_from_local_market(sid);
			register_domestic_supply(state, mid, cid, production, economy_reason::artisan);
		});
	}
}

void update_factories_production(
	sys::state& state
) {
	// could be done in parallel over markets???
	state.world.for_each_factory([&](auto factory) {
		auto province = state.world.factory_get_province_from_factory_location(factory);
		auto local_state = state.world.province_get_state_membership(province);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);
		auto production = state.world.factory_get_output(factory);
		auto factory_type = state.world.factory_get_building_type(factory);
		auto cid = state.world.factory_type_get_output(factory_type);
		register_domestic_supply(state, local_market, cid, production, economy_reason::factory);
	});
}


// currently rgos consume only labor and efficiency goods
template<typename PROV>
void update_rgo_consumption(
	sys::state& state,
	PROV p,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_demanded,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_consumed,
	ve::fp_vector mobilization_impact
) {
	auto n = state.world.province_get_nation_from_province_ownership(p);
	auto sid = state.world.province_get_state_membership(p);
	auto m = state.world.state_instance_get_market_from_local_market(sid);

	state.world.province_set_rgo_profit(p, 0.f);

	// update efficiency and consume efficiency_inputs:
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto size = state.world.province_get_rgo_size(p, c);
		if(size.reduce() <= 0.f) {
			return;
		}

		auto max_efficiency = max_rgo_efficiency(state, n, p, c);
		auto free_efficiency = max_efficiency * 0.1f;
		auto current_efficiency = state.world.province_get_rgo_efficiency(p, c);
		// Calculation of e_inputs is duplicated in rgo_calculate_actual_efficiency_inputs function for use in the UI.
		auto& e_inputs = state.world.commodity_get_rgo_efficiency_inputs(c);
		auto e_inputs_data = get_inputs_data(state, m, e_inputs);
		// we try to update our efficiency according to current profit derivative
		// if higher efficiency brings profit, we increase it
		// 10% of max efficiency is free
		auto cost_derivative = adaptive_ve::select<ve::mask_vector, ve::fp_vector>(current_efficiency > free_efficiency, e_inputs_data.total_cost, 0.f);

		auto profit_derivative =
			state.world.commodity_get_rgo_amount(c)
			/ state.defines.alice_rgo_per_size_employment
			* state.world.market_get_price(m, c);
		auto efficiency_growth = 100.f * (profit_derivative - cost_derivative);
		// we assume that we can maintain efficiency even when there is not enough goods on the market:
		// efficiency goods are "preserved" (but still demanded to "update")
		// it's very risky to decrease efficiency depending on available efficiency goods directly
		// because it might lead to instability cycles
		// there is some natural decay of efficiency
		// we decrease efficiency outside of decay only if decreasing efficiency is profitable
		// but to increase efficiency, there should be enough of goods on the local market unless it's a free efficiency

		efficiency_growth = ve::select(
			(efficiency_growth > 0.f) && (current_efficiency > free_efficiency),
			efficiency_growth * e_inputs_data.min_available,
			efficiency_growth
		);

		auto new_efficiency = ve::min(max_efficiency, ve::max(free_efficiency, current_efficiency * 0.9999f + efficiency_growth));
		state.world.province_set_rgo_efficiency(p, c, ve::select(size == 0.f, 0.f, new_efficiency));

		auto workers = state.world.province_get_rgo_target_employment(p, c)
			* state.world.province_get_labor_demand_satisfaction(p, labor::no_education)
			* mobilization_impact;
		auto demand_scale = workers * ve::max(new_efficiency - free_efficiency, 0.f);

		save_inputs_to_buffers(state, p, buffer_demanded, buffer_consumed, e_inputs, demand_scale, e_inputs_data.min_available);

		auto target = state.world.province_get_rgo_target_employment(p, c);
		auto cur_labor_demand = state.world.province_get_labor_demand(p, labor::no_education);
		state.world.province_set_labor_demand(p, labor::no_education, cur_labor_demand + target);

		auto per_worker = state.world.commodity_get_rgo_amount(c)
			* state.world.province_get_rgo_efficiency(p, c)
			/ state.defines.alice_rgo_per_size_employment;
		auto amount = workers * per_worker;

		auto profit =
			ve::select(
				state.world.commodity_get_money_rgo(c),
				amount * state.world.market_get_price(m, c),
				amount * state.world.market_get_price(m, c) * state.world.market_get_supply_sold_ratio(m, c)
			);

		state.world.province_set_rgo_profit(p, state.world.province_get_rgo_profit(p) + profit);
		auto wages = workers
			* state.world.province_get_labor_price(p, labor::no_education);
		auto spent_on_efficiency = demand_scale * e_inputs_data.total_cost * e_inputs_data.min_available;

		state.world.province_set_rgo_profit(p, state.world.province_get_rgo_profit(p) - wages - spent_on_efficiency);
		state.world.province_set_rgo_output(p, c, amount);
		state.world.province_set_rgo_output_per_worker(p, c, per_worker);
	});
}

void update_rgo_production(sys::state& state) {
	state.world.for_each_province([&](auto province) {
		auto local_state = state.world.province_get_state_membership(province);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);
		auto controller = state.world.province_get_nation_from_province_control(province);
		auto owner = state.world.province_get_nation_from_province_ownership(province);

		if(!owner) {
			return;
		}

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(state.world.commodity_get_rgo_amount(c) < 0.f) {
				return;
			}
			auto amount = state.world.province_get_rgo_output(province, c);
			register_domestic_supply(state, local_market, c, amount, economy_reason::rgo);

			// if the rgo is a money RGO and it is not rebel controlled, give the controller the cash from it
			if(state.world.commodity_get_money_rgo(c) && bool(controller)) {
				assert(
					std::isfinite(
						amount
						* state.defines.gold_to_cash_rate
						* state.world.commodity_get_cost(c)
					)
					&& amount * state.defines.gold_to_cash_rate >= 0.0f
				);

				// #CAUTION# changes nation values!
				auto& cur_money = state.world.nation_get_stockpiles(controller, money);
				state.world.nation_set_stockpiles(controller, money, cur_money + amount
					* state.defines.gold_to_cash_rate
					* state.world.commodity_get_cost(c)
				);
			}
		});
	});
}

template<size_t N, typename VALUE>
struct employment_data {
	std::array<VALUE, N> target;
	std::array<VALUE, N> actual;
	std::array<VALUE, N> power;
	std::array<VALUE, N> wage;
};

template<size_t N, typename VALUE>
struct employment_vector {
	std::array<VALUE, N> primary;
	VALUE secondary;
};

template<typename VALUE>
VALUE gradient_employment_i(
	VALUE expected_profit_per_perfect_worker,
	VALUE power,
	VALUE wage,
	VALUE secondary_employment,
	VALUE secondary_power
) {
	return expected_profit_per_perfect_worker * power * (1.f + secondary_employment * secondary_power) - wage;
}

template<typename VALUE>
VALUE gradient_employment_i(
	VALUE expected_profit_per_perfect_worker,
	VALUE power,
	VALUE wage
) {
	return expected_profit_per_perfect_worker * power - wage;
}

template<size_t N, typename VALUE>
VALUE gradient_employment_secondary(
	VALUE expected_profit_per_perfect_worker,
	VALUE secondary_power,
	VALUE secondary_wage,
	employment_data<N, VALUE>& primary_employment
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

template<size_t N, typename VALUE>
employment_vector<N, VALUE> get_profit_gradient(
	VALUE expected_profit_per_perfect_primary_worker,
	VALUE secondary_target,
	VALUE secondary_actual,
	VALUE secondary_power,
	VALUE secondary_wage,
	employment_data<N, VALUE>& primary_employment
) {
	employment_vector<N, VALUE> result{ };
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

template<typename VALUE>
VALUE gradient_to_employment_change(VALUE gradient, VALUE wage, VALUE current_employment, VALUE sat) {

	if constexpr(std::same_as<VALUE, float>) {
		auto mult = 
			gradient > 0.f
			? std::max(
				0.f,
				sat - 0.5f
			)
			: 1.f
		;
		return std::min(0.01f * (current_employment + 100.f),
			std::max(-0.04f * (current_employment + 100.f),
				gradient / wage
			)
		) * mult;
	} else {
		auto mult = ve::select(
			gradient > 0.f,
			ve::max(
				0.f,
				sat - 0.5f
			),
			1.f
		);
		return ve::min(0.01f * (current_employment + 100.f),
			ve::max(-0.04f * (current_employment + 100.f),
				gradient / wage
			)
		) * mult;
	}
}

void update_employment(sys::state& state, float presim_employment_mult) {
	// note: markets are independent, so nations are independent:
	// so we can execute in parallel over nations but not over provinces

	concurrency::parallel_for(uint32_t(0), state.world.commodity_size(), [&](uint32_t k) {
		dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };
		auto rgo_output = state.world.commodity_get_rgo_amount(c);
		if(rgo_output <= 0.f) {
			return;
		}

		province::ve_for_each_land_province(state, [&](auto pids) {
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
			auto supply = state.world.market_get_supply(m, c);
			auto demand = state.world.market_get_demand(m, c);

			auto price_speed_change =
				state.world.commodity_get_money_rgo(c)
				? 0.f
				: price_properties::change(current_price, supply, demand);
			auto predicted_price = current_price + price_speed_change * 0.5f;

			auto gradient = gradient_employment_i<ve::fp_vector>(
				output_per_worker * predicted_price,
				1.f,
				wage_per_worker * (1.f + aristocrats_greed)
			);

			auto mult = ve::select(
				gradient > 0.f,
				ve::max(
					0.f,
					state.world.province_get_labor_demand_satisfaction(pids, labor::no_education) - 0.5f
				),
				1.f
			);

			auto new_employment = ve::max((current_employment_target + 10.f * presim_employment_mult * gradient / wage_per_worker * mult), 0.0f);

			// we don't want wages to rise way too high relatively to profits
			// as we do not have actual budgets, we  consider that our workers budget is as follows
			new_employment = ve::min(rgo_profit_to_wage_bound * output_per_worker * predicted_price * current_size / wage_per_worker, new_employment);
			new_employment = ve::min(new_employment, current_size);
			state.world.province_set_rgo_target_employment(pids, c, new_employment);
			state.world.province_set_rgo_output(pids, c, output_per_worker * current_employment);
		});
	});

	state.world.execute_serial_over_factory([&](auto facids) {
		auto pid = state.world.factory_get_province_from_factory_location(facids);
		auto sid = state.world.province_get_state_membership(pid);
		auto mid = state.world.state_instance_get_market_from_local_market(sid);
		auto factory_type = state.world.factory_get_building_type(facids);


		auto min_available_input = ve::apply([&](auto ftid, auto factory_market) {
			auto inputs = state.world.factory_type_get_inputs(ftid);
			auto inputs_data = get_inputs_data(state, factory_market, inputs);
			return inputs_data.min_available;
		}, factory_type, mid);

		auto output = state.world.factory_type_get_output(factory_type);

		auto price_output = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_price(market, cid);
		}, mid, output);

		auto supply = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_supply(market, cid);
		}, mid, output);

		auto demand = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_demand(market, cid);
		}, mid, output);

		auto output_per_worker = state.world.factory_get_output_per_worker(facids);
		auto unqualified = state.world.factory_get_unqualified_employment(facids);
		auto primary = state.world.factory_get_primary_employment(facids);
		auto secondary = state.world.factory_get_secondary_employment(facids);
		auto profit_push = output_per_worker * price_output;
		auto spending_push = state.world.factory_get_input_cost_per_worker(facids);
		auto wage_no_education = state.world.province_get_labor_price(pid, labor::no_education);
		auto wage_basic_education = state.world.province_get_labor_price(pid, labor::basic_education);
		auto wage_high_education = state.world.province_get_labor_price(pid, labor::high_education);

		employment_data<2, decltype(wage_no_education)> primary_employment{
			{ unqualified, primary },
			{
				unqualified * state.world.province_get_labor_demand_satisfaction(pid, labor::no_education),
				primary * state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education)
			},
			{ unqualified_throughput_multiplier, 1.f },
			{
				wage_no_education * (1.f + capitalists_greed) / (0.01f + state.world.province_get_labor_demand_satisfaction(pid, labor::no_education)),
				wage_basic_education * (1.f + capitalists_greed) / (0.01f + state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education))
			}
		};

		auto price_speed = price_properties::change(price_output, supply, demand);

		auto price_prediction = (price_output + price_speed);
		auto size = state.world.factory_get_size(facids);

		auto profit_per_worker = output_per_worker * price_prediction - state.world.factory_get_input_cost_per_worker(facids) * (1.f + capitalists_greed);

		auto gradient = get_profit_gradient(
			profit_per_worker,
			secondary,
			secondary * state.world.province_get_labor_demand_satisfaction(pid, labor::high_education),
			1.f / size * economy::secondary_employment_output_bonus,
			wage_high_education * (1.f + capitalists_greed) / (0.01f + state.world.province_get_labor_demand_satisfaction(pid, labor::high_education)),
			primary_employment
		);

		auto unqualified_now = unqualified * state.world.province_get_labor_demand_satisfaction(pid, labor::no_education);
		auto unqualified_next = unqualified
			+ gradient_to_employment_change(gradient.primary[0] * presim_employment_mult, wage_no_education, unqualified_now, state.world.province_get_labor_demand_satisfaction(pid, labor::no_education) * min_available_input);

		auto primary_now = primary * state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education);
		auto primary_next = primary
			+ gradient_to_employment_change(gradient.primary[1] * presim_employment_mult, wage_basic_education, primary_now, state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education) * min_available_input);

		auto secondary_now = secondary * state.world.province_get_labor_demand_satisfaction(pid, labor::high_education);
		auto secondary_next = secondary
			+ gradient_to_employment_change(gradient.secondary * presim_employment_mult, wage_high_education, secondary_now, state.world.province_get_labor_demand_satisfaction(pid, labor::high_education) * min_available_input);

		// do not hire too expensive workers:
		// ideally decided by factory budget but it is what it is

		auto budget = factory_profit_to_wage_bound * state.world.factory_get_size(facids) * output_per_worker * price_prediction;

		unqualified_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(budget / wage_no_education - 1.f, unqualified_next)));
		primary_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(budget / wage_basic_education - 1.f, primary_next)));
		secondary_next = ve::max(0.f, ve::min(state.world.factory_get_size(facids), ve::min(budget / wage_high_education - 1.f, secondary_next)));

		// no available workers at all?
		// reduce and set to zero if it's low enough
		// TODO: more complicated logic would be nice
		// when we implement migration dependent on wages
		// because high wage would attract people
		// and hiring people when they are not there yet
		// would be intended behaviour

		unqualified_next = ve::select(
			state.world.province_get_labor_supply(pid, economy::labor::no_education) == 0.f,
			ve::select(
				unqualified_next < 5.f,
				0.f,
				unqualified_next * 0.1f
			),
			unqualified_next
		);
		primary_next = ve::select(
			state.world.province_get_labor_supply(pid, economy::labor::basic_education) == 0.f,
			ve::select(
				primary_next < 5.f,
				0.f,
				primary_next * 0.1f
			),
			primary_next
		);
		secondary_next = ve::select(
			state.world.province_get_labor_supply(pid, economy::labor::high_education) == 0.f,
			ve::select(
				secondary_next < 5.f,
				0.f,
				secondary_next * 0.1f
			),
			secondary_next
		);

		auto total = unqualified_next + primary_next + secondary_next;
		auto scaler = ve::select(total > state.world.factory_get_size(facids), state.world.factory_get_size(facids) / total, 1.f);

#ifndef NDEBUG
		ve::apply([&](auto p) { assert(std::isfinite(state.world.province_get_labor_demand_satisfaction(p, labor::high_education))); },
			pid
		);
		ve::apply([&](auto factory) { if(state.world.factory_is_valid(factory)) {
			assert(std::isfinite(state.world.factory_get_size(factory)));
			assert(state.world.factory_get_size(factory) > 0.f);
		}},
			facids
		);
		ve::apply([&](auto factory, auto value) { if(state.world.factory_is_valid(factory)) assert(std::isfinite(value) && (value >= 0.f)); },
			facids, unqualified
		);
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
			province::ve_for_each_land_province(state, [&](auto ids) {
				auto local_states = state.world.province_get_state_membership(ids);
				auto nations = state.world.province_get_nation_from_province_ownership(ids);
				auto markets = state.world.state_instance_get_market_from_local_market(local_states);

				//safeguard against runaway artisans target employment
				auto local_population = state.world.province_get_demographics(ids, demographics::total);

				//auto min_wage = ve_artisan_min_wage(state, markets) / state.defines.alice_needs_scaling_factor;
				//auto actual_wage = state.world.province_get_labor_price(ids, labor::guild_education);
				auto mask = ve_valid_artisan_good(state, nations, cid);

				auto price_today = ve_price(state, markets, cid);
				auto supply = state.world.market_get_supply(markets, cid);
				auto demand = state.world.market_get_demand(markets, cid);
				auto predicted_price = price_today + price_properties::change(price_today, supply, demand) * 2.f;

				auto base_profit = base_artisan_profit(state, markets, nations, cid, predicted_price / (1.f + artisans_greed));
				auto base_profit_per_worker = base_profit / artisans_per_employment_unit;
				auto current_employment_target = state.world.province_get_artisan_score(ids, cid);
				auto current_employment_actual = current_employment_target
					* state.world.province_get_labor_demand_satisfaction(ids, labor::guild_education);
				auto profit_per_worker = ve::select(
					mask,
					base_profit_per_worker,
					std::numeric_limits<float>::lowest()
				);
				auto gradient = gradient_employment_i<ve::fp_vector>(
					profit_per_worker,
					1.f,
					0.f
				);

				// prevent artisans from expanding demand on missing goods too fast
				auto inputs_data = get_inputs_data(state, markets, state.world.commodity_get_artisan_inputs(cid));
				gradient = ve::select(gradient > 0.f, gradient * ve::max(0.01f, inputs_data.min_available - 0.3f), gradient);

				ve::fp_vector decay_profit = ve::select(base_profit < 0.f, ve::fp_vector{ 0.9f }, ve::fp_vector{ 1.f });
				ve::fp_vector decay_lack = 0.9999f + inputs_data.min_available * 0.0001f;

				auto decay = decay_lack * decay_profit;

				auto new_employment = ve::select(mask, ve::max(current_employment_target * decay + 10.f * presim_employment_mult * gradient, 0.0f), 0.f);
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
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>> buffer_demanded{};
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>> buffer_consumed{};

	for(size_t i = 0; i < state.world.commodity_size(); i++) {
		buffer_demanded.push_back(state.world.province_make_vectorizable_float_buffer());
		buffer_consumed.push_back(state.world.province_make_vectorizable_float_buffer());
	}

	province::ve_parallel_for_each_land_province(state, [&](auto ids) {
		auto nations = state.world.province_get_nation_from_province_ownership(ids);
		auto mobilization_impact =
			ve::select(
				state.world.nation_get_is_mobilized(nations),
				military::ve_mobilization_impact(state, nations), 1.f
			);
		update_artisan_consumption(state, ids, buffer_demanded, buffer_consumed, mobilization_impact);
		update_rgo_consumption(state, ids, buffer_demanded, buffer_consumed, mobilization_impact);
	});

	concurrency::parallel_for(dcon::market_id::value_base_t(0), dcon::market_id::value_base_t(state.world.market_size()), [&](auto market_raw_index) {
		auto market = dcon::market_id{ market_raw_index };
		auto sid = state.world.market_get_zone_from_local_market(market);
		auto nation = state.world.state_instance_get_nation_from_state_ownership(sid);
		auto mobilization_impact =
			state.world.nation_get_is_mobilized(nation)
			? military::mobilization_impact(state, nation)
			: 1.f;

		province::for_each_province_in_state_instance(state, sid, [&](auto p) {
			for(auto f : state.world.province_get_factory_location(p)) {
				update_single_factory_consumption(
					state,
					f.get_factory(),
					p,
					sid,
					market,
					nation, buffer_demanded, buffer_consumed,
					mobilization_impact,
					state.world.province_get_nation_from_province_control(p) != nation // is occupied
				);
			}
		});
	});

	concurrency::parallel_for(dcon::market_id::value_base_t(0), dcon::market_id::value_base_t(state.world.market_size()), [&](auto market_raw_index) {
		auto market = dcon::market_id{ market_raw_index };
		auto sid = state.world.market_get_zone_from_local_market(market);
		state.world.for_each_commodity([&](auto cid) {
			auto median_price = state.world.commodity_get_median_price(cid);

			province::for_each_province_in_state_instance(state, sid, [&](auto p) {
				auto demanded = buffer_demanded[cid.index()].get(p);
				auto consumed = buffer_consumed[cid.index()].get(p);
				assert(std::isfinite(demanded));
				assert(std::isfinite(consumed));
				register_intermediate_demand(state, market, cid, demanded);
				state.world.market_set_gdp(market, state.world.market_get_gdp(market) - consumed * median_price);
			});
		});
	});
}

float factory_type_build_cost(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::factory_type_id factory_type, bool is_pop_project) {
	auto fat = dcon::fatten(state.world, factory_type);
	auto& costs = fat.get_construction_costs();
	auto cost_factor = economy::factory_build_cost_multiplier(state, n, p, is_pop_project);

	auto sid = state.world.province_get_state_membership(p);
	auto m = state.world.state_instance_get_market_from_local_market(sid);

	auto total = 0.0f;
	for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
		auto cid = costs.commodity_type[i];
		if(bool(cid)) {
			total += price(state, m, cid) * costs.commodity_amounts[i] * cost_factor;
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

float estimate_factory_profit_margin(
	sys::state& state,
	dcon::province_id pid,
	dcon::factory_type_id factory_type
) {
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);
	auto nid = state.world.province_get_nation_from_province_ownership(pid);
	auto output_cost = factory_type_output_cost(
		state, nid, mid, factory_type
	);
	auto input_cost = factory_type_input_cost(
		state, nid, mid, factory_type
	);
	//auto wages = state.world.province_get_labor_price(pid, labor::basic_education) * state.world.factory_type_get_base_workforce(factory_type);

	return (output_cost - input_cost) / output_cost;
}

float estimate_factory_payback_time(
	sys::state& state,
	dcon::province_id pid,
	dcon::factory_type_id factory_type
) {
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);
	auto nid = state.world.province_get_nation_from_province_ownership(pid);
	auto output_cost = factory_type_output_cost(
		state, nid, mid, factory_type
	);
	auto input_cost = factory_type_input_cost(
		state, nid, mid, factory_type
	);
	//auto wages = state.world.province_get_labor_price(pid, labor::basic_education) * state.world.factory_type_get_base_workforce(factory_type);

	return factory_type_build_cost(state, nid, pid, factory_type, false) / std::max(0.f, (output_cost - input_cost));
}

float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::factory_id f) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();
	auto& direct_inputs = fac_type.get_inputs();
	auto& efficiency_inputs = fac_type.get_efficiency_inputs();
	auto result = 0.f;
	auto states = state.world.province_get_state_membership(fac.get_province_from_factory_location());
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto data = imitate_single_factory_consumption(state, f);
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(direct_inputs.commodity_type[i]) {
			if(direct_inputs.commodity_type[i] == c) {
				result +=
					data.consumption.direct_inputs_scale
					* direct_inputs.commodity_amounts[i]
					* state.world.market_get_demand_satisfaction(markets, direct_inputs.commodity_type[i]);
				break;
			}
		} else {
			break;
		}
	}
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(efficiency_inputs.commodity_type[i]) {
			if(efficiency_inputs.commodity_type[i] == c) {
				result +=
					data.consumption.efficiency_inputs_scale
					* efficiency_inputs.commodity_amounts[i]
					* state.world.market_get_demand_satisfaction(markets, efficiency_inputs.commodity_type[i]);
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
				result +=
					data.consumption.direct_inputs_scale
					* direct_inputs.commodity_amounts[i];
				break;
			}
		} else {
			break;
		}
	}
	return result;
}
float estimate_artisan_gdp_intermediate_consumption(sys::state& state, dcon::province_id p, dcon::commodity_id output) {
	auto states = state.world.province_get_state_membership(p);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto mob_impact = military::mobilization_impact(state, state.world.province_get_nation_from_province_ownership(p));
	auto data = imitate_artisan_consumption(state, p, output, mob_impact);
	auto& direct_inputs = state.world.commodity_get_artisan_inputs(output);
	auto result = 0.f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(!direct_inputs.commodity_type[i]) break;
		result +=
			data.consumption.direct_inputs_scale
			* direct_inputs.commodity_amounts[i]
			* state.world.commodity_get_median_price(direct_inputs.commodity_type[i])
			* data.base.direct_inputs_data.min_available;
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
	auto target = state.world.province_get_rgo_target_employment(p, c);
	auto sat = state.world.province_get_labor_demand_satisfaction(p, labor::no_education);
	return target * sat;
}

float rgo_wage(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	return rgo_employment(state, c, p) * state.world.province_get_labor_price(p, labor::no_education);
}

float rgo_efficiency_spending(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	auto n = state.world.province_get_nation_from_province_ownership(p);
	auto s = state.world.province_get_state_membership(p);
	auto m = state.world.state_instance_get_market_from_local_market(s);
	auto max_efficiency = max_rgo_efficiency<dcon::nation_id, dcon::province_id>(state, n, p, c);
	auto free_efficiency = max_efficiency * 0.1f;
	auto efficiency = state.world.province_get_rgo_efficiency(p, c);
	auto& e_inputs = state.world.commodity_get_rgo_efficiency_inputs(c);
	auto e_inputs_data = get_inputs_data(state, m, e_inputs);
	auto workers = rgo_employment(state, c, p);
	auto demand_scale = workers * std::max(efficiency - free_efficiency, 0.f);
	return demand_scale * e_inputs_data.total_cost * e_inputs_data.min_available;
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
	return rgo_max_employment(state, id) + economy::subsistence_max_pseudoemployment(state, id);
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
	if(state.world.commodity_get_money_rgo(c)) {
		return rgo_output(state, c, id)	* price(state, mid, c);
	}
	return rgo_output(state, c, id)
		* price(state, mid, c)
		* state.world.market_get_supply_sold_ratio(mid, c);
}

// Duplicates the calculation of RGO efficiency inputs consumption for the RGO tooltip.
commodity_set rgo_calculate_actual_efficiency_inputs(sys::state& state, dcon::nation_id n, dcon::market_id m, dcon::province_id pid, dcon::commodity_id c, float mobilization_impact) {
	auto size = state.world.province_get_rgo_size(pid, c);
	if(size <= 0.f) {
		return commodity_set{};
	}
	auto max_efficiency = max_rgo_efficiency<dcon::nation_id, dcon::province_id>(state, n, pid, c);
	auto free_efficiency = max_efficiency * 0.1f;
	auto current_efficiency = state.world.province_get_rgo_efficiency(pid, c);
	auto e_inputs = state.world.commodity_get_rgo_efficiency_inputs(c);
	auto workers = state.world.province_get_rgo_target_employment(pid, c)
		* state.world.province_get_labor_demand_satisfaction(pid, labor::no_education)
		* mobilization_impact;
	auto demand_scale = workers * std::max(state.world.province_get_rgo_efficiency(pid, c) - free_efficiency, 0.f);
	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(e_inputs.commodity_type[i]) {
			e_inputs.commodity_amounts[i] *= demand_scale;
		} else {
			break;
		}
	}

	return e_inputs;
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

float factory_total_desired_employment(sys::state const& state, dcon::factory_id f) {
	return (
		state.world.factory_get_unqualified_employment(f)
		+ state.world.factory_get_primary_employment(f)
		+ state.world.factory_get_secondary_employment(f)
	);
}
float factory_total_desired_employment_score(sys::state const& state, dcon::factory_id f) {
	return factory_total_desired_employment(state, f) / state.world.factory_get_size(f);
}
float factory_total_employment_score(sys::state const& state, dcon::factory_id f) {
	return factory_total_employment (state, f) / state.world.factory_get_size(f);
}



float estimate_intermediate_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	auto artisans = estimate_artisan_consumption(state, c, p);
	auto factories = estimate_factory_consumption(state, c, p);
	// todo: rgo
	return artisans + factories;
}

float estimate_production(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	auto artisans = artisan_output(state, c, p);
	auto factories = factory_output(state, c, p);
	auto rgo = rgo_output(state, c, p);

	return artisans + factories + rgo;
}

namespace factory_operation {
detailed_explanation explain_everything(sys::state const& state, dcon::factory_id f) {
	detailed_explanation result{ };

	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();
	auto p = fac.get_province_from_factory_location();
	auto s = p.get_state_membership();
	auto m = s.get_market_from_local_market();
	auto n = p.get_nation_from_province_ownership();


	auto& primary_inputs = fac_type.get_inputs();
	auto& efficiency_inputs = fac_type.get_efficiency_inputs();
	auto& construction_costs = state.world.factory_type_get_construction_costs(fac_type);

	inputs_data primary_inputs_data = get_inputs_data<economy::commodity_set>(state, m, primary_inputs);
	inputs_data efficiency_inputs_data = get_inputs_data<economy::small_commodity_set>(state, m, efficiency_inputs);
	inputs_data construction_costs_data = get_inputs_data<economy::commodity_set>(state, m, construction_costs);

	result.base_type = fac.get_building_type();
	result.input_multipliers = explain_input_multiplier(state, f);
	result.maintenance_multiplier = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	result.throughput_multipliers = explain_throughput_multiplier(state, f);
	result.output_multipliers = explain_output_multiplier(state, f);

	result.employment_target = {
		fac.get_unqualified_employment(),
		fac.get_primary_employment(),
		fac.get_secondary_employment()
	};

	result.employment_available_ratio = {
		state.world.province_get_labor_demand_satisfaction(p, labor::no_education),
		state.world.province_get_labor_demand_satisfaction(p, labor::basic_education),
		state.world.province_get_labor_demand_satisfaction(p, labor::high_education),
	};

	result.employment = {
		result.employment_target.unqualified * result.employment_available_ratio.unqualified,
		result.employment_target.primary * result.employment_available_ratio.primary,
		result.employment_target.secondary * result.employment_available_ratio.secondary,
	};

	result.employment_wages_per_person = {
		state.world.province_get_labor_price(p, labor::no_education),
		state.world.province_get_labor_price(p, labor::basic_education),
		state.world.province_get_labor_price(p, labor::high_education),
	};

	result.employment_wages_paid = {
		result.employment_wages_per_person.unqualified * result.employment.unqualified,
		result.employment_wages_per_person.primary * result.employment.primary,
		result.employment_wages_per_person.secondary * result.employment.secondary,
	};

	result.employment_units = employment_units(
		state,
		p,
		result.employment_target.unqualified,
		result.employment_target.primary,
		result.employment_target.secondary,
		float(fac_type.get_base_workforce())
	);

	result.production_units = result.employment_units
		* result.throughput_multipliers.total;

	result.primary_inputs = add_details_to_commodity_set(
		state, m, primary_inputs,
		result.input_multipliers.total
		* result.production_units,
		true
	);

	result.efficiency_inputs = add_details_to_commodity_set(
		state, m, efficiency_inputs,
		result.input_multipliers.total
		* result.required_efficiency_inputs_multiplier
		* result.production_units,
		true
	);

	result.output = fac_type.get_output();
	result.output_price = price(state, m, result.output);
	result.output_base_amount = fac_type.get_output_amount();
	result.output_amount_per_production_unit = result.output_base_amount
		* result.output_multipliers.total;

	result.output_amount_per_production_unit_ignore_inputs = result.output_base_amount
		* result.output_multipliers.total_ignore_inputs;

	auto cost_of_efficiency_inputs = efficiency_inputs_data.total_cost * result.maintenance_multiplier;
	auto value_from_efficiency_inputs = result.output_amount_per_production_unit_ignore_inputs
		* result.output_price
		/ result.output_multipliers.from_efficiency_goods
		* lack_of_efficiency_inputs_penalty;

	if(value_from_efficiency_inputs < cost_of_efficiency_inputs) {
		result.efficiency_inputs_worth_it = false;
		result.output_multipliers.total /= result.output_multipliers.from_efficiency_goods;
		result.output_multipliers.from_efficiency_goods = (1.f - lack_of_efficiency_inputs_penalty);
	}

	result.output_amount_per_employment_unit =
		result.output_amount_per_production_unit
		* result.throughput_multipliers.total;

	result.output_amount_per_employment_unit_ignore_inputs =
		result.output_amount_per_production_unit_ignore_inputs
		* result.throughput_multipliers.total;

	result.output_actually_sold_ratio =
		state.world.market_get_supply_sold_ratio(m, result.output);
	result.output_actual_amount =
		result.output_amount_per_production_unit
		* result.production_units;

	auto current_size = state.world.factory_get_size(f);
	auto base_size = (float)state.world.factory_type_get_base_workforce(fac_type);
	auto construction_units = 0.1f * current_size / base_size;
	auto construction_units_per_day = construction_units / state.world.factory_type_get_construction_time(fac_type);
	auto maintenance_scale = construction_units_per_day * construction_units_to_maintenance_units;
	result.maintenance_inputs = add_details_to_commodity_set(
		state, m, construction_costs, maintenance_scale, true
	);

	result.income_from_sales =
		result.output_actual_amount
		* result.output_actually_sold_ratio
		* result.output_price;
	result.spending_from_primary_inputs =
		primary_inputs_data.total_cost
		* result.input_multipliers.total
		* result.production_units
		* primary_inputs_data.min_available;
	result.spending_from_efficiency_inputs =
		efficiency_inputs_data.total_cost
		* result.input_multipliers.total
		* result.production_units
		* efficiency_inputs_data.min_available;
	result.spending_from_maintenance =
		construction_costs_data.total_cost
		* maintenance_scale
		* construction_costs_data.min_available;
	result.spending_from_wages = (
		result.employment.unqualified * result.employment_wages_per_person.unqualified
		+ result.employment.primary * result.employment_wages_per_person.primary
		+ result.employment.secondary * result.employment_wages_per_person.secondary
	);

	result.profit = result.income_from_sales
		- result.spending_from_primary_inputs
		- result.spending_from_efficiency_inputs
		- result.spending_from_maintenance
		- result.spending_from_wages;

	auto total_employment =
		result.employment_target.unqualified
		+ result.employment_target.primary
		+ result.employment_target.secondary;

	auto expansion_scale = base_expansion_scale / state.world.factory_type_get_construction_time(fac_type);

	if(result.profit > 0 && total_employment >= current_size * expansion_trigger) {
		// limit with 10% budget
		expansion_scale = std::min(
			expansion_scale,
			result.profit * 0.1f / construction_costs_data.total_cost
		);
		// reduce speed during shortage of direct inputs 
		expansion_scale *= std::max(0.f, primary_inputs_data.min_available - 0.5f);
	} else {
		expansion_scale = 0.f;
	}

	result.expansion_inputs = add_details_to_commodity_set(
		state, m, construction_costs, expansion_scale, true
	);
	result.spending_from_expansion =
		construction_costs_data.total_cost
		* expansion_scale
		* construction_costs_data.min_available;
	result.profit = result.profit - result.spending_from_expansion;

	result.expansion_scale = expansion_scale;
	result.expansion_size = expansion_scale * base_size * construction_costs_data.min_available;

	employment_data<2, float> basic_employment{
		{ result.employment_target.unqualified, result.employment_target.primary },
		{
			result.employment.unqualified,
			result.employment.primary
		},
		{ unqualified_throughput_multiplier, 1.f },
		{
			result.employment_wages_per_person.unqualified * (1.f + capitalists_greed) / (result.employment_available_ratio.unqualified + 0.01f),
			result.employment_wages_per_person.primary * (1.f + capitalists_greed) / (result.employment_available_ratio.primary + 0.01f)
		}
	};

	auto income_per_employment_unit =
		result.output_amount_per_employment_unit_ignore_inputs
		* result.output_price;
	auto cost_per_employment_unit =
		primary_inputs_data.total_cost
		* result.input_multipliers.total
		* result.throughput_multipliers.total;

	auto profit_per_worker = (income_per_employment_unit - cost_per_employment_unit * (1.f + capitalists_greed))
		/ fac_type.get_base_workforce();

	auto gradient = get_profit_gradient(
		profit_per_worker,
		result.employment_target.secondary,
		result.employment.secondary,
		1.f / fac.get_size() * economy::secondary_employment_output_bonus,
		result.employment_wages_per_person.secondary * (1.f + capitalists_greed) / (result.employment_available_ratio.secondary + 0.01f),
		basic_employment
	);

	result.expected_profit_gradient = {
		gradient.primary[0],
		gradient.primary[1],
		gradient.secondary
	};

	result.employment_expected_change = {
		gradient_to_employment_change(gradient.primary[0], result.employment_wages_per_person.unqualified, result.employment.unqualified, result.employment_available_ratio.unqualified * primary_inputs_data.min_available),
		gradient_to_employment_change(gradient.primary[1], result.employment_wages_per_person.primary, result.employment.primary, result.employment_available_ratio.primary * primary_inputs_data.min_available),
		gradient_to_employment_change(gradient.secondary, result.employment_wages_per_person.secondary, result.employment.secondary, result.employment_available_ratio.secondary * primary_inputs_data.min_available),
	};

	return result;
}
}

namespace gdp {

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

float value_market(sys::state& state, dcon::market_id n) {
	return state.world.market_get_gdp(n);
}

float value_nation(sys::state& state, dcon::nation_id n) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total = total + value_market(state, market);
	});
	return total;
}

float value_nation_adjusted(sys::state& state, dcon::nation_id n) {
	auto conversion = ideal_pound_to_real_pound(state);
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total = total + value_market(state, market);
	});
	return total / conversion;
}

breakdown breakdown_province(sys::state& state, dcon::province_id pid) {
	// rgo
	float rgo_gdp = 0.f;
	state.world.for_each_commodity([&](auto cid) {
		auto value_produced = economy::rgo_output(state, cid, pid) * state.world.commodity_get_median_price(cid);
		auto intermediate_consumption = economy::rgo_efficiency_spending(state, cid, pid);
		rgo_gdp += value_produced - intermediate_consumption;
	});

	// factories
	float factories_gdp = 0.f;
	state.world.province_for_each_factory_location_as_province(pid, [&](auto flid) {
		auto fid = state.world.factory_location_get_factory(flid);
		auto ftid = state.world.factory_get_building_type(fid);
		auto cid = state.world.factory_type_get_output(ftid);
		auto factory_details = economy::factory_operation::explain_everything(state, fid);

		auto value_produced = factory_details.output_actual_amount * state.world.commodity_get_median_price(cid);

		float intermediate_consumption = 0.f;

		for(uint32_t i = 0; i < factory_details.primary_inputs.set_size; i++) {
			auto cid_in = factory_details.primary_inputs.commodity_type[i];
			if(!cid_in) break;

			intermediate_consumption +=
				factory_details.primary_inputs.commodity_actual_amount[i]
				* state.world.commodity_get_median_price(cid_in);
		}

		if(factory_details.efficiency_inputs_worth_it) {
			for(uint32_t i = 0; i < factory_details.primary_inputs.set_size; i++) {
				auto cid_in = factory_details.primary_inputs.commodity_type[i];
				if(!cid_in) break;

				intermediate_consumption +=
					factory_details.primary_inputs.commodity_actual_amount[i]
					* state.world.commodity_get_median_price(cid_in);
			}
		}

		factories_gdp += value_produced - intermediate_consumption;
	});

	// artisans
	float artisans_gdp = 0.f;
	state.world.for_each_commodity([&](auto cid) {
		if(economy::valid_artisan_good(state, state.world.province_get_nation_from_province_ownership(pid), cid)) {
			auto value_produced = economy::artisan_output(state, cid, pid) * state.world.commodity_get_median_price(cid);
			auto intermediate_consumption = economy::estimate_artisan_gdp_intermediate_consumption(state, pid, cid);
			artisans_gdp += value_produced - intermediate_consumption;
		}
	});

	auto local_gdp = artisans_gdp + rgo_gdp + factories_gdp;

	return {
		.primary = rgo_gdp,
		.secondary_factory = factories_gdp,
		.secondary_artisan = artisans_gdp,
		.total = local_gdp,
		.total_non_negative = std::max(0.f, artisans_gdp) + std::max(0.f, rgo_gdp) + std::max(0.f, factories_gdp)
	};
}
}

}
