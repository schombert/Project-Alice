#include "construction.hpp"
#include "economy_stats.hpp"
#include "province_templates.hpp"

namespace economy {

economy::commodity_set calculate_factory_refit_goods_cost(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid, dcon::factory_type_id from, dcon::factory_type_id to) {
	auto& from_cost = state.world.factory_type_get_construction_costs(from);
	auto& to_cost = state.world.factory_type_get_construction_costs(to);

	float level = 1;

	auto d = state.world.state_instance_get_definition(sid);
	auto o = state.world.state_instance_get_nation_from_state_ownership(sid);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			for(auto f : p.get_province().get_factory_location()) {
				if(f.get_factory().get_building_type() == from) {
					level = f.get_factory().get_size() / f.get_factory().get_building_type().get_base_workforce();
				}
			}
		}
	}
 

	// Refit cost = (to_cost) - (from_cost) + (0.1f * to_cost)
	float refit_mod = 1.0f + state.defines.alice_factory_refit_cost_modifier;

	economy::commodity_set res;

	// First take 110% of to_cost as a baseline
	if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
		for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
			if(to_cost.commodity_type[j]) {
				res.commodity_type[j] = to_cost.commodity_type[j];
				res.commodity_amounts[j] = to_cost.commodity_amounts[j] * refit_mod * level;
			} else {
				break;
			}
		}
	}

	// Substract from_cost to represent refit discount
	if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(!from_cost.commodity_type[i]) {
				break;
			}

			auto from_amount = from_cost.commodity_amounts[i] * level;
			auto from_commodity = from_cost.commodity_type[i];

			for(uint32_t j = 0; i < commodity_set::set_size; ++j) {
				if(!res.commodity_type[j]) {
					break;
				}

				if(res.commodity_type[j] == from_commodity) {
					res.commodity_amounts[j] = std::max(res.commodity_amounts[j] - from_amount, 0.f);
				}
			}
		}
	}

	return res;
}
float calculate_factory_refit_money_cost(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid, dcon::factory_type_id from, dcon::factory_type_id to) {
	auto goods_cost = calculate_factory_refit_goods_cost(state, n, sid, from, to);

	float admin_eff = state.world.nation_get_administrative_efficiency(n);
	float admin_cost_factor = 2.0f - admin_eff;
	float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

	auto total = 0.0f;
	for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
		if(goods_cost.commodity_type[i]) {
			total += economy::price(state, sid, goods_cost.commodity_type[i]) * goods_cost.commodity_amounts[i] * factory_mod * admin_cost_factor;
		}
	}

	return total;
}

float global_non_factory_construction_time_modifier(sys::state& state) {
	return 0.1f;
}

float global_factory_construction_time_modifier(sys::state& state) {
	return 0.1f;
}

float build_cost_multiplier(sys::state& state, dcon::nation_id n, dcon::province_id location, bool is_pop_project) {
	float admin_eff = state.world.nation_get_administrative_efficiency(n);
	return is_pop_project ? 1.f : 2.0f - admin_eff;
}

float factory_build_cost_multiplier(sys::state& state, dcon::nation_id n, dcon::province_id location, bool is_pop_project) {
	return build_cost_multiplier(state, n, location, is_pop_project)
		* (std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost)) + 1.0f);
}

float unit_construction_time(
	sys::state& state,
	dcon::unit_type_id utid
) {
	return global_non_factory_construction_time_modifier(state)
		* float(state.military_definitions.unit_base_definitions[utid].build_time);
}

float province_building_construction_time(
	sys::state& state,
	economy::province_building_type building_type
) {
	assert(0 <= int32_t(building_type) && int32_t(building_type) < int32_t(economy::max_building_types));
	return global_non_factory_construction_time_modifier(state)
		* float(state.economy_definitions.building_definitions[int32_t(building_type)].time);
}

float factory_building_construction_time(
	sys::state& state, dcon::factory_type_id ftid, bool is_upgrade
) {
	return global_factory_construction_time_modifier(state)
		* float(state.world.factory_type_get_construction_time(ftid))
		* (is_upgrade ? 0.5f : 1.0f);
}

// it's registered as demand separately, do not add actual demand here
void register_construction_demand(sys::state& state, dcon::market_id s, dcon::commodity_id commodity_type, float amount) {
	state.world.market_get_construction_demand(s, commodity_type) += amount;
	assert(state.world.market_get_construction_demand(s, commodity_type) >= 0.f);
}

void reset_construction_demand(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_construction_demand(ids, cid, 0.0f);
		});
	}
}
void reset_private_construction_demand(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_private_construction_demand(ids, cid, 0.0f);
		});
	}
}

