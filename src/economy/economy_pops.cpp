#include "economy_pops.hpp"
#include "economy_production.hpp"
#include "price.hpp"
#include "province_templates.hpp"
#include "demographics.hpp"

namespace economy {
namespace pops {
void update_consumption(
	sys::state& state,
	ve::vectorizable_buffer<float, dcon::nation_id>& invention_count,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_life,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_everyday,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_luxury,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_education_private,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_education_public,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_life,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_everyday,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_luxury,
	ve::vectorizable_buffer<float, dcon::pop_id>& subsistence_ratio
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

	ve::fp_vector total_spendings{};

	// temporary buffers for actual pop demand
	auto demand_education_public_forbidden = state.world.pop_make_vectorizable_float_buffer();
	auto demand_education_public_allowed = state.world.pop_make_vectorizable_float_buffer();

	auto to_bank = state.world.pop_make_vectorizable_float_buffer();
	auto to_investments = state.world.pop_make_vectorizable_float_buffer();

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto pop_size = state.world.pop_get_size(ids);

		// get all data into vectors
		vectorized_pops_budget<ve::fp_vector> data = prepare_pop_budget(state, ids);

		// "for free" in the context of life/everyday/luxury needs means subsistence/gifts
		// so we apply it directly there, it's not registered as demand
		// "for free" in the context of education means that someone paid for you
		// so it will be registered as demand

		buffer_life.set(ids, data.life_needs.satisfied_with_money_ratio);
		buffer_everyday.set(ids, data.everyday_needs.satisfied_with_money_ratio);
		buffer_luxury.set(ids, data.luxury_needs.satisfied_with_money_ratio);
		buffer_education_private.set(ids, data.education.satisfied_with_money_ratio);
		buffer_education_public.set(ids, data.education.satisfied_for_free_ratio);
		subsistence_ratio.set(ids, data.life_needs.satisfied_for_free_ratio);

		auto multiplier = pop_size / state.defines.alice_needs_scaling_factor;

		demand_life.set(ids, multiplier * data.life_needs.demand_scale * data.life_needs.satisfied_with_money_ratio);
		demand_everyday.set(ids, multiplier * data.everyday_needs.demand_scale * data.everyday_needs.satisfied_with_money_ratio);
		demand_luxury.set(ids, multiplier * data.luxury_needs.demand_scale * data.luxury_needs.satisfied_with_money_ratio);
		demand_education_public_forbidden.set(
			ids,
			pop_size
			* (1.f - data.can_use_free_services)
			* data.education.demand_scale * data.education.satisfied_with_money_ratio
		);
		demand_education_public_allowed.set(
			ids,
			pop_size
			* data.can_use_free_services
			* data.education.demand_scale * (data.education.satisfied_with_money_ratio + data.education.satisfied_for_free_ratio)
		);

		to_bank.set(ids, data.bank_savings.spent);
		to_investments.set(ids, data.investments.spent);

		// we do save savings here because a part of education is given for free
		// which leads to some part of wealth not being spent most of the time
		state.world.pop_set_savings(ids, ve::max(0.f, data.remaining_savings));
	});

	// services are at province level:

	concurrency::parallel_for(int32_t(0), state.province_definitions.first_sea_province.index(), [&](uint32_t raw_province_id) {
		auto pid = dcon::province_id{ dcon::province_id::value_base_t(raw_province_id) };
		state.world.province_for_each_pop_location(pid, [&](auto location) {
			auto pop = state.world.pop_location_get_pop(location);

			auto demand_allow_public = demand_education_public_allowed.get(pop);
			auto demand_forbid_public = demand_education_public_forbidden.get(pop);

			auto old_allow = state.world.province_get_service_demand_allowed_public_supply(pid, services::list::education);
			auto old_forbid = state.world.province_get_service_demand_forbidden_public_supply(pid, services::list::education);

			state.world.province_set_service_demand_allowed_public_supply(pid, services::list::education, old_allow + demand_allow_public);
			state.world.province_set_service_demand_forbidden_public_supply(pid, services::list::education, old_forbid + demand_forbid_public);
		});
	});

