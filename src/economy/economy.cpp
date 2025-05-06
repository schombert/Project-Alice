#include "economy.hpp"
#include "economy_templates.hpp"
#include "economy_government.hpp"
#include "economy_stats.hpp"
#include "economy_production.hpp"
#include "construction.hpp"
#include "demographics.hpp"
#include "dcon_generated.hpp"
#include "ai.hpp"
#include "system_state.hpp"
#include "prng.hpp"
#include "math_fns.hpp"
#include "nations_templates.hpp"
#include "province_templates.hpp"
#include "triggers.hpp"

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

constexpr inline auto utility_decay_p = 0.998f;
constexpr inline auto current_profits_weight_p =
utility_decay_p
/ (1 - utility_decay_p);
constexpr inline auto short_term_profits_weight_p =
current_profits_weight_p
/ (1 - utility_decay_p);
constexpr inline auto mid_term_profits_weight_p =
(2 * short_term_profits_weight_p - current_profits_weight_p)
/ (1.f - utility_decay_p);
constexpr inline auto long_term_profits_weight_p =
(3 * mid_term_profits_weight_p - 3 * short_term_profits_weight_p + current_profits_weight_p)
/ (1.f - utility_decay_p);

//constexpr inline auto utility_decay_n = 0.6f;
constexpr inline auto utility_decay_n = 0.998f;
constexpr inline auto current_profits_weight_n =
utility_decay_n
/ (1 - utility_decay_n);
constexpr inline auto short_term_profits_weight_n =
current_profits_weight_n
/ (1 - utility_decay_n);
constexpr inline auto mid_term_profits_weight_n =
(2 * short_term_profits_weight_n - current_profits_weight_n)
/ (1.f - utility_decay_n);
constexpr inline auto long_term_profits_weight_n =
(3 * mid_term_profits_weight_n - 3 * short_term_profits_weight_n + current_profits_weight_n)
/ (1.f - utility_decay_n);

constexpr inline auto utility_decay_trade = 0.1f;
constexpr inline auto current_profits_weight_trade =
utility_decay_trade
/ (1 - utility_decay_trade);
constexpr inline auto short_term_profits_weight_trade =
current_profits_weight_trade
/ (1 - utility_decay_trade);
constexpr inline auto mid_term_profits_weight_trade =
(2 * short_term_profits_weight_trade - current_profits_weight_trade)
/ (1.f - utility_decay_trade);
constexpr inline auto long_term_profits_weight_trade =
(3 * mid_term_profits_weight_trade - 3 * short_term_profits_weight_trade + current_profits_weight_trade)
/ (1.f - utility_decay_trade);

float expected_savings_per_capita(sys::state& state) {
	return 0.0001f;
}

void sanity_check(sys::state& state) {
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

template void for_each_new_factory<std::function<void(new_factory)>>(sys::state&, dcon::province_id, std::function<void(new_factory)>&&);
template void for_each_upgraded_factory<std::function<void(upgraded_factory)>>(sys::state&, dcon::province_id, std::function<void(upgraded_factory)>&&);

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

float ideal_pound_conversion_rate(sys::state& state, dcon::market_id n) {
	return state.world.market_get_life_needs_costs(n, state.culture_definitions.primary_factory_worker)
		+ 0.1f * state.world.market_get_everyday_needs_costs(n, state.culture_definitions.primary_factory_worker);
}

float gdp_adjusted(sys::state& state, dcon::market_id n) {
	float raw = state.world.market_get_gdp(n);
	float ideal_pound = ideal_pound_conversion_rate(state, n);
	return raw / ideal_pound;
}

float gdp_adjusted(sys::state& state, dcon::nation_id n) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto sid = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		total = total + economy::gdp_adjusted(state, market);
	});
	return total;
}

struct spending_cost {
	float construction;
	float other;
	float total;
};

spending_cost full_spending_cost(sys::state& state, dcon::nation_id n);
void populate_army_consumption(sys::state& state);
void populate_navy_consumption(sys::state& state);
void populate_construction_consumption(sys::state& state);

float commodity_daily_production_amount(sys::state& state, dcon::commodity_id c) {
	// TODO
	return 0.f;
}

float stockpile_commodity_daily_increase(sys::state& state, dcon::commodity_id c, dcon::nation_id n) {
	// TODO
	return 0.f;
}

float global_market_commodity_daily_increase(sys::state& state, dcon::commodity_id c) {
	// TODO
	return 0.f;
}

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

	auto const csize = state.world.commodity_size();

	state.world.for_each_commodity([&](auto cid) {
		state.world.execute_serial_over_province([&](auto ids) {
			state.world.province_set_artisan_score(ids, cid, 0.f);
		});
	});
}
bool valid_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_available_from_start(c)
		|| state.world.nation_get_unlocked_commodities(n, c);
}

template<typename T>
ve::mask_vector valid_need(sys::state& state, T n, dcon::commodity_id c) {
	ve::mask_vector available_at_start = state.world.commodity_get_is_available_from_start(c);
	ve::mask_vector active_mask = state.world.nation_get_unlocked_commodities(n, c);
	return available_at_start || active_mask;
}

bool valid_life_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_life_need(c)
		&& (
			state.world.commodity_get_is_available_from_start(c)
			|| state.world.nation_get_unlocked_commodities(n, c)
		);
}
bool valid_everyday_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_everyday_need(c)
		&& (
			state.world.commodity_get_is_available_from_start(c)
			|| state.world.nation_get_unlocked_commodities(n, c)
		);
}
bool valid_luxury_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_luxury_need(c)
		&& (
			state.world.commodity_get_is_available_from_start(c)
			|| state.world.nation_get_unlocked_commodities(n, c)
		);
}

void initialize_needs_weights(sys::state& state, dcon::market_id n) {
	auto zone = state.world.market_get_zone_from_local_market(n);
	auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_life_need(state, nation, c)) {
				auto& w = state.world.market_get_life_needs_weights(n, c);
				w = 0.001f;
			}
		});
	}
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_everyday_need(state, nation, c)) {
				auto& w = state.world.market_get_everyday_needs_weights(n, c);
				w = 0.001f;
			}
		});
	}
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_luxury_need(state, nation, c)) {
				auto& w = state.world.market_get_luxury_needs_weights(n, c);
				w = 0.001f;
			}
		});
	}
}

float need_weight(sys::state& state, dcon::market_id n, dcon::commodity_id c, float base_wage) {
	auto cost_per_person = std::max(price(state, n, c), 0.0001f) / state.defines.alice_needs_scaling_factor;
	auto wage_ratio = base_wage / cost_per_person;
	return std::min(1.f, wage_ratio * wage_ratio / 2.f);
	//return 1.f;
}

void rebalance_needs_weights(sys::state& state, dcon::market_id n) {
	auto zone = state.world.market_get_zone_from_local_market(n);
	auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
	auto capital = state.world.state_instance_get_capital(zone);

	{
		auto wage = state.world.province_get_labor_price(capital, labor::no_education);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_life_need(state, nation, c)) {
				auto weight = need_weight(state, n, c, wage);
				auto& w = state.world.market_get_life_needs_weights(n, c);
				w = weight * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed);

				assert(std::isfinite(w));
				assert(w <= 1.f + 0.01f);
			}
		});
	}

	{
		auto wage = state.world.province_get_labor_price(capital, labor::no_education) * 2.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_everyday_need(state, nation, c)) {
				auto weight = need_weight(state, n, c, wage);
				auto& w = state.world.market_get_everyday_needs_weights(n, c);
				w = weight * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed);

				assert(std::isfinite(w));
				assert(w <= 1.f + 0.01f);
			}
		});
	}

	{
		auto wage = state.world.province_get_labor_price(capital, labor::no_education) * 20.f;
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(valid_luxury_need(state, nation, c)) {
				auto weight = need_weight(state, n, c, wage);
				auto& w = state.world.market_get_luxury_needs_weights(n, c);
				w = weight * state.defines.alice_need_drift_speed + w * (1.0f - state.defines.alice_need_drift_speed);

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
	uint32_t steps = 365;
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

inline constexpr float ln_2 = 0.30103f;

//crude approximation of exp
float pseudo_exp_for_negative(float f) {
	if(f < -128.f) {
		return 0.f;
	}

	f = f / 128.f;
	f = 1 + f + f * f / 2 + f * f * f / 6;

	f = f * f; // 2
	f = f * f; // 4
	f = f * f; // 8
	f = f * f; // 16
	f = f * f; // 32
	f = f * f; // 64
	f = f * f; // 128

	return f;
}
//crude vectorised approximation of exp
ve::fp_vector ve_pseudo_exp_for_negative(ve::fp_vector f) {
	ve::fp_vector temp = f;

	f = f / 128.f;
	f = 1 + f + f * f / 2 + f * f * f / 6;

	f = f * f; // 2
	f = f * f; // 4
	f = f * f; // 8
	f = f * f; // 16
	f = f * f; // 32
	f = f * f; // 64
	f = f * f; // 128

	ve::fp_vector result = ve::select(temp < -128.f, 0.f, f);

	ve::apply(
		[](float value) {
			assert(std::isfinite(value));
		}, result
	);

	return result;
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


	auto expected_savings = expected_savings_per_capita(state);

	state.world.for_each_pop([&](dcon::pop_id p) {
		auto fp = fatten(state.world, p);
		pop_demographics::set_life_needs(state, p, 1.0f);
		pop_demographics::set_everyday_needs(state, p, 0.1f);
		pop_demographics::set_luxury_needs(state, p, 0.0f);
		fp.set_savings(fp.get_size() * expected_savings);
	});

	sanity_check(state);

	state.world.execute_serial_over_market([&](auto mid) {
		state.world.market_set_income_scale(mid, 1.f);
	});
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
			auto max_rgo_size = std::ceil(4000.f * state.map_state.map_data.province_area[province::to_map_id(p)]);
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

			dcon::commodity_id main_trade_good = state.world.province_get_rgo(p);
			bool is_mine = state.world.commodity_get_is_mine(main_trade_good);

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
				auto fc = fatten(state.world, c);
				assert(std::isfinite(true_distribution[c.index()]));
				auto proposed_size = (pop_amount * 10.f + state.defines.alice_base_rgo_employment_bonus) * true_distribution[c.index()];
				if(proposed_size > state.defines.alice_secondary_rgos_min_employment) {
					state.world.province_set_rgo_size(p, c,
						state.world.province_get_rgo_size(p, c) + proposed_size
					);
					state.world.province_set_rgo_potential(p, c,
						state.world.province_get_rgo_potential(p, c) + max_rgo_size * true_distribution[c.index()]
					);
					state.world.province_set_rgo_efficiency(p, c, 1.f);
					state.world.province_set_rgo_target_employment(p, c, pop_amount);
				}
			});
		});
	}

	state.world.for_each_nation([&](dcon::nation_id n) {
		auto fn = fatten(state.world, n);
		fn.set_administrative_spending(int8_t(25));
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

float effective_tariff_import_rate(sys::state& state, dcon::nation_id n, dcon::market_id m) {
	auto tariff_efficiency = std::max(0.0f, nations::tariff_efficiency(state, n, m));
	auto r = tariff_efficiency * float(state.world.nation_get_tariffs_import(n)) / 100.0f;
	return std::max(r, 0.0f);
}
float effective_tariff_export_rate(sys::state& state, dcon::nation_id n, dcon::market_id m) {
	auto tariff_efficiency = std::max(0.0f, nations::tariff_efficiency(state, n, m));
	auto r = tariff_efficiency * float(state.world.nation_get_tariffs_export(n)) / 100.0f;
	return std::max(r, 0.0f);
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

float subsistence_max_pseudoemployment(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	return subsistence_size(state, p) * 1.1f;
}

void update_local_subsistence_factor(sys::state& state) {
	state.world.execute_serial_over_province([&](auto ids) {
		auto quality = (ve::to_float(state.world.province_get_life_rating(ids)) - 10.f) / 10.f;
		quality = ve::max(quality, 0.f) + 0.01f;
		auto score = (subsistence_factor * quality) + subsistence_score_life * 0.5f;
		state.world.province_set_subsistence_score(ids, score);
	});
}

float adjusted_subsistence_score(sys::state& state, dcon::province_id p) {
	return state.world.province_get_subsistence_score(p)
		* state.world.province_get_subsistence_employment(p)
		/ (state.world.province_get_demographics(p, demographics::total) + 1.f);
}

template<typename T>
ve::fp_vector ve_adjusted_subsistence_score(
	sys::state& state,
	T p
) {
	return state.world.province_get_subsistence_score(p)
		* state.world.province_get_subsistence_employment(p)
		/ (state.world.province_get_demographics(p, demographics::total) + 1.f);
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

float factory_total_desired_employment(sys::state const& state, dcon::factory_id f) {
	// TODO: Document this, also is this a stub?
	auto pid = state.world.factory_get_province_from_factory_location(f);
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	return (
		state.world.factory_get_unqualified_employment(f)
		+ state.world.factory_get_primary_employment(f)
		+ state.world.factory_get_secondary_employment(f)
	);
}

float factory_total_desired_employment_score(sys::state const& state, dcon::factory_id f) {
	// TODO: Document this, also is this a stub?
	auto pid = state.world.factory_get_province_from_factory_location(f);
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	return factory_total_desired_employment(state, f) / state.world.factory_get_size(f);
}

float factory_total_employment_score(sys::state const& state, dcon::factory_id f) {
	// TODO: Document this, also is this a stub?
	auto pid = state.world.factory_get_province_from_factory_location(f);
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	return factory_total_employment (state, f) / state.world.factory_get_size(f);
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
						subsistence_max_pseudoemployment(state, n, p)
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
		auto scale = owner.get_spending_level();
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
					state.world.market_get_army_demand(market, supply_cost.commodity_type[i]) +=
						supply_cost.commodity_amounts[i]
						* state.world.nation_get_unit_stats(owner, type).supply_consumption
						* o_sc_mod * strength;
				} else {
					break;
				}
			}
			auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto reinforcement = military::unit_calculate_reinforcement(state, reg);
					if(reinforcement > 0) {
						// Regiment needs reinforcement - add extra consumption. Every 1% of reinforcement demands 1% of unit cost
						state.world.market_get_army_demand(market, build_cost.commodity_type[i]) +=
							build_cost.commodity_amounts[i]
							* reinforcement;
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
					state.world.market_get_navy_demand(market, supply_cost.commodity_type[i]) +=
						supply_cost.commodity_amounts[i]
						* state.world.nation_get_unit_stats(owner, type).supply_consumption
						* o_sc_mod;
				} else {
					break;
				}
			}

			auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto reinforcement = military::unit_calculate_reinforcement(state, shp);
					if(reinforcement > 0) {
						// Ship needs repair - add extra consumption. Every 1% of reinforcement demands 1% of unit cost
						state.world.market_get_army_demand(market, build_cost.commodity_type[i]) +=
							build_cost.commodity_amounts[i]
							* reinforcement;
					}
				} else {
					break;
				}
			}
		}
	});
}