unit_construction_data explain_land_unit_construction(
	sys::state& state,
	dcon::province_land_construction_id construction
) {
	auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(construction));
	auto owner = state.world.province_get_nation_from_province_ownership(province);
	auto local_zone = state.world.province_get_state_membership(province);
	auto unit_type = state.world.province_land_construction_get_type(construction);
	unit_construction_data result = {
		.can_be_advanced = (owner && state.world.province_get_nation_from_province_control(province) == owner),
		.construction_time = unit_construction_time(state, unit_type),
		.cost_multiplier = build_cost_multiplier(state, owner, province, false),
		.owner = owner,
		.market = state.world.state_instance_get_market_from_local_market(local_zone),
		.province = province,
		.unit_type = unit_type
	};
	return result;
}

void advance_land_unit_construction(
	sys::state& state,
	dcon::province_land_construction_id lc
) {
	auto details = explain_land_unit_construction(state, lc);
	auto& base_cost = state.military_definitions.unit_base_definitions[details.unit_type].build_cost;
	auto& current_purchased = state.world.province_land_construction_get_purchased_goods(lc);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid)
			break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required)	continue;
		auto& source = state.world.market_get_construction_demand(details.market, cid);
		auto delta = std::clamp(required / details.construction_time, 0.f, source);
		current_purchased.commodity_amounts[i] += delta;
		source -= delta;
	}
}

void populate_land_unit_construction_demand(
	sys::state& state,
	dcon::province_land_construction_id lc,
	float& budget,
	float budget_limit
) {
	auto details = explain_land_unit_construction(state, lc);
	if(!details.can_be_advanced) {
		return;
	}
	auto& base_cost =
		state.military_definitions.unit_base_definitions[
			state.world.province_land_construction_get_type(lc)
		].build_cost;
	auto& current_purchased	= state.world.province_land_construction_get_purchased_goods(lc);

	auto unit_type = state.world.province_land_construction_get_type(lc);
	float construction_time = unit_construction_time(state, unit_type);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required)	continue;
		auto local_price = price(state, details.market, cid);
		auto can_purchase_budget = std::min(budget_limit, budget) / (local_price + 0.001f);
		auto can_purchase_construction = required / construction_time;
		auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
		auto satisfaction = state.world.market_get_demand_satisfaction(details.market, cid);
		budget = std::max(0.f, budget - can_purchase * local_price * satisfaction);
		register_construction_demand(state,	details.market,	cid, can_purchase);
	}
}

unit_construction_data explain_naval_unit_construction(
	sys::state& state,
	dcon::province_naval_construction_id construction
) {
	auto province = state.world.province_naval_construction_get_province(construction);
	auto owner = state.world.province_get_nation_from_province_ownership(province);
	auto local_zone = state.world.province_get_state_membership(province);
	auto unit_type = state.world.province_naval_construction_get_type(construction);
	unit_construction_data result = {
		.can_be_advanced = (owner && state.world.province_get_nation_from_province_control(province) == owner),
		.construction_time = unit_construction_time(state, unit_type),
		.cost_multiplier = build_cost_multiplier(state, owner, province, false),
		.owner = owner,
		.market = state.world.state_instance_get_market_from_local_market(local_zone),
		.province = province,
		.unit_type = unit_type
	};
	return result;
}

void advance_naval_unit_construction(
	sys::state& state,
	dcon::province_naval_construction_id construction
) {
	auto details = explain_naval_unit_construction(state, construction);
	auto& base_cost = state.military_definitions.unit_base_definitions[details.unit_type].build_cost;
	auto& current_purchased = state.world.province_naval_construction_get_purchased_goods(construction);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required)	continue;
		auto& source = state.world.market_get_construction_demand(details.market, cid);
		auto delta = std::clamp(required / details.construction_time, 0.f, source);
		current_purchased.commodity_amounts[i] += delta;
		source -= delta;
	}
}

void populate_naval_unit_construction_demand(
	sys::state& state,
	dcon::province_naval_construction_id construction,
	float& budget,
	float budget_limit
) {
	auto details = explain_naval_unit_construction(state, construction);
	if(!details.can_be_advanced) return;
	auto& base_cost = state.military_definitions.unit_base_definitions[details.unit_type].build_cost;
	auto& current_purchased = state.world.province_naval_construction_get_purchased_goods(construction);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid)
			break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required)
			continue;
		auto local_price = price(state, details.market, cid);
		auto can_purchase_budget = std::min(budget_limit, budget) / (local_price + 0.001f);
		auto can_purchase_construction = required / details.construction_time;
		auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
		auto satisfaction = state.world.market_get_demand_satisfaction(details.market, cid);
		budget = std::max(0.f, budget - can_purchase * local_price * satisfaction);
		register_construction_demand(state, details.market, cid, can_purchase);
	}
}

struct province_building_construction_data {
	bool can_be_advanced;
	bool is_pop_project;
	bool is_upgrade;
	float construction_time;
	float cost_multiplier;
	dcon::nation_id owner;
	dcon::market_id market;
	dcon::province_id province;
	economy::province_building_type building_type;
};

