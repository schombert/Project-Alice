#include "economy.hpp"
#include "economy_templates.hpp"
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

enum class economy_reason {
	pop, factory, rgo, artisan, construction, nation, stockpile, overseas_penalty
};

std::string_view reason_to_string(economy_reason reason) {
	switch(reason) {
	case economy::economy_reason::pop:
		return "POP";
	case economy::economy_reason::factory:
		return "FACTORY";
	case economy::economy_reason::rgo:
		return "RGO";
	case economy::economy_reason::artisan:
		return "ARTISAN";
	case economy::economy_reason::construction:
		return "CONSTRUCTION";
	case economy_reason::nation:
		return "NATION";
	case economy_reason::stockpile:
		return "STOCKPILE";
	case economy_reason::overseas_penalty:
		return "OVERSEAS PENALTY";
	default:
		return "UNKNOWN";
		break;
	}
}

void register_demand(sys::state& state, dcon::nation_id n, dcon::commodity_id commodity_type, float amount, economy_reason reason) {
	state.world.nation_get_real_demand(n, commodity_type) += amount;
	assert(std::isfinite(state.world.nation_get_real_demand(n, commodity_type)));
}

template void for_each_new_factory<std::function<void(new_factory)>>(sys::state&, dcon::state_instance_id, std::function<void(new_factory)>&&);
template void for_each_upgraded_factory<std::function<void(upgraded_factory)>>(sys::state&, dcon::state_instance_id, std::function<void(upgraded_factory)>&&);

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
	auto debt = state.world.nation_get_stockpiles(n, money);
	if(debt >= 0)
		return 0.0f;

	return -debt * std::max(0.01f, (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::loan_interest) + 1.0f) * state.defines.loan_base_interest) / 30.0f;
}
float max_loan(sys::state& state, dcon::nation_id n) {
	/*
	There is an income cap to how much may be borrowed, namely: define:MAX_LOAN_CAP_FROM_BANKS x (national-modifier-to-max-loan-amount + 1) x national-tax-base.
	*/
	auto mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_loan_modifier) + 1.0f);
	auto total_tax_base = state.world.nation_get_total_rich_income(n) + state.world.nation_get_total_middle_income(n) + state.world.nation_get_total_poor_income(n);
	return std::max(0.0f, total_tax_base * mod);
}

int32_t most_recent_price_record_index(sys::state& state) {
	return (state.current_date.value >> 4) % price_history_length;
}
int32_t previous_price_record_index(sys::state& state) {
	return ((state.current_date.value >> 4) + price_history_length - 1) % price_history_length;
}

float full_spending_cost(sys::state& state, dcon::nation_id n,
		ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices);
void populate_army_consumption(sys::state& state);
void populate_navy_consumption(sys::state& state);
void populate_construction_consumption(sys::state& state);

float commodity_daily_production_amount(sys::state& state, dcon::commodity_id c) {
	return state.world.commodity_get_total_production(c);
}

float stockpile_commodity_daily_increase(sys::state& state, dcon::commodity_id c, dcon::nation_id n) {
	// TODO
	return 0.f;
}

float global_market_commodity_daily_increase(sys::state& state, dcon::commodity_id c) {
	// TODO
	return 0.f;
}

bool has_factory(sys::state const& state, dcon::state_instance_id si) {
	auto sdef = state.world.state_instance_get_definition(si);
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	auto crng = state.world.state_instance_get_state_building_construction(si);
	if(crng.begin() != crng.end())
		return true;

	for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			auto rng = p.get_province().get_factory_location();
			if(rng.begin() != rng.end())
				return true;
		}
	}
	return false;
}



void initialize_artisan_distribution(sys::state& state) {
	state.world.nation_resize_artisan_distribution(state.world.commodity_size());
	state.world.nation_resize_artisan_actual_production(state.world.commodity_size());

	auto const csize = state.world.commodity_size();

	for(auto n : state.world.in_nation) {
		float total = 0.0f;
		for(uint32_t i = 1; i < csize; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto kf = state.world.commodity_get_key_factory(cid);

			if(state.world.commodity_get_artisan_output_amount(cid) > 0.0f && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {

				total += 1.0f;
			}
		}
		assert(total > 0.0f);
		for(uint32_t i = 1; i < csize; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto kf = state.world.commodity_get_key_factory(cid);

			if(state.world.commodity_get_artisan_output_amount(cid) > 0.0f && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {

				n.set_artisan_distribution(cid, 1.0f / total);
			}
		}
	}
}


void initialize_needs_weights(sys::state& state, dcon::nation_id n) {
	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_life_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto& w = state.world.nation_get_life_needs_weights(n, c);
				w = 1.f;
			}
		});
	}

	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_everyday_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto& w = state.world.nation_get_everyday_needs_weights(n, c);
				w = 1.f;
			}
		});
	}

	{
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_luxury_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto& w = state.world.nation_get_luxury_needs_weights(n, c);
				w = 1.f;
			}
		});
	}
}

float need_weight(
	sys::state& state,
	dcon::nation_id n,
	dcon::commodity_id c,
	ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices
) {
	return 1 / math::sqrt(effective_prices.get(c));
}

void rebalance_needs_weights(
	sys::state& state,
	dcon::nation_id n,
	ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices
) {
	constexpr float need_drift_speed = 0.1f;

	{
		float total_weights = 0.0f;
		uint32_t count = 0;

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_life_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto weight = need_weight(state, n, c, effective_prices);
				total_weights += weight;
				count++;
			}
		});

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_life_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto weight = need_weight(state, n, c, effective_prices);
				auto ideal_weighting = weight / total_weights * count;
				auto& w = state.world.nation_get_life_needs_weights(n, c);
				w = ideal_weighting * need_drift_speed + w * (1.0f - need_drift_speed);

				assert(std::isfinite(w));
				assert(w <= count + 0.01f);
			}
		});
	}

	{
		float total_weights = 0.0f;
		uint32_t count = 0;

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_everyday_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto weight = need_weight(state, n, c, effective_prices);
				total_weights += weight;
				count++;
			}
		});

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_everyday_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto weight = need_weight(state, n, c, effective_prices);
				auto ideal_weighting = weight / total_weights * count;
				auto& w = state.world.nation_get_everyday_needs_weights(n, c);
				w = ideal_weighting * need_drift_speed + w * (1.0f - need_drift_speed);

				assert(std::isfinite(w));
				assert(w <= count + 0.01f);
			}
		});
	}

	{
		float total_weights = 0.0f;
		uint32_t count = 0;

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_luxury_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto weight = need_weight(state, n, c, effective_prices);
				total_weights += weight;
				count++;
			}
		});

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_luxury_need(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto weight = need_weight(state, n, c, effective_prices);
				auto ideal_weighting = weight / total_weights * count;
				auto& w = state.world.nation_get_luxury_needs_weights(n, c);
				w = ideal_weighting * need_drift_speed + w * (1.0f - need_drift_speed);

				assert(std::isfinite(w));
				assert(w <= count + 0.01f);
			}
		});
	}
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

// something is wrong with this whole process,
// for some reason it does not behave like
// economy simulation of the game itself
// should be replaced with proper complete simulation
// and export of resulting data into scenario
void presimulate(sys::state& state) {
	/*
	//save the treasury:
	std::vector<float> treasuries;
	treasuries.resize(2000);
	state.world.for_each_nation([&](dcon::nation_id id) {
		treasuries[id.index()] = state.world.nation_get_stockpiles(id, economy::money);
	});


	// adjust rgo power to economy setup
	// 5 years

	//static demographics::promotion_buffer pbuf;

	// wait for 10 years for economy to stabilize
	for(int j = 0; j < 10; j++) {
		for(int i = 0; i < 12; i++) {
			ai::update_ai_econ_construction(state);

			for(int k = 0; k < 30; k++) {
				//demographics::update_type_changes(state, k, 30, pbuf);
				//demographics::apply_type_changes(state, k, 30, pbuf);

				economy::update_rgo_employment(state);
				economy::update_factory_employment(state);
				economy::daily_update(state);
			}
		}
	}

	// update rgo power
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto modifer = std::min(10.f, (1.f +
			(state.world.commodity_get_total_real_demand(c) + 1.f)
			/ (state.world.commodity_get_total_production(c) + 1.f)));
		state.world.commodity_set_rgo_amount(c,
			state.world.commodity_get_rgo_amount(c) * modifer);
	});

	// wait for 10 years
	for(int j = 0; j < 10; j++) {
		for(int i = 0; i < 12; i++) {
			ai::update_ai_econ_construction(state);

			for(int k = 0; k < 30; k++) {
				//demographics::update_type_changes(state, k, 30, pbuf);
				//demographics::apply_type_changes(state, k, 30, pbuf);

				economy::update_rgo_employment(state);
				economy::update_factory_employment(state);
				economy::daily_update(state);
			}
		}
	}

	// reset data

	state.world.for_each_nation([&](dcon::nation_id id) {
		state.world.nation_set_stockpiles(id, economy::money, treasuries[id.index()]);
	});
	*/

	//as simulation above just doesn't work, lets calculate and compare needs of initial population and initial production:


	uint32_t total_commodities = state.world.commodity_size();

	std::vector<float> max_rgo_production_buffer;
	max_rgo_production_buffer.resize(total_commodities + 1);

	float total_max_rgo_production = 0.f;

	std::vector<float> rgo_weights;
	rgo_weights.resize(total_commodities + 1);



	// calculate total rgo production
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto fp = fatten(state.world, p);
		auto n = fp.get_nation_from_province_ownership();
		auto c = state.world.province_get_rgo(p);
		bool is_mine = state.world.commodity_get_is_mine(c);

		//currently working pops there
		float pop_amount = 0.0f;
		for(auto pt : state.world.in_pop_type) {
			if(pt == state.culture_definitions.slaves) {
				pop_amount += state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves));
			} else if(pt.get_is_paid_rgo_worker()) {
				pop_amount += state.world.province_get_demographics(p, demographics::to_key(state, pt));
			}
		}

		auto max_production = rgo_full_production_quantity(state, n, p); // maximal amount of goods which rgo could potentially produce
		auto pops_max = rgo_max_employment(state, n, p); // maximal amount of workers which rgo could potentially employ

		max_rgo_production_buffer[c.id.index()] += pop_amount / pops_max * max_production; // rough estimate again
		total_max_rgo_production += pop_amount / pops_max * max_production;
	});

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		rgo_weights[c.index()] = max_rgo_production_buffer[c.index()] / total_max_rgo_production;
	});

	std::vector<float> max_demand_buffer_depth_0;
	max_demand_buffer_depth_0.resize(total_commodities + 1);

	std::vector<float> max_demand_buffer_depth_1;
	max_demand_buffer_depth_1.resize(total_commodities + 1);

	std::vector<float> max_demand_buffer_depth_2;
	max_demand_buffer_depth_2.resize(total_commodities + 1);

	std::vector<float> max_demand_buffer_depth_3;
	max_demand_buffer_depth_3.resize(total_commodities + 1);

	//populate max demand
	for(auto n : state.nations_by_rank) {
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
				auto adj_pop_of_type = state.world.nation_get_demographics(n, demographics::to_key(state, pt)) / needs_scaling_factor;

				float base_life = state.world.pop_type_get_life_needs(pt, c) * 0.9f;
				float base_everyday = state.world.pop_type_get_everyday_needs(pt, c) * 0.3f;
				float base_luxury = state.world.pop_type_get_luxury_needs(pt, c) * 0.005f;

				max_demand_buffer_depth_0[c.index()] += (base_life + base_everyday + base_luxury) * adj_pop_of_type;
			});
		});
	}

	// turn goods into ingredients
	convert_commodities_into_ingredients(state, max_demand_buffer_depth_0, max_demand_buffer_depth_1, rgo_weights);

	// turn ingredients into their ingredients
	convert_commodities_into_ingredients(state, max_demand_buffer_depth_1, max_demand_buffer_depth_2, rgo_weights);
	convert_commodities_into_ingredients(state, max_demand_buffer_depth_2, max_demand_buffer_depth_3, rgo_weights);
	// should be enough steps for rough estimate...
	// TODO: make a loop with switching buffers

	// reduce imbalance
	float total_shortage = 0.f;
	float count = 0.f;

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		if(state.world.commodity_get_rgo_amount(c) > 0.f && max_rgo_production_buffer[c.index()] > 1.f) {
			float shortage = max_demand_buffer_depth_3[c.index()] / max_rgo_production_buffer[c.index()];
			float current_amount = state.world.commodity_get_rgo_amount(c);

			// do not decrease amounts, only increase
			if(shortage > 1.f) {
				state.world.commodity_set_rgo_amount(c, current_amount * shortage);

				total_shortage += shortage;
				count++;
			}			
		}
	});

	float average_shortage = total_shortage / count;

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		if(state.world.commodity_get_rgo_amount(c) > 0.f && max_rgo_production_buffer[c.index()] <= 1.f) {
			float current_amount = state.world.commodity_get_rgo_amount(c);
			state.world.commodity_set_rgo_amount(c, current_amount *  average_shortage);
		}
	});
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
	auto rng = state.world.nation_get_state_building_construction(n);
	return rng.begin() != rng.end();
}
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n) { // TODO - should be "good" now
	auto nation_fat = dcon::fatten(state.world, n);
	for(auto prov_owner : nation_fat.get_province_ownership()) {
		auto prov = prov_owner.get_province();
		for(auto factloc : prov.get_factory_location()) {
			auto scale = factloc.get_factory().get_production_scale();
			if(scale < production_scale_delta) {
				return true;
			}
		}
	}
	return false;
}

/*
// ASSUMED NOT TO RUN IN PARALLEL WITH ITSELF
void generate_national_artisan_prefs(sys::state& state, dcon::nation_id owner, std::vector<dcon::commodity_id>& nation_artisan_prefs) {
	nation_artisan_prefs.clear();
	auto const csize = state.world.commodity_size();

	static std::vector<dcon::commodity_id> input_available;
	static std::vector<dcon::commodity_id> output_shortage;
	static std::vector<dcon::commodity_id> feasible;
	input_available.clear();
	output_shortage.clear();
	feasible.clear();

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto kf = state.world.commodity_get_key_factory(cid);
		if(state.world.commodity_get_artisan_output_amount(cid) > 0
			&& (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(owner, kf)))) {

			feasible.push_back(cid);

			bool over_production = state.world.commodity_get_total_production(cid) > state.world.commodity_get_total_real_demand(cid);

			auto& inputs = state.world.commodity_get_artisan_inputs(cid);

			bool lacking_input = false;
			for(uint32_t j = 0; j < economy::commodity_set::set_size; ++j) {
				if(inputs.commodity_type[j]) {
					if(state.world.nation_get_demand_satisfaction(owner, inputs.commodity_type[j]) < 1.0f) {
						lacking_input = true;
						break;
					}
				} else {
					break;
				}
			}

			if(!lacking_input) {
				input_available.push_back(cid);
				if(state.world.nation_get_demand_satisfaction(owner, cid) < 1.0f || !over_production) {
					output_shortage.push_back(cid);
				}
			}
		}
	}

	if(!output_shortage.empty()) {
		nation_artisan_prefs = output_shortage;
	} else if(!input_available.empty()) {
		nation_artisan_prefs = input_available;
	} else if(!feasible.empty()) {
		nation_artisan_prefs = feasible;
	}
}

void randomly_assign_artisan_production(sys::state& state, dcon::province_id p, std::vector<dcon::commodity_id> const& nation_artisan_prefs) {
	if(!nation_artisan_prefs.empty()) {
		auto selected = rng::reduce(uint32_t(rng::get_random(state, uint32_t(p.value))), uint32_t(nation_artisan_prefs.size()));
		state.world.province_set_artisan_production(p, nation_artisan_prefs[selected]);
	}
}
*/

float base_artisan_profit(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto& inputs = state.world.commodity_get_artisan_inputs(c);

	float input_total = 0.0f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total += inputs.commodity_amounts[i] * state.world.commodity_get_current_price(inputs.commodity_type[i]);
		} else {
			break;
		}
	}

 	float output_total = state.world.commodity_get_artisan_output_amount(c) * state.world.commodity_get_current_price(c);

	float input_multiplier = std::max(0.1f, inputs_base_factor_artisans + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::artisan_input));
	float output_multiplier = std::max(0.1f, output_base_factor_artisans + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::artisan_output));

	return output_total * output_multiplier - input_multiplier * input_total;
}
float artisan_scale_limit(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	float least = 1.0f;
	auto& inputs = state.world.commodity_get_artisan_inputs(c);

	float input_total = 0.0f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			least = std::min(least, state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[i]));
		} else {
			break;
		}
	}
	return least;
}

void adjust_artisan_balance(sys::state& state, dcon::nation_id n) {
	auto const csize = state.world.commodity_size();
	float distribution_drift_speed = 0.01f;

	auto num_artisans = state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.artisans));

	std::vector<float> profits;
	profits.resize(csize);

	{
		float total_weights = 0.0f;
		float total_ex_weights = 0.0f;
		float total_possible = 0.0f;

		for(uint32_t i = 1; i < csize; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto kf = state.world.commodity_get_key_factory(cid);

			if(state.world.commodity_get_artisan_output_amount(cid) > 0.0f && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				//auto limit = artisan_scale_limit(state, n, cid);
				auto profit = base_artisan_profit(state, n, cid); //* limit;
				profits[cid.index()] = profit;

				if(profit > 0.f) {
					total_weights += profit;
					total_possible += 1.0f;
				}
			}
		}

		for(uint32_t i = 1; i < csize; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto kf = state.world.commodity_get_key_factory(cid);

			float market_size = state.world.commodity_get_total_consumption(cid) * 0.001f;
			auto consumed_ratio = std::min(1.f, (state.world.commodity_get_total_consumption(cid) + 0.0001f) / (state.world.commodity_get_total_production(cid) + 0.0001f));
			float output_size = state.world.commodity_get_artisan_output_amount(cid);

			if(state.world.commodity_get_artisan_output_amount(cid) > 0.0f && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				auto& w = state.world.nation_get_artisan_distribution(n, cid);
				if(profits[cid.index()] < 0) {
					w = std::max(0.f, (1.0f + profits[cid.index()] * w * market_size / consumed_ratio / consumed_ratio / output_size - 100.f / num_artisans) * w);
				} else if(total_weights > 0.0001f) {
					float ideal_weight = (profits[cid.index()] / total_weights);
					float speed = std::clamp(distribution_drift_speed * w * profits[cid.index()] * market_size / output_size + 100.f / num_artisans, 0.f, 0.01f);
					w = std::clamp((1.0f - speed) * w + speed * ideal_weight, 0.f, 1.f);

					assert(std::isfinite(w));
				} else if (total_possible > 0.f) {
					// move toward 0 production
					w = (1.0f - distribution_drift_speed) * w;
					assert(std::isfinite(w));
				}
			} else {
				state.world.nation_set_artisan_distribution(n, cid, 0.0f);
			}
		}
	}
}

