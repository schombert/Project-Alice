#include "dcon_generated_ids.hpp"
#include "system_state.hpp"

#include "adaptive_ve.hpp"

#include "economy_production.hpp"
#include "economy_stats.hpp"
#include "demographics.hpp"
#include "construction.hpp"
#include "price.hpp"

#include "province_templates.hpp"
#include "advanced_province_buildings.hpp"
#include "economy_constants.hpp"
#include "money.hpp"
#include "economy.hpp"


namespace production_directives {
dcon::production_directive_id to_key(sys::state const& state, dcon::commodity_id v) {
	return dcon::production_directive_id{ dcon::production_directive_id::value_base_t( count_special_keys + v.index() ) };
}
uint32_t size(sys::state const& state) {
	return state.world.commodity_size() + count_special_keys;
}
}

namespace economy {

constexpr float base_expansion_scale = 10.f;

/*
Promising to buy and refine goods is essentially a speculation.
Factory doesn't know if it will be able to buy or sell goods
Pessimism means that factory would account for current statistic of sales.
Optimism means that factory ignores bleak reality and changes production values as if it was able to buy and sell everything.
Pessimism is great in established industries to avoid losses.
Optimism is required to break into new industries to eat up initial loss in order to start up production.
It would be great to track optimism and pessimism, but for now we would use a static mixture of optimistic and pessimistic strategies.
Factories are currently pessimistic about workers.
*/
constexpr float sales_optimism = 0.05f;
constexpr float purchase_optimism = 0.2f;

template<typename T>
auto artisan_input_multiplier(
	const sys::state& state,
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
	const sys::state& state,
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
		return std::max(
			0.01f,
			1.0f
			+ state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::artisan_throughput)
		);
	} else {
		return ve::max(
			0.01f,
			1.0f
			+ state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::artisan_throughput)
		);
	}
}
/*
template<typename T, typename S>
ve::fp_vector base_artisan_profit(
	const sys::state& state,
	T markets,
	S nations,
	dcon::commodity_id c,
	ve::fp_vector predicted_price
) {
	auto const& inputs = state.world.commodity_get_artisan_inputs(c);
	ve::fp_vector input_total = 0.0f;
	ve::fp_vector expected_min_available = 1.f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(!inputs.commodity_type[i]) break;

		input_total = input_total + inputs.commodity_amounts[i] * ve_price(state, markets, inputs.commodity_type[i]);
		expected_min_available = ve::min(expected_min_available, state.world.market_get_expected_probability_to_buy(markets, inputs.commodity_type[i]));
	}

	auto output_total = state.world.commodity_get_artisan_output_amount(c) * predicted_price;
	auto input_multiplier = artisan_input_multiplier(state, nations);
	auto output_multiplier = artisan_output_multiplier(state, nations);
	return output_total * output_multiplier - input_multiplier * input_total;
}
*/
template<typename VALUE, typename M>
VALUE base_artisan_output_cost(
	const sys::state& state,
	M market,
	dcon::commodity_id c,
	VALUE predicted_price
) {
	auto sid = state.world.market_get_zone_from_local_market(market);
	auto nid = state.world.state_instance_get_nation_from_state_ownership(sid);
	auto output_total = state.world.commodity_get_artisan_output_amount(c) * predicted_price;
	auto output_multiplier = artisan_output_multiplier(state, nid);
	return output_total * output_multiplier;
}

inline constexpr float free_efficiency = 0.3f;

template<typename NATIONS, typename PROV>
auto max_rgo_efficiency(sys::state& state, NATIONS n, PROV p, dcon::commodity_id c) {
	using VALUE = adaptive_ve::convert_to_float<NATIONS>;
	//using MASK = adaptive_ve::convert_to_bool<NATIONS>;

	auto is_mine = state.world.commodity_get_is_mine(c);

	auto prov_mod_offset = is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff;
	auto nation_mod_offset = is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff;

	VALUE throughput =
		1.0f
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_throughput)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_throughput)
		+ state.world.province_get_modifier_values(p, prov_mod_offset)
		+ state.world.nation_get_modifier_values(n, nation_mod_offset);

	VALUE result =
		(
			(state.world.province_get_demographics(p, demographics::literacy) + 1.f)
			/ (state.world.province_get_demographics(p, demographics::total) + 1.f)
			+ 0.01f
		)
		* adaptive_ve::max<VALUE>(0.5f, throughput)
		* state.defines.alice_rgo_boost
		* adaptive_ve::max<VALUE>(0.5f, (
			1.0f
			+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_output)
			+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_output)
			+ state.world.nation_get_rgo_goods_output(n, c)
		));

	if(state.world.commodity_get_money_rgo(c)) {
		return free_efficiency + result * 0.1f;
	}

	return free_efficiency + result;
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


float base_artisan_output_cost(
	const sys::state& state,
	dcon::market_id market,
	dcon::commodity_id c
) {
	auto sid = state.world.market_get_zone_from_local_market(market);
	auto nid = state.world.state_instance_get_nation_from_state_ownership(sid);
	auto output_total = state.world.commodity_get_artisan_output_amount(c) * price(state, market, c);
	auto output_multiplier = artisan_output_multiplier<dcon::nation_id>(state, nid);
	return output_total * output_multiplier;
}
float base_artisan_input_cost(
	const sys::state& state,
	dcon::market_id market,
	dcon::commodity_id c
) {
	auto sid = state.world.market_get_zone_from_local_market(market);
	auto nid = state.world.state_instance_get_nation_from_state_ownership(sid);
	auto data = get_inputs_data(state, market, state.world.commodity_get_artisan_inputs(c));
	auto input_multiplier = artisan_input_multiplier<dcon::nation_id>(state, nid);
	return input_multiplier * data.total_cost;
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
			result.efficient_ratio[j] = state.world.market_get_actual_probability_to_buy(markets, cid);
		} else {
			result.efficient_ratio[j] = state.world.market_get_actual_probability_to_sell(markets, cid);
		}
		result.commodity_price[j] = state.world.market_get_price(markets, cid);
	}

	return result;
}

template<typename M, typename SET>
ve_inputs_data get_inputs_data(sys::state const& state, M markets, SET const& inputs) {
	ve::fp_vector input_total = 0.0f;
	ve::fp_vector input_total_adjusted = 0.0f;
	ve::fp_vector min_available = 1.0f;
	ve::fp_vector min_expected = 1.0f;
	for(uint32_t j = 0; j < SET::set_size; ++j) {
		if(inputs.commodity_type[j]) {
			input_total =
				input_total
				+ inputs.commodity_amounts[j]
				* ve_price(state, markets, inputs.commodity_type[j]);
			input_total_adjusted =
				input_total_adjusted
				+ inputs.commodity_amounts[j]
				* ve_price(state, markets, inputs.commodity_type[j])
				* state.world.market_get_actual_probability_to_buy(markets, inputs.commodity_type[j]);
			min_available = ve::min(
				min_available,
				state.world.market_get_actual_probability_to_buy(markets, inputs.commodity_type[j])
			);
			min_expected = ve::min(
				min_expected,
				state.world.market_get_expected_probability_to_buy(markets, inputs.commodity_type[j])
			);
		} else {
			break;
		}
	}
	return { min_expected, min_available, input_total, input_total_adjusted };
}
template<typename SET>
inputs_data get_inputs_data(sys::state const& state, dcon::market_id markets, SET const& inputs) {
	float input_total = 0.0f;
	float input_total_adjusted = 0.0f;
	float min_available = 1.0f;
	float min_expected = 1.0f;
	for(uint32_t j = 0; j < SET::set_size; ++j) {
		if(inputs.commodity_type[j]) {
			input_total =
				input_total
				+ inputs.commodity_amounts[j]
				* price(state, markets, inputs.commodity_type[j]);
			input_total_adjusted =
				input_total_adjusted
				+ inputs.commodity_amounts[j]
				* price(state, markets, inputs.commodity_type[j])
				* state.world.market_get_actual_probability_to_buy(markets, inputs.commodity_type[j]);
			min_available = std::min(
				min_available,
				state.world.market_get_actual_probability_to_buy(markets, inputs.commodity_type[j])
			);
			min_expected = std::min(
				min_expected,
				state.world.market_get_expected_probability_to_buy(markets, inputs.commodity_type[j])
			);
		} else {
			break;
		}
	}

	assert(input_total >= 0.f);
	return { min_expected, min_available, input_total, input_total_adjusted };
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

float high_education_power(
	sys::state const& state,
	dcon::nation_id nation,
	dcon::commodity_id output
) {
	return
		std::max(0.f, 1.f + state.world.nation_get_factory_goods_output(nation, output))
		* std::max(0.f, 1.f + state.world.nation_get_modifier_values(nation, sys::national_mod_offsets::factory_output))
		* economy::secondary_employment_output_bonus;
}

float output_multiplier_from_workers_with_high_education(
	sys::state const& state,
	dcon::commodity_id output,
	dcon::province_id labor_market,
	float target_workers_high_education
) {
	assert(target_workers_high_education >= 0.f);
	float workers_high_education = target_workers_high_education * state.world.province_get_labor_demand_satisfaction(labor_market, labor::high_education);
	assert(workers_high_education >= 0.f);
	return (1.f + high_education_power(state, state.world.province_get_nation_from_province_ownership(labor_market), output) * workers_high_education);
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
	assert(std::isfinite(target_workers_no_education)&& target_workers_no_education >= 0.f);
	assert(std::isfinite(target_workers_basic_education)&& target_workers_basic_education >=0.f);
	assert(std::isfinite(target_workers_high_education)&& target_workers_high_education >= 0.f);

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
	ve::apply([&](float value) {assert(std::isfinite(value)&& value>=0.f); }, target_guild_size);
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

	float output_price = 0.f;
	float output_amount_per_production_unit = 0.f;
	float direct_inputs_cost_per_production_unit = 0.f;
	float direct_inputs_cost_per_production_unit_availability_adjusted = 0.f;
};

struct ve_preconsumption_data {
	ve_inputs_data direct_inputs_data{ };

	ve::fp_vector output_price = 0.f;
	ve::fp_vector output_amount_per_production_unit = 0.f;
	ve::fp_vector direct_inputs_cost_per_production_unit = 0.f;
	ve::fp_vector direct_inputs_cost_per_production_unit_availability_adjusted = 0.f;
};

preconsumption_data prepare_data_for_consumption(
	sys::state& state,
	dcon::market_id market,
	economy::commodity_set const& inputs,
	dcon::commodity_id output,
	float output_amount,

	float input_multiplier, float output_multiplier
) {
	auto direct_inputs_data = get_inputs_data(state, market, inputs);
	assert(direct_inputs_data.min_available >= 0.f);

	auto output_price = price(state, market, output);

	auto output_per_production_unit =
		output_amount
		* output_multiplier;

	float output_cost_per_production_unit =
		output_per_production_unit
		* output_price;

	preconsumption_data result = {
		.direct_inputs_data = direct_inputs_data,
		.output_price = output_price,
		.output_amount_per_production_unit = output_per_production_unit,
		.direct_inputs_cost_per_production_unit = direct_inputs_data.total_cost * input_multiplier,
		.direct_inputs_cost_per_production_unit_availability_adjusted = direct_inputs_data.total_cost_availability_adjusted * input_multiplier,
	};

	return result;
}

template<typename M>
ve_preconsumption_data ve_prepare_data_for_consumption(
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
	ve_preconsumption_data result = {
		.direct_inputs_data = direct_inputs_data,
		.output_price = output_price,
		.output_amount_per_production_unit = output_per_production_unit,
		.direct_inputs_cost_per_production_unit = direct_inputs_data.total_cost * input_multiplier,
		.direct_inputs_cost_per_production_unit_availability_adjusted = direct_inputs_data.total_cost_availability_adjusted * input_multiplier,
	};
	return result;
}

struct consumption_data {
	float direct_inputs_cost;
	float output;

	float direct_inputs_scale;

	float input_cost_per_employment_unit;
	float output_per_employment_unit;
};
struct ve_consumption_data {
	ve::fp_vector direct_inputs_cost;
	ve::fp_vector output;

	ve::fp_vector direct_inputs_scale;

	ve::fp_vector input_cost_per_employment_unit;
	ve::fp_vector output_per_employment_unit;
};

consumption_data imitate_consume(
	sys::state& state,
	economy::commodity_set const& inputs,
	preconsumption_data& additional_data,

	float input_multiplier, float throughput_multiplier, float output_multiplier,
	float employment_units, float output_multiplier_from_workers_with_high_education,
	float max_employment
) {
	float production_units = employment_units * throughput_multiplier;
	float input_scale =
		input_multiplier
		* production_units;

	consumption_data result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit_availability_adjusted * production_units,
		.output = additional_data.output_amount_per_production_unit * production_units * additional_data.direct_inputs_data.min_available,

		.direct_inputs_scale = input_scale,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit * throughput_multiplier,
		.output_per_employment_unit = additional_data.output_amount_per_production_unit * throughput_multiplier
	};

	return result;
}