province_building_construction_data explain_province_building_construction(
	sys::state& state,
	dcon::province_building_construction_id construction
) {
	auto owner = state.world.province_building_construction_get_nation(construction);
	auto province = state.world.province_building_construction_get_province(construction);
	auto local_zone = state.world.province_get_state_membership(province);
	auto raw_type = state.world.province_building_construction_get_type(construction);
	auto t = economy::province_building_type(raw_type);
	auto is_pop_project = state.world.province_building_construction_get_is_pop_project(construction);
	province_building_construction_data result = {
		.can_be_advanced = (owner && state.world.province_get_nation_from_province_control(province) == owner),
		.is_pop_project = is_pop_project,
		.is_upgrade = false,
		.construction_time = province_building_construction_time(state, t),
		.cost_multiplier = build_cost_multiplier(state, owner, province, is_pop_project),
		.owner = owner,
		.market = state.world.state_instance_get_market_from_local_market(local_zone),
		.province = province,
		.building_type = t,
	};
	return result;
}

//handles both private and national building
void advance_province_building_construction(
	sys::state& state,
	dcon::province_building_construction_id construction
) {
	auto details = explain_province_building_construction(state, construction);
	assert(0 <= int32_t(details.building_type) && int32_t(details.building_type) < int32_t(economy::max_building_types));
	auto& base_cost = state.economy_definitions.building_definitions[int32_t(details.building_type)].cost;
	auto& current_purchased = state.world.province_building_construction_get_purchased_goods(construction);

	// Rationale for not checking the building type:
	// Pop projects created for forts and naval bases should NOT happen in the first place, so checking against them
	// is a waste of resources
	// peter: i do not understand what the above comment means
	// but i guess it was an important piece of info during the ancient times

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required)
			continue;
		auto amount = required / details.construction_time;
		if(details.is_pop_project) {
			auto& source_private = state.world.market_get_private_construction_demand(details.market, base_cost.commodity_type[i]);
			auto delta = std::clamp(required / details.construction_time, 0.f, source_private);
			current_purchased.commodity_amounts[i] += delta;
			source_private -= delta;
		} else {
			auto& source_national = state.world.market_get_construction_demand(details.market, base_cost.commodity_type[i]);
			auto delta = std::clamp(required / details.construction_time, 0.f, source_national);
			current_purchased.commodity_amounts[i] += delta;
			source_national -= delta;
		}
	}
}

void populate_province_building_construction_demand(
	sys::state& state,
	dcon::province_building_construction_id construction,
	float& budget,
	float budget_limit
) {
	auto details = explain_province_building_construction(state, construction);
	if(!details.can_be_advanced) return;
	if(details.is_pop_project) return;

	assert(0 <= int32_t(details.building_type) && int32_t(details.building_type) < int32_t(economy::max_building_types));
	auto& base_cost = state.economy_definitions.building_definitions[int32_t(details.building_type)].cost;
	auto& current_purchased = state.world.province_building_construction_get_purchased_goods(construction);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required) continue;
		auto local_price = price(state, details.market, cid);
		auto can_purchase_budget = std::min(budget_limit, budget) / (local_price + 0.001f);
		auto can_purchase_construction = required / details.construction_time;
		auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
		auto satisfaction = state.world.market_get_demand_satisfaction(details.market, cid);
		budget = std::max(0.f, budget - can_purchase * local_price * satisfaction);
		register_construction_demand(state, details.market, cid, can_purchase);
	}
}

struct factory_construction_data {
	bool can_be_advanced;
	bool is_pop_project;
	bool is_upgrade;
	float construction_time;
	float cost_multiplier;
	dcon::nation_id owner;
	dcon::market_id market;
	dcon::province_id province;
	dcon::state_instance_id state_instance;
	dcon::factory_type_id building_type;
	dcon::factory_type_id refit_target;
};

factory_construction_data explain_factory_building_construction(
	sys::state& state,
	dcon::state_building_construction_id construction
) {
	auto owner = state.world.state_building_construction_get_nation(construction);
	auto local_zone = state.world.state_building_construction_get_state(construction);
	auto market = state.world.state_instance_get_market_from_local_market(local_zone);
	auto refit_target = state.world.state_building_construction_get_refit_target(construction);
	auto building_type = state.world.state_building_construction_get_type(construction);
	auto province = state.world.state_instance_get_capital(local_zone);
	auto is_pop_project = state.world.state_building_construction_get_is_pop_project(construction);
	auto is_upgrade = state.world.state_building_construction_get_is_upgrade(construction);
	factory_construction_data result = {
		.can_be_advanced = (owner && state.world.province_get_nation_from_province_control(province) == owner),
		.is_pop_project = is_pop_project,
		.is_upgrade = is_upgrade,
		.construction_time = factory_building_construction_time(state, building_type, is_upgrade),
		.cost_multiplier = factory_build_cost_multiplier(state, owner, province, is_pop_project),
		.owner = owner,
		.market = state.world.state_instance_get_market_from_local_market(local_zone),
		.province = province,
		.state_instance = local_zone,
		.building_type = building_type,
		.refit_target = refit_target
	};
	return result;
}