spending_cost full_spending_cost(sys::state& state, dcon::nation_id n) {
	spending_cost costs = {
		.construction = 0.f,
		.other = 0.f,
		.total = 0.f,
	};

	float total = 0.0f;
	float military_total = 0.f;
	uint32_t total_commodities = state.world.commodity_size();

	float base_budget = state.world.nation_get_stockpiles(n, economy::money);
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
	auto const admin_spending = full_spendings_administration(state, n);
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

	auto const e_spending = float(state.world.nation_get_education_spending(n)) * float(state.world.nation_get_education_spending(n)) / 100.0f / 100.0f;
	auto const m_spending = float(state.world.nation_get_military_spending(n)) * float(state.world.nation_get_military_spending(n)) / 100.0f / 100.f;

	auto const p_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level));
	auto const unemp_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit));

	if(p_level + unemp_level > 0.f) {
		total += base_budget * float(state.world.nation_get_social_spending(n)) / 100.0f;
	}

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		auto local_capital = state.world.state_instance_get_capital(local_state);
		auto local_control = state.world.province_get_control_ratio(local_capital);

		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			auto key = demographics::to_key(state, pt);
			auto employment_key = demographics::to_employment_key(state, pt);

			auto adj_pop_of_type =
				state.world.state_instance_get_demographics(local_state, key)
				/ state.defines.alice_needs_scaling_factor;

			if(adj_pop_of_type <= 0)
				return;

			auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
			if(ln_type == culture::income_type::education) {
				total += e_spending * adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt) * payouts_spending_multiplier;
			} else if(ln_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt) * payouts_spending_multiplier;
			}

			auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
			if(en_type == culture::income_type::education) {
				total += e_spending * adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt) * payouts_spending_multiplier;
			} else if(en_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt) * payouts_spending_multiplier;
			}

			auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
			if(lx_type == culture::income_type::education) {
				total += e_spending * adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt) * payouts_spending_multiplier;
			} else if(lx_type == culture::income_type::military) {
				total += m_spending * adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt) * payouts_spending_multiplier;
			}

			assert(std::isfinite(total) && total >= 0.0f);
		});
	});

	assert(std::isfinite(total) && total >= 0.0f);

	costs.total = total;
	costs.other = total - costs.construction;

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

void update_national_consumption(sys::state& state, dcon::nation_id n, float spending_scale, float private_investment_scale) {
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

	update_consumption_administration(state, n);
}

void update_pop_consumption(
	sys::state& state,
	ve::vectorizable_buffer<float, dcon::nation_id>& invention_count
) {
	uint32_t total_commodities = state.world.commodity_size();

	state.ui_state.last_tick_investment_pool_change = 0;

	// satisfaction buffers
	// they store how well pops satisfy their needs
	// we store them per pop now
	// because iteration per state
	// and per pop of each state is way too slow
	// we start with filling them with according subsistence values
	// and then attempt to buy the rest

	ve::int_vector build_factory = issue_rule::pop_build_factory;
	ve::int_vector expand_factory = issue_rule::pop_expand_factory;
	ve::int_vector can_invest = expand_factory | build_factory;

	ve::fp_vector total_spendings{};

	state.world.execute_serial_over_pop([&](auto ids) {
		// get all data into vectors
		auto provs = state.world.pop_get_province_from_pop_location(ids);
		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
		auto pop_type = state.world.pop_get_poptype(ids);
		auto strata = state.world.pop_type_get_strata(pop_type);
		auto life_costs = ve::apply(
			[&](dcon::market_id m, dcon::pop_type_id pt) {
				return state.world.market_get_life_needs_costs(m, pt);
			}, markets, pop_type
		);
		auto everyday_costs = ve::apply(
			[&](dcon::market_id m, dcon::pop_type_id pt) {
				return state.world.market_get_everyday_needs_costs(m, pt);
			}, markets, pop_type
		);
		auto luxury_costs = ve::apply(
			[&](dcon::market_id m, dcon::pop_type_id pt) {
				return state.world.market_get_luxury_needs_costs(m, pt);
			}, markets, pop_type
		);
		auto pop_size = state.world.pop_get_size(ids);
		auto savings = state.world.pop_get_savings(ids);
		auto subsistence = ve_adjusted_subsistence_score(state, provs);

		auto available_subsistence = ve::min(subsistence_score_life, subsistence);
		subsistence = subsistence - available_subsistence;
		auto life_needs_satisfaction = available_subsistence / subsistence_score_life;

		auto everyday_needs_satisfaction = ve::fp_vector{ 0.f };
		auto luxury_needs_satisfaction = ve::fp_vector{ 0.f };

		// calculate market expenses

		auto life_to_satisfy = ve::max(0.f, 1.f - life_needs_satisfaction);
		auto everyday_to_satisfy = ve::max(0.f, 1.f - everyday_needs_satisfaction);
		auto luxury_to_satisfy = ve::max(0.f, 1.f - luxury_needs_satisfaction);

		auto required_spendings_for_life_needs =
			life_to_satisfy
			* life_costs
			* pop_size / state.defines.alice_needs_scaling_factor;

		auto required_spendings_for_everyday_needs =
			everyday_to_satisfy
			* everyday_costs
			* pop_size / state.defines.alice_needs_scaling_factor;

		auto required_spendings_for_luxury_needs =
			luxury_to_satisfy
			* luxury_costs
			* pop_size / state.defines.alice_needs_scaling_factor;

		// if goods are way too expensive:
		// reduce spendings, possibly to zero
		// even if it will lead to loss of money:
		// because at some point optimal way to satisfy your needs
		// is to not satisfy them immediately
		// we don't have base production for every type of goods,
		// so it's needed to avoid inflation spiral in early game
		//
		// PS it's a way to embed "soft" price limits without breaking the ingame economy

		// we want to focus on life needs first if we are poor AND our satisfaction is low
		auto is_poor = ve::max(0.f, 1.f - 4.f * savings / (0.00001f + required_spendings_for_life_needs));
		is_poor = ve::min(1.f, ve::max(0.f, is_poor * 2.f * (life_to_satisfy - 0.5f)));

		auto life_spending_mod = //ve::fp_vector{ 1.f };
			(savings * state.defines.alice_needs_lf_spend) * (1.f - is_poor) + is_poor;
		auto everyday_spending_mod =
			(savings * state.defines.alice_needs_ev_spend) * (1.f - is_poor);
		auto luxury_spending_mod =
			(savings * state.defines.alice_needs_lx_spend) * (1.f - is_poor);

		// clamp
		life_spending_mod = ve::max(0.f, ve::min(1.f, life_spending_mod));
		everyday_spending_mod = ve::max(0.f, ve::min(1.f, everyday_spending_mod));
		luxury_spending_mod = ve::max(0.f, ve::min(1.f, luxury_spending_mod));

		auto zero_life_costs = required_spendings_for_life_needs == 0;
		auto zero_everyday_costs = required_spendings_for_everyday_needs == 0;
		auto zero_luxury_costs = required_spendings_for_luxury_needs == 0;


		// handle investments now to prevent construction being stuck with poor investors:
		auto nation_rules = state.world.nation_get_combined_issue_rules(nations);

		auto is_civilised = state.world.nation_get_is_civilized(nations);
		auto allows_investment_mask = (nation_rules & can_invest) != 0;
		ve::mask_vector nation_allows_investment = is_civilised && allows_investment_mask;

		auto capitalists_mask = pop_type == state.culture_definitions.capitalists;
		auto middle_class_investors_mask = pop_type == state.culture_definitions.artisans || pop_type == state.culture_definitions.secondary_factory_worker;
		auto farmers_mask = pop_type == state.culture_definitions.farmers;
		auto landowners_mask = pop_type == state.culture_definitions.aristocrat;

		auto capitalists_mod = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::capitalist_reinvestment);
		auto middle_class_investors_mod = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::middle_class_reinvestment);
		auto farmers_mod = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::farmers_reinvestment);
		auto landowners_mod = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::aristocrat_reinvestment);

		auto investment_ratio =
			ve::select(nation_allows_investment && capitalists_mask, capitalists_mod + state.defines.alice_invest_capitalist, 0.0f)
			+ ve::select(nation_allows_investment && landowners_mask, landowners_mod + state.defines.alice_invest_aristocrat, 0.0f)
			+ ve::select(nation_allows_investment && middle_class_investors_mask, middle_class_investors_mod + state.defines.alice_invest_middle_class, 0.0f)
			+ ve::select(nation_allows_investment && farmers_mask, farmers_mod + state.defines.alice_invest_farmer, 0.0f);

		investment_ratio = ve::max(investment_ratio, 0.0f);

		ve::apply([&](float r) {
			assert(r >= 0.f);
		}, investment_ratio);

		auto investment = savings * investment_ratio;

		savings = savings - investment;
		total_spendings = total_spendings + investment;


		// buy life needs

		auto spend_on_life_needs = life_spending_mod * ve::min(savings, required_spendings_for_life_needs);
		auto satisfaction_life_money = ve::select(
			zero_life_costs,
			life_to_satisfy,
			spend_on_life_needs / required_spendings_for_life_needs);

		savings = savings - spend_on_life_needs;
		total_spendings = total_spendings + spend_on_life_needs;		

		// buy everyday needs

		auto spend_on_everyday_needs = everyday_spending_mod * ve::min(savings, required_spendings_for_everyday_needs);
		auto satisfaction_everyday_money = ve::select(
			zero_everyday_costs,
			everyday_to_satisfy,
			spend_on_everyday_needs / required_spendings_for_everyday_needs);

		savings = savings - spend_on_everyday_needs;
		total_spendings = total_spendings + spend_on_everyday_needs;

		// handle bank savings before luxury goods spending
		// Note that farmers and middle_class don't do bank savings by default - that doens't mean they don't have savings. They don't use banks for savings without modifier (from tech, from example).
		auto capitalists_savings_mod = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::capitalist_savings);
		auto middle_class_savings_mod = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::middle_class_savings);
		auto farmers_savings_mod = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::farmers_savings);
		auto landowners_savings_mod = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::aristocrat_savings);

		auto saving_ratio =
			ve::select(capitalists_mask, capitalists_savings_mod + state.defines.alice_save_capitalist, 0.0f)
			+ ve::select(landowners_mask, landowners_savings_mod + state.defines.alice_save_aristocrat, 0.0f)
			+ ve::select(middle_class_investors_mask, middle_class_savings_mod + state.defines.alice_save_middle_class, 0.0f)
			+ ve::select(farmers_mask, farmers_savings_mod + state.defines.alice_save_farmer, 0.0f);

		auto bank_deposits = savings * saving_ratio;
		bank_deposits = ve::max(bank_deposits, 0.0f);

		ve::apply([&](float r) {
			assert(r >= 0.f);
		}, bank_deposits);

		ve::apply(
			[&](float transfer, dcon::nation_id n) {
				state.world.nation_get_national_bank(n) += transfer;
				return 0;
			}, bank_deposits, nations
		);

		savings = savings - bank_deposits;

		// buy luxury needs

		auto spend_on_luxury_needs = luxury_spending_mod * ve::min(savings, required_spendings_for_luxury_needs);
		auto satisfaction_luxury_money = ve::select(
			zero_luxury_costs,
			luxury_to_satisfy,
			spend_on_luxury_needs / required_spendings_for_luxury_needs);

		savings = savings - spend_on_luxury_needs;
		total_spendings = total_spendings + spend_on_luxury_needs;

		ve::fp_vector old_life = pop_demographics::get_life_needs(state, ids);
		ve::fp_vector old_everyday = pop_demographics::get_everyday_needs(state, ids);
		ve::fp_vector old_luxury = pop_demographics::get_luxury_needs(state, ids);

		savings = savings * (
			1.f
			- state.defines.alice_needs_lf_spend
			- state.defines.alice_needs_ev_spend
			- state.defines.alice_needs_lx_spend
		);

		// suppose that old satisfaction was calculated
		// for the same local subsistence conditions and find "raw" satisfaction
		// old = raw + sub ## first summand is "raw satisfaction"
		// we assume that currently calculated satisfaction is caused only by subsistence

		auto old_life_money =
			ve::max(0.f, old_life - life_needs_satisfaction);
		auto old_everyday_money =
			ve::max(0.f, old_everyday - everyday_needs_satisfaction);
		auto old_luxury_money =
			ve::max(0.f, old_luxury - luxury_needs_satisfaction);

		auto delayed_life_from_money =
			(old_life_money * 0.5f) + (satisfaction_life_money * 0.5f);
		auto delayed_everyday_from_money =
			(old_everyday_money * 0.5f) + (satisfaction_everyday_money * 0.5f);
		auto delayed_luxury_from_money =
			(old_luxury_money * 0.5f) + (satisfaction_luxury_money * 0.5f);

		// clamping for now
		auto result_life = ve::max(0.f, ve::min(1.f, delayed_life_from_money + life_needs_satisfaction));
		auto result_everyday = ve::max(0.f, ve::min(1.f, delayed_everyday_from_money + everyday_needs_satisfaction));
		auto result_luxury = ve::max(0.f, ve::min(1.f, delayed_luxury_from_money + luxury_needs_satisfaction));

		ve::apply([&](float life, float everyday, float luxury) {
			assert(life >= 0.f && life <= 1.f);
			assert(everyday >= 0.f && everyday <= 1.f);
			assert(luxury >= 0.f && luxury <= 1.f);
		}, result_life, result_everyday, result_luxury);


		pop_demographics::set_life_needs(state, ids, result_life);
		pop_demographics::set_everyday_needs(state, ids, result_everyday);
		pop_demographics::set_luxury_needs(state, ids, result_luxury);

		auto final_demand_scale_life =
			pop_size / state.defines.alice_needs_scaling_factor
			* delayed_life_from_money;

		auto final_demand_scale_everyday =
			pop_size / state.defines.alice_needs_scaling_factor
			* delayed_everyday_from_money;

		auto final_demand_scale_luxury =
			pop_size / state.defines.alice_needs_scaling_factor
			* delayed_luxury_from_money;

		ve::apply([&](
			dcon::market_id m,
			float scale_life,
			float scale_everyday,
			float scale_luxury,
			float investment,
			auto pop_type
			) {
				assert(scale_life >= 0.0f);
				assert(scale_everyday >= 0.0f);
				assert(scale_luxury >= 0.0f);
				assert(!isinf(scale_life));
				assert(!isinf(scale_everyday));
				assert(!isinf(scale_luxury));

				state.world.market_get_life_needs_scale(m, pop_type) += scale_life;
				state.world.market_get_everyday_needs_scale(m, pop_type) += scale_everyday;
				state.world.market_get_luxury_needs_scale(m, pop_type) += scale_luxury;
				auto zone = state.world.market_get_zone_from_local_market(m);
				auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
				state.world.nation_get_private_investment(nation) += investment;
				if(nation == state.local_player_nation) {
					state.ui_state.last_tick_investment_pool_change += investment;
				}
		},
			markets,
			final_demand_scale_life,
			final_demand_scale_everyday,
			final_demand_scale_luxury,
			investment,
			pop_type
		);

		// we do not save savings here as they will be overwritten with their income later
		// state.world.pop_set_savings(ids, savings);
	});

