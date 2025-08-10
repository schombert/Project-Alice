#include "economy_pops.hpp"
#include "economy_production.hpp"
#include "price.hpp"
#include "nations.hpp"
#include "province_templates.hpp"

namespace economy {
namespace pops {
void update_consumption(
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

	ve::fp_vector total_spendings{};

	state.world.execute_serial_over_pop([&](auto ids) {
		auto pop_size = state.world.pop_get_size(ids);

		// get all data into vectors
		vectorized_pops_budget<ve::fp_vector> data = prepare_pop_budget(state, ids);

		auto base_shift = ve::fp_vector{ 1.f / 200.f };
		auto small_shift = pop_demographics::pop_u16_scaling;

		ve::fp_vector shift_life = ve::select(
			data.life_needs.satisfied_with_money_ratio + data.life_needs.satisfied_for_free_ratio > 0.9f,
			base_shift,
			ve::select(
				data.life_needs.satisfied_with_money_ratio + data.life_needs.satisfied_for_free_ratio < 0.7f,
				-base_shift,
				ve::fp_vector{ 0.f }
			)
		);
		ve::fp_vector shift_everyday = ve::select(
			data.everyday_needs.satisfied_with_money_ratio > 0.9f,
			base_shift,
			ve::select(
				data.everyday_needs.satisfied_with_money_ratio < 0.7f,
				-base_shift,
				ve::fp_vector{ 0.f }
			)
		);
		ve::fp_vector shift_luxury = ve::select(
			data.luxury_needs.satisfied_with_money_ratio > 0.9f,
			base_shift,
			ve::select(
				data.luxury_needs.satisfied_with_money_ratio < 0.7f,
				-base_shift,
				ve::fp_vector{ 0.f }
			)
		);
		ve::fp_vector shift_literacy = ve::select(
			data.education.satisfied_with_money_ratio + data.education.satisfied_for_free_ratio > 0.9f,
			small_shift,
			ve::select(
				data.education.satisfied_with_money_ratio + data.education.satisfied_for_free_ratio < 0.7f,
				-small_shift,
				ve::fp_vector{ 0.f }
			)
		);

		// clamping for now
		auto result_life = ve::max(0.f, ve::min(1.f, pop_demographics::get_life_needs(state, ids) + shift_life));
		auto result_everyday = ve::max(0.f, ve::min(1.f, pop_demographics::get_everyday_needs(state, ids) + shift_everyday));
		auto result_luxury = ve::max(0.f, ve::min(1.f, pop_demographics::get_luxury_needs(state, ids) + shift_luxury));
		auto result_literacy = ve::max(0.f, ve::min(1.f, pop_demographics::get_literacy(state, ids) + shift_literacy));

		pop_demographics::set_life_needs(state, ids, result_life);
		pop_demographics::set_everyday_needs(state, ids, result_everyday);
		pop_demographics::set_luxury_needs(state, ids, result_luxury);
		pop_demographics::set_literacy(state, ids, result_literacy);

		auto final_demand_scale_life =
			pop_size / state.defines.alice_needs_scaling_factor
			* data.life_needs.demand_scale
			* data.life_needs.satisfied_with_money_ratio;

		auto final_demand_scale_everyday =
			pop_size / state.defines.alice_needs_scaling_factor
			* data.everyday_needs.demand_scale
			* data.everyday_needs.satisfied_with_money_ratio;

		auto final_demand_scale_luxury =
			pop_size / state.defines.alice_needs_scaling_factor
			* data.luxury_needs.demand_scale
			* data.luxury_needs.satisfied_with_money_ratio;

		auto final_demand_scale_education_free_allowed =
			data.can_use_free_services
			* data.education.demand_scale
			* pop_size;
		auto final_demand_scale_education_free_not_allowed =
			(1.f - data.can_use_free_services)
			* data.education.demand_scale
			* pop_size;

		auto provs = state.world.pop_get_province_from_pop_location(ids);

		ve::apply([&](
			dcon::province_id pid,
			float potentially_free,
			float paid_only
			) {
				if(bool(pid)) {
					{
						auto old_value = state.world.province_get_service_demand_allowed_public_supply(pid, services::list::education);
						state.world.province_set_service_demand_allowed_public_supply(pid, services::list::education, old_value + potentially_free);
					}
					{
						auto old_value = state.world.province_get_service_demand_forbidden_public_supply(pid, services::list::education);
						state.world.province_set_service_demand_forbidden_public_supply(pid, services::list::education, old_value + paid_only);
					}
				}
				
		},
			provs, final_demand_scale_education_free_allowed, final_demand_scale_education_free_not_allowed
		);

		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);

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
				if(state.world.market_is_valid(m)) {
					state.world.market_set_life_needs_scale(m, pop_type, state.world.market_get_life_needs_scale(m, pop_type) + scale_life);
					state.world.market_set_everyday_needs_scale(m, pop_type, state.world.market_get_everyday_needs_scale(m, pop_type) + scale_everyday);
					state.world.market_set_luxury_needs_scale(m, pop_type, state.world.market_get_luxury_needs_scale(m, pop_type) + scale_luxury);
					auto zone = state.world.market_get_zone_from_local_market(m);
					auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
					state.world.nation_set_private_investment(nation, state.world.nation_get_private_investment(nation) + investment);
					if(nation == state.local_player_nation) {
						state.ui_state.last_tick_investment_pool_change += investment;
					}
				}
		},
			markets,
			final_demand_scale_life,
			final_demand_scale_everyday,
			final_demand_scale_luxury,
			data.investments.spent,
			state.world.pop_get_poptype(ids)
		);