//handles both private and public factories
void advance_factory_construction(
	sys::state& state,
	dcon::state_building_construction_id construction
) {
	auto details = explain_factory_building_construction(state, construction);
	auto base_cost =
		details.refit_target
		? calculate_factory_refit_goods_cost(
			state, details.owner, details.state_instance, details.building_type, details.refit_target
		)
		: state.world.factory_type_get_construction_costs(details.building_type);
	auto& current_purchased = state.world.state_building_construction_get_purchased_goods(construction);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required)	continue;

		if(details.is_pop_project) {
			auto& source_private = state.world.market_get_private_construction_demand(details.market, base_cost.commodity_type[i]);
			auto delta = std::clamp(required / details.construction_time, 0.f, source_private);
			current_purchased.commodity_amounts[i] += delta;
			source_private -= delta;
		} else {
			auto& source_national = state.world.market_get_construction_demand(details.market, base_cost.commodity_type[i]);
			auto delta = std::clamp(required / details.construction_time, 0.f, source_national);
			current_purchased.commodity_amounts[i] += delta;
			source_national -= delta;
		}
	}
}

void populate_state_construction_demand(
	sys::state& state,
	dcon::state_building_construction_id construction,
	float& budget,
	float budget_limit
) {
	auto details = explain_factory_building_construction(state, construction);
	if(!details.can_be_advanced) return;
	if(details.is_pop_project) return;

	auto base_cost = details.refit_target
		? calculate_factory_refit_goods_cost(
			state, details.owner, details.state_instance, details.building_type, details.refit_target
		) : state.world.factory_type_get_construction_costs(details.building_type);
	auto& current_purchased = state.world.state_building_construction_get_purchased_goods(construction);

	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required) continue;
		auto local_price = price(state, details.market, cid);
		auto can_purchase_budget = std::min(budget_limit, budget) / (local_price + 0.001f);
		auto can_purchase_construction = required / details.construction_time;
		auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
		auto satisfaction = state.world.market_get_demand_satisfaction(details.market, cid);
		budget = std::max(0.f, budget - can_purchase * local_price * satisfaction);
		register_construction_demand(state, details.market, cid, can_purchase);
	}
}


void populate_construction_consumption(sys::state& state) {
	reset_construction_demand(state);

	static auto total_budget = state.world.nation_make_vectorizable_float_buffer();
	static auto current_budget = state.world.nation_make_vectorizable_float_buffer();
	static auto going_constructions = state.world.nation_make_vectorizable_int_buffer();

	//reset static data

	state.world.execute_serial_over_nation([&](auto ids) {
		auto base_budget = state.world.nation_get_stockpiles(ids, economy::money);
		auto construction_priority = ve::to_float(state.world.nation_get_construction_spending(ids)) / 100.f;
		current_budget.set(ids, ve::max(0.f, base_budget * construction_priority));
		total_budget.set(ids, ve::max(0.f, base_budget * construction_priority));
		going_constructions.set(ids, 0);
	});

	// count ongoing constructions
	// we need this number to limit amount of money going into individual constructions

	for(auto lc : state.world.in_province_land_construction) {
		auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(lc));
		auto owner = state.world.province_get_nation_from_province_ownership(province);
		if(owner && state.world.province_get_nation_from_province_control(province) == owner) {
			going_constructions.get(owner) += 1;
		}
	}
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(!owner || state.world.province_get_nation_from_province_control(p) != owner)
			return;
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() == rng.end())
			return;
		going_constructions.get(owner) += 1;
	});
	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		if(owner && c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control() && !c.get_is_pop_project()) {
			going_constructions.get(owner) += 1;
		}
	};
	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		if(owner && !c.get_is_pop_project()) {
			going_constructions.get(owner) += 1;
		}
	};


	for(auto lc : state.world.in_province_land_construction) {
		auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(lc));
		auto owner = state.world.province_get_nation_from_province_ownership(province);
		float& base_budget = current_budget.get(owner);
		float budget = total_budget.get(owner);
		float owners_projects = float(std::max(1, going_constructions.get(owner)));
		populate_land_unit_construction_demand(state, lc, base_budget, budget/owners_projects);
	}
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(!owner || state.world.province_get_nation_from_province_control(p) != owner)
			return;
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() == rng.end())
			return;
		auto c = *(rng.begin());
		float& base_budget = current_budget.get(owner);
		float budget_limit = total_budget.get(owner) / float(std::max(1, going_constructions.get(owner)));
		populate_naval_unit_construction_demand(state, c, base_budget, budget_limit);
	});
	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		float& base_budget = current_budget.get(owner);
		float budget_limit = total_budget.get(owner) / float(std::max(1, going_constructions.get(owner)));
		populate_province_building_construction_demand(state, c, base_budget, budget_limit);
	}
	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		float& base_budget = current_budget.get(owner);
		float budget_limit = total_budget.get(owner) / float(std::max(1, going_constructions.get(owner)));
		populate_state_construction_demand(state, c, base_budget, budget_limit);
	}
}