#ifndef NDEBUG
	std::string debug_output = "Total pops spendings on needs: " + std::to_string(total_spendings.reduce());
	state.console_log(debug_output);
#endif // !NDEBUG

	// iterate over all (market,pop type,trade good) pairs to finalise this calculation
	state.world.execute_serial_over_market([&](auto ids) {
		auto states = state.world.market_get_zone_from_local_market(ids);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
		auto invention_factor = state.defines.invention_impact_on_demand * invention_count.get(nations) + 1.f;

		ve::fp_vector life_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_life_needs) + 1.0f
		};
		ve::fp_vector everyday_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
		};
		ve::fp_vector luxury_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
		};

		for(const auto t : state.world.in_pop_type) {
			auto scale_life = state.world.market_get_life_needs_scale(ids, t);
			auto scale_everyday = state.world.market_get_everyday_needs_scale(ids, t);
			auto scale_luxury = state.world.market_get_luxury_needs_scale(ids, t);

			auto strata = t.get_strata();

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

				auto life_weight =
					state.world.market_get_life_needs_weights(ids, cid);
				auto everyday_weight =
					state.world.market_get_everyday_needs_weights(ids, cid);
				auto luxury_weight =
					state.world.market_get_luxury_needs_weights(ids, cid);
				auto base_life =
					state.world.pop_type_get_life_needs(t, cid);
				auto base_everyday =
					state.world.pop_type_get_everyday_needs(t, cid);
				auto base_luxury =
					state.world.pop_type_get_luxury_needs(t, cid);

				auto valid_good_mask = valid_need(state, nations, cid);

				auto demand_life =
					base_life
					* scale_life
					* life_mul[strata]
					* life_weight
					* state.defines.alice_lf_needs_scale;
				auto demand_everyday =
					base_everyday
					* scale_everyday
					* everyday_mul[strata]
					* everyday_weight
					* state.defines.alice_ev_needs_scale
					* invention_factor;
				auto demand_luxury =
					base_luxury
					* scale_luxury
					* luxury_mul[strata]
					* luxury_weight
					* state.defines.alice_lx_needs_scale
					* invention_factor;

				demand_life = ve::select(valid_good_mask, demand_life, 0.f);
				demand_everyday = ve::select(valid_good_mask, demand_everyday, 0.f);
				demand_luxury = ve::select(valid_good_mask, demand_luxury, 0.f);

				register_demand(state, ids, cid, demand_life, economy_reason::pop);
				register_demand(state, ids, cid, demand_everyday, economy_reason::pop);
				register_demand(state, ids, cid, demand_luxury, economy_reason::pop);
			}
		}
	});
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
	bool found_investment = false;

	for(auto s : provinces_in_order) {
		auto sid = state.world.province_get_state_membership(s);
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		auto pw_num = state.world.province_get_demographics(s,
				demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		auto pw_employed = state.world.province_get_demographics(s,
				demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));

		int32_t num_factories = 0;
		float profit = 0.0f;
		dcon::factory_id selected_factory;

		// is there an upgrade target ?
		for(auto f : state.world.province_get_factory_location(s)) {
			++num_factories;

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
				state.world.nation_get_private_investment(n) -= amt;

				auto subjects = nations::nation_get_subjects(state, n);
				if(subjects.size() > 0) {
					auto part = amt / subjects.size();
					for(auto s : subjects) {
						state.world.nation_get_private_investment(s) += part;
					}
				} else if(overlord) {
					state.world.nation_get_private_investment(overlord) += amt;
				}
			}
		} else { // private investment not allowed
			state.world.nation_set_private_investment(n, 0.0f);
		}
	}

		
}

