#include "economy.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "prng.hpp"

namespace economy {

int32_t most_recent_price_record_index(sys::state& state) {
	return (state.current_date.value >> 4) % 32;
}

float full_spending_cost(sys::state& state, dcon::nation_id n, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices);
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
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n) {	// TODO - should be "good" now
	auto nation_fat = dcon::fatten(state.world, n);
	for(auto prov_owner : nation_fat.get_province_ownership()) {
		auto prov = prov_owner.get_province();
		for(auto factloc : prov.get_factory_location()) {
			auto scale = factloc.get_factory().get_production_scale();
			if(scale < 0.05) {
				return true;
			}
		}
	}
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
	state.world.nation_resize_life_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_everyday_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_luxury_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_imports(state.world.commodity_size());
	state.world.nation_resize_army_demand(state.world.commodity_size());
	state.world.nation_resize_navy_demand(state.world.commodity_size());
	state.world.nation_resize_construction_demand(state.world.commodity_size());
	state.world.nation_resize_private_construction_demand(state.world.commodity_size());
	state.world.nation_resize_demand_satisfaction(state.world.commodity_size());
	state.world.commodity_resize_price_record(32);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto fc = fatten(state.world, c);
		fc.set_current_price(fc.get_cost());
		fc.set_total_consumption(1.0f);
		fc.set_total_production(1.0f);
		fc.set_total_real_demand(1.0f);

		for(int32_t i = 0; i < 32; ++i) {
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

		auto amount = std::ceil(1.1f * (state.world.province_get_demographics(p, demographics::to_key(state, is_mine ? state.culture_definitions.laborers : state.culture_definitions.farmers)) +
			state.world.province_get_demographics(p, demographics::to_key(state, state.culture_definitions.slaves))) / 40000.0f);

		fp.set_rgo_size(std::max(1.0f, amount));

		fp.set_rgo_production_scale(1.0f);
		fp.set_artisan_production_scale(1.0f);

		randomly_assign_artisan_production(state, p);
	});

	state.world.for_each_nation([&](dcon::nation_id n) {
		auto fn = fatten(state.world, n);
		fn.set_administrative_spending(int8_t(80));
		fn.set_military_spending(int8_t(60));
		fn.set_education_spending(int8_t(100));
		fn.set_social_spending(int8_t(100));
		fn.set_land_spending(int8_t(30));
		fn.set_naval_spending(int8_t(30));
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
	state.world.for_each_commodity([&](auto id) {
		base_prices.set(id, state.world.commodity_get_cost(id));
	});

	state.world.for_each_nation([&](dcon::nation_id n) {
		state.world.nation_set_stockpiles(n, money, 2.0f * full_spending_cost(state, n, base_prices));
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
		return std::max(0.0f, effective_tariff_rate(state, n)) + float(state.world.nation_get_central_blockaded(n)) / float(central_ports) + 1.0f;
	} else {
		return std::max(0.0f, effective_tariff_rate(state, n)) + 1.0f;
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

float factory_total_employment(sys::state const& state, dcon::factory_id f) {
	// TODO: Document this, also is this a stub?
	auto primary_employment = state.world.factory_get_primary_employment(f);
	auto secondary_employment = state.world.factory_get_secondary_employment(f);
	return factory_max_employment(state, f) * (state.economy_definitions.craftsmen_fraction * primary_employment + (1 - state.economy_definitions.craftsmen_fraction) * secondary_employment);
}

void update_factory_employment(sys::state& state) {
	state.world.for_each_state_instance([&](dcon::state_instance_id si) {
		float primary_pool = state.world.state_instance_get_demographics(si, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		float secondary_pool = state.world.state_instance_get_demographics(si, demographics::to_key(state, state.culture_definitions.secondary_factory_worker));

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
					state.world.factory_set_secondary_employment(ordered_factories[i], scale * state.world.factory_get_production_scale(ordered_factories[i]));
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

float rgo_full_production_quantity(sys::state const& state, dcon::nation_id n, dcon::province_id p) {
	/*
	- We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size + technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
	- We add its production to domestic supply, calculating that amount basically in the same way we do for factories, by computing RGO-throughput x RGO-output x RGO-size x base-commodity-production-quantity, except that it is affected by different modifiers.
	*/
	auto c = state.world.province_get_rgo(p);
	bool is_mine = state.world.commodity_get_is_mine(c);

	auto val = rgo_effective_size(state, n, p) * state.world.commodity_get_rgo_amount(c) *
		(1.0f
			+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_throughput)
			+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_throughput)
			+ state.world.province_get_modifier_values(p, is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff)
			+ state.world.nation_get_modifier_values(n, is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff)
			) *
		(1.0f
			+ state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::local_rgo_output)
			+ state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rgo_output)
			+ state.world.nation_get_rgo_goods_output(n, c)
			);
	return val;
}

inline constexpr float production_scale_delta = 0.05f;

void update_single_factory_consumption(sys::state& state, dcon::factory_id f, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices, float mobilization_impact, float expected_min_wage, bool occupied, bool overseas) {

	auto fac = fatten(state.world, f);
	auto fac_type = fac.get_building_type();

	float input_total = 0.0f;
	float min_input_available = 1.0f;
	auto& inputs = fac_type.get_inputs();
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total += inputs.commodity_amounts[i] * effective_prices.get(inputs.commodity_type[i]);
			min_input_available = std::min(min_input_available, state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[i]));
		} else {
			break;
		}
	}
	float min_e_input_available = 1.0f;
	auto& e_inputs = fac_type.get_efficiency_inputs();
	for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
		if(e_inputs.commodity_type[i]) {
			input_total += e_inputs.commodity_amounts[i] * effective_prices.get(e_inputs.commodity_type[i]);
			min_e_input_available = std::min(min_e_input_available, state.world.nation_get_demand_satisfaction(n, e_inputs.commodity_type[i]));
		} else {
			break;
		}
	}

	float output_total = fac_type.get_output_amount() * state.world.commodity_get_current_price(fac_type.get_output());


	float total_state_pop = std::max(0.01f, state.world.state_instance_get_demographics(s, demographics::total));
	float owner_fraction = total_state_pop > 0 ? std::min(0.05f, state.world.state_instance_get_demographics(s, demographics::to_key(state, state.culture_definitions.capitalists)) / total_state_pop) : 0.0f;

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

	bool profitable = ((0.75f + 0.25f * min_e_input_available) * output_total * throughput_multiplier * output_multiplier * min_input_available - expected_min_wage * (factory_per_level_employment / needs_scaling_factor) - input_multiplier * throughput_multiplier * input_total * min_input_available) >= 0.0f;
	state.world.factory_set_unprofitable(f, !profitable);

	if(profitable || state.world.factory_get_subsidized(f)) {
		// increase production scale ?
		auto new_production_scale = std::min(1.0f, state.world.factory_get_production_scale(f) + production_scale_delta);
		state.world.factory_set_production_scale(f, new_production_scale);
		auto effective_production_scale = std::min(new_production_scale * fac.get_level(), max_production_scale);

		// register real demand : input_multiplier * throughput_multiplier * level * primary_employment
		float input_scale = input_multiplier * throughput_multiplier * effective_production_scale;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(inputs.commodity_type[i]) {
				state.world.nation_get_real_demand(n, inputs.commodity_type[i]) += input_scale * inputs.commodity_amounts[i];
				assert(std::isfinite(state.world.nation_get_real_demand(n, inputs.commodity_type[i])));
			} else {
				break;
			}
		}

		// and for efficiency inputs
		//  the consumption of efficiency inputs is (national-factory-maintenance-modifier + 1) x input-multiplier x throughput-multiplier x factory level
		auto const mfactor = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
		for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
			if(e_inputs.commodity_type[i]) {
				state.world.nation_get_real_demand(n, e_inputs.commodity_type[i]) += mfactor * input_scale * e_inputs.commodity_amounts[i];
				assert(std::isfinite(state.world.nation_get_real_demand(n, e_inputs.commodity_type[i])));
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
		for(uint32_t i = 0; i < small_commodity_set::set_size; ++i) {
			if(fac_type.get_efficiency_inputs().commodity_type[i]) {
				min_efficiency_input = std::min(min_efficiency_input, state.world.nation_get_demand_satisfaction(n, fac_type.get_efficiency_inputs().commodity_type[i]));
			} else {
				break;
			}
		}

		auto amount = (0.75f + 0.25f * min_efficiency_input) * min_input * production;

		state.world.factory_set_actual_production(f, amount);
		state.world.nation_get_domestic_market_pool(n, fac_type.get_output()) += amount;

		auto money_made = (0.75f + 0.25f * min_efficiency_input) * min_input * state.world.factory_get_full_profit(f);
		state.world.factory_set_full_profit(f, money_made);
		// pay wages ?
	} else {

	}
}

