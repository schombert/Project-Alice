#include "economy.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"

namespace economy {

bool has_factory(sys::state const& state, dcon::state_instance_id si) {
	auto sdef = state.world.state_instance_get_definition(si);
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			auto rng = p.get_province().get_factory_location();
			if(rng.begin() != rng.end())
				return true;
		}
	}
	return false;
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
		state.world.unilateral_relationship_get_owns_debt_of(state.world.get_unilateral_relationship_by_unilateral_pair(debtor, debt_holder)) > 0.1f;
}

bool nation_is_constructing_factories(sys::state& state, dcon::nation_id n) {
	// TODO
	return false;
}
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n) {
	// TODO
	return false;
}

void initialize(sys::state& state) {
	state.world.nation_resize_domestic_market_pool(state.world.commodity_size());
	state.world.nation_resize_real_demand(state.world.commodity_size());

	// employment

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto fc = fatten(state.world, c);
		fc.set_current_price(fc.get_cost());
		fc.set_total_consumption(1.0f);
		fc.set_total_production(1.0f);
		fc.set_total_real_demand(1.0f);

		// fc.set_global_market_pool();
	});

	auto savings_buffer = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		auto ft = fatten(state.world, t);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			savings_buffer.get(t) += state.world.commodity_get_is_available_from_start(c)
				? state.world.commodity_get_cost(c) * ft.get_life_needs(c)
				: 0.0f;
		});
		auto strata = (ft.get_strata() * 2) + 1;
		savings_buffer.get(t) *= strata;
	});

	state.world.for_each_pop([&](dcon::pop_id p) {
		auto fp = fatten(state.world, p);
		fp.set_life_needs_satisfaction(1.0f);
		fp.set_everyday_needs_satisfaction(0.5f);
		fp.set_savings(savings_buffer.get(fp.get_poptype()));
	});

	state.world.for_each_factory([&](dcon::factory_id f) {
		auto ff = fatten(state.world, f);
		ff.set_production_scale(1.0f);
		//ff.set_employment();
	});

	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto fp = fatten(state.world, p);
		//fp.set_rgo_employment();
		//fp.set_rgo_size();
		fp.set_rgo_production_scale(1.0f);
		fp.set_artisan_production_scale(1.0f);
		//fp.set_artisan_production();
	});

	state.world.for_each_nation([&](dcon::nation_id n) {
		auto fn = fatten(state.world, n);
		fn.set_administrative_spending(int8_t(100));
		fn.set_military_spending(int8_t(100));
		fn.set_education_spending(int8_t(100));
		fn.set_social_spending(int8_t(100));
		fn.set_land_spending(int8_t(30));
		fn.set_naval_spending(int8_t(30));
		fn.set_construction_spending(int8_t(100));

		fn.set_poor_tax(int8_t(30));
		fn.set_middle_tax(int8_t(40));
		fn.set_rich_tax(int8_t(50));

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			// set domestic market pool
		});
	});
}