void daily_update(sys::state& state, bool presimulation, float presimulation_stage) {
	float average_expected_savings = expected_savings_per_capita(state);

	sanity_check(state);

	/* initialization parallel block */

	concurrency::parallel_for(0, 10, [&](int32_t index) {
		switch(index) {
		case 0:
			populate_army_consumption(state);
			break;
		case 1:
			populate_navy_consumption(state);
			break;
		case 2:
			populate_private_construction_consumption(state);
			break;
		case 3:
			update_factory_triggered_modifiers(state);
			break;
		case 4:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_everyday_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 5:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_luxury_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 6:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_market([&](auto nids) {
					state.world.market_set_life_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 7:
			state.world.execute_serial_over_nation([&](auto ids) {
				state.world.nation_set_subsidies_spending(ids, 0.0f);
			});
			break;
		case 8:
			state.world.execute_serial_over_nation([&](auto ids) {
				auto treasury = state.world.nation_get_stockpiles(ids, economy::money);
				state.world.nation_set_last_treasury(ids, treasury);
			});
			break;
		}
	});

	populate_construction_consumption(state);

	sanity_check(state);

	/* end initialization parallel block */

	auto const num_nation = state.world.nation_size();
	uint32_t total_commodities = state.world.commodity_size();

	/*
		update scoring for provinces
	*/

	update_land_ownership(state);
	update_local_subsistence_factor(state);

	sanity_check(state);

	// update employment before zeroing demand/supply	

	sanity_check(state);

	auto coastal_capital_buffer = ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>(state.world.state_instance_size());

	state.world.execute_parallel_over_state_instance([&](auto ids) {
		ve::apply([&](auto sid) {
		coastal_capital_buffer.set(sid, province::state_get_coastal_capital(state, sid));
		}, ids);
	});

	auto export_tariff_buffer = state.world.market_make_vectorizable_float_buffer();
	auto import_tariff_buffer = state.world.market_make_vectorizable_float_buffer();

	state.world.execute_serial_over_market([&](auto ids) {
		auto sids = state.world.market_get_zone_from_local_market(ids);
		auto nids = state.world.state_instance_get_nation_from_state_ownership(sids);
		ve::apply([&](auto nid, auto mid) {
			export_tariff_buffer.set(mid, effective_tariff_export_rate(state, nid, mid));
			import_tariff_buffer.set(mid, effective_tariff_import_rate(state, nid, mid));
		}, nids, ids);
	});


	//static auto median_price_buffer = state.world.commodity_make_vectorizable_float_buffer();

	//state.world.for_each_commodity([&](auto id) {
	//	median_price_buffer.set(median_price(state, id));
	//});


	// update trade volume based on potential profits right at the start
	// we can't put it between demand and supply generation!
	state.world.execute_parallel_over_trade_route([&](auto trade_route) {

		//concurrency::parallel_for(uint32_t(0), total_commodities, [&](uint32_t k) {
			//dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

		auto A = ve::apply([&](auto route) {
			return state.world.trade_route_get_connected_markets(route, 0);
			}, trade_route);

		auto B = ve::apply([&](auto route) {
			return state.world.trade_route_get_connected_markets(route, 1);
		}, trade_route);

		auto s_A = state.world.market_get_zone_from_local_market(A);
		auto s_B = state.world.market_get_zone_from_local_market(B);

		auto n_A = state.world.state_instance_get_nation_from_state_ownership(s_A);
		auto n_B = state.world.state_instance_get_nation_from_state_ownership(s_B);

		auto capital_A = state.world.state_instance_get_capital(s_A);
		auto capital_B = state.world.state_instance_get_capital(s_B);

		auto port_A = coastal_capital_buffer.get(s_A);
		auto port_B = coastal_capital_buffer.get(s_B);

		auto controller_capital_A = state.world.province_get_nation_from_province_control(capital_A);
		auto controller_capital_B = state.world.province_get_nation_from_province_control(capital_B);

		auto controller_port_A = state.world.province_get_nation_from_province_control(port_A);
		auto controller_port_B = state.world.province_get_nation_from_province_control(port_B);

		auto sphere_A = state.world.nation_get_in_sphere_of(controller_capital_A);
		auto sphere_B = state.world.nation_get_in_sphere_of(controller_capital_B);

		auto overlord_A = state.world.overlord_get_ruler(
			state.world.nation_get_overlord_as_subject(controller_capital_A)
		);
		auto overlord_B = state.world.overlord_get_ruler(
			state.world.nation_get_overlord_as_subject(controller_capital_B)
		);

		auto has_overlord_mask_A = overlord_A != dcon::nation_id{};
		auto has_overlord_mask_B = overlord_B != dcon::nation_id{};
		auto has_sphere_mask_A = sphere_A != dcon::nation_id{};
		auto has_sphere_mask_B = sphere_B != dcon::nation_id{};

		// Subjects have embargo of overlords propagated onto them
		auto market_leader_A = ve::select(has_overlord_mask_A, overlord_A, ve::select(has_sphere_mask_A, sphere_A, n_A));
		auto market_leader_B = ve::select(has_overlord_mask_B, overlord_B, ve::select(has_sphere_mask_B, sphere_B, n_B));

		// Equal/unequal trade treaties
		auto A_open_to_B = ve::apply([&](auto n_a, auto n_b) {
			auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_b, n_a);
			if(source_tariffs_rel) {
				auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
				// Enddt empty signalises revoken agreement
				// Enddt > cur_date signalises that the agreement can't be broken
				if(enddt) {
					return true;
				}
			}
			return false;
		}, market_leader_A, controller_capital_B);

		auto B_open_to_A = ve::apply([&](auto n_a, auto n_b) {
			auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_a, n_b);

			if(target_tariffs_rel) {
				auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
				if(enddt) {
					return true;
				}
			}

			return false;
		}, market_leader_B, controller_capital_A);

		auto A_is_open_to_B = sphere_A == controller_capital_B || overlord_A == controller_capital_B || A_open_to_B;
		auto B_is_open_to_A = sphere_B == controller_capital_A || overlord_B == controller_capital_A || B_open_to_A;

		// these are not needed at the moment
		// because overlord and subject are always in the same war

		/*
		auto A_joins_overlord_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, overlord_A, controller_capital_B);

		auto B_joins_overlord_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, overlord_B, controller_capital_A);
		*/

		// if market capital controller is at war with market coastal controller is different
		// or it's actually blockaded
		// consider province blockaded

		ve::mask_vector port_occupied_A = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, controller_capital_A, controller_port_A);
		ve::mask_vector port_occupied_B = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, controller_capital_B, controller_port_B);

		ve::mask_vector is_A_blockaded = state.world.province_get_is_blockaded(port_A) || port_occupied_A;
		ve::mask_vector is_B_blockaded = state.world.province_get_is_blockaded(port_B) || port_occupied_B;

		// if market capital controllers are at war then we will break the link
		auto at_war = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, controller_capital_A, controller_capital_B);

		// it created quite bad oscilation
		// so i decided to transfer goods into stockpile directly
		// and consider that all of them were sold at given price
		//auto actually_bought_ratio_A = state.world.market_get_supply_sold_ratio(A, c);
		//auto actually_bought_ratio_B = state.world.market_get_supply_sold_ratio(B, c);

		auto is_A_civ = state.world.nation_get_is_civilized(n_A);
		auto is_B_civ = state.world.nation_get_is_civilized(n_B);

		auto is_sea_route = state.world.trade_route_get_is_sea_route(trade_route);
		auto is_land_route = state.world.trade_route_get_is_land_route(trade_route);
		auto same_nation = controller_capital_A == controller_capital_B;

		// Ban international sea routes or international land routes based on the corresponding modifiers
		auto A_bans_sea_trade = state.world.nation_get_modifier_values(n_A, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
		auto B_bans_sea_trade = state.world.nation_get_modifier_values(n_B, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
		auto sea_trade_banned = A_bans_sea_trade || B_bans_sea_trade;
		auto A_bans_land_trade = state.world.nation_get_modifier_values(n_A, sys::national_mod_offsets::disallow_land_trade) > 0.f;
		auto B_bans_land_trade = state.world.nation_get_modifier_values(n_B, sys::national_mod_offsets::disallow_land_trade) > 0.f;
		auto land_trade_banned = A_bans_land_trade || B_bans_land_trade;
		auto trade_banned = (is_sea_route && sea_trade_banned && !same_nation) || (is_land_route && land_trade_banned && !same_nation);

		is_sea_route = is_sea_route && !is_A_blockaded && !is_B_blockaded;

		// sphere joins embargo
		// subject joins embargo
		// diplomatic embargos
		auto A_joins_sphere_wide_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, sphere_A, controller_capital_B);

		auto B_joins_sphere_wide_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, sphere_B, controller_capital_A);

		auto A_has_embargo = ve::apply([&](auto n_a, auto n_b) {
			auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_b, n_a);
			auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_a, n_b);

			return state.world.unilateral_relationship_get_embargo(source_tariffs_rel) || state.world.unilateral_relationship_get_embargo(target_tariffs_rel);
		}, market_leader_A, controller_capital_B);

		A_joins_sphere_wide_embargo = A_has_embargo || A_joins_sphere_wide_embargo;

		auto B_has_embargo = ve::apply([&](auto n_a, auto n_b) {
			auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_b, n_a);
			auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_a, n_b);

			return state.world.unilateral_relationship_get_embargo(source_tariffs_rel) || state.world.unilateral_relationship_get_embargo(target_tariffs_rel);
		}, market_leader_B, controller_capital_A);

		B_joins_sphere_wide_embargo = B_has_embargo || B_joins_sphere_wide_embargo;

		// these are not needed at the moment
		// because overlord and subject are always in the same war

		/*
		auto A_joins_overlord_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, overlord_A, controller_capital_B);

		auto B_joins_overlord_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, overlord_B, controller_capital_A);
		*/

		// it created quite bad oscilation
		// so i decided to transfer goods into stockpile directly
		// and consider that all of them were sold at given price
		//auto actually_bought_ratio_A = state.world.market_get_supply_sold_ratio(A, c);
		//auto actually_bought_ratio_B = state.world.market_get_supply_sold_ratio(B, c);

		auto merchant_cut = ve::select(same_nation, ve::fp_vector{ 1.f + economy::merchant_cut_domestic }, ve::fp_vector{ 1.f + economy::merchant_cut_foreign });

		auto import_tariff_A = ve::select(same_nation || A_is_open_to_B, ve::fp_vector{ 0.f }, import_tariff_buffer.get(A));
		auto export_tariff_A = ve::select(same_nation || A_is_open_to_B, ve::fp_vector{ 0.f }, export_tariff_buffer.get(A));
		auto import_tariff_B = ve::select(same_nation || B_is_open_to_A, ve::fp_vector{ 0.f }, import_tariff_buffer.get(B));
		auto export_tariff_B = ve::select(same_nation || B_is_open_to_A, ve::fp_vector{ 0.f }, export_tariff_buffer.get(B));

		auto imports_aversion_A = ve::min(1.f, ve::max(0.f, 1.f + state.world.market_get_stockpile(A, economy::money) / (state.world.province_get_labor_price(capital_A, labor::no_education) + 0.00001f) / market_savings_target));
		auto imports_aversion_B = ve::min(1.f, ve::max(0.f, 1.f + state.world.market_get_stockpile(B, economy::money) / (state.world.province_get_labor_price(capital_B, labor::no_education) + 0.00001f) / market_savings_target));

		ve::fp_vector distance = 999999.f;
		auto land_distance = state.world.trade_route_get_land_distance(trade_route);
		auto sea_distance = state.world.trade_route_get_sea_distance(trade_route);

		distance = ve::select(is_land_route, ve::min(distance, land_distance), distance);
		distance = ve::select(is_sea_route, ve::min(distance, sea_distance), distance);

		state.world.trade_route_set_distance(trade_route, distance);

		auto trade_good_loss_mult = ve::max(0.f, 1.f - 0.0001f * distance);

		// we assume that 2 uneducated persons (1 from each market) can transport 1 unit of goods along path of 1 effective day length
		// we do it this way to avoid another assymetry in calculations
		auto transport_cost =
			distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good
			* (
				state.world.province_get_labor_price(capital_A, labor::no_education)
				+ state.world.province_get_labor_price(capital_B, labor::no_education)
			);

		auto reset_route = at_war
			|| A_joins_sphere_wide_embargo
			|| B_joins_sphere_wide_embargo
			|| trade_banned
			|| !ve::apply([&](auto r) { return state.world.trade_route_is_valid(r); }, trade_route)
			|| (!is_sea_route && !is_land_route);

		for(auto c : state.world.in_commodity) {
			if(state.world.commodity_get_money_rgo(c) || state.world.commodity_get_is_local(c)) {
				continue;
			}

			auto unlocked_A = state.world.nation_get_unlocked_commodities(n_A, c);
			auto unlocked_B = state.world.nation_get_unlocked_commodities(n_B, c);

			auto reset_route_commodity = reset_route;

			if(!state.world.commodity_get_is_available_from_start(c)) {
				reset_route_commodity = reset_route_commodity
					|| (!unlocked_A && !unlocked_B);
			}

			//state.world.execute_serial_over_trade_route([&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, c);

			auto absolute_volume = ve::abs(current_volume);
			//auto sat = state.world.market_get_direct_demand_satisfaction(origin, c);

			// effect of scale
			// volume reduces transport costs
			auto effect_of_scale = ve::max(0.1f, 1.f - absolute_volume * effect_of_transportation_scale);

			auto price_A_export = ve_price(state, A, c) * (1.f + export_tariff_A);
			auto price_B_export = ve_price(state, B, c) * (1.f + export_tariff_B);

			auto price_A_import = ve_price(state, A, c) * (1.f - import_tariff_A) * trade_good_loss_mult;
			auto price_B_import = ve_price(state, B, c) * (1.f - import_tariff_B) * trade_good_loss_mult;

			auto current_profit_A_to_B = price_B_import - price_A_export * merchant_cut - transport_cost * effect_of_scale;
			auto current_profit_B_to_A = price_A_import - price_B_export * merchant_cut - transport_cost * effect_of_scale;

			auto none_is_profiable = (current_profit_A_to_B <= 0.f) && (current_profit_B_to_A <= 0.f);

			// we make changes slow to ensure more or less smooth changes
			// otherwise assumption of stable demand breaks
			// constant term is there to allow moving away from 0
			auto max_expansion = 0.5f + absolute_volume * 0.1f;
			auto max_shrinking = -absolute_volume * 0.005f;

			auto change = ve::select(current_profit_A_to_B > 0.f, current_profit_A_to_B / price_B_import, 0.f);
			change = ve::select(current_profit_B_to_A > 0.f, -current_profit_B_to_A / price_A_import, change);
			change = ve::select(none_is_profiable, -current_volume * 0.1f, change);

			

			// modifier for trade to slowly decay to create soft limit on transportation
			// essentially, regularisation of trade weights, but can lead to weird effects
			ve::fp_vector decay = 1.f;

			// dirty, embarassing and disgusting hack
			// to avoid trade generating too much demand
			// on already expensive goods
			// but it works well
			decay = ve::select(
				current_volume + change > 0.f,
				decay * ve::min(1.f, trade_transaction_soft_limit / price_A_export / ve::max(0.01f, current_volume + change)) * imports_aversion_B,
				decay * ve::min(1.f, trade_transaction_soft_limit / price_B_export / ve::max(0.01f, current_volume + change)) * imports_aversion_A
			);

			decay = ve::max(decay, 0.95f);
			
			// expand the route slower if goods are not actually bought:
			auto bought_A = state.world.market_get_demand_satisfaction(A, c);
			auto bought_B = state.world.market_get_demand_satisfaction(B, c);
			auto bought = ve::select(
				current_volume > 0.f,
				bought_A,
				bought_B
			);
			change = ve::select(
				change * current_volume > 0.f, // change and volume are collinear
				change * ve::max(0.2f, (bought - 0.5f) * 2.f),
				change
			);


			change = ve::select(current_volume > 0.05f,
				ve::min(ve::max(change, max_shrinking), max_expansion),
				ve::select(current_volume < -0.05f,
					ve::min(ve::max(change, -max_expansion), -max_shrinking),
					ve::min(ve::max(change, -max_expansion), max_expansion)
				)
			);

			// decay is unconditional safeguard against high weights
			change = change - current_volume * (1.f - decay);

			auto new_volume = ve::select(reset_route_commodity, 0.f, current_volume + change);

			state.world.trade_route_set_volume(trade_route, c, new_volume);

			ve::apply([&](auto route) {
				assert(std::isfinite(state.world.trade_route_get_volume(route, c)));
			}, trade_route);
		}
	});

	sanity_check(state);

	// limit trade with local throughput

	concurrency::parallel_for(uint32_t(0), total_commodities, [&](uint32_t k) {
		dcon::commodity_id commodity{ dcon::commodity_id::value_base_t(k) };
		state.world.for_each_market([&](auto market) {
			auto province = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(market));
			auto satisfied_local_transport = state.world.province_get_labor_demand_satisfaction(province, labor::no_education);
			state.world.market_for_each_trade_route(market, [&](auto trade_route) {
				state.world.trade_route_set_volume(
					trade_route,
					commodity,
					state.world.trade_route_get_volume(trade_route, commodity)
					* std::max(0.999999f, satisfied_local_transport)
				);
			});
		});
	});

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

				auto available = state.world.commodity_get_is_available_from_start(c);
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

				auto available = state.world.commodity_get_is_available_from_start(c);
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

				auto available = state.world.commodity_get_is_available_from_start(c);
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

	update_production_consumption(state);

	sanity_check(state);

	// RGO do not consume anything... yet

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

	update_pop_consumption(state, invention_count);

	sanity_check(state);

	static auto spent_on_construction_buffer = state.world.nation_make_vectorizable_float_buffer();

	// STEP 4 national budget updates
	for(auto n : state.nations_by_rank) {
		spent_on_construction_buffer.set(n, 0.f);

		auto cap_prov = state.world.nation_get_capital(n);
		auto cap_continent = state.world.province_get_continent(cap_prov);
		auto cap_region = state.world.province_get_connected_region_id(cap_prov);
		{
			// update national spending
			//
			// step 1: figure out total
			auto costs = full_spending_cost(state, n);

			float total = costs.total;

			// step 2: limit to actual budget
			float budget = 0.0f;
			float spending_scale = 0.0f;
			if(state.world.nation_get_is_player_controlled(n)) {
				auto& sp = state.world.nation_get_stockpiles(n, economy::money);

				/*
				BANKRUPTCY
				*/
				auto ip = interest_payment(state, n);
				// To become bankrupt nation should be unable to cover its interest payments with its actual money or more loans
				if(sp < ip && state.world.nation_get_local_loan(n) >= max_loan(state, n)) {
					go_bankrupt(state, n);
					spending_scale = 0.f;
				}
				// Interest payments
				if(ip > 0) {
					sp -= ip;
					state.world.nation_get_national_bank(n) += ip;
				}

				// If available loans don't allow run 100% of spending, adjust spending scale
				if(can_take_loans(state, n) && total - state.world.nation_get_stockpiles(n, economy::money) <= max_loan(state, n) - state.world.nation_get_local_loan(n)) {
					budget = total;
					spending_scale = 1.0f;
				} else {
					budget = std::max(0.0f, state.world.nation_get_stockpiles(n, economy::money));
					spending_scale = (total < 0.001f || total <= budget) ? 1.0f : budget / total;
				}
			} else {
				budget = std::max(0.0f, state.world.nation_get_stockpiles(n, economy::money));
				spending_scale = (total < 0.001f || total <= budget) ? 1.0f : budget / total;

				auto& sp = state.world.nation_get_stockpiles(n, economy::money);

				if(sp < 0 && sp < -max_loan(state, n)) {
					go_bankrupt(state, n);
					spending_scale = 0.f;
				}
			}

			assert(spending_scale >= 0);
			assert(std::isfinite(spending_scale));
			assert(std::isfinite(budget));

			state.world.nation_get_stockpiles(n, economy::money) -= std::min(budget, total * spending_scale);
			state.world.nation_set_spending_level(n, spending_scale);

			auto& s = state.world.nation_get_stockpiles(n, economy::money);
			auto& l = state.world.nation_get_local_loan(n);

			// Take loan
			if(s < 0 && l < max_loan(state, n) && std::abs(s) <= max_loan(state, n) - l) {
				state.world.nation_get_local_loan(n) += std::abs(s);
				state.world.nation_set_stockpiles(n, economy::money, 0);
			} else if(s < 0) {
				// Nation somehow got into negative bigger than its loans allow
				go_bankrupt(state, n);
			// Repay loan
			} else if(s > 0 && l > 0) {
				auto change = std::min(s, l);
				state.world.nation_get_local_loan(n) -= change;
				state.world.nation_get_stockpiles(n, economy::money) -= change;
			}

			float pi_total = full_private_investment_cost(state, n);
			float perceived_spending = pi_total;
			float pi_budget = state.world.nation_get_private_investment(n);
			auto pi_scale = perceived_spending <= pi_budget ? 1.0f : pi_budget / perceived_spending;
			//cut away low values:
			//pi_scale = std::max(0.f, pi_scale - 0.1f);
			state.world.nation_set_private_investment_effective_fraction(n, pi_scale);
			state.world.nation_set_private_investment(n, std::max(0.0f, pi_budget - pi_total * pi_scale));

			update_national_consumption(state, n, spending_scale, pi_scale);

			spent_on_construction_buffer.set(n, spending_scale * costs.construction);
		}
	}

	sanity_check(state);

	// register trade demand on goods
	concurrency::parallel_for(uint32_t(0), total_commodities, [&](uint32_t k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

		if(state.world.commodity_get_money_rgo(cid)) {
			return;
		}

		state.world.for_each_trade_route([&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
			auto origin =
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			auto actually_bought_at_origin = state.world.market_get_direct_demand_satisfaction(origin, cid);

			// reduce volume in case of low supply
			// do not reduce volume too much to avoid hardcore jumps
			current_volume = current_volume * std::max(0.999999f, actually_bought_at_origin);
			state.world.trade_route_set_volume(trade_route, cid, current_volume);

			auto absolute_volume = std::abs(current_volume);

			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);

			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			register_demand(state, origin, cid, absolute_volume, economy_reason::trade);
		});
	});

	// register trade demand on transportation labor:
	// money are paid during calculation of trade route profits and actual movement of goods
	state.world.for_each_trade_route([&](auto trade_route) {
		auto A = state.world.trade_route_get_connected_markets(trade_route, 0);
		auto B = state.world.trade_route_get_connected_markets(trade_route, 1);

		auto A_capital = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(A));
		auto B_capital = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(B));

		auto total_demanded_labor = 0.f;
		auto available_labor = std::min(
			state.world.province_get_labor_demand_satisfaction(A_capital, labor::no_education),
			state.world.province_get_labor_demand_satisfaction(B_capital, labor::no_education)
		);

		state.world.for_each_commodity([&](auto cid) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
			if(current_volume == 0.f) {
				return;
			}
			current_volume = current_volume * std::max(0.999999f, available_labor);
			state.world.trade_route_set_volume(trade_route, cid, current_volume);
			auto effect_of_scale = std::max(
				0.1f,
				1.f - std::abs(current_volume) * effect_of_transportation_scale
			);
			total_demanded_labor += std::abs(current_volume)
				* state.world.trade_route_get_distance(trade_route)
				/ trade_distance_covered_by_pair_of_workers_per_unit_of_good
				* effect_of_scale;
			assert(std::isfinite(total_demanded_labor));
		});

		state.world.province_get_labor_demand(A_capital, labor::no_education) += total_demanded_labor;
		state.world.province_get_labor_demand(B_capital, labor::no_education) += total_demanded_labor;
		assert(std::isfinite(total_demanded_labor));
		assert(std::isfinite(state.world.province_get_labor_demand(A_capital, labor::no_education)));
		assert(std::isfinite(state.world.province_get_labor_demand(B_capital, labor::no_education)));
	});

	// register demand on local transportation/accounting due to trade
	// all trade generates uneducated labor demand for goods transport locally
	// labor demand satisfaction does not set limits on transportation: it would be way too jumpy
	// we assume that 1 human could move 100 units of goods daily locally

	state.world.for_each_market([&](auto market) {
		auto capital = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(market));
		auto base_cargo_transport_demand = 0.f;
		auto soft_transport_demand_limit = state.world.market_get_max_throughput(market);

		state.world.for_each_commodity([&](auto commodity) {
			state.world.market_for_each_trade_route(market, [&](auto trade_route) {
				auto current_volume = state.world.trade_route_get_volume(trade_route, commodity);
				auto origin =
					current_volume > 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto target =
					current_volume <= 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);

				//auto sat = state.world.market_get_direct_demand_satisfaction(origin, commodity);
				base_cargo_transport_demand += std::abs(current_volume);
			});
		});

		//if(base_cargo_transport_demand > soft_transport_demand_limit) {
		//	base_cargo_transport_demand = base_cargo_transport_demand * base_cargo_transport_demand / soft_transport_demand_limit;
		//}

		state.world.province_get_labor_demand(capital, labor::no_education) += base_cargo_transport_demand / 100.f;
		assert(std::isfinite(base_cargo_transport_demand));
		assert(std::isfinite(state.world.province_get_labor_demand(capital, labor::no_education)));
		// proceed payments:
		state.world.market_get_stockpile(market, money) -=
			base_cargo_transport_demand
			* state.world.province_get_labor_demand_satisfaction(capital, labor::no_education)
			* state.world.province_get_labor_price(capital, labor::no_education);
	});

	sanity_check(state);

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

	state.world.execute_parallel_over_market([&](auto ids) {
		auto zones = state.world.market_get_zone_from_local_market(ids);
		auto market_capitals = state.world.state_instance_get_capital(zones);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(zones);
		auto capital = state.world.nation_get_capital(nations);
		auto capital_states = state.world.province_get_state_membership(capital);
		auto capital_mask = capital_states == zones;
		auto income_scale = state.world.market_get_income_scale(ids);

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
			auto stockpile_target_merchants = ve::max(0.f, ve_market_speculation_budget(state, ids, c)) / (ve_price(state, ids, c) + 1.f);

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
			auto supply_sold_ratio = ve::select(total_supply > 0.f, supply_sold / total_supply, ve::select(total_demand == 0.f, ve::fp_vector{ 0.f }, ve::fp_vector{ 1.f }));

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
						* state.inflation
						* state.world.market_get_income_scale(ids_i);
					state.world.nation_set_stockpiles(nations_i, c, national_stockpile_i - buy_from_nation);
					auto treasury = state.world.nation_get_stockpiles(nations_i, economy::money);
					state.world.nation_set_stockpiles(
						nations_i, economy::money, treasury + bought_from_nation_cost);
				}
			}, capital_mask, total_demand, national_stockpile, supply_sold_ratio, nations, ids);
		}
	});