	// commodities (actually groups of commodities) are at market level

	concurrency::parallel_for(uint32_t(0), state.world.market_size(), [&](uint32_t raw_market_id) {
		auto m = dcon::market_id{ dcon::market_id::value_base_t(raw_market_id) };
		if(!state.world.market_is_valid(m)) return;
		auto zone = state.world.market_get_zone_from_local_market(m);
		province::for_each_province_in_state_instance(state, zone, [&](auto pid) {
			state.world.province_for_each_pop_location(pid, [&](auto location) {
				auto pop = state.world.pop_location_get_pop(location);
				auto pop_type = state.world.pop_get_poptype(pop);

				auto life = demand_life.get(pop);
				auto everyday = demand_everyday.get(pop);
				auto luxury = demand_luxury.get(pop);

				auto old_life = state.world.market_get_life_needs_scale(m, pop_type);
				auto old_everyday = state.world.market_get_everyday_needs_scale(m, pop_type);
				auto old_luxury = state.world.market_get_luxury_needs_scale(m, pop_type);

				state.world.market_set_life_needs_scale(m, pop_type, old_life + life);
				state.world.market_set_everyday_needs_scale(m, pop_type, old_everyday + everyday);
				state.world.market_set_luxury_needs_scale(m, pop_type, old_luxury + luxury);
			});
		});
	});

	// investments are at national level:

	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t raw_nation_id) {
		auto n = dcon::nation_id{ dcon::nation_id::value_base_t(raw_nation_id) };
		if(!state.world.nation_is_valid(n)) return;
		state.world.nation_for_each_province_ownership(n, [&](auto poid) {
			state.world.province_for_each_pop_location(state.world.province_ownership_get_province(poid), [&](auto location) {
				auto pop = state.world.pop_location_get_pop(location);

				auto investment = to_investments.get(pop);
				auto current_inv = state.world.nation_get_private_investment(n);
				state.world.nation_set_private_investment(n, current_inv + investment);

				if(n == state.local_player_nation) {
					state.ui_state.last_tick_investment_pool_change += investment;
				}

				auto banking = to_bank.get(pop);
				auto current_bank = state.world.nation_get_national_bank(n);
				state.world.nation_set_national_bank(n, current_bank + banking);
			});
		});
	});

	// iterate over all (market, pop type, trade good) triples to finalise this calculation and register demand to actual commodities
	state.world.execute_parallel_over_market([&](auto ids) {
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

				register_demand(state, ids, cid, demand_life);
				register_demand(state, ids, cid, demand_everyday);
				register_demand(state, ids, cid, demand_luxury);
			}
		}
	});
}

float estimate_artisan_income(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, float size) {
	auto const artisan_type = state.culture_definitions.artisans;
	auto key = demographics::to_key(state, artisan_type);

	if(ptid != artisan_type) {
		return 0.f;
	}

	auto artisan_profit = state.world.province_get_artisan_profit(pid);
	auto current_bank = state.world.province_get_artisan_bank(pid);
	auto total = artisan_profit + current_bank;
	auto dividents = total > 0.f ? total * 0.1f : 0.f;

	auto num_artisans = state.world.province_get_demographics(pid, key);
	auto per_artisan = num_artisans > 0.f ? dividents / num_artisans : 0.f;
	return size * per_artisan;
}