float sphere_leader_share_factor(sys::state const& state, dcon::nation_id sphere_leader, dcon::nation_id sphere_member) {
	/*
	Share factor : If the nation is a civ and is a secondary power start with define : SECOND_RANK_BASE_SHARE_FACTOR, and otherwise start with define : CIV_BASE_SHARE_FACTOR.Also calculate the sphere owner's foreign investment in the nation as a fraction of the total foreign investment in the nation (I believe that this is treated as zero if there is no foreign investment at all). The share factor is (1 - base share factor) x sphere owner investment fraction + base share factor. For uncivs, the share factor is simply equal to define:UNCIV_BASE_SHARE_FACTOR (so 1, by default). If a nation isn't in a sphere, we let the share factor be 0 if it needs to be used in any other calculation.
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
			// - Each sphere member has its domestic x its-share-factor (see above) of its base supply and demand added to its sphere leader's domestic supply and demand (this does not affect global supply and demand)

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
		//- Every nation in a sphere (after the above has been calculated for the entire sphere) has their effective domestic supply set to (1 - its-share-factor) x original-domestic-supply + sphere-leader's-domestic supply
		
		float share = sphere_leader_share_factor(state, sl, n);
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			state.world.nation_get_domestic_market_pool(n, c) *= (1.0f - share);
		});
	}
}

float effective_tariff_rate(sys::state const& state, dcon::nation_id n) {
	//- tariff efficiency: define:BASE_TARIFF_EFFICIENCY + national-modifier-to-tariff-efficiency + administrative-efficiency, limited to at most 1.0
	auto tariff_efficiency = std::clamp(state.defines.base_tariff_efficiency + state.world.nation_get_administrative_efficiency(n) + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::tariff_efficiency_modifier), 0.001f, 1.0f);
	return tariff_efficiency * float(state.world.nation_get_tariffs(n)) / 100.0f;
}

float global_market_price_multiplier(sys::state const& state, dcon::nation_id n) {
	auto central_ports = state.world.nation_get_central_ports(n);
	if(central_ports > 0) {
		return effective_tariff_rate(state, n) + float(state.world.nation_get_central_blockaded(n)) / float(central_ports) + 1.0f;
	} else {
		return effective_tariff_rate(state, n) + 1.0f;
	}
}

/*
* 
- Each factory has an input, output, and throughput multipliers.
- These are computed from the employees present. Input and output are 1 + employee effects, throughput starts at 0
- The input multiplier is also multiplied by (1 + sum-of-any-triggered-modifiers-for-the-factory) x 0v(national-mobilization-impact - (overseas-penalty if overseas, or 0 otherwise))
- Owner fraction is calculated from the fraction of owners in the state to total state population in the state (with some cap -- 5%?)
- For each pop type employed, we calculate the ratio of number-of-pop-employed-of-a-type / (base-workforce x level) to the optimal fraction defined for the production type (capping it at 1). That ratio x the-employee-effect-amount is then added into the input/output/throughput modifier for the factory.
- Then, for input/output/throughput we sum up national and provincial modifiers to general factory input/output/throughput are added, plus technology modifiers to its specific output commodity, add one to the sum, and then multiply the input/output/throughput modifier from the workforce by it.

- The target input consumption scale is: input-multiplier x throughput-multiplier x factory level
- The actual consumption scale is limited by the input commodities sitting in the stockpile (i.e. input-consumption-scale x input-quantity must be less than the amount in the stockpile)
- A similar process is done for efficiency inputs, except the consumption of efficiency inputs is (national-factory-maintenance-modifier + 1) x input-multiplier x throughput-multiplier x factory level
- Finally, we get the efficiency-adjusted consumption scale by multiplying the base consumption scale by (0.75 + 0.25 x the efficiency consumption scale)

*/

float factory_full_production_quantity(sys::state const& state, dcon::factory_id f, dcon::nation_id n, float mobilization_impact) {
	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();

	/*
	float total_state_pop = std::max(0.01f, state.world.state_instance_get_demographics(s, demographics::total));
	float owner_fraction = std::min(0.05f, state.world.state_instance_get_demographics(s, demographics::to_key(state, state.culture_definitions.capitalists)) / total_state_pop);
	float input_multiplier = fac.get_triggered_modifiers() * (1.0f + owner_fraction * -2.5f);
	*/

	float throughput_multiplier = (state.world.nation_get_factory_goods_throughput(n, fac_type.get_output()) + 1.0f);
	float output_multiplier = state.world.nation_get_factory_goods_output(n, fac_type.get_output()) + 1.0f + fac.get_secondary_employment() * (1.0f - state.economy_definitions.craftsmen_fraction) * fac.get_secondary_employment() * 1.5f;
	float max_production_scale = fac.get_primary_employment() * fac.get_level() * std::max(0.0f, (mobilization_impact - state.world.nation_get_overseas_penalty(n)));

	return throughput_multiplier * output_multiplier * max_production_scale;
}

float rgo_full_production_quantity(sys::state const& state, dcon::province_id p) {
	/*
	- We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size + technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
	- We add its production to domestic supply, calculating that amount basically in the same way we do for factories, by computing RGO-throughput x RGO-output x RGO-size x base-commodity-production-quantity, except that it is affected by different modifiers.
	*/
	return 0.0f;
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
				mod_a && trigger::evaluate_trigger(state, mod_a, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum += fac_type.get_bonus_1_amount();
			}
			if(auto mod_b = fac_type.get_bonus_2_trigger();
				mod_b && trigger::evaluate_trigger(state, mod_b, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum += fac_type.get_bonus_2_amount();
			}
			if(auto mod_c = fac_type.get_bonus_3_trigger();
				mod_c && trigger::evaluate_trigger(state, mod_c, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum += fac_type.get_bonus_3_amount();
			}
		}

		state.world.factory_set_triggered_modifiers(f, sum);
	});
}

float rgo_effective_size(sys::state const& state, dcon::nation_id n, dcon::province_id p) {
	bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));

	// - We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size + technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)

	auto sz = state.world.province_get_rgo_size(p);
	auto bonus = state.world.province_get_modifier_values(p, is_mine ? sys::provincial_mod_offsets::mine_rgo_size : sys::provincial_mod_offsets::farm_rgo_size)
		+ state.world.nation_get_modifier_values(n, is_mine ? sys::national_mod_offsets::mine_rgo_size : sys::national_mod_offsets::farm_rgo_size)
		+ state.world.nation_get_rgo_size(n, state.world.province_get_rgo(p))
		+ 1.0f;

	return sz * bonus;
}

float rgo_max_employment(sys::state const& state, dcon::nation_id n, dcon::province_id p) {
	return 40000.0f * rgo_effective_size(state, n, p);
}

int32_t factory_priority(sys::state const& state, dcon::factory_id f) {
	return (state.world.factory_get_priority_low(f) ? 1 : 0) + (state.world.factory_get_priority_high(f) ? 2 : 0);
}
void set_factory_priority(sys::state& state, dcon::factory_id f, int32_t priority) {
	state.world.factory_set_priority_high(f, priority >= 2);
	state.world.factory_set_priority_low(f, (priority & 1) != 0);
}