void update_province_rgo_consumption(sys::state& state, dcon::province_id p, dcon::nation_id n, float mobilization_impact, float expected_min_wage, bool occupied) {

	auto max_production = rgo_full_production_quantity(state, n, p);
	auto pops_max = rgo_max_employment(state, n, p);

	bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));

	auto pop_wages = std::min(pops_max, state.world.province_get_demographics(p, demographics::to_employment_key(state, is_mine ? state.culture_definitions.laborers : state.culture_definitions.farmers))) * expected_min_wage / needs_scaling_factor;
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

	if(c == money) {
		assert(std::isfinite(amount * state.defines.gold_to_cash_rate));
		state.world.nation_get_stockpiles(n, money) += amount * state.defines.gold_to_cash_rate;
	}
}

void update_province_artisan_consumption(sys::state& state, dcon::province_id p, dcon::nation_id n, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices, float mobilization_impact, float expected_min_wage, bool occupied) {

	if((state.current_date.to_raw_value() & 63) == (p.value & 63)) {
		randomly_assign_artisan_production(state, p); // randomly switch once every other month
	}

	float input_total = 0.0f;
	auto artisan_prod_type = state.world.province_get_artisan_production(p);
	auto& inputs = state.world.commodity_get_artisan_inputs(artisan_prod_type);
	float min_available = 1.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_total += inputs.commodity_amounts[i] * effective_prices.get(inputs.commodity_type[i]);
			min_available = std::min(min_available, state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[i]));
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

	// note: we add min available to try to anticipate how availability will limit profits
	// note scaled minimum wage to aim at 0.6 ln satisfaction
	bool profitable = (output_total * output_multiplier * throughput_multiplier * min_available - 0.6f * expected_min_wage * (10'000.0f / needs_scaling_factor) - input_multiplier * input_total * throughput_multiplier * min_available) >= 0.0f;

	if(profitable) {
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(inputs.commodity_type[i]) {
				state.world.nation_get_real_demand(n, inputs.commodity_type[i]) += input_multiplier * throughput_multiplier * max_production_scale * inputs.commodity_amounts[i];
				assert(std::isfinite(state.world.nation_get_real_demand(n, inputs.commodity_type[i])));
			} else {
				break;
			}
		}

		state.world.province_set_artisan_actual_production(p, state.world.commodity_get_artisan_output_amount(artisan_prod_type) * throughput_multiplier * output_multiplier * max_production_scale);
		state.world.province_set_artisan_full_profit(p, (output_total * output_multiplier - input_multiplier * input_total) * throughput_multiplier * max_production_scale);
	} else {
		// switch production type
		randomly_assign_artisan_production(state, p);
		state.world.province_set_artisan_actual_production(p, 0.0f);
		state.world.province_set_artisan_full_profit(p, 0.0f);
	}
}

void update_province_artisan_production(sys::state& state, dcon::province_id p, dcon::nation_id n) {

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
	} else {

	}
}



void populate_army_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_nation([&](auto ids) {
			state.world.nation_set_army_demand(ids, cid, 0.0f);
		});
	}

	state.world.for_each_regiment([&](dcon::regiment_id r) {
		auto reg = fatten(state.world, r);
		auto type = state.world.regiment_get_type(r);
		auto owner = reg.get_army_from_army_membership().get_controller_from_army_control();

		if(owner) {
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					state.world.nation_get_army_demand(owner, supply_cost.commodity_type[i]) += supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(owner, type).supply_consumption * (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption) + 1.0f);
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
		state.world.execute_serial_over_nation([&](auto ids) {
			state.world.nation_set_navy_demand(ids, cid, 0.0f);
		});
	}

	state.world.for_each_ship([&](dcon::ship_id r) {
		auto shp = fatten(state.world, r);
		auto type = state.world.ship_get_type(r);
		auto owner = shp.get_navy_from_navy_membership().get_controller_from_navy_control();

		if(owner) {
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					state.world.nation_get_navy_demand(owner, supply_cost.commodity_type[i]) += supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(owner, type).supply_consumption * (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption) + 1.0f);
				} else {
					break;
				}
			}
		}
	});
}

void populate_construction_consumption(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_nation([&](auto ids) {
			state.world.nation_set_construction_demand(ids, cid, 0.0f);
		});
	}

	for(auto lc : state.world.in_province_land_construction) {
		auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(lc));
		auto owner = state.world.province_get_nation_from_province_ownership(province);
		if(owner && state.world.province_get_nation_from_province_control(province) == owner) {

			auto& base_cost = state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(lc)].build_cost;
			auto& current_purchased = state.world.province_land_construction_get_purchased_goods(lc);
			float construction_time = float(state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(lc)].build_time);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
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
				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
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
			switch(province_building_type(c.get_type())) {
				case province_building_type::railroad:
				{
					auto& base_cost = state.economy_definitions.railroad_definition.cost;
					auto& current_purchased = c.get_purchased_goods();
					float construction_time = float(state.economy_definitions.railroad_definition.time);

					for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
						if(base_cost.commodity_type[i]) {
							if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
								state.world.nation_get_construction_demand(owner, base_cost.commodity_type[i]) +=
								base_cost.commodity_amounts[i] / construction_time;
						} else {
							break;
						}
					}
					break;
				}
				case province_building_type::fort:
				{
					auto& base_cost = state.economy_definitions.fort_definition.cost;
					auto& current_purchased = c.get_purchased_goods();
					float construction_time = float(state.economy_definitions.fort_definition.time);

					for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
						if(base_cost.commodity_type[i]) {
							if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
								state.world.nation_get_construction_demand(owner, base_cost.commodity_type[i]) +=
								base_cost.commodity_amounts[i] / construction_time;
						} else {
							break;
						}
					}
					break;
				}
				case province_building_type::naval_base:
				{
					auto& base_cost = state.economy_definitions.naval_base_definition.cost;
					auto& current_purchased = c.get_purchased_goods();
					float construction_time = float(state.economy_definitions.naval_base_definition.time);

					for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
						if(base_cost.commodity_type[i]) {
							if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
								state.world.nation_get_construction_demand(owner, base_cost.commodity_type[i]) +=
								base_cost.commodity_amounts[i] / construction_time;
						} else {
							break;
						}
					}
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
			float construction_time = float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_cost) + 1.0f;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod)
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
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_nation([&](auto ids) {
			state.world.nation_set_private_construction_demand(ids, cid, 0.0f);
		});
	}

	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		if(owner && owner == c.get_province().get_nation_from_province_control() && c.get_is_pop_project() && province_building_type(c.get_type()) == province_building_type::railroad) {
			auto& base_cost = state.economy_definitions.railroad_definition.cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = float(state.economy_definitions.railroad_definition.time);

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
			float construction_time = float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_cost) + 1.0f)
				* state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::factory_owner_cost);

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
	uint32_t total_commodities = state.world.commodity_size();
	float l_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float n_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float c_spending = float(state.world.nation_get_construction_spending(n)) / 100.0f;

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += state.world.nation_get_army_demand(n, cid) * l_spending * effective_prices.get(cid);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += state.world.nation_get_navy_demand(n, cid) * n_spending * effective_prices.get(cid);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += state.world.nation_get_construction_demand(n, cid) * c_spending * effective_prices.get(cid);
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			total += difference * effective_prices.get(cid);
		}
	}
	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));

	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				total += overseas_factor * effective_prices.get(cid);
			}
		}
	}

	// direct payments to pops

	auto const a_spending = float(state.world.nation_get_administrative_spending(n)) / 100.0f;
	auto const s_spending = state.world.nation_get_administrative_efficiency(n) * float(state.world.nation_get_social_spending(n)) / 100.0f;
	auto const e_spending = float(state.world.nation_get_education_spending(n)) / 100.0f;
	auto const m_spending = float(state.world.nation_get_military_spending(n)) / 100.0f;
	auto const p_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level);
	auto const unemp_level = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit);

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
	});

	assert(std::isfinite(total));

	return total;
}