float estimate_artisan_income(sys::state const& state, dcon::pop_id pop) {
	return estimate_artisan_income(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}

void update_income_artisans(sys::state& state) {
	auto const artisan_type = state.culture_definitions.artisans;
	auto key = demographics::to_key(state, artisan_type);

	state.world.execute_parallel_over_province([&](auto pids) {
		auto artisan_profit = state.world.province_get_artisan_profit(pids);
		auto current_bank = state.world.province_get_artisan_bank(pids);
		state.world.province_set_artisan_bank(pids, current_bank + artisan_profit);
		state.world.province_set_artisan_profit(pids, 0.f);

		auto new_bank = state.world.province_get_artisan_bank(pids);
		auto dividents = ve::select(new_bank > 0.f, new_bank * 0.1f, 0.f);
		state.world.province_set_artisan_bank(pids, new_bank - dividents);

		auto num_artisans = state.world.province_get_demographics(pids, key);
		auto per_artisan = ve::select(num_artisans > 0.f, dividents / num_artisans, 0.f);

		ve::apply([&](auto province, auto payment) {
			for(auto pl : state.world.province_get_pop_location(province)) {
				if(artisan_type == pl.get_pop().get_poptype()) {
					pl.get_pop().set_savings(
						pl.get_pop().get_savings()
						+ state.inflation
						* pl.get_pop().get_size()
						* payment
					);
#ifndef NDEBUG
					assert(
						std::isfinite(pl.get_pop().get_savings())
						&& pl.get_pop().get_savings() >= 0
					);
#endif
				}
			}
		}, pids, per_artisan);
	});
}

float estimate_trade_income(sys::state const& state, dcon::market_id mid, dcon::pop_type_id ptid, float size) {
	auto const artisan_def = state.culture_definitions.artisans;
	auto artisan_key = demographics::to_key(state, artisan_def);
	auto const clerks_def = state.culture_definitions.secondary_factory_worker;
	auto clerks_key = demographics::to_key(state, clerks_def);
	auto const capis_def = state.culture_definitions.capitalists;
	auto capis_key = demographics::to_key(state, capis_def);
	auto sids = state.world.market_get_zone_from_local_market(mid);
	auto artisans = state.world.state_instance_get_demographics(sids, artisan_key);
	auto clerks = state.world.state_instance_get_demographics(sids, clerks_key);
	auto capis = state.world.state_instance_get_demographics(sids, capis_key);
	auto artisans_weight = state.world.state_instance_get_demographics(sids, artisan_key) / 1000.f;
	auto clerks_weight = state.world.state_instance_get_demographics(sids, clerks_key) * 100.f;
	auto capis_weight = state.world.state_instance_get_demographics(sids, capis_key) * 100'000.f;
	auto total_weight = artisans_weight + clerks_weight + capis_weight;

	if(total_weight == 0.f) {
		return 0.f;
	}

	auto balance = state.world.market_get_stockpile(mid, economy::money);
	auto trade_dividents = balance > 0.f ? balance * 0.001f : 0.f;

	auto artisans_share = artisans_weight / total_weight * trade_dividents;
	auto clerks_share = clerks_weight / total_weight * trade_dividents;
	auto capis_share = capis_weight / total_weight * trade_dividents;

	auto per_artisan = artisans > 0.f ? artisans_share / artisans : 0.f;
	auto per_clerk = clerks > 0.f ? clerks_share / clerks : 0.f;
	auto per_capi = capis > 0.f ? capis_share / capis : 0.f;

	if(artisan_def == ptid) {
		return state.inflation * size * per_artisan;
	} else if(clerks_def == ptid) {
		return state.inflation * size * per_clerk;
	} else if(capis_def == ptid) {
		return state.inflation * size * per_capi;
	}

	return 0.f;
}

float estimate_trade_income(sys::state const& state, dcon::pop_id pop) {
	auto provs = state.world.pop_get_province_from_pop_location(pop);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);

	return estimate_trade_income(
		state,
		markets,
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}

void update_income_trade(sys::state& state) {
	auto const artisan_def = state.culture_definitions.artisans;
	auto artisan_key = demographics::to_key(state, artisan_def);
	auto const clerks_def = state.culture_definitions.secondary_factory_worker;
	auto clerks_key = demographics::to_key(state, clerks_def);
	auto const capis_def = state.culture_definitions.capitalists;
	auto capis_key = demographics::to_key(state, capis_def);


	state.world.execute_parallel_over_market([&](auto markets) {
		/* {
			// we forgive a part of the debt to avoid monetary death spirals:
			auto current = state.world.market_get_stockpile(markets, economy::money);
			state.world.market_set_stockpile(markets, economy::money, current * 0.9f);
		}*/

		auto sids = state.world.market_get_zone_from_local_market(markets);

		auto artisans = state.world.state_instance_get_demographics(sids, artisan_key);
		auto clerks = state.world.state_instance_get_demographics(sids, clerks_key);
		auto capis = state.world.state_instance_get_demographics(sids, capis_key);

		auto artisans_weight = state.world.state_instance_get_demographics(sids, artisan_key) / 1000.f;
		auto clerks_weight = state.world.state_instance_get_demographics(sids, clerks_key) * 100.f;
		auto capis_weight = state.world.state_instance_get_demographics(sids, capis_key) * 100'000.f;

		auto total_weight = artisans_weight + clerks_weight + capis_weight;

		auto balance = state.world.market_get_stockpile(markets, economy::money);
		auto trade_dividents = ve::select(balance > 0.f, balance * 0.001f, ve::fp_vector{ 0.f });
		state.world.market_set_stockpile(markets, economy::money, balance - trade_dividents);

		auto artisans_share = artisans_weight / total_weight * trade_dividents;
		auto clerks_share = clerks_weight / total_weight * trade_dividents;
		auto capis_share = capis_weight / total_weight * trade_dividents;

		auto per_artisan = ve::select(artisans > 0.f, artisans_share / artisans, ve::fp_vector{ 0.f });
		auto per_clerk = ve::select(clerks > 0.f, clerks_share / clerks, ve::fp_vector{ 0.f });
		auto per_capi = ve::select(capis > 0.f, capis_share / capis, ve::fp_vector{ 0.f });

		// proceeed payments:

		ve::apply([&](auto sid, auto to_artisan, auto to_clerk, auto to_capi) {
			if(to_artisan == 0.f && to_clerk == 0.f && to_capi == 0.f) {
				return;
			}
			province::for_each_province_in_state_instance(state, sid, [&](dcon::province_id p) {
				for(auto pl : state.world.province_get_pop_location(p)) {
					auto pop = pl.get_pop();
					auto savings = pop.get_savings();
					auto size = pop.get_size();

					if(artisan_def == pl.get_pop().get_poptype()) {
						pop.set_savings(savings + state.inflation * size * to_artisan);
#ifndef NDEBUG
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
					} else if(clerks_def == pl.get_pop().get_poptype()) {
						pop.set_savings(savings + state.inflation * size * to_clerk);
#ifndef NDEBUG
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
					} else if(capis_def == pl.get_pop().get_poptype()) {
						pop.set_savings(savings + state.inflation * size * to_capi);
#ifndef NDEBUG
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
					}
					assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
				}
			});
		}, sids, per_artisan, per_clerk, per_capi);
	});
}


money_from_nation estimate_income_from_nation(sys::state const& state, dcon::pop_id pop) {
	auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
	auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);

	auto prov = state.world.pop_get_province_from_pop_location(pop);
	auto owner = state.world.province_get_nation_from_province_ownership(prov);
	auto population = state.world.nation_get_demographics(owner, demographics::total);
	auto unemployed = population - state.world.nation_get_demographics(owner, demographics::employed);
	auto capitalists = state.world.nation_get_demographics(owner, capitalists_key);
	auto aristocrats = state.world.nation_get_demographics(owner, aristocracy_key);
	auto investors = capitalists + aristocrats;

	auto states = state.world.province_get_state_membership(prov);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto owner_spending = state.world.nation_get_spending_level(owner);

	auto size = state.world.pop_get_size(pop);
	auto adj_size = size / state.defines.alice_needs_scaling_factor;

	auto budget = state.world.nation_get_last_base_budget(owner);

	auto social_budget =
		owner_spending
		* budget
		* float(state.world.nation_get_social_spending(owner))
		/ 100.f;

	auto investment_budget =
		owner_spending
		* budget
		* float(state.world.nation_get_domestic_investment_spending(owner))
		/ 100.f;

	auto const p_level = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pension_level);
	auto const unemp_level = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::unemployment_benefit);

	auto pension_ratio = p_level * population > 0.f ? p_level * population / (p_level * population + unemp_level * unemployed) : 0.f;
	auto unemployment_ratio = unemp_level * unemployed > 0.f ? unemp_level * unemployed / (p_level * population + unemp_level * unemployed) : 0.f;

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

	auto const m_spending = owner_spending * float(state.world.nation_get_military_spending(owner)) / 100.0f;

	auto types = state.world.pop_get_poptype(pop);

	auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
	auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
	auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

	auto ln_costs = state.world.market_get_life_needs_costs(markets, types);
	auto en_costs = state.world.market_get_everyday_needs_costs(markets, types);
	auto lx_costs = state.world.market_get_luxury_needs_costs(markets, types);

	auto total_costs = ln_costs + en_costs + lx_costs;

	auto is_military_requires_life_needs = ln_types == int32_t(culture::income_type::military);
	auto is_military_requires_everyday_needs = en_types == int32_t(culture::income_type::military);
	auto is_military_requires_luxury_needs = lx_types == int32_t(culture::income_type::military);
	auto is_military = is_military_requires_life_needs || is_military_requires_everyday_needs || is_military_requires_luxury_needs;
	auto is_investor = (types == state.culture_definitions.capitalists) || (types == state.culture_definitions.aristocrat);

	auto mil_pay = 0.f;
	mil_pay += is_military_requires_life_needs ? m_spending * adj_size * ln_costs * payouts_spending_multiplier : 0.0f;
	mil_pay += is_military_requires_everyday_needs ? m_spending * adj_size * en_costs * payouts_spending_multiplier : 0.0f;
	mil_pay += is_military_requires_luxury_needs ? m_spending * adj_size * lx_costs * payouts_spending_multiplier : 0.0f;

	return {
		.pension = pension_per_person * size,
		.unemployment = is_military ? 0.f : benefits_per_person * (size - pop_demographics::get_employment(state, pop)),
		.military = mil_pay,
		.investment = size * price_properties::labor::min * 0.05f + (is_investor ? payment_per_investor * size : 0.f)
	};
}