int32_t count_ongoing_constructions(sys::state& state, dcon::nation_id n) {
	auto count = 0;
	for(auto lc : state.world.in_province_land_construction) {
		auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(lc));
		auto owner = state.world.province_get_nation_from_province_ownership(province);
		if(owner != n) {
			continue;
		}
		if(owner && state.world.province_get_nation_from_province_control(province) == owner) {
			count++;
		}
	}
	state.world.nation_for_each_province_ownership(n, [&](auto ownership) {
		auto owner = n;
		auto p = state.world.province_ownership_get_province(ownership);

		auto local_zone = state.world.province_get_state_membership(p);
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);

		if(!owner || state.world.province_get_nation_from_province_control(p) != owner)
			return;
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() == rng.end())
			return;

		count++;
	});
	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		if(owner != n) {
			continue;
		}
		auto spending_scale = state.world.nation_get_spending_level(owner);
		auto local_zone = c.get_province().get_state_membership();
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);
		if(owner && c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control() && !c.get_is_pop_project()) {
			count++;
		}
	}
	for(auto c : state.world.in_state_building_construction) {
		auto owner = c.get_nation().id;
		if(owner != n) {
			continue;
		}
		auto spending_scale = state.world.nation_get_spending_level(owner);
		auto market = state.world.state_instance_get_market_from_local_market(c.get_state());
		if(owner && !c.get_is_pop_project()) {
			count++;
		}
	}

	return count;
}

void populate_explanation_province_construction(
	sys::state& state,
	std::vector<province_construction_spending_entry>& data,
	dcon::nation_id n,
	float& dedicated_budget,
	float& estimated_spendings,
	float budget_limit_per_project
) {
	for(auto c : state.world.in_province_building_construction) {
		auto details = explain_province_building_construction(state, c);
		if(details.owner != n) continue;
		if(!details.can_be_advanced) continue;
		if(details.is_pop_project) continue;

		assert(0 <= int32_t(details.building_type) && int32_t(details.building_type) < int32_t(economy::max_building_types));
		auto& base_cost = state.economy_definitions.building_definitions[int32_t(details.building_type)].cost;
		auto& current_purchased = c.get_purchased_goods();
		float total_cost = 0.f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid) break;
			auto current = current_purchased.commodity_amounts[i];
			auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
			if(current >= required) continue;
			auto local_price = price(state, details.market, cid);
			auto can_purchase_budget = std::min(budget_limit_per_project, dedicated_budget) / (local_price + 0.001f);
			auto can_purchase_construction = required / details.construction_time;
			auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
			auto satisfaction = state.world.market_get_demand_satisfaction(details.market, cid);
			auto cost = std::min(dedicated_budget, can_purchase * satisfaction * local_price);
			dedicated_budget -= cost;
			estimated_spendings += cost;
			total_cost += cost;
		}
		province_construction_spending_entry to_add{
			.construction = c.id, .spending = total_cost
		};
		data.push_back(to_add);
	}
}
void populate_explanation_state_construction(
	sys::state& state,
	std::vector<state_construction_spending_entry>& data,
	dcon::nation_id n,
	float& dedicated_budget,
	float& estimated_spendings,
	float budget_limit_per_project
) {
	for(auto c : state.world.in_state_building_construction) {
		auto details = explain_factory_building_construction(state, c);
		if(details.owner != n) continue;
		if(!details.can_be_advanced) continue;
		if(details.is_pop_project) continue;
		auto& base_cost = state.world.factory_type_get_construction_costs(details.building_type);
		auto& current_purchased = c.get_purchased_goods();
		float total_cost = 0.f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid) break;
			auto current = current_purchased.commodity_amounts[i];
			auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
			if(current >= required) continue;
			auto local_price = price(state, details.market, cid);
			auto can_purchase_budget = std::min(budget_limit_per_project, dedicated_budget) / (local_price + 0.001f);
			auto can_purchase_construction = required / details.construction_time;
			auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
			auto satisfaction = state.world.market_get_demand_satisfaction(details.market, cid);
			auto cost = std::min(dedicated_budget, can_purchase * satisfaction * local_price);
			dedicated_budget -= cost;
			estimated_spendings += cost;
			total_cost += cost;
		}
		state_construction_spending_entry to_add{
			.construction = c.id, .spending = total_cost
		};
		data.push_back(to_add);
	}
}

