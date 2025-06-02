#include "ai_economy.hpp"
#include "ai_campaign_values.hpp"
#include "economy_stats.hpp"
#include "economy_production.hpp"
#include "economy_government.hpp"
#include "construction.hpp"
#include "demographics.hpp"
#include "prng.hpp"
#include "math_fns.hpp"

namespace ai {

void update_factory_types_priority(sys::state& state) {
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t id) {
		dcon::nation_id n{ dcon::nation_id::value_base_t(id) };

		if(state.world.nation_get_owned_province_count(n) == 0) {
			// skip -- do not need experience
			return;
		}
		// private research

		std::vector<float> employment;
		employment.resize(state.world.factory_type_size());

		state.world.nation_for_each_province_ownership_as_nation(n, [&](dcon::province_ownership_id province_ownership) {
			auto province = state.world.province_ownership_get_province(province_ownership);
			auto sid = state.world.province_get_state_membership(province);
			auto market = state.world.state_instance_get_market_from_local_market(sid);
			state.world.province_for_each_factory_location_as_province(province, [&](auto location) {
				auto factory = state.world.factory_location_get_factory(location);
				auto type = state.world.factory_get_building_type(factory);
				employment[type.id.index()] +=
					state.world.factory_get_primary_employment(factory)
					* economy::factory_type_input_cost(
						state, n, market, type
					);
			});
		});

		state.world.for_each_factory_type([&](dcon::factory_type_id factory_type_id) {

			if(
				!state.world.nation_get_active_building(n, factory_type_id)
				&& !state.world.factory_type_get_is_available_from_start(factory_type_id)
			) {
				state.world.nation_set_factory_type_experience_priority_private(n, factory_type_id, 0.f);
				return;
			}

			// capitalists want to minimise costs related to current factories
			// so they choose factory types
			// according to:
			// 1) current amount of workers in factories of a given type
			// 2) costs of inputs

			auto costs = employment[factory_type_id.index()];
			state.world.nation_set_factory_type_experience_priority_private(n, factory_type_id, costs);
		});

		if(state.world.nation_get_is_player_controlled(n)) {
			// skip -- do not need AI
			return;
		}
		// national research

		std::vector<float> supply;
		supply.resize(state.world.commodity_size());

		state.world.for_each_commodity([&](dcon::commodity_id cid) {
			supply[cid.index()] = economy::supply(state, n, cid);
		});

		state.world.for_each_factory_type([&](dcon::factory_type_id factory_type_id) {
			if(
				!state.world.nation_get_active_building(n, factory_type_id)
				&& !state.world.factory_type_get_is_available_from_start(factory_type_id)
			) {
				state.world.nation_set_factory_type_experience_priority_private(n, factory_type_id, 0.f);
				return;
			}

			// auto priority = 0.5f + float(rng::get_random(state, uint32_t(id) ^ uint32_t(factory_type_id)) & 0xFFFF) / float(0xFFFF);

			auto& inputs = state.world.factory_type_get_inputs(factory_type_id);

			auto min_effective_supply = std::numeric_limits<float>::infinity();
			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
				auto input = inputs.commodity_type[i];
				if(input) {
					min_effective_supply = std::min(supply[input.index()] / inputs.commodity_amounts[i], min_effective_supply);
				} else {
					break;
				}
			}

			//check if there are "rivals" which would push you away from the industry
			auto local_price = economy::price(state, n, state.world.factory_type_get_output(factory_type_id));
			auto rival_price = local_price * 2.f;
			for(auto adj : state.world.nation_get_nation_adjacency(n)) {
				auto other = adj.get_connected_nations(0) != n ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
				rival_price = std::min(rival_price, economy::price(state, other, state.world.factory_type_get_output(factory_type_id)));
			}

			auto rival_modifier = (rival_price + 0.01f) / (local_price + 0.01f);

			// we want this modifier to be really strong
			rival_modifier = (rival_modifier * rival_modifier) * (rival_modifier * rival_modifier) * rival_modifier;

			state.world.nation_set_factory_type_experience_priority_national(n, factory_type_id, min_effective_supply * rival_modifier);
		});
	});
}


void get_craved_factory_types(sys::state& state, dcon::nation_id nid, dcon::market_id mid, dcon::province_id pid, std::vector<dcon::factory_type_id>& desired_types, bool pop_project) {
	assert(desired_types.empty());
	auto n = dcon::fatten(state.world, nid);
	auto m = dcon::fatten(state.world, mid);
	auto sid = m.get_zone_from_local_market();

	auto const tax_eff = economy::tax_collection_rate(state, nid, pid);
	auto const rich_effect = (1.0f - tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f);
	auto wage = state.world.province_get_labor_price(pid, economy::labor::basic_education) * 2.f;

	if(desired_types.empty()) {
		for(auto type : state.world.in_factory_type) {
			if(n.get_active_building(type) || type.get_is_available_from_start()) {
				float cost = economy::factory_type_build_cost(state, n, pid, type, pop_project);
				float output = economy::factory_type_output_cost(state, n, m, type);
				float input = economy::factory_type_input_cost(state, n, m, type);

				auto profit = (output - input - wage * type.get_base_workforce()) * (1.0f - rich_effect);
				auto roi = profit / cost;

				if(profit / input > 10.f && roi > 0.01f)
					desired_types.push_back(type.id);
			} // END if building unlocked
		}
	}
}