consumption_data imitate_consume(
	sys::state& state,
	economy::commodity_set const& inputs,
	preconsumption_data& additional_data,
	float input_multiplier, float throughput_multiplier, float output_multiplier,
	float employment_units
) {
	float production_units = employment_units * throughput_multiplier;
	float input_scale =
		input_multiplier
		* production_units;

	consumption_data result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit_availability_adjusted * production_units,
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
	preconsumption_data& additional_data,

	float input_multiplier, float throughput_multiplier, float output_multiplier,
	float employment_units, float output_multiplier_from_workers_with_high_education,
	float max_employment,
	economy_reason reason
) {
	assert(input_multiplier >= 0.f);
	assert(throughput_multiplier >= 0.f);
	assert(output_multiplier >= 0.f);

	float production_units = employment_units * throughput_multiplier;
	assert(production_units >= 0.f);

	float input_scale =
		input_multiplier
		* production_units;
	assert(input_scale >= 0.f);

	save_inputs_to_buffers(state, province, buffer_demanded, buffer_consumed, inputs, input_scale, additional_data.direct_inputs_data.min_available);

	consumption_data result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit_availability_adjusted
		* production_units,

		.output =
			additional_data.output_amount_per_production_unit
			* production_units
			* additional_data.direct_inputs_data.min_available
			* output_multiplier_from_workers_with_high_education,

		.direct_inputs_scale = input_scale,

		.input_cost_per_employment_unit = additional_data.direct_inputs_cost_per_production_unit * throughput_multiplier,
		.output_per_employment_unit =
			additional_data.output_amount_per_production_unit
			* throughput_multiplier
			* output_multiplier_from_workers_with_high_education
	};

	return result;
}

template<typename PROV>
ve_consumption_data consume(
	sys::state& state,
	PROV province,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_demanded,
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>>& buffer_consumed,
	economy::commodity_set const& inputs,
	ve_preconsumption_data& additional_data,
	ve::fp_vector input_multiplier, ve::fp_vector throughput_multiplier, ve::fp_vector output_multiplier,
	ve::fp_vector employment_units,
	economy_reason reason
) {
	ve::fp_vector production_units = employment_units * throughput_multiplier;
	ve::fp_vector input_scale = input_multiplier * production_units;

	save_inputs_to_buffers(state, province, buffer_demanded, buffer_consumed, inputs, input_scale, additional_data.direct_inputs_data.min_available);

	ve_consumption_data result = {
		.direct_inputs_cost = additional_data.direct_inputs_cost_per_production_unit_availability_adjusted * production_units,
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

		ve_preconsumption_data prepared_data = ve_prepare_data_for_consumption(
			state, markets, inputs, cid, output_amount, input_multiplier, output_multiplier
		);
		ve_consumption_data consumption_data = consume(
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
			+ consumption_data.output * prepared_data.output_price * state.world.market_get_actual_probability_to_sell(markets, cid)
			- consumption_data.direct_inputs_cost;
	}
	state.world.province_set_artisan_profit(provinces, total_profit);
}


float factory_min_input_actually_available(
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
					state.world.market_get_actual_probability_to_buy(m, inputs.commodity_type[i])
				);
		} else {
			break;
		}
	}
	return min_input_available;
}

float factory_min_input_expected_to_be_available(
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
					state.world.market_get_expected_probability_to_buy(m, inputs.commodity_type[i])
				);
		} else {
			break;
		}
	}
	return min_input_available;
}

float priority_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto exp = state.world.nation_get_factory_type_experience(n, fac_type);
	return 100000.f / (100000.f + std::max(0.f, exp));
}

float nation_factory_input_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto mult = priority_multiplier(state, fac_type, n);

	return std::max(
		0.1f,
		mult * (
			state.defines.alice_inputs_base_factor
			+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input)
		)
	);
}
float nation_factory_output_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n) {
	auto output = state.world.factory_type_get_output(fac_type);
	return state.world.nation_get_factory_goods_output(n, output)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_output)
		+ 1.0f;
}

inline constexpr float input_multiplier_per_capitalist_percentage = -2.5f;
static constexpr float max_premium_size = 1'500'000.f;
static constexpr float base_urban_premium = 0.5f;

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
		result.base = state.world.factory_get_technology_scale(f);
	}

	{
		auto provincial_fac_t = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput);
		auto triggered = state.world.factory_get_triggered_modifiers(fac);
		result.from_modifiers = 1.f
			* std::max(0.f, 1.f + provincial_fac_t)
			* std::max(0.f, 1.f + triggered);
	}

	{
		auto local_urbanisation = state.world.province_get_advanced_province_building_max_private_size(p, advanced_province_buildings::list::local_cities_and_towns);
		auto local_population = state.world.province_get_demographics(p, demographics::total);
		auto urban_premium = base_urban_premium + local_urbanisation / max_premium_size;
		result.from_forced_subsistence = std::clamp(0.5f + 0.5f * local_urbanisation / (local_population + 1.f), 0.f, 1.f) * urban_premium;
	}

	{
		result.total = result.from_modifiers * result.from_scale * result.from_forced_subsistence * result.base;
	}

	return result;
}
}

float factory_throughput_multiplier(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s, float size) {
	auto fac_type = state.world.factory_get_building_type(fac);
	auto output = state.world.factory_type_get_output(fac_type);
	auto triggered = state.world.factory_get_triggered_modifiers(fac);
	auto provincial_fac_t = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput);
	
	/*
	We assume that local people who do not live in urban environment
	have to spend a lot of time on managing their household and subsist in a certain way
	(beggary, crime, farming)
	It means that local industries can't convert their labour into production units as efficiently
	*/

	auto local_urbanisation = state.world.province_get_advanced_province_building_max_private_size(p, advanced_province_buildings::list::local_cities_and_towns);
	auto local_population = state.world.province_get_demographics(p, demographics::total);
	auto urban_premium = base_urban_premium + local_urbanisation / max_premium_size;
	auto forced_subsistence = std::clamp(0.5f + 0.5f * local_urbanisation / (local_population + 1.f), 0.f, 1.f) * urban_premium;

	auto result = 1.f
		* state.world.factory_get_technology_scale(fac)
		* std::max(0.f, 1.f + provincial_fac_t)
		* (1.f + size / state.world.factory_type_get_base_workforce(fac_type) / 100.f)
		* std::max(0.f, 1.f + triggered)
		* forced_subsistence;

	return result;
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
			state, factory_type.get_output(), p,
			fac.get_secondary_employment()
		);
	}

	{
		inputs_data inputs_data = get_inputs_data<economy::commodity_set>(state, m, factory_type.get_inputs());
		result.from_inputs_lack = inputs_data.min_available;
	}

	{
		result.total =
			result.from_inputs_lack
			* result.from_modifiers
			* result.from_secondary_workers;

		result.total_ignore_inputs =
			result.from_modifiers
			* result.from_secondary_workers;
	}

	return result;
}
}