void populate_explanation_land_construction(
	sys::state& state,
	std::vector<province_land_construction_spending_entry>& data,
	dcon::nation_id n,
	float& dedicated_budget,
	float& estimated_spendings,
	float budget_limit_per_project
) {
	for(auto lc : state.world.in_province_land_construction) {
		auto details = explain_land_unit_construction(state, lc);
		if(details.owner != n) continue;
		if(!details.can_be_advanced) continue;
		auto& base_cost = state.military_definitions.unit_base_definitions[details.unit_type].build_cost;
		auto& current_purchased	= state.world.province_land_construction_get_purchased_goods(lc);
		float total_cost = 0.f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid) break;
			auto current = current_purchased.commodity_amounts[i];
			auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
			if(current >= required) continue;
			auto local_price = price(state, details.market, cid);
			auto actual_budget = std::min(budget_limit_per_project, dedicated_budget);
			auto can_purchase_budget = actual_budget / (local_price + 0.001f);
			auto can_purchase_construction = required / details.construction_time;
			auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
			auto satisfaction = state.world.market_get_demand_satisfaction(details.market, cid);
			auto cost = std::min(dedicated_budget, can_purchase * satisfaction * local_price);
			dedicated_budget -= cost;
			estimated_spendings += cost;
			total_cost += cost;
		}
		province_land_construction_spending_entry to_add{
			.construction = lc, .spending = total_cost
		};
		data.push_back(to_add);
	}
}
void populate_explanation_naval_construction(
	sys::state& state,
	std::vector<province_naval_construction_spending_entry>& data,
	dcon::nation_id n,
	float& dedicated_budget,
	float& estimated_spendings,
	float budget_limit_per_project
) {
	state.world.nation_for_each_province_ownership(n, [&](auto ownership) {
		auto p = state.world.province_ownership_get_province(ownership);
		auto rng = state.world.province_get_province_naval_construction(p);
		if(rng.begin() == rng.end()) return;
		auto c = *(rng.begin());
		auto details = explain_naval_unit_construction(state, c);
		if(!details.can_be_advanced) return;
		auto& base_cost = state.military_definitions.unit_base_definitions[details.unit_type].build_cost;
		auto& current_purchased = state.world.province_naval_construction_get_purchased_goods(c);
		float total_cost = 0.f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid) break;
			auto current = current_purchased.commodity_amounts[i];
			auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
			if(current >= required) continue;
			auto local_price = price(state, details.market, cid);
			auto actual_budget = std::min(budget_limit_per_project, dedicated_budget);
			auto can_purchase_budget = actual_budget / (local_price + 0.001f);
			auto can_purchase_construction = required / details.construction_time;
			auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
			auto satisfaction = state.world.market_get_demand_satisfaction(details.market, cid);
			auto cost = std::min(dedicated_budget, can_purchase * satisfaction * local_price);
			dedicated_budget -= cost;
			estimated_spendings += cost;
			total_cost += cost;
		}
		province_naval_construction_spending_entry to_add{
			.construction = c, .spending = total_cost
		};
		data.push_back(to_add);
	});
}

construction_spending_explanation explain_construction_spending(
	sys::state& state,
	dcon::nation_id n,
	float dedicated_budget
) {
	construction_spending_explanation result = {
		.ongoing_projects = count_ongoing_constructions(state, n),
		.budget_limit_per_project = 0.f,
		.estimated_spendings = 0.f,
		.province_buildings = { },
		.factories = { },
		.land_units = { },
		.naval_units = { },
	};

	if(result.ongoing_projects == 0) return result;
	result.budget_limit_per_project = dedicated_budget / float(result.ongoing_projects);

	populate_explanation_land_construction(
		state, result.land_units,
		n, dedicated_budget, result.estimated_spendings, result.budget_limit_per_project
	);
	populate_explanation_naval_construction(
		state, result.naval_units,
		n, dedicated_budget, result.estimated_spendings, result.budget_limit_per_project
	);
	populate_explanation_province_construction(
		state, result.province_buildings,
		n, dedicated_budget, result.estimated_spendings, result.budget_limit_per_project
	);
	populate_explanation_state_construction(
		state, result.factories,
		n, dedicated_budget, result.estimated_spendings, result.budget_limit_per_project
	);

	return result;
}

// TODO: write a lighter version which doesn't include all the current projects and calculates only costs
float estimate_construction_spending_from_budget(sys::state& state, dcon::nation_id n, float current_budget) {
	return explain_construction_spending(state, n, current_budget).estimated_spendings;
}

construction_spending_explanation explain_construction_spending_now(sys::state& state, dcon::nation_id n) {
	auto treasury = state.world.nation_get_stockpiles(n, economy::money);
	auto priority = float(state.world.nation_get_construction_spending(n)) / 100.f;
	auto current_budget = std::max(0.f, treasury * priority);
	return explain_construction_spending(state, n, current_budget);
}

float estimate_construction_spending(sys::state& state, dcon::nation_id n) {
	auto treasury = state.world.nation_get_stockpiles(n, economy::money);
	auto priority = float(state.world.nation_get_construction_spending(n)) / 100.f;
	auto current_budget = std::max(0.f, treasury * priority);
	return estimate_construction_spending_from_budget(state, n, current_budget);
}