void initialize(sys::state& state) {
	state.world.nation_resize_domestic_market_pool(state.world.commodity_size());
	state.world.nation_resize_real_demand(state.world.commodity_size());
	state.world.nation_resize_stockpile_targets(state.world.commodity_size());
	state.world.nation_resize_drawing_on_stockpiles(state.world.commodity_size());
	state.world.nation_resize_life_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_everyday_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_luxury_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_imports(state.world.commodity_size());
	state.world.nation_resize_army_demand(state.world.commodity_size());
	state.world.nation_resize_navy_demand(state.world.commodity_size());
	state.world.nation_resize_construction_demand(state.world.commodity_size());
	state.world.nation_resize_private_construction_demand(state.world.commodity_size());
	state.world.nation_resize_demand_satisfaction(state.world.commodity_size());
	state.world.nation_resize_life_needs_weights(state.world.commodity_size());
	state.world.nation_resize_everyday_needs_weights(state.world.commodity_size());
	state.world.nation_resize_luxury_needs_weights(state.world.commodity_size());
	state.world.commodity_resize_price_record(price_history_length);

	initialize_artisan_distribution(state);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto fc = fatten(state.world, c);
		fc.set_current_price(fc.get_cost());
		fc.set_total_consumption(0.0f);
		fc.set_total_production(0.0f);
		fc.set_total_real_demand(0.0f);

		for(uint32_t i = 0; i < price_history_length; ++i) {
			fc.set_price_record(i, fc.get_cost());
		}
		// fc.set_global_market_pool();
	});

	auto savings_buffer = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		auto ft = fatten(state.world, t);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			savings_buffer.get(t) += state.world.commodity_get_is_available_from_start(c)
				? state.world.commodity_get_cost(c) * ft.get_life_needs(c) + 0.5f * state.world.commodity_get_cost(c) * ft.get_everyday_needs(c)
				: 0.0f;
		});
		auto strata = (ft.get_strata() * 2) + 1;
		savings_buffer.get(t) *= strata;
	});

	state.world.for_each_pop([&](dcon::pop_id p) {
		auto fp = fatten(state.world, p);
		fp.set_life_needs_satisfaction(1.0f);
		fp.set_everyday_needs_satisfaction(0.5f);
		fp.set_savings(savings_buffer.get(fp.get_poptype()) * fp.get_size() / economy::needs_scaling_factor);
	});

	state.world.for_each_factory([&](dcon::factory_id f) {
		auto ff = fatten(state.world, f);
		ff.set_production_scale(1.0f);
	});

	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto fp = fatten(state.world, p);

		bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));

		//float pop_amount = 0.0f;
		//for(auto pt : state.world.in_pop_type) {
		//	if(pt == state.culture_definitions.slaves) {
		//		pop_amount += state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves));
		//	} else if(pt.get_is_paid_rgo_worker()) {
		//		pop_amount += state.world.province_get_demographics(p, demographics::to_key(state, pt));
		//	}
		//}

		//auto amount = std::ceil(1.1f * pop_amount / rgo_per_size_employment);
		auto amount = std::ceil(1000.f / rgo_per_size_employment * state.map_state.map_data.province_area[province::to_map_id(p)]);
		fp.set_rgo_size(std::max(1.0f, amount));
		fp.set_rgo_production_scale(0.f);
	});

	state.world.for_each_nation([&](dcon::nation_id n) {
		initialize_needs_weights(state, n);

		auto fn = fatten(state.world, n);
		fn.set_administrative_spending(int8_t(80));
		fn.set_military_spending(int8_t(60));
		fn.set_education_spending(int8_t(100));
		fn.set_social_spending(int8_t(100));
		fn.set_land_spending(int8_t(100));
		fn.set_naval_spending(int8_t(100));
		fn.set_construction_spending(int8_t(100));

		fn.set_poor_tax(int8_t(75));
		fn.set_middle_tax(int8_t(75));
		fn.set_rich_tax(int8_t(75));

		fn.set_spending_level(1.0f);

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			state.world.nation_set_demand_satisfaction(n, c, 1.0f);
			// set domestic market pool
		});
	});

	update_rgo_employment(state);
	update_factory_employment(state);

	populate_army_consumption(state);
	populate_navy_consumption(state);
	populate_construction_consumption(state);

	auto base_prices = state.world.commodity_make_vectorizable_float_buffer();
	state.world.for_each_commodity([&](auto id) { base_prices.set(id, state.world.commodity_get_cost(id)); });

	state.world.for_each_nation([&](dcon::nation_id n) {
		state.world.nation_set_stockpiles(n, money, 2.0f * full_spending_cost(state, n, base_prices));
	});
}

float sphere_leader_share_factor(sys::state const& state, dcon::nation_id sphere_leader, dcon::nation_id sphere_member) {
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
		float total_investment = 0;
		float sl_investment = 0;
		for(auto gp : state.world.nation_get_unilateral_relationship_as_target(sphere_member)) {
			if(gp.get_source() == sphere_leader) {
				sl_investment = gp.get_foreign_investment();
			}
			total_investment += gp.get_foreign_investment();
		}
		float investment_fraction = total_investment > 0.0001f ? sl_investment / total_investment : 0.0f;
		return base + (1.0f - base) * investment_fraction;
	} else {
		return state.defines.unciv_base_share_factor;
	}
}

void absorb_sphere_member_production(sys::state& state, dcon::nation_id n) {
	for(auto gp : state.world.nation_get_gp_relationship_as_great_power(n)) {
		if((gp.get_status() & nations::influence::level_mask) == nations::influence::level_in_sphere) {
			// - Each sphere member has its domestic x its-share-factor (see above) of its base supply and demand added to its
			// sphere leader's domestic supply and demand (this does not affect global supply and demand)

			auto t = gp.get_influence_target();
			float share = sphere_leader_share_factor(state, n, t);
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				state.world.nation_get_domestic_market_pool(n, c) += share * state.world.nation_get_domestic_market_pool(t, c);
			});
		}
	}
}

void give_sphere_leader_production(sys::state& state, dcon::nation_id n) {
	if(auto sl = state.world.nation_get_in_sphere_of(n); sl) {
		//- Every nation in a sphere (after the above has been calculated for the entire sphere) has their effective domestic
		// supply set
		// to (1 - its-share-factor) x original-domestic-supply + sphere-leader's-domestic supply

		float share = sphere_leader_share_factor(state, sl, n);
		state.world.for_each_commodity(
				[&](dcon::commodity_id c) { state.world.nation_get_domestic_market_pool(n, c) *= (1.0f - share); });
	}
}

float effective_tariff_rate(sys::state& state, dcon::nation_id n) {
	auto tariff_efficiency = nations::tariff_efficiency(state, n);
	return tariff_efficiency * float(state.world.nation_get_tariffs(n)) / 100.0f;
}

float global_market_price_multiplier(sys::state& state, dcon::nation_id n) {
	auto central_ports = state.world.nation_get_central_ports(n);
	if(central_ports > 0) {
		return effective_tariff_rate(state, n)
			+ float(state.world.nation_get_central_blockaded(n)) / float(central_ports)
			+ 1.0f;
	} else {
		return effective_tariff_rate(state, n) + 1.0f;
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


float rgo_effective_size(sys::state const& state, dcon::nation_id n, dcon::province_id p) {
	auto c = state.world.province_get_rgo(p);
	if(!c)
		return 0.01f;

	bool is_mine = state.world.commodity_get_is_mine(c);

	// - We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size +
	// technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
	auto sz = state.world.province_get_rgo_size(p);
	auto pmod = state.world.province_get_modifier_values(p,  is_mine ? sys::provincial_mod_offsets::mine_rgo_size : sys::provincial_mod_offsets::farm_rgo_size);
	auto nmod = state.world.nation_get_modifier_values(n, is_mine ? sys::national_mod_offsets::mine_rgo_size : sys::national_mod_offsets::farm_rgo_size);
	auto specific_pmod = state.world.nation_get_rgo_size(n, c);
	auto bonus = pmod + nmod + specific_pmod + 1.0f;

	return std::max(sz * bonus, 0.01f);
}

float rgo_max_employment(sys::state const& state, dcon::nation_id n, dcon::province_id p) {
	return rgo_per_size_employment * rgo_effective_size(state, n, p);
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

void update_rgo_employment(sys::state& state) {
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		auto rgo_max = rgo_max_employment(state, owner, p);
		auto scale = state.world.province_get_rgo_production_scale(p);
		auto rgo_max_scaled = rgo_max * scale;

		bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));
		float worker_pool = 0.0f;
		for(auto wt : state.culture_definitions.rgo_workers) {
			worker_pool += state.world.province_get_demographics(p, demographics::to_key(state, wt));
		}
		float slave_pool = state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves));
		float labor_pool = worker_pool + slave_pool;

		auto new_val = labor_pool >= rgo_max_scaled ? scale : labor_pool / rgo_max;
		assert(new_val >= 0.0f && new_val <= 1.0f);
		state.world.province_set_rgo_employment(p, new_val);

		auto slave_fraction = (slave_pool > rgo_max_scaled) ? rgo_max_scaled / slave_pool : 1.0f;
		auto free_fraction = std::max(0.0f, (worker_pool > rgo_max_scaled - slave_pool) ? (rgo_max_scaled - slave_pool) / std::max(worker_pool, 0.01f) : 1.0f);

		for(auto pop : state.world.province_get_pop_location(p)) {
			auto pt = pop.get_pop().get_poptype();
			if(pt == state.culture_definitions.slaves) {
				pop.get_pop().set_employment(pop.get_pop().get_size() * slave_fraction);
			} else if(pt.get_is_paid_rgo_worker()) {
				pop.get_pop().set_employment(pop.get_pop().get_size() * free_fraction);
			}
		}
	});
}

float factory_max_employment(sys::state const& state, dcon::factory_id f) {
	return factory_per_level_employment * state.world.factory_get_level(f);
}

float factory_primary_employment(sys::state const& state, dcon::factory_id f) {
	auto primary_employment = state.world.factory_get_primary_employment(f);
	return factory_max_employment(state, f) * (state.economy_definitions.craftsmen_fraction * primary_employment);
}
float factory_secondary_employment(sys::state const& state, dcon::factory_id f) {
	auto secondary_employment = state.world.factory_get_secondary_employment(f);
	return factory_max_employment(state, f) * ((1 - state.economy_definitions.craftsmen_fraction) * secondary_employment);
}
float factory_total_employment(sys::state const& state, dcon::factory_id f) {
	// TODO: Document this, also is this a stub?
	auto primary_employment = state.world.factory_get_primary_employment(f);
	auto secondary_employment = state.world.factory_get_secondary_employment(f);
	return factory_max_employment(state, f) * (state.economy_definitions.craftsmen_fraction * primary_employment + (1 -state.economy_definitions.craftsmen_fraction) * secondary_employment);
}

void update_factory_employment(sys::state& state) {
	state.world.for_each_state_instance([&](dcon::state_instance_id si) {
		float primary_pool = state.world.state_instance_get_demographics(si,
				demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		float secondary_pool = state.world.state_instance_get_demographics(si,
				demographics::to_key(state, state.culture_definitions.secondary_factory_worker));

		static std::vector<dcon::factory_id> ordered_factories;
		ordered_factories.clear();

		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			for(auto fac : state.world.province_get_factory_location(p)) {
				ordered_factories.push_back(fac.get_factory());
			}
		});

		std::sort(ordered_factories.begin(), ordered_factories.end(), [&](dcon::factory_id a, dcon::factory_id b) {
			if(factory_is_profitable(state, a) != factory_is_profitable(state, b)) {
				return factory_is_profitable(state, a);
			}
			if(factory_priority(state, a) != factory_priority(state, b)) {
				return factory_priority(state, a) > factory_priority(state, b);
			}
			return a.index() < b.index();
		});

		float primary_pool_copy = primary_pool;
		float secondary_pool_copy = secondary_pool;
		for(uint32_t index = 0; index < ordered_factories.size();) {
			uint32_t next_index = index;

			float total_workforce = 0.0f;
			for(; next_index < ordered_factories.size(); ++next_index) {
				if(factory_is_profitable(state, ordered_factories[index]) !=
								factory_is_profitable(state, ordered_factories[next_index]) ||
						factory_priority(state, ordered_factories[index]) != factory_priority(state, ordered_factories[next_index])) {
					break;
				}
				total_workforce += factory_max_employment(state, ordered_factories[next_index]) *
													 state.world.factory_get_production_scale(ordered_factories[next_index]);
			}

			{
				float type_share = state.economy_definitions.craftsmen_fraction * total_workforce;
				float scale = primary_pool_copy >= type_share ? 1.0f : primary_pool_copy / type_share;
				primary_pool_copy = std::max(0.0f, primary_pool_copy - type_share);
				for(uint32_t i = index; i < next_index; ++i) {
					state.world.factory_set_primary_employment(ordered_factories[i],
							scale * state.world.factory_get_production_scale(ordered_factories[i]));
				}
			}
			{
				float type_share = (1.0f - state.economy_definitions.craftsmen_fraction) * total_workforce;
				float scale = secondary_pool_copy >= type_share ? 1.0f : secondary_pool_copy / type_share;
				secondary_pool_copy = std::max(0.0f, secondary_pool_copy - type_share);
				for(uint32_t i = index; i < next_index; ++i) {
					state.world.factory_set_secondary_employment(ordered_factories[i],
							scale * state.world.factory_get_production_scale(ordered_factories[i]));
				}
			}

			index = next_index;
		}

		float prim_employment = 1.0f - (primary_pool > 0 ? primary_pool_copy / primary_pool : 0.0f);
		float sec_employment = 1.0f - (secondary_pool > 0 ? secondary_pool_copy / secondary_pool : 0.0f);

		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			for(auto pop : state.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype() == state.culture_definitions.primary_factory_worker) {
					pop.get_pop().set_employment(pop.get_pop().get_size() * prim_employment);
				} else if(pop.get_pop().get_poptype() == state.culture_definitions.secondary_factory_worker) {
					pop.get_pop().set_employment(pop.get_pop().get_size() * sec_employment);
				}
			}
		});
	});
}

/*
*
- Each factory has an input, output, and throughput multipliers.
- These are computed from the employees present. Input and output are 1 + employee effects, throughput starts at 0
- The input multiplier is also multiplied by (1 + sum-of-any-triggered-modifiers-for-the-factory) x
0v(national-mobilization-impact - (overseas-penalty if overseas, or 0 otherwise))
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

float factory_full_production_quantity(sys::state const& state, dcon::factory_id f, dcon::nation_id n,
		float mobilization_impact) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();

	/*
	float total_state_pop = std::max(0.01f, state.world.state_instance_get_demographics(s, demographics::total));
	float owner_fraction = std::min(0.05f, state.world.state_instance_get_demographics(s, demographics::to_key(state,
	state.culture_definitions.capitalists)) / total_state_pop); float input_multiplier = fac.get_triggered_modifiers() * (1.0f +
	owner_fraction * -2.5f);
	*/

	float throughput_multiplier = (state.world.nation_get_factory_goods_throughput(n, fac_type.get_output()) + 1.0f);
	float output_multiplier = state.world.nation_get_factory_goods_output(n, fac_type.get_output()) + 1.0f +
														fac.get_secondary_employment() * (1.0f - state.economy_definitions.craftsmen_fraction) *
																fac.get_secondary_employment() * 1.5f;
	float max_production_scale = fac.get_primary_employment() * fac.get_level() *
															 std::max(0.0f, (mobilization_impact - state.world.nation_get_overseas_penalty(n)));

	return throughput_multiplier * output_multiplier * max_production_scale;
}

float rgo_efficiency(sys::state const& state, dcon::nation_id n, dcon::province_id p) {
	auto c = state.world.province_get_rgo(p);
	if(!c)
		return 0.0f;

	bool is_mine = state.world.commodity_get_is_mine(c);

	return state.world.commodity_get_rgo_amount(c)*
		std::max(0.5f, (1.0f + state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_throughput) +
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_throughput) +
			state.world.province_get_modifier_values(p, is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff) + state.world.nation_get_modifier_values(n, is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff)))*
		std::max(0.5f, (1.0f + state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_output) +
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_output) +
			state.world.nation_get_rgo_goods_output(n, c)));
}

float rgo_full_production_quantity(sys::state const& state, dcon::nation_id n, dcon::province_id p) {
	/*
	- We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size +
	technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
	- We add its production to domestic supply, calculating that amount basically in the same way we do for factories, by
	computing RGO-throughput x RGO-output x RGO-size x base-commodity-production-quantity, except that it is affected by different
	modifiers.
	*/
	auto c = state.world.province_get_rgo(p);
	if(!c)
		return 0.0f;
	auto eff_size = rgo_effective_size(state, n, p);
	auto val = eff_size * rgo_efficiency(state, n, p);
	auto rval = val * rgo_boost;
	assert(rval >= 0.0f && std::isfinite(rval));
	return rval;
}

float factory_min_input_available(
	sys::state& state,
	dcon::nation_id n,
	dcon::factory_type_fat_id fac_type
) {
	float min_input_available = 1.0f;
	auto& inputs = fac_type.get_inputs();
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			min_input_available =
				std::min(
					min_input_available,
					state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[i])
				);
		} else {
			break;
		}
	}
	return min_input_available;
}

float factory_input_total_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::factory_type_fat_id fac_type,
	ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices
) {
	float input_total = 0.0f;
	auto& inputs = fac_type.get_inputs();
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total += inputs.commodity_amounts[i] * effective_prices.get(inputs.commodity_type[i]);
		} else {
			break;
		}
	}
	return input_total;
}

float factory_min_e_input_available(
	sys::state& state,
	dcon::nation_id n,
	dcon::factory_type_fat_id fac_type
) {
	float min_e_input_available = 1.0f;
	auto& e_inputs = fac_type.get_efficiency_inputs();
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(e_inputs.commodity_type[i]) {
			min_e_input_available =
				std::min(
					min_e_input_available,
					state.world.nation_get_demand_satisfaction(n, e_inputs.commodity_type[i])
				);
		} else {
			break;
		}
	}

	return min_e_input_available;
}

float factory_e_input_total_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::factory_type_fat_id fac_type,
	ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices
) {
	float e_input_total = 0.0f;
	auto& e_inputs = fac_type.get_efficiency_inputs();
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(e_inputs.commodity_type[i]) {
			e_input_total += e_inputs.commodity_amounts[i] * effective_prices.get(e_inputs.commodity_type[i]);
		} else {
			break;
		}
	}

	return e_input_total;
}

float factory_input_multiplier(
	sys::state& state,
	dcon::factory_fat_id fac,
	dcon::nation_id n,
	dcon::province_id p,
	dcon::state_instance_id s
) {
	float total_workers = factory_max_employment(state, fac);
	float small_size_effect = 1.f;
	float small_bound = factory_per_level_employment * 5.f;
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

	return small_size_effect *
		fac.get_triggered_modifiers() *
		std::max(
			0.1f,
			(inputs_base_factor
				+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_input)
				+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input)
				+ owner_fraction * -2.5f)
		);
}

float factory_throughput_multiplier(
	sys::state& state,
	dcon::factory_type_fat_id fac_type,
	dcon::nation_id n,
	dcon::province_id p,	
	dcon::state_instance_id s
) {
	return state.world.nation_get_factory_goods_throughput(n, fac_type.get_output())
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_throughput)
		+ 1.0f;
}

float factory_output_multiplier(
	sys::state& state,
	dcon::factory_fat_id fac,
	dcon::nation_id n,
	dcon::province_id p
) {
	auto fac_type = fac.get_building_type();

	return state.world.nation_get_factory_goods_output(n, fac_type.get_output())
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_output)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_output)
		+ fac.get_secondary_employment()
			* (1.0f - state.economy_definitions.craftsmen_fraction)
			* 1.5f
		+ 1.0f;
}

float factory_max_production_scale(
	sys::state& state,
	dcon::factory_fat_id fac,
	float mobilization_impact,
	bool occupied,
	bool overseas
) {
	return fac.get_primary_employment()
		* fac.get_level()
		* (occupied ? 0.1f : 1.0f)
		* std::max(0.0f, (mobilization_impact /* - (overseas ? state.world.nation_get_overseas_penalty(n) : 0)*/));
	// i have no idea why being overseas should affect local factories in any way
}