float full_private_investment_cost(sys::state& state, dcon::nation_id n, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices) {

	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += state.world.nation_get_private_construction_demand(n, cid) * effective_prices.get(cid);
	}
	return total;
}

void update_national_consumption(sys::state& state, dcon::nation_id n, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices, float spending_scale) { // returns: national spending level

	uint32_t total_commodities = state.world.commodity_size();

	float l_spending = float(state.world.nation_get_land_spending(n)) / 100.0f;
	float n_spending = float(state.world.nation_get_naval_spending(n)) / 100.0f;
	float c_spending = float(state.world.nation_get_construction_spending(n)) / 100.0f;


	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.nation_get_real_demand(n, cid) += state.world.nation_get_army_demand(n, cid) * l_spending * spending_scale;
		assert(std::isfinite(state.world.nation_get_real_demand(n, cid)));
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.nation_get_real_demand(n, cid) += state.world.nation_get_navy_demand(n, cid) * n_spending * spending_scale;
		assert(std::isfinite(state.world.nation_get_real_demand(n, cid)));
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.nation_get_real_demand(n, cid) += state.world.nation_get_construction_demand(n, cid) * c_spending * spending_scale;
		assert(std::isfinite(state.world.nation_get_real_demand(n, cid)));
	}
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
		if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
			state.world.nation_get_real_demand(n, cid) += difference * spending_scale;
			assert(std::isfinite(state.world.nation_get_real_demand(n, cid)));
		}
	}

	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));

	if(overseas_factor > 0) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
				state.world.nation_get_real_demand(n, cid) += overseas_factor * spending_scale;
				assert(std::isfinite(state.world.nation_get_real_demand(n, cid)));
			}
		}
	}

}

void update_pop_consumption(sys::state& state, dcon::nation_id n, dcon::province_id p, float base_demand, float invention_factor) {
	uint32_t total_commodities = state.world.commodity_size();

	static auto ln_demand_vector = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.execute_serial_over_pop_type([&](auto ids) {
		ln_demand_vector.set(ids, ve::fp_vector{});
	});
	static auto en_demand_vector = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.execute_serial_over_pop_type([&](auto ids) {
		en_demand_vector.set(ids, ve::fp_vector{});
	});
	static auto lx_demand_vector = state.world.pop_type_make_vectorizable_float_buffer();
	state.world.execute_serial_over_pop_type([&](auto ids) {
		lx_demand_vector.set(ids, ve::fp_vector{});
	});

	//needs_scaling_factor

	auto nation_rules = state.world.nation_get_combined_issue_rules(n);
	bool nation_allows_investment = state.world.nation_get_is_civilized(n) && (nation_rules & (issue_rule::pop_build_factory | issue_rule::pop_expand_factory)) != 0;

	for(auto pl : state.world.province_get_pop_location(p)) {
		auto t = pl.get_pop().get_poptype();
		assert(t);
		auto total_budget = pl.get_pop().get_savings();
		auto total_pop = pl.get_pop().get_size();

		float ln_cost = state.world.nation_get_life_needs_costs(n, t) * total_pop / needs_scaling_factor;
		float en_cost = state.world.nation_get_everyday_needs_costs(n, t) * total_pop / needs_scaling_factor;
		float xn_cost = state.world.nation_get_luxury_needs_costs(n, t) * total_pop / needs_scaling_factor;

		float life_needs_fraction = total_budget >= ln_cost ? 1.0f : total_budget / ln_cost;
		total_budget -= ln_cost;
		float everyday_needs_fraction = total_budget >= en_cost ? 1.0f : std::max(0.0f, total_budget / en_cost);
		total_budget -= en_cost;
		float luxury_needs_fraction =
			[&]() {
			if(!nation_allows_investment || (t != state.culture_definitions.aristocrat && t != state.culture_definitions.capitalists))
				return xn_cost > 0.0f ? std::clamp(total_budget / xn_cost, 0.0f, 1.0f) : 0.0f;
			else if(t == state.culture_definitions.capitalists) {
				state.world.nation_get_private_investment(n) += total_budget > 0.0f ? total_budget * 0.005f : 0.0f;
				return xn_cost > 0.0f ? std::clamp(total_budget * 0.995f / xn_cost, 0.0f, 1.0f) : 0.0f;
			} else {
				state.world.nation_get_private_investment(n) += total_budget > 0.0f ? total_budget * 0.05f : 0.0f;
				return xn_cost > 0.0f ? std::clamp(total_budget * 0.95f / xn_cost, 0.0f, 1.0f) : 0.0f;
			}
			}();
		assert(std::isfinite(life_needs_fraction));
		assert(std::isfinite(everyday_needs_fraction));
		assert(std::isfinite(luxury_needs_fraction));

		state.world.pop_set_life_needs_satisfaction(pl.get_pop(), life_needs_fraction);
		state.world.pop_set_everyday_needs_satisfaction(pl.get_pop(), everyday_needs_fraction);
		state.world.pop_set_luxury_needs_satisfaction(pl.get_pop(), luxury_needs_fraction);

		ln_demand_vector.get(t) += life_needs_fraction * total_pop / needs_scaling_factor;
		en_demand_vector.get(t) += everyday_needs_fraction * total_pop / needs_scaling_factor;
		lx_demand_vector.get(t) += luxury_needs_fraction * total_pop / needs_scaling_factor;
	}


	float ln_mul[] = {
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_life_needs) + 1.0f
	};
	float en_mul[] = {
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
	};
	float lx_mul[] = {
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
	};

	state.world.for_each_pop_type([&](dcon::pop_type_id t) {
		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf))) {
				auto strata = state.world.pop_type_get_strata(t);

				state.world.nation_get_real_demand(n, cid) += state.world.pop_type_get_life_needs(t, cid) * ln_demand_vector.get(t) * base_demand * ln_mul[strata];

				state.world.nation_get_real_demand(n, cid) += state.world.pop_type_get_everyday_needs(t, cid) * en_demand_vector.get(t) * base_demand * invention_factor * en_mul[strata];

				state.world.nation_get_real_demand(n, cid) += state.world.pop_type_get_luxury_needs(t, cid) * lx_demand_vector.get(t) * base_demand * invention_factor * lx_mul[strata];

				assert(std::isfinite(state.world.nation_get_real_demand(n, cid)));
			}
		}
	});

}


void populate_needs_costs(sys::state& state, ve::vectorizable_buffer<float, dcon::commodity_id> const& effective_prices, dcon::nation_id n, float base_demand, float invention_factor) {

	/*
	- Each pop strata and needs type has its own demand modifier, calculated as follows:
	- (national-modifier-to-goods-demand + define:BASE_GOODS_DEMAND) x (national-modifier-to-specific-strata-and-needs-type + 1) x (define:INVENTION_IMPACT_ON_DEMAND x number-of-unlocked-inventions + 1, but for non-life-needs only)
	- Each needs demand is also multiplied by  2 - the nation's administrative efficiency if the pop has education / admin / military income for that need category
	- We calculate an adjusted pop-size as (0.5 + pop-consciousness / define:PDEF_BASE_CON) x (for non-colonial pops: 1 + national-plurality (as a fraction of 100)) x pop-size
	*/

	uint32_t total_commodities = state.world.commodity_size();

	{
		float mul[] = {
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_life_needs) + 1.0f
		};

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf))) {
				state.world.for_each_pop_type([&](auto ids) {
					auto ln = state.world.pop_type_get_life_needs(ids, c) * effective_prices.get(c) * base_demand * mul[state.world.pop_type_get_strata(ids)];
					state.world.nation_set_life_needs_costs(n, ids, ln + state.world.nation_get_life_needs_costs(n, ids));
					assert(std::isfinite(state.world.nation_get_life_needs_costs(n, ids)));
				});
			}
		}
	}
	{
		float mul[] = {
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
		};

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf))) {
				state.world.for_each_pop_type([&](auto ids) {
					auto ln = state.world.pop_type_get_everyday_needs(ids, c) * effective_prices.get(c) * base_demand * invention_factor * mul[state.world.pop_type_get_strata(ids)];
					state.world.nation_set_everyday_needs_costs(n, ids, ln + state.world.nation_get_everyday_needs_costs(n, ids));
					assert(std::isfinite(state.world.nation_get_everyday_needs_costs(n, ids)));
				});
			}
		}
	}
	{
		float mul[] = {
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_luxury_needs) + 1.0f
		};

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
			auto kf = state.world.commodity_get_key_factory(c);
			if(state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf))) {
				state.world.for_each_pop_type([&](auto ids) {
					auto ln = state.world.pop_type_get_luxury_needs(ids, c) * effective_prices.get(c) * base_demand * invention_factor * mul[state.world.pop_type_get_strata(ids)];
					state.world.nation_set_luxury_needs_costs(n, ids, ln + state.world.nation_get_luxury_needs_costs(n, ids));
					assert(std::isfinite(state.world.nation_get_luxury_needs_costs(n, ids)));
				});
			}
		}
	}
}

