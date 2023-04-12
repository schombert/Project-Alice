#include "economy.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "prng.hpp"

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

void randomly_assign_artisan_production(sys::state& state, dcon::province_id p) {
	int32_t num_active_productions = 0;
	auto owner = state.world.province_get_nation_from_province_ownership(p);
	auto const csize = state.world.commodity_size();

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto kf = state.world.commodity_get_key_factory(cid);
		if(state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(owner, kf))) {
			++num_active_productions;
		}
	}

	uint32_t rval = uint32_t(rng::get_random(state, uint32_t(p.value) | uint32_t(owner.value << 16)));
	auto selected = rval % num_active_productions; // bleh

	for(uint32_t i = 1; i < csize; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto kf = state.world.commodity_get_key_factory(cid);
		if(state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(owner, kf))) {
			if(selected == 0) {
				state.world.province_set_artisan_production(p, cid);
				break;
			} else {
				--selected;
			}
		}
	}
}

void initialize(sys::state& state) {
	state.world.nation_resize_domestic_market_pool(state.world.commodity_size());
	state.world.nation_resize_real_demand(state.world.commodity_size());
	state.world.nation_resize_stockpile_targets(state.world.commodity_size());
	state.world.nation_resize_drawing_on_stockpiles(state.world.commodity_size());

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
	});

	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto fp = fatten(state.world, p);
		
		bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));

		auto amount = std::ceil(1.1f * (state.world.province_get_demographics(p, demographics::to_key(state, is_mine ? state.culture_definitions.laborers : state.culture_definitions.farmers)) +
			state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves))) / 40000.0f);

		fp.set_rgo_size(std::max(1.0f, amount));

		fp.set_rgo_production_scale(1.0f);
		fp.set_artisan_production_scale(1.0f);

		randomly_assign_artisan_production(state, p);
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

	update_rgo_employment(state);
	update_factory_employment(state);
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
				sum -= fac_type.get_bonus_1_amount();
			}
			if(auto mod_b = fac_type.get_bonus_2_trigger();
				mod_b && trigger::evaluate_trigger(state, mod_b, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum -= fac_type.get_bonus_2_amount();
			}
			if(auto mod_c = fac_type.get_bonus_3_trigger();
				mod_c && trigger::evaluate_trigger(state, mod_c, trigger::to_generic(pstate), trigger::to_generic(powner), 0)) {
				sum -= fac_type.get_bonus_3_amount();
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

inline constexpr float rgo_per_size_employment = 40'000.0f;

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
		auto rgo_max = rgo_max_employment(state, owner, p) * state.world.province_get_rgo_production_scale(p);

		bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));
		float worker_pool = state.world.province_get_demographics(p, demographics::to_key(state, is_mine ? state.culture_definitions.laborers : state.culture_definitions.farmers));
		float slave_pool = state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves));
		float labor_pool = worker_pool + slave_pool;

		state.world.province_set_rgo_employment(p, labor_pool >= rgo_max ? 1.0f : labor_pool / rgo_max);

		auto slave_fraction = (slave_pool > rgo_max) ? rgo_max / slave_pool : 1.0f;
		auto free_fraction = std::max(0.0f, (worker_pool > rgo_max - slave_pool) ? (rgo_max - slave_pool) / std::max(worker_pool, 0.01f) : 1.0f);

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.slaves) {
				pop.get_pop().set_employment(pop.get_pop().get_size() * slave_fraction);
			} else if(pop.get_pop().get_poptype() == (is_mine ? state.culture_definitions.laborers : state.culture_definitions.farmers)) {
				pop.get_pop().set_employment(pop.get_pop().get_size() * free_fraction);
			}
		}
	});
}

inline constexpr float factory_per_level_employment = 10'000.0f;

float factory_max_employment(sys::state const& state, dcon::factory_id f) {
	return factory_per_level_employment * state.world.factory_get_level(f);
}

