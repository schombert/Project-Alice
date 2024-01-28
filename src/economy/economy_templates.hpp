#pragma once
#include "system_state.hpp"

namespace economy {

template<typename F>
void for_each_new_factory(sys::state& state, dcon::state_instance_id s, F&& func) {
	for(auto st_con : state.world.state_instance_get_state_building_construction(s)) {
		if(!st_con.get_is_upgrade()) {
			float admin_eff = state.world.nation_get_administrative_efficiency(st_con.get_nation());
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
void for_each_upgraded_factory(sys::state& state, dcon::state_instance_id s, F&& func) {
	for(auto st_con : state.world.state_instance_get_state_building_construction(s)) {
		if(st_con.get_is_upgrade()) {
			float admin_eff = state.world.nation_get_administrative_efficiency(st_con.get_nation());
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

			func(upgraded_factory{total > 0.0f ? purchased / total : 0.0f, st_con.get_type().id});
		}
	}
}

} // namespace economy