float update_factory_scale(
	sys::state& state,
	dcon::factory_fat_id fac,
	float max_production_scale,
	float raw_profit,
	float desired_raw_profit
) {
	float total_workers = factory_max_employment(state, fac);
	float several_workers_scale = 10.f / total_workers;

	float effective_production_scale = 0.0f;
	if(raw_profit > desired_raw_profit || state.world.factory_get_subsidized(fac)) {
		// increase production scale

		// calculate percentage of income relatively to spendings
		float profit_ratio = std::min(100.f, (raw_profit + 0.0001f) / (desired_raw_profit + 0.0001f) - 1.f);

		// divide by a factory level to keep it consistent in "workers" units ?
		float speed_modifier = std::max(0.f, profit_ratio * profit_ratio);// / fac.get_level();

		// update production scale
		auto new_production_scale = std::min(1.0f, fac.get_production_scale() + production_scale_delta * speed_modifier + several_workers_scale);
		fac.set_production_scale(new_production_scale);
		effective_production_scale = std::min(new_production_scale * fac.get_level(), max_production_scale);
	} else {
		// reduce production scale
		float spendings_ratio = std::min(100.f, (desired_raw_profit + 0.0001f) / (raw_profit + 0.0001f) - 1.f);
		float speed_modifier = std::max(0.f, spendings_ratio) * 5.f;// / fac.get_level() * 100.f; // we want to reduce production much faster
		auto new_production_scale = std::max(0.0f, fac.get_production_scale() - production_scale_delta * speed_modifier - several_workers_scale);
		fac.set_production_scale(new_production_scale);
		effective_production_scale = std::min(new_production_scale * fac.get_level(), max_production_scale);
	}

	return effective_production_scale;
}

float factory_desired_raw_profit(
	dcon::factory_fat_id fac,
	float spendings
) {
	return spendings
		* (
			1.2f
			+ fac.get_secondary_employment() * fac.get_level() / 150.f
		);
}

void update_single_factory_consumption(
	sys::state& state,
	dcon::factory_id f,
	dcon::nation_id n,
	dcon::province_id p,
	dcon::state_instance_id s,
	ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices,
	float mobilization_impact,
	float expected_min_wage,
	bool occupied,
	bool overseas
) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();

	assert(fac_type);
	assert(fac_type.get_output());
	assert(n);
	assert(p);
	assert(s);

	float total_workers = factory_max_employment(state, f);
	float several_workers_scale = 10.f / total_workers;
	float max_production_scale = factory_max_production_scale(
		state,
		fac,
		mobilization_impact,
		occupied,
		overseas
	);
		
	float current_workers = total_workers * max_production_scale;

	//inputs

	float input_total = factory_input_total_cost(state, n, fac_type, effective_prices);
	float min_input_available = factory_min_input_available(state, n, fac_type);
	float e_input_total = factory_e_input_total_cost(state, n , fac_type, effective_prices);
	float min_e_input_available = factory_min_e_input_available(state, n, fac_type);

	//modifiers

	float input_multiplier = factory_input_multiplier(state, fac, n, p, s);
	float throughput_multiplier = factory_throughput_multiplier(state, fac_type, n, p, s);
	float output_multiplier = factory_output_multiplier(state, fac, n, p);

	//this value represents total production if 1 lvl of this factory is filled with workers
	float total_production = fac_type.get_output_amount()
		* (0.75f + 0.25f * min_e_input_available)
		* throughput_multiplier
		* output_multiplier
		* min_input_available;

	//this value represents raw profit if 1 lvl of this factory is filled with workers
	float profit =
		total_production
		* state.world.commodity_get_current_price(fac_type.get_output());

	//this value represents spendings if 1 lvl of this factory is filled with workers
	float spendings =
			expected_min_wage
			* (factory_per_level_employment / needs_scaling_factor)
		+
			input_multiplier
			* throughput_multiplier
			* input_total
			* min_input_available
		+
			input_multiplier
			* e_input_total
			* min_e_input_available
			* min_input_available;

	float desired_profit = factory_desired_raw_profit(fac, spendings);
	float max_pure_profit = profit - spendings;
	state.world.factory_set_unprofitable(f, !(max_pure_profit > 0.0f));

	float effective_production_scale = update_factory_scale(
		state,
		fac,
		max_production_scale,
		profit,
		desired_profit
	);	

	auto& inputs = fac_type.get_inputs();
	auto& e_inputs = fac_type.get_efficiency_inputs();

	// register real demand : input_multiplier * throughput_multiplier * level * primary_employment
	// also multiply by target production scale... otherwise too much excess demand is generated
	// also multiply by something related to minimal satisfied input
	// to prevent generation of too much demand on rgos already influenced by a shortage

	float input_scale =
		input_multiplier
		* throughput_multiplier
		* effective_production_scale
		* (0.1f + min_input_available * 0.9f);
	
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			register_demand(state,
				n,
				inputs.commodity_type[i],
				input_scale
				* inputs.commodity_amounts[i]
				, economy_reason::factory
			);
		} else {
			break;
		}
	}

	// and for efficiency inputs
	//  the consumption of efficiency inputs is (national-factory-maintenance-modifier + 1) x input-multiplier x
	//  throughput-multiplier x factory level
	auto const mfactor = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(e_inputs.commodity_type[i]) {
			register_demand(
				state,
				n,
				e_inputs.commodity_type[i],
				mfactor
				* input_scale
				* e_inputs.commodity_amounts[i]
				* (0.1f + min_e_input_available * 0.9f)
				, economy_reason::factory
			);
		} else {
			break;
		}
	}

	float actual_production = total_production * effective_production_scale;
	float pure_profit = max_pure_profit * effective_production_scale;

	state.world.factory_set_actual_production(f, actual_production);
	state.world.factory_set_full_profit(f, pure_profit);
}

void update_single_factory_production(sys::state& state, dcon::factory_id f, dcon::nation_id n, float expected_min_wage) {

	auto production = state.world.factory_get_actual_production(f);
	if(production > 0) {
		auto fac = fatten(state.world, f);
		auto fac_type = fac.get_building_type();

		/*
		float min_input = 1.0f;
		float min_efficiency_input = 1.0f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(fac_type.get_inputs().commodity_type[i]) {
				min_input = std::min(min_input, state.world.nation_get_demand_satisfaction(n, fac_type.get_inputs().commodity_type[i]));
			} else {
				break;
			}
		}

		// and for efficiency inputs
		for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
			if(fac_type.get_efficiency_inputs().commodity_type[i]) {
				min_efficiency_input = std::min(min_efficiency_input,
						state.world.nation_get_demand_satisfaction(n, fac_type.get_efficiency_inputs().commodity_type[i]));
			} else {
				break;
			}
		}
		*/

		auto amount = production;
		auto money_made = state.world.factory_get_full_profit(f);

		state.world.factory_set_actual_production(f, amount);
		state.world.nation_get_domestic_market_pool(n, fac_type.get_output()) += amount;

		
		if(!fac.get_subsidized()) {
			state.world.factory_set_full_profit(f, money_made);
		} else {
			float min_wages =
				expected_min_wage
				* fac.get_level()
				* fac.get_primary_employment()
				* (factory_per_level_employment / needs_scaling_factor);
			if(money_made < min_wages) {
				auto diff = min_wages - money_made;
				if(state.world.nation_get_stockpiles(n, money) > diff || can_take_loans(state, n)) {
					state.world.factory_set_full_profit(f, min_wages);
					state.world.nation_get_stockpiles(n, money) -= diff;
					state.world.nation_get_subsidies_spending(n) += diff;
				} else {
					state.world.factory_set_full_profit(f, std::max(money_made, 0.0f));
					fac.set_subsidized(false);
				}
			} else {
				state.world.factory_set_full_profit(f, money_made);
			}
		}
	} else {
	}
}

std::tuple<float, float, float> rgo_relevant_population(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	auto relevant_paid_population = 0.f;
	for(auto wt : state.culture_definitions.rgo_workers) {
		relevant_paid_population += state.world.province_get_demographics(p, demographics::to_key(state, wt));
	}
	auto slaves = state.world.province_get_demographics(p, demographics::to_employment_key(state, state.culture_definitions.slaves));

	return std::tuple<float, float, float>(relevant_paid_population, slaves, relevant_paid_population + slaves);
}

float rgo_overhire_modifier(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	auto pops_max = rgo_max_employment(state, n, p);
	auto current_employment = pops_max * state.world.province_get_rgo_employment(p);
	float max_employment_before_overhire = pops_max / rgo_overhire_multiplier;

	float overhire_modifier = 1.f;
	if(current_employment < max_employment_before_overhire) {
		overhire_modifier = 2.f - 1.f * current_employment / max_employment_before_overhire;
	} else if(current_employment > max_employment_before_overhire) {
		overhire_modifier = 1.f - (current_employment - max_employment_before_overhire) / pops_max;
	}
	assert(overhire_modifier >= 0.f);

	return overhire_modifier;
}


float rgo_desired_worker_norm_profit(sys::state& state, dcon::province_id p, dcon::nation_id n, float min_wage, float total_relevant_population) {
	auto pops_max = rgo_max_employment(state, n, p); // maximal amount of workers which rgo could potentially employ
	auto current_employment = pops_max * state.world.province_get_rgo_employment(p);

	//we assume a "perfect ratio" of 1 aristo per N pops
	float perfect_aristos_amount = total_relevant_population / 1000.f;
	float perfect_aristos_amount_adjusted = perfect_aristos_amount / needs_scaling_factor;
	float aristos_desired_cut = perfect_aristos_amount_adjusted * (
		state.world.nation_get_everyday_needs_costs(n, state.culture_definitions.aristocrat)
		+ state.world.nation_get_life_needs_costs(n, state.culture_definitions.aristocrat)
	);
	float aristo_burden_per_worker = aristos_desired_cut / (total_relevant_population + 1);

	float min_wage_burden_per_worker = min_wage / needs_scaling_factor;

	float desired_profit_by_worker = aristo_burden_per_worker + min_wage_burden_per_worker;

	// we want to employ at least someone, so we decrease our desired profits when employment is low.
	// aristocracy would prefer to gain less money instead of suffering constant revolts
	// not exactly an ideal solution but it works and doesn't create goods or wealth out of thin air
	float employment_ratio = (current_employment + 1.f) / total_relevant_population;
	desired_profit_by_worker = desired_profit_by_worker * employment_ratio;

	return desired_profit_by_worker;

	//return (aristos_desired_cut + min_wage / needs_scaling_factor * (current_employment + 1));// / total_relevant_population; //* total_relevant_population;
}

float rgo_expected_worker_norm_profit(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	auto overhire_modifier = rgo_overhire_modifier(state, p, n);
	auto efficiency = rgo_efficiency(state, n, p);
	auto rgo = state.world.province_get_rgo(p);
	auto current_price = state.world.commodity_get_current_price(rgo);
	auto consumed_ratio = std::min(1.f, (state.world.commodity_get_total_consumption(rgo) + 0.0001f) / (state.world.commodity_get_total_production(rgo) + 0.0001f));

	return
		consumed_ratio
		* overhire_modifier
		* efficiency
		* current_price
		/ rgo_per_size_employment;
}

void update_province_rgo_consumption(sys::state& state, dcon::province_id p, dcon::nation_id n, float mobilization_impact,
		float expected_min_wage, bool occupied) {

	auto max_production = rgo_full_production_quantity(state, n, p);
	auto overhire_modifier = rgo_overhire_modifier(state, p, n);

	auto pops_max = rgo_max_employment(state, n, p); // maximal amount of workers which rgo could potentially employ
	auto current_employment = pops_max * state.world.province_get_rgo_employment(p);

	auto [non_slaves, slaves, total_relevant] = rgo_relevant_population(state, p, n);

	auto relevant_to_max_ratio = total_relevant / (pops_max + 1.f);
	auto current_scale = std::min(state.world.province_get_rgo_production_scale(p), relevant_to_max_ratio);

	float expected_profit = rgo_expected_worker_norm_profit(state, p, n);
	float desired_profit = rgo_desired_worker_norm_profit(state, p, n, expected_min_wage, total_relevant);

	auto c = state.world.province_get_rgo(p);
	if(c.get_money_rgo()) {
		// gold does not respond to supply and demand, so we can safely produce as much as we want
		state.world.province_set_rgo_production_scale(p, 1.f);
	} else if(expected_profit >= desired_profit) {
		float profit_ratio = std::min(100.f, (expected_profit + 0.00000001f) / (desired_profit + 0.00000001f) - 1.f);
		float speed_modifier = profit_ratio / rgo_effective_size(state, n, p) * 100.f;
		float change = std::min(0.5f * relevant_to_max_ratio, (rgo_production_scale_neg_delta + 0.001f * relevant_to_max_ratio) * speed_modifier + 100.f / pops_max);

		change = std::max(change, 0.f);

		auto new_production_scale = std::min(1.0f, current_scale + change);

		assert(change >= -0.0001f);
		state.world.province_set_rgo_production_scale(p, new_production_scale);
	} else {
		float spendings_ratio = std::min(100.f, (desired_profit + 0.00000001f) / (expected_profit + 0.00000001f) - 1.f);
		float speed_modifier = spendings_ratio / rgo_effective_size(state, n, p);
		float change = -std::min(0.05f * relevant_to_max_ratio, (rgo_production_scale_neg_delta + 0.001f * relevant_to_max_ratio) * speed_modifier + 100.f / pops_max);

		change = std::min(change, 0.f);

		auto new_production_scale = std::max(0.0f, current_scale + change);

		assert(change <= 0.00001f);
		state.world.province_set_rgo_production_scale(p, new_production_scale);
	}

	// rgos produce all the way down
	assert(max_production * state.world.province_get_rgo_employment(p) >= 0);
	float employment_ratio = state.world.province_get_rgo_employment(p);
	state.world.province_set_rgo_actual_production(p, max_production * overhire_modifier * employment_ratio);
}

void update_province_rgo_production(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	auto amount = state.world.province_get_rgo_actual_production(p);
	auto c = state.world.province_get_rgo(p);
	assert(c);
	if(!c)
		return;

	state.world.nation_get_domestic_market_pool(n, c) += amount;
	assert(amount >= 0);
	assert(state.world.commodity_get_current_price(c) >= 0);
	assert(amount * state.world.commodity_get_current_price(c) >= 0);
	state.world.province_set_rgo_full_profit(p, amount * state.world.commodity_get_current_price(c));

	if(c.get_money_rgo()) {
		assert(std::isfinite(amount * state.defines.gold_to_cash_rate) && amount * state.defines.gold_to_cash_rate >= 0.0f);
		state.world.nation_get_stockpiles(n, money) += amount * state.defines.gold_to_cash_rate;
	}
}


void update_national_artisan_consumption(
	sys::state& state,
	dcon::nation_id n,
	ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices,
	float expected_min_wage,
	float mobilization_impact
) {
	auto const csize = state.world.commodity_size();
	auto num_artisans = state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.artisans));
	float total_profit = 0.0f;

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.nation_set_artisan_actual_production(n, cid, 0.0f);
		if(state.world.nation_get_artisan_distribution(n, cid) > 0.0f) {

			float input_total = 0.0f;
			auto& inputs = state.world.commodity_get_artisan_inputs(cid);
			float min_available = 1.0f;

			for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
				if(inputs.commodity_type[j]) {
					input_total += inputs.commodity_amounts[j] * effective_prices.get(inputs.commodity_type[j]);
					min_available = std::min(min_available, state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[j]));
				} else {
					break;
				}
			}

			float output_total = state.world.commodity_get_artisan_output_amount(cid) * state.world.commodity_get_current_price(cid);

			float input_multiplier = std::max(0.1f, inputs_base_factor_artisans
				+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::artisan_input));
			float throughput_multiplier = std::max(0.1f, 1.0f
				+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::artisan_throughput));
			float output_multiplier = std::max(0.1f, output_base_factor_artisans
				+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::artisan_output));

			float max_production_scale = num_artisans * state.world.nation_get_artisan_distribution(n, cid) / 10'000.0f * std::max(0.0f, mobilization_impact);

			auto profitability_factor = (output_total * output_multiplier * throughput_multiplier * min_available - input_multiplier * input_total * throughput_multiplier * min_available) / (0.5f * expected_min_wage * (10'000.0f / needs_scaling_factor));

			bool profitable = (output_total * output_multiplier - input_multiplier * input_total) >= 0.0f;

			//if(profitability_factor <= -1.0f) {

			//} else {
				//profitability_factor = std::clamp(profitability_factor * 0.5f + 0.5f, 0.0f, 1.0f);
				for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
					if(inputs.commodity_type[j]) {
						register_demand(
							state,
							n,
							inputs.commodity_type[j],
							input_multiplier * throughput_multiplier * max_production_scale * inputs.commodity_amounts[j] * (0.1f + 0.9f * min_available),
							economy_reason::artisan
						);
					} else {
						break;
					}
				}
				state.world.nation_set_artisan_actual_production(n, cid, state.world.commodity_get_artisan_output_amount(cid) * throughput_multiplier * output_multiplier * max_production_scale * min_available);
				total_profit += std::max(0.0f, (output_total * output_multiplier - input_multiplier * input_total) * throughput_multiplier * max_production_scale * min_available);
			//}
		}
	}

	state.world.nation_set_artisan_profit(n, total_profit);
}

void update_national_artisan_production(sys::state& state, dcon::nation_id n) {
	auto const csize = state.world.commodity_size();
	auto num_artisans = state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.artisans));
	float total_profit = 0.0f;

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

		if(state.world.nation_get_artisan_distribution(n, cid) > 0.0f) {

			auto production = state.world.nation_get_artisan_actual_production(n, cid);
			if(production > 0) {
				auto& inputs = state.world.commodity_get_artisan_inputs(cid);

				float min_input = 1.0f;
				for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
					if(inputs.commodity_type[j]) {
						min_input = std::min(min_input, state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[j]));
					} else {
						break;
					}
				}

				auto amount = min_input * production;
				state.world.nation_set_artisan_actual_production(n, cid, amount);
				state.world.nation_get_domestic_market_pool(n, cid) += amount;
			}
		}
	}
}

void populate_army_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_army_demand(ids, cid, 0.0f); });
	}

	state.world.for_each_regiment([&](dcon::regiment_id r) {
		auto reg = fatten(state.world, r);
		auto type = state.world.regiment_get_type(r);
		auto owner = reg.get_army_from_army_membership().get_controller_from_army_control();

		if(owner) {
			auto o_sc_mod = std::max(0.01f, state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption) + 1.0f);
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					state.world.nation_get_army_demand(owner, supply_cost.commodity_type[i]) +=
						 supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(owner, type).supply_consumption *
						o_sc_mod;
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
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_navy_demand(ids, cid, 0.0f); });
	}

	state.world.for_each_ship([&](dcon::ship_id r) {
		auto shp = fatten(state.world, r);
		auto type = state.world.ship_get_type(r);
		auto owner = shp.get_navy_from_navy_membership().get_controller_from_navy_control();

		if(owner) {
			auto o_sc_mod = std::max(0.01f, state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption) + 1.0f);
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					state.world.nation_get_navy_demand(owner, supply_cost.commodity_type[i]) +=
						supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(owner, type).supply_consumption *
						o_sc_mod;
				} else {
					break;
				}
			}
		}
	});
}


// we want "cheaper per day"(= slower) construction at the start to avoid initial demand bomb
// and "more expensive"(=faster) construction at late game
inline constexpr float day_1_build_time_modifier_non_factory = 2.f;
inline constexpr float day_inf_build_time_modifier_non_factory = 0.5f;
inline constexpr float day_1_derivative_non_factory = -0.2f;