#ifndef NDEBUG
	state.world.execute_serial_over_market([&](auto markets) {
		total_markets_income = total_markets_income + state.world.market_get_stockpile(markets, economy::money);
	});
	state.console_log("Total markets income: " + std::to_string(total_markets_income.reduce()));
#endif // !NDEBUG


	sanity_check(state);

	/*
	pay non "employed" pops (also zeros money for "employed" pops)
	*/

	auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
	auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto owners = nations::owner_of_pop(state, ids);
		auto population = state.world.nation_get_demographics(owners, demographics::total);
		auto unemployed = population - state.world.nation_get_demographics(owners, demographics::employed);
		auto capitalists = state.world.nation_get_demographics(owners, capitalists_key);
		auto aristocrats = state.world.nation_get_demographics(owners, aristocracy_key);
		auto investors = capitalists + aristocrats;

		auto provs = state.world.pop_get_province_from_pop_location(ids);
		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto income_scale = state.world.market_get_income_scale(markets);
		auto owner_spending = state.world.nation_get_spending_level(owners);

		auto pop_savings = state.world.pop_get_savings(ids);

		auto pop_of_type = state.world.pop_get_size(ids);
		auto adj_pop_of_type = pop_of_type / state.defines.alice_needs_scaling_factor;

		auto budget = state.world.nation_get_stockpiles(owners, economy::money);

		auto social_budget =
			owner_spending
			* budget
			* ve::to_float(state.world.nation_get_social_spending(owners))
			/ 100.f;

		auto investment_budget =
			owner_spending
			* budget
			* ve::to_float(state.world.nation_get_domestic_investment_spending(owners))
			/ 100.f;

		auto const p_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::pension_level);
		auto const unemp_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::unemployment_benefit);

		auto pension_ratio = ve::select(p_level > 0.f, p_level * population / (p_level * population + unemp_level * unemployed), 0.f);
		auto unemployment_ratio = ve::select(unemp_level > 0.f, unemp_level * unemployed / (p_level * population + unemp_level * unemployed), 0.f);

		// +1.f is here to avoid division by zero
		// which should never happen
		// but i want to be extra sure

		auto const pension_per_person =
			pension_ratio
			* social_budget
			/ (population + 1.f);

		auto const benefits_per_person =
			unemployment_ratio
			* social_budget
			/ (unemployed + 1.f);

		auto const payment_per_investor =
			investment_budget
			/ (investors + 1.f);

		auto const e_spending = owner_spending * ve::to_float(state.world.nation_get_education_spending(owners)) * ve::to_float(state.world.nation_get_education_spending(owners)) / 100.0f / 100.f;
		auto const m_spending = owner_spending * ve::to_float(state.world.nation_get_military_spending(owners)) * ve::to_float(state.world.nation_get_military_spending(owners)) / 100.0f / 100.0f;

		auto types = state.world.pop_get_poptype(ids);

		auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
		auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
		auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

		auto ln_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_life_needs_costs(n, pt) : 0.0f; },
				types, markets);
		auto en_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_everyday_needs_costs(n, pt) : 0.0f; },
				types, markets);
		auto lx_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_luxury_needs_costs(n, pt) : 0.0f; },
				types, markets);

		auto total_costs = ln_costs + en_costs + lx_costs;

		auto acc_e = ve::select(ln_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * ln_costs * payouts_spending_multiplier, 0.0f);
		auto acc_m = ve::select(ln_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * ln_costs * payouts_spending_multiplier, 0.0f);

		auto none_of_above = ln_types != int32_t(culture::income_type::military) &&
			ln_types != int32_t(culture::income_type::education) &&
			ln_types != int32_t(culture::income_type::administration);

		auto acc_u = ve::select(
			none_of_above,
			pension_per_person
			* pop_of_type,
			0.0f
		);

		acc_e = acc_e + ve::select(en_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * en_costs * payouts_spending_multiplier, 0.0f);
		acc_m = acc_m + ve::select(en_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * en_costs * payouts_spending_multiplier, 0.0f);

		acc_u = acc_u + ve::select(
			types == state.culture_definitions.capitalists,
			payment_per_investor
			* pop_of_type,
			0.0f
		);
		acc_u = acc_u + ve::select(
			types == state.culture_definitions.aristocrat,
			payment_per_investor
			* pop_of_type,
			0.0f
		);

		acc_e = acc_e + ve::select(lx_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * lx_costs * payouts_spending_multiplier, 0.0f);
		acc_m = acc_m + ve::select(lx_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * lx_costs * payouts_spending_multiplier, 0.0f);

		auto employment = pop_demographics::get_employment(state, ids);

		acc_u = acc_u + ve::select(
			none_of_above
			&& state.world.pop_type_get_has_unemployment(types),
			benefits_per_person
			* (pop_of_type - employment),
			0.0f
		);

		ve::fp_vector base_income{};
		if(presimulation) {
			base_income = pop_of_type * average_expected_savings * (1.f - presimulation_stage);
		} else {
			// miniscule base income to counteract deflation spiral
			base_income = pop_of_type * 0.00001f;
		}

		state.world.pop_set_savings(ids, (income_scale * state.inflation) * ((base_income + acc_u) + (acc_e + acc_m)));
#ifndef NDEBUG
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_e);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_m);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_u);
#endif
	});

	sanity_check(state);

	// updates of national purchases:
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		auto n = dcon::nation_id{ dcon::nation_id::value_base_t(i) };
		if(state.world.nation_get_owned_province_count(n) == 0)
			return;

		auto capital = state.world.nation_get_capital(n);
		auto capital_market = state.world.state_instance_get_market_from_local_market(
			state.world.province_get_state_membership(capital)
		);

		float base_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));

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
			state.world.nation_set_effective_land_spending(
				n, nations_commodity_spending * max_sp * spending_level);
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
				state.world.nation_get_stockpiles(n, c) +=
					difference * nations_commodity_spending * sat;
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
		state.world.nation_get_stockpiles(n, money) += refund;
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

	// finally we can move to production:
	// reset supply:

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.execute_serial_over_market([&](auto markets) {
			state.world.market_set_supply(markets, c, ve::fp_vector{});
			state.world.market_set_import(markets, c, ve::fp_vector{});
			state.world.market_set_export(markets, c, ve::fp_vector{});
		});
	});

	for(int32_t i = 0; i < labor::total; i++) {
		state.world.execute_serial_over_province([&](auto markets) {
			state.world.province_set_labor_supply(markets, i, 0.f);
		});
	}

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
			state.world.market_get_export(route_data.origin, cid) += route_data.amount_origin;
			state.world.market_get_import(route_data.target, cid) += route_data.amount_target;			
			state.world.market_get_stockpile(route_data.origin, economy::money) += route_data.amount_origin * route_data.payment_received_per_unit;
			state.world.market_get_stockpile(route_data.target, economy::money) -= route_data.amount_origin * route_data.payment_per_unit;

#ifndef NDEBUG
			total_trade_paid += route_data.amount_origin * route_data.payment_per_unit;
			total_merchant_cut += route_data.amount_origin * route_data.payment_received_per_unit;