float factory_output_multiplier_no_secondary_workers(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p) {
	auto fac_type = state.world.factory_get_building_type(fac);
	return std::max(0.f, 1.f + state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_output));
}

float factory_throughput_additional_multiplier(sys::state const& state, dcon::factory_id fac, float mobilization_impact, bool occupied) {
	return (occupied ? 0.1f : 1.0f) * std::max(0.0f, mobilization_impact);
}

float get_total_wage(const sys::state& state, dcon::factory_id f) {
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
float get_total_target_wage(const sys::state& state, dcon::factory_id f) {
	auto labor_market = state.world.factory_get_province_from_factory_location(f);
	return state.world.province_get_labor_price(labor_market, labor::no_education)
		* state.world.factory_get_unqualified_employment(f)
		+
		state.world.province_get_labor_price(labor_market, labor::basic_education)
		* state.world.factory_get_primary_employment(f)
		+
		state.world.province_get_labor_price(labor_market, labor::high_education)
		* state.world.factory_get_secondary_employment(f);
}

profit_explanation explain_last_factory_profit(sys::state const& state, dcon::factory_id f) {
	auto location = state.world.factory_get_province_from_factory_location(f);
	auto nation = state.world.province_get_nation_from_province_ownership(location);
	auto zone = state.world.province_get_state_membership(location);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto ftid = state.world.factory_get_building_type(f);
	auto output_commodity = state.world.factory_type_get_output(ftid);
	auto local_price = price(state, market, output_commodity);
	auto last_output = state.world.factory_get_output(f) * local_price;

	auto priority = state.world.nation_get_production_directive(nation, production_directives::to_key(state, output_commodity));
	auto priority_local = state.world.state_instance_get_production_directive(zone, production_directives::to_key(state, output_commodity));
	auto subsidy = 0.f;

	if(priority || priority_local) {
		auto base_output = state.world.factory_type_get_output_amount(ftid);
		auto factory_output = state.world.factory_get_output(f);
		auto effective_output = factory_output / base_output;
		auto tokens = state.world.nation_get_subsidy_token_total(nation);
		auto last_token_price = state.world.nation_get_subsidy_token_price(nation);
		subsidy = last_token_price * effective_output;
	}
	auto last_inputs = state.world.factory_get_input_cost(f);
	auto wages = get_total_wage(state, f);
	auto profit = subsidy + last_output * state.world.market_get_actual_probability_to_sell(market, output_commodity) - wages - last_inputs;

	return {
		.inputs = last_inputs,
		.wages = wages,
		.output = last_output,
		.subsidy = subsidy,
		.profit = profit
	};
}

struct factory_update_data {
	preconsumption_data base;
	consumption_data consumption;
};

struct guild_update_data {
	preconsumption_data base;
	consumption_data consumption;
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

	preconsumption_data prepared_data = prepare_data_for_consumption(
		state, markets, inputs, cid, output_amount, input_multiplier, output_multiplier
	);
	consumption_data consumption_data = imitate_consume(
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
	auto max_employment = fac.get_size();

	economy::preconsumption_data base_data = prepare_data_for_consumption(
		state, m,
		direct_inputs,
		fac_type.get_output(), fac_type.get_output_amount(),
		input_multiplier, output_multiplier
	);

	auto employment = employment_units(
		state, p,
		fac.get_unqualified_employment(), fac.get_primary_employment(), fac.get_secondary_employment(),
		float(fac_type.get_base_workforce())
	);

	auto consumption_data = imitate_consume(
		state, direct_inputs, base_data,
		input_multiplier, throughput_multiplier, output_multiplier,
		employment,
		output_multiplier_from_workers_with_high_education(
			state, fac_type.get_output(), fac.get_province_from_factory_location(), fac.get_secondary_employment()
		), max_employment
	);

	return {
		.base = base_data, .consumption = consumption_data
	};
}

/*
	Investment logic is similar to subsidies
	but now investors spend money on expansion
	while profitability of factory decides amount of "investment" priority.

	Half of investment goes toward efficiency (throughput)
*/

constexpr float factory_priority_bonus = 10.f;

float factory_investment_tokens(
	const sys::state& state,
	dcon::nation_id nation,
	dcon::province_id province,
	dcon::factory_id factory
) {	
	auto area = state.world.province_get_state_membership(province);
	auto market = state.world.state_instance_get_market_from_local_market(area);

	auto factory_type = state.world.factory_get_building_type(factory);
	auto output_type = state.world.factory_type_get_output(factory_type);
	auto priority = state.world.nation_get_production_directive(nation, production_directives::to_key(state, output_type));
	auto priority_local = state.world.state_instance_get_production_directive(area, production_directives::to_key(state, output_type));
	auto subsidy = 0.f;

	auto size = state.world.factory_get_size(factory);
	if(priority || priority_local) {
		auto base_output = state.world.factory_type_get_output_amount(factory_type);
		auto factory_output = state.world.factory_get_output(factory);
		auto effective_output = factory_output / base_output;
		auto tokens = state.world.nation_get_subsidy_token_total(nation);
		auto last_token_price = state.world.nation_get_subsidy_token_price(nation);
		subsidy = last_token_price * effective_output / (size + 1.f);
	}

	auto time = state.world.factory_type_get_construction_time(factory_type);
	auto per_worker_output_cost = state.world.factory_get_output_per_worker(factory) * price(state, market, output_type);
	auto per_worker_input_cost = state.world.factory_get_input_cost_per_worker(factory);
	auto profit = per_worker_output_cost;

	return  factory_priority_bonus * (profit + subsidy);
}

float rgo_investment_tokens(
	const sys::state& state,
	dcon::nation_id nation,
	dcon::province_id province,
	dcon::commodity_id commodity
) {
	auto area = state.world.province_get_state_membership(province);
	auto market = state.world.state_instance_get_market_from_local_market(area);
	auto base_output = state.world.commodity_get_rgo_amount(commodity);
	if (base_output == 0.f) return 0.f;
	auto current_max_size = state.world.province_get_rgo_potential(province, commodity);
	if (current_max_size == 0.f) return 0.f;
	auto current_size = state.world.province_get_rgo_size(province, commodity);
	auto priority = state.world.nation_get_production_directive(nation, production_directives::to_key(state, commodity));
	auto priority_local = state.world.state_instance_get_production_directive(area, production_directives::to_key(state, commodity));
	auto subsidy_tokens = 0.f;
	if (priority || priority_local) {
		auto last_token_price = state.world.nation_get_subsidy_token_price(nation);
		subsidy_tokens = last_token_price / state.defines.alice_rgo_per_size_employment;
	}
	auto output_tokens = state.world.commodity_get_rgo_amount(commodity)
		* state.world.province_get_rgo_base_efficiency(province, commodity)
		* state.world.market_get_price(market, commodity)
		/ state.defines.alice_rgo_per_size_employment;
	auto base_tokens = 1.f / state.defines.alice_rgo_per_size_employment;
	auto local_tokens = subsidy_tokens + output_tokens + base_tokens;
	return local_tokens;
}

float total_nation_investments_tokens(
	sys::state& state,
	dcon::nation_id nation
) {
	auto total = 0.f;
	state.world.nation_for_each_province_ownership(nation, [&](auto province_ownership) {
		auto province = state.world.province_ownership_get_province(province_ownership);

		// FACTORIES
		for(auto f : state.world.province_get_factory_location(province)) {
			auto to_add = factory_investment_tokens(state, nation, province, f.get_factory());
			total += to_add;
		}

		// RGO
		state.world.for_each_commodity([&](auto c){
			auto to_add = rgo_investment_tokens(state, nation, province, c);
			total += to_add;
		});
	});

	return total;
}

inline float investment_expand_factory_priority(
	const sys::state& state,
	dcon::factory_id factory
) {
	auto size = state.world.factory_get_size(factory);
	return std::clamp((factory_total_desired_employment(state, factory) - 0.8f * size) * 5.f / (size + 1.f), 0.f, 1.f);
}

void update_production_investement_consumption(
	sys::state& state
) {
	auto investment_tokens = state.world.nation_make_vectorizable_float_buffer();

	province::for_each_nation_owned_province_parallel_over_nation(state, [&](dcon::nation_id nation, dcon::province_id province) {
		auto area = state.world.province_get_state_membership(province);
		auto market = state.world.state_instance_get_market_from_local_market(area);

		// FACTORIES
		for(auto f : state.world.province_get_factory_location(province)) {
			auto factory = f.get_factory();
			auto old = investment_tokens.get(nation);
			auto factory_tokens = factory_investment_tokens(state, nation, province, factory);
			investment_tokens.set(nation, old + factory_tokens);
		}

		// RGO
		state.world.for_each_commodity([&](auto c){
			auto old = investment_tokens.get(nation);
			auto rgo_tokens = rgo_investment_tokens(state, nation, province, c);
			investment_tokens.set(nation, old + rgo_tokens);
		});
	});

	province::for_each_nation_owned_province_parallel_over_nation(state, [&](dcon::nation_id nation, dcon::province_id province) {
		auto available_investment = state.world.nation_get_private_investment(nation);
		auto actually_spent = 0.f;
		auto total_tokens = investment_tokens.get(nation);
		auto area = state.world.province_get_state_membership(province);
		auto market = state.world.state_instance_get_market_from_local_market(area);
		bool investment_spent = false;

		if (total_tokens == 0.f) return;

		// FACTORY
		for(auto f : state.world.province_get_factory_location(province)) {
			auto factory = f.get_factory();
			auto factory_type = factory.get_building_type();
			auto output_type = factory_type.get_output();
			auto size = state.world.factory_get_size(factory);
			auto factory_tokens = factory_investment_tokens(state, nation, province, factory);

			//auto time = state.world.factory_type_get_construction_time(factory_type);
			auto base_size = state.world.factory_type_get_base_workforce(factory_type);
			auto per_worker_output_cost = state.world.factory_get_output_per_worker(factory) * price(state, output_type);
			auto per_worker_input_cost = state.world.factory_get_input_cost_per_worker(factory);
			auto profit = per_worker_output_cost; // - per_worker_input_cost - state.world.province_get_labor_price(province, economy::labor::basic_education);

			// SIZE

			float expand_priority = investment_expand_factory_priority(state, factory);

			if(expand_priority > 0.f) {
				auto investment = available_investment * factory_tokens / total_tokens * expand_priority;

				auto& costs = state.world.factory_type_get_construction_costs(factory_type);
				auto costs_data = get_inputs_data(state, market, costs);
				// expansion is cheaper than new construction:
				auto cost_per_unit = costs_data.total_cost / base_size / 2.f;
				auto expansion_scale = std::min(100.f + size * 0.05f, investment / cost_per_unit);

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					auto cid = costs.commodity_type[i];
					if (!cid) break;
					auto amount = costs.commodity_amounts[i] / base_size / 2.f;
					economy::register_demand(state, market, costs.commodity_type[i], expansion_scale * amount);
					actually_spent = actually_spent + expansion_scale * amount * price(state, market, cid) * state.world.market_get_actual_probability_to_buy(market, cid);
				}
				auto actual_expansion = expansion_scale * costs_data.min_available;
				state.world.factory_set_size(factory, std::max(1.f, size * 0.99999f - 0.01f) + actual_expansion);
			} else {
				state.world.factory_set_size(factory, std::max(1.f, size * 0.99999f - 0.01f));
			}

			/*
			EFFICIENCY
			Investment logic into efficiency is pretty simple:
			Invest proportionally to the current level of efficiency investments into every category.
			- Base efficiency additive growth is 1.
			- Base efficiency additive decay is 1.
			- Base efficiency multiplicative decay is whatever number you see in the code below. 
			Satisfaction of every category multiplies growth by (1 + alpha * effective scale)
			Costs are scaled with current level of investment and factory size.
			Technological advances increase potential level of efficiency investments.
			Final efficiency would influence factory throughput.
			*/

			auto base_growth = 1.f;
			auto decay_mult = 0.0001f;

			auto national_t = state.world.nation_get_factory_goods_throughput(nation, output_type);
			auto nationnal_fac_t = state.world.nation_get_modifier_values(nation, sys::national_mod_offsets::factory_throughput);
			auto investment_efficiency = 1.f
				* std::max(0.f, 1.f + national_t)
				* std::max(0.f, 1.f + nationnal_fac_t);

			if (expand_priority < 1.f) {
				auto investment = available_investment * factory_tokens / total_tokens * (1.f - expand_priority);
				auto& efficiency_inputs = state.world.factory_type_get_efficiency_inputs(factory_type);
				auto total_can_afford = 0.f;
				for(uint8_t i = 0; i < economy::small_commodity_set::set_size; i++) {
					auto cid = efficiency_inputs.commodity_type[i];
					if (!cid) break;

					auto current_scale = (1.f + size) / state.world.factory_type_get_base_workforce(factory_type);

					auto amount = 0.01f * efficiency_inputs.commodity_amounts[i] * current_scale;
					auto cost = amount * price(state, market, cid);
					auto can_afford = std::max(0.f, investment / cost - 1.f);

					total_can_afford = total_can_afford + can_afford;
				}

				if(total_can_afford > 0.f) {
					for(uint8_t i = 0; i < economy::small_commodity_set::set_size; i++) {
						auto cid = efficiency_inputs.commodity_type[i];
						if (!cid) break;

						auto current_scale = (1.f + size) / state.world.factory_type_get_base_workforce(factory_type);

						auto amount = 0.01f * efficiency_inputs.commodity_amounts[i] * current_scale;
						auto cost = amount * price(state, market, cid);
						auto can_afford = std::max(0.f, investment / cost - 1.f);
						auto investment_into_category = investment * can_afford / total_can_afford;

						auto can_actually_afford = std::min(10.f, investment_into_category / cost);
						economy::register_demand(state, market, cid, can_actually_afford * amount);
						auto probability_to_buy = state.world.market_get_actual_probability_to_buy(market, cid);
						auto growth = 0.05f * investment_efficiency * can_actually_afford * probability_to_buy;
						base_growth = base_growth * (1.f + decay_mult * growth);

						actually_spent = actually_spent + can_actually_afford * cost;
					}
				}
			}

			auto old = state.world.factory_get_technology_scale(factory);
			auto decay = std::max(0.f, (old - investment_efficiency) * decay_mult);
			state.world.factory_set_technology_scale(factory, std::max(0.05f, old - decay + base_growth - 1.f));
		}

		auto total_agriculture = 0.f;

		//RGO
		state.world.for_each_commodity([&](auto c){
			auto decay_mult = 0.0001f;
			auto base_output = state.world.commodity_get_rgo_amount(c);
			if (base_output == 0.f) return;
			auto current_max_size = state.world.province_get_rgo_potential(province, c);
			if (current_max_size == 0.f) return;
			auto current_size = state.world.province_get_rgo_size(province, c);

			auto local_tokens = rgo_investment_tokens(state, nation, province, c);
			auto local_investment = available_investment * local_tokens / total_tokens;
			auto investment_efficiency = state.world.province_get_rgo_max_efficiency(province, c);

			{
				auto current_efficiency = state.world.province_get_rgo_efficiency(province, c);

				auto current_employment = state.world.province_get_rgo_target_employment(province, c);
				auto priority_from_max_size = current_max_size < 1.f ? 0.f : std::max(0.01f, 1.f - (current_size / current_max_size));
				auto priority_from_employment = current_size < 1.f ? 1.f : std::max(0.01f, (current_employment / current_size - 0.8f) * 5.f);
				auto expansion_priority = priority_from_max_size * priority_from_employment;
				auto total_local_priority = 1.f + expansion_priority;

				auto investment_rgo_expansion = local_investment * expansion_priority / total_local_priority;
				auto investment_rgo_efficiency = local_investment * 1.f / total_local_priority;

				auto size = state.world.province_get_rgo_size(province, c);

				// EFFICIENCY
				{
					auto efficiency_growth = 1.f;
					auto& efficiency_inputs = state.world.commodity_get_rgo_efficiency_inputs(c);
					auto total_can_afford = 0.f;
					for(uint8_t i = 0; i < economy::small_commodity_set::set_size; i++) {
						auto cid = efficiency_inputs.commodity_type[i];
						if (!cid) break;
						auto current_scale = (1.f + size) / state.defines.alice_rgo_per_size_employment;
						auto amount = efficiency_inputs.commodity_amounts[i] * current_scale;
						auto cost = amount * price(state, market, cid);
						auto can_afford = std::max(0.f, investment_rgo_efficiency / cost - 1.f);
						total_can_afford = total_can_afford + can_afford;
					}
					if(total_can_afford > 0.f) {
						for(uint8_t i = 0; i < economy::small_commodity_set::set_size; i++) {
							auto cid = efficiency_inputs.commodity_type[i];
							if (!cid) break;
							auto current_scale = (1.f + size) / state.defines.alice_rgo_per_size_employment;
							auto amount = efficiency_inputs.commodity_amounts[i] * current_scale;
							auto cost = amount * price(state, market, cid);
							auto can_afford = std::max(0.f, investment_rgo_efficiency / cost - 1.f);
							auto investment_into_category = investment_rgo_efficiency * can_afford / total_can_afford;
							auto can_actually_afford = std::min(10.f, investment_into_category / cost);
							economy::register_demand(state, market, cid, can_actually_afford * amount);
							auto probability_to_buy = state.world.market_get_actual_probability_to_buy(market, cid);
							auto growth = 0.05f * investment_efficiency * can_actually_afford * probability_to_buy;
							efficiency_growth = efficiency_growth * (1.f + decay_mult * growth);
							actually_spent = actually_spent + can_actually_afford * cost;
						}
					}

					auto decay = std::max(0.f, (current_efficiency - investment_efficiency) * decay_mult);
					state.world.province_set_rgo_efficiency(province, c, std::max(free_efficiency, current_efficiency - decay + efficiency_growth - 1.f));
				}

				// SIZE

				/*
				For now expansion of rgo requires only skilled labor.
				Later it would be nice to have actual expansion costs.
				Larger rgos require more labor to expand
				*/

				float labor_to_expand_rgo = 20.f * (1.f + size / 100'000.f);

				auto expansion_cost = labor_to_expand_rgo * state.world.province_get_labor_price(province, economy::labor::basic_education);
				auto available_labor = state.world.province_get_labor_demand_satisfaction(province, economy::labor::basic_education);
				auto can_expand = std::max(0.f, std::min(500.f, investment_rgo_expansion / expansion_cost - 1.f));
				auto new_size = std::min(current_max_size, size * 0.9999f + can_expand * available_labor);
				state.world.province_set_rgo_size(province, c, new_size);

				auto labor_demand = state.world.province_get_labor_demand(province, economy::labor::basic_education);

#ifndef NDEBUG
				ve::apply([&](float value) {assert(std::isfinite(value) && value >= 0.f); }, labor_demand + labor_to_expand_rgo * can_expand);
#endif // !NDEBUG

				state.world.province_set_labor_demand(province, economy::labor::basic_education, labor_demand + labor_to_expand_rgo * can_expand);
				actually_spent = actually_spent + expansion_cost * can_expand * available_labor;
			}
			/*
			{
				auto size = state.world.province_get_rgo_size(province, c);
				auto current_efficiency = state.world.province_get_rgo_efficiency(province, c);
				auto new_efficiency = current_efficiency * 0.9999f;
				auto validated_efficiency = ve::select(size == 0.f, 0.f, ve::max(free_efficiency, new_efficiency));
				state.world.province_set_rgo_efficiency(province, c, validated_efficiency);

				auto new_size = std::min(current_max_size, size * 0.9999f);
				state.world.province_set_rgo_size(province, c, new_size);
			}
			*/
			if(!state.world.commodity_get_is_mine(c)) {
				total_agriculture += state.world.province_get_rgo_size(province, c);
			}
		});

		//respect max rgo size
		auto max_agriculture_size = state.world.province_get_rgo_base_size(province);

		if(total_agriculture > max_agriculture_size) {
			state.world.for_each_commodity([&](auto c) {
				if(state.world.commodity_get_is_mine(c)) return; 
				auto size = state.world.province_get_rgo_size(province, c);
				state.world.province_set_rgo_size(province, c, size * max_agriculture_size / total_agriculture);
			});
		}

		// guard against fp errors
		state.world.nation_set_private_investment(nation, std::max(0.f, state.world.nation_get_private_investment(nation) - actually_spent));
	});
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

	auto base_data = prepare_data_for_consumption(
		state, m,
		direct_inputs,
		fac_type.get_output(), fac_type.get_output_amount(),
		input_multiplier, output_multiplier
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
		direct_inputs,
		base_data,
		input_multiplier, throughput_multiplier, output_multiplier,
		employment_units,
		output_multiplier_from_workers_with_high_education(
			state, fac_type.get_output(), fac.get_province_from_factory_location(), fac.get_secondary_employment()
		), max_employment, economy_reason::factory
	);

	auto current_size = state.world.factory_get_size(f);
	auto ftid = state.world.factory_get_building_type(f);
	auto base_size = state.world.factory_type_get_base_workforce(ftid);
	float actual_wages = get_total_wage(state, f);
	float actual_profit =
		data.output
		* base_data.output_price
		* state.world.market_get_actual_probability_to_sell(m, fac_type.get_output())
		- data.direct_inputs_cost
		- actual_wages;

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
	fac.set_input_cost(data.direct_inputs_cost);
}

void set_initial_factory_values(sys::state& state, dcon::factory_id f) {
	auto ftid = state.world.factory_get_building_type(f);
	auto output = state.world.factory_type_get_output_amount(ftid);
	auto base_workforce = state.world.factory_type_get_base_workforce(ftid);
	state.world.factory_set_output_per_worker(f, output / base_workforce);
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
		
	// update efficiency and consume efficiency_inputs:
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto size = state.world.province_get_rgo_size(p, c);
		if(size.reduce() <= 0.f) {
			return;
		}

		// recalculate max efficiency here (doesn't really matter where, just do it once)
		auto max_efficiency = max_rgo_efficiency(state, n, p, c);
		state.world.province_set_rgo_max_efficiency(p, c, max_efficiency);

		// estimate amount of workers using previous day data
		auto estimated_amount_of_workers = state.world.province_get_rgo_target_employment(p, c)
			* state.world.province_get_labor_demand_satisfaction(p, labor::no_education)
			* mobilization_impact;

		// DEMAND ON LABOR

		auto target = state.world.province_get_rgo_target_employment(p, c);
		auto cur_labor_demand = state.world.province_get_labor_demand(p, labor::no_education);

#ifndef NDEBUG
		ve::apply([&](float value) {assert(std::isfinite(value) && value >= 0.f); }, cur_labor_demand + target);
#endif // !NDEBUG

		state.world.province_set_labor_demand(p, labor::no_education, cur_labor_demand + target);

		// SAVE ADDITIONAL DATA

		auto per_worker = state.world.commodity_get_rgo_amount(c)
			* state.world.province_get_rgo_efficiency(p, c)
			* state.world.province_get_rgo_base_efficiency(p, c)
			/ state.defines.alice_rgo_per_size_employment;
		state.world.province_set_rgo_output_per_worker(p, c, per_worker);
	});
}