inline constexpr float diff_non_factory = day_1_build_time_modifier_non_factory - day_inf_build_time_modifier_non_factory;
inline constexpr float shift_non_factory = - diff_non_factory / day_1_derivative_non_factory;
inline constexpr float slope_non_factory = diff_non_factory * shift_non_factory;


float global_non_factory_construction_time_modifier(sys::state& state) {
	float t = math::sqrt((static_cast<float>(state.current_date.value) * 0.01f + 2.f));
	return day_inf_build_time_modifier_non_factory + slope_non_factory / (t + shift_non_factory);
}

inline constexpr float day_1_build_time_modifier_factory = 0.5f;
inline constexpr float day_inf_build_time_modifier_factory = 0.1f;
inline constexpr float day_1_derivative_factory = -0.1f;

inline constexpr float diff_factory = day_1_build_time_modifier_factory - day_inf_build_time_modifier_factory;
inline constexpr float shift_factory = -diff_factory / day_1_derivative_factory;
inline constexpr float slope_factory = diff_factory * shift_factory;

// also we want to speed up factories construction right at the start
// as it's the most vulnerable time for them
// and we need to establish *some* industrial base for world to develop
// their build time should also become faster with time to delay growth bottleneck
float global_factory_construction_time_modifier(sys::state& state) {
	float t = math::sqrt((static_cast<float>(state.current_date.value) * 0.01f + 2.f));
	return day_inf_build_time_modifier_factory + slope_factory / (t + shift_factory);
}

void populate_construction_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_construction_demand(ids, cid, 0.0f); });
	}

	for(auto lc : state.world.in_province_land_construction) {
		auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(lc));
		auto owner = state.world.province_get_nation_from_province_ownership(province);

		float admin_eff = state.world.nation_get_administrative_efficiency(owner);
		float admin_cost_factor = 2.0f - admin_eff;

		if(owner && state.world.province_get_nation_from_province_control(province) == owner) {

			auto& base_cost =
					state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(lc)].build_cost;
			auto& current_purchased = state.world.province_land_construction_get_purchased_goods(lc);
			float construction_time = global_non_factory_construction_time_modifier(state) *
					float(state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(lc)].build_time);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor)
						state.world.nation_get_construction_demand(owner, base_cost.commodity_type[i]) +=
								base_cost.commodity_amounts[i] / construction_time;
				} else {
					break;
				}
			}
		}
	}

	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(!owner || state.world.province_get_nation_from_province_control(p) != owner)
			return;
		{
			auto rng = state.world.province_get_province_naval_construction(p);
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());

				float admin_eff = state.world.nation_get_administrative_efficiency(owner);
				float admin_cost_factor = 2.0f - admin_eff;

				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = global_non_factory_construction_time_modifier(state) *
					float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor)
							state.world.nation_get_construction_demand(owner, base_cost.commodity_type[i]) +=
									base_cost.commodity_amounts[i] / construction_time;
					} else {
						break;
					}
				}
			}
		}
	});

	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		if(owner && c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control() && !c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) *
				float(state.economy_definitions.building_definitions[int32_t(t)].time);

			float admin_eff = state.world.nation_get_administrative_efficiency(owner);
			float admin_cost_factor = 2.0f - admin_eff;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor)
						state.world.nation_get_construction_demand(owner, base_cost.commodity_type[i]) +=
								base_cost.commodity_amounts[i] / construction_time;
				} else {
					break;
				}
			}
		}
	}

	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		if(owner && !c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) *
				float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_cost) + 1.0f;

			float admin_eff = state.world.nation_get_administrative_efficiency(owner);
			float admin_cost_factor = 2.0f - admin_eff;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod * admin_cost_factor)
						state.world.nation_get_construction_demand(owner, base_cost.commodity_type[i]) +=
								base_cost.commodity_amounts[i] * factory_mod / construction_time;
				} else {
					break;
				}
			}
		}
	}
}

void populate_private_construction_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_private_construction_demand(ids, cid, 0.0f); });
	}

	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		// Rationale for not checking building type: Its an invalid state; should not occur under normal circumstances
		if(owner && owner == c.get_province().get_nation_from_province_control() && c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) *
				float(state.economy_definitions.building_definitions[int32_t(t)].time);
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
						state.world.nation_get_private_construction_demand(owner, base_cost.commodity_type[i]) +=
								base_cost.commodity_amounts[i] / construction_time;
				} else {
					break;
				}
			}
		}
	}

	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		if(owner && c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) *
				float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_cost) + 1.0f) * std::max(0.1f, state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_owner_cost));

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod)
						state.world.nation_get_private_construction_demand(owner, base_cost.commodity_type[i]) +=
								base_cost.commodity_amounts[i] * factory_mod / construction_time;
				} else {
					break;
				}
			}
		}
	}
}

float full_spending_cost(sys::state& state, dcon::nation_id n, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices) {

	float total = 0.0f;
	float military_total = 0;

	uint32_t total_commodities = state.world.commodity_size();
	float l_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float n_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float c_spending = float(state.world.nation_get_construction_spending(n)) / 100.0f;

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		auto v = state.world.nation_get_army_demand(n, cid) * l_spending * effective_prices.get(cid);
		assert(std::isfinite(v) && v >= 0.0f);
		total += v;
		military_total += v;
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		auto v = state.world.nation_get_navy_demand(n, cid) * n_spending * effective_prices.get(cid);
		assert(std::isfinite(v) && v >= 0.0f);
		total += v;
		military_total += v;
	}
	assert(std::isfinite(total) && total >= 0.0f);
	state.world.nation_set_maximum_military_costs(n, military_total);

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		total += state.world.nation_get_construction_demand(n, cid) * c_spending * effective_prices.get(cid);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			total += difference * effective_prices.get(cid);
		}
	}
	assert(std::isfinite(total) && total >= 0.0f);

	auto overseas_factor = state.defines.province_overseas_penalty *  float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};

			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				total += overseas_factor * effective_prices.get(cid);
			}
		}
	}

	assert(std::isfinite(total) && total >= 0.0f);
	// direct payments to pops

	auto const a_spending = float(state.world.nation_get_administrative_spending(n)) / 100.0f * float(state.world.nation_get_administrative_spending(n)) / 100.0f;
	auto const s_spending = state.world.nation_get_administrative_efficiency(n) * float(state.world.nation_get_social_spending(n)) / 100.0f;
	auto const e_spending = float(state.world.nation_get_education_spending(n)) * float(state.world.nation_get_education_spending(n)) / 100.0f / 100.0f;
	auto const m_spending = float(state.world.nation_get_military_spending(n)) * float(state.world.nation_get_military_spending(n)) / 100.0f / 100.f;
	auto const p_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level);
	auto const unemp_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit);
	auto const di_spending = float(state.world.nation_get_domestic_investment_spending(n)) * float(state.world.nation_get_domestic_investment_spending(n)) / 100.0f / 100.0f;

	total += domestic_investment_multiplier * di_spending *
		(state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.capitalists))
			* state.world.nation_get_luxury_needs_costs(n, state.culture_definitions.capitalists)
		+ state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.aristocrat))
			* state.world.nation_get_luxury_needs_costs(n, state.culture_definitions.aristocrat))
		/ needs_scaling_factor;

	state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
		auto adj_pop_of_type = state.world.nation_get_demographics(n, demographics::to_key(state, pt)) / needs_scaling_factor;

		if(adj_pop_of_type <= 0)
			return;

		auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
		if(ln_type == culture::income_type::administration) {
			total += a_spending * adj_pop_of_type * state.world.nation_get_life_needs_costs(n, pt);
		} else if(ln_type == culture::income_type::education) {
			total += e_spending * adj_pop_of_type * state.world.nation_get_life_needs_costs(n, pt);
		} else if(ln_type == culture::income_type::military) {
			total += m_spending * adj_pop_of_type * state.world.nation_get_life_needs_costs(n, pt);
		} else { // unemployment, pensions
			total += s_spending * adj_pop_of_type * p_level * state.world.nation_get_life_needs_costs(n, pt);
			if(state.world.pop_type_get_has_unemployment(pt)) {
				auto emp = state.world.nation_get_demographics(n, demographics::to_employment_key(state, pt)) / needs_scaling_factor;
				total += s_spending * (adj_pop_of_type - emp) * unemp_level * state.world.nation_get_life_needs_costs(n, pt);
			}
		}

		auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
		if(en_type == culture::income_type::administration) {
			total += a_spending * adj_pop_of_type * state.world.nation_get_everyday_needs_costs(n, pt);
		} else if(en_type == culture::income_type::education) {
			total += e_spending * adj_pop_of_type * state.world.nation_get_everyday_needs_costs(n, pt);
		} else if(en_type == culture::income_type::military) {
			total += m_spending * adj_pop_of_type * state.world.nation_get_everyday_needs_costs(n, pt);
		}

		auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
		if(lx_type == culture::income_type::administration) {
			total += a_spending * adj_pop_of_type * state.world.nation_get_luxury_needs_costs(n, pt);
		} else if(lx_type == culture::income_type::education) {
			total += e_spending * adj_pop_of_type * state.world.nation_get_luxury_needs_costs(n, pt);
		} else if(lx_type == culture::income_type::military) {
			total += m_spending * adj_pop_of_type * state.world.nation_get_luxury_needs_costs(n, pt);
		}

		assert(std::isfinite(total) && total >= 0.0f);
	});

	assert(std::isfinite(total) && total >= 0.0f);

	return total;
}

float estimate_stockpile_filling_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			total += difference * state.world.commodity_get_current_price(cid) * state.world.nation_get_demand_satisfaction(n, cid);
		}
	}

	return total;
}

float estimate_overseas_penalty_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	uint32_t total_commodities = state.world.commodity_size();

	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				total += overseas_factor * state.world.commodity_get_current_price(cid) * state.world.nation_get_demand_satisfaction(n, cid);
			}
		}
	}

	return total;
}

float full_private_investment_cost(sys::state& state, dcon::nation_id n,
		ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices) {

	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		total += state.world.nation_get_private_construction_demand(n, cid) * effective_prices.get(cid);
	}
	return total;
}

void update_national_consumption(sys::state& state, dcon::nation_id n,
		ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices,
		float spending_scale, float private_investment_scale) {

	uint32_t total_commodities = state.world.commodity_size();

	float l_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float n_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float c_spending = float(state.world.nation_get_construction_spending(n)) / 100.0f;

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		register_demand(state, n, cid, state.world.nation_get_army_demand(n, cid) * l_spending * spending_scale, economy_reason::nation);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		register_demand(state, n, cid, state.world.nation_get_navy_demand(n, cid) * n_spending * spending_scale, economy_reason::nation);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		register_demand(state, n, cid, state.world.nation_get_construction_demand(n, cid) * c_spending * spending_scale, economy_reason::construction);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		register_demand(state, n, cid, state.world.nation_get_private_construction_demand(n, cid) * private_investment_scale, economy_reason::construction);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			register_demand(state, n, cid, difference * spending_scale, economy_reason::stockpile);
		}
	}

	auto overseas_factor = state.defines.province_overseas_penalty *
												 float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));

	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};

			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				register_demand(state, n, cid, overseas_factor * spending_scale, economy_reason::overseas_penalty);
			}
		}
	}
}

void update_pop_consumption(sys::state& state, dcon::nation_id n, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices, float base_demand, float invention_factor) {
	uint32_t total_commodities = state.world.commodity_size();

	static auto ln_demand_vector = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.execute_serial_over_pop_type([&](auto ids) { ln_demand_vector.set(ids, ve::fp_vector{}); });
	static auto en_demand_vector = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.execute_serial_over_pop_type([&](auto ids) { en_demand_vector.set(ids, ve::fp_vector{}); });
	static auto lx_demand_vector = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.execute_serial_over_pop_type([&](auto ids) { lx_demand_vector.set(ids, ve::fp_vector{}); });

	// needs_scaling_factor
	auto nation_rules = state.world.nation_get_combined_issue_rules(n);
	bool nation_allows_investment = state.world.nation_get_is_civilized(n) && (nation_rules & (issue_rule::pop_build_factory | issue_rule::pop_expand_factory)) != 0;
	for(auto p : state.world.nation_get_province_ownership(n)) {
		for(auto pl : state.world.province_get_pop_location(p.get_province())) {
			auto t = pl.get_pop().get_poptype();
			assert(t);
			auto total_budget = pl.get_pop().get_savings();
			auto total_pop = pl.get_pop().get_size();

			float ln_cost = state.world.nation_get_life_needs_costs(n, t) * total_pop / needs_scaling_factor;
			float en_cost = state.world.nation_get_everyday_needs_costs(n, t) * total_pop / needs_scaling_factor;
			float xn_cost = state.world.nation_get_luxury_needs_costs(n, t) * total_pop / needs_scaling_factor;

			float life_needs_fraction = total_budget >= ln_cost ? 1.0f : total_budget / ln_cost;
			total_budget -= ln_cost;
			

			//eliminate potential negative number before investment
			total_budget = std::max(total_budget, 0.f);

			//handle investment before everyday goods - they could be very hard to satisfy, depending on a mod:
			if(!nation_allows_investment || (t != state.culture_definitions.aristocrat && t != state.culture_definitions.capitalists)) {

			} else if(t == state.culture_definitions.capitalists) {
				state.world.nation_get_private_investment(n) += total_budget > 0.0f ? total_budget * 0.125f : 0.0f;
				total_budget -= total_budget * 0.125f;
			} else {
				state.world.nation_get_private_investment(n) += total_budget > 0.0f ? total_budget * 0.025f : 0.0f;
				total_budget -= total_budget * 0.025f;
			}

			float everyday_needs_fraction = total_budget >= en_cost ? 1.0f : std::max(0.0f, total_budget / en_cost);
			total_budget -= en_cost;

			float luxury_needs_fraction = total_budget >= xn_cost ? 1.0f : std::max(0.0f, total_budget / xn_cost);
			total_budget -= xn_cost;

			// induce demand across all categories
			// maybe we need some kind of banking and ability to save up money for future instead of spending them all...

			float total_cost = ln_cost + en_cost + xn_cost;

			if((total_budget > 0.f)) {
				float life_needs_budget		= total_budget * 0.20f;
				float everyday_needs_budget = total_budget * 0.40f;
				float luxury_needs_budget	= total_budget * 0.40f;

				float induced_life_needs_demand = life_needs_budget / std::max(0.001f, ln_cost);
				float induced_everyday_needs_demand = everyday_needs_budget / std::max(0.001f, en_cost);
				float induced_luxury_needs_demand = luxury_needs_budget / std::max(0.001f, xn_cost);

				life_needs_fraction += induced_life_needs_demand;
				everyday_needs_fraction += induced_everyday_needs_demand;
				luxury_needs_fraction += induced_luxury_needs_demand;
			}

			assert(std::isfinite(life_needs_fraction));
			assert(std::isfinite(everyday_needs_fraction));
			assert(std::isfinite(luxury_needs_fraction));

			state.world.pop_set_life_needs_satisfaction(pl.get_pop(), life_needs_fraction);
			state.world.pop_set_everyday_needs_satisfaction(pl.get_pop(), everyday_needs_fraction);
			state.world.pop_set_luxury_needs_satisfaction(pl.get_pop(), std::min(1.0f, luxury_needs_fraction));

			ln_demand_vector.get(t) += life_needs_fraction * total_pop / needs_scaling_factor;
			en_demand_vector.get(t) += everyday_needs_fraction * total_pop / needs_scaling_factor;
			lx_demand_vector.get(t) += luxury_needs_fraction * total_pop / needs_scaling_factor;

			assert(std::isfinite(ln_demand_vector.get(t)));
			assert(std::isfinite(en_demand_vector.get(t)));
			assert(std::isfinite(lx_demand_vector.get(t)));
		}
	}

	float ln_mul[] = {state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_life_needs) + 1.0f};
	float en_mul[] = {state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_everyday_needs) + 1.0f};
	float lx_mul[] = {
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
	};

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto kf = state.world.commodity_get_key_factory(cid);
		if(state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf))) {
			for(const auto t : state.world.in_pop_type) {
				auto strata = state.world.pop_type_get_strata(t);
				float life_weight = state.world.nation_get_life_needs_weights(n, cid);
				float everyday_weight = state.world.nation_get_everyday_needs_weights(n, cid);
				float luxury_weight = state.world.nation_get_luxury_needs_weights(n, cid);

				float base_life = state.world.pop_type_get_life_needs(t, cid);
				float base_everyday = state.world.pop_type_get_everyday_needs(t, cid);
				float base_luxury = state.world.pop_type_get_luxury_needs(t, cid);

				float dist_life = ln_demand_vector.get(t);
				float dist_everyday = en_demand_vector.get(t);
				float dist_luxury = lx_demand_vector.get(t);

				float demand_life = base_life * dist_life * base_demand * ln_mul[strata] * life_weight;
				float demand_everyday = base_everyday * dist_everyday * base_demand * invention_factor * en_mul[strata] * everyday_weight * en_extra_factor;
				float demand_luxury = base_luxury * dist_luxury * base_demand * invention_factor * lx_mul[strata] * luxury_weight * lx_extra_factor;

				register_demand(state, n, cid, demand_life, economy_reason::pop);
				register_demand(state, n, cid, demand_everyday,	economy_reason::pop);
				register_demand(state, n, cid, demand_luxury, economy_reason::pop);
			}
		}
	}
}

void populate_needs_costs(sys::state& state, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices,
		dcon::nation_id n, float base_demand, float invention_factor) {

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

	float ln_mul[] = { state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_life_needs) + 1.0f };
	float en_mul[] = { state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_everyday_needs) + 1.0f };
	float lx_mul[] = { state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_luxury_needs) + 1.0f };

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id c{dcon::commodity_id::value_base_t(i)};
		auto kf = state.world.commodity_get_key_factory(c);
		if(state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf))) {
			float effective_price = effective_prices.get(c);
			auto ln_weight = state.world.nation_get_life_needs_weights(n, c);
			auto en_weight = state.world.nation_get_everyday_needs_weights(n, c);
			auto lx_weight = state.world.nation_get_luxury_needs_weights(n, c);

			state.world.for_each_pop_type([&](dcon::pop_type_id ids) {
				auto ln_base = state.world.pop_type_get_life_needs(ids, c);
				auto ln = ln_base * effective_price * base_demand * ln_mul[state.world.pop_type_get_strata(ids)] * ln_weight;
				state.world.nation_set_life_needs_costs(n, ids, ln + state.world.nation_get_life_needs_costs(n, ids));
				auto en_base = state.world.pop_type_get_everyday_needs(ids, c);
				auto en = en_base * effective_price * base_demand * invention_factor * en_mul[state.world.pop_type_get_strata(ids)] * en_weight * en_extra_factor;
				state.world.nation_set_everyday_needs_costs(n, ids, en + state.world.nation_get_everyday_needs_costs(n, ids));
				auto lx_base = state.world.pop_type_get_luxury_needs(ids, c);
				auto lx = lx_base * effective_price * base_demand * invention_factor * lx_mul[state.world.pop_type_get_strata(ids)] * lx_weight * lx_extra_factor;
				state.world.nation_set_luxury_needs_costs(n, ids, lx + state.world.nation_get_luxury_needs_costs(n, ids));
				assert(std::isfinite(state.world.nation_get_life_needs_costs(n, ids)) && state.world.nation_get_life_needs_costs(n, ids) >= 0.f);
				assert(std::isfinite(state.world.nation_get_everyday_needs_costs(n, ids)) && state.world.nation_get_everyday_needs_costs(n, ids) >= 0.f);
				assert(std::isfinite(state.world.nation_get_luxury_needs_costs(n, ids)) && state.world.nation_get_luxury_needs_costs(n, ids) >= 0.f);
			});
		}
	}
}

