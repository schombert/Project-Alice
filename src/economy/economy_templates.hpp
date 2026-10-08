#pragma once
#include "system_state.hpp"
#include "economy_constants.hpp"
#include "adaptive_ve.hpp"
#include "economy_common_api_containers.hpp"

namespace economy {

template<typename F>
void for_each_new_factory(sys::state& state, dcon::province_id s, F&& func) {
	for(auto st_con : state.world.province_get_factory_construction(s)) {
		if(!st_con.get_is_upgrade() && !st_con.get_refit_target()) {
			float admin_eff = state.world.province_get_control_ratio(st_con.get_province());
			float factory_mod = state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_cost) + 1.0f;
			float pop_factory_mod = std::max(0.1f, state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_owner_cost));
			float admin_cost_factor = (st_con.get_is_pop_project() ? pop_factory_mod : (2.0f - admin_eff)) * factory_mod;

			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(st_con.get_type());

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i] * admin_cost_factor;
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			func(new_factory{total > 0.0f ? purchased / total : 0.0f, st_con.get_type().id});
		}
	}
}

template<typename F>
void for_each_upgraded_factory(sys::state& state, dcon::province_id s, F&& func) {
	for(auto st_con : state.world.province_get_factory_construction(s)) {
		if(st_con.get_is_upgrade() || st_con.get_refit_target()) {
			float admin_eff = state.world.province_get_control_ratio(st_con.get_province());
			float factory_mod = state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_cost) + 1.0f;
			float pop_factory_mod = std::max(0.1f, state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_owner_cost));
			float admin_cost_factor = (st_con.get_is_pop_project() ? pop_factory_mod : (2.0f - admin_eff)) * factory_mod;
			float refit_discount = (st_con.get_refit_target()) ? state.defines.alice_factory_refit_cost_modifier : 1.0f;

			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(st_con.get_type());

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i] * admin_cost_factor * refit_discount;
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			func(upgraded_factory{total > 0.0f ? purchased / total : 0.0f, st_con.get_type().id, st_con.get_refit_target().id});
		}
	}
}


// Iterates over each commodity but skips money
template<typename F>
void for_each_commodity_no_money(const sys::state& state, F&& func) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		func(cid);
	}
}
template<typename F>
void for_each_unit_construction(const sys::state& state, F&& func) {
	state.world.for_each_province_land_construction(func);
	state.world.for_each_province_naval_construction(func);
}
template<typename F>
void ve_for_each_unit_construction(const sys::state& state, F&& func) {
	state.world.execute_serial_over_province_land_construction(func);
	state.world.execute_serial_over_province_naval_construction(func);
}

template<typename F>
void parallel_for_each_unit_construction(const sys::state& state, F&& func) {
	concurrency::parallel_for(uint32_t(0), state.world.province_land_construction_size(), [&](uint32_t i) {
		dcon::province_land_construction_id construction = dcon::province_land_construction_id{ dcon::province_land_construction_id::value_base_t(i) };
		if(state.world.province_land_construction_is_valid(construction)) {
			func(construction);
		}
	});
	concurrency::parallel_for(uint32_t(0), state.world.province_naval_construction_size(), [&](uint32_t i) {
		dcon::province_naval_construction_id construction = dcon::province_naval_construction_id{ dcon::province_naval_construction_id::value_base_t(i) };
		if(state.world.province_naval_construction_is_valid(construction)) {
			func(construction);
		}
	});
}

template<typename F>
void ve_parallel_for_each_unit_construction(const sys::state& state, F&& func) {
	state.world.execute_parallel_over_province_land_construction(func);
	state.world.execute_parallel_over_province_naval_construction(func);
}


template<typename F>
void for_each_building_construction(const sys::state& state, F&& func) {
	state.world.for_each_factory_construction(func);
	state.world.for_each_province_building_construction(func);
}
template<typename F>
void ve_for_each_building_construction(const sys::state& state, F&& func) {
	state.world.execute_serial_over_factory_construction(func);
	state.world.execute_serial_over_province_building_construction(func);
}