void advance_construction(sys::state& state, dcon::nation_id n) {
	uint32_t total_commodities = state.world.commodity_size();

	float c_spending = state.world.nation_get_spending_level(n) * float(state.world.nation_get_construction_spending(n)) / 100.0f;
	float p_spending = state.world.nation_get_private_investment_effective_fraction(n);

	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
		auto d_sat = state.world.nation_get_demand_satisfaction(n, c);
		state.world.nation_get_construction_demand(n, c) *= c_spending * d_sat;
		state.world.nation_get_private_construction_demand(n, c) *= p_spending * d_sat;
	}


	for(auto p : state.world.nation_get_province_ownership(n)) {
		if(p.get_province().get_nation_from_province_control() != n)
			continue;

		for(auto pops : p.get_province().get_pop_location()) {
			auto rng = pops.get_pop().get_province_land_construction();
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());
				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i]) {
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
				float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i]) {
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
			if(!c.get_is_pop_project()) {
				switch(province_building_type(c.get_type())) {
					case province_building_type::railroad:
					{
						auto& base_cost = state.economy_definitions.railroad_definition.cost;
						auto& current_purchased = c.get_purchased_goods();
						float construction_time = float(state.economy_definitions.railroad_definition.time);

						for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
							if(base_cost.commodity_type[i]) {
								if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i]) {
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
						break;
					}
					case province_building_type::fort:
					{
						auto& base_cost = state.economy_definitions.fort_definition.cost;
						auto& current_purchased = c.get_purchased_goods();
						float construction_time = float(state.economy_definitions.fort_definition.time);

						for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
							if(base_cost.commodity_type[i]) {
								if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i]) {
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
						break;
					}
					case province_building_type::naval_base:
					{
						auto& base_cost = state.economy_definitions.naval_base_definition.cost;
						auto& current_purchased = c.get_purchased_goods();
						float construction_time = float(state.economy_definitions.naval_base_definition.time);

						for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
							if(base_cost.commodity_type[i]) {
								if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i]) {
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
						break;
					}
				}
			} else if(c.get_is_pop_project() && province_building_type(c.get_type()) == province_building_type::railroad) {
				auto& base_cost = state.economy_definitions.railroad_definition.cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = float(state.economy_definitions.railroad_definition.time);

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
			float construction_time = float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod) {
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
			float construction_time = float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f)
				* state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_owner_cost);

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