void advance_construction(sys::state& state, dcon::nation_id n) {
	uint32_t total_commodities = state.world.commodity_size();

	float c_spending = state.world.nation_get_spending_level(n) * float(state.world.nation_get_construction_spending(n)) / 100.0f;
	float p_spending = state.world.nation_get_private_investment_effective_fraction(n);

	float refund_amount = 0.0f;

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id c{dcon::commodity_id::value_base_t(i)};
		auto d_sat = state.world.nation_get_demand_satisfaction(n, c);
		auto& nat_demand = state.world.nation_get_construction_demand(n, c);
		refund_amount += nat_demand * c_spending * (1.0f - d_sat) * state.world.commodity_get_current_price(c);
		nat_demand *= c_spending * d_sat;
		state.world.nation_get_private_construction_demand(n, c) *= p_spending * d_sat;
	}

	assert(refund_amount >= 0.0f);
	state.world.nation_get_stockpiles(n, economy::money) += refund_amount;

	float admin_eff = state.world.nation_get_administrative_efficiency(n);
	float admin_cost_factor = 2.0f - admin_eff;

	for(auto p : state.world.nation_get_province_ownership(n)) {
		if(p.get_province().get_nation_from_province_control() != n)
			continue;

		for(auto pops : p.get_province().get_pop_location()) {
			auto rng = pops.get_pop().get_province_land_construction();
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());
				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = global_non_factory_construction_time_modifier(state) *  float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor) {
							auto amount = base_cost.commodity_amounts[i] / construction_time;
							auto& source = state.world.nation_get_construction_demand(n, base_cost.commodity_type[i]);
							auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] / construction_time));
							current_purchased.commodity_amounts[i] += delta;
							source -= delta;
						}
					} else {
						break;
					}
				}
				break; // only advance one construction per province
			}
		}
		{
			auto rng = p.get_province().get_province_naval_construction();
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());
				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = global_non_factory_construction_time_modifier(state) * float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor) {
							auto amount = base_cost.commodity_amounts[i] / construction_time;
							auto& source = state.world.nation_get_construction_demand(n, base_cost.commodity_type[i]);
							auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] / construction_time));

							current_purchased.commodity_amounts[i] += delta;
							source -= delta;
						}
					} else {
						break;
					}
				}
			}
		}
	}

	for(auto c : state.world.nation_get_province_building_construction(n)) {
		if(c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control()) {
			auto t = economy::province_building_type(c.get_type());
			// Rationale for not checking the building type:
			// Pop projects created for forts and naval bases should NOT happen in the first place, so checking against them
			// is a waste of resources
			if(!c.get_is_pop_project()) {
				auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = global_non_factory_construction_time_modifier(state) * float(state.economy_definitions.building_definitions[int32_t(t)].time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor) {
							auto amount = base_cost.commodity_amounts[i] / construction_time;
							auto& source = state.world.nation_get_construction_demand(n, base_cost.commodity_type[i]);
							auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] / construction_time));

							current_purchased.commodity_amounts[i] += delta;
							source -= delta;
						}
					} else {
						break;
					}
				}
			} else if(c.get_is_pop_project()) {
				auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = global_non_factory_construction_time_modifier(state) * float(state.economy_definitions.building_definitions[int32_t(t)].time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i]) {
							auto amount = base_cost.commodity_amounts[i] / construction_time;
							auto& source = state.world.nation_get_private_construction_demand(n, base_cost.commodity_type[i]);
							auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] / construction_time));

							current_purchased.commodity_amounts[i] += delta;
							source -= delta;
						}
					} else {
						break;
					}
				}
			}
		}
	}

	for(auto c : state.world.nation_get_state_building_construction(n)) {
		if(!c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) * float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod * admin_cost_factor) {
						auto amount = base_cost.commodity_amounts[i] / construction_time;
						auto& source = state.world.nation_get_construction_demand(n, base_cost.commodity_type[i]);
						auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] * factory_mod / construction_time));

						current_purchased.commodity_amounts[i] += delta;
						source -= delta;
					}
				} else {
					break;
				}
			}
		} else {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) * float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f) *
													std::max(0.1f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_owner_cost));

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod) {
						auto amount = base_cost.commodity_amounts[i] / construction_time;
						auto& source = state.world.nation_get_private_construction_demand(n, base_cost.commodity_type[i]);
						auto delta = std::max(0.0f, std::min(source, base_cost.commodity_amounts[i] * factory_mod / construction_time));

						current_purchased.commodity_amounts[i] += delta;
						source -= delta;
					}
				} else {
					break;
				}
			}
		}
	}
}

float pop_min_wage_factor(sys::state& state, dcon::nation_id n) {
	return (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::minimum_wage) + 0.2f);
}

float pop_farmer_min_wage(sys::state& state, dcon::nation_id n, float min_wage_factor) {
	return (1.0f * state.world.nation_get_life_needs_costs(n, state.culture_definitions.farmers)
		+ 0.05f * state.world.nation_get_everyday_needs_costs(n, state.culture_definitions.farmers)) * min_wage_factor;
}

float pop_laborer_min_wage(sys::state& state, dcon::nation_id n, float min_wage_factor) {
	return (1.0f * state.world.nation_get_life_needs_costs(n, state.culture_definitions.laborers)
		+ 0.05f * state.world.nation_get_everyday_needs_costs(n, state.culture_definitions.laborers)) * min_wage_factor;
}

float pop_factory_min_wage(sys::state& state, dcon::nation_id n, float min_wage_factor) {
	return min_wage_factor * (
		state.world.nation_get_life_needs_costs(n, state.culture_definitions.primary_factory_worker)
		+ 0.05f * state.world.nation_get_everyday_needs_costs(n, state.culture_definitions.primary_factory_worker)
	);
}

void populate_effective_prices(
	sys::state& state,
	dcon::nation_id n,
	ve::vectorizable_buffer<float, dcon::commodity_id> & effective_prices
) {
	auto global_price_multiplier = global_market_price_multiplier(state, n);

	auto sl = state.world.nation_get_in_sphere_of(n);

	if(global_price_multiplier >= 1.0f) { // prefer domestic
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto domestic_supply =
				state.world.nation_get_domestic_market_pool(n, c) + (sl ? state.world.nation_get_domestic_market_pool(sl, c) : 0.0f) +
				(state.world.nation_get_drawing_on_stockpiles(n, c) ? state.world.nation_get_stockpiles(n, c) : 0.0f);
			auto global_supply = state.world.commodity_get_global_market_pool(c);
			auto last_demand = state.world.nation_get_real_demand(n, c);
			auto base_price = state.world.commodity_get_current_price(c);

			if(last_demand <= domestic_supply) {
				effective_prices.set(c, base_price);
			} else if(last_demand <= domestic_supply + global_supply) {
				auto domestic_satisfiable_fraction = domestic_supply / last_demand;
				effective_prices.set(c, base_price * domestic_satisfiable_fraction +
																		base_price * (1.0f - domestic_satisfiable_fraction) * global_price_multiplier);
			} else if(domestic_supply + global_supply > 0) {
				auto domestic_satisfiable_fraction = domestic_supply / (domestic_supply + global_supply);
				effective_prices.set(c, base_price * domestic_satisfiable_fraction +
																		base_price * (1.0f - domestic_satisfiable_fraction) * global_price_multiplier);
			} else {
				effective_prices.set(c, base_price * global_price_multiplier);
			}

			assert(std::isfinite(effective_prices.get(c)));
		});
	} else { // prefer global
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto domestic_supply =
				state.world.nation_get_domestic_market_pool(n, c) + (sl ? state.world.nation_get_domestic_market_pool(sl, c) : 0.0f) +
				(state.world.nation_get_drawing_on_stockpiles(n, c) ? state.world.nation_get_stockpiles(n, c) : 0.0f);
			auto global_supply = state.world.commodity_get_global_market_pool(c);
			auto last_demand = state.world.nation_get_real_demand(n, c);
			auto base_price = state.world.commodity_get_current_price(c);
			if(last_demand <= global_supply) {
				effective_prices.set(c, base_price);
			} else if(last_demand <= domestic_supply + global_supply) {
				auto global_satisfiable_fraction = global_supply / last_demand;
				effective_prices.set(c, base_price * global_satisfiable_fraction * global_price_multiplier +
																		base_price * (1.0f - global_satisfiable_fraction));
			} else if(domestic_supply + global_supply > 0) {
				auto global_satisfiable_fraction = global_supply / (domestic_supply + global_supply);
				effective_prices.set(c, base_price * global_satisfiable_fraction * global_price_multiplier +
																		base_price * (1.0f - global_satisfiable_fraction));
			} else {
				effective_prices.set(c, base_price);
			}

			assert(std::isfinite(effective_prices.get(c)));
		});
	}
}