void update_rgo_production(sys::state& state) {

	// calculate production

	province::ve_parallel_for_each_land_province(state, [&](auto p) {
		auto sid = state.world.province_get_state_membership(p);
		auto m = state.world.state_instance_get_market_from_local_market(sid);
		auto nations = state.world.province_get_nation_from_province_ownership(p);
		auto mobilization_impact =
			ve::select(
				state.world.nation_get_is_mobilized(nations),
				military::ve_mobilization_impact(state, nations), 1.f
			);

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto size = state.world.province_get_rgo_size(p, c);
			if(size.reduce() <= 0.f) {
				return;
			}

			// INPUT LABOUR -> COMMODITY			
			auto actual_amount_of_workers =
				state.world.province_get_rgo_target_employment(p, c)
				* state.world.province_get_labor_demand_satisfaction(p, labor::no_education)
				* mobilization_impact;
			auto per_worker = state.world.province_get_rgo_output_per_worker(p, c);
			auto amount = actual_amount_of_workers * per_worker;
			state.world.province_set_rgo_output(p, c, amount);
		});
	});

	// registed calculated production
	// can't do in parallel over provinces
	// therefore do it in parallel over markets. Cannot do it over commodities because register_domestic_supply writes to market_gdp

	concurrency::parallel_for(uint32_t(0), state.world.market_size(), [&](uint32_t k) {
		dcon::market_id local_market{ dcon::market_id::value_base_t(k) };
		if(!state.world.market_is_valid(local_market)) {
			return;
		}
		state.world.for_each_commodity([&](dcon::commodity_id commodity) {
			auto rgo_output = state.world.commodity_get_rgo_amount(commodity);
			if(rgo_output <= 0.f) {
				return;
			}
			if(state.world.commodity_get_money_rgo(commodity)) {
				return;
			}
			auto state_instance = state.world.market_get_zone_from_local_market(local_market);
			assert(state_instance);
			province::for_each_province_in_state_instance(state, state_instance, [&](dcon::province_id province) {
				auto amount = state.world.province_get_rgo_output(province, commodity);
				register_domestic_supply(state, local_market, commodity, amount, economy_reason::rgo);
			});
		});
	});
}