#endif // !NDEBUG

			state.world.market_get_tariff_collected(route_data.origin) += route_data.tariff_origin;
			state.world.market_get_tariff_collected(route_data.target) += route_data.tariff_target;
			state.world.market_get_stockpile(route_data.target, cid) += route_data.amount_target;

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
				auto stockpile_target_merchants = ve_market_speculation_budget(state, markets, c) / (price + 1.f);
				auto local_wage_rating = state.defines.alice_needs_scaling_factor * state.world.province_get_labor_price(capitals, labor::no_education) + 0.00001f;
				auto price_rating = (ve_price(state, markets, c)) / local_wage_rating;
				auto actual_stockpile_to_supply = ve::min(1.f, stockpile_to_supply + price_rating);
				auto merchants_supply = ve::max(0.f, stockpiles - stockpile_target_merchants) * actual_stockpile_to_supply;
				state.world.market_set_supply(markets, c, state.world.market_get_supply(markets, c) + merchants_supply);
			}
		});
	});

	sanity_check(state);

	// labor supply
	{
		auto const primary_def = state.culture_definitions.primary_factory_worker;
		auto primary_key = demographics::to_key(state, primary_def);

		auto const secondary_def = state.culture_definitions.secondary_factory_worker;
		auto secondary_key = demographics::to_key(state, secondary_def);

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
				+ 0.00000001f;

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

	auto amount_of_nations = state.world.nation_size();

#ifndef NDEBUG
	float total_trade_income = 0.f;
	float total_artisan_income = 0.f;
	float total_wages = 0.f;
	float total_national_payments = 0.f;
	float total_factory_owner_income = 0.f;
	float total_rgo_owner_income = 0.f;
#endif

	// artisans production
	update_artisan_production(state);

	for(auto n : state.world.in_nation) {
		auto const min_wage_factor = pop_min_wage_factor(state, n);

		for(auto p : state.world.nation_get_province_ownership(n)) {
			auto province = p.get_province();
			auto local_state = province.get_state_membership();
			auto market = local_state.get_market_from_local_market();

			// factories production
			for(auto f : state.world.province_get_factory_location(p.get_province())) {
				update_single_factory_production(
					state,
					f.get_factory(),
					market,
					n
				);
			}

			// rgo production
			update_province_rgo_production(state, p.get_province(), market, n);

			/* adjust pop satisfaction based on consumption and subsistence */

			float subsistence = adjusted_subsistence_score(state, p.get_province());
			float subsistence_life = std::clamp(subsistence, 0.f, subsistence_score_life);
			subsistence -= subsistence_life;

			subsistence_life /= subsistence_score_life;

			for(auto pl : p.get_province().get_pop_location()) {
				auto t = pl.get_pop().get_poptype();

				auto ln = pop_demographics::get_life_needs(state, pl.get_pop());
				auto en = pop_demographics::get_everyday_needs(state, pl.get_pop());
				auto lx = pop_demographics::get_luxury_needs(state, pl.get_pop());


				// sat = raw + sub ## first summand is "raw satisfaction"
				ln -= subsistence_life;

				// as it a very rough estimation based on very rough values,
				// we have to sanitise values:
				ln = std::max(0.f, ln);
				en = std::max(0.f, en);
				lx = std::max(0.f, lx);

				assert(std::isfinite(ln));
				assert(std::isfinite(en));
				assert(std::isfinite(lx));

				ln = std::min(1.f, ln) * state.world.market_get_max_life_needs_satisfaction(market, t);
				en = std::min(1.f, en) * state.world.market_get_max_everyday_needs_satisfaction(market, t);
				lx = std::min(1.f, lx) * state.world.market_get_max_luxury_needs_satisfaction(market, t);

				ln = std::min(1.f, ln + subsistence_life);

				pop_demographics::set_life_needs(state, pl.get_pop(), ln);
				pop_demographics::set_everyday_needs(state, pl.get_pop(), en);
				pop_demographics::set_luxury_needs(state, pl.get_pop(), lx);
			}
		}


		// pay trade income:

		{
			auto const artisan_def = state.culture_definitions.artisans;
			auto artisan_key = demographics::to_key(state, artisan_def);
			auto const clerks_def = state.culture_definitions.secondary_factory_worker;
			auto clerks_key = demographics::to_key(state, clerks_def);
			auto const capis_def = state.culture_definitions.capitalists;
			auto capis_key = demographics::to_key(state, capis_def);

			for(auto si : state.world.nation_get_state_ownership(n)) {
				float total_profit = 0.f;
				float rgo_owner_profit = 0.f;

				auto sid = si.get_state();

				auto market = si.get_state().get_market_from_local_market();

				// we forgive some debt to avoid death spirals:
				// it generates a bit of money to keep the economy running
				if(market.get_stockpile(economy::money) < 0.f) {
					market.get_stockpile(economy::money) *= 0.9f;
				}

				auto income_scale = state.world.market_get_income_scale(market);
				auto trade_income = market.get_stockpile(economy::money) * 0.1f;
				if(trade_income > 0.f) {
					market.get_stockpile(economy::money) *= 0.9f;
				} else {
					continue;
				}

				auto artisans = state.world.state_instance_get_demographics(sid, artisan_key);
				auto clerks = state.world.state_instance_get_demographics(sid, clerks_key);
				auto capis = state.world.state_instance_get_demographics(sid, capis_key);

				auto artisans_weight = state.world.state_instance_get_demographics(sid, artisan_key) / 1000.f;
				auto clerks_weight = state.world.state_instance_get_demographics(sid, clerks_key) * 100.f;
				auto capis_weight = state.world.state_instance_get_demographics(sid, capis_key) * 100'000.f;

				auto total_weight = artisans_weight + clerks_weight + capis_weight;

				if(total_weight > 0 && trade_income > 0) {
					auto artisans_share = artisans_weight / total_weight * trade_income;
					auto clerks_share = clerks_weight / total_weight * trade_income;
					auto capis_share = capis_weight / total_weight * trade_income;

					auto per_artisan = 0.f;
					auto per_clerk = 0.f;
					auto per_capi = 0.f;

					if(artisans > 0.f) {
						per_artisan = artisans_share / artisans;
					}
					if(clerks > 0.f) {
						per_clerk = clerks_share / clerks;
					}
					if(capis > 0.f) {
						per_capi = capis_share / capis;
					}

					province::for_each_province_in_state_instance(state, sid, [&](dcon::province_id p) {
						for(auto pl : state.world.province_get_pop_location(p)) {
							if(artisan_def == pl.get_pop().get_poptype()) {
								pl.get_pop().set_savings(pl.get_pop().get_savings() + income_scale * state.inflation * pl.get_pop().get_size() * per_artisan);
#ifndef NDEBUG
								total_trade_income += income_scale * state.inflation * pl.get_pop().get_size() * per_artisan;
								assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
							} else if(clerks_def == pl.get_pop().get_poptype()) {
								pl.get_pop().set_savings(pl.get_pop().get_savings() + income_scale * state.inflation * pl.get_pop().get_size() * per_clerk);
#ifndef NDEBUG
								total_trade_income += income_scale * state.inflation * pl.get_pop().get_size() * per_clerk;
								assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
							} else if(capis_def == pl.get_pop().get_poptype()) {
								pl.get_pop().set_savings(pl.get_pop().get_savings() + income_scale * state.inflation * pl.get_pop().get_size() * per_capi);
#ifndef NDEBUG
								total_trade_income += income_scale * state.inflation * pl.get_pop().get_size() * per_capi;
								assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
							}
							assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
						}
					});
				}
			}
		}

		/*
		pay "employed" pops
		*/

		{
			// ARTISAN
			auto const artisan_type = state.culture_definitions.artisans;
			auto key = demographics::to_key(state, artisan_type);

			float num_artisans = state.world.nation_get_demographics(n, key);
			if(num_artisans > 0) {
				for(auto p : state.world.nation_get_province_ownership(n)) {
					auto province = p.get_province();
					auto local_state = province.get_state_membership();
					auto market = local_state.get_market_from_local_market();
					auto income_scale = state.world.market_get_income_scale(market);
					float artisan_profit = state.world.province_get_artisan_profit(province);
					auto local_artisans = state.world.province_get_demographics(province, key);
					if(local_artisans == 0.f) {
						continue;
					}
					auto per_profit = artisan_profit / num_artisans;
					for(auto pl : province.get_pop_location()) {
						if(artisan_type == pl.get_pop().get_poptype()) {
							pl.get_pop().set_savings(
								pl.get_pop().get_savings()
								+ income_scale
								* state.inflation
								* pl.get_pop().get_size()
								* per_profit
							);
#ifndef NDEBUG
							total_artisan_income += income_scale
								* state.inflation
								* pl.get_pop().get_size()
								* per_profit;
							assert(
								std::isfinite(pl.get_pop().get_savings())
								&& pl.get_pop().get_savings() >= 0
							);
#endif
						}
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					}
				}
			}
		}

		/*
		pay for provided labor / to capitalists / to rgo owners / to market
		*/

		for(auto poid : state.world.nation_get_province_ownership(n)) {
			float total_factory_profit = 0.f;
			float total_rgo_profit = 0.f;

			float rgo_owner_profit = 0.f;
			float market_profit = 0.f;

			auto pid = poid.get_province();
			auto sid = pid.get_state_membership();
			auto market = sid.get_market_from_local_market();
			auto income_scale = state.world.market_get_income_scale(market);

			float no_education_wage =
				state.world.province_get_labor_price(pid, labor::no_education)
				* state.world.province_get_labor_supply_sold(pid, labor::no_education);
			float basic_education_wage =
				state.world.province_get_labor_price(pid, labor::basic_education)
				* state.world.province_get_labor_supply_sold(pid, labor::basic_education); // craftsmen
			float high_education_wage =
				state.world.province_get_labor_price(pid, labor::high_education)
				* state.world.province_get_labor_supply_sold(pid, labor::high_education); // clerks, clergy and bureaucrats
			float guild_education_wage =
				state.world.province_get_labor_price(pid, labor::guild_education)
				* state.world.province_get_labor_supply_sold(pid, labor::guild_education); // artisans
			float high_education_and_accepted_wage =
				state.world.province_get_labor_price(pid, labor::high_education_and_accepted)
				* state.world.province_get_labor_supply_sold(pid, labor::high_education_and_accepted); // clerks, clergy and bureaucrats of accepted culture

			float sum_of_wages = no_education_wage + basic_education_wage + high_education_wage + guild_education_wage + high_education_and_accepted_wage;

			float rgo_workers_wage =
				state.world.province_get_pop_labor_distribution(pid, pop_labor::rgo_worker_no_education)
				* no_education_wage;

			float primary_workers_wage =
				state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_no_education)
				* no_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_basic_education)
				* basic_education_wage;

			float high_not_accepted_workers_wage =
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_no_education)
				* no_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_basic_education)
				* basic_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_high_education)
				* high_education_wage;

			float high_accepted_workers_wage =
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_no_education)
				* no_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_basic_education)
				* basic_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education)
				* high_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education_accepted)
				* high_education_and_accepted_wage;

			float num_aristocrat = state.world.province_get_demographics(
				pid,
				demographics::to_key(state, state.culture_definitions.aristocrat)
			);

			float payment_per_aristocrat = 0.f;
			float aristocrats_share = state.world.province_get_landowners_share(pid);
			float others_share = (1.f - aristocrats_share);
			
			// FACTORIES
			// all profits go to market stockpiles and then they are distributed to capitalists
			for(auto f : state.world.province_get_factory_location(pid)) {
				auto fac = f.get_factory();
				auto profit = explain_last_factory_profit(state, fac);
				total_factory_profit += profit.profit;
			}
			market_profit += total_factory_profit;

			// RGOS and slaves cashback
			{
				total_rgo_profit += state.world.province_get_rgo_profit(pid);
				for(auto pl : state.world.province_get_pop_location(pid)) {
					if(pl.get_pop().get_poptype() == state.culture_definitions.slaves) {
						total_rgo_profit += pl.get_pop().get_size() * rgo_workers_wage;
					}
				}
			}

			auto local_market_cut = local_market_cut_baseline - state.world.market_get_stockpile(market, economy::money) / (no_education_wage + 0.000001f) / 10'000'000.f;
			local_market_cut = std::clamp(local_market_cut, 0.f, 1.f);

			auto market_rgo_activity_cut = total_rgo_profit * local_market_cut;
			total_rgo_profit -= market_rgo_activity_cut;

			if(total_rgo_profit >= 0.f && num_aristocrat > 0.f) {
				payment_per_aristocrat += total_rgo_profit * aristocrats_share / num_aristocrat;
				market_profit += total_rgo_profit * others_share;
			} else {
				market_profit += total_rgo_profit;
			}

			state.world.market_get_stockpile(market, economy::money) += market_profit + market_rgo_activity_cut;

			auto market_cut_from_wages = 0.f;
			auto total_wage = 0.f;

			for(auto pl : state.world.province_get_pop_location(pid)) {
				auto pop = pl.get_pop();
				auto savings = pop.get_savings();
				auto poptype = pop.get_poptype();
				auto size = pop.get_size();

				auto accepted = state.world.nation_get_accepted_cultures(n, pop.get_culture())
					|| state.world.nation_get_primary_culture(n) == pop.get_culture();

				if(poptype.get_is_paid_rgo_worker()) {
					pop.set_savings(savings + income_scale * state.inflation * size * rgo_workers_wage * (1.f - local_market_cut));
					total_wage += size * rgo_workers_wage;
#ifndef NDEBUG
					total_wages += income_scale * state.inflation * size * rgo_workers_wage;
#endif
				} else if(state.culture_definitions.primary_factory_worker == poptype) {
					pop.set_savings(savings + income_scale * state.inflation * size * primary_workers_wage * (1.f - local_market_cut));
					total_wage += size * primary_workers_wage;
#ifndef NDEBUG
					total_wages += income_scale * state.inflation * size * primary_workers_wage;
					assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
#endif
				} else if(state.culture_definitions.secondary_factory_worker == pop.get_poptype()) {
					if(accepted) {
						pop.set_savings(savings + income_scale * state.inflation * size * high_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_accepted_workers_wage;
#ifndef NDEBUG
						total_wages += income_scale * state.inflation * size * high_accepted_workers_wage;
#endif
					} else {
						pop.set_savings(savings + income_scale * state.inflation * size * high_not_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_not_accepted_workers_wage;
#ifndef NDEBUG
						total_wages += income_scale * state.inflation * size * high_not_accepted_workers_wage;
#endif
					}
					assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
				} else if(pop.get_poptype() == state.culture_definitions.bureaucrat) {
					if(accepted) {
						pop.set_savings(savings + income_scale * state.inflation * size * high_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_accepted_workers_wage;
#ifndef NDEBUG
						total_wages += income_scale * state.inflation * size * high_accepted_workers_wage;
#endif
					} else {
						pop.set_savings(savings + income_scale * state.inflation * size * high_not_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_not_accepted_workers_wage;
#ifndef NDEBUG
						total_wages += income_scale * state.inflation * size * high_not_accepted_workers_wage;
#endif
					}
				} else if(pop.get_poptype() == state.culture_definitions.clergy) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + income_scale * state.inflation * size * high_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_accepted_workers_wage;
#ifndef NDEBUG
						total_wages += income_scale * state.inflation * size * high_accepted_workers_wage;
#endif
					} else {
						pop.set_savings(pop.get_savings() + income_scale * state.inflation * size * high_not_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_not_accepted_workers_wage;
#ifndef NDEBUG
						total_wages += income_scale * state.inflation * size * high_not_accepted_workers_wage;
#endif
					}
				} else if(state.culture_definitions.aristocrat == pop.get_poptype()) {
					pop.set_savings(savings + income_scale * state.inflation * size * payment_per_aristocrat);
#ifndef NDEBUG
					total_rgo_owner_income += income_scale * state.inflation * size * payment_per_aristocrat;
					assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
#endif
				}

				// all rich pops pay a "tax" to market to import expensive goods:

				if (pop.get_savings() / (size + 1) > 10.f * sum_of_wages) {
					float new_savings = pop.get_savings();
					market_cut_from_wages += new_savings * 0.5f;
					pop.set_savings(new_savings * 0.5f);
				}

				assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
			}

			market_cut_from_wages += total_wage * local_market_cut;
			state.world.market_get_stockpile(market, economy::money) += market_cut_from_wages;
		}

		/* advance construction */
		advance_construction(state, n, spent_on_construction_buffer.get(n));

		if(presimulation) {
			emulate_construction_demand(state, n);
		}

		/*
			collect and distribute money for private education and other abstracted spendings
			also rich pops pay to the market extra to import overseas goods
		*/
		auto edu_money = 0.f;
		auto adm_money = 0.f;
		auto const edu_adm_spending = 0.01f;
		auto const edu_adm_effect = 1.f - edu_adm_spending;
		auto const education_ratio = 0.8f;
		auto from_investment_pool = state.world.nation_get_private_investment(n);
		state.world.nation_set_private_investment(n, from_investment_pool * 0.999f);
		from_investment_pool *= 0.001f;
		auto payment_per_pop = from_investment_pool / (1.f + state.world.nation_get_demographics(n, demographics::total));

		for(auto p : state.world.nation_get_province_ownership(n)) {
			auto province = p.get_province();
			if(state.world.province_get_nation_from_province_ownership(province) == state.world.province_get_nation_from_province_control(province)) {
				float current = 0.f;
				float local_teachers = 0.f;
				float local_managers = 0.f;
				for(auto pl : province.get_pop_location()) {

					auto pop = pl.get_pop();
					auto pt = pop.get_poptype();
					auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
					if(ln_type == culture::income_type::administration) {
						local_managers += pop.get_size();
					} else if(ln_type == culture::income_type::education) {
						local_teachers += pop.get_size();
					}

					// services/bribes/charity and other nonsense
					auto const pop_money = pop.get_savings();
					pop.set_savings(pop_money + payment_per_pop);
				}
				if(local_teachers + local_managers > 0.f) {
					for(auto pl : province.get_pop_location()) {
						auto const pop_money = pl.get_pop().get_savings();
						current += pop_money * edu_adm_spending;
						pl.get_pop().set_savings(pop_money * edu_adm_effect);
						assert(std::isfinite(pl.get_pop().get_savings()));
					}
				}
				float local_education_ratio = education_ratio;
				if(local_managers == 0.f) {
					local_education_ratio = 1.f;
				}
				for(auto pl : province.get_pop_location()) {
					auto pop = pl.get_pop();
					auto pt = pop.get_poptype();
					auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
					if(ln_type == culture::income_type::administration) {
						float ratio = (local_managers > 0.f) ? pop.get_size() / local_managers : 0.f;
						pop.set_savings(pop.get_savings() + current * (1.f - local_education_ratio) * ratio);
						assert(std::isfinite(pop.get_savings()));
						adm_money += current * (1.f - local_education_ratio) * ratio;
					} else if(ln_type == culture::income_type::education) {
						float ratio = (local_teachers > 0.f) ? pop.get_size() / local_teachers : 0.f;
						pop.set_savings(pop.get_savings() + current * local_education_ratio * ratio);
						assert(std::isfinite(pop.get_savings()));
						edu_money += current * local_education_ratio * ratio;
					}
				}
			}
		}
		state.world.nation_set_private_investment_education(n, edu_money);
		state.world.nation_set_private_investment_administration(n, adm_money);

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
				float total_profit = 0.f;
				float rgo_owner_profit = 0.f;

				auto market = si.get_state().get_market_from_local_market();
				auto capital = si.get_state().get_capital();
				if(capital.get_nation_from_province_control() == n) {
					t_total += state.world.market_get_tariff_collected(market);
				}
				state.world.market_set_tariff_collected(market, 0.f);
			}

			assert(std::isfinite(t_total));
			assert(t_total >= 0);
			state.world.nation_get_stockpiles(n, money) += t_total;
		}

		// shift needs weights
		for(auto si : state.world.nation_get_state_ownership(n)) {
			float total_profit = 0.f;
			float rgo_owner_profit = 0.f;

			auto market = si.get_state().get_market_from_local_market();
			rebalance_needs_weights(state, market);
		}
	};