template<typename F>
void parallel_for_each_building_construction(const sys::state& state, F&& func) {
	concurrency::parallel_for(uint32_t(0), state.world.factory_construction_size(), [&](uint32_t i) {
		dcon::factory_construction_id construction = dcon::factory_construction_id{ dcon::factory_construction_id::value_base_t(i) };
		if(state.world.factory_construction_is_valid(construction)) {
			func(construction);
		}
	});
	concurrency::parallel_for(uint32_t(0), state.world.province_building_construction_size(), [&](uint32_t i) {
		dcon::province_building_construction_id construction = dcon::province_building_construction_id{ dcon::province_building_construction_id::value_base_t(i) };
		if(state.world.province_building_construction_is_valid(construction)) {
			func(construction);
		}
	});
}

template<typename F>
void ve_parallel_for_each_building_construction(const sys::state& state, F&& func) {
	state.world.execute_parallel_over_factory_construction(func);
	state.world.execute_parallel_over_province_building_construction(func);
}


template<typename F>
void for_each_construction(const sys::state& state, F&& func) {
	for_each_unit_construction(state, func);
	for_each_building_construction(state, func);
}
template<typename F>
void ve_for_each_construction(const sys::state& state, F&& func) {
	ve_for_each_unit_construction(state, func);
	ve_for_each_building_construction(state, func);
}

template<typename F>
void parallel_for_each_construction(const sys::state& state, F&& func) {
	parallel_for_each_unit_construction(state, func);
	parallel_for_each_building_construction(state, func);
}

template<typename F>
void ve_parallel_for_each_construction(const sys::state& state, F&& func) {
	ve_parallel_for_each_unit_construction(state, func);
	ve_parallel_for_each_building_construction(state, func);
}

template<price_estimation price_est, concepts::any_dcon_id_type<dcon::market_id> market_type, concepts::regular_or_ve_value_type<float> float_type>
auto get_estimated_state_stockpile_purchase_price(const sys::state& state, market_type market, dcon::commodity_id com_id, float_type goods_desired) {
	auto price = state.world.market_get_price(market, com_id);
	auto percentage_weight = state.world.market_get_government_stockpile_demand_weights(market, com_id);
	if constexpr(price_est == price_estimation::theoretical_max) {
		return price * goods_desired * percentage_weight;
	} else if constexpr(price_est == price_estimation::capped_by_availability) {
		return price * goods_desired * percentage_weight * state.world.market_get_actual_probability_to_buy(market, com_id);
	}
}