		auto bank_deposits = data.bank_savings.spent;
		bank_deposits = ve::max(bank_deposits, 0.0f);
#ifndef NDEBUG
		ve::apply([&](float r) {
			assert(r >= 0.f);
		}, bank_deposits);
#endif
		ve::apply(
			[&](float transfer, dcon::nation_id n) {
				if(state.world.nation_is_valid(n)) {
					state.world.nation_set_national_bank(n, state.world.nation_get_national_bank(n) + transfer);
				}
				return 0;
			}, bank_deposits, nations
		);

		// we do save savings here because a part of education is given for free
		// which leads to some part of wealth not being spent most of the time
		state.world.pop_set_savings(ids, data.remaining_savings);
		total_spendings = total_spendings + data.spent_total;
	});

#ifndef NDEBUG
	std::string debug_output = "Total pops spendings on needs: " + std::to_string(total_spendings.reduce());
	state.console_log(debug_output);
#endif // !NDEBUG

	// iterate over all (market,pop type,trade good) triples to finalise this calculation
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

void update_income_trade(sys::state& state) {
	auto const artisan_def = state.culture_definitions.artisans;
	auto artisan_key = demographics::to_key(state, artisan_def);
	auto const clerks_def = state.culture_definitions.secondary_factory_worker;
	auto clerks_key = demographics::to_key(state, clerks_def);
	auto const capis_def = state.culture_definitions.capitalists;
	auto capis_key = demographics::to_key(state, capis_def);


	state.world.execute_parallel_over_market([&](auto markets) {
		{
			// we forgive a part of the debt to avoid monetary death spirals:
			auto current = state.world.market_get_stockpile(markets, economy::money);
			state.world.market_set_stockpile(markets, economy::money, current * 0.9f);
		}

		auto sids = state.world.market_get_zone_from_local_market(markets);

		auto artisans = state.world.state_instance_get_demographics(sids, artisan_key);
		auto clerks = state.world.state_instance_get_demographics(sids, clerks_key);
		auto capis = state.world.state_instance_get_demographics(sids, capis_key);

		auto artisans_weight = state.world.state_instance_get_demographics(sids, artisan_key) / 1000.f;
		auto clerks_weight = state.world.state_instance_get_demographics(sids, clerks_key) * 100.f;
		auto capis_weight = state.world.state_instance_get_demographics(sids, capis_key) * 100'000.f;

		auto total_weight = artisans_weight + clerks_weight + capis_weight;

		auto balance = state.world.market_get_stockpile(markets, economy::money);
		auto trade_dividents = ve::select(balance > 0.f, balance * 0.3f, ve::fp_vector{ 0.f });
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

		auto acc_m = ve::select(ln_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * ln_costs * payouts_spending_multiplier, 0.0f);

		auto none_of_above = ln_types != int32_t(culture::income_type::military);

		auto acc_u = ve::select(
			none_of_above,
			pension_per_person
			* pop_of_type,
			0.0f
		);

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

		auto employment = pop_demographics::get_employment(state, ids);

		acc_u = acc_u + ve::select(
			none_of_above
			&& state.world.pop_type_get_has_unemployment(types),
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

			auto local_market_cut = local_market_cut_baseline - state.world.market_get_stockpile(market, economy::money) / (no_education_wage + 0.000001f) / 100'000'000.f;
			local_market_cut = std::clamp(local_market_cut, 0.f, 1.f);

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
			auto total_wage = 0.f;

			for(auto pl : state.world.province_get_pop_location(pid)) {
				auto pop = pl.get_pop();
				auto savings = pop.get_savings();
				auto poptype = pop.get_poptype();
				auto size = pop.get_size();

				auto accepted = state.world.nation_get_accepted_cultures(n, pop.get_culture())
					|| state.world.nation_get_primary_culture(n) == pop.get_culture();

				if(poptype.get_is_paid_rgo_worker()) {
					pop.set_savings(pop.get_savings() + state.inflation * size * rgo_workers_wage * (1.f - local_market_cut));
					total_wage += size * rgo_workers_wage;
				} else if(state.culture_definitions.primary_factory_worker == poptype) {
					pop.set_savings(pop.get_savings() + state.inflation * size * primary_workers_wage * (1.f - local_market_cut));
					total_wage += size * primary_workers_wage;
				} else if(state.culture_definitions.secondary_factory_worker == pop.get_poptype()) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + state.inflation * size * high_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_accepted_workers_wage;
					} else {
						pop.set_savings(pop.get_savings() + state.inflation * size * high_not_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_not_accepted_workers_wage;
					}
					assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
				} else if(pop.get_poptype() == state.culture_definitions.bureaucrat) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + state.inflation * size * high_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_accepted_workers_wage;
					} else {
						pop.set_savings(pop.get_savings() + state.inflation * size * high_not_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_not_accepted_workers_wage;
					}
				} else if(pop.get_poptype() == state.culture_definitions.clergy) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + state.inflation * size * high_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_accepted_workers_wage;
					} else {
						pop.set_savings(pop.get_savings() + state.inflation * size * high_not_accepted_workers_wage * (1.f - local_market_cut));
						total_wage += size * high_not_accepted_workers_wage;
					}
				} else if(state.culture_definitions.aristocrat == pop.get_poptype()) {
					pop.set_savings(pop.get_savings() + state.inflation * size * payment_per_aristocrat);
				}

				// all rich pops pay a "tax" to market to import expensive goods:

				if(pop.get_savings() / (size + 1) > 10.f * sum_of_wages) {
					float new_savings = pop.get_savings();
					market_cut_from_wages += new_savings * 0.5f;
					pop.set_savings(new_savings * 0.5f);
				}

				assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
			}

			market_cut_from_wages += total_wage * local_market_cut;
			state.world.market_set_stockpile(market, economy::money, state.world.market_get_stockpile(market, economy::money) + market_cut_from_wages);
		}
	}
}

}
}