void get_desired_factory_types(sys::state& state, dcon::nation_id nid, dcon::market_id mid, dcon::province_id pid, std::vector<dcon::factory_type_id>& desired_types, bool pop_project) {
	assert(desired_types.empty());
	auto n = dcon::fatten(state.world, nid);
	auto m = dcon::fatten(state.world, mid);
	auto sid = m.get_zone_from_local_market();

	auto const tax_eff = economy::tax_collection_rate(state, nid, pid);
	auto const rich_effect = (1.0f - tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f);
	auto wage = state.world.province_get_labor_price(pid, economy::labor::basic_education) * 2.f;

	// pass zero:
	// factories with stupid income margins
	// which are impossible to ignore if you are sane
	if(desired_types.empty()) {
		for(auto type : state.world.in_factory_type) {
			if(n.get_active_building(type) || type.get_is_available_from_start()) {
				auto& inputs = type.get_inputs();
				bool lacking_input = false;
				bool lacking_output = m.get_demand_satisfaction(type.get_output()) < 0.98f;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(inputs.commodity_type[i]) {
						if(m.get_demand_satisfaction(inputs.commodity_type[i]) < 0.5f)
							lacking_input = true;
					} else {
						break;
					}
				}

				auto& constr_cost = type.get_construction_costs();
				auto lacking_constr = false;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(constr_cost.commodity_type[i]) {
						// If there is absolute deficit of construction goods, don't build for now
						// However, we're interested in this market signal only if there are any transactions happening
						if(m.get_demand(constr_cost.commodity_type[i]) > 0.01f && m.get_demand_satisfaction(constr_cost.commodity_type[i]) < 0.1f)
							lacking_constr = true;
					} else {
						break;
					}
				}

				float cost = economy::factory_type_build_cost(state, n, pid, type, pop_project) + 0.1f;
				float output = economy::factory_type_output_cost(state, n, m, type);
				float input = economy::factory_type_input_cost(state, n, m, type) + 0.1f;

				auto profit = (output - input - wage * type.get_base_workforce()) * (1.0f - rich_effect);
				auto roi = profit / cost;

				if(!lacking_constr && profit / input > 2.f && roi > 0.01f)
					desired_types.push_back(type.id);
			} // END if building unlocked
		}
	}

	// first pass: try to create factories which will pay back investment fast - in a year at most:
	// or have very high margins
	if(desired_types.empty()) {
		for(auto type : state.world.in_factory_type) {
			if(n.get_active_building(type) || type.get_is_available_from_start()) {
				auto& inputs = type.get_inputs();
				bool lacking_input = false;
				bool lacking_output = m.get_demand_satisfaction(type.get_output()) < 0.98f;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(inputs.commodity_type[i]) {
						if(m.get_demand_satisfaction(inputs.commodity_type[i]) < 0.5f)
							lacking_input = true;
					} else {
						break;
					}
				}

				auto& constr_cost = type.get_construction_costs();
				auto lacking_constr = false;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(constr_cost.commodity_type[i]) {
						// If there is absolute deficit of construction goods, don't build for now
						// However, we're interested in this market signal only if there are any transactions happening
						if(m.get_demand(constr_cost.commodity_type[i]) > 0.01f && m.get_demand_satisfaction(constr_cost.commodity_type[i]) < 0.1f)
							lacking_constr = true;
					} else {
						break;
					}
				}

				float cost = economy::factory_type_build_cost(state, n, pid, type, pop_project) + 0.1f;
				float output = economy::factory_type_output_cost(state, n, m, type);
				float input = economy::factory_type_input_cost(state, n, m, type) + 0.1f;
				auto profit = (output - input - wage * type.get_base_workforce()) * (1.0f - rich_effect);
				auto roi = profit / cost;

				if((!lacking_input && !lacking_constr && (lacking_output || (profit / cost > 0.005f))) || profit / input > 1.00f)
					desired_types.push_back(type.id);
			} // END if building unlocked
		}
	}

	// second pass: try to create factories which have a good profit margin
	if(desired_types.empty()) {
		for(auto type : state.world.in_factory_type) {
			if(n.get_active_building(type) || type.get_is_available_from_start()) {
				auto& inputs = type.get_inputs();
				bool lacking_input = false;
				bool lacking_output = m.get_demand_satisfaction(type.get_output()) < 0.98f;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(inputs.commodity_type[i]) {
						if(m.get_demand_satisfaction(inputs.commodity_type[i]) < 0.5f)
							lacking_input = true;
					} else {
						break;
					}
				}

				auto& constr_cost = type.get_construction_costs();
				auto lacking_constr = false;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(constr_cost.commodity_type[i]) {
						if(m.get_demand_satisfaction(constr_cost.commodity_type[i]) < 0.1f)
							lacking_constr = true;
					} else {
						break;
					}
				}

				float cost = economy::factory_type_build_cost(state, n, pid, type, pop_project) + 0.1f;
				float output = economy::factory_type_output_cost(state, n, m, type);
				float input = economy::factory_type_input_cost(state, n, m, type) + 0.1f;
				auto profit = (output - input - wage * type.get_base_workforce()) * (1.0f - rich_effect);
				auto roi = profit / cost;

				if(!lacking_input && !lacking_constr && profit / input > 0.3f && roi > 0.001f)
					desired_types.push_back(type.id);
			} // END if building unlocked
		}
	}
}