template<price_estimation price_est, typename buf_getter>
requires std::invocable<buf_getter, dcon::unit_supply_commodity_id> || std::invocable<buf_getter, dcon::unit_build_commodity_id> || std::invocable<buf_getter, dcon::unit_supply_and_build_commodity_id> || std::invocable<buf_getter, dcon::commodity_id>
float get_estimated_stockpile_total_purchase_price(const sys::state& state, dcon::nation_id for_nation, buf_getter&& getter_func) {
	auto for_each_func = [&]<typename F>(F && func) {
		if constexpr(std::is_invocable_r_v<float, buf_getter, dcon::unit_supply_commodity_id>) {
			state.world.for_each_unit_supply_commodity(func);
		} else if constexpr(std::is_invocable_r_v<float, buf_getter, dcon::unit_build_commodity_id>) {
			state.world.for_each_unit_build_commodity(func);
		} else if constexpr(std::is_invocable_r_v<float, buf_getter, dcon::unit_supply_and_build_commodity_id>) {
			state.world.for_each_unit_supply_and_build_commodity(func);
		} else if constexpr(std::is_invocable_r_v<float, buf_getter, dcon::commodity_id>) {
			economy::for_each_commodity_no_money(state, func);
		} else {
			static_assert(false, "Unsupported functor signature");
		}
	};
	float total_expected_price = 0.0f;
	for_each_func([&](auto com_id) {
		dcon::commodity_id base_com_id = [&]() {
			if constexpr(std::is_same_v<decltype(com_id), dcon::commodity_id>) {
				return com_id; // We already have the base commodity
			} else {
				return unit_commodity_get_base_commodity(state, com_id);
			}
			}();
		state.world.nation_for_each_state_control(for_nation, [&](dcon::state_control_id sc) {
			auto state_inst = state.world.state_control_get_state(sc);
			auto market = state.world.state_instance_get_market_from_local_market(state_inst);
			float goods_desired = getter_func(com_id);
			total_expected_price += get_estimated_state_stockpile_purchase_price<price_est>(state, market, base_com_id, goods_desired);
		});
	});
	return total_expected_price;
}
// How many goods do we need (or have in excess) in order to be at exactly the stockpile target? If return is negative, that means we excess commodities over the target. Positive means we need more commodities to reach the target
template<concepts::any_dcon_id_type<dcon::nation_id> nation_type>
auto government_stockpile_target_balance(const sys::state& state, nation_type nation, dcon::commodity_id com_id) {
	return state.world.nation_get_stockpile_targets(nation, com_id) - state.world.nation_get_total_stockpiles(nation, com_id);
}

// How many goods do we need in order to be at exactly the stockpile target? If we are at the target OR have goods in excess, then the result is 0
template<concepts::any_dcon_id_type<dcon::nation_id> nation_type>
auto government_stockpile_desired_commodity_amount(const sys::state& state, nation_type nation, dcon::commodity_id com_id) {
	if constexpr(std::is_same_v<nation_type, dcon::nation_id>) {
		return std::max(government_stockpile_target_balance(state, nation, com_id), 0.0f);
	} else {
		return ve::max(government_stockpile_target_balance(state, nation, com_id), 0.0f);
	}
}

// Accumulates all construction good requirements via the "acc_func" functor of a given construction. Already-fufilled good requirements are subtracted and result passed as the "required_amount" argument, whereas the "total_required" argument is the amount required is the amount required if the construction has not accumulated any goods yet
// Functor signatures:
// (dcon::commodity_id commodity, float required_amount),
// (dcon::commodity_id commodity, float required_amount, float total_required),
// (uint32_t set_index, float required_amount),
// (uint32_t set_index, float required_amount, float total_required)
template<concepts::construction_type construction_type, typename FAccumulate>
void accumulate_construction_daily_goods_requirements(const sys::state& state, construction_type c, FAccumulate& acc_func) {
	const economy::commodity_set actual_cost = construction_get_actual_build_cost(state, c);

	const economy::commodity_set& currently_fufilled = construction_get_purchased_goods(state, c);
	for(uint32_t i = 0; i < actual_cost.set_size; i++) {
		dcon::commodity_id cid = actual_cost.commodity_type[i];
		if(cid) {
			float total_required = actual_cost.commodity_amounts[i];
			float fufilled_amount = currently_fufilled.commodity_amounts[i];
			float required = std::max(total_required - fufilled_amount, 0.0f);
			if constexpr(std::is_invocable_r_v<void, FAccumulate, dcon::commodity_id, float>) {
				acc_func(cid, required);
			}
			else if constexpr(std::is_invocable_r_v<void, FAccumulate, dcon::commodity_id, float, float>) {
				acc_func(cid, required, total_required);
			}
			else if constexpr(std::is_invocable_r_v<void, FAccumulate, uint32_t, float>) {
				acc_func(i, required);
			}
			else if constexpr(std::is_invocable_r_v<void, FAccumulate, uint32_t, float, float>) {
				acc_func(i, required, total_required);
			}
			else {
				static_assert(false, "Invalid functor signature");
			}
		} else {
			break;
		}
	}
}


} // namespace economy