void update_income_national_subsidy(sys::state& state){
	auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
	auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto provs = state.world.pop_get_province_from_pop_location(ids);
		auto owners = state.world.province_get_nation_from_province_ownership(provs);
		auto population = state.world.nation_get_demographics(owners, demographics::total);
		auto unemployed = population - state.world.nation_get_demographics(owners, demographics::employed);
		auto capitalists = state.world.nation_get_demographics(owners, capitalists_key);
		auto aristocrats = state.world.nation_get_demographics(owners, aristocracy_key);
		auto investors = capitalists + aristocrats;

		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto owner_spending = state.world.nation_get_spending_level(owners);

		auto pop_savings = state.world.pop_get_savings(ids);

		auto pop_of_type = state.world.pop_get_size(ids);
		auto adj_pop_of_type = pop_of_type / state.defines.alice_needs_scaling_factor;

		auto budget = state.world.nation_get_last_base_budget(owners);

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

		auto pension_ratio = ve::select(p_level * population > 0.f, p_level * population / (p_level * population + unemp_level * unemployed), 0.f);
		auto unemployment_ratio = ve::select(unemp_level * unemployed > 0.f, unemp_level * unemployed / (p_level * population + unemp_level * unemployed), 0.f);

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

		auto const m_spending = owner_spending * ve::to_float(state.world.nation_get_military_spending(owners)) / 100.0f;

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

		auto acc_m = ve::select(ln_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * ln_costs * payouts_spending_multiplier, 0.0f);


		auto acc_u = pension_per_person * pop_of_type;

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

		acc_m = acc_m + ve::select(lx_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * lx_costs * payouts_spending_multiplier, 0.0f);

		auto not_military = !((ln_types == int32_t(culture::income_type::military)) & (en_types == int32_t(culture::income_type::military)) & (lx_types == int32_t(culture::income_type::military)));
		auto employment = pop_demographics::get_employment(state, ids);
		acc_u = acc_u + ve::select(
			not_military,
			benefits_per_person
			* (pop_of_type - employment),
			0.0f
		);

		ve::fp_vector base_income = pop_of_type * price_properties::labor::min * 0.05f;

		state.world.pop_set_savings(ids, state.inflation * ((base_income + state.world.pop_get_savings(ids)) + (acc_u + acc_m)));
#ifndef NDEBUG
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_m);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_u);
#endif
	});
}