void update_factory_employment(sys::state& state) {
	state.world.for_each_state_instance([&](dcon::state_instance_id si) {
		float primary_pool = state.world.state_instance_get_demographics(si, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		float secondary_pool = state.world.state_instance_get_demographics(si, demographics::to_key(state, state.culture_definitions.primary_factory_worker));

		static std::vector<dcon::factory_id> ordered_factories;
		ordered_factories.clear();

		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			for(auto fac : state.world.province_get_factory_location(p)) {
				ordered_factories.push_back(fac.get_factory());
			}
		});

		std::sort(ordered_factories.begin(), ordered_factories.end(), [&](dcon::factory_id a, dcon::factory_id b) {
			if(factory_is_profitable(state, a) && !factory_is_profitable(state, b)) {
				return true;
			}
			if(factory_priority(state, a) > factory_priority(state, b)) {
				return true;
			}
			return a.index() < b.index();
		});

		float primary_pool_copy = primary_pool;
		float secondary_pool_copy = secondary_pool;
		for(uint32_t index = 0; index < ordered_factories.size(); ) {
			uint32_t next_index = index;

			float total_workforce = 0.0f;
			for(; next_index < ordered_factories.size(); ++next_index) {
				if(factory_is_profitable(state, ordered_factories[index]) != factory_is_profitable(state, ordered_factories[next_index])
					|| factory_priority(state, ordered_factories[index]) != factory_priority(state, ordered_factories[next_index])) {
					break;
				}
				total_workforce += factory_max_employment(state, ordered_factories[next_index]) * state.world.factory_get_production_scale(ordered_factories[next_index]);
			}

			{
				float type_share = state.economy_definitions.craftsmen_fraction * total_workforce;
				float scale = primary_pool_copy >= type_share ? 1.0f : primary_pool_copy / type_share;
				primary_pool_copy = std::max(0.0f, primary_pool_copy - type_share);
				for(uint32_t i = index; i < next_index; ++i) {
					state.world.factory_set_primary_employment(ordered_factories[i], scale * state.world.factory_get_production_scale(ordered_factories[i]));
				}
			}
			{
				float type_share = (1.0f - state.economy_definitions.craftsmen_fraction) * total_workforce;
				float scale = secondary_pool_copy >= type_share ? 1.0f : secondary_pool_copy / type_share;
				secondary_pool_copy = std::max(0.0f, secondary_pool_copy - type_share);
				for(uint32_t i = index; i < next_index; ++i) {
					state.world.factory_set_primary_employment(ordered_factories[i], scale * state.world.factory_get_production_scale(ordered_factories[i]));
				}
			}

			index = next_index;
		}

		float prim_employment = 1.0f - primary_pool_copy / primary_pool;
		float sec_employment = 1.0f - secondary_pool_copy / secondary_pool;

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

inline constexpr float production_scale_delta = 0.05f;

void update_single_factory_consumption(sys::state& state, dcon::factory_id f, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices, float mobilization_impact, float expected_min_wage, bool occupied, bool overseas) {

	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();

	float input_total = 0.0f;
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(fac_type.get_inputs().commodity_type[i]) {
			input_total += fac_type.get_inputs().commodity_amounts[i] * effective_prices.get(fac_type.get_inputs().commodity_type[i]);
		} else {
			break;
		}
	}
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(fac_type.get_efficiency_inputs().commodity_type[i]) {
			input_total += fac_type.get_efficiency_inputs().commodity_amounts[i] * effective_prices.get(fac_type.get_efficiency_inputs().commodity_type[i]);
		} else {
			break;
		}
	}

	float output_total = fac_type.get_output_amount() * state.world.commodity_get_current_price(fac_type.get_output());

	
	float total_state_pop = std::max(0.01f, state.world.state_instance_get_demographics(s, demographics::total));
	float owner_fraction = std::min(0.05f, state.world.state_instance_get_demographics(s, demographics::to_key(state, state.culture_definitions.capitalists)) / total_state_pop);

	float input_multiplier = fac.get_triggered_modifiers() * (
		1.0f
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_input)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_input)
		+ owner_fraction * -2.5f);
	
	float throughput_multiplier = 
		state.world.nation_get_factory_goods_throughput(n, fac_type.get_output())
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_throughput)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_throughput)
		+ 1.0f;

	float output_multiplier =
		state.world.nation_get_factory_goods_output(n, fac_type.get_output())
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_factory_output)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_output)
		+ fac.get_secondary_employment() * (1.0f - state.economy_definitions.craftsmen_fraction) * 1.5f
		+ 1.0f;
	

	float max_production_scale = fac.get_primary_employment() * fac.get_level() * (occupied ? 0.0f : 1.0f) * std::max(0.0f, (mobilization_impact - (overseas ? state.world.nation_get_overseas_penalty(n) : 0)));

	bool profitable = (output_total * throughput_multiplier * output_multiplier - expected_min_wage * (factory_per_level_employment / needs_scaling_factor) - input_multiplier * throughput_multiplier * input_total) >= 0.0f;
	state.world.factory_set_unprofitable(f, !profitable);
	
	if(profitable || state.world.factory_get_subsidized(f)) {
		// increase production scale ?
		auto new_production_scale = std::min(1.0f, state.world.factory_get_production_scale(f) + production_scale_delta);
		state.world.factory_set_production_scale(f, new_production_scale);
		auto effective_production_scale = std::min(new_production_scale * fac.get_level(), max_production_scale);

		// register real demand : input_multiplier * throughput_multiplier * level * primary_employment
		float input_scale = input_multiplier * throughput_multiplier * effective_production_scale;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(fac_type.get_inputs().commodity_type[i]) {
				state.world.nation_get_real_demand(n, fac_type.get_inputs().commodity_type[i]) += input_scale * fac_type.get_inputs().commodity_amounts[i];
			} else {
				break;
			}
		}

		// and for efficiency inputs
		//  the consumption of efficiency inputs is (national-factory-maintenance-modifier + 1) x input-multiplier x throughput-multiplier x factory level
		auto const mfactor = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(fac_type.get_efficiency_inputs().commodity_type[i]) {
				state.world.nation_get_real_demand(n, fac_type.get_efficiency_inputs().commodity_type[i]) += mfactor * input_scale * fac_type.get_efficiency_inputs().commodity_amounts[i];
			} else {
				break;
			}
		}

		state.world.factory_set_actual_production(f, fac_type.get_output_amount() * throughput_multiplier * output_multiplier * effective_production_scale);
		state.world.factory_set_full_profit(f, (output_total * output_multiplier - input_multiplier * input_total) * throughput_multiplier * effective_production_scale);
	} else {
		// reduce production scale
		state.world.factory_set_production_scale(f, std::max(0.0f, state.world.factory_get_production_scale(f) - production_scale_delta));
		state.world.factory_set_actual_production(f, 0.0f);
		state.world.factory_set_full_profit(f, 0.0f);
	}
}