void update_rgo_profit(sys::state& state) {
	// reset profit
	state.world.execute_serial_over_province([&](auto p) {
		state.world.province_set_rgo_profit(p, 0.f);
	});

	// national gold profit is moved to nations:
	// there is no opportunity to do things in parallel
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		if(state.world.commodity_get_rgo_amount(c) < 0.f) {
			return;
		}
		if(!state.world.commodity_get_money_rgo(c)) {
			return;
		}
		state.world.for_each_province([&](auto province) {
			auto local_state = state.world.province_get_state_membership(province);
			auto local_market = state.world.state_instance_get_market_from_local_market(local_state);
			auto controller = state.world.province_get_nation_from_province_control(province);
			// if the rgo is a money RGO and it is not rebel controlled, give the controller the cash from it
			if(!controller) {
				return;
			}
			auto produced = state.world.province_get_rgo_output(province, c);
			auto move_to_nation = produced
				* state.defines.gold_to_cash_rate
				* state.world.commodity_get_cost(c);
			assert(std::isfinite(move_to_nation) && produced >= 0.0f);
			// #CAUTION# changes nation values!
			auto& cur_money = state.world.nation_get_stockpiles(controller, economy::money);
			state.world.nation_set_stockpiles(controller, economy::money, cur_money + move_to_nation);
		});
	});

	// otherwise profit is parallel over provinces
	province::ve_parallel_for_each_land_province(state, [&](auto p) {
		auto sid = state.world.province_get_state_membership(p);
		auto m = state.world.state_instance_get_market_from_local_market(sid);
		auto nations = state.world.province_get_nation_from_province_ownership(p);
		auto mobilization_impact =
			ve::select(
				state.world.nation_get_is_mobilized(nations),
				military::ve_mobilization_impact(state, nations), 1.f
			);

		state.world.province_set_rgo_profit(p, 0.f);
		// sell goods and pay wages
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto size = state.world.province_get_rgo_size(p, c);
			if(size.reduce() <= 0.f) {
				return;
			}
			auto actual_amount_of_workers =
				state.world.province_get_rgo_target_employment(p, c)
				* state.world.province_get_labor_demand_satisfaction(p, labor::no_education)
				* mobilization_impact;
			auto per_worker = state.world.province_get_rgo_output_per_worker(p, c);
			auto amount = actual_amount_of_workers * per_worker;
			auto profit =
				ve::select(
					state.world.commodity_get_money_rgo(c),
					amount * state.world.market_get_price(m, c),
					amount * state.world.market_get_price(m, c) * state.world.market_get_actual_probability_to_sell(m, c)
				);
			auto wages =
				actual_amount_of_workers
				* state.world.province_get_labor_price(p, labor::no_education);			

			auto recorded_profit = state.world.province_get_rgo_profit(p);
			state.world.province_set_rgo_profit(p, recorded_profit + (profit - wages));
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

/*
Steal logic from trade routes.
*/
template<typename VALUE>
VALUE gradient_employment_i(
	VALUE expected_profit_per_perfect_worker,
	VALUE expected_input_cost_per_perfect_worker,
	VALUE power,
	VALUE wage,
	VALUE secondary_employment,
	VALUE secondary_power
) {
	auto earn = expected_profit_per_perfect_worker * power * (1.f + secondary_employment * secondary_power);
	auto spend = wage + expected_input_cost_per_perfect_worker  * power;
	auto diff = 2.f * (earn - spend) / (earn + economy::price_properties::labor::min);
	auto clamped = adaptive_ve::min<VALUE>(100.f, adaptive_ve::max<VALUE>(-100.f, diff));
	return clamped;
}

template float gradient_employment_i<float>(
	float expected_profit_per_perfect_worker,
	float expected_input_cost_per_perfect_worker,
	float power,
	float wage,
	float secondary_employment,
	float secondary_power
);

template ve::fp_vector gradient_employment_i<ve::fp_vector>(
	ve::fp_vector expected_profit_per_perfect_worker,
	ve::fp_vector expected_input_cost_per_perfect_worker,
	ve::fp_vector power,
	ve::fp_vector wage,
	ve::fp_vector secondary_employment,
	ve::fp_vector secondary_power
);

template<typename VALUE>
VALUE gradient_employment_i(
	VALUE expected_profit_per_perfect_worker,
	VALUE expected_input_cost_per_perfect_worker,
	VALUE power,
	VALUE wage
) {
	auto earn = expected_profit_per_perfect_worker * power;
	auto spend = wage + expected_input_cost_per_perfect_worker * power;
	auto diff = 2.f * (earn - spend) / (earn + economy::price_properties::labor::min);
	auto clamped = adaptive_ve::min<VALUE>(100.f, adaptive_ve::max<VALUE>(-100.f, diff));
	return clamped;
}
template float gradient_employment_i<float>(
	float expected_profit_per_perfect_worker,
	float expected_input_cost_per_perfect_worker,
	float power,
	float wage
);

template ve::fp_vector gradient_employment_i<ve::fp_vector>(
	ve::fp_vector expected_profit_per_perfect_worker,
	ve::fp_vector expected_input_cost_per_perfect_worker,
	ve::fp_vector power,
	ve::fp_vector wage
);

template<size_t N, typename VALUE>
VALUE gradient_employment_secondary(
	VALUE expected_profit_per_perfect_worker,
	VALUE current_secondary_workers_multiplier,
	VALUE secondary_power,
	VALUE secondary_wage,
	employment_data<N, VALUE>& primary_employment
) {
	VALUE earn = 0.f;
	for(size_t i = 0; i < N; i++) {
		earn = earn
			+ primary_employment.actual[i]
			* primary_employment.power[i]
			* expected_profit_per_perfect_worker
			/ current_secondary_workers_multiplier
			* secondary_power;
	}
	auto spend = secondary_wage;
	auto diff = 2.f * (earn - spend) / (earn + economy::price_properties::labor::min);
	auto clamped = adaptive_ve::min<VALUE>(100.f, adaptive_ve::max<VALUE>(-100.f, diff));
	return clamped;
}

template<size_t N, typename VALUE>
employment_vector<N, VALUE> get_profit_gradient(
	// already accounts for secondary workers
	VALUE expected_profit_per_perfect_primary_worker,
	VALUE expected_input_cost_per_perfect_worker,
	VALUE secondary_target,
	VALUE secondary_actual,
	VALUE current_secondary_workers_multiplier,
	VALUE secondary_power,
	VALUE secondary_wage,
	employment_data<N, VALUE>& primary_employment
) {
	employment_vector<N, VALUE> result{ };
	for(size_t i = 0; i < N; i++) {
		result.primary[i] = gradient_employment_i(
			expected_profit_per_perfect_primary_worker,
			expected_input_cost_per_perfect_worker,
			primary_employment.power[i],
			primary_employment.wage[i]//,
			//secondary_actual,
			//secondary_power
		);
	}
	result.secondary = gradient_employment_secondary(
		expected_profit_per_perfect_primary_worker,
		current_secondary_workers_multiplier,
		secondary_power,
		secondary_wage,
		primary_employment
	);
	return result;
}

constexpr inline float employment_strategy_caution = 0.75f;
static_assert(employment_strategy_caution >= 0.f);
static_assert(employment_strategy_caution < 1.f);
constexpr inline float employment_strategy_caution_multiplier = 1.f / (1.f - employment_strategy_caution);

template<typename VALUE>
VALUE gradient_to_employment_change(VALUE gradient, VALUE wage, VALUE current_employment, VALUE sat) {
	if constexpr(std::same_as<VALUE, float>) {
		auto mult = 
			gradient > 0.f
			? std::max(0.f, (sat - employment_strategy_caution) * employment_strategy_caution_multiplier)
			: 1.f;
		return (current_employment * 0.001f + 1.f) * gradient * mult;
	} else {
		auto mult = ve::select(
			gradient > 0.f,
			ve::max(0.f, (sat - employment_strategy_caution) * employment_strategy_caution_multiplier),
			1.f
		);
		return (current_employment * 0.001f + 1.f) * gradient * mult;
	}
}
template float gradient_to_employment_change<float>(float gradient, float wage, float current_employment, float sat);
template ve::fp_vector gradient_to_employment_change<ve::fp_vector>(ve::fp_vector gradient, ve::fp_vector wage, ve::fp_vector current_employment, ve::fp_vector sat);

void update_employment(sys::state& state, bool ignore_reality, float presim_employment_mult) {
	// note: markets are independent, so nations are independent:
	// so we can execute in parallel over nations but not over provinces

	auto workers_optimism = 0.1f;

	concurrency::parallel_for(uint32_t(0), state.world.commodity_size(), [&](uint32_t k) {
		dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };
		auto base_output = state.world.commodity_get_rgo_amount(c);
		if(base_output <= 0.f) {
			return;
		}

		auto& inputs = state.world.commodity_get_rgo_efficiency_inputs(c);

		province::ve_for_each_land_province(state, [&](auto pids) {
			auto state_instance = state.world.province_get_state_membership(pids);
			auto m = state.world.state_instance_get_market_from_local_market(state_instance);
			auto n = state.world.province_get_nation_from_province_ownership(pids);

			auto priority = state.world.nation_get_production_directive(n, production_directives::to_key(state, c));
			auto priority_local = state.world.state_instance_get_production_directive(state_instance, production_directives::to_key(state, c));
			auto subsidy = ve::select(priority_local || priority, state.world.nation_get_subsidy_token_price(n), 0.f) / base_output;

			auto wage_per_worker = state.world.province_get_labor_price(pids, labor::no_education);

			auto workers_availability = state.world.province_get_labor_demand_satisfaction(pids, labor::no_education);
			if(ignore_reality) {
				workers_availability = 1.f;
			}

			auto current_size = state.world.province_get_rgo_size(pids, c);
			auto efficiency = state.world.province_get_rgo_efficiency(pids, c)
				* state.world.province_get_rgo_base_efficiency(pids, c);
			auto current_employment_target = state.world.province_get_rgo_target_employment(pids, c);
			auto current_employment = current_employment_target * workers_availability;
			auto output_per_worker = base_output * efficiency / state.defines.alice_rgo_per_size_employment;
			
			auto supply = state.world.market_get_aggregated_supply_history(m, c);
			auto demand = state.world.market_get_aggregated_demand_history(m, c);
			auto current_price = ve_price(state, m, c) + subsidy;
			auto price_speed_change =
				state.world.commodity_get_money_rgo(c)
				? 0.f
				: price_properties::commodity::change<ve::fp_vector>(current_price, supply, demand);
			auto predicted_price = current_price + price_speed_change * 0.5f;

			auto sales_expected_rate = state.world.market_get_expected_probability_to_sell(m, c);
			if(ignore_reality) {
				sales_expected_rate = 1.f;
			}
			auto spending_per_worker_perception = wage_per_worker * (1.f + aristocrats_greed);
			auto gradient = gradient_employment_i<ve::fp_vector>(
				(workers_optimism + (1.f - workers_optimism) * workers_availability) * output_per_worker * predicted_price * (sales_optimism + (1.f - sales_optimism) * sales_expected_rate),
				0.f,
				1.f,
				spending_per_worker_perception
			);

			auto employment_change =gradient_to_employment_change(presim_employment_mult * gradient, spending_per_worker_perception, current_employment_target, workers_availability);

			auto new_employment = ve::max((current_employment_target + employment_change), 0.0f);
			
			// we don't want wages to rise way too high relatively to profits
			// as we do not have actual budgets, we  consider that our workers budget is as follows
			new_employment = ve::min(
				rgo_profit_to_wage_bound * output_per_worker * predicted_price * current_size // budget
				/ wage_per_worker,
				new_employment
			);
			new_employment = ve::max(ve::fp_vector{0.f}, ve::min(new_employment, current_size));
			state.world.province_set_rgo_target_employment(pids, c, new_employment);
			state.world.province_set_rgo_output(pids, c, output_per_worker * current_employment);
		});
	});

	state.world.execute_serial_over_factory([&](auto facids) {
		auto pid = state.world.factory_get_province_from_factory_location(facids);
		auto nation = state.world.province_get_nation_from_province_ownership(pid);
		auto sid = state.world.province_get_state_membership(pid);
		auto mid = state.world.state_instance_get_market_from_local_market(sid);
		auto factory_type = state.world.factory_get_building_type(facids);
		auto base_size = state.world.factory_type_get_base_workforce(factory_type);


		auto min_expected_input = ve::apply([&](auto ftid, auto factory_market) {
			auto inputs = state.world.factory_type_get_inputs(ftid);
			auto inputs_data = get_inputs_data(state, factory_market, inputs);
			return inputs_data.min_expected;
		}, factory_type, mid);

		auto output = state.world.factory_type_get_output(factory_type);
		auto base_output = state.world.factory_type_get_output_amount(factory_type);


		auto price_output = ve::apply([&](dcon::nation_id n, dcon::state_instance_id state_instance, dcon::market_id market, dcon::commodity_id cid, float base) {
			auto priority = state.world.nation_get_production_directive(n, production_directives::to_key(state, cid));
			auto priority_local = state.world.state_instance_get_production_directive(state_instance, production_directives::to_key(state, cid));
			auto subsidy = (priority_local || priority ? state.world.nation_get_subsidy_token_price(n) / base : 0.f) ;
			return state.world.market_get_price(market, cid) + subsidy;
		}, state.world.province_get_nation_from_province_ownership(pid), sid, mid, output, base_output);

		auto supply = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_aggregated_supply_history(market, cid);
		}, mid, output);

		auto demand = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_aggregated_demand_history(market, cid);
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

		auto probability_to_hire_uneducated_worker = state.world.province_get_labor_demand_satisfaction(pid, labor::no_education);

		employment_data<2, decltype(wage_no_education)> primary_employment{
			{ unqualified, primary },
			{
				unqualified * probability_to_hire_uneducated_worker,
				primary * state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education)
			},
			{ unqualified_throughput_multiplier, 1.f },
			{
				2.f * wage_no_education * (1.f + capitalists_greed) / (0.01f + state.world.province_get_labor_demand_satisfaction(pid, labor::no_education)),
				2.f * wage_basic_education * (1.f + capitalists_greed) / (0.01f + state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education))
			}
		};

		auto price_speed = price_properties::commodity::change<ve::fp_vector>(price_output, supply, demand);

		auto price_prediction = (price_output + price_speed);
		auto size = state.world.factory_get_size(facids);

		auto sold_expectation = ve::apply([&](dcon::market_id market, dcon::commodity_id cid) {
			return state.world.market_get_expected_probability_to_sell(market, cid);
		}, mid, output);

		auto profit_per_worker =
			output_per_worker
			* price_prediction
			* (sales_optimism + (1.f - sales_optimism) * sold_expectation)
			* (purchase_optimism + (1.f - purchase_optimism) * min_expected_input);

		auto input_cost_per_worker = state.world.factory_get_input_cost_per_worker(facids) * (1.f + capitalists_greed);

		auto secondary_power = ve::apply([&](dcon::nation_id local_nation, dcon::commodity_id output_commodity) {
			return high_education_power(state, local_nation, output_commodity);
		}, nation, output);

		auto current_secondary_multiplier = 1.f + secondary_power * secondary * state.world.province_get_labor_demand_satisfaction(pid, labor::high_education);

		auto gradient = get_profit_gradient(
			profit_per_worker,
			input_cost_per_worker,
			secondary,
			secondary * state.world.province_get_labor_demand_satisfaction(pid, labor::high_education),
			current_secondary_multiplier,
			secondary_power,
			wage_high_education * (1.f + capitalists_greed) / (0.01f + state.world.province_get_labor_demand_satisfaction(pid, labor::high_education)),
			primary_employment
		);

		auto unqualified_now = unqualified * state.world.province_get_labor_demand_satisfaction(pid, labor::no_education);
		auto unqualified_next = unqualified
			+ gradient_to_employment_change(gradient.primary[0] * presim_employment_mult, wage_no_education, unqualified, state.world.province_get_labor_demand_satisfaction(pid, labor::no_education) * sold_expectation);

		auto primary_now = primary * state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education);
		auto primary_next = primary
			+ gradient_to_employment_change(gradient.primary[1] * presim_employment_mult, wage_basic_education, primary, state.world.province_get_labor_demand_satisfaction(pid, labor::basic_education) * sold_expectation);

		auto secondary_now = secondary * state.world.province_get_labor_demand_satisfaction(pid, labor::high_education);
		auto secondary_next = secondary
			+ gradient_to_employment_change(gradient.secondary * presim_employment_mult, wage_high_education, secondary, state.world.province_get_labor_demand_satisfaction(pid, labor::high_education) * sold_expectation);

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
			facids, unqualified_next * scaler
		);
		ve::apply([&](auto factory, auto value) { if(state.world.factory_is_valid(factory)) assert(std::isfinite(value) && (value >= 0.f)); },
			facids, primary_next * scaler
		);
		ve::apply([&](auto factory, auto value) { if(state.world.factory_is_valid(factory)) assert(std::isfinite(value) && (value >= 0.f)); },
			facids, secondary_next * scaler
		);