std::vector<labor_ratio_wage> estimate_wage(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, bool accepted, float size) {
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

	if(state.world.pop_type_get_is_paid_rgo_worker(ptid)) {
		return { {labor::no_education, 1.f, size * no_education_wage } };
	} else if(state.culture_definitions.primary_factory_worker == ptid) {
		auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_no_education);
		auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_basic_education);
		return {
			{labor::no_education, no_education, no_education * size * no_education_wage },
			{labor::basic_education, basic_education, basic_education * size * basic_education_wage }
		};
	} else if(state.culture_definitions.secondary_factory_worker == ptid || state.culture_definitions.bureaucrat == ptid || state.culture_definitions.clergy == ptid) {
		if(accepted) {
			auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_no_education);
			auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_basic_education);
			auto high_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education);
			auto high_education_accepted = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education_accepted);
			return {
				{labor::no_education, no_education, no_education * size * no_education_wage },
				{labor::basic_education, basic_education, basic_education * size * basic_education_wage },
				{labor::high_education, high_education, high_education * size * high_education_wage },
				{labor::high_education_and_accepted, high_education_accepted, high_education_accepted * size * high_education_and_accepted_wage }
			};
		} else {
			auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_no_education);
			auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_basic_education);
			auto high_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_high_education);
			return {
				{labor::no_education, no_education, no_education * size * no_education_wage },
				{labor::basic_education, basic_education, basic_education * size * basic_education_wage },
				{labor::high_education, high_education, high_education * size * high_education_wage },
			};
		}
	}
	return {};
}