#ifndef NDEBUG
	state.console_log("Total pops trade income: " + std::to_string(total_trade_income));
	state.console_log("Total artisans income: " + std::to_string(total_artisan_income));
	state.console_log("Total wages income: " + std::to_string(total_wages));
	state.console_log("Total capitalists income: " + std::to_string(total_factory_owner_income));
	state.console_log("Total rgo owners income: " + std::to_string(total_rgo_owner_income));
#endif

	sanity_check(state);

	/*
	adjust prices based on global production & consumption
	*/
	state.world.execute_serial_over_market([&](auto ids) {
		auto costs = state.world.market_get_everyday_needs_costs(ids, state.culture_definitions.laborers);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			if(!state.world.commodity_get_money_rgo(c))
				return;
			state.world.market_set_price(ids, c, ve::min(costs * 10.f, state.world.commodity_get_cost(c) * 0.1f));
		});
	});


	// price of labor

	for(int32_t i = 0; i < labor::total; i++) {
		state.world.execute_serial_over_province([&](auto ids) {
			ve::fp_vector supply =
				state.world.province_get_labor_supply(ids, i)
				+ price_rigging;
			ve::fp_vector demand =
				state.world.province_get_labor_demand(ids, i)
				+ price_rigging;

			auto current_price = state.world.province_get_labor_price(ids, i);
			auto oversupply_factor = ve::max(supply / demand - 1.f, 0.f);
			auto overdemand_factor = ve::max(demand / supply - 1.f, 0.f);
			auto speed_modifer = (overdemand_factor - oversupply_factor);
			auto price_speed = ve::min(ve::max(price_speed_mod * speed_modifer, -0.025f), 0.025f);
			price_speed = price_speed * current_price;
			current_price = current_price + price_speed;

			auto nids = state.world.province_get_nation_from_province_ownership(ids);
			auto sids = state.world.province_get_state_membership(ids);
			auto mids = state.world.state_instance_get_market_from_local_market(sids);
			auto min_wage_factor = ve_pop_min_wage_factor(state, nids);

			// labor price control

			ve::fp_vector price_control = ve::fp_vector{ 0.f };
			price_control = price_control + state.world.market_get_life_needs_costs(mids, state.culture_definitions.secondary_factory_worker) * 1.2f;
			price_control = price_control + state.world.market_get_everyday_needs_costs(mids, state.culture_definitions.secondary_factory_worker) * 0.1f;

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
			state.world.province_set_labor_price(ids, i, ve::min(ve::max(current_price, ve::max(0.00001f, price_control)), 1'000'000'000'000.f));
		});
	}

	state.world.execute_serial_over_market([&](auto ids) {
		concurrency::parallel_for(uint32_t(1), total_commodities, [&](uint32_t k) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

			//handling gold cost separetely
			if(state.world.commodity_get_money_rgo(cid)) {
				return;
			}

			// dirty hack ...
			// for now
			// ideally simulation should stop demanding goods
			// when they are way too expensive

			ve::fp_vector supply =
				state.world.market_get_supply(ids, cid)
				+ price_rigging * 3.f; // while it's logical to have insane prices on high tech goods to encourage their production, it's more healthy for simulation to have them moderately low
			ve::fp_vector demand =
				state.world.market_get_demand(ids, cid)
				+ price_rigging;

			auto current_price = ve_price(state, ids, cid);
			auto oversupply_factor = ve::max(supply / demand - 1.f, 0.f);
			auto overdemand_factor = ve::max(demand / supply - 1.f, 0.f);
			auto speed_modifer = (overdemand_factor - oversupply_factor);
			auto price_speed = ve::min(ve::max(price_speed_mod * speed_modifer, -0.025f), 0.025f);
			price_speed = price_speed * current_price;
			current_price = current_price + price_speed;

#ifndef NDEBUG
			ve::apply([&](auto value) { assert(std::isfinite(value)); }, current_price);
#endif

			//the only purpose of upper price bound is to prevent float overflow
			state.world.market_set_price(ids, cid, ve::min(ve::max(current_price, 0.001f), 1'000'000'000'000.f));
		});
	});

	sanity_check(state);

	if(state.cheat_data.ecodump) {
		float accumulator[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

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
			state.world.nation_get_stockpiles(n, money) -= transferamt;
			state.world.nation_get_stockpiles(overlord, money) += transferamt;
		}

		for(auto uni : n.get_unilateral_relationship_as_source()) {
			if(uni.get_war_subsidies()) {
				auto sub_size = estimate_war_subsidies(state, uni.get_target(), uni.get_source());

				if(sub_size <= n.get_stockpiles(money)) {
					n.get_stockpiles(money) -= sub_size;
					uni.get_target().get_stockpiles(money) += sub_size;
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

				n.get_stockpiles(money) -= capped_payout;
				uni.get_target().get_stockpiles(money) += capped_payout;
			}
		}
	}

	sanity_check(state);

	/*
	update inflation
	*/

	// control over money mass is two-step
	// firstly we control global amount of money with "inflation": global income scaler
	// secondly we control local amount of money with local income scalers
	// we want to smooth out absurd "spikes"
	// in per capita money distributions of markets

	// essentially upper bound on wealth in the system
	state.inflation = 0.999f;

	float total_pop = 0.0f;
	float total_pop_money = 0.0f;
	state.world.for_each_nation([&](dcon::nation_id n) {
		total_pop += state.world.nation_get_demographics(n, demographics::total);
		total_pop_money +=
			state.world.nation_get_total_rich_income(n)
			+ state.world.nation_get_total_middle_income(n)
			+ state.world.nation_get_total_poor_income(n);
	});
	float target_money = total_pop * average_expected_savings;

	// prevent runaway savings of some pops

	//if(target_money < total_pop_money)
	//	state.inflation = 0.99f;

	state.world.for_each_market([&](auto ids) {
		auto sid = state.world.market_get_zone_from_local_market(ids);
		auto local_pop = state.world.state_instance_get_demographics(sid, demographics::total);
		float target_money = local_pop * average_expected_savings;

		float local_money = 0.f;
		province::for_each_province_in_state_instance(state, sid, [&](auto pid) {
			state.world.province_for_each_pop_location(pid, [&](auto pop_location) {
				auto pop = state.world.pop_location_get_pop(pop_location);
				local_money += state.world.pop_get_savings(pop);
			});
		});

		float target_scale = target_money / (local_money + 0.0001f);
		float prev_scale = state.world.market_get_income_scale(ids);
		//state.world.market_set_income_scale(ids, (target_scale + prev_scale) / 2.f);
		state.world.market_set_income_scale(ids, ve::fp_vector{ 1.f });
	});

	sanity_check(state);

	// make constructions:
	resolve_constructions(state);

	if(!presimulation) {
		run_private_investment(state);
	}

	sanity_check(state);

	//write gdp and total savings to file
	if(state.cheat_data.ecodump) {
		/* Too expensive to write unconditionally
		for(auto si : state.world.in_state_instance) {
			auto market = si.get_market_from_local_market();
			auto nation = si.get_nation_from_state_ownership();

			auto life_costs =
				state.world.market_get_life_needs_costs(
					market, state.culture_definitions.primary_factory_worker)
				+ state.world.market_get_everyday_needs_costs(
					market, state.culture_definitions.primary_factory_worker)
				+ state.world.market_get_luxury_needs_costs(
					market, state.culture_definitions.primary_factory_worker);

			auto tag = nations::int_to_tag(
				state.world.national_identity_get_identifying_int(
					state.world.nation_get_identity_from_identity_holder(nation)
				)
			);
			auto state_name = text::produce_simple_string(
				state,
				state.world.state_definition_get_name(
					si.get_definition()
				)
			);
			auto name = text::produce_simple_string(state, text::get_name(state, nation));
			state.cheat_data.national_economy_dump_buffer +=
				tag + ","
				+ name + ","
				+ state_name + ","
				+ std::to_string(state.world.market_get_gdp(market)) + ","
				+ std::to_string(life_costs) + ","
				+ std::to_string(state.world.state_instance_get_demographics(si, demographics::total)) + ","
				+ std::to_string(state.current_date.value) + "\n";
		}
		*/

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
				state.cheat_data.savings_buffer += text::produce_simple_string(
					state,
					state.world.pop_type_get_name(pop_type)
				);
				state.cheat_data.savings_buffer += ";";
			});

			state.cheat_data.savings_buffer += "markets;nations;investments\n";
		} else {
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

tariff_data explain_trade_route(sys::state& state, dcon::trade_route_id trade_route) {
	auto m0 = state.world.trade_route_get_connected_markets(trade_route, 0);
	auto m1 = state.world.trade_route_get_connected_markets(trade_route, 1);
	auto s0 = state.world.market_get_zone_from_local_market(m0);
	auto s1 = state.world.market_get_zone_from_local_market(m1);
	auto n0 = state.world.state_instance_get_nation_from_state_ownership(s0);
	auto n1 = state.world.state_instance_get_nation_from_state_ownership(s1);

	bool same_nation = n0 == n1;

	auto capital_0 = state.world.state_instance_get_capital(s0);
	auto capital_1 = state.world.state_instance_get_capital(s1);

	auto controller_capital_0 = state.world.province_get_nation_from_province_control(capital_0);
	auto controller_capital_1 = state.world.province_get_nation_from_province_control(capital_1);

	auto sphere_0 = state.world.nation_get_in_sphere_of(controller_capital_0);
	auto sphere_1 = state.world.nation_get_in_sphere_of(controller_capital_1);

	auto overlord_0 = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(sphere_0)
	);
	auto overlord_1 = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(sphere_1)
	);

	auto applies_tariffs_0 = true;
	auto applies_tariffs_1 = true;
	auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n1, n0);
	auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n0, n1);
	if(source_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
		if(state.current_date < enddt) {
			applies_tariffs_0 = false;
		}
	}
	if(target_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
		if(state.current_date < enddt) {
			applies_tariffs_1 = false;
		}
	}

	auto is_open_0_to_1 = sphere_0 == controller_capital_1 || overlord_0 == controller_capital_1 || !applies_tariffs_0;
	auto is_open_1_to_0 = sphere_1 == controller_capital_0 || overlord_1 == controller_capital_0 || !applies_tariffs_1;

	auto distance = state.world.trade_route_get_distance(trade_route);
	auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);

	return {
		.applies_tariff = {is_open_0_to_1, is_open_1_to_0},
		.export_tariff = {effective_tariff_export_rate(state, n0, m0), effective_tariff_export_rate(state, n1, m1) },
		.import_tariff = {effective_tariff_import_rate(state, n0, m0), effective_tariff_import_rate(state, n1, m1) },
		.distance = distance,
		.loss = trade_good_loss_mult,
		.base_distance_cost = distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good
		* (
			state.world.province_get_labor_price(capital_0, labor::no_education)
			+ state.world.province_get_labor_price(capital_1, labor::no_education)
		),
		.workers_satisfaction = std::min(
			state.world.province_get_labor_demand_satisfaction(capital_0, labor::no_education),
			state.world.province_get_labor_demand_satisfaction(capital_1, labor::no_education)
		)
	};
}

// CAUTION: when we generate trade demand for a good, we promise to pay money to local producers during the next tick
// it means that during economy update, rgo profits were calculated according to this promise
// so local producers ALREADY received money for their production and local market take only a "merchant cut"

trade_and_tariff explain_trade_route_commodity(
	sys::state& state,
	dcon::trade_route_id trade_route,
	tariff_data& additional_data,
	dcon::commodity_id cid
) {
	auto current_volume = state.world.trade_route_get_volume(trade_route, cid);

	auto origin_i = current_volume > 0.f ? 0 : 1;
	auto target_i = current_volume > 0.f ? 1 : 0;
	auto origin = state.world.trade_route_get_connected_markets(trade_route, origin_i);
	auto target = state.world.trade_route_get_connected_markets(trade_route, target_i);
	auto s_origin = state.world.market_get_zone_from_local_market(origin);
	auto s_target = state.world.market_get_zone_from_local_market(target);
	auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
	auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
	auto capital_origin = state.world.state_instance_get_capital(s_origin);
	auto capital_target = state.world.state_instance_get_capital(s_target);
	bool same_nation = n_origin == n_target;

	auto origin_is_open_to_target = additional_data.applies_tariff[origin_i];
	auto target_is_open_to_origin = additional_data.applies_tariff[target_i];

	auto price_origin = price(state, origin, cid);
	auto price_target = price(state, target, cid);

	auto sat =
		state.world.market_get_direct_demand_satisfaction(origin, cid);
		/** std::min(1.f, state.world.market_get_supply_sold_ratio(target, cid)
		+ 100.f / (price(state, origin, cid) + price(state, target, cid))
		);*/

	auto absolute_volume = /*additional_data.workers_satisfaction * */ sat * std::abs(current_volume);

	auto import_amount = absolute_volume * additional_data.loss;
	auto effect_of_scale = std::max(0.1f, 1.f - absolute_volume * effect_of_transportation_scale);
	auto transport_cost = additional_data.base_distance_cost * effect_of_scale;
	auto export_tariff = origin_is_open_to_target ? 0.f : additional_data.export_tariff[origin_i];
	auto import_tariff = target_is_open_to_origin ? 0.f : additional_data.import_tariff[target_i];

	if(same_nation) {
		return {
			.origin = origin,
			.target = target,
			.origin_nation = n_origin,
			.target_nation = n_target,

			.amount_origin = absolute_volume,
			.amount_target = import_amount,

			.tariff_origin = 0.f,
			.tariff_target = 0.f,

			.tariff_rate_origin = 0.f,
			.tariff_rate_target = 0.f,

			.price_origin = price_origin,
			.price_target = price_target,

			.transport_cost = transport_cost,
			.transportaion_loss = additional_data.loss,
			.distance = additional_data.distance,

			.payment_per_unit = price_origin * (1 + economy::merchant_cut_domestic) + transport_cost,
			// the rest of payment is handled as satisfaction of generic demand
			.payment_received_per_unit = price_origin * economy::merchant_cut_domestic
		};
	} else {
		return {
			.origin = origin,
			.target = target,
			.origin_nation = n_origin,
			.target_nation = n_target,

			.amount_origin = absolute_volume,
			.amount_target = import_amount,
			.tariff_origin = absolute_volume * price_origin * export_tariff,
			.tariff_target = import_amount * price_target * import_tariff,

			.tariff_rate_origin = export_tariff,
			.tariff_rate_target = import_tariff,

			.price_origin = price_origin,
			.price_target = price_target,

			.transport_cost = transport_cost,
			.transportaion_loss = additional_data.loss,
			.distance = additional_data.distance,

			.payment_per_unit = price_origin
				* (1.f + export_tariff)
				* (1 + economy::merchant_cut_foreign)
				+ price(state, target, cid)
				* import_tariff
				+ transport_cost,
			// the rest of payment is handled as satisfaction of generic demand
			.payment_received_per_unit = price_origin * economy::merchant_cut_foreign
		};
	}
}