void update_single_factory_production(sys::state& state, dcon::factory_id f, dcon::nation_id n) {

	auto production = state.world.factory_get_actual_production(f);
	if(production > 0) {
		auto fac = fatten(state.world, f);
		auto fac_type = fac.get_building_type();

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
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(fac_type.get_efficiency_inputs().commodity_type[i]) {
				min_efficiency_input = std::min(min_efficiency_input, state.world.nation_get_demand_satisfaction(n, fac_type.get_efficiency_inputs().commodity_type[i]));
			} else {
				break;
			}
		}

		auto amount = (0.75f + 0.25f * min_efficiency_input) * min_input * production;
		state.world.factory_set_production_scale(f, (min_input * 0.75f + 0.25f) * state.world.factory_get_production_scale(f));
		state.world.factory_set_actual_production(f, amount);
		state.world.nation_get_domestic_market_pool(n, fac_type.get_output()) += amount;

		auto money_made = (0.75f + 0.25f * min_efficiency_input) * min_input * state.world.factory_get_full_profit(f);
		state.world.factory_set_full_profit(f, money_made);

		// pay wages ?
	} else {

	}
}

// TODO: special logic for money rgo

void update_province_rgo_consumption(sys::state& state, dcon::province_id p, dcon::nation_id n, float mobilization_impact, float expected_min_wage, bool occupied) {

	auto max_production = rgo_full_production_quantity(state, p);
	auto pops_max = rgo_max_employment(state, n, p);
	auto pop_wages = pops_max * expected_min_wage / needs_scaling_factor;
	auto max_profit = max_production * state.world.commodity_get_current_price(state.world.province_get_rgo(p));

	if(max_profit >= pop_wages) {
		auto new_production_scale = std::min(1.0f, state.world.province_get_rgo_production_scale(p) + production_scale_delta);
		state.world.province_set_rgo_production_scale(p, new_production_scale);
	} else {
		auto new_production_scale = std::max(0.0f, state.world.province_get_rgo_production_scale(p) - production_scale_delta);
		state.world.province_set_rgo_production_scale(p, new_production_scale);
	}
	// rgos produce all the way down
	state.world.province_set_rgo_actual_production(p, max_production * state.world.province_get_rgo_employment(p));
}