std::vector<labor_ratio_wage> estimate_wage(sys::state const& state, dcon::pop_id pop) {
	return estimate_wage(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_is_primary_or_accepted_culture(pop),
		state.world.pop_get_size(pop)
	);
}

float estimate_total_wage(sys::state const& state, dcon::pop_id pop) {
	float total = 0.f;
	auto list = estimate_wage(state, pop);
	for(auto& item : list) {
		total += item.wage;
	}
	return total;
}


float estimate_rgo_income(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, float size) {
	float no_education_wage =
		state.world.province_get_labor_price(pid, labor::no_education)
		* state.world.province_get_labor_supply_sold(pid, labor::no_education);
	float rgo_workers_wage =
		state.world.province_get_pop_labor_distribution(pid, pop_labor::rgo_worker_no_education)
		* no_education_wage;
	auto total_rgo_profit = state.world.province_get_rgo_profit(pid);
	for(auto pl : state.world.province_get_pop_location(pid)) {
		if(pl.get_pop().get_poptype() == state.culture_definitions.slaves) {
			total_rgo_profit += pl.get_pop().get_size() * rgo_workers_wage;
		}
	}
	float aristocrats_share = state.world.province_get_landowners_share(pid);
	float num_aristocrat = state.world.province_get_demographics(
		pid,
		demographics::to_key(state, state.culture_definitions.aristocrat)
	);
	if(total_rgo_profit >= 0.f && num_aristocrat > 0.f && state.culture_definitions.aristocrat == ptid) {
		return size * total_rgo_profit * aristocrats_share / num_aristocrat;
	} else {
		return 0.f;
	}
}