void update_employement(sys::state& state, dcon::nation_id n, float mobilization_impact) {

	static auto production_consumption_ratio = state.world.commodity_make_vectorizable_float_buffer();
	state.world.execute_serial_over_commodity([&](auto ids) {
		auto consumption = state.world.commodity_get_total_consumption(ids);
		auto production = state.world.commodity_get_total_production(ids);
		production_consumption_ratio.set(ids, ve::select(consumption > 0.0f, production / consumption, 1.0f));
	});

	for(auto p : state.world.nation_get_province_ownership(n)) {
		/*
		* RGO employment
		*/
		{
			auto rgo_makes = p.get_province().get_rgo();
			if(rgo_makes.get_is_mine()) {

			}
			if(rgo_makes == economy::money) {

			} else {
				
			}
		}
		/*
		* Factory employment
		*/

		for(auto f : p.get_province().get_factory_location()) {

		}
	}
	for(auto si : state.world.nation_get_state_ownership(n)) {

	}

}

void daily_update(sys::state& state) {

	update_factory_triggered_modifiers(state);

	auto num_nation = state.world.nation_size();
	for(auto n : state.nations_by_rank) {
		if(!n) // test for running out of sorted nations
			break;

		/*
		As the day starts, we move production, fractionally, into the sphere leaders domestic production pool, following the same logic as Victoria 2
		*/

		absorb_sphere_member_production(state, n); // no need for redundant checks here
		give_sphere_leader_production(state, n);


		/*
		### Calculate effective prices
		We will use the real demand from the *previous* day to determine how much of the purchasing will be done from the domestic and global pools (i.e. what percentage was able to be done from the cheaper pool). We will use that to calculate an effective price. And then, at the end of the current day, we will see how much of that purchasing actually came from each pool, etc. Depending on the stability of the simulation, we may, instead of taking the previous day, instead build this value iteratively as a linear combination of the new day and the previous day.

		when purchasing from global supply, prices are multiplied by (the nation's current effective tariff rate + its blockaded fraction + 1)
		*/

		static auto effective_prices = state.world.commodity_make_vectorizable_float_buffer();

		auto global_price_multiplier = global_market_price_multiplier(state, n);

		if(global_price_multiplier >= 1.0f) { // prefer domestic
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				auto domestic_supply = state.world.nation_get_domestic_market_pool(n, c);
				auto global_supply = state.world.commodity_get_global_market_pool(c);
				auto last_demand = state.world.nation_get_real_demand(n, c);
				auto base_price = state.world.commodity_get_current_price(c);
				if(last_demand <= domestic_supply) {
					effective_prices.set(c, base_price);
				} else if(last_demand <= domestic_supply + global_supply) {
					auto domestic_satisfiable_fraction = domestic_supply / last_demand;
					effective_prices.set(c,
						base_price * domestic_satisfiable_fraction
						+ base_price * (1.0f - domestic_satisfiable_fraction) * global_price_multiplier);
				} else {
					auto domestic_satisfiable_fraction = domestic_supply / (domestic_supply + global_supply);
					effective_prices.set(c,
						base_price * domestic_satisfiable_fraction
						+ base_price * (1.0f - domestic_satisfiable_fraction) * global_price_multiplier);
				}
			});
		} else { // prefer global
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				auto domestic_supply = state.world.nation_get_domestic_market_pool(n, c);
				auto global_supply = state.world.commodity_get_global_market_pool(c);
				auto last_demand = state.world.nation_get_real_demand(n, c);
				auto base_price = state.world.commodity_get_current_price(c);
				if(last_demand <= global_supply) {
					effective_prices.set(c, base_price);
				} else if(last_demand <= domestic_supply + global_supply) {
					auto global_satisfiable_fraction = global_supply / last_demand;
					effective_prices.set(c,
						base_price * global_satisfiable_fraction * global_price_multiplier
						+ base_price * (1.0f - global_satisfiable_fraction));
				} else {
					auto global_satisfiable_fraction = global_supply / (domestic_supply + global_supply);
					effective_prices.set(c,
						base_price * global_satisfiable_fraction * global_price_multiplier
						+ base_price * (1.0f - global_satisfiable_fraction));
				}
			});
		}

		float mobilization_impact = state.world.nation_get_is_mobilized(n) ? military::mobilization_impact(state, n) : 1.0f;

		update_employement(state, n, mobilization_impact);
	}
}

void regenerate_unsaved_values(sys::state& state) {
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto fc = fatten(state.world, c);
		if(fc.get_total_production() > 0.0001f) {
			fc.set_producer_payout_fraction(std::min(fc.get_total_consumption() / fc.get_total_production(), 1.0f));
		} else {
			fc.set_producer_payout_fraction(1.0f);
		}
	});
}

}