void daily_update(sys::state& state) {

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
			populate_construction_consumption(state);
			break;
		case 3:
			populate_private_construction_consumption(state);
			break;
		case 4:
			update_factory_triggered_modifiers(state);
			break;
		case 5:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_nation([&](auto nids) {
					state.world.nation_set_everyday_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 6:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_nation([&](auto nids) {
					state.world.nation_set_luxury_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 7:
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				state.world.execute_serial_over_nation([&](auto nids) {
					state.world.nation_set_life_needs_costs(nids, t, ve::fp_vector{});
				});
			});
			break;
		case 8:
			state.world.execute_serial_over_nation([&](auto ids) {
				state.world.nation_set_subsidies_spending(ids, 0.0f);
			});
			break;
		case 9:
			state.world.execute_serial_over_nation([&](auto ids) {
				auto treasury = state.world.nation_get_stockpiles(ids, economy::money);
				state.world.nation_set_last_treasury(ids, treasury);
			});
			break;
		}
	});

	/* end initialization parallel block */

	auto const num_nation = state.world.nation_size();
	uint32_t total_commodities = state.world.commodity_size();

	/*
	As the day starts, we move production, fractionally, into the sphere leaders domestic production pool,
	following the same logic as Victoria 2
	*/

	for(auto n : state.nations_by_rank) {
		if(!n) // test for running out of sorted nations
			break;
		absorb_sphere_member_production(state, n); // no need for redundant checks here
	}
	for(auto n : state.nations_by_rank) {
		if(!n) // test for running out of sorted nations
			break;
		give_sphere_leader_production(state, n); // no need for redundant checks here
	}

	for(auto n : state.nations_by_rank) {
		if(!n) // test for running out of sorted nations
			break;
		/*
		### Calculate effective prices
		We will use the real demand from the *previous* day to determine how much of the purchasing will be done from the domestic
		and global pools (i.e. what percentage was able to be done from the cheaper pool). We will use that to calculate an
		effective price. And then, at the end of the current day, we will see how much of that purchasing actually came from each
		pool, etc. Depending on the stability of the simulation, we may, instead of taking the previous day, instead build this
		value iteratively as a linear combination of the new day and the previous day.

		when purchasing from global supply, prices are multiplied by (the nation's current effective tariff rate + its blockaded
		fraction
		+ 1)
		*/

		static auto effective_prices = state.world.commodity_make_vectorizable_float_buffer();
		populate_effective_prices(state, n, effective_prices);
		auto global_price_multiplier = global_market_price_multiplier(state, n);
		auto sl = state.world.nation_get_in_sphere_of(n);

		float base_demand =
				state.defines.base_goods_demand + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::goods_demand);

		int32_t num_inventions = 0;
		state.world.for_each_invention(
				[&](auto iid) { num_inventions += int32_t(state.world.nation_get_active_inventions(n, iid)); });
		float invention_factor = float(num_inventions) * state.defines.invention_impact_on_demand + 1.0f;

		populate_needs_costs(state, effective_prices, n, base_demand, invention_factor);

		float mobilization_impact = state.world.nation_get_is_mobilized(n) ? military::mobilization_impact(state, n) : 1.0f;

		auto const min_wage_factor = pop_min_wage_factor(state, n);
		float factory_min_wage = pop_factory_min_wage(state, n, min_wage_factor);
		float artisan_min_wage = (
			1.0f * state.world.nation_get_life_needs_costs(n, state.culture_definitions.artisans)
			+ 0.5f * state.world.nation_get_everyday_needs_costs(n, state.culture_definitions.artisans));
		float farmer_min_wage = pop_farmer_min_wage(state, n, min_wage_factor);
		float laborer_min_wage = pop_laborer_min_wage(state, n, min_wage_factor);

		// clear real demand
		state.world.for_each_commodity([&](dcon::commodity_id c) { state.world.nation_set_real_demand(n, c, 0.0f); });

		/*
		consumption updates
		*/

		static std::vector<dcon::commodity_id> artisan_prefs;
		//generate_national_artisan_prefs(state, n, artisan_prefs);

		auto cap_prov = state.world.nation_get_capital(n);
		auto cap_continent = state.world.province_get_continent(cap_prov);
		auto cap_region = state.world.province_get_connected_region_id(cap_prov);

		update_national_artisan_consumption(state, n, effective_prices, artisan_min_wage, mobilization_impact);

		for(auto p : state.world.nation_get_province_ownership(n)) {
			for(auto f : state.world.province_get_factory_location(p.get_province())) {
				// factory

				update_single_factory_consumption(
					state,
					f.get_factory(),
					n,
					p.get_province(),
					p.get_province().get_state_membership(),
					effective_prices,
					mobilization_impact,
					factory_min_wage,
					p.get_province().get_nation_from_province_control() != n, // is occupied
					p.get_province().get_connected_region_id() != cap_region
						&& p.get_province().get_continent() != cap_continent // is overseas
				);
			}

			// artisan
			//update_province_artisan_consumption(state, p.get_province(), n, effective_prices, mobilization_impact, artisan_min_wage,
			//		p.get_province().get_nation_from_province_control() != n, artisan_prefs);

			// rgo
			bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p.get_province()));
			update_province_rgo_consumption(state, p.get_province(), n, mobilization_impact,
					is_mine ? laborer_min_wage : farmer_min_wage, p.get_province().get_nation_from_province_control() != n);
		}

		update_pop_consumption(state, n, effective_prices, base_demand, invention_factor);

		{
			// update national spending
			//
			// step 1: figure out total
			float total = full_spending_cost(state, n, effective_prices);

			// step 2: limit to actual budget
			float budget = 0.0f;
			float spending_scale = 0.0f;
			if(state.world.nation_get_is_player_controlled(n)) {
				auto& sp = state.world.nation_get_stockpiles(n, economy::money);
				sp -= interest_payment(state, n);

				if(can_take_loans(state, n)) {
					budget = total;
					spending_scale = 1.0f;
				} else {
					budget = std::max(0.0f, state.world.nation_get_stockpiles(n, economy::money));
					spending_scale = (total < 0.001f || total <= budget) ? 1.0f : budget / total;
				}
			} else {
				budget = std::max(0.0f, state.world.nation_get_stockpiles(n, economy::money));
				spending_scale = (total < 0.001f || total <= budget) ? 1.0f : budget / total;
			}

			assert(spending_scale >= 0);
			assert(std::isfinite(spending_scale));
			assert(std::isfinite(budget));

			state.world.nation_get_stockpiles(n, economy::money) -= std::min(budget, total * spending_scale);
			state.world.nation_set_spending_level(n, spending_scale);

			float pi_total = full_private_investment_cost(state, n, effective_prices);
			float pi_budget = state.world.nation_get_private_investment(n);
			auto pi_scale = pi_total <= pi_budget ? 1.0f : pi_budget / pi_total;
			state.world.nation_set_private_investment_effective_fraction(n, pi_scale);
			state.world.nation_set_private_investment(n, std::max(0.0f, pi_budget - pi_total));

			update_national_consumption(state, n, effective_prices, spending_scale, pi_scale);
		}

		/*
		perform actual consumption / purchasing subject to availability
		*/

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{dcon::commodity_id::value_base_t(i)};

			auto dom_pool = state.world.nation_get_domestic_market_pool(n, c);
			auto sl_pool = (sl ? state.world.nation_get_domestic_market_pool(sl, c) : 0.0f);
			auto sp_pool = (state.world.nation_get_drawing_on_stockpiles(n, c) ? state.world.nation_get_stockpiles(n, c) : 0.0f);
			auto wm_pool = state.world.commodity_get_global_market_pool(c);

			auto total_supply = dom_pool + sl_pool + sp_pool + wm_pool;

			constexpr float sat_delay_factor = 0.95f;

			auto rd = state.world.nation_get_real_demand(n, c);
			auto old_sat = state.world.nation_get_demand_satisfaction(n, c);
			auto new_sat = rd > 0.0001f ? total_supply / rd : total_supply;
			auto adj_sat = old_sat * sat_delay_factor + new_sat * (1.0f - sat_delay_factor);
			state.world.nation_set_demand_satisfaction(n, c, std::min(1.0f, adj_sat));

			if(global_price_multiplier >= 1.0f) { // prefer domestic
				state.world.nation_set_domestic_market_pool(n, c, std::max(0.0f, dom_pool - rd));
				rd = std::max(rd - dom_pool, 0.0f);
				if(sl) {
					state.world.nation_set_domestic_market_pool(sl, c, std::max(0.0f, sl_pool - rd));
					rd = std::max(rd - sl_pool, 0.0f);
				}
				if(state.world.nation_get_drawing_on_stockpiles(n, c)) {
					state.world.nation_set_stockpiles(n, c, std::max(0.0f, sp_pool - rd));
					rd = std::max(rd - sp_pool, 0.0f);
				}
				state.world.commodity_set_global_market_pool(c, std::max(0.0f, wm_pool - rd));

				state.world.nation_set_imports(n, c, std::min(wm_pool, rd));
			} else {
				state.world.nation_set_imports(n, c, std::min(wm_pool, rd));

				state.world.commodity_set_global_market_pool(c, std::max(0.0f, wm_pool - rd));
				rd = std::max(rd - wm_pool, 0.0f);

				state.world.nation_set_domestic_market_pool(n, c, std::max(0.0f, dom_pool - rd));
				rd = std::max(rd - dom_pool, 0.0f);
				if(sl) {
					state.world.nation_set_domestic_market_pool(sl, c, std::max(0.0f, sl_pool - rd));
					rd = std::max(rd - sl_pool, 0.0f);
				}
				if(state.world.nation_get_drawing_on_stockpiles(n, c)) {
					state.world.nation_set_stockpiles(n, c, std::max(0.0f, sp_pool - rd));
				}
			}
		}
	}

	/*
	move remaining domestic supply to global pool, clear domestic market
	*/
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		// per good decay would be nice...
		float decay = 0.5f;
		float world_pool = state.world.commodity_get_global_market_pool(c) * decay;
		ve::fp_vector sum;
		state.world.execute_serial_over_nation([&](auto nids) {
			sum = sum + state.world.nation_get_domestic_market_pool(nids, c);
			state.world.nation_set_domestic_market_pool(nids, c, 0.0f);
		});
		state.world.commodity_set_global_market_pool(c, world_pool + sum.reduce());
	});

	/*
	pay non "employed" pops (also zeros money for "employed" pops)
	*/

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto owners = nations::owner_of_pop(state, ids);
		auto owner_spending = state.world.nation_get_spending_level(owners);

		auto pop_of_type = state.world.pop_get_size(ids);
		auto adj_pop_of_type = pop_of_type / needs_scaling_factor;

		auto const a_spending = owner_spending * ve::to_float(state.world.nation_get_administrative_spending(owners)) * ve::to_float(state.world.nation_get_administrative_spending(owners)) / 100.0f / 100.f;
		auto const s_spending = owner_spending * state.world.nation_get_administrative_efficiency(owners) *
														ve::to_float(state.world.nation_get_social_spending(owners)) / 100.0f;
		auto const e_spending = owner_spending * ve::to_float(state.world.nation_get_education_spending(owners)) * ve::to_float(state.world.nation_get_education_spending(owners)) / 100.0f / 100.f;
		auto const m_spending = owner_spending * ve::to_float(state.world.nation_get_military_spending(owners)) * ve::to_float(state.world.nation_get_military_spending(owners)) / 100.0f / 100.0f;
		auto const p_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::pension_level);
		auto const unemp_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::unemployment_benefit);
		auto const di_level = owner_spending * ve::to_float(state.world.nation_get_domestic_investment_spending(owners)) * ve::to_float(state.world.nation_get_domestic_investment_spending(owners)) / 100.0f / 100.f;

		auto types = state.world.pop_get_poptype(ids);

		auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
		auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
		auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

		auto ln_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::nation_id n) { return pt ? state.world.nation_get_life_needs_costs(n, pt) : 0.0f; },
				types, owners);
		auto en_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::nation_id n) { return pt ? state.world.nation_get_everyday_needs_costs(n, pt) : 0.0f; },
				types, owners);
		auto lx_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::nation_id n) { return pt ? state.world.nation_get_luxury_needs_costs(n, pt) : 0.0f; },
				types, owners);

		auto acc_a =
				ve::select(ln_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * ln_costs, 0.0f);
		auto acc_e = ve::select(ln_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * ln_costs, 0.0f);
		auto acc_m = ve::select(ln_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * ln_costs, 0.0f);

		auto none_of_above = ln_types != int32_t(culture::income_type::military) &&
												 ln_types != int32_t(culture::income_type::education) &&
												 ln_types != int32_t(culture::income_type::administration);

		auto acc_u = ve::select(none_of_above, s_spending * adj_pop_of_type * p_level * ln_costs, 0.0f);

		acc_a = acc_a + ve::select(en_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * en_costs, 0.0f);
		acc_e = acc_e + ve::select(en_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * en_costs, 0.0f);
		acc_m = acc_m + ve::select(en_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * en_costs, 0.0f);

		acc_u = acc_u + ve::select(types == state.culture_definitions.capitalists, di_level * adj_pop_of_type * domestic_investment_multiplier * lx_costs, 0.0f);
		acc_u = acc_u + ve::select(types == state.culture_definitions.aristocrat, di_level * adj_pop_of_type * domestic_investment_multiplier * lx_costs, 0.0f);

		acc_a = acc_a + ve::select(lx_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * lx_costs, 0.0f);
		acc_e = acc_e + ve::select(lx_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * lx_costs, 0.0f);
		acc_m = acc_m + ve::select(lx_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * lx_costs, 0.0f);

		acc_u = acc_u + ve::select(none_of_above && state.world.pop_type_get_has_unemployment(types),
												s_spending * (pop_of_type - state.world.pop_get_employment(ids)) / needs_scaling_factor * unemp_level * ln_costs, 0.0f);

		state.world.pop_set_savings(ids, state.inflation * ((acc_e + acc_m) + (acc_u + acc_a)));
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_e);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_m);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_u);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_a);
	});

	/*
	add up production, collect taxes and tariffs, other updates purely internal to each nation
	*/

	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		auto n = dcon::nation_id{dcon::nation_id::value_base_t(i)};

		if(state.world.nation_get_owned_province_count(n) == 0)
			return;

		/*
		prepare needs satisfaction caps
		*/

		ve::vectorizable_buffer<float, dcon::pop_type_id> ln_max = state.world.pop_type_make_vectorizable_float_buffer();
		ve::vectorizable_buffer<float, dcon::pop_type_id> en_max = state.world.pop_type_make_vectorizable_float_buffer();
		ve::vectorizable_buffer<float, dcon::pop_type_id> lx_max = state.world.pop_type_make_vectorizable_float_buffer();

		static auto effective_prices = state.world.commodity_make_vectorizable_float_buffer();
		populate_effective_prices(state, n, effective_prices);

		uint32_t total_commodities = state.world.commodity_size();

		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			float ln_total = 0.0f;
			float en_total = 0.0f;
			float lx_total = 0.0f;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{dcon::commodity_id::value_base_t(i)};
				auto kf = state.world.commodity_get_key_factory(c);
				if(state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf))) {
					auto sat = state.world.nation_get_demand_satisfaction(n, c);

					auto ln_val = state.world.pop_type_get_life_needs(pt, c) * state.world.nation_get_life_needs_weights(n, c);
					ln_total += ln_val;
					ln_max.get(pt) += ln_val * sat;

					auto en_val = state.world.pop_type_get_everyday_needs(pt, c) * state.world.nation_get_everyday_needs_weights(n, c);
					en_total += en_val;
					en_max.get(pt) += en_val * sat;

					auto lx_val = state.world.pop_type_get_luxury_needs(pt, c) * state.world.nation_get_luxury_needs_weights(n, c);
					lx_total += lx_val;
					lx_max.get(pt) += lx_val * sat;
				}
			}

			if(ln_total > 0)
				ln_max.get(pt) /= ln_total;
			else
				ln_max.get(pt) = 1.f;

			if(en_total > 0)
				en_max.get(pt) /= en_total;
			else
				en_max.get(pt) = 1.f;

			if(lx_total > 0)
				lx_max.get(pt) /= lx_total;
			else
				lx_max.get(pt) = 1.f;
		});

		/*
		determine effective spending levels
		*/
		auto nations_commodity_spending = state.world.nation_get_spending_level(n);
		float refund = 0.0f;
		{
			float max_sp = 0.0f;
			float total = 0.0f;
			float spending_level = float(state.world.nation_get_naval_spending(n)) / 100.0f;

			for(uint32_t k = 1; k < total_commodities; ++k) {
				dcon::commodity_id c{dcon::commodity_id::value_base_t(k)};

				auto sat = state.world.nation_get_demand_satisfaction(n, c);
				auto val = state.world.nation_get_navy_demand(n, c);
				refund += val * (1.0f - sat) * nations_commodity_spending * spending_level * state.world.commodity_get_current_price(c);
				total += val;
				max_sp += val * sat;
			}
			if(total > 0)
				max_sp /= total;

			state.world.nation_set_effective_naval_spending(n,
					nations_commodity_spending * max_sp * spending_level);
		}
		{
			float max_sp = 0.0f;
			float total = 0.0f;
			float spending_level = float(state.world.nation_get_land_spending(n)) / 100.0f;

			for(uint32_t k = 1; k < total_commodities; ++k) {
				dcon::commodity_id c{dcon::commodity_id::value_base_t(k)};

				auto sat = state.world.nation_get_demand_satisfaction(n, c);
				auto val = state.world.nation_get_army_demand(n, c);
				refund += val * (1.0f - sat) * nations_commodity_spending * spending_level * state.world.commodity_get_current_price(c);
				total += val;
				max_sp += val * sat;
			}
			if(total > 0)
				max_sp /= total;

			state.world.nation_set_effective_land_spending(n,
					nations_commodity_spending * max_sp * spending_level);
		}
		{
			float max_sp = 0.0f;
			float total = 0.0f;
			float spending_level = float(state.world.nation_get_construction_spending(n)) / 100.0f;

			for(uint32_t k = 1; k < total_commodities; ++k) {
				dcon::commodity_id c{dcon::commodity_id::value_base_t(k)};

				auto sat = state.world.nation_get_demand_satisfaction(n, c);
				auto val = state.world.nation_get_construction_demand(n, c);
				total += val;
				max_sp += val * sat;
			}
			if(total > 0)
				max_sp /= total;

			state.world.nation_set_effective_construction_spending(n,
					nations_commodity_spending * max_sp * spending_level);
		}
		/*
		fill stockpiles
		*/

		for(uint32_t k = 1; k < total_commodities; ++k) {
			dcon::commodity_id cid{dcon::commodity_id::value_base_t(k)};
			auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
			if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
				auto sat = state.world.nation_get_demand_satisfaction(n, cid);
				state.world.nation_get_stockpiles(n, cid) += difference * nations_commodity_spending * sat;
				refund += difference * (1.0f - sat) * nations_commodity_spending * state.world.commodity_get_current_price(cid);
			}
		}

		/*
		calculate overseas penalty
		*/

		{
			float count = 0.0f;
			float total = 0.0f;

			auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
			if(overseas_factor > 0) {
				for(uint32_t k = 1; k < total_commodities; ++k) {
					dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

					auto kf = state.world.commodity_get_key_factory(cid);
					if(state.world.commodity_get_overseas_penalty(cid) &&
							(state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
						auto sat = state.world.nation_get_demand_satisfaction(n, cid);
						total += sat;
						refund += overseas_factor * (1.0f - sat) * nations_commodity_spending * state.world.commodity_get_current_price(cid);
						count += 1.0f;
					}
				}
				state.world.nation_set_overseas_penalty(n, 0.25f * (1.0f - nations_commodity_spending * total / count));
			} else {
				state.world.nation_set_overseas_penalty(n, 0.0f);
			}
		}

		assert(std::isfinite(refund) && refund >= 0.0f);
		state.world.nation_get_stockpiles(n, money) += refund;

		auto const min_wage_factor = pop_min_wage_factor(state, n);

		float factory_min_wage = pop_factory_min_wage(state, n, min_wage_factor);
		float farmer_min_wage = pop_farmer_min_wage(state, n, min_wage_factor);
		float laborer_min_wage = pop_laborer_min_wage(state, n, min_wage_factor);

		update_national_artisan_production(state, n);

		for(auto p : state.world.nation_get_province_ownership(n)) {
			/*
			perform production
			*/

			for(auto f : state.world.province_get_factory_location(p.get_province())) {
				// factory
				update_single_factory_production(state, f.get_factory(), n, factory_min_wage);
			}

			// artisan
			//update_province_artisan_production(state, p.get_province(), n);

			// rgo
			update_province_rgo_production(state, p.get_province(), n);

			/*
			adjust pop satisfaction based on consumption
			*/

			for(auto pl : p.get_province().get_pop_location()) {
				auto t = pl.get_pop().get_poptype();

				auto ln = pl.get_pop().get_life_needs_satisfaction();
				pl.get_pop().set_life_needs_satisfaction(std::min(ln, ln_max.get(t)));

				auto en = pl.get_pop().get_everyday_needs_satisfaction();
				pl.get_pop().set_everyday_needs_satisfaction(std::min(en, en_max.get(t)));

				auto lx = pl.get_pop().get_luxury_needs_satisfaction();
				pl.get_pop().set_luxury_needs_satisfaction(std::min(lx, lx_max.get(t)));
			}
		}

		/*
		pay "employed" pops
		*/

		{
			// ARTISAN
			auto const artisan_type = state.culture_definitions.artisans;
			float artisan_profit = state.world.nation_get_artisan_profit(n);
			float num_artisans = state.world.nation_get_demographics(n, demographics::to_key(state, artisan_type));
			if(num_artisans > 0) {
				auto per_profit = artisan_profit / num_artisans;

				for(auto p : state.world.nation_get_province_ownership(n)) {
					for(auto pl : p.get_province().get_pop_location()) {
						if(artisan_type == pl.get_pop().get_poptype()) {
							pl.get_pop().set_savings(pl.get_pop().get_savings() + state.inflation * pl.get_pop().get_size() * per_profit);
							assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
						}
					}
				}
			}
		}

		/*
		pay factory workers / capitalists
		*/

		for(auto si : state.world.nation_get_state_ownership(n)) {
			float total_profit = 0;
			float rgo_owner_profit = 0;

			float num_rgo_owners = state.world.state_instance_get_demographics(si.get_state(),
					demographics::to_key(state, state.culture_definitions.aristocrat));

			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto f : state.world.province_get_factory_location(p)) {
					total_profit += std::max(0.f, f.get_factory().get_full_profit());
				}

				{
					// FARMER / LABORER
					bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));
					//auto const worker = is_mine ? state.culture_definitions.laborers : state.culture_definitions.farmers;

					auto const min_wage = (is_mine ? laborer_min_wage : farmer_min_wage) / needs_scaling_factor;

					float total_min_to_workers = 0.0f;
					float num_workers = 0.0f;
					for(auto wt : state.culture_definitions.rgo_workers) {
						total_min_to_workers += min_wage* state.world.province_get_demographics(p, demographics::to_employment_key(state, wt));
						num_workers += state.world.province_get_demographics(p, demographics::to_key(state, wt));
					}
					float total_rgo_profit = state.world.province_get_rgo_full_profit(p);
					float total_worker_wage = 0.0f;

					if(num_rgo_owners > 0) {
						// owners ALWAYS get some chunk of income
						rgo_owner_profit += 0.3f * total_rgo_profit;
						total_rgo_profit = 0.7f * total_rgo_profit;
					}

					if(total_min_to_workers <= total_rgo_profit && num_rgo_owners > 0) {
						total_worker_wage = total_min_to_workers + (total_rgo_profit - total_min_to_workers) * 0.2f;
						rgo_owner_profit += (total_rgo_profit - total_min_to_workers) * 0.8f;
					} else {
						total_worker_wage = total_rgo_profit;
					}

					auto per_worker_profit = num_workers > 0 ? total_worker_wage / num_workers : 0.0f;

					for(auto pl : state.world.province_get_pop_location(p)) {
						if(pl.get_pop().get_poptype().get_is_paid_rgo_worker()) {
							pl.get_pop().set_savings(pl.get_pop().get_savings() + state.inflation * pl.get_pop().get_size() * per_worker_profit);
							assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
						}
					}
				}
			});

			auto const per_rgo_owner_profit = num_rgo_owners > 0 ? rgo_owner_profit / num_rgo_owners : 0.0f;

			auto const min_wage = factory_min_wage / needs_scaling_factor;

			float total_min_to_pworkers =
					min_wage * state.world.state_instance_get_demographics(si.get_state(),
						demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
			float total_min_to_sworkers =
					min_wage * state.world.state_instance_get_demographics(si.get_state(),
						 demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));

			float num_pworkers = state.world.state_instance_get_demographics(si.get_state(),
					demographics::to_key(state, state.culture_definitions.primary_factory_worker));
			float num_sworkers = state.world.state_instance_get_demographics(si.get_state(),
					demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
			float num_owners = state.world.state_instance_get_demographics(si.get_state(),
					demographics::to_key(state, state.culture_definitions.capitalists));

			auto per_pworker_profit = 0.0f;
			auto per_sworker_profit = 0.0f;
			auto per_owner_profit = 0.0f;

			if(total_min_to_pworkers + total_min_to_sworkers <= total_profit && num_owners > 0) {
				auto surplus = total_profit - (total_min_to_pworkers + total_min_to_sworkers);
				per_pworker_profit = num_pworkers > 0 ? (total_min_to_pworkers + surplus * 0.1f) / num_pworkers : 0.0f;
				per_sworker_profit = num_sworkers > 0 ? (total_min_to_sworkers + surplus * 0.2f) / num_sworkers : 0.0f;
				per_owner_profit = (surplus * 0.7f) / num_owners;
			} else if(total_min_to_pworkers + total_min_to_sworkers <= total_profit && num_sworkers > 0) {
				auto surplus = total_profit - (total_min_to_pworkers + total_min_to_sworkers);
				per_pworker_profit = num_pworkers > 0 ? (total_min_to_pworkers + surplus * 0.5f) / num_pworkers : 0.0f;
				per_sworker_profit = num_sworkers > 0 ? (total_min_to_sworkers + surplus * 0.5f) / num_sworkers : 0.0f;
			} else if(total_min_to_pworkers + total_min_to_sworkers <= total_profit) {
				per_pworker_profit = num_pworkers > 0 ? total_profit / num_pworkers : 0.0f;
			} else if(num_pworkers + num_sworkers > 0) {
				per_pworker_profit = total_profit / (num_pworkers + num_sworkers);
				per_sworker_profit = total_profit / (num_pworkers + num_sworkers);
			}

			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto pl : state.world.province_get_pop_location(p)) {
					if(state.culture_definitions.primary_factory_worker == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(pl.get_pop().get_savings() + state.inflation * pl.get_pop().get_size() * per_pworker_profit);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.secondary_factory_worker == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(pl.get_pop().get_savings() + state.inflation * pl.get_pop().get_size() * per_sworker_profit);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.capitalists == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(pl.get_pop().get_savings() + state.inflation * pl.get_pop().get_size() * per_owner_profit);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.aristocrat == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(pl.get_pop().get_savings() + state.inflation * pl.get_pop().get_size() * per_rgo_owner_profit);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					}
				}
			});
		}

		/*
		advance construction
		*/

		advance_construction(state, n);

		/*
		collect and distribute money for private education
		*/

		auto edu_money = 0.f;
		auto adm_money = 0.f;

		auto const edu_adm_spending = 0.1f;
		auto const edu_adm_effect = 1.f - edu_adm_spending;
		auto const education_ratio = 0.8f;

		for(auto p : state.world.nation_get_province_ownership(n)) {
			auto province = p.get_province();
			if(state.world.province_get_nation_from_province_ownership(province) == state.world.province_get_nation_from_province_control(province)) {
				float current = 0;


				float local_teachers = 0.f;
				float local_managers = 0.f;

				for(auto pl : province.get_pop_location()) {
					auto pop = pl.get_pop();
					auto pt = pop.get_poptype();

					auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));

					if(ln_type == culture::income_type::administration)
						local_managers += pop.get_size();

					if(ln_type == culture::income_type::education)
						local_teachers += pop.get_size();
				}

				if(local_teachers + local_managers > 0)
					for(auto pl : province.get_pop_location()) {
						auto& pop_money = pl.get_pop().get_savings();
						current += pop_money;
						pop_money *= edu_adm_effect;
					}

				float local_education_ratio = education_ratio;
				if(local_managers == 0) {
					local_education_ratio = 1.f;
				}

				for(auto pl : province.get_pop_location()) {
					auto pop = pl.get_pop();
					auto pt = pop.get_poptype();

					auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));

					if(ln_type == culture::income_type::administration) {
						float ratio = pop.get_size() / local_managers;
						pop.set_savings(pop.get_savings() + current * (1.f - local_education_ratio) * ratio);

						adm_money += current * (1.f - local_education_ratio) * ratio;
					} else if(ln_type == culture::income_type::education) {
						float ratio = pop.get_size() / local_teachers;
						pop.set_savings(pop.get_savings() + current * local_education_ratio * ratio);

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

		auto const tax_eff = std::clamp(state.defines.base_country_tax_efficiency + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tax_efficiency), 0.1f, 1.0f);

		float total_poor_tax_base = 0.0f;
		float total_mid_tax_base = 0.0f;
		float total_rich_tax_base = 0.0f;

		auto const poor_effect = (1.0f - tax_eff * float(state.world.nation_get_poor_tax(n)) / 100.0f);
		auto const middle_effect = (1.0f - tax_eff * float(state.world.nation_get_middle_tax(n)) / 100.0f);
		auto const rich_effect = (1.0f - tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f);

		assert(poor_effect >= 0 && middle_effect >= 0 && rich_effect >= 0);

		for(auto p : state.world.nation_get_province_ownership(n)) {
			auto province = p.get_province();
			if(state.world.province_get_nation_from_province_ownership(province) == state.world.province_get_nation_from_province_control(province)) {
				for(auto pl : province.get_pop_location()) {
					auto& pop_money = pl.get_pop().get_savings();
					auto strata = culture::pop_strata(pl.get_pop().get_poptype().get_strata());
					if(strata == culture::pop_strata::poor) {
						total_poor_tax_base += pop_money;
						pop_money *= poor_effect;
					} else if(strata == culture::pop_strata::middle) {
						total_mid_tax_base += pop_money;
						pop_money *= middle_effect;
					} else if(strata == culture::pop_strata::rich) {
						total_rich_tax_base += pop_money;
						pop_money *= rich_effect;
					}
				}
			}
		}

		state.world.nation_set_total_rich_income(n, total_rich_tax_base);
		state.world.nation_set_total_middle_income(n, total_mid_tax_base);
		state.world.nation_set_total_poor_income(n, total_poor_tax_base);
		auto collected_tax = total_poor_tax_base * tax_eff * float(state.world.nation_get_poor_tax(n)) / 100.0f +
												 total_mid_tax_base * tax_eff * float(state.world.nation_get_middle_tax(n)) / 100.0f +
												 total_rich_tax_base * tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f;
		assert(std::isfinite(collected_tax));
		assert(collected_tax >= 0);
		state.world.nation_get_stockpiles(n, money) += collected_tax;

		{
			/*
			collect tariffs
			*/

			auto tariff_rate = effective_tariff_rate(state, n);
			float t_total = 0.0f;

			for(uint32_t k = 1; k < total_commodities; ++k) {
				dcon::commodity_id cid{dcon::commodity_id::value_base_t(k)};
				t_total += state.world.commodity_get_current_price(cid) * tariff_rate * state.world.nation_get_imports(n, cid);
			}
			assert(std::isfinite(t_total));
			assert(t_total >= 0);
			state.world.nation_get_stockpiles(n, money) += t_total;
		}

		// shift needs weights
		rebalance_needs_weights(state, n, effective_prices);

		adjust_artisan_balance(state, n);
	});

	/*
	adjust prices based on global production & consumption
	*/

	concurrency::parallel_for(uint32_t(1), total_commodities, [&](uint32_t k) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(k)};
		if(state.world.commodity_get_money_rgo(cid))
			return;

		float total_r_demand = 0.0f;
		float total_consumption = 0.0f;
		float total_production = 0.0f;

		state.world.for_each_nation([&](dcon::nation_id n) {
			total_r_demand += state.world.nation_get_real_demand(n, cid);
			total_consumption += state.world.nation_get_real_demand(n, cid) * state.world.nation_get_demand_satisfaction(n, cid);
			total_production += state.world.nation_get_domestic_market_pool(n, cid);
		});

		state.world.commodity_set_total_consumption(cid, total_consumption);
		state.world.commodity_set_total_real_demand(cid, total_r_demand);

		auto prior_production = state.world.commodity_get_total_production(cid);
		state.world.commodity_set_total_production(cid, total_production);

		float supply = prior_production + state.world.commodity_get_global_market_pool(cid) / 12.f;
		float demand = total_r_demand;

		auto base_price = state.world.commodity_get_cost(cid);
		auto current_price = state.world.commodity_get_current_price(cid);
		//float market_balance = demand - supply;

		float oversupply_factor = std::clamp(((supply + 2.0f) / (demand + 2.0f) - 1.f) * 10.f, 0.f, 10.f);
		float overdemand_factor = std::clamp(((demand + 2.0f) / (supply + 2.0f) - 1.f) * 10.f, 0.f, 10.f);

		float speed_modifer = (overdemand_factor - oversupply_factor);
		if(std::abs(overdemand_factor - overdemand_factor) < 1.f) {
			speed_modifer = (overdemand_factor - oversupply_factor) * (overdemand_factor - oversupply_factor) * (overdemand_factor - oversupply_factor);
		}

		float price_speed = 0.00005f * std::max(0.1f, current_price) * speed_modifer;
		current_price += price_speed;

		state.world.commodity_set_current_price(cid, std::clamp(current_price, 0.001f, 100000.0f));
	});

	/*
	* Enforce price floors
	*/

	/*
	for(auto cid : state.world.in_commodity) {
		if(cid.get_artisan_output_amount() > 0.0f) {
			float min_price = 0.0f;
			auto& inputs = cid.get_artisan_inputs();
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(inputs.commodity_type[i]) {
					min_price += state.world.commodity_get_current_price(inputs.commodity_type[i]) * inputs.commodity_amounts[i];
				} else {
					break;
				}
			}
			min_price /= cid.get_artisan_output_amount();
			auto current = cid.get_current_price();
			cid.set_current_price(0.9f * current + 0.1f * std::max(min_price * 0.6f, current));
		}
	}
	*/

	/*
	DIPLOMATIC EXPENSES
	*/

	for(auto n : state.world.in_nation) {
		for(auto uni : n.get_unilateral_relationship_as_source()) {
			if(uni.get_war_subsidies()) {
				auto target_m_costs = uni.get_target().get_maximum_military_costs() * state.defines.warsubsidies_percent;
				if(target_m_costs <= n.get_stockpiles(money)) {
					n.get_stockpiles(money) -= target_m_costs;
					uni.get_target().get_stockpiles(money) += target_m_costs;
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
				auto const tax_eff = std::clamp(state.defines.base_country_tax_efficiency + n.get_modifier_values(sys::national_mod_offsets::tax_efficiency), 0.1f, 1.0f);
				auto total_tax_base = n.get_total_rich_income() + n.get_total_middle_income() + n.get_total_poor_income();

				auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
				auto capped_payout = std::min(n.get_stockpiles(money), payout);
				assert(capped_payout >= 0.0f);

				n.get_stockpiles(money) -= capped_payout;
				uni.get_target().get_stockpiles(money) += capped_payout;
			}
		}
	}

	/*
	BANKRUPTCY
	*/
	for(auto n : state.world.in_nation) {
		auto m = n.get_stockpiles(money);
		if(m < 0) {
			if(m < -max_loan(state, n)) {
				go_bankrupt(state, n);
			}
		}
	}

	/*
	update inflation
	*/

	float primary_commodity_basket = 0.0f;
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			primary_commodity_basket += 1.0f * state.world.commodity_get_cost(c) * state.world.pop_type_get_life_needs(pt, c);
			primary_commodity_basket +=  1.0f * state.world.commodity_get_cost(c) * state.world.pop_type_get_everyday_needs(pt, c);
		});
	});
	primary_commodity_basket /= float(state.world.pop_type_size());

	float total_pop = 0.0f;
	float total_pop_money = 0.0f;

	state.world.for_each_nation([&](dcon::nation_id n) {
		total_pop += state.world.nation_get_demographics(n, demographics::total);
		total_pop_money += state.world.nation_get_total_rich_income(n) + state.world.nation_get_total_middle_income(n) +
											 state.world.nation_get_total_poor_income(n);
	});

	float target_money = total_pop * primary_commodity_basket / needs_scaling_factor;

	// total_pop_money / inflation = real, unadjustead money
	// want inflation s.t. inflation * r_money = target_money
	// ideal inflation = inflation * target_money / total_pop_money

	if(total_pop_money > 0.001f) {
		state.inflation = (state.inflation * 0.9f) + (0.1f * target_money / total_pop_money);
	}

	// make constructions:
	resolve_constructions(state);

	// make new investments
	for(auto n : state.world.in_nation) {
		auto nation_rules = n.get_combined_issue_rules();

		if(n.get_private_investment() > 0.001f
			&& n.get_is_civilized()
			&& (nation_rules & (issue_rule::pop_build_factory | issue_rule::pop_expand_factory)) != 0) {

			static std::vector<dcon::factory_type_id> desired_types;
			desired_types.clear();
					

			static std::vector<dcon::state_instance_id> states_in_order;
			states_in_order.clear();
			for(auto si : n.get_state_ownership()) {
				if(si.get_state().get_capital().get_is_colonial() == false) {
					states_in_order.push_back(si.get_state().id);
				}
			}
			std::sort(states_in_order.begin(), states_in_order.end(), [&](dcon::state_instance_id a, dcon::state_instance_id b) {
				auto a_pop = state.world.state_instance_get_demographics(a, demographics::total);
				auto b_pop = state.world.state_instance_get_demographics(b, demographics::total);
				if(a_pop != b_pop)
					return a_pop > b_pop;
				return a.index() < b.index(); // force total ordering
			});

			if(!states_in_order.empty() && (nation_rules & issue_rule::pop_build_factory) != 0) {
				ai::get_desired_factory_types(state, n, desired_types);
			}

			//upgrade all good targets!!!
			//upgrading only one per run is too slow and leads to massive unemployment!!!

			for(auto s : states_in_order) {
				auto pw_num = state.world.state_instance_get_demographics(s,
						demographics::to_key(state, state.culture_definitions.primary_factory_worker));
				auto pw_employed = state.world.state_instance_get_demographics(s,
						demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));

				if(pw_employed >= pw_num && pw_num > 0.0f)
					continue; // no spare workers

				int32_t num_factories = 0;
				float profit = 0.0f;
				dcon::factory_id selected_factory;

				// is there an upgrade target ?
				auto d = state.world.state_instance_get_definition(s);
				for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
					if(p.get_province().get_nation_from_province_ownership() == n) {
						for(auto f : p.get_province().get_factory_location()) {
							++num_factories;

							if(
								(nation_rules & issue_rule::pop_expand_factory) != 0
								&& f.get_factory().get_production_scale() >= 0.9f
								&& f.get_factory().get_primary_employment() >= 0.9f
								&& f.get_factory().get_level() < uint8_t(255))
							{

								auto type = f.get_factory().get_building_type();
								auto ug_in_progress = false;
								for(auto c : state.world.state_instance_get_state_building_construction(s)) {
									if(c.get_type() == type) {
										ug_in_progress = true;
										break;
									}
								}

								if(ug_in_progress) {
									continue;
								}

								if(auto new_p = f.get_factory().get_full_profit() / f.get_factory().get_level(); new_p > profit) {
									profit = new_p;
									selected_factory = f.get_factory();
								}
							}
						}
					}
				}
				if(selected_factory && profit > 0.f) {
					auto new_up = fatten(state.world, state.world.force_create_state_building_construction(s, n));
					new_up.set_is_pop_project(true);
					new_up.set_is_upgrade(true);
					new_up.set_type(state.world.factory_get_building_type(selected_factory));
				}

				//try to invest into something new...
				//bool found_investment = false;
				auto existing_constructions = state.world.state_instance_get_state_building_construction(s);
				if(existing_constructions.begin() != existing_constructions.end())
					continue; // already building
				
				if((num_factories < int32_t(state.defines.factories_per_state)) && (nation_rules & issue_rule::pop_build_factory) != 0) {
					// randomly try a valid (check coastal, unlocked, non existing) factory
					if(!desired_types.empty()) {
						auto selected = desired_types[rng::get_random(state, uint32_t((n.id.index() << 6) ^ s.index())) % desired_types.size()];

						if(state.world.factory_type_get_is_coastal(selected) && !province::state_is_coastal(state, s))
							continue;

						bool already_in_progress = [&]() {
							for(auto p : state.world.state_instance_get_state_building_construction(s)) {
								if(p.get_type() == selected)
									return true;
							}
							return false;
						}();

						if(already_in_progress)
							continue;


						bool present_in_location = false;
						province::for_each_province_in_state_instance(state, s, [&](dcon::province_id p) {
							for(auto fac : state.world.province_get_factory_location(p)) {
								auto type = fac.get_factory().get_building_type();
								if(selected == type) {
									present_in_location = true;
									return;
								}
							}
						});

						if(present_in_location)
							continue;

						auto new_up = fatten(state.world, state.world.force_create_state_building_construction(s, n));
						new_up.set_is_pop_project(true);
						new_up.set_is_upgrade(false);
						new_up.set_type(selected);
						//found_investment = true;
					}
				}		
			}
			

			if((nation_rules & issue_rule::pop_build_factory) != 0) {

				static std::vector<std::pair<dcon::province_id, int32_t>> provinces_in_order;
				provinces_in_order.clear();
				for(auto si : n.get_state_ownership()) {
					if(si.get_state().get_capital().get_is_colonial() == false) {
						auto s = si.get_state().id;
						auto d = state.world.state_instance_get_definition(s);
						int32_t num_factories = 0;
						for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
							if(province::generic_can_build_railroads(state, p.get_province(), n) &&
									p.get_province().get_nation_from_province_ownership() == n) {
								for(auto f : p.get_province().get_factory_location())
									num_factories += int32_t(f.get_factory().get_level());
								provinces_in_order.emplace_back(p.get_province().id, num_factories);
							}
						}
						// The state's number of factories is intentionally given to all the provinces within the state so the
						// railroads aren't just built on a single province within a state
						for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
							if(province::generic_can_build_railroads(state, p.get_province(), n) &&
									p.get_province().get_nation_from_province_ownership() == n)
								provinces_in_order.emplace_back(p.get_province().id, num_factories);
						}
					}
				}
				if(!provinces_in_order.empty()) {
					std::pair<dcon::province_id, int32_t> best_p = provinces_in_order[0];
					for(auto e : provinces_in_order)
						if(e.second > best_p.second)
							best_p = e;

					auto new_rr = fatten(state.world, state.world.force_create_province_building_construction(best_p.first, n));
					new_rr.set_is_pop_project(true);
					new_rr.set_type(uint8_t(province_building_type::railroad));
					//found_investment = true;
				}
			}
		}
		n.set_private_investment(0.0f);
	}
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


	state.world.nation_resize_life_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_everyday_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_luxury_needs_costs(state.world.pop_type_size());

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto fc = fatten(state.world, c);
		state.world.commodity_set_key_factory(c, dcon::factory_type_id{});
		if(fc.get_total_production() > 0.0001f) {
			fc.set_producer_payout_fraction(std::min(fc.get_total_consumption() / fc.get_total_production(), 1.0f));
		} else {
			fc.set_producer_payout_fraction(1.0f);
		}
	});
	state.world.for_each_factory_type([&](dcon::factory_type_id t) {
		auto o = state.world.factory_type_get_output(t);
		if(o)
			state.world.commodity_set_key_factory(o, t);
	});
}