void daily_update(sys::state& state) {

	/* initialization parallel block */

	concurrency::parallel_for(0, 8, [&](int32_t index) {
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
				} else if(domestic_supply + global_supply > 0) {
					auto domestic_satisfiable_fraction = domestic_supply / (domestic_supply + global_supply);
					effective_prices.set(c,
						base_price * domestic_satisfiable_fraction
						+ base_price * (1.0f - domestic_satisfiable_fraction) * global_price_multiplier);
				} else {
					effective_prices.set(c, base_price * global_price_multiplier);
				}

				assert(std::isfinite(effective_prices.get(c)));
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
				} else if(domestic_supply + global_supply > 0) {
					auto global_satisfiable_fraction = global_supply / (domestic_supply + global_supply);
					effective_prices.set(c,
						base_price * global_satisfiable_fraction * global_price_multiplier
						+ base_price * (1.0f - global_satisfiable_fraction));
				} else {
					effective_prices.set(c, base_price);
				}

				assert(std::isfinite(effective_prices.get(c)));
			}
		}

		float base_demand = state.defines.base_goods_demand + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::goods_demand);

		int32_t num_inventions = 0;
		state.world.for_each_invention([&](auto iid) {
			num_inventions += int32_t(state.world.nation_get_active_inventions(n, iid));
		});
		float invention_factor = float(num_inventions) * state.defines.invention_impact_on_demand + 1.0f;

		populate_needs_costs(state, effective_prices, n, base_demand, invention_factor);

		float mobilization_impact = state.world.nation_get_is_mobilized(n) ? military::mobilization_impact(state, n) : 1.0f;

		auto const min_wage_factor = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::minimum_wage) + 0.2f);
		float factory_min_wage = state.world.nation_get_life_needs_costs(n, state.culture_definitions.primary_factory_worker) * min_wage_factor;
		float artisan_min_wage = state.world.nation_get_life_needs_costs(n, state.culture_definitions.artisans);
		float farmer_min_wage = state.world.nation_get_life_needs_costs(n, state.culture_definitions.farmers) * min_wage_factor;
		float laborer_min_wage = state.world.nation_get_life_needs_costs(n, state.culture_definitions.laborers) * min_wage_factor;

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

			update_pop_consumption(state, n, p.get_province(), base_demand, invention_factor);
		}

		{
			// update national spending
			//
			// step 1: figure out total
			float total = full_spending_cost(state, n, effective_prices);

			// step 2: limit to actual budget
			float budget = state.world.nation_get_stockpiles(n, economy::money); // (TODO: make debt possible)
			float spending_scale = (total < 0.001 || total <= budget) ? 1.0f : budget / total;

			assert(spending_scale >= 0);
			assert(budget >= 0);
			assert(std::isfinite(spending_scale));
			assert(std::isfinite(budget));

			state.world.nation_get_stockpiles(n, economy::money) -= std::min(budget, total * spending_scale);
			state.world.nation_set_spending_level(n, spending_scale);

			float pi_total = full_private_investment_cost(state, n, effective_prices);
			float pi_budget = state.world.nation_get_private_investment(n);
			state.world.nation_set_private_investment_effective_fraction(n, pi_total <= pi_budget ? 1.0f : pi_budget / pi_total);
			state.world.nation_set_private_investment(n, std::max(0.0f, pi_budget - pi_total));

			update_national_consumption(state, n, effective_prices, spending_scale);
		}

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
		ve::fp_vector sum;
		state.world.execute_serial_over_nation([&](auto nids) {
			sum = sum + state.world.nation_get_domestic_market_pool(nids, c);
			state.world.nation_set_domestic_market_pool(nids, c, 0.0f);
		});
		state.world.commodity_set_global_market_pool(c, sum.reduce());
	});

	/*
	pay non "employed" pops (also zeros money for "employed" pops)
	*/

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto owners = nations::owner_of_pop(state, ids);
		auto owner_spending = state.world.nation_get_spending_level(owners);

		auto pop_of_type = state.world.pop_get_size(ids);
		auto adj_pop_of_type = pop_of_type / needs_scaling_factor;

		auto const a_spending = owner_spending * ve::to_float(state.world.nation_get_administrative_spending(owners)) / 100.0f;
		auto const s_spending = owner_spending * state.world.nation_get_administrative_efficiency(owners) * ve::to_float(state.world.nation_get_social_spending(owners)) / 100.0f;
		auto const e_spending = owner_spending * ve::to_float(state.world.nation_get_education_spending(owners)) / 100.0f;
		auto const m_spending = owner_spending * ve::to_float(state.world.nation_get_military_spending(owners)) / 100.0f;
		auto const p_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::pension_level);
		auto const unemp_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::unemployment_benefit);

		auto types = state.world.pop_get_poptype(ids);

		auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
		auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
		auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

		auto ln_costs = ve::apply([&](dcon::pop_type_id pt, dcon::nation_id n) {
			return pt ? state.world.nation_get_life_needs_costs(n, pt) : 0.0f;
		}, types, owners);
		auto en_costs = ve::apply([&](dcon::pop_type_id pt, dcon::nation_id n) {
			return pt ? state.world.nation_get_everyday_needs_costs(n, pt) : 0.0f;
		}, types, owners);
		auto lx_costs = ve::apply([&](dcon::pop_type_id pt, dcon::nation_id n) {
			return pt ? state.world.nation_get_luxury_needs_costs(n, pt) : 0.0f;
		}, types, owners);

		auto acc_a = ve::select(ln_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * ln_costs, 0.0f);
		auto acc_e = ve::select(ln_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * ln_costs, 0.0f);
		auto acc_m = ve::select(ln_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * ln_costs, 0.0f);

		auto none_of_above = ln_types != int32_t(culture::income_type::military) && ln_types != int32_t(culture::income_type::education) && ln_types != int32_t(culture::income_type::administration);

		auto acc_u = ve::select(none_of_above,
			s_spending * adj_pop_of_type * p_level * ln_costs,
			0.0f);

		acc_a = acc_a + ve::select(en_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * en_costs, 0.0f);
		acc_e = acc_e + ve::select(en_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * en_costs, 0.0f);
		acc_m = acc_m + ve::select(en_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * en_costs, 0.0f);

		acc_a = acc_a + ve::select(lx_types == int32_t(culture::income_type::administration), a_spending * adj_pop_of_type * lx_costs, 0.0f);
		acc_e = acc_e + ve::select(lx_types == int32_t(culture::income_type::education), e_spending * adj_pop_of_type * lx_costs, 0.0f);
		acc_m = acc_m + ve::select(lx_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * lx_costs, 0.0f);

		acc_u = acc_u + ve::select(none_of_above && state.world.pop_type_get_has_unemployment(types),
			s_spending * (pop_of_type - state.world.pop_get_employment(ids)) * unemp_level * ln_costs,
			0.0f);

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
		auto n = dcon::nation_id{ dcon::nation_id::value_base_t(i) };

		if(state.world.nation_get_owned_province_count(n) == 0)
			return;

		/*
		prepare needs satisfaction caps
		*/

		ve::vectorizable_buffer<float, dcon::pop_type_id> ln_max = state.world.pop_type_make_vectorizable_float_buffer();
		ve::vectorizable_buffer<float, dcon::pop_type_id> en_max = state.world.pop_type_make_vectorizable_float_buffer();
		ve::vectorizable_buffer<float, dcon::pop_type_id> lx_max = state.world.pop_type_make_vectorizable_float_buffer();

		uint32_t total_commodities = state.world.commodity_size();

		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			float ln_total = 0.0f;
			float en_total = 0.0f;
			float lx_total = 0.0f;

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
				auto kf = state.world.commodity_get_key_factory(c);
				if(state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf))) {
					auto sat = state.world.nation_get_demand_satisfaction(n, c);

					auto ln_val = state.world.pop_type_get_life_needs(pt, c);
					ln_total += ln_val;
					ln_max.get(pt) += ln_val * sat;

					auto en_val = state.world.pop_type_get_everyday_needs(pt, c);
					en_total += en_val;
					en_max.get(pt) += en_val * sat;

					auto lx_val = state.world.pop_type_get_luxury_needs(pt, c);
					lx_total += lx_val;
					lx_max.get(pt) += lx_val * sat;
				}
			}
			if(ln_total > 0)
				ln_max.get(pt) /= ln_total;
			if(en_total > 0)
				en_max.get(pt) /= en_total;
			if(lx_total > 0)
				lx_max.get(pt) /= lx_total;
		});

		/*
		determine effective spending levels
		*/
		auto nations_commodity_spending = state.world.nation_get_spending_level(n);
		{
			float max_sp = 0.0f;
			float total = 0.0f;

			for(uint32_t k = 1; k < total_commodities; ++k) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

				auto sat = state.world.nation_get_demand_satisfaction(n, c);
				auto val = state.world.nation_get_navy_demand(n, c);
				total += val;
				max_sp += val * sat;
			}
			if(total > 0)
				max_sp /= total;

			state.world.nation_set_effective_naval_spending(n, nations_commodity_spending * max_sp * float(state.world.nation_get_naval_spending(n)) / 100.0f);
		}
		{
			float max_sp = 0.0f;
			float total = 0.0f;

			for(uint32_t k = 1; k < total_commodities; ++k) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

				auto sat = state.world.nation_get_demand_satisfaction(n, c);
				auto val = state.world.nation_get_army_demand(n, c);
				total += val;
				max_sp += val * sat;
			}
			if(total > 0)
				max_sp /= total;

			state.world.nation_set_effective_land_spending(n, nations_commodity_spending * max_sp * float(state.world.nation_get_naval_spending(n)) / 100.0f);
		}
		{
			float max_sp = 0.0f;
			float total = 0.0f;

			for(uint32_t k = 1; k < total_commodities; ++k) {
				dcon::commodity_id c{ dcon::commodity_id::value_base_t(k) };

				auto sat = state.world.nation_get_demand_satisfaction(n, c);
				auto val = state.world.nation_get_construction_demand(n, c);
				total += val;
				max_sp += val * sat;
			}
			if(total > 0)
				max_sp /= total;

			state.world.nation_set_effective_construction_spending(n, nations_commodity_spending * max_sp * float(state.world.nation_get_naval_spending(n)) / 100.0f);
		}
		/*
		fill stockpiles
		*/

		for(uint32_t k = 1; k < total_commodities; ++k) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
			auto difference = state.world.nation_get_stockpile_targets(n, cid) - state.world.nation_get_stockpiles(n, cid);
			if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(n, cid) == false) {
				state.world.nation_get_stockpiles(n, cid) += difference * nations_commodity_spending * state.world.nation_get_demand_satisfaction(n, cid);
			}
		}


		/*
		calculate overseas penalty
		*/

		{
			float count = 0.0f;
			float total = 0.0f;
			for(uint32_t k = 1; k < total_commodities; ++k) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

				auto kf = state.world.commodity_get_key_factory(cid);
				if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(n, kf)))) {
					total += state.world.nation_get_demand_satisfaction(n, cid);
					count += 1.0f;
				}
			}
			state.world.nation_set_overseas_penalty(n, 0.25f * (1.0f - nations_commodity_spending * total / count));
		}


		auto const min_wage_factor = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::minimum_wage) + 0.2f);
		float factory_min_wage = state.world.nation_get_life_needs_costs(n, state.culture_definitions.primary_factory_worker) * min_wage_factor;
		float farmer_min_wage = state.world.nation_get_life_needs_costs(n, state.culture_definitions.farmers) * min_wage_factor;
		float laborer_min_wage = state.world.nation_get_life_needs_costs(n, state.culture_definitions.laborers) * min_wage_factor;

		for(auto p : state.world.nation_get_province_ownership(n)) {
			/*
			perform production
			*/

			for(auto f : state.world.province_get_factory_location(p.get_province())) {
				// factory
				update_single_factory_production(state, f.get_factory(), n);
			}

			// artisan
			update_province_artisan_production(state, p.get_province(), n);

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

			/*
			pay "employed" pops
			*/

			{
				// ARTISAN
				auto const artisan_type = state.culture_definitions.artisans;
				float artisan_profit = state.world.province_get_artisan_full_profit(p.get_province());
				float num_artisans = state.world.province_get_demographics(p.get_province(), demographics::to_key(state, artisan_type));
				if(num_artisans > 0) {
					auto per_profit = artisan_profit / num_artisans;

					for(auto pl : p.get_province().get_pop_location()) {
						if(artisan_type == pl.get_pop().get_poptype()) {
							pl.get_pop().set_savings(state.inflation * pl.get_pop().get_size() * per_profit);
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

			float num_rgo_owners = state.world.state_instance_get_demographics(si.get_state(), demographics::to_key(state, state.culture_definitions.aristocrat));

			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto f : state.world.province_get_factory_location(p)) {
					total_profit += f.get_factory().get_full_profit();
				}

				{
					// FARMER / LABORER
					bool is_mine = state.world.commodity_get_is_mine(state.world.province_get_rgo(p));
					auto const worker = is_mine ? state.culture_definitions.laborers : state.culture_definitions.farmers;

					auto const min_wage = (is_mine ? laborer_min_wage : farmer_min_wage) / needs_scaling_factor;

					float total_min_to_workers = min_wage * state.world.province_get_demographics(p, demographics::to_employment_key(state, worker));
					float total_rgo_profit = state.world.province_get_rgo_full_profit(p);

					float total_worker_wage = 0.0f;

					float num_workers = state.world.province_get_demographics(p, demographics::to_key(state, worker));


					if(total_min_to_workers <= total_rgo_profit && num_rgo_owners > 0) {
						total_worker_wage = total_min_to_workers + (total_rgo_profit - total_min_to_workers) * 0.2f;
						rgo_owner_profit += (total_rgo_profit - total_min_to_workers) * 0.8f;
					} else {
						total_worker_wage = total_rgo_profit;
					}

					auto per_worker_profit = num_workers > 0 ? total_worker_wage / num_workers : 0.0f;

					for(auto pl : state.world.province_get_pop_location(p)) {
						if(worker == pl.get_pop().get_poptype()) {
							pl.get_pop().set_savings(state.inflation * pl.get_pop().get_size() * per_worker_profit);
							assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
						}
					}
				}
			});

			auto const per_rgo_owner_profit = num_rgo_owners > 0 ? rgo_owner_profit / num_rgo_owners : 0.0f;

			auto const min_wage = factory_min_wage / needs_scaling_factor;

			float total_min_to_pworkers = min_wage * state.world.state_instance_get_demographics(si.get_state(), demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
			float total_min_to_sworkers = min_wage * state.world.state_instance_get_demographics(si.get_state(), demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));

			float num_pworkers = state.world.state_instance_get_demographics(si.get_state(), demographics::to_key(state, state.culture_definitions.primary_factory_worker));
			float num_sworkers = state.world.state_instance_get_demographics(si.get_state(), demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
			float num_owners = state.world.state_instance_get_demographics(si.get_state(), demographics::to_key(state, state.culture_definitions.capitalists));

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
			} else if(num_pworkers + num_sworkers > 0){
				per_pworker_profit = total_profit / (num_pworkers + num_sworkers);
				per_sworker_profit = total_profit / (num_pworkers + num_sworkers);
			}


			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto pl : state.world.province_get_pop_location(p)) {
					if(state.culture_definitions.primary_factory_worker == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(state.inflation * pl.get_pop().get_size() * per_pworker_profit);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.secondary_factory_worker == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(state.inflation * pl.get_pop().get_size() * per_sworker_profit);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.capitalists == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(state.inflation * pl.get_pop().get_size() * per_owner_profit);
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
					} else if(state.culture_definitions.aristocrat == pl.get_pop().get_poptype()) {
						pl.get_pop().set_savings(state.inflation * pl.get_pop().get_size() * per_rgo_owner_profit);
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
			for(auto pl : p.get_province().get_pop_location()) {
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

		state.world.nation_set_total_rich_income(n, total_rich_tax_base);
		state.world.nation_set_total_middle_income(n, total_mid_tax_base);
		state.world.nation_set_total_poor_income(n, total_poor_tax_base);
		auto collected_tax = total_poor_tax_base * tax_eff * float(state.world.nation_get_poor_tax(n)) / 100.0f
			+ total_mid_tax_base * tax_eff * float(state.world.nation_get_middle_tax(n)) / 100.0f
			+ total_rich_tax_base * tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f;
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
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
				t_total += state.world.commodity_get_current_price(cid) * tariff_rate * state.world.nation_get_imports(n, cid);
			}
			assert(std::isfinite(t_total));
			assert(t_total >= 0);
			state.world.nation_get_stockpiles(n, money) += t_total;
		}

		// TODO: pay subsidies
	});

	/*
	adjust prices based on global production & consumption
	*/

	concurrency::parallel_for(uint32_t(1), total_commodities, [&](uint32_t k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
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

		auto base_price = state.world.commodity_get_cost(cid);
		auto current_price = state.world.commodity_get_current_price(cid);
		if(total_r_demand > prior_production * 1.02f) {
			current_price += 0.01f;
		} else if(total_r_demand < prior_production * 0.98f) {
			current_price -= 0.01f;
		}

		state.world.commodity_set_current_price(cid, std::clamp(current_price, base_price * 0.2f, base_price * 5.0f));
	});

	/*
	update inflation
	*/

	float primary_commodity_basket = 0.0f;
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			primary_commodity_basket += 0.7f * state.world.commodity_get_current_price(c) * state.world.pop_type_get_life_needs(pt, c);
			primary_commodity_basket += 0.3f * state.world.commodity_get_current_price(c) * state.world.pop_type_get_everyday_needs(pt, c);
		});
	});
	primary_commodity_basket /= float(state.world.pop_type_size());

	float total_pop = 0.0f;
	float total_pop_money = 0.0f;

	state.world.for_each_nation([&](dcon::nation_id n) {
		total_pop += state.world.nation_get_demographics(n, demographics::total);
		total_pop_money += state.world.nation_get_total_rich_income(n) + state.world.nation_get_total_middle_income(n) + state.world.nation_get_total_poor_income(n);
	});

	float target_money = total_pop * primary_commodity_basket / needs_scaling_factor;

	// total_pop_money / inflation = real, unadjustead money
	// want inflation s.t. inflation * r_money = target_money
	// ideal inflation = inflation * target_money / total_pop_money

	if(total_pop_money > 0.001f) {
		state.inflation = state.inflation * (0.8f + 0.2f * target_money / total_pop_money);
	}

	// make constructions:
	resolve_constructions(state);

	// make new investments
	for(auto n : state.world.in_nation) {
		auto nation_rules = n.get_combined_issue_rules();

		if(n.get_private_investment() > 0.001 &&  (nation_rules & (issue_rule::pop_build_factory | issue_rule::pop_expand_factory)) != 0) {

			bool found_investment = false;
			
			if(!found_investment) {
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

				for(auto s : states_in_order) {
					auto existing_constructions = state.world.state_instance_get_state_building_construction(s);
					if(existing_constructions.begin() != existing_constructions.end())
						break; // already building

					auto pw_num = state.world.state_instance_get_demographics(s, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
					auto pw_employed = state.world.state_instance_get_demographics(s, demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));

					if(pw_employed >= pw_num && pw_num > 0.0f)
						break; // no spare workers

					int32_t num_factories = 0;
					float profit = 0.0f;
					dcon::factory_id selected_factory;

					// is there an upgrade target ?
					auto d = state.world.state_instance_get_definition(s);
					for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
						if(p.get_province().get_nation_from_province_ownership() == n) {
							for(auto f : p.get_province().get_factory_location()) {
								++num_factories;
								if((nation_rules & issue_rule::pop_expand_factory) != 0 && f.get_factory().get_production_scale() >= 0.9f && f.get_factory().get_level() < uint8_t(255)) {
									if(auto new_p = f.get_factory().get_full_profit() / f.get_factory().get_level(); new_p > profit) {
										profit = new_p;
										selected_factory = f.get_factory();
									}
								}
							}
						}
					}

					if(selected_factory) {
						auto new_up = fatten(state.world, state.world.force_create_state_building_construction(s, n));
						new_up.set_is_pop_project(true);
						new_up.set_is_upgrade(true);
						new_up.set_type(state.world.factory_get_building_type(selected_factory));
						found_investment = true;
					}

					if(!found_investment && num_factories < int32_t(state.defines.factories_per_state) && (nation_rules & issue_rule::pop_build_factory) != 0) {
						// randomly try a valid (check coastal, unlocked, non existing) factory
						static std::vector<dcon::factory_type_id> possible;
						possible.clear();

						auto is_coastal = province::state_is_coastal(state, s);
						for(auto t : state.world.in_factory_type) {
							if(t.get_is_available_from_start() || state.world.nation_get_active_building(n, t)) {
								if(!state_contains_constructed_factory(state, s, t) && (is_coastal || !t.get_is_coastal())) {
									possible.push_back(t.id);
								}
							}
						}

						if(possible.size() != 0) {
							auto selected = rng::get_random(state, uint32_t((n.id.index() << 6) ^ s.index())) % possible.size();

							auto new_up = fatten(state.world, state.world.force_create_state_building_construction(s, n));
							new_up.set_is_pop_project(true);
							// new_up.set_is_upgrade(false);
							new_up.set_type(possible[selected]);
							found_investment = true;
						}
					}

					if(found_investment) {
						break;
					}
				}
			}
			if(!found_investment && (nation_rules & issue_rule::pop_build_factory) != 0) {
				static std::vector<std::pair<dcon::province_id, int32_t>> provinces_in_order;
				provinces_in_order.clear();
				for(auto si : n.get_state_ownership()) {
					if(si.get_state().get_capital().get_is_colonial() == false) {
						auto s = si.get_state().id;
						auto d = state.world.state_instance_get_definition(s);
						int32_t num_factories = 0;
						for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
							if(province::generic_can_build_railroads(state, p.get_province(), n) && p.get_province().get_nation_from_province_ownership() == n) {
								for(auto f : p.get_province().get_factory_location())
									num_factories += int32_t(f.get_factory().get_level());
								provinces_in_order.emplace_back(p.get_province().id, num_factories);
							}
						}
						// The state's number of factories is intentionally given to all the provinces within the state so the railroads aren't just built on a single province within a state
						for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
							if(province::generic_can_build_railroads(state, p.get_province(), n) && p.get_province().get_nation_from_province_ownership() == n)
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
					found_investment = true;
				}
			}
		}
		n.set_private_investment(0.0f);
	}
}