float estimate_rgo_income(sys::state const& state, dcon::pop_id pop) {
	return estimate_rgo_income(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}


float market_cut(sys::state const& state, dcon::market_id market, float no_education_wage) {
	auto modified = local_market_cut_baseline - state.world.market_get_stockpile(market, economy::money) / (no_education_wage + 0.000001f) / 100'000.f;
	return std::clamp(modified, 0.f, 0.1f);
}

constexpr inline float market_tax = 0.05f;

void update_income_wages(sys::state& state){
	// TODO: rewrite in vectorized way

	for(auto n : state.world.in_nation) {
		for(auto poid : state.world.nation_get_province_ownership(n)) {
			float total_factory_profit = 0.f;
			float total_rgo_profit = 0.f;

			float market_profit = 0.f;

			auto pid = poid.get_province();
			auto sid = pid.get_state_membership();
			auto market = sid.get_market_from_local_market();

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

			float sum_of_wages = no_education_wage + basic_education_wage + high_education_wage + high_education_and_accepted_wage;

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

			auto local_market_cut = market_cut(state, market, no_education_wage);

			auto market_rgo_activity_cut = total_rgo_profit * local_market_cut;
			total_rgo_profit -= market_rgo_activity_cut;

			if(total_rgo_profit >= 0.f && num_aristocrat > 0.f) {
				payment_per_aristocrat += total_rgo_profit * aristocrats_share / num_aristocrat;
				market_profit += total_rgo_profit * others_share;
			} else {
				market_profit += total_rgo_profit;
			}
			auto& cur_money = state.world.market_get_stockpile(market, economy::money);
			state.world.market_set_stockpile(market, economy::money, cur_money + market_profit + market_rgo_activity_cut);

			auto market_cut_from_wages = 0.f;

			for(auto pl : state.world.province_get_pop_location(pid)) {
				auto pop = pl.get_pop();
				auto savings = pop.get_savings();
				auto poptype = pop.get_poptype();
				auto size = pop.get_size();

				auto accepted = state.world.nation_get_accepted_cultures(n, pop.get_culture())
					|| state.world.nation_get_primary_culture(n) == pop.get_culture();

				if(poptype.get_is_paid_rgo_worker()) {
					pop.set_savings(pop.get_savings() + size * rgo_workers_wage);
				} else if(state.culture_definitions.primary_factory_worker == poptype) {
					pop.set_savings(pop.get_savings() + size * primary_workers_wage);
				} else if(state.culture_definitions.secondary_factory_worker == pop.get_poptype()) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + size * high_accepted_workers_wage);
					} else {
						pop.set_savings(pop.get_savings() + size * high_not_accepted_workers_wage);
					}
					assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
				} else if(pop.get_poptype() == state.culture_definitions.bureaucrat) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + size * high_accepted_workers_wage);
					} else {
						pop.set_savings(pop.get_savings() + size * high_not_accepted_workers_wage);
					}
				} else if(pop.get_poptype() == state.culture_definitions.clergy) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + size * high_accepted_workers_wage);
					} else {
						pop.set_savings(pop.get_savings() + size * high_not_accepted_workers_wage);
					}
				} else if(state.culture_definitions.aristocrat == pop.get_poptype()) {
					pop.set_savings(pop.get_savings() + size * payment_per_aristocrat);
				}

				// pops pay a "tax" to market to import expensive goods:
				{
					float new_savings = pop.get_savings();
					pop.set_savings(state.inflation * new_savings * (1.f - market_tax));
					market_cut_from_wages += new_savings * market_tax;
				}

				assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
			}

			state.world.market_set_stockpile(market, economy::money, state.world.market_get_stockpile(market, economy::money) + market_cut_from_wages);
		}
	}
}