float government_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto overseas_factor = state.defines.province_overseas_penalty *
												 float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	auto o_adjust = 0.0f;
	if(overseas_factor > 0) {
		auto kf = state.world.commodity_get_key_factory(c);
		if(state.world.commodity_get_overseas_penalty(c) &&
				(state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
			o_adjust = overseas_factor;
		}
	}

	return (state.world.nation_get_army_demand(n, c) + state.world.nation_get_navy_demand(n, c) +
					state.world.nation_get_construction_demand(n, c) + o_adjust);
}

float nation_factory_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto amount = 0.f;
	return amount;
}

float nation_pop_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto amount = 0.f;
	auto kf = state.world.commodity_get_key_factory(c);
	if(state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf))) {
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			amount += (state.world.pop_type_get_life_needs(pt, c) + state.world.pop_type_get_everyday_needs(pt, c) +
										state.world.pop_type_get_luxury_needs(pt, c)) *
								state.world.nation_get_demographics(n, demographics::to_key(state, pt)) / needs_scaling_factor;
		});
	}
	return amount;
}

float nation_total_imports(sys::state& state, dcon::nation_id n) {
	float t_total = 0.0f;

	auto const total_commodities = state.world.commodity_size();
	for(uint32_t k = 1; k < total_commodities; ++k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		t_total += state.world.commodity_get_current_price(cid) * state.world.nation_get_imports(n, cid);
	}

	return t_total;
}

float pop_income(sys::state& state, dcon::pop_id p) {

	auto saved = state.world.pop_get_savings(p);

	if(saved <= 0.0f)
		return 0.0f;

	auto owner = nations::owner_of_pop(state, p);
	auto const tax_eff = std::clamp(state.defines.base_country_tax_efficiency + state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::tax_efficiency), 0.1f, 1.0f);
	auto strata = culture::pop_strata(state.world.pop_type_get_strata(state.world.pop_get_poptype(p)));

	switch(strata) {
	default:
	case culture::pop_strata::poor:
		return saved / std::max(0.0001f, (1.0f - tax_eff * float(state.world.nation_get_poor_tax(owner)) / 100.0f));
	case culture::pop_strata::middle:
		return saved / std::max(0.0001f, (1.0f - tax_eff * float(state.world.nation_get_middle_tax(owner)) / 100.0f));
	case culture::pop_strata::rich:
		return saved / std::max(0.0001f, (1.0f - tax_eff * float(state.world.nation_get_rich_tax(owner)) / 100.0f));
	}
}

float estimate_gold_income(sys::state& state, dcon::nation_id n) {
	auto amount = 0.f;
	for(auto poid : state.world.nation_get_province_ownership_as_nation(n)) {
		auto prov = poid.get_province();
		if(prov.get_rgo().get_money_rgo()) {
			amount += province::rgo_production_quantity(state, prov.id);
		}
	}
	return amount * state.defines.gold_to_cash_rate;
}

float estimate_tariff_income(sys::state& state, dcon::nation_id n) {
	return nations::tariff_efficiency(state, n) * economy::nation_total_imports(state, n);
}

float estimate_social_spending(sys::state& state, dcon::nation_id n) {
	auto total = 0.f;
	auto const p_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level);
	auto const unemp_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit);

	state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
		auto adj_pop_of_type =
				state.world.nation_get_demographics(n, demographics::to_key(state, pt)) / economy::needs_scaling_factor;

		if(adj_pop_of_type <= 0)
			return;

		auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
		if(ln_type == culture::income_type::administration || ln_type == culture::income_type::education || ln_type == culture::income_type::military) {
			//nothing
		} else { // unemployment, pensions
			total +=  adj_pop_of_type * p_level * state.world.nation_get_life_needs_costs(n, pt);
			if(state.world.pop_type_get_has_unemployment(pt)) {
				auto emp = state.world.nation_get_demographics(n, demographics::to_employment_key(state, pt)) / needs_scaling_factor;
				total +=  (adj_pop_of_type - emp) * unemp_level * state.world.nation_get_life_needs_costs(n, pt);
			}
		}
	});
	return total;
}

float estimate_pop_payouts_by_income_type(sys::state& state, dcon::nation_id n, culture::income_type in) {
	auto total = 0.f;
	state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
		auto adj_pop_of_type =
				state.world.nation_get_demographics(n, demographics::to_key(state, pt)) / economy::needs_scaling_factor;

		if(adj_pop_of_type <= 0)
			return;

		auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
		if(ln_type == in) {
			total += adj_pop_of_type * state.world.nation_get_life_needs_costs(n, pt);
		}

		auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
		if(en_type == in) {
			total += adj_pop_of_type * state.world.nation_get_everyday_needs_costs(n, pt);
		}

		auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
		if(lx_type == in) {
			total += adj_pop_of_type * state.world.nation_get_luxury_needs_costs(n, pt);
		}
	});
	return total;
}

float estimate_tax_income_by_strata(sys::state& state, dcon::nation_id n, culture::pop_strata ps) {
	auto const tax_eff = std::clamp(state.defines.base_country_tax_efficiency + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tax_efficiency), 0.1f, 1.0f);

	switch(ps) {
	default:
	case culture::pop_strata::poor:
		return state.world.nation_get_total_poor_income(n) * tax_eff;
	case culture::pop_strata::middle:
		return state.world.nation_get_total_middle_income(n) * tax_eff;
	case culture::pop_strata::rich:
		return state.world.nation_get_total_rich_income(n) * tax_eff;
	}
}

float estimate_subsidy_spending(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_subsidies_spending(n);
}