void regenerate_unsaved_values(sys::state& state) {

	state.world.nation_resize_life_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_everyday_needs_costs(state.world.pop_type_size());
	state.world.nation_resize_luxury_needs_costs(state.world.pop_type_size());

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

float government_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(n) - state.world.nation_get_central_province_count(n));
	auto o_adjust = 0.0f;
	if(overseas_factor > 0) {
		auto kf = state.world.commodity_get_key_factory(c);
		if(state.world.commodity_get_overseas_penalty(c) && (state.world.commodity_get_is_available_from_start(c) || (kf && state.world.nation_get_active_building(n, kf)))) {
			o_adjust = overseas_factor;
		}
	}

	return (state.world.nation_get_army_demand(n, c) + state.world.nation_get_navy_demand(n, c) + state.world.nation_get_construction_demand(n, c) + o_adjust);
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
			amount += (state.world.pop_type_get_life_needs(pt, c) + state.world.pop_type_get_everyday_needs(pt, c) + state.world.pop_type_get_luxury_needs(pt, c)) * state.world.nation_get_demographics(n, demographics::to_key(state, pt)) / needs_scaling_factor;
		});
	}
	return amount;
}

float nation_total_imports(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;

	auto const total_commodities = state.world.commodity_size();
	for(uint32_t k = 1; k < total_commodities; ++k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		total += std::max(0.0f, state.world.nation_get_real_demand(n, cid) * state.world.nation_get_demand_satisfaction(n, cid) - state.world.nation_get_domestic_market_pool(n, cid)) * state.world.commodity_get_current_price(cid);
	}

	return total;
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
		if(prov.get_rgo().id == economy::money) {
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
		auto adj_pop_of_type = state.world.nation_get_demographics(n, demographics::to_key(state, pt)) / economy::needs_scaling_factor;

		if(adj_pop_of_type <= 0)
			return;

		total += adj_pop_of_type * p_level * state.world.nation_get_life_needs_costs(n, pt);
		if(state.world.pop_type_get_has_unemployment(pt)) {
			auto emp = state.world.nation_get_demographics(n, demographics::to_employment_key(state, pt)) / economy::needs_scaling_factor;
			total += (adj_pop_of_type - emp) * unemp_level * state.world.nation_get_life_needs_costs(n, pt);
		}
	});
	return total;
}