float estimate_private_construction_spendings(sys::state& state, dcon::nation_id nid) {
	float total = 0.f;

	for(auto c : state.world.nation_get_province_building_construction(nid)) {
		auto market = state.world.state_instance_get_market_from_local_market(
			c.get_province().get_state_membership()
		);

		// Rationale for not checking building type: Its an invalid state; should not occur under normal circumstances
		if(nid == c.get_province().get_nation_from_province_control() && c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			assert(0 <= int32_t(t) && int32_t(t) < int32_t(economy::max_building_types));
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_non_factory_construction_time_modifier(state) *
				float(state.economy_definitions.building_definitions[int32_t(t)].time);
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
						total +=
						base_cost.commodity_amounts[i]
						* price(state, market, base_cost.commodity_type[i])
						/ construction_time;
				} else {
					break;
				}
			}
		}
	}

	for(auto c : state.world.nation_get_state_building_construction(nid)) {
		auto market = state.world.state_instance_get_market_from_local_market(c.get_state());
		if(c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) *
				float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = (state.world.nation_get_modifier_values(nid, sys::national_mod_offsets::factory_cost) + 1.0f) * std::max(0.1f, state.world.nation_get_modifier_values(nid, sys::national_mod_offsets::factory_owner_cost));

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod)
						total +=
						base_cost.commodity_amounts[i]
						* price(state, market, base_cost.commodity_type[i])
						* factory_mod
						/ construction_time;
				} else {
					break;
				}
			}
		}
	}

	return total;
}

void populate_province_building_construction_private_demand(
	sys::state& state,
	dcon::province_building_construction_id construction
) {
	auto details = explain_province_building_construction(state, construction);
	// Rationale for not checking building type: Its an invalid state; should not occur under normal circumstances
	if(!details.can_be_advanced) return;
	if(!details.is_pop_project) return;

	assert(0 <= int32_t(details.building_type) && int32_t(details.building_type) < int32_t(economy::max_building_types));
	auto& base_cost = state.economy_definitions.building_definitions[int32_t(details.building_type)].cost;
	auto& current_purchased = state.world.province_building_construction_get_purchased_goods(construction);
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required) continue;
		state.world.market_get_private_construction_demand(details.market, cid) += required / details.construction_time;
	}
}

void populate_state_construction_private_demand(
	sys::state& state,
	dcon::state_building_construction_id construction
) {
	auto details = explain_factory_building_construction(state, construction);
	if(!details.can_be_advanced) return;
	if(!details.is_pop_project)	return;
	auto base_cost = details.refit_target
		? calculate_factory_refit_goods_cost(
			state, details.owner, details.state_instance, details.building_type, details.refit_target
		) : state.world.factory_type_get_construction_costs(details.building_type);
	auto& current_purchased = state.world.state_building_construction_get_purchased_goods(construction);
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required) continue;
		state.world.market_get_private_construction_demand(details.market, cid) += required / details.construction_time;
	}
}

void populate_private_construction_consumption(sys::state& state) {
	reset_private_construction_demand(state);
	for(auto c : state.world.in_province_building_construction) {
		populate_province_building_construction_private_demand(state, c);
	}
	for(auto c : state.world.in_state_building_construction) {
		populate_state_construction_private_demand(state, c);
	}
}

// this function handles refund logic for construction demand:
// during update of national payments
// nation pays for all generated demand
// even if there are not enough goods on the market
// if nation was unable to buy out all demanded goods due to low amount of actually sold goods,
// it receives the refund while construction demand is multiplied by actual demand satisfaction
// after usage of this function, construction demand actually becomes a stockpile for construction projects
void refund_construction_demand(sys::state& state, dcon::nation_id n, float total_spent_on_construction) {
	uint32_t total_commodities = state.world.commodity_size();
	float p_spending = state.world.nation_get_private_investment_effective_fraction(n);
	float refund_amount = 0.0f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
			auto& nat_demand = state.world.market_get_construction_demand(market, c);
			auto com_price = price(state, market, c);
			auto d_sat = state.world.market_get_demand_satisfaction(market, c);
			refund_amount +=
				nat_demand
				* (1.0f - d_sat)
				* com_price;
			assert(refund_amount >= 0.0f);

			nat_demand *= d_sat;
			state.world.market_get_private_construction_demand(market, c) *= p_spending * d_sat;
		}
	});
	assert(refund_amount >= 0.0f);
	state.world.nation_get_stockpiles(n, economy::money) += std::min(refund_amount, total_spent_on_construction);
}