#endif // !NDEBUG

		state.world.factory_set_unqualified_employment(facids, unqualified_next * scaler);
		state.world.factory_set_primary_employment(facids, primary_next * scaler);
		state.world.factory_set_secondary_employment(facids, secondary_next * scaler);
	});

	auto const csize = state.world.commodity_size();

	// artisans do not have natural max production size, so we use total population as a limit
	// they also don't have secondary workers

	auto artisans = state.culture_definitions.artisans;
	auto keys = demographics::to_key(state, artisans);

	state.world.execute_parallel_over_commodity([&](auto cids) {
		ve::apply([&](dcon::commodity_id cid) {
			auto base_output = state.world.commodity_get_artisan_output_amount(cid);
			if(base_output == 0.f)
				return;

			province::ve_for_each_land_province(state, [&](auto ids) {
				auto local_states = state.world.province_get_state_membership(ids);
				auto nations = state.world.province_get_nation_from_province_ownership(ids);
				auto markets = state.world.state_instance_get_market_from_local_market(local_states);

				//safeguard against runaway artisans target employment
				auto local_population = state.world.province_get_demographics(ids, keys) * 2.f;

				//auto min_wage = ve_artisan_min_wage(state, markets) / state.defines.alice_needs_scaling_factor;
				//auto actual_wage = state.world.province_get_labor_price(ids, labor::guild_education);
				auto mask = ve_valid_artisan_good(state, nations, cid);


				auto priority = state.world.nation_get_production_directive(nations, production_directives::to_key(state, cid));
				auto priority_local = state.world.state_instance_get_production_directive(local_states, production_directives::to_key(state, cid));
				auto subsidy = ve::select(priority_local || priority, state.world.nation_get_subsidy_token_price(nations), 0.f) / base_output;

				auto price_today = ve_price(state, markets, cid) + subsidy;
				auto supply = state.world.market_get_aggregated_supply_history(markets, cid);
				auto demand = state.world.market_get_aggregated_demand_history(markets, cid);
				auto predicted_price = price_today + price_properties::commodity::change<ve::fp_vector>(price_today, supply, demand) * 2.f;

				auto inputs_data = get_inputs_data(state, markets, state.world.commodity_get_artisan_inputs(cid));
				auto expected_sales = state.world.market_get_expected_probability_to_sell(markets, cid);

				auto sales_expectation_perception = (sales_optimism * 0.5f + (1.f - sales_optimism * 0.5f) * expected_sales);
				auto purchase_expectation_perception = (purchase_optimism * 0.5f + (1.f - purchase_optimism * 0.5f) * inputs_data.min_expected);

				auto output_cost = base_artisan_output_cost(state, markets, cid, predicted_price / (1.f + artisans_greed)) 
					* sales_expectation_perception
					* purchase_expectation_perception;

				auto input_cost = inputs_data.total_cost * artisan_input_multiplier(state, nations);

				auto base_output_cost_per_worker = output_cost / artisans_per_employment_unit;
				auto base_input_cost_per_worker = input_cost / artisans_per_employment_unit;

				auto current_employment_target = state.world.province_get_artisan_score(ids, cid);
				auto current_employment_actual = current_employment_target
					* state.world.province_get_labor_demand_satisfaction(ids, labor::guild_education);
				auto profit_per_worker = ve::select(
					mask,
					base_output_cost_per_worker,
					0.f
				);

				auto gradient = gradient_employment_i<ve::fp_vector>(
					profit_per_worker,
					base_input_cost_per_worker,
					1.f,
					0.f
				);

				auto employment_change = gradient_to_employment_change<ve::fp_vector>(gradient, 0.f, current_employment_target, purchase_expectation_perception * sales_expectation_perception);
				auto decay = 0.9999f;
				auto new_employment = ve::select(mask, ve::max(current_employment_target * decay + presim_employment_mult * employment_change, 0.0f), 0.f);
				state.world.province_set_artisan_score(ids, cid, new_employment);
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
	std::vector<ve::vectorizable_buffer<float, dcon::province_id>> buffer_consumed_estimation{};

	for(size_t i = 0; i < state.world.commodity_size(); i++) {
		buffer_demanded.push_back(state.world.province_make_vectorizable_float_buffer());
		buffer_consumed_estimation.push_back(state.world.province_make_vectorizable_float_buffer());
	}

	province::ve_parallel_for_each_land_province(state, [&](auto ids) {
		auto nations = state.world.province_get_nation_from_province_ownership(ids);
		auto mobilization_impact =
			ve::select(
				state.world.nation_get_is_mobilized(nations),
				military::ve_mobilization_impact(state, nations), 1.f
			);
		update_artisan_consumption(state, ids, buffer_demanded, buffer_consumed_estimation, mobilization_impact);
		update_rgo_consumption(state, ids, buffer_demanded, buffer_consumed_estimation, mobilization_impact);
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
					nation, buffer_demanded, buffer_consumed_estimation,
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
				auto consumed = buffer_consumed_estimation[cid.index()].get(p);
				assert(std::isfinite(demanded));
				assert(std::isfinite(consumed));
				register_intermediate_demand(state, market, cid, demanded);
				state.world.market_set_gdp(market, state.world.market_get_gdp(market) - consumed * median_price);
			});
		});
	});

	update_production_investement_consumption(state);
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

	auto priority = state.world.nation_get_production_directive(n, production_directives::to_key(state, fac_type.get_output()));
	auto priority_local = state.world.state_instance_get_production_directive(state.world.market_get_zone_from_local_market(m), production_directives::to_key(state, fac_type.get_output()));
	auto base_output = state.world.factory_type_get_output_amount(factory_type);
	auto subsidy = (priority_local || priority ? state.world.nation_get_subsidy_token_price(n) : 0.f) / base_output;

	return total_production * (price(state, m, fac_type.get_output()) + subsidy);
}

float factory_type_input_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::market_id m,
	dcon::factory_type_id factory_type
) {
	auto fac_type = dcon::fatten(state.world, factory_type);
	auto const& inputs = fac_type.get_inputs();
	auto inputs_data = get_inputs_data(state, m, inputs);
	float input_multiplier = nation_factory_input_multiplier(state, fac_type, n);
	auto result = inputs_data.total_cost * input_multiplier;
	//assert(result > 0.f);
	return result;
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
	auto wages = state.world.province_get_labor_price(pid, labor::basic_education) * state.world.factory_type_get_base_workforce(factory_type);

	return (output_cost - input_cost - wages) / (input_cost + wages);
}