void update_province_rgo_production(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	auto amount = state.world.province_get_rgo_actual_production(p);
	auto c = state.world.province_get_rgo(p);
	state.world.nation_get_domestic_market_pool(n, c) += amount;
	state.world.province_set_rgo_full_profit(p, amount * state.world.commodity_get_current_price(c));
}

void update_province_artisan_consumption(sys::state& state, dcon::province_id p, dcon::nation_id n, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices, float mobilization_impact, float expected_min_wage, bool occupied) {

	if((state.current_date.to_raw_value() & 31) == (p.value & 31)) {
		randomly_assign_artisan_production(state, p); // randomly switch once a month 
	}

	float input_total = 0.0f;
	auto artisan_prod_type = state.world.province_get_artisan_production(p);
	auto& inputs = state.world.commodity_get_artisan_inputs(artisan_prod_type);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total += inputs.commodity_amounts[i] * effective_prices.get(inputs.commodity_type[i]);
		} else {
			break;
		}
	}

	float output_total = state.world.commodity_get_artisan_output_amount(artisan_prod_type) * state.world.commodity_get_current_price(artisan_prod_type);

	float artisan_pop = std::max(0.01f, state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.artisans)));

	float input_multiplier =
		1.0f
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_artisan_input)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::artisan_input)
		;
	float throughput_multiplier =
		1.0f
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_artisan_throughput)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::artisan_throughput);
	float output_multiplier =
		1.0f
		+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_artisan_output)
		+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::artisan_output)
		;

	float max_production_scale = artisan_pop / 10'000.0f * (occupied ? 0.0f : 1.0f) * std::max(0.0f, mobilization_impact);

	bool profitable = (output_total * output_multiplier - expected_min_wage * (10'000.0f / needs_scaling_factor) - input_multiplier * input_total) >= 0.0f;

	if(profitable) {
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(inputs.commodity_type[i]) {
				state.world.nation_get_real_demand(n, inputs.commodity_type[i]) += input_multiplier * throughput_multiplier * max_production_scale * inputs.commodity_amounts[i];
			} else {
				break;
			}
		}

		state.world.province_set_artisan_actual_production(p, state.world.commodity_get_artisan_output_amount(artisan_prod_type) * throughput_multiplier * output_multiplier * max_production_scale);
		state.world.province_set_artisan_full_profit(p, (output_total * output_multiplier - input_multiplier * input_total) * throughput_multiplier);
	} else {
		// switch production type
		randomly_assign_artisan_production(state, p);
		state.world.province_set_artisan_actual_production(p, 0.0f);
		state.world.province_set_artisan_full_profit(p, 0.0f);
	}
}