void get_state_craved_factory_types(sys::state& state, dcon::nation_id nid, dcon::market_id mid, dcon::province_id pid, std::vector<dcon::factory_type_id>& desired_types) {
	assert(desired_types.empty());
	auto n = dcon::fatten(state.world, nid);
	auto m = dcon::fatten(state.world, mid);
	auto sid = m.get_zone_from_local_market();
	auto treasury = n.get_stockpiles(economy::money);
	auto wage = state.world.province_get_labor_price(pid, economy::labor::basic_education) * 2.f;


	if(desired_types.empty()) {
		for(auto type : state.world.in_factory_type) {
			if(n.get_active_building(type) || type.get_is_available_from_start()) {

				float cost = economy::factory_type_build_cost(state, n, pid, type, false) + 0.1f;
				float output = economy::factory_type_output_cost(state, n, m, type);
				float input = economy::factory_type_input_cost(state, n, m, type) + 0.1f;

				if((output - input - wage * type.get_base_workforce()) / input > 20.f)
					desired_types.push_back(type.id);
			} // END if building unlocked
		}
	}
}

void get_state_desired_factory_types(sys::state& state, dcon::nation_id nid, dcon::market_id mid, dcon::province_id pid, std::vector<dcon::factory_type_id>& desired_types) {
	assert(desired_types.empty());
	auto n = dcon::fatten(state.world, nid);
	auto m = dcon::fatten(state.world, mid);
	auto wage = state.world.province_get_labor_price(pid, economy::labor::basic_education) * 2.f;

	auto treasury = n.get_stockpiles(economy::money);

	// first pass: try to create factories which will pay back investment fast - in a year at most:
	if(desired_types.empty()) {
		for(auto type : state.world.in_factory_type) {
			if(n.get_active_building(type) || type.get_is_available_from_start()) {
				auto& inputs = type.get_inputs();
				bool lacking_input = false;
				bool lacking_output = m.get_demand_satisfaction(type.get_output()) < 0.98f;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(inputs.commodity_type[i]) {
						if(m.get_demand_satisfaction(inputs.commodity_type[i]) < 0.5f)
							lacking_input = true;
					} else {
						break;
					}
				}

				float cost = economy::factory_type_build_cost(state, n, pid, type, false) + 0.1f;
				float output = economy::factory_type_output_cost(state, n, m, type);
				float input = economy::factory_type_input_cost(state, n, m, type) + 0.1f;

				if((lacking_output || ((output - input - wage * type.get_base_workforce()) / cost < 365.f)))
					desired_types.push_back(type.id);
			} // END if building unlocked
		}
	}

	// second pass: try to create factories which have a good profit margin
	if(desired_types.empty()) {
		for(auto type : state.world.in_factory_type) {
			if(n.get_active_building(type) || type.get_is_available_from_start()) {
				auto& inputs = type.get_inputs();
				bool lacking_input = false;
				bool lacking_output = m.get_demand_satisfaction(type.get_output()) < 0.98f;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(inputs.commodity_type[i]) {
						if(m.get_demand_satisfaction(inputs.commodity_type[i]) < 0.5f)
							lacking_input = true;
					} else {
						break;
					}
				}

				float cost = economy::factory_type_build_cost(state, n, pid, type, false) + 0.1f;
				float output = economy::factory_type_output_cost(state, n, m, type);
				float input = economy::factory_type_input_cost(state, n, m, type) + 0.1f;
				auto profitabilitymark = std::max(0.01f, cost * 10.f / treasury);

				if((lacking_output || ((output - input - wage * type.get_base_workforce()) / input > profitabilitymark)))
					desired_types.push_back(type.id);
			} // END if building unlocked
		}
	}
}