float estimate_war_subsidies_income(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	for(auto uni : state.world.nation_get_unilateral_relationship_as_target(n)) {
		if(uni.get_war_subsidies()) {
			total += uni.get_target().get_maximum_military_costs() * state.defines.warsubsidies_percent;
		}
	}
	return total;
}
float estimate_reparations_income(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	if(state.current_date < state.world.nation_get_reparations_until(n)) {
		for(auto uni : state.world.nation_get_unilateral_relationship_as_target(n)) {
			if(uni.get_reparations()) {
				auto source = uni.get_source();
				auto const tax_eff =
					std::clamp(
					state.defines.base_country_tax_efficiency + state.world.nation_get_modifier_values(source, sys::national_mod_offsets::tax_efficiency),
					0.1f, 1.0f);
				auto total_tax_base = state.world.nation_get_total_rich_income(source) +
					state.world.nation_get_total_middle_income(source) +
					state.world.nation_get_total_poor_income(source);

				auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
				total += payout;
			}
		}
	}

	return total;
}

float estimate_war_subsidies_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	for(auto uni : state.world.nation_get_unilateral_relationship_as_source(n)) {
		if(uni.get_war_subsidies()) {
			total += uni.get_target().get_maximum_military_costs() * state.defines.warsubsidies_percent;
		}
	}

	return total;
}

float estimate_reparations_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	if(state.current_date < state.world.nation_get_reparations_until(n)) {
		for(auto uni : state.world.nation_get_unilateral_relationship_as_source(n)) {
			if(uni.get_reparations()) {
				auto const tax_eff = std::clamp(state.defines.base_country_tax_efficiency + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tax_efficiency), 0.1f, 1.0f);
				auto total_tax_base = state.world.nation_get_total_rich_income(n) + state.world.nation_get_total_middle_income(n) + state.world.nation_get_total_poor_income(n);

				auto payout = total_tax_base * tax_eff * state.defines.reparations_tax_hit;
				total += payout;
			}
		}
	}

	return total;
}

float estimate_diplomatic_balance(sys::state& state, dcon::nation_id n) {
	return estimate_war_subsidies_income(state, n) + estimate_reparations_income(state, n) - estimate_war_subsidies_spending(state, n) -
		estimate_reparations_spending(state, n);
}

float estimate_domestic_investment(sys::state& state, dcon::nation_id n) {
	auto adj_pop_of_type_capis = (state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.capitalists))) / economy::needs_scaling_factor;
	auto adj_pop_of_type_arist = (state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.aristocrat))) / economy::needs_scaling_factor;

	float arist_costs =
		state.world.nation_get_life_needs_costs(n, state.culture_definitions.aristocrat)
		+ state.world.nation_get_everyday_needs_costs(n, state.culture_definitions.aristocrat)
		+ state.world.nation_get_luxury_needs_costs(n, state.culture_definitions.aristocrat);

	float capis_costs =
		state.world.nation_get_life_needs_costs(n, state.culture_definitions.capitalists)
		+ state.world.nation_get_everyday_needs_costs(n, state.culture_definitions.capitalists)
		+ state.world.nation_get_luxury_needs_costs(n, state.culture_definitions.capitalists);

	return domestic_investment_multiplier
	* (
		adj_pop_of_type_capis * capis_costs
		+ adj_pop_of_type_arist * arist_costs
	);
}

float estimate_land_spending(sys::state& state, dcon::nation_id n) {
	static auto effective_prices = state.world.commodity_make_vectorizable_float_buffer();
	populate_effective_prices(state, n, effective_prices);

	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		total += state.world.nation_get_army_demand(n, cid) * effective_prices.get(cid) * state.world.nation_get_demand_satisfaction(n, cid);
	}
	return total;
}

float estimate_naval_spending(sys::state& state, dcon::nation_id n) {
	static auto effective_prices = state.world.commodity_make_vectorizable_float_buffer();
	populate_effective_prices(state, n, effective_prices);

	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{dcon::commodity_id::value_base_t(i)};
		total += state.world.nation_get_navy_demand(n, cid) * effective_prices.get(cid) * state.world.nation_get_demand_satisfaction(n, cid);
	}
	return total;
}

float estimate_construction_spending(sys::state& state, dcon::nation_id n) {
	static auto effective_prices = state.world.commodity_make_vectorizable_float_buffer();
	populate_effective_prices(state, n, effective_prices);

	float total = 0.0f;
	float admin_eff = state.world.nation_get_administrative_efficiency(n);
	float admin_cost_factor = 2.0f - admin_eff;

	for(auto lc : state.world.nation_get_province_land_construction(n)) {
		auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(lc));
		if(state.world.province_get_nation_from_province_control(province) == n) {

			auto& base_cost = state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(lc)].build_cost;
			auto& current_purchased = state.world.province_land_construction_get_purchased_goods(lc);
			float construction_time = global_non_factory_construction_time_modifier(state) * float(state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(lc)].build_time);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor)
						total += effective_prices.get(base_cost.commodity_type[i]) * state.world.nation_get_demand_satisfaction(n, base_cost.commodity_type[i]) * base_cost.commodity_amounts[i] / construction_time;
				} else {
					break;
				}
			}
		}
	}

	for(auto po : state.world.nation_get_province_ownership(n)) {
		auto p = po.get_province();
		if(state.world.province_get_nation_from_province_control(p) != n)
			continue;

		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() != rng.end()) {
			auto c = *(rng.begin());
			auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) * float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor)
						total += effective_prices.get(base_cost.commodity_type[i]) * state.world.nation_get_demand_satisfaction(n, base_cost.commodity_type[i]) * base_cost.commodity_amounts[i] / construction_time;
				} else {
					break;
				}
			}
		}
	}

	for(auto c : state.world.nation_get_province_building_construction(n)) {
		if(n == c.get_province().get_nation_from_province_control() && !c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) * float(state.economy_definitions.building_definitions[int32_t(t)].time);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor)
						total += effective_prices.get(base_cost.commodity_type[i]) * state.world.nation_get_demand_satisfaction(n, base_cost.commodity_type[i]) * base_cost.commodity_amounts[i] / construction_time;
				} else {
					break;
				}
			}
		}
	}

	float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

	for(auto c : state.world.nation_get_state_building_construction(n)) {
		if(!c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) * float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod * admin_cost_factor)
						total += effective_prices.get(base_cost.commodity_type[i]) * state.world.nation_get_demand_satisfaction(n, base_cost.commodity_type[i]) * base_cost.commodity_amounts[i] * factory_mod / construction_time;
				} else {
					break;
				}
			}
		}
	}

	return total;
}

float estimate_war_subsidies(sys::state& state, dcon::nation_id n) {
	/* total-nation-expenses x defines:WARSUBSIDIES_PERCENT */
	return state.world.nation_get_maximum_military_costs(n) * state.defines.warsubsidies_percent;
}

construction_status province_building_construction(sys::state& state, dcon::province_id p, province_building_type t) {
	for(auto pb_con : state.world.province_get_province_building_construction(p)) {
		if(pb_con.get_type() == uint8_t(t)) {
			float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_get_nation_from_province_ownership(p));
			float admin_cost_factor = pb_con.get_is_pop_project() ? 1.0f : 2.0f - admin_eff;

			float total = 0.0f;
			float purchased = 0.0f;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += state.economy_definitions.building_definitions[int32_t(t)].cost.commodity_amounts[i] * admin_cost_factor;
				purchased += pb_con.get_purchased_goods().commodity_amounts[i];
			}
			return construction_status{total > 0.0f ? purchased / total : 0.0f, true};
		}
	}
	return construction_status{0.0f, false};
}

construction_status factory_upgrade(sys::state& state, dcon::factory_id f) {
	auto in_prov = state.world.factory_get_province_from_factory_location(f);
	auto in_state = state.world.province_get_state_membership(in_prov);
	auto fac_type = state.world.factory_get_building_type(f);

	for(auto st_con : state.world.state_instance_get_state_building_construction(in_state)) {
		if(st_con.get_type() == fac_type) {
			float admin_eff = state.world.nation_get_administrative_efficiency(st_con.get_nation());
			float factory_mod = state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_cost) + 1.0f;
			float pop_factory_mod = std::max(0.1f, state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_owner_cost));
			float admin_cost_factor = (st_con.get_is_pop_project() ? pop_factory_mod : (2.0f - admin_eff)) * factory_mod;


			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(fac_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i] * admin_cost_factor;
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			return construction_status{total > 0.0f ? purchased / total : 0.0f, true};
		}
	}

	return construction_status{0.0f, false};
}

bool state_contains_constructed_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft) {
	auto d = state.world.state_instance_get_definition(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_state_membership() == s) {
			for(auto f : p.get_province().get_factory_location()) {
				if(f.get_factory().get_building_type() == ft)
					return true;
			}
		}
	}
	return false;
}

bool state_contains_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft) {
	auto d = state.world.state_instance_get_definition(s);

	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_state_membership() == s) {
			for(auto f : p.get_province().get_factory_location()) {
				if(f.get_factory().get_building_type() == ft)
					return true;
			}
		}
	}
	for(auto sc : state.world.state_instance_get_state_building_construction(s)) {
		if(sc.get_type() == ft)
			return true;
	}

	return false;
}

int32_t state_factory_count(sys::state& state, dcon::state_instance_id sid, dcon::nation_id n) {
	int32_t num_factories = 0;
	auto d = state.world.state_instance_get_definition(sid);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d))
		if(p.get_province().get_nation_from_province_ownership() == n)
			num_factories += int32_t(state.world.province_get_factory_location(p.get_province()).end() - state.world.province_get_factory_location(p.get_province()).begin());
	for(auto p : state.world.state_instance_get_state_building_construction(sid))
		if(p.get_is_upgrade() == false)
			++num_factories;

	// For new factories: no more than defines:FACTORIES_PER_STATE existing + under construction new factories must be
	assert(num_factories <= int32_t(state.defines.factories_per_state));
	return num_factories;
}

float unit_construction_progress(sys::state& state, dcon::province_land_construction_id c) {

	float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_land_construction_get_nation(c));
	float admin_cost_factor = 2.0f - admin_eff;

	auto& goods = state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(c)].build_cost;
	auto& cgoods = state.world.province_land_construction_get_purchased_goods(c);

	float total = 0.0f;
	float purchased = 0.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		total += goods.commodity_amounts[i] * admin_cost_factor;
		purchased += cgoods.commodity_amounts[i];
	}

	return total > 0.0f ? purchased / total : 0.0f;
}

float unit_construction_progress(sys::state& state, dcon::province_naval_construction_id c) {
	float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_naval_construction_get_nation(c));
	float admin_cost_factor = 2.0f - admin_eff;

	auto& goods = state.military_definitions.unit_base_definitions[state.world.province_naval_construction_get_type(c)].build_cost;
	auto& cgoods = state.world.province_naval_construction_get_purchased_goods(c);

	float total = 0.0f;
	float purchased = 0.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		total += goods.commodity_amounts[i] * admin_cost_factor;
		purchased += cgoods.commodity_amounts[i];
	}

	return total > 0.0f ? purchased / total : 0.0f;
}

void add_factory_level_to_state(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id t, bool is_upgrade) {

	if(is_upgrade) {
		auto d = state.world.state_instance_get_definition(s);
		auto o = state.world.state_instance_get_nation_from_state_ownership(s);
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto f : p.get_province().get_factory_location()) {
					if(f.get_factory().get_building_type() == t) {
						auto factory_level = f.get_factory().get_level();
						auto new_factory_level = std::min(float(std::numeric_limits<uint8_t>::max()), float(factory_level) + 1.f + math::sqrt(f.get_factory().get_level()) / 2.f);
						f.get_factory().get_level() = uint8_t(new_factory_level);
						return;
					}
				}
			}
		}
	}
	auto state_cap = state.world.state_instance_get_capital(s);
	auto new_fac = fatten(state.world, state.world.create_factory());
	new_fac.set_building_type(t);
	new_fac.set_level(uint8_t(1));
	new_fac.set_production_scale(1.0f);

	state.world.try_create_factory_location(new_fac, state_cap);
}

void resolve_constructions(sys::state& state) {

	for(uint32_t i = state.world.province_land_construction_size(); i-- > 0;) {
		auto c = fatten(state.world, dcon::province_land_construction_id{dcon::province_land_construction_id::value_base_t(i)});

		float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_land_construction_get_nation(c));
		float admin_cost_factor = 2.0f - admin_eff;

		auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
		auto& current_purchased = c.get_purchased_goods();
		float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

		bool all_finished = true;
		if(!(c.get_nation().get_is_player_controlled() && state.cheat_data.instant_army)) {
			for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
				if(base_cost.commodity_type[j]) {
					if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * admin_cost_factor) {
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
		{
			auto rng = state.world.province_get_province_naval_construction(p);
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());

				float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_naval_construction_get_nation(c));
				float admin_cost_factor = 2.0f - admin_eff;

				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				bool all_finished = true;
				for(uint32_t i = 0; i < commodity_set::set_size && all_finished; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * admin_cost_factor) {
							all_finished = false;
						}
					} else {
						break;
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
		}
	});

	for(uint32_t i = state.world.province_building_construction_size(); i-- > 0;) {
		dcon::province_building_construction_id c{dcon::province_building_construction_id::value_base_t(i)};
		auto for_province = state.world.province_building_construction_get_province(c);

		float admin_eff = state.world.nation_get_administrative_efficiency(state.world.province_building_construction_get_nation(c));
		float admin_cost_factor = state.world.province_building_construction_get_is_pop_project(c) ? 1.0f : 2.0f - admin_eff;

		auto t = province_building_type(state.world.province_building_construction_get_type(c));
		auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
		auto& current_purchased = state.world.province_building_construction_get_purchased_goods(c);
		bool all_finished = true;
		
		for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
			if(base_cost.commodity_type[j]) {
				if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * admin_cost_factor) {
					all_finished = false;
				}
			} else {
				break;
			}
		}
		
		if(all_finished) {
			if(state.world.province_get_building_level(for_province, t) < state.world.nation_get_max_building_level(state.world.province_get_nation_from_province_ownership(for_province), t)) {
				state.world.province_get_building_level(for_province, t) += 1;

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

	for(uint32_t i = state.world.state_building_construction_size(); i-- > 0;) {
		dcon::state_building_construction_id c{dcon::state_building_construction_id::value_base_t(i)};
		auto n = state.world.state_building_construction_get_nation(c);
		auto type = state.world.state_building_construction_get_type(c);
		auto& base_cost = state.world.factory_type_get_construction_costs(type);
		auto& current_purchased = state.world.state_building_construction_get_purchased_goods(c);

		if(!state.world.state_building_construction_get_is_pop_project(c)) {
			float admin_eff = state.world.nation_get_administrative_efficiency(n);
			float admin_cost_factor = 2.0f - admin_eff;

			float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

			bool all_finished = true;
			if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
				for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
					if(base_cost.commodity_type[j]) {
						if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * factory_mod * admin_cost_factor) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
			}
			if(all_finished) {
				add_factory_level_to_state(state, state.world.state_building_construction_get_state(c), type,
						state.world.state_building_construction_get_is_upgrade(c));
				state.world.delete_state_building_construction(c);
			}
		} else {
			float factory_mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f) *
				std::max(0.1f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_owner_cost));

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
			if(all_finished) {
				add_factory_level_to_state(state, state.world.state_building_construction_get_state(c), type,
						state.world.state_building_construction_get_is_upgrade(c));

				if(state.world.state_building_construction_get_nation(c) == state.local_player_nation) {
					notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "amsg_factory_complete");
						},
						"amsg_factory_complete",
						state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::factory_complete
					});
				}

				state.world.delete_state_building_construction(c);
			}
		}
	}
}

/* TODO -
 * This should return what we think the income will be next day, and as a result wont account for any unprecedented actions
 * return value is passed directly into text::fp_currency{} without adulteration.
 */
float estimate_daily_income(sys::state& state, dcon::nation_id n) {
	auto const tax_eff = std::clamp(
		state.defines.base_country_tax_efficiency
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tax_efficiency), 0.1f, 1.0f
	);

	return (
		state.world.nation_get_total_poor_income(n) * state.world.nation_get_poor_tax(n) / 100.f
		+ state.world.nation_get_total_middle_income(n) * state.world.nation_get_middle_tax(n) / 100.f
		+ state.world.nation_get_total_rich_income(n) * state.world.nation_get_rich_tax(n) / 100.f
	) * tax_eff;
}

void try_add_factory_to_state(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id t) {
	auto n = state.world.state_instance_get_nation_from_state_ownership(s);

	if(state.world.factory_type_get_is_coastal(t)) {
		if(!province::state_is_coastal(state, s))
			return; // requires coast to build coastal factory
	}

	auto existing_constructions = state.world.state_instance_get_state_building_construction(s);
	int32_t num_factories = 0;
	for(auto prj : existing_constructions) {
		if(!prj.get_is_upgrade())
			++num_factories;
		if(prj.get_type() == t)
			return; // can't duplicate type
	}

	// is there an upgrade target ?
	auto d = state.world.state_instance_get_definition(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == n) {
			for(auto f : p.get_province().get_factory_location()) {
				++num_factories;
				if(f.get_factory().get_building_type() == t)
					return; // can't build another of this type
			}
		}
	}

	if(num_factories < int32_t(state.defines.factories_per_state)) {
		add_factory_level_to_state(state, s, t, false);
	}
}

void bound_budget_settings(sys::state& state, dcon::nation_id n) {
	{
		auto min_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_tariff));
		auto max_tariff = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_tariff));
		max_tariff = std::max(min_tariff, max_tariff);

		auto& tariff = state.world.nation_get_tariffs(n);
		tariff = int8_t(std::clamp(std::clamp(int32_t(tariff), min_tariff, max_tariff), -100, 100));
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
	for(auto si : state.world.in_state_instance) {
		auto owner = si.get_nation_from_state_ownership();
		auto rules = owner.get_combined_issue_rules();

		if(owner.get_is_player_controlled() && (rules & issue_rule::destroy_factory) != 0) // not for players who can manually destroy
			continue;

		dcon::factory_id deletion_choice;
		int32_t factory_count = 0;

		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			for(auto f : state.world.province_get_factory_location(p)) {
				++factory_count;
				auto scale = f.get_factory().get_production_scale();
				float ten_workers = 10.f / factory_max_employment(state, f.get_factory());
				bool unprofitable = f.get_factory().get_unprofitable();
				if(((scale < ten_workers) && unprofitable) && (!deletion_choice || state.world.factory_get_level(deletion_choice) > f.get_factory().get_level())) {
					deletion_choice = f.get_factory();
				}
			}
		});

		// aggressive pruning
		// to help building more healthy economy instead of 1 profitable giant factory with 6 small 0 scale factories
		if(deletion_choice && (4 + factory_count) >= int32_t(state.defines.factories_per_state)) {
			auto production_type = state.world.factory_get_building_type(deletion_choice);
			state.world.delete_factory(deletion_choice);

			for(auto proj : si.get_state_building_construction()) {
				if(proj.get_type() == production_type) {
					state.world.delete_state_building_construction(proj);
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
	auto& debt = state.world.nation_get_stockpiles(n, economy::money);

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

commodity_production_type get_commodity_production_type(sys::state& state, dcon::commodity_id c) {
	auto commodity = dcon::fatten(state.world, c);
	if(commodity.get_rgo_amount() > 0 && (commodity.get_artisan_output_amount() > 0 || commodity.get_key_factory()))
		return commodity_production_type::both;
	else if(commodity.get_key_factory())
		return commodity_production_type::derivative;
	else
		return commodity_production_type::primary;


}

} // namespace economy