trade_and_tariff explain_trade_route_commodity(sys::state& state, dcon::trade_route_id trade_route, dcon::commodity_id cid) {
	auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
	auto origin =
		current_volume > 0.f
		? state.world.trade_route_get_connected_markets(trade_route, 0)
		: state.world.trade_route_get_connected_markets(trade_route, 1);
	auto target =
		current_volume <= 0.f
		? state.world.trade_route_get_connected_markets(trade_route, 0)
		: state.world.trade_route_get_connected_markets(trade_route, 1);

	auto s_origin = state.world.market_get_zone_from_local_market(origin);
	auto s_target = state.world.market_get_zone_from_local_market(target);
	auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
	auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
	bool same_nation = n_origin == n_target;
	auto capital_origin = state.world.state_instance_get_capital(s_origin);
	auto capital_target = state.world.state_instance_get_capital(s_target);
	auto controller_capital_origin = state.world.province_get_nation_from_province_control(capital_origin);
	auto controller_capital_target = state.world.province_get_nation_from_province_control(capital_target);
	auto sphere_origin = state.world.nation_get_in_sphere_of(controller_capital_origin);
	auto sphere_target = state.world.nation_get_in_sphere_of(controller_capital_target);

	auto overlord_origin = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(controller_capital_origin)
	);
	auto overlord_target = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(controller_capital_target)
	);

	// Equal/unequal trade agreements
	// Rel source if obliged towards target
	auto source_applies_tariffs = true;
	auto target_applies_tariffs = true;
	auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_target, n_origin);
	auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_origin, n_target);
	if(source_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
		if(state.current_date < enddt) {
			source_applies_tariffs = false;
		}
	}
	if(target_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
		if(state.current_date < enddt) {
			target_applies_tariffs = false;
		}
	}

	auto origin_is_open_to_target = sphere_origin == controller_capital_target || overlord_origin == controller_capital_target || !source_applies_tariffs;
	auto target_is_open_to_origin = sphere_target == controller_capital_origin || overlord_target == controller_capital_origin || !target_applies_tariffs;

	auto sat =
		state.world.market_get_direct_demand_satisfaction(origin, cid);
		//* std::min(1.f, state.world.market_get_supply_sold_ratio(target, cid)
		//+ 100.f / (price(state, origin, cid) + price(state, target, cid)));

	auto absolute_volume =
		/* std::min(
			state.world.province_get_labor_demand_satisfaction(capital_origin, labor::no_education),
			state.world.province_get_labor_demand_satisfaction(capital_target, labor::no_education)
		) * */ sat
		* std::abs(current_volume);
	auto distance = state.world.trade_route_get_distance(trade_route);

	auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);
	auto import_amount = absolute_volume * trade_good_loss_mult;

	auto effect_of_scale = std::max(0.1f, 1.f - absolute_volume * effect_of_transportation_scale);
	auto transport_cost =
		distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good
		* (
			state.world.province_get_labor_price(capital_origin, labor::no_education)
			+ state.world.province_get_labor_price(capital_target, labor::no_education)
		)
		* effect_of_scale;

	auto export_tariff = origin_is_open_to_target ? 0.f : effective_tariff_export_rate(state, n_origin, origin);
	auto import_tariff = target_is_open_to_origin ? 0.f : effective_tariff_import_rate(state, n_target, target);

	auto price_origin = price(state, origin, cid);
	auto price_target = price(state, target, cid);

	if(same_nation) {
		return {
			.origin = origin,
			.target = target,
			.origin_nation = n_origin,
			.target_nation = n_target,

			.amount_origin = absolute_volume,
			.amount_target = import_amount,

			.tariff_origin = 0.f,
			.tariff_target = 0.f,

			.tariff_rate_origin = 0.f,
			.tariff_rate_target = 0.f,

			.price_origin = price_origin,
			.price_target = price_target,

			.transport_cost = transport_cost,
			.transportaion_loss = trade_good_loss_mult,
			.distance = distance,

			.payment_per_unit = price_origin * (1 + economy::merchant_cut_domestic) + transport_cost,
			// the rest of payment is handled as satisfaction of generic demand
			.payment_received_per_unit = price_origin * economy::merchant_cut_domestic
		};
	} else {
		return {
			.origin = origin,
			.target = target,
			.origin_nation = n_origin,
			.target_nation = n_target,

			.amount_origin = absolute_volume,
			.amount_target = import_amount,
			.tariff_origin = absolute_volume * price_origin * export_tariff,
			.tariff_target = import_amount * price_target * import_tariff,

			.tariff_rate_origin = export_tariff,
			.tariff_rate_target = import_tariff,

			.price_origin = price_origin,
			.price_target = price_target,

			.transport_cost = transport_cost,
			.transportaion_loss = trade_good_loss_mult,
			.distance = distance,

			.payment_per_unit = price_origin
				* (1.f + export_tariff)
				* (1 + economy::merchant_cut_foreign)
				+ price(state, target, cid)
				* import_tariff
				+ transport_cost,
			// the rest of payment is handled as satisfaction of generic demand
			.payment_received_per_unit = price_origin * economy::merchant_cut_foreign
		};
	}
}

// DO NOT USE OUTSIDE OF UI
std::vector<trade_breakdown_item> explain_national_tariff(sys::state& state, dcon::nation_id n, bool import_flag, bool export_flag) {
	std::vector<trade_breakdown_item> result;
	auto buffer_volume_per_nation = state.world.nation_make_vectorizable_float_buffer();
	auto buffer_tariff_per_nation = state.world.nation_make_vectorizable_float_buffer();

	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		state.world.execute_serial_over_nation([&](auto nids) {
			buffer_volume_per_nation.set(nids, 0.f);
			buffer_tariff_per_nation.set(nids, 0.f);
		});

		state.world.nation_for_each_state_ownership(n, [&](auto sid) {
			auto mid = state.world.state_instance_get_market_from_local_market(state.world.state_ownership_get_state(sid));
			state.world.market_for_each_trade_route(mid, [&](auto trade_route) {
				trade_and_tariff route_data = explain_trade_route_commodity(state, trade_route, cid);

				if(!export_flag) {
					if(route_data.origin == mid) {
						return;
					}
				}

				if(!import_flag) {
					if(route_data.target == mid) {
						return;
					}
				}

				if(import_flag && route_data.target == mid) {
					buffer_volume_per_nation.get(route_data.origin_nation) += route_data.amount_target;
					buffer_tariff_per_nation.get(route_data.origin_nation) += route_data.tariff_target;
				}

				if(export_flag && route_data.origin == mid) {
					buffer_volume_per_nation.get(route_data.target_nation) += route_data.amount_origin;
					buffer_tariff_per_nation.get(route_data.target_nation) += route_data.tariff_origin;
				}
			});
		});

		state.world.for_each_nation([&](auto nid) {
			trade_breakdown_item item = {
				.trade_partner = nid,
				.commodity = cid,
				.traded_amount = buffer_volume_per_nation.get(nid),
				.tariff = buffer_tariff_per_nation.get(nid)
			};

			if(item.traded_amount == 0.f || item.tariff < 0.001f) {
				return;
			}

			result.push_back(item);
		});
	});

	return result;
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
	auto total = 0.f;
	auto base_budget = state.world.nation_get_stockpiles(n, economy::money);
	auto const p_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level));
	auto const unemp_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit));

	if(p_level + unemp_level == 0.f) {
		return 0.f;
	}

	auto social_budget = base_budget * float(state.world.nation_get_social_spending(n)) / 100.0f;

	return social_budget;
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

	return total > 0.0f ? purchased / total : 0.0f;
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

void add_factory_level_to_province(sys::state& state, dcon::province_id p, dcon::factory_type_id t, bool is_upgrade) {

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

	for(auto c : state.world.in_province_land_construction) {
		auto pop = state.world.province_land_construction_get_pop(c);
		auto province = state.world.pop_get_province_from_pop_location(pop);
		float cost_factor = economy::build_cost_multiplier(state, province, false);

		auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
		auto& current_purchased = c.get_purchased_goods();
		float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

		bool all_finished = true;
		if(!(c.get_nation().get_is_player_controlled() && state.cheat_data.instant_army)) {
			for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
				if(base_cost.commodity_type[j]) {
					if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * cost_factor) {
						all_finished = false;
					}
				} else {
					break;
				}
			}
		}

		if(all_finished) {
			auto pop_location = c.get_pop().get_province_from_pop_location();

			auto new_reg = military::create_new_regiment(state, c.get_nation(), c.get_type());
			auto a = fatten(state.world, state.world.create_army());

			a.set_controller_from_army_control(c.get_nation());
			state.world.try_create_army_membership(new_reg, a);
			state.world.try_create_regiment_source(new_reg, c.get_pop());
			military::army_arrives_in_province(state, a, pop_location, military::crossing_type::none);
			military::move_land_to_merge(state, c.get_nation(), a, pop_location, c.get_template_province());

			if(c.get_nation() == state.local_player_nation) {
				notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "amsg_army_built");
					},
					"amsg_army_built",
					state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
					sys::message_base_type::army_built
				});
			}

			state.world.delete_province_land_construction(c);
		}
	}

	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() != rng.end()) {
			auto c = *(rng.begin());

			auto province = state.world.province_naval_construction_get_province(c);
			float cost_factor = economy::build_cost_multiplier(state, province, false);

			auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

			bool all_finished = true;
			if(!(c.get_nation().get_is_player_controlled() && state.cheat_data.instant_navy)) {
				for(uint32_t i = 0; i < commodity_set::set_size && all_finished; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * cost_factor) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
			}

			if(all_finished) {
				auto new_ship = military::create_new_ship(state, c.get_nation(), c.get_type());
				auto a = fatten(state.world, state.world.create_navy());
				a.set_controller_from_navy_control(c.get_nation());
				a.set_location_from_navy_location(p);
				state.world.try_create_navy_membership(new_ship, a);
				military::move_navy_to_merge(state, c.get_nation(), a, c.get_province(), c.get_template_province());

				if(c.get_nation() == state.local_player_nation) {
					notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "amsg_navy_built");
						},
						"amsg_navy_built",
						state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::navy_built
					});
				}

				state.world.delete_province_naval_construction(c);
			}
		}
	});

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
				state.world.province_get_building_level(for_province, uint8_t(t)) += 1;

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
					state, c.get_province(), type,
					state.world.factory_construction_get_is_upgrade(c)
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
				add_factory_level_to_province(state, state.world.factory_construction_get_province(c), type,
						state.world.factory_construction_get_is_upgrade(c));

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
	auto n = state.world.state_instance_get_nation_from_state_ownership(s);
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
	auto d = state.world.state_instance_get_definition(s);
	for(auto f : state.world.province_get_factory_location(province)) {
		++num_factories;
		if(f.get_factory().get_building_type() == t)
			return; // can't build another of this type
	}

	if(num_factories < int32_t(state.defines.factories_per_state)) {
		add_factory_level_to_province(state, province, t, false);
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
			tariff = int8_t(std::clamp(std::clamp(int32_t(tariff), min_tariff, max_tariff), 0, 100));
		}

		{
			auto& tariff = state.world.nation_get_tariffs_export(n);
			tariff = int8_t(std::clamp(std::clamp(int32_t(tariff), min_tariff, max_tariff), 0, 100));
		}
	}
	{
		auto min_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tax));
		auto max_tax = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_tax));
		if(max_tax <= 0)
			max_tax = 100;
		max_tax = std::max(min_tax, max_tax);

		auto& ptax = state.world.nation_get_poor_tax(n);
		ptax = int8_t(std::clamp(std::clamp(int32_t(ptax), min_tax, max_tax), 0, 100));
		auto& mtax = state.world.nation_get_middle_tax(n);
		mtax = int8_t(std::clamp(std::clamp(int32_t(mtax), min_tax, max_tax), 0, 100));
		auto& rtax = state.world.nation_get_rich_tax(n);
		rtax = int8_t(std::clamp(std::clamp(int32_t(rtax), min_tax, max_tax), 0, 100));
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
		v = int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_social_spending));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_social_spending));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_social_spending(n);
		v = int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100));
	}
	{
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_domestic_investment));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_domestic_investment));
		if(max_spend <= 0)
			max_spend = 100;
		max_spend = std::max(min_spend, max_spend);

		auto& v = state.world.nation_get_domestic_investment_spending(n);
		v = int8_t(std::clamp(std::clamp(int32_t(v), min_spend, max_spend), 0, 100));
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

	debt = 0.0f;
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

bool do_resource_potentials_allow_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type) {
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

bool do_resource_potentials_allow_upgrade(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type) {
	/* If mod uses Factory Province limits */
	auto output = state.world.factory_type_get_output(type);
	auto limit = economy::calculate_province_factory_limit(state, location, output);

	if(!output.get_uses_potentials()) {
		return true;
	}

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

bool do_resource_potentials_allow_refit(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id from, dcon::factory_type_id refit_target) {
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
		if(f.get_factory().get_building_type() == from) {
			existing_levels += f.get_factory().get_size();
		}
		if(existing_levels + refit_levels > limit) {
			return false;
		}
	}

	return true;
}

} // namespace economy