void update_ai_econ_construction(sys::state& state) {
	for(auto n : state.world.in_nation) {
		// skip over: non ais, dead nations, and nations that aren't making money
		if(n.get_owned_province_count() == 0 || !n.get_is_civilized())
			continue;

		if(n.get_is_player_controlled()) {
			// to handle the logic of player building automation later
			continue;
		}

		/*
		if(n.get_spending_level() < 1.0f || n.get_last_treasury() >= n.get_stockpiles(economy::money))
			continue;
		*/

		float treasury = n.get_stockpiles(economy::money);
		float base_income = economy::estimate_daily_income(state, n);
		float estimated_construction_costs = economy::estimate_construction_spending_from_budget(state, n, std::max(treasury, 1'000'000'000'000.f));

		//if our army is too small, ignore buildings:
		if(calculate_desired_army_size(state, n) * 0.4f > n.get_active_regiments())
			continue;

		float budget = treasury * 0.5f + base_income - estimated_construction_costs * 2.f;
		float additional_expenses = 0.f;
		float days_prepaid = 10.f;
		auto rules = n.get_combined_issue_rules();

		if(budget < 0.f) {
			continue;
		}

		if((rules & issue_rule::expand_factory) != 0 || (rules & issue_rule::build_factory) != 0) {
			// prepare a list of states
			static std::vector<dcon::province_id> ordered_provinces;
			ordered_provinces.clear();
			for(auto p : n.get_province_ownership()) {
				if(p.get_province().get_state_membership().get_capital().get_is_colonial() == false)
					ordered_provinces.push_back(p.get_province());
			}
			std::sort(ordered_provinces.begin(), ordered_provinces.end(), [&](auto a, auto b) {
				auto apop = state.world.province_get_demographics(a, demographics::total);
				auto bpop = state.world.province_get_demographics(b, demographics::total);
				if(apop != bpop)
					return apop > bpop;
				else
					return a.index() < b.index();
			});

			// try to build
			static::std::vector<dcon::factory_type_id> craved_types;

			// desired types filled: try to construct or upgrade
			if(!craved_types.empty()) {
				if((rules & issue_rule::build_factory) == 0 && (rules & issue_rule::expand_factory) != 0) { // can't build -- by elimination, can upgrade

				} else if((rules & issue_rule::build_factory) != 0) { // -- i.e. if building is possible
					for(auto p : ordered_provinces) {
						auto sid = state.world.province_get_state_membership(p);
						auto market = state.world.state_instance_get_market_from_local_market(sid);

						if(budget - additional_expenses <= 0.f)
							break;

						craved_types.clear();
						get_state_craved_factory_types(state, n, market, p, craved_types);

						// check -- either unemployed factory workers or no factory workers
						auto pw_num = state.world.province_get_demographics(p,
								demographics::to_key(state, state.culture_definitions.primary_factory_worker));
						pw_num += state.world.province_get_demographics(p,
								demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
						auto pw_employed = state.world.province_get_demographics(p,
								demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
						pw_employed += state.world.province_get_demographics(p,
								demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));

						if(pw_employed >= float(pw_num) * 2.5f && pw_num > 0.0f)
							continue; // no spare workers

						auto type_selection = craved_types[rng::get_random(state, uint32_t(n.id.index() + int32_t(budget))) % craved_types.size()];
						assert(type_selection);

						if(state.world.factory_type_get_is_coastal(type_selection) && !state.world.province_get_port_to(p))
							continue;

						bool already_in_progress = [&]() {
							for(auto fc : state.world.province_get_factory_construction(p)) {
								if(fc.get_type() == type_selection)
									return true;
							}
							return false;
							}();

						if(already_in_progress)
							continue;

						// check: if present, try to upgrade
						bool present_in_location = false;
						bool under_cap = false;

						for(auto fac : state.world.province_get_factory_location(p)) {
							auto type = fac.get_factory().get_building_type();
							if(type_selection == type) {
								under_cap = economy::factory_total_employment_score(state, fac.get_factory()) < 0.9f;
								present_in_location = true;
								return;
							}
						}
						if(under_cap) {
							continue; // factory doesn't need to get larger
						}

						auto expected_item_cost = 0.f;
						auto costs = state.world.factory_type_get_construction_costs(type_selection);
						auto time = state.world.factory_type_get_construction_time(type_selection);
						for(uint32_t i = 0; i < costs.set_size; ++i) {
							if(costs.commodity_type[i]) {
								expected_item_cost +=
									costs.commodity_amounts[i]
									* economy::price(state, market, costs.commodity_type[i])
									/ float(time)
									* days_prepaid;
							} else {
								break;
							}
						}

						if(budget - additional_expenses - expected_item_cost <= 0.f)
							continue;

						if(present_in_location) {
							if((rules & issue_rule::expand_factory) != 0 && economy::do_resource_potentials_allow_upgrade(state, n, p, type_selection)) {
								auto new_up = fatten(state.world, state.world.force_create_factory_construction(p, n));
								new_up.set_is_pop_project(false);
								new_up.set_is_upgrade(true);
								new_up.set_type(type_selection);

								additional_expenses += expected_item_cost;
							}
							continue;
						}

						// else -- try to build -- must have room
						auto num_factories = economy::province_factory_count(state, p);
						if(num_factories < int32_t(state.defines.factories_per_state) && economy::do_resource_potentials_allow_construction(state, n, p, type_selection)) {
							auto new_up = fatten(state.world, state.world.force_create_factory_construction(p, n));
							new_up.set_is_pop_project(false);
							new_up.set_is_upgrade(false);
							new_up.set_type(type_selection);
							additional_expenses += expected_item_cost;
							continue;
						} else {
							// TODO: try to delete a factory here
						}
					} // END for(auto si : ordered_states) {
				} // END if((rules & issue_rule::build_factory) == 0)
			} // END if(!desired_types.empty()) {

			// try to upgrade factories first:
			if((rules & issue_rule::expand_factory) != 0) { // can't build -- by elimination, can upgrade
				for(auto pid : ordered_provinces) {

					auto sid = state.world.province_get_state_membership(pid);
					auto market = state.world.state_instance_get_market_from_local_market(sid);

					if(budget - additional_expenses <= 0.f)
						break;

					for(auto fac : state.world.province_get_factory_location(pid)) {
						auto type = fac.get_factory().get_building_type();


						auto unprofitable = fac.get_factory().get_unprofitable();

						if(!unprofitable && economy::factory_total_employment_score(state, fac.get_factory()) >= 0.9f) {
							// test if factory is already upgrading
							auto ug_in_progress = false;
							for(auto c : state.world.province_get_factory_construction(pid)) {
								if(c.get_type() == type) {
									ug_in_progress = true;
									break;
								}
							}

							auto expected_item_cost = 0.f;
							auto& costs = state.world.factory_type_get_construction_costs(type);
							auto& time = state.world.factory_type_get_construction_time(type);
							for(uint32_t i = 0; i < costs.set_size; ++i) {
								if(costs.commodity_type[i]) {
									expected_item_cost +=
										costs.commodity_amounts[i]
										* economy::price(state, market, costs.commodity_type[i])
										/ float(time)
										* days_prepaid;
								} else {
									break;
								}
							}

							if(budget - additional_expenses - expected_item_cost <= 0.f)
								continue;

							if(!ug_in_progress && economy::do_resource_potentials_allow_upgrade(state, n, pid, type)) {
								auto new_up = fatten(state.world, state.world.force_create_factory_construction(pid, n));
								new_up.set_is_pop_project(false);
								new_up.set_is_upgrade(true);
								new_up.set_type(type);

								additional_expenses += expected_item_cost;
							}
						}
					}
				}
			}

			// try to build
			static::std::vector<dcon::factory_type_id> desired_types;

			// desired types filled: try to construct or upgrade
			if(!desired_types.empty()) {
				if((rules & issue_rule::build_factory) == 0 && (rules & issue_rule::expand_factory) != 0) { // can't build -- by elimination, can upgrade

				} else if((rules & issue_rule::build_factory) != 0) { // -- i.e. if building is possible
					for(auto pid : ordered_provinces) {

						auto sid = state.world.province_get_state_membership(pid);
						auto market = state.world.state_instance_get_market_from_local_market(sid);

						if(budget - additional_expenses <= 0.f)
							break;

						desired_types.clear();
						get_state_desired_factory_types(state, n, market, pid, desired_types);

						// check -- either unemployed factory workers or no factory workers
						auto pw_num = state.world.province_get_demographics(pid,
								demographics::to_key(state, state.culture_definitions.primary_factory_worker));
						pw_num += state.world.province_get_demographics(pid,
								demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
						auto pw_employed = state.world.province_get_demographics(pid,
								demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
						pw_employed += state.world.province_get_demographics(pid,
								demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));

						if(pw_employed >= float(pw_num) * 2.5f && pw_num > 0.0f)
							continue; // no spare workers

						auto type_selection = desired_types[rng::get_random(state, uint32_t(n.id.index() + int32_t(budget))) % desired_types.size()];
						assert(type_selection);

						if(state.world.factory_type_get_is_coastal(type_selection) && !state.world.province_get_port_to(pid))
							continue;

						bool already_in_progress = [&]() {
							for(auto p : state.world.province_get_factory_construction(pid)) {
								if(p.get_type() == type_selection)
									return true;
							}
							return false;
							}();

						if(already_in_progress)
							continue;

						// check: if present, try to upgrade
						bool present_in_location = false;
						bool under_cap = false;

						for(auto fac : state.world.province_get_factory_location(pid)) {
							auto type = fac.get_factory().get_building_type();
							if(type_selection == type) {
								under_cap = economy::factory_total_employment_score(state, fac.get_factory()) < 0.9f;
								present_in_location = true;
								return;
							}
						}
						if(under_cap) {
							continue; // factory doesn't need to get larger
						}

						auto expected_item_cost = 0.f;
						auto costs = state.world.factory_type_get_construction_costs(type_selection);
						auto time = state.world.factory_type_get_construction_time(type_selection);
						for(uint32_t i = 0; i < costs.set_size; ++i) {
							if(costs.commodity_type[i]) {
								expected_item_cost +=
									costs.commodity_amounts[i]
									* economy::price(state, market, costs.commodity_type[i])
									/ float(time)
									* days_prepaid;
							} else {
								break;
							}
						}

						if(budget - additional_expenses - expected_item_cost <= 0.f)
							continue;

						if(present_in_location) {
							if((rules & issue_rule::expand_factory) != 0 && economy::do_resource_potentials_allow_upgrade(state, n, pid, type_selection)) {
								auto new_up = fatten(state.world, state.world.force_create_factory_construction(pid, n));
								new_up.set_is_pop_project(false);
								new_up.set_is_upgrade(true);
								new_up.set_type(type_selection);

								additional_expenses += expected_item_cost;
							}
							continue;
						}

						// else -- try to build -- must have room
						int32_t num_factories = economy::province_factory_count(state, pid);
						if(num_factories < int32_t(state.defines.factories_per_state) && economy::do_resource_potentials_allow_construction(state, n, pid, type_selection)) {
							auto new_up = fatten(state.world, state.world.force_create_factory_construction(pid, n));
							new_up.set_is_pop_project(false);
							new_up.set_is_upgrade(false);
							new_up.set_type(type_selection);
							additional_expenses += expected_item_cost;
							continue;
						} else {
							// TODO: try to delete a factory here
						}
					} // END for(auto si : ordered_states) {
				} // END if((rules & issue_rule::build_factory) == 0)
			} // END if(!desired_types.empty()) {
		} // END  if((rules & issue_rule::expand_factory) != 0 || (rules & issue_rule::build_factory) != 0)

		// AI has smarter desired army size, this check stops economic devt.
		//if(0.9f * n.get_recruitable_regiments() > n.get_active_regiments())
		//	continue;

		static std::vector<dcon::province_id> project_provs;
		project_provs.clear();

		// try naval bases
		if(budget - additional_expenses >= 0.f) {
			project_provs.clear();
			for(auto o : n.get_province_ownership()) {
				if(!o.get_province().get_is_coast())
					continue;
				if(n != o.get_province().get_nation_from_province_control())
					continue;

				if(military::province_is_under_siege(state, o.get_province()))
					continue;
				if(o.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)) == 0 && o.get_province().get_state_membership().get_naval_base_is_taken())
					continue;

				int32_t current_lvl = o.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base));
				int32_t max_local_lvl = n.get_max_building_level(uint8_t(economy::province_building_type::naval_base));
				int32_t min_build = int32_t(o.get_province().get_modifier_values(sys::provincial_mod_offsets::min_build_naval_base));

				if(max_local_lvl - current_lvl - min_build <= 0)
					continue;

				if(!province::has_naval_base_being_built(state, o.get_province())) {
					project_provs.push_back(o.get_province().id);
				}
			}

			auto cap = n.get_capital();
			std::sort(project_provs.begin(), project_provs.end(), [&](dcon::province_id a, dcon::province_id b) {
				auto a_dist = province::sorting_distance(state, a, cap);
				auto b_dist = province::sorting_distance(state, b, cap);
				if(a_dist != b_dist)
					return a_dist < b_dist;
				else
					return a.index() < b.index();
			});
			if(!project_provs.empty()) {
				auto si = state.world.province_get_state_membership(project_provs[0]);
				auto market = state.world.state_instance_get_market_from_local_market(si);

				// avoid overbuilding!

				auto expected_item_cost = 0.f;
				auto& costs = state.economy_definitions.building_definitions[int32_t(economy::province_building_type::naval_base)].cost;
				auto& time = state.economy_definitions.building_definitions[int32_t(economy::province_building_type::naval_base)].time;
				for(uint32_t i = 0; i < costs.set_size; ++i) {
					if(costs.commodity_type[i]) {
						expected_item_cost +=
							costs.commodity_amounts[i]
							* economy::price(state, market, costs.commodity_type[i])
							/ float(time)
							* days_prepaid;
					} else {
						break;
					}
				}

				if(budget - additional_expenses - expected_item_cost <= 0.f)
					continue;

				if(si)
					si.set_naval_base_is_taken(true);
				auto new_rr = fatten(state.world, state.world.force_create_province_building_construction(project_provs[0], n));
				new_rr.set_is_pop_project(false);
				new_rr.set_type(uint8_t(economy::province_building_type::naval_base));
				additional_expenses += expected_item_cost;
			}
		}

		// try railroads
		const struct {
			bool buildable;
			economy::province_building_type type;
			dcon::provincial_modifier_value mod;
		} econ_buildable[3] = {
			{ (rules & issue_rule::build_railway) != 0, economy::province_building_type::railroad, sys::provincial_mod_offsets::min_build_railroad },
			{ (rules & issue_rule::build_bank) != 0 && state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined, economy::province_building_type::bank, sys::provincial_mod_offsets::min_build_bank },
			{ (rules & issue_rule::build_university) != 0 && state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined, economy::province_building_type::university, sys::provincial_mod_offsets::min_build_university }
		};
		for(auto i = 0; i < 3; i++) {
			if(econ_buildable[i].buildable && budget - additional_expenses > 0) {
				project_provs.clear();
				for(auto o : n.get_province_ownership()) {
					if(n != o.get_province().get_nation_from_province_control())
						continue;
					if(military::province_is_under_siege(state, o.get_province()))
						continue;
					int32_t current_lvl = state.world.province_get_building_level(o.get_province(), uint8_t(econ_buildable[i].type));
					int32_t max_local_lvl = state.world.nation_get_max_building_level(n, uint8_t(econ_buildable[i].type));
					int32_t min_build = int32_t(state.world.province_get_modifier_values(o.get_province(), econ_buildable[i].mod));
					if(max_local_lvl - current_lvl - min_build <= 0)
						continue;
					if(!province::has_province_building_being_built(state, o.get_province(), econ_buildable[i].type)) {
						project_provs.push_back(o.get_province().id);
					}
				}
				auto cap = n.get_capital();
				std::sort(project_provs.begin(), project_provs.end(), [&](dcon::province_id a, dcon::province_id b) {
					auto a_dist = province::sorting_distance(state, a, cap);
					auto b_dist = province::sorting_distance(state, b, cap);
					if(a_dist != b_dist)
						return a_dist < b_dist;
					else
						return a.index() < b.index();
				});
				for(uint32_t j = 0; j < project_provs.size() && budget - additional_expenses > 0; ++j) {
					auto sid = state.world.province_get_state_membership(project_provs[j]);
					auto market = state.world.state_instance_get_market_from_local_market(sid);

					// avoid overbuilding!

					auto expected_item_cost = 0.f;
					auto& costs = state.economy_definitions.building_definitions[uint8_t(econ_buildable[i].type)].cost;
					auto& time = state.economy_definitions.building_definitions[uint8_t(econ_buildable[i].type)].time;
					for(uint32_t k = 0; k < costs.set_size; ++k) {
						if(costs.commodity_type[k]) {
							expected_item_cost +=
								costs.commodity_amounts[k]
								* economy::price(state, market, costs.commodity_type[k])
								/ float(time)
								* days_prepaid;
						} else {
							break;
						}
					}

					if(budget - additional_expenses - expected_item_cost <= 0.f)
						continue;

					auto new_proj = fatten(state.world, state.world.force_create_province_building_construction(project_provs[j], n));
					new_proj.set_is_pop_project(false);
					new_proj.set_type(uint8_t(econ_buildable[i].type));
					additional_expenses += expected_item_cost;
				}
			}
		}

		// AI has smarter desired army size, this check stops economic devt.
		//if(0.95f * n.get_recruitable_regiments() > n.get_active_regiments())
		//	continue;

		// try forts
		if(budget - additional_expenses > 0.f) {
			project_provs.clear();

			for(auto o : n.get_province_ownership()) {
				if(n != o.get_province().get_nation_from_province_control())
					continue;

				if(military::province_is_under_siege(state, o.get_province()))
					continue;

				int32_t current_lvl = state.world.province_get_building_level(o.get_province(), uint8_t(economy::province_building_type::fort));
				int32_t max_local_lvl = state.world.nation_get_max_building_level(n, uint8_t(economy::province_building_type::fort));
				int32_t min_build = int32_t(state.world.province_get_modifier_values(o.get_province(), sys::provincial_mod_offsets::min_build_fort));

				if(max_local_lvl - current_lvl - min_build <= 0)
					continue;

				if(!province::has_fort_being_built(state, o.get_province())) {
					project_provs.push_back(o.get_province().id);
				}
			}

			auto cap = n.get_capital();
			std::sort(project_provs.begin(), project_provs.end(), [&](dcon::province_id a, dcon::province_id b) {
				auto a_dist = province::sorting_distance(state, a, cap);
				auto b_dist = province::sorting_distance(state, b, cap);
				if(a_dist != b_dist)
					return a_dist < b_dist;
				else
					return a.index() < b.index();
			});

			for(uint32_t i = 0; i < project_provs.size() && budget - additional_expenses > 0.f; ++i) {

				auto sid = state.world.province_get_state_membership(project_provs[i]);
				auto market = state.world.state_instance_get_market_from_local_market(sid.id);

				// avoid overbuilding!

				auto expected_item_cost = 0.f;
				auto& costs = state.economy_definitions.building_definitions[uint8_t(economy::province_building_type::fort)].cost;
				auto& time = state.economy_definitions.building_definitions[uint8_t(economy::province_building_type::fort)].time;
				for(uint32_t k = 0; k < costs.set_size; ++k) {
					if(costs.commodity_type[k]) {
						expected_item_cost +=
							costs.commodity_amounts[k]
							* economy::price(state, market, costs.commodity_type[k])
							* days_prepaid
							* 100000.f;
						// forts are very bad investment and demand volatile goods,
						// so build them if AI is really rich and has no idea where how to spend money
					} else {
						break;
					}
				}

				if(budget - additional_expenses - expected_item_cost <= 0.f)
					continue;

				auto new_rr = fatten(state.world, state.world.force_create_province_building_construction(project_provs[i], n));
				new_rr.set_is_pop_project(false);
				new_rr.set_type(uint8_t(economy::province_building_type::fort));
				additional_expenses += expected_item_cost;
			}
		}
	}
}