void update_province_artisan_production(sys::state& state, dcon::province_id p, dcon::nation_id n, dcon::state_instance_id s) {

	auto production = state.world.province_get_artisan_actual_production(p);
	if(production > 0) {

		auto artisan_prod_type = state.world.province_get_artisan_production(p);
		auto& inputs = state.world.commodity_get_artisan_inputs(artisan_prod_type);

		float min_input = 1.0f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(inputs.commodity_type[i]) {
				min_input = std::min(min_input, state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[i]));
			} else {
				break;
			}
		}


		auto amount = min_input * production;
		state.world.province_set_artisan_actual_production(p, amount);
		state.world.nation_get_domestic_market_pool(n, artisan_prod_type) += amount;

		auto money_made = min_input * state.world.province_get_artisan_full_profit(p);
		state.world.province_set_artisan_full_profit(p, money_made);

		// pay wages ?
	} else {
		
	}
}

struct commodity_by_nation_array {
private:
	std::vector<float> internal_array;
	uint32_t const commodity_size;
public:
	commodity_by_nation_array(sys::state const& state) : commodity_size(state.world.commodity_size()) {
	}
	void prepare(sys::state const& state) {
		if(internal_array.size() < state.world.nation_size() * commodity_size) {
			internal_array.resize(state.world.nation_size() * commodity_size);
		}
		std::fill_n(internal_array.data(), internal_array.size(), 0.0f);
	}
	float& get(dcon::nation_id n, dcon::commodity_id c) {
		return internal_array[n.index() * commodity_size + c.index()];
	}
	float get(dcon::nation_id n, dcon::commodity_id c) const {
		return internal_array[n.index() * commodity_size + c.index()];
	}
};

struct pop_type_by_nation_array {
private:
	std::vector<float> internal_array;
	uint32_t const poptype_size;
public:
	pop_type_by_nation_array(sys::state const& state) : poptype_size(state.world.pop_type_size()) { }
	void prepare(sys::state const& state) {
		if(internal_array.size() < state.world.nation_size() * poptype_size) {
			internal_array.resize(state.world.nation_size() * poptype_size);
		}
		std::fill_n(internal_array.data(), internal_array.size(), 0.0f);
	}
	float& get(dcon::nation_id n, dcon::pop_type_id t) {
		return internal_array[n.index() * poptype_size + t.index()];
	}
	float get(dcon::nation_id n, dcon::pop_type_id t) const {
		return internal_array[n.index() * poptype_size + t.index()];
	}
};

void populate_pop_money(sys::state& state, pop_type_by_nation_array& storage) {
	state.world.for_each_pop([&](dcon::pop_id p) {
		auto owner = nations::owner_of_pop(state, p);
		auto pt = state.world.pop_get_poptype(p);
		if(owner && pt)
			storage.get(owner, pt) += state.world.pop_get_savings(p);
	});
}

void populate_army_consumption(sys::state& state, commodity_by_nation_array& storage) {
	state.world.for_each_regiment([&](dcon::regiment_id r) {
		auto reg = fatten(state.world, r);
		auto type = state.world.regiment_get_type(r);
		auto owner = reg.get_army_from_army_membership().get_controller_from_army_control();

		if(owner) {
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					storage.get(owner, supply_cost.commodity_type[i]) += supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(owner, type).supply_consumption * (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption) + 1.0f);
				} else {
					break;
				}
			}
		}
	});
}

void populate_navy_consumption(sys::state& state, commodity_by_nation_array& storage) {
	state.world.for_each_ship([&](dcon::ship_id r) {
		auto shp = fatten(state.world, r);
		auto type = state.world.ship_get_type(r);
		auto owner = shp.get_navy_from_navy_membership().get_controller_from_navy_control();

		if(owner) {
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					storage.get(owner, supply_cost.commodity_type[i]) += supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(owner, type).supply_consumption * (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption) + 1.0f);
				} else {
					break;
				}
			}
		}
	});
}

void populate_constuction_consumption(sys::state& state, commodity_by_nation_array& storage) {
	// TODO
}