float estimate_pop_payouts_by_income_type(sys::state& state, dcon::nation_id n, culture::income_type in) {
	auto total = 0.f;
	state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
		auto adj_pop_of_type = state.world.nation_get_demographics(n, demographics::to_key(state, pt)) / economy::needs_scaling_factor;

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
	switch(ps) {
		default:
		case culture::pop_strata::poor:
			return state.world.nation_get_total_poor_income(n);
		case culture::pop_strata::middle:
			return state.world.nation_get_total_middle_income(n);
		case culture::pop_strata::rich:
			return state.world.nation_get_total_rich_income(n);
	}
}

float estimate_loan_payments(sys::state& state, dcon::nation_id n) {
	// TODO
	return 0.f;
}

float estimate_subsidy_spending(sys::state& state, dcon::nation_id n) {
	// TODO
	return 0.f;
}

float estimate_diplomatic_balance(sys::state& state, dcon::nation_id n) {
	// TODO
	return 0.f;
}

float estimate_land_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += state.world.nation_get_army_demand(n, cid) * state.world.commodity_get_current_price(cid);
	}
	return total;
}

float estimate_naval_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += state.world.nation_get_navy_demand(n, cid) * state.world.commodity_get_current_price(cid);
	}
	return total;
}

float estimate_construction_spending(sys::state& state, dcon::nation_id n) {
	float total = 0.0f;
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		total += state.world.nation_get_construction_demand(n, cid) * state.world.commodity_get_current_price(cid);
	}
	return total;
}

float estimate_total_spending(sys::state& state, dcon::nation_id n) {
	float v = 0.f;
	v += economy::estimate_construction_spending(state, state.local_player_nation);
	v += economy::estimate_land_spending(state, state.local_player_nation);
	v += economy::estimate_naval_spending(state, state.local_player_nation);
	v += economy::estimate_social_spending(state, state.local_player_nation);
	v += economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education);
	v += economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::administration);
	v += economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military);
	v += economy::estimate_loan_payments(state, state.local_player_nation);
	v += economy::estimate_subsidy_spending(state, state.local_player_nation);
	return v;
}

float estimate_war_subsidies(sys::state& state, dcon::nation_id n) {
	/* total-nation-expenses x defines:WARSUBSIDIES_PERCENT */
	return estimate_total_spending(state, n) * state.defines.warsubsidies_percent;
}

construction_status province_building_construction(sys::state& state, dcon::province_id p, province_building_type t) {
	for(auto pb_con : state.world.province_get_province_building_construction(p)) {
		if(pb_con.get_type() == uint8_t(t)) {
			float total = 0.0f;
			float purchased = 0.0f;

			switch(t) {
				case province_building_type::railroad:
					for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
						total += state.economy_definitions.railroad_definition.cost.commodity_amounts[i];
						purchased += pb_con.get_purchased_goods().commodity_amounts[i];
					}
					return construction_status{ total > 0.0f ? purchased / total : 0.0f, true };
					break;
				case province_building_type::fort:
					for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
						total += state.economy_definitions.fort_definition.cost.commodity_amounts[i];
						purchased += pb_con.get_purchased_goods().commodity_amounts[i];
					}
					return construction_status{ total > 0.0f ? purchased / total : 0.0f, true };
					break;
				case province_building_type::naval_base:
					for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
						total += state.economy_definitions.naval_base_definition.cost.commodity_amounts[i];
						purchased += pb_con.get_purchased_goods().commodity_amounts[i];
					}
					return construction_status{ total > 0.0f ? purchased / total : 0.0f, true };
					break;
			}
		}
	}
	return construction_status{ 0.0f, false };
}

construction_status factory_upgrade(sys::state& state, dcon::factory_id f) {
	auto in_prov = state.world.factory_get_province_from_factory_location(f);
	auto in_state = state.world.province_get_state_membership(in_prov);
	auto fac_type = state.world.factory_get_building_type(f);

	for(auto st_con : state.world.state_instance_get_state_building_construction(in_state)) {
		if(st_con.get_type() == fac_type) {
			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(fac_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i];
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			return construction_status{ total > 0.0f ? purchased / total : 0.0f, true };
		}
	}

	return construction_status{ 0.0f, false };
}