void update_budget(sys::state& state) {
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id nid{ dcon::nation_id::value_base_t(i) };
		auto n = fatten(state.world, nid);
		if(n.get_is_player_controlled() || n.get_owned_province_count() == 0)
			return;

		float base_income = economy::estimate_daily_income(state, n) + n.get_stockpiles(economy::money) / 365.f;

		// they don't have to add up to 1.f
		// the reason they are there is to slow down AI spendings,
		// make them more or less balanced
		// and stabilize economy faster
		// not to allow it to hoard money

		float land_budget_ratio = 0.15f;
		float sea_budget_ratio = 0.05f;
		float education_budget_ratio = 0.30f;
		float investments_budget_ratio = 0.05f;
		float soldiers_budget_ratio = 0.30f;
		float construction_budget_ratio = 0.45f;
		float overseas_maintenance_budget_ratio = 0.10f;

		if(n.get_is_at_war()) {
			land_budget_ratio *= 1.75f;
			sea_budget_ratio *= 1.75f;
			education_budget_ratio *= 0.15f;
			overseas_maintenance_budget_ratio *= 0.15f;
			//n.set_land_spending(int8_t(100));
			//n.set_naval_spending(int8_t(100));
		} else if(n.get_ai_is_threatened()) {
			land_budget_ratio *= 1.25f;
			sea_budget_ratio *= 1.25f;
			education_budget_ratio *= 0.75f;
			overseas_maintenance_budget_ratio *= 0.75f;
			//n.set_land_spending(int8_t(50));
			//n.set_naval_spending(int8_t(50));
		} else {
			//n.set_land_spending(int8_t(25));
			//n.set_naval_spending(int8_t(25));
		}
		float land_budget = land_budget_ratio * base_income;
		float naval_budget = sea_budget_ratio * base_income;
		float education_budget = education_budget_ratio * base_income;
		float construction_budget = construction_budget_ratio * base_income;
		float soldiers_budget = soldiers_budget_ratio * base_income;
		float overseas_budget = overseas_maintenance_budget_ratio * base_income;

		float ratio_land = 100.f * land_budget / (1.f + economy::estimate_land_spending(state, n));
		float ratio_naval = 100.f * naval_budget / (1.f + economy::estimate_naval_spending(state, n));

		ratio_land = std::clamp(ratio_land, 0.f, 100.f);
		ratio_naval = std::clamp(ratio_naval, 0.f, 100.f);
		// Reduce spending at peace
		if(!state.world.nation_get_is_at_war(n) && future_rebels_in_nation(state, n) == 0) {
			ratio_land /= 10.f;
			ratio_naval /= 10.f;
		}
		n.set_land_spending(int8_t(ratio_land));
		n.set_naval_spending(int8_t(ratio_naval));

		n.set_construction_spending(50);
		n.set_administrative_spending(25);

		float max_education_budget = 1.f + economy::estimate_pop_payouts_by_income_type(state, n, culture::income_type::education);
		float max_soldiers_budget = 1.f + economy::estimate_pop_payouts_by_income_type(state, n, culture::income_type::military);
		float max_overseas_budget = 1.f + economy::estimate_overseas_penalty_spending(state, n);

		// solving x^2 * max = desired
		float ratio_education = 100.f * math::sqrt(education_budget / max_education_budget);
		ratio_education = std::clamp(ratio_education, 0.f, 100.f);
		n.set_education_spending(int8_t(ratio_education));

		// If State can build factories - why subsidize capitalists
		auto rules = n.get_combined_issue_rules();
		if(n.get_is_civilized() && (rules & issue_rule::build_factory) == 0) {
			n.set_domestic_investment_spending(int8_t(investments_budget_ratio * 100.f));
		} else {
			n.set_domestic_investment_spending(int8_t(0));
		}

		float soldiers_max_ratio = 100.f * math::sqrt(soldiers_budget / max_soldiers_budget);
		soldiers_max_ratio = std::clamp(soldiers_max_ratio, 0.f, 100.f);

		float overseas_max_ratio = std::clamp(100.f * overseas_budget / max_overseas_budget, 0.f, 100.f);

		n.set_tariffs_import(int8_t(10));
		n.set_tariffs_export(int8_t(10));

		float poor_militancy = (state.world.nation_get_demographics(n, demographics::poor_militancy) / std::max(1.0f, state.world.nation_get_demographics(n, demographics::poor_total))) / 10.f;
		float mid_militancy = (state.world.nation_get_demographics(n, demographics::middle_militancy) / std::max(1.0f, state.world.nation_get_demographics(n, demographics::middle_total))) / 10.f;
		float rich_militancy = (state.world.nation_get_demographics(n, demographics::rich_militancy) / std::max(1.0f, state.world.nation_get_demographics(n, demographics::rich_total))) / 10.f;

		// don't tax pops too much
		if((rules & issue_rule::expand_factory) != 0 || (rules & issue_rule::build_factory) != 0) {
			// Non-lf prioritize poor people
			int max_poor_tax = int(10.f + 25.f * (1.f - poor_militancy));
			int max_mid_tax = int(10.f + 35.f * (1.f - mid_militancy));
			int max_rich_tax = int(10.f + 50.f * (1.f - rich_militancy));
			int max_social = int(100.f * poor_militancy);

			// enough tax?
			bool enough_tax = true;
			if(ratio_education < 50.f) {
				enough_tax = false;
				n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() + 2, 10, std::max(10, max_poor_tax))));
				n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() + 3, 10, std::max(10, max_mid_tax))));
				n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() + 5, 10, std::max(10, max_rich_tax))));
			}

			if(n.get_spending_level() < 1.0f || n.get_last_treasury() >= n.get_stockpiles(economy::money) || ratio_education < 50.f) { // losing money
				if(!n.get_ai_is_threatened()) {
					n.set_military_spending(int8_t(std::max(50, n.get_military_spending() - 5)));
				}
				n.set_social_spending(1);

				n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() + 2, 10, std::max(10, max_poor_tax))));
				n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() + 3, 10, std::max(10, max_mid_tax))));
				n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() + 5, 10, std::max(10, max_rich_tax))));
			} else if(n.get_last_treasury() < n.get_stockpiles(economy::money)) { // gaining money
				if(n.get_ai_is_threatened()) {
					n.set_military_spending(int8_t(std::min(100, n.get_military_spending() + 10)));
				} else {
					n.set_military_spending(int8_t(std::min(75, n.get_military_spending() + 10)));
				}
				n.set_social_spending(3);

				if(enough_tax) {
					n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() - 2, 10, std::max(10, max_poor_tax))));
					n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() - 3, 10, std::max(10, max_mid_tax))));
					n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() - 5, 10, std::max(10, max_rich_tax))));
				}
			}
		} else {
			int max_poor_tax = int(10.f + 30.f * (1.f - poor_militancy));
			int max_mid_tax = int(10.f + 30.f * (1.f - mid_militancy));
			int max_rich_tax = int(10.f + 10.f * (1.f - rich_militancy));
			int max_social = int(20.f * poor_militancy);

			// enough tax?
			bool enough_tax = true;
			if(ratio_education < 50.f) {
				enough_tax = false;
				n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() + 5, 10, std::max(10, max_poor_tax))));
				n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() + 3, 10, std::max(10, max_mid_tax))));
				n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() + 2, 10, std::max(10, max_rich_tax))));
			}

			// Laissez faire prioritize tax free capitalists
			if(n.get_spending_level() < 1.0f || n.get_last_treasury() >= n.get_stockpiles(economy::money) || ratio_education < 50.f) { // losing money
				if(!n.get_ai_is_threatened()) {
					n.set_military_spending(int8_t(std::max(50, n.get_military_spending() - 5)));
				}
				n.set_social_spending(int8_t(max_social / 2));

				n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() + 5, 10, std::max(10, max_poor_tax))));
				n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() + 3, 10, std::max(10, max_mid_tax))));
				n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() + 2, 10, std::max(10, max_rich_tax))));
			} else if(n.get_last_treasury() < n.get_stockpiles(economy::money)) { // gaining money
				if(n.get_ai_is_threatened()) {
					n.set_military_spending(int8_t(std::min(100, n.get_military_spending() + 10)));
				} else {
					n.set_military_spending(int8_t(std::min(75, n.get_military_spending() + 10)));
				}
				n.set_social_spending(int8_t(max_social));

				if(enough_tax) {
					n.set_poor_tax(int8_t(std::clamp(n.get_poor_tax() - 5, 10, std::max(10, max_poor_tax))));
					n.set_middle_tax(int8_t(std::clamp(n.get_middle_tax() - 3, 10, std::max(10, max_mid_tax))));
					n.set_rich_tax(int8_t(std::clamp(n.get_rich_tax() - 2, 10, std::max(10, max_rich_tax))));
				}
			}
		}

		n.set_military_spending(int8_t(std::min(int8_t(soldiers_max_ratio), n.get_military_spending())));
		n.set_overseas_spending(int8_t(overseas_max_ratio));

		economy::bound_budget_settings(state, n);
	});
}

}