float update_national_consumption(sys::state& state, dcon::nation_id n, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices, commodity_by_nation_array const& army_costs, commodity_by_nation_array const& naval_costs, commodity_by_nation_array const& construction_costs) { // returns: national spending level

	float army_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float navy_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float construction_spending = float(state.world.nation_get_construction_spending(n)) / 100.0f;

	// step 1: figure out total
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += army_costs.get(n, cid) * army_spending * effective_prices.get(cid);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += naval_costs.get(n, cid) * navy_spending * effective_prices.get(cid);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += construction_costs.get(n, cid) * construction_spending * effective_prices.get(cid);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			total += difference * effective_prices.get(cid);
		}
	}
	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) -state.world.nation_get_central_province_count(n));

	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				total += overseas_factor * effective_prices.get(cid);
			}
		}
	}

	// step 2: limit to actual budget
	float budget = state.world.nation_get_stockpiles(n, economy::money) / 2.0f; // max half of treasury on spending (TODO: make debt possible)
	float spending_scale = (total < 0.001 || total <= budget) ? 1.0f : budget / total;
	
	// step 3: add to real demand
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.nation_get_real_demand(n, cid) += army_costs.get(n, cid) * army_spending * spending_scale;
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.nation_get_real_demand(n, cid) += naval_costs.get(n, cid) * navy_spending * spending_scale;
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.nation_get_real_demand(n, cid) += construction_costs.get(n, cid) * construction_spending * spending_scale;
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			state.world.nation_get_real_demand(n, cid) += difference * spending_scale;
		}
	}

	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				state.world.nation_get_real_demand(n, cid) += overseas_factor * spending_scale;
			}
		}
	}

	return spending_scale;
}

void update_pop_consumption(sys::state& state, dcon::nation_id n, pop_type_by_nation_array const& pop_money, ve::vectorizable_buffer<float, dcon::pop_type_id> const& life_needs_costs, ve::vectorizable_buffer<float, dcon::pop_type_id> const& everyday_needs_costs, ve::vectorizable_buffer<float, dcon::pop_type_id> const& luxury_needs_costs) {

	uint32_t total_commodities = state.world.commodity_size();
	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		float total_budget = pop_money.get(n, t);
		float total_pop_scaled = state.world.nation_get_demographics(n, demographics::to_key(state, t)) / needs_scaling_factor;
		float ln_cost = life_needs_costs.get(t) * total_pop_scaled;
		float en_cost = everyday_needs_costs.get(t) * total_pop_scaled;
		float xn_cost = luxury_needs_costs.get(t) * total_pop_scaled;

		float life_needs_fraction = total_budget > ln_cost ? 1.0f : total_budget / ln_cost;
		total_budget -= ln_cost;
		float everyday_needs_fraction = total_budget > en_cost ? 1.0f : std::max(0.0f, total_budget / en_cost);
		total_budget -= en_cost;
		float luxury_needs_fraction = std::max(0.0f, total_budget / xn_cost);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			state.world.nation_get_real_demand(n, cid) += state.world.pop_type_get_life_needs(t, cid) * life_needs_fraction * total_pop_scaled;
		}
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			state.world.nation_get_real_demand(n, cid) += state.world.pop_type_get_everyday_needs(t, cid) * everyday_needs_fraction * total_pop_scaled;
		}
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			state.world.nation_get_real_demand(n, cid) += state.world.pop_type_get_luxury_needs(t, cid) * luxury_needs_fraction * total_pop_scaled;
		}
	});

}