void advance_construction(sys::state& state, dcon::nation_id n, float total_spent_on_construction) {
	refund_construction_demand(state, n, total_spent_on_construction);
	for(auto p : state.world.nation_get_province_ownership(n)) {
		if(p.get_province().get_nation_from_province_control() != n)
			continue;
		for(auto pops : p.get_province().get_pop_location()) {
			auto rng = pops.get_pop().get_province_land_construction();
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());
				advance_land_unit_construction(state, c);
				break; // only advance one construction per province
			}
		}
		{
			auto rng = p.get_province().get_province_naval_construction();
			if(rng.begin() != rng.end()) {
				auto c = *(rng.begin());
				advance_naval_unit_construction(state, c);
			}
		}
	}
	for(auto c : state.world.nation_get_province_building_construction(n)) {
		if(c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control()) {
			advance_province_building_construction(state, c);
		}
	}
	for(auto c : state.world.nation_get_state_building_construction(n)) {
		advance_factory_construction(state, c);
	}
}

// this function partly emulates demand generated by nations
void emulate_construction_demand(sys::state& state, dcon::nation_id n) {
	// phase 1:
	// simulate spending on construction of units
	// useful to help the game start with some production of artillery and small arms

	float income_to_build_units = 10'000.f;

	if(state.world.nation_get_owned_province_count(n) == 0) {
		return;
	}

	// we build infantry and artillery:
	auto infantry = state.military_definitions.infantry;
	auto artillery = state.military_definitions.artillery;

	auto& infantry_def = state.military_definitions.unit_base_definitions[infantry];
	auto& artillery_def = state.military_definitions.unit_base_definitions[artillery];

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		float daily_cost = 0.f;

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(infantry_def.build_cost.commodity_type[i]) {
				auto p = price(state, market, infantry_def.build_cost.commodity_type[i]);
				daily_cost += infantry_def.build_cost.commodity_amounts[i] / infantry_def.build_time * p;
			} else {
				break;
			}
		}
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(artillery_def.build_cost.commodity_type[i]) {
				auto p = price(state, market, artillery_def.build_cost.commodity_type[i]);
				daily_cost += artillery_def.build_cost.commodity_amounts[i] / artillery_def.build_time * p;
			} else {
				break;
			}
		}

		auto pairs_to_build = std::max(0.f, income_to_build_units / (daily_cost + 1.f) - 0.1f);

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(infantry_def.build_cost.commodity_type[i]) {
				auto daily_amount = infantry_def.build_cost.commodity_amounts[i] / infantry_def.build_time;
				register_demand(state, market, infantry_def.build_cost.commodity_type[i], daily_amount * pairs_to_build, economy_reason::construction);
				state.world.market_get_stockpile(market, infantry_def.build_cost.commodity_type[i]) += daily_amount * pairs_to_build * 0.05f;
			} else {
				break;
			}
		}
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(artillery_def.build_cost.commodity_type[i]) {
				auto daily_amount = artillery_def.build_cost.commodity_amounts[i] / artillery_def.build_time;
				register_demand(state, market, artillery_def.build_cost.commodity_type[i], daily_amount * pairs_to_build, economy_reason::construction);
				state.world.market_get_stockpile(market, artillery_def.build_cost.commodity_type[i]) += daily_amount * pairs_to_build * 0.05f;
			} else {
				break;
			}
		}
	});

	// phase 2:
	// simulate spending on construction of factories
	// helps with machine tools and cement

	float income_to_build_factories = 100'000.f;

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		// iterate over all factory types available from the start and find "average" daily construction cost:
		float sum_of_build_times = 0.f;
		float cost_factory_set = 0.f;
		float count = 0.f;

		state.world.for_each_factory_type([&](dcon::factory_type_id factory_type) {
			if(!state.world.factory_type_get_is_available_from_start(factory_type)) {
				return;
			}

			auto build_time = state.world.factory_type_get_construction_time(factory_type);
			auto& build_cost = state.world.factory_type_get_construction_costs(factory_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto pr = price(state, market, build_cost.commodity_type[i]);
					cost_factory_set += pr * build_cost.commodity_amounts[i] / build_time;
				} else {
					break;
				}
			}
			count++;
		});


		// calculate amount of factory sets we are building:
		auto num_of_factory_sets = std::max(0.f, income_to_build_factories / (cost_factory_set + 1.f) - 0.1f);

		// emulate construction demand
		state.world.for_each_factory_type([&](dcon::factory_type_id factory_type) {
			if(!state.world.factory_type_get_is_available_from_start(factory_type)) {
				return;
			}

			auto build_time = state.world.factory_type_get_construction_time(factory_type);
			auto& build_cost = state.world.factory_type_get_construction_costs(factory_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto amount = build_cost.commodity_amounts[i];
					register_demand(
						state,
						market,
						build_cost.commodity_type[i], amount / build_time * num_of_factory_sets,
						economy_reason::construction
					);
					state.world.market_get_stockpile(market, build_cost.commodity_type[i]) += amount / build_time * num_of_factory_sets / 100.f;
				} else {
					break;
				}
			}
			count++;
		});
	});
}
}