float estimate_next_day_budget_before_taxes(
	sys::state const& state,
	dcon::pop_id pop
) {
	auto current = state.world.pop_get_savings(pop);
	current -= prepare_pop_budget(state, pop).spent_total;

	auto estimated = current
		+ estimate_artisan_income(state, pop)
		+ estimate_rgo_income(state, pop)
		+ estimate_trade_income(state, pop)
		+ estimate_total_wage(state, pop);

	auto from_nation = estimate_income_from_nation(state, pop);

	estimated +=
		from_nation.investment
		+ from_nation.military
		+ from_nation.pension
		+ from_nation.unemployment;

	return estimated;
}

float estimate_trade_spending(
	sys::state const& state,
	dcon::pop_id pop
) {
	auto next_day = estimate_next_day_budget_before_taxes(state, pop);
	return market_tax * next_day;
}

float estimate_tax_spending(
	sys::state const& state,
	dcon::pop_id pop,
	float tax_rate
) {
	auto next_day = estimate_next_day_budget_before_taxes(state, pop);
	return next_day * (1.f - market_tax) * tax_rate;
}

float estimate_pop_demand_internal_life(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.life_needs.demand_scale
		* budget.life_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_lf_needs_scale
		* state.world.pop_type_get_life_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
}
float estimate_pop_demand_internal_everyday(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.everyday_needs.demand_scale
		* budget.everyday_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_ev_needs_scale
		* state.world.pop_type_get_everyday_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor
		* invention_factor;
}
float estimate_pop_demand_internal_luxury(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.luxury_needs.demand_scale
		* budget.luxury_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_lx_needs_scale
		* state.world.pop_type_get_luxury_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor
		* invention_factor;
}

float estimate_pop_spending_life(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_life_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_life_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_life(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, cid);
	return demand * actually_bought * cost;
}

float estimate_pop_spending_everyday(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_everyday_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_everyday(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, cid);
	return demand * actually_bought * cost;
}

float estimate_pop_spending_luxury(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_luxury_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_luxury_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_luxury(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, cid);
	return demand * actually_bought * cost;
}

}

float estimate_pops_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	auto zone = state.world.province_get_state_membership(p);
	auto market = state.world.state_instance_get_market_from_local_market(zone);

	auto satisfaction = state.world.market_get_actual_probability_to_buy(market, c);

	auto nation = state.world.province_get_nation_from_province_ownership(p);

	auto weight_life = state.world.market_get_life_needs_weights(market, c);
	auto weight_everyday = state.world.market_get_everyday_needs_weights(market, c);
	auto weight_luxury = state.world.market_get_luxury_needs_weights(market, c);

	float life_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_life_needs) + 1.0f
	};
	float everyday_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
	};
	float luxury_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
	};

	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;

	float total = 0.f;
	state.world.province_for_each_pop_location(p, [&](auto location) {
		dcon::pop_id pop = state.world.pop_location_get_pop(location);

		auto pop_type = state.world.pop_get_poptype(pop);
		auto strata = state.world.pop_type_get_strata(pop_type);

		pops::vectorized_pops_budget<float> budget = pops::prepare_pop_budget(state, pop);

		auto consumption_life = pops::estimate_pop_demand_internal_life(
			state, c, pop, budget, life_mul, weight_life, invention_factor
		);
		auto consumption_everyday = pops::estimate_pop_demand_internal_everyday(
			state, c, pop, budget, everyday_mul, weight_everyday, invention_factor
		);
		auto consumption_luxury = pops::estimate_pop_demand_internal_luxury(
			state, c, pop, budget, luxury_mul, weight_luxury, invention_factor
		);

		total += consumption_life + consumption_everyday + consumption_luxury;
	});

	return total * satisfaction;
}
}