void populate_needs_costs(sys::state& state, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices,
	ve::vectorizable_buffer<float, dcon::pop_type_id>& life_needs_costs, ve::vectorizable_buffer<float, dcon::pop_type_id>& everyday_needs_costs, ve::vectorizable_buffer<float, dcon::pop_type_id>& luxury_needs_costs) {

	state.world.execute_serial_over_pop_type([&](auto ids) {
		life_needs_costs.set(ids, ve::fp_vector{});
	});

	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_pop_type([&](auto ids) {
			auto ln = state.world.pop_type_get_life_needs(ids, c) * effective_prices.get(c);
			life_needs_costs.set(ids, ln + life_needs_costs.get(ids));
		});
	}

	state.world.execute_serial_over_pop_type([&](auto ids) {
		everyday_needs_costs.set(ids, ve::fp_vector{});
	});
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_pop_type([&](auto ids) {
			auto ln = state.world.pop_type_get_everyday_needs(ids, c) * effective_prices.get(c);
			everyday_needs_costs.set(ids, ln + everyday_needs_costs.get(ids));
		});
	}

	state.world.execute_serial_over_pop_type([&](auto ids) {
		luxury_needs_costs.set(ids, ve::fp_vector{});
	});
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_pop_type([&](auto ids) {
			auto ln = state.world.pop_type_get_luxury_needs(ids, c) * effective_prices.get(c);
			luxury_needs_costs.set(ids, ln + luxury_needs_costs.get(ids));
		});
	}
}