template<typename F>
void for_each_new_factory(sys::state& state, dcon::state_instance_id s, F&& func) {
	for(auto st_con : state.world.state_instance_get_state_building_construction(s)) {
		if(!st_con.get_is_upgrade()) {
			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(st_con.get_type());

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i];
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			func(new_factory{ total > 0.0f ? purchased / total : 0.0f, st_con.get_type().id });
		}
	}
}

template<typename F>
void for_each_upgraded_factory(sys::state& state, dcon::state_instance_id s, F&& func) {
	for(auto st_con : state.world.state_instance_get_state_building_construction(s)) {
		if(st_con.get_is_upgrade()) {
			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(st_con.get_type());

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i];
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			func(upgraded_factory{ total > 0.0f ? purchased / total : 0.0f, st_con.get_type().id });
		}
	}
}

bool state_contains_constructed_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft) {
	auto d = state.world.state_instance_get_definition(s);
	auto o = state.world.state_instance_get_nation_from_state_ownership(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			for(auto f : p.get_province().get_factory_location()) {
				if(f.get_factory().get_building_type() == ft)
					return true;
			}
		}
	}
	return false;
}

float unit_construction_progress(sys::state& state, dcon::province_land_construction_id c) {
	auto& goods = state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(c)].build_cost;
	auto& cgoods = state.world.province_land_construction_get_purchased_goods(c);

	float total = 0.0f;
	float purchased = 0.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		total += goods.commodity_amounts[i];
		purchased += cgoods.commodity_amounts[i];
	}

	return total > 0.0f ? purchased / total : 0.0f;
}

float unit_construction_progress(sys::state& state, dcon::province_naval_construction_id c) {
	auto& goods = state.military_definitions.unit_base_definitions[state.world.province_naval_construction_get_type(c)].build_cost;
	auto& cgoods = state.world.province_naval_construction_get_purchased_goods(c);

	float total = 0.0f;
	float purchased = 0.0f;

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		total += goods.commodity_amounts[i];
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
						f.get_factory().get_level() += 1;
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

	for(uint32_t i = state.world.province_land_construction_size(); i-->0; ) {
		auto c = fatten(state.world, dcon::province_land_construction_id{dcon::province_land_construction_id::value_base_t(i)});

		auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
		auto& current_purchased = c.get_purchased_goods();
		float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

		bool all_finished = true;
		for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
			if(base_cost.commodity_type[j]) {
				if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j]) {
					all_finished = false;
				}
			} else {
				break;
			}
		}

		if(all_finished) {
			auto pop_location = c.get_pop().get_province_from_pop_location();

			auto new_reg = military::create_new_regiment(state, c.get_nation(), c.get_type());
			auto a = [&]() {
				for(auto ar : state.world.province_get_army_location(pop_location)) {
					if(ar.get_army().get_controller_from_army_control() == c.get_nation())
						return ar.get_army().id;
				}
				auto new_army = fatten(state.world, state.world.create_army());
				new_army.set_controller_from_army_control(c.get_nation());
				new_army.set_location_from_army_location(pop_location);
				return new_army.id;
			}();
			state.world.try_create_army_membership(new_reg, a);
			state.world.try_create_regiment_source(new_reg, c.get_pop());
			
			state.world.delete_province_land_construction(c);
		}
	}

	province::for_each_land_province(state, [&](dcon::province_id p) {
		{
			auto rng = state.world.province_get_province_naval_construction(p);
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());
				auto& base_cost = state.military_definitions.unit_base_definitions[c.get_type()].build_cost;
				auto& current_purchased = c.get_purchased_goods();
				float construction_time = float(state.military_definitions.unit_base_definitions[c.get_type()].build_time);

				bool all_finished = true;
				for(uint32_t i = 0; i < commodity_set::set_size && all_finished; ++i) {
					if(base_cost.commodity_type[i]) {
						if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i]) {
							all_finished = false;
						}
					} else {
						break;
					}
				}

				if(all_finished) {
					auto new_ship = military::create_new_ship(state, c.get_nation(), c.get_type());
					auto a = [&]() {
						auto navies = state.world.province_get_navy_location(p);
						if(navies.begin() != navies.end()) {
							return navies.begin().operator*().get_navy().id;
						}
						auto new_navy = fatten(state.world, state.world.create_navy());
						new_navy.set_controller_from_navy_control(c.get_nation());
						new_navy.set_location_from_navy_location(p);
						return new_navy.id;
					}();
					state.world.try_create_navy_membership(new_ship, a);

					state.world.delete_province_naval_construction(c);
				}
			}
		}
	});


	for(uint32_t i = state.world.province_building_construction_size(); i-- > 0; ) {
		dcon::province_building_construction_id c{ dcon::province_building_construction_id::value_base_t(i) };
		auto for_province = state.world.province_building_construction_get_province(c);

		switch(province_building_type(state.world.province_building_construction_get_type(c))) {
			case province_building_type::railroad:
			{
				auto& base_cost = state.economy_definitions.railroad_definition.cost;
				auto& current_purchased = state.world.province_building_construction_get_purchased_goods(c);

				bool all_finished = true;
				for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
					if(base_cost.commodity_type[j]) {
						if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j]) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
				if(all_finished) {
					state.world.province_get_railroad_level(for_province) += 1;
					state.world.delete_province_building_construction(c);
				}
				break;
			}
			case province_building_type::fort:
			{
				auto& base_cost = state.economy_definitions.fort_definition.cost;
				auto& current_purchased = state.world.province_building_construction_get_purchased_goods(c);

				bool all_finished = true;
				for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
					if(base_cost.commodity_type[j]) {
						if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j]) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
				if(all_finished) {
					state.world.province_get_fort_level(for_province) += 1;
					state.world.delete_province_building_construction(c);
				}
				break;
			}
			case province_building_type::naval_base:
			{
				auto& base_cost = state.economy_definitions.naval_base_definition.cost;
				auto& current_purchased = state.world.province_building_construction_get_purchased_goods(c);

				bool all_finished = true;
				for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
					if(base_cost.commodity_type[j]) {
						if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j]) {
							all_finished = false;
						}
					} else {
						break;
					}
				}
				if(all_finished) {
					state.world.province_get_naval_base_level(for_province) += 1;
					state.world.delete_province_building_construction(c);
				}
				break;
			}
		}

	}

	for(uint32_t i = state.world.state_building_construction_size(); i-- > 0; ) {
		dcon::state_building_construction_id c{ dcon::state_building_construction_id::value_base_t(i) };
		auto n = state.world.state_building_construction_get_nation(c);
		auto type = state.world.state_building_construction_get_type(c);
		auto& base_cost = state.world.factory_type_get_construction_costs(type);
		auto& current_purchased = state.world.state_building_construction_get_purchased_goods(c);

		if(!state.world.state_building_construction_get_is_pop_project(c)) {
			float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

			bool all_finished = true;
			for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
				if(base_cost.commodity_type[j]) {
					if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * factory_mod) {
						all_finished = false;
					}
				} else {
					break;
				}
			}
			if(all_finished) {
				add_factory_level_to_state(state, state.world.state_building_construction_get_state(c), type, state.world.state_building_construction_get_is_upgrade(c));
				state.world.delete_state_building_construction(c);
			}
		} else {
			float factory_mod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f)
				* state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_owner_cost);

			bool all_finished = true;
			for(uint32_t j = 0; j < commodity_set::set_size && all_finished; ++j) {
				if(base_cost.commodity_type[j]) {
					if(current_purchased.commodity_amounts[j] < base_cost.commodity_amounts[j] * factory_mod) {
						all_finished = false;
					}
				} else {
					break;
				}
			}
			if(all_finished) {
				add_factory_level_to_state(state, state.world.state_building_construction_get_state(c), type, state.world.state_building_construction_get_is_upgrade(c));
				state.world.delete_state_building_construction(c);
			}
		}
	}
}

float estimate_daily_income(sys::state& state, dcon::nation_id n) {
	/* TODO -
	 * This should return what we think the income will be next day, and as a result wont account for any unprecedented actions
	 * return value is passed directly into text::fp_currency{} without adulteration.
	 */
	return 0;
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
		auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::min_military_spending));
		auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::max_military_spending));
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
}

}