float estimate_factory_payback_time(
	sys::state& state,
	dcon::province_id pid,
	dcon::factory_type_id factory_type,
	bool pop_project
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
	auto wages = state.world.province_get_labor_price(pid, labor::basic_education) * state.world.factory_type_get_base_workforce(factory_type);
	return factory_type_build_cost(state, nid, pid, factory_type, pop_project) / std::max(economy::price_properties::commodity::epsilon, (output_cost - input_cost));
}

float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::factory_id f) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();
	auto& direct_inputs = fac_type.get_inputs();
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
					* state.world.market_get_actual_probability_to_buy(markets, direct_inputs.commodity_type[i]);
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

float estimate_factory_consumption_in_production(sys::state& state, dcon::commodity_id c, dcon::state_instance_id s, dcon::commodity_id production_of) {
	auto result = 0.f;
	province::for_each_province_in_state_instance(state, s, [&](dcon::province_id p) {
		for(auto fac : state.world.province_get_factory_location(p)) {
			if(fac.get_factory().get_building_type().get_output() == production_of)
				result += estimate_factory_consumption(state, c, fac.get_factory());
		}
	});
	return result;
}
float estimate_factory_consumption_in_production(sys::state& state, dcon::commodity_id c, dcon::nation_id n, dcon::commodity_id production_of) {
	auto result = 0.f;
	for(auto p : state.world.nation_get_province_ownership(n)) {
		for(auto fac : p.get_province().get_factory_location()) {
			if(fac.get_factory().get_building_type().get_output() == production_of)
				result += estimate_factory_consumption(state, c, fac.get_factory());
		}
	}
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
					* direct_inputs.commodity_amounts[i]
					* data.base.direct_inputs_data.min_available;
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
float estimate_artisan_consumption_in_production(sys::state& state, dcon::commodity_id c, dcon::state_instance_id s, dcon::commodity_id production_of) {
	auto result = 0.0f;
	auto mob_impact = military::mobilization_impact(state, state.world.state_instance_get_nation_from_state_ownership(s));
	province::for_each_province_in_state_instance(state, s, [&](dcon::province_id p) {
		auto data = imitate_artisan_consumption(state, p, production_of, mob_impact);
		result += estimate_artisan_consumption(state, c, p, production_of);
	});
	return result;
}
float estimate_artisan_consumption_in_production(sys::state& state, dcon::commodity_id c, dcon::nation_id n, dcon::commodity_id production_of) {
	auto result = 0.0f;
	auto mob_impact = military::mobilization_impact(state, n);
	for(auto p : state.world.nation_get_province_ownership(n)) {
		auto data = imitate_artisan_consumption(state, p.get_province(), production_of, mob_impact);
		result += estimate_artisan_consumption(state, c, p.get_province(), production_of);
	}
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
		* state.world.province_get_rgo_base_efficiency(id, c)
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
		* state.world.market_get_actual_probability_to_sell(mid, c);
}

// Duplicates the calculation of RGO efficiency inputs consumption for the RGO tooltip.
commodity_set rgo_calculate_actual_efficiency_inputs(sys::state& state, dcon::nation_id n, dcon::market_id m, dcon::province_id pid, dcon::commodity_id c, float mobilization_impact) {
	auto size = state.world.province_get_rgo_size(pid, c);
	if(size <= 0.f) {
		return commodity_set{};
	}
	return state.world.commodity_get_rgo_efficiency_inputs(c);
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

float artisan_potential_output(sys::state& state, dcon::commodity_id c, dcon::province_id id) {
	// TODO
	return 0.0f;
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

float factory_potential_output(sys::state& state, dcon::commodity_id c, dcon::province_id id) {
	// TODO
	return 0.0f;
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
	return state.world.market_get_expected_probability_to_sell(market, c) < 0.8f;
}
bool commodity_shortage(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id){
	auto market = state.world.state_instance_get_market_from_local_market(id);
	return state.world.market_get_expected_probability_to_buy(market, c) < 0.8f;
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
	auto& construction_costs = state.world.factory_type_get_construction_costs(fac_type);

	inputs_data primary_inputs_data = get_inputs_data<economy::commodity_set>(state, m, primary_inputs);
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

	result.output = fac_type.get_output();
	result.output_price = price(state, m, result.output);
	result.output_base_amount = fac_type.get_output_amount();
	result.output_amount_per_production_unit = result.output_base_amount
		* result.output_multipliers.total;

	result.output_amount_per_production_unit_ignore_inputs = result.output_base_amount
		* result.output_multipliers.total_ignore_inputs;

	result.output_amount_per_employment_unit =
		result.output_amount_per_production_unit
		* result.throughput_multipliers.total;

	result.output_amount_per_employment_unit_ignore_inputs =
		result.output_amount_per_production_unit_ignore_inputs
		* result.throughput_multipliers.total;

	result.output_actually_sold_ratio =
		state.world.market_get_actual_probability_to_sell(m, result.output);
	result.output_actual_amount =
		result.output_amount_per_production_unit
		* result.production_units;

	auto current_size = state.world.factory_get_size(f);
	auto base_size = (float)state.world.factory_type_get_base_workforce(fac_type);

	result.income_from_sales =
		result.output_actual_amount
		* result.output_actually_sold_ratio
		* result.output_price;
	result.spending_from_primary_inputs =
		primary_inputs_data.total_cost_availability_adjusted
		* result.input_multipliers.total
		* result.production_units;
	result.spending_from_wages = (
		result.employment.unqualified * result.employment_wages_per_person.unqualified
		+ result.employment.primary * result.employment_wages_per_person.primary
		+ result.employment.secondary * result.employment_wages_per_person.secondary
	);

	result.profit = result.income_from_sales
		- result.spending_from_primary_inputs
		- result.spending_from_wages;

	auto total_employment =
		result.employment_target.unqualified
		+ result.employment_target.primary
		+ result.employment_target.secondary;

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

	auto sold_expectation = state.world.market_get_expected_probability_to_sell(m, result.output);

	auto priority = state.world.nation_get_production_directive(n, production_directives::to_key(state, result.output));
	auto priority_local = state.world.state_instance_get_production_directive(s, production_directives::to_key(state, result.output));
	auto subsidy = (priority_local || priority ? state.world.nation_get_subsidy_token_price(n) / result.output_base_amount : 0.f);

	auto profit_per_employment_unit =
		result.output_amount_per_employment_unit_ignore_inputs
		* (result.output_price + subsidy)
		* (sales_optimism + (1.f - sales_optimism) * sold_expectation)
		* (purchase_optimism + (1.f - purchase_optimism) * primary_inputs_data.min_expected);

	result.revenue_from_subsidies = subsidy * result.output_actual_amount;

	result.investments_tokens = factory_investment_tokens(state, n, p, f);
	result.investments_expansion_priority = investment_expand_factory_priority(state, f);
	

	auto gradient = get_profit_gradient(
		profit_per_employment_unit / base_size,
		cost_per_employment_unit * (1.f + capitalists_greed) / base_size,
		result.employment_target.secondary,
		result.employment.secondary,
		result.output_multipliers.from_secondary_workers,
		high_education_power(state, n, result.output),
		result.employment_wages_per_person.secondary * (1.f + capitalists_greed) / (result.employment_available_ratio.secondary + 0.01f),
		basic_employment
	);

	result.expected_profit_gradient = {
		gradient.primary[0],
		gradient.primary[1],
		gradient.secondary
	};

	result.employment_expected_change = {
		gradient_to_employment_change(gradient.primary[0], result.employment_wages_per_person.unqualified, result.employment.unqualified, result.employment_available_ratio.unqualified * sold_expectation),
		gradient_to_employment_change(gradient.primary[1], result.employment_wages_per_person.primary, result.employment.primary, result.employment_available_ratio.primary * sold_expectation),
		gradient_to_employment_change(gradient.secondary, result.employment_wages_per_person.secondary, result.employment.secondary, result.employment_available_ratio.secondary * sold_expectation),
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
		rgo_gdp += value_produced;
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