void daily_update(sys::state& state) {

	static commodity_by_nation_array army_costs(state);
	static commodity_by_nation_array navy_costs(state);
	static commodity_by_nation_array construction_costs(state);
	static pop_type_by_nation_array pop_money(state);

	/* potentially parallel block */

	army_costs.prepare(state);
	navy_costs.prepare(state);
	construction_costs.prepare(state);
	pop_money.prepare(state);

	
	update_factory_triggered_modifiers(state);
	populate_army_consumption(state, army_costs);
	populate_navy_consumption(state, navy_costs);
	populate_constuction_consumption(state, construction_costs);
	populate_pop_money(state, pop_money);

	/* end potentially parallel block */

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
		We will use the real demand from the *previous* day to determine how much of the purchasing will be done from the domestic and global pools (i.e. what percentage was able to be done from the cheaper pool). We will use that to calculate an effective price. And then, at the end of the current day, we will see how much of that purchasing actually came from each pool, etc. Depending on the stability of the simulation, we may, instead of taking the previous day, instead build this value iteratively as a linear combination of the new day and the previous day.

		when purchasing from global supply, prices are multiplied by (the nation's current effective tariff rate + its blockaded fraction + 1)
		*/

		static auto effective_prices = state.world.commodity_make_vectorizable_float_buffer();

		auto global_price_multiplier = global_market_price_multiplier(state, n);

		auto sl = state.world.nation_get_in_sphere_of(n);

		if(global_price_multiplier >= 1.0f) { // prefer domestic
			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

				auto domestic_supply = state.world.nation_get_domestic_market_pool(n, c)
					+ (sl ? state.world.nation_get_domestic_market_pool(sl, c)  : 0.0f)
					+ (state.world.nation_get_drawing_on_stockpiles(n, c) ?  state.world.nation_get_stockpiles(n, c) : 0.0f);
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
			}
		} else { // prefer global
			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

				auto domestic_supply = state.world.nation_get_domestic_market_pool(n, c)
					+ (sl ? state.world.nation_get_domestic_market_pool(sl, c) : 0.0f)
					+ (state.world.nation_get_drawing_on_stockpiles(n, c) ? state.world.nation_get_stockpiles(n, c) : 0.0f);
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
			}
		}

		static auto life_needs_costs = state.world.pop_type_make_vectorizable_float_buffer();
		static auto everyday_needs_costs = state.world.pop_type_make_vectorizable_float_buffer();
		static auto luxury_needs_costs = state.world.pop_type_make_vectorizable_float_buffer();

		populate_needs_costs(state, effective_prices, life_needs_costs, everyday_needs_costs, luxury_needs_costs);

		float mobilization_impact = state.world.nation_get_is_mobilized(n) ? military::mobilization_impact(state, n) : 1.0f;

		auto const min_wage_factor = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::minimum_wage) + 0.2f);
		float factory_min_wage = life_needs_costs.get(state.culture_definitions.primary_factory_worker) * min_wage_factor;
		float artisan_min_wage = life_needs_costs.get(state.culture_definitions.artisans);
		float farmer_min_wage = life_needs_costs.get(state.culture_definitions.farmers) * min_wage_factor;
		float laborer_min_wage = life_needs_costs.get(state.culture_definitions.laborers) * min_wage_factor;

		// clear real demand
		state.world.for_each_commodity([&](dcon::commodity_id c) {
			state.world.nation_set_real_demand(n, c, 0.0f);
		});

		/*
		consumption updates
		*/

		auto cap_prov = state.world.nation_get_capital(n);
		auto cap_continent = state.world.province_get_continent(cap_prov);
		auto cap_region = state.world.province_get_connected_region_id(cap_prov);

		for(auto p : state.world.nation_get_province_ownership(n)) {
			for(auto f : state.world.province_get_factory_location(p.get_province())) {
				// factory

				update_single_factory_consumption(state, f.get_factory(), n, p.get_province(), p.get_province().get_state_membership(),
					effective_prices, mobilization_impact, factory_min_wage,
					p.get_province().get_nation_from_province_control() != n, // is occupied
					p.get_province().get_connected_region_id() != cap_region && p.get_province().get_continent() != cap_continent // is overseas
				);
			}

			// artisan
			update_province_artisan_consumption(state, p.get_province(), n, effective_prices, mobilization_impact, artisan_min_wage, p.get_province().get_nation_from_province_control() != n);

			// rgo
			bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p.get_province()));
			update_province_rgo_consumption(state, p.get_province(), n, mobilization_impact, is_mine ? laborer_min_wage : farmer_min_wage, p.get_province().get_nation_from_province_control() != n);
		}

		update_pop_consumption(state, n, pop_money, life_needs_costs, everyday_needs_costs, luxury_needs_costs);

		auto national_commodities_spending = update_national_consumption(state, n, effective_prices, army_costs, navy_costs, construction_costs);

		/*
		perform actual consumption / purchasing subject to availability
		*/

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };

			auto dom_pool = state.world.nation_get_domestic_market_pool(n, c);
			auto sl_pool = (sl ? state.world.nation_get_domestic_market_pool(sl, c) : 0.0f);
			auto sp_pool = (state.world.nation_get_drawing_on_stockpiles(n, c) ? state.world.nation_get_stockpiles(n, c) : 0.0f);
			auto wm_pool = state.world.commodity_get_global_market_pool(c);

			auto total_supply = dom_pool + sl_pool + sp_pool + wm_pool;

			auto rd = state.world.nation_get_real_demand(n, c);
			state.world.nation_set_demand_satisfaction(n, c, rd > 0.0001f ? std::min(1.0f, total_supply / rd) : 1.0f);

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
			} else {
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
		ve::fp_vector sum;
		state.world.execute_serial_over_nation([&](auto nids) {
			sum = sum + state.world.nation_get_domestic_market_pool(nids, c);
			state.world.nation_set_domestic_market_pool(nids, c, 0.0f);
		});
		state.world.commodity_set_global_market_pool(c, sum.reduce());
	});

	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		auto n = dcon::nation_id{ dcon::nation_id::value_base_t(i) };

		/*
		perform production
		*/

		for(auto p : state.world.nation_get_province_ownership(n)) {
			for(auto f : state.world.province_get_factory_location(p.get_province())) {
				// factory
				update_single_factory_production(state, f.get_factory(), n);
			}

			// artisan
			update_province_artisan_production(state, p.get_province(), n);

			// rgo
			update_province_rgo_production(state, p.get_province(), n);
		}

		/*
		adjust pop satisfaction based on consumption
		*/

		/*
		pay pops, collect taxes & tariffs
		*/

		
	});

	/*
	adjust prices based on global production & consumption
	*/
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
	state.world.for_each_factory_type([&](dcon::factory_type_id t) {
		auto o = state.world.factory_type_get_output(t);
		if(o)
			state.world.commodity_set_key_factory(o,t);
	});
}

}

