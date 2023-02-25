#include "dcon_generated.hpp"
#include "culture.hpp"
#include "system_state.hpp"

namespace culture {

void repopulate_technology_effects(sys::state& state) {
	state.world.for_each_technology([&](dcon::technology_id t_id) {
		auto tech_id = fatten(state.world, t_id);

		// apply modifiers from active technologies
		auto tech_mod = tech_id.get_modifier();
		if(tech_mod) {
			auto& tech_nat_values = tech_mod.get_national_values();
			for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
				if(tech_nat_values.offsets[i] == 0)
					break; // no more modifier values attached to this tech

				state.world.execute_serial_over_nation([&,
					fixed_offset = tech_nat_values.get_offet_at_index(i) - sys::provincial_mod_offsets::count,
					modifier_amount = tech_nat_values.values[i]
				](auto nation_indices) {
					auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
					auto old_mod_value = state.world.nation_get_static_modifier_values(nation_indices, fixed_offset);
					state.world.nation_set_static_modifier_values(nation_indices, fixed_offset,
						ve::select(has_tech_mask, old_mod_value + modifier_amount, old_mod_value));
				});
			}
		}

		if(tech_id.get_increase_railroad()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
				auto old_rr_value = state.world.nation_get_max_railroad_level(nation_indices);
				state.world.nation_set_max_railroad_level(nation_indices,
					ve::select(has_tech_mask, old_rr_value + 1, old_rr_value));
			});
		}
		if(tech_id.get_increase_fort()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
				auto old_fort_value = state.world.nation_get_max_fort_level(nation_indices);
				state.world.nation_set_max_fort_level(nation_indices,
					ve::select(has_tech_mask, old_fort_value + 1, old_fort_value));
			});
		}
		if(tech_id.get_increase_naval_base()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
				auto old_nb_value = state.world.nation_get_max_naval_base_level(nation_indices);
				state.world.nation_set_max_naval_base_level(nation_indices,
					ve::select(has_tech_mask, old_nb_value + 1, old_nb_value));
			});
		}

		state.world.for_each_factory_type([&](dcon::factory_type_id id){
			if(tech_id.get_activate_building(id)) {
				state.world.execute_serial_over_nation([&](auto nation_indices) {
					auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
					auto old_value = state.world.nation_get_active_building(nation_indices, id);
					state.world.nation_set_active_building(nation_indices, id, old_value | has_tech_mask);
				});
			}
		});
		for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
			dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
			if(tech_id.get_activate_unit(uid)) {
				state.world.execute_serial_over_nation([&](auto nation_indices) {
					auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
					auto old_value = state.world.nation_get_active_unit(nation_indices, uid);
					state.world.nation_set_active_unit(nation_indices, uid, old_value | has_tech_mask);
				});
			}
		}

		for(auto cmod : tech_id.get_rgo_goods_output()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
				auto old_value = state.world.nation_get_rgo_goods_output(nation_indices, cmod.type);
				state.world.nation_set_rgo_goods_output(nation_indices, cmod.type,
					ve::select(has_tech_mask, old_value + cmod.amount, old_value));
			});
		}
		for(auto cmod : tech_id.get_factory_goods_output()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
				auto old_value = state.world.nation_get_factory_goods_output(nation_indices, cmod.type);
				state.world.nation_set_factory_goods_output(nation_indices, cmod.type,
					ve::select(has_tech_mask, old_value + cmod.amount, old_value));
			});
		}
		for(auto cmod : tech_id.get_rgo_size()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
				auto old_value = state.world.nation_get_rgo_size(nation_indices, cmod.type);
				state.world.nation_set_rgo_size(nation_indices, cmod.type,
					ve::select(has_tech_mask, old_value + cmod.amount, old_value));
			});
		}
		for(auto& umod : tech_id.get_modified_units()) {
			state.world.for_each_nation([&](dcon::nation_id nid) {
				if(state.world.nation_get_active_technologies(nid, t_id)) {
					state.world.nation_get_unit_stats(nid, umod.type) += umod;
				}
			});
		}
	});
}

void repopulate_invention_effects(sys::state& state) {
	state.world.for_each_invention([&](dcon::invention_id i_id) {
		auto inv_id = fatten(state.world, i_id);

		// apply modifiers from active inventions
		auto inv_mod = inv_id.get_modifier();
		if(inv_mod) {
			auto& inv_nat_values = inv_mod.get_national_values();
			for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
				if(inv_nat_values.offsets[i] == 0)
					break; // no more modifier values attached to this invention

				state.world.execute_serial_over_nation([&,
					fixed_offset = inv_nat_values.get_offet_at_index(i) - sys::provincial_mod_offsets::count,
					modifier_amount = inv_nat_values.values[i]
				](auto nation_indices) {
					auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
					auto old_mod_value = state.world.nation_get_static_modifier_values(nation_indices, fixed_offset);
					state.world.nation_set_static_modifier_values(nation_indices, fixed_offset,
						ve::select(has_inv_mask, old_mod_value + modifier_amount, old_mod_value));
				});
			}
		}

		if(inv_id.get_enable_gas_attack()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
				auto old_value = state.world.nation_get_has_gas_attack(nation_indices);
				state.world.nation_set_has_gas_attack(nation_indices, old_value | has_inv_mask);
			});
		}
		if(inv_id.get_enable_gas_defence()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
				auto old_value = state.world.nation_get_has_gas_defence(nation_indices);
				state.world.nation_set_has_gas_defence(nation_indices, old_value | has_inv_mask);
			});
		}

		state.world.for_each_factory_type([&](dcon::factory_type_id id) {
			if(inv_id.get_activate_building(id)) {
				state.world.execute_serial_over_nation([&](auto nation_indices) {
					auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
					auto old_value = state.world.nation_get_active_building(nation_indices, id);
					state.world.nation_set_active_building(nation_indices, id, old_value | has_inv_mask);
				});
			}
		});
		for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
			dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
			if(inv_id.get_activate_unit(uid)) {
				state.world.execute_serial_over_nation([&](auto nation_indices) {
					auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
					auto old_value = state.world.nation_get_active_unit(nation_indices, uid);
					state.world.nation_set_active_unit(nation_indices, uid, old_value | has_inv_mask);
				});
			}
		}
		for(uint32_t i = 0; i < state.culture_definitions.crimes.size(); ++i) {
			dcon::crime_id uid = dcon::crime_id{ dcon::crime_id::value_base_t(i) };
			if(inv_id.get_activate_crime(uid)) {
				state.world.execute_serial_over_nation([&](auto nation_indices) {
					auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
					auto old_value = state.world.nation_get_active_crime(nation_indices, uid);
					state.world.nation_set_active_crime(nation_indices, uid, old_value | has_inv_mask);
				});
			}
		}

		for(auto cmod : inv_id.get_rgo_goods_output()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
				auto old_value = state.world.nation_get_rgo_goods_output(nation_indices, cmod.type);
				state.world.nation_set_rgo_goods_output(nation_indices, cmod.type,
					ve::select(has_inv_mask, old_value + cmod.amount, old_value));
			});
		}
		for(auto cmod : inv_id.get_factory_goods_output()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
				auto old_value = state.world.nation_get_factory_goods_output(nation_indices, cmod.type);
				state.world.nation_set_factory_goods_output(nation_indices, cmod.type,
					ve::select(has_inv_mask, old_value + cmod.amount, old_value));
			});
		}
		for(auto cmod : inv_id.get_factory_goods_throughput()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
				auto old_value = state.world.nation_get_factory_goods_throughput(nation_indices, cmod.type);
				state.world.nation_set_factory_goods_throughput(nation_indices, cmod.type,
					ve::select(has_inv_mask, old_value + cmod.amount, old_value));
			});
		}
		for(auto cmod : inv_id.get_rebel_org()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
				auto old_value = state.world.nation_get_rebel_org_modifier(nation_indices, cmod.type);
				state.world.nation_set_rebel_org_modifier(nation_indices, cmod.type,
					ve::select(has_inv_mask, old_value + cmod.amount, old_value));
			});
		}
		for(auto& umod : inv_id.get_modified_units()) {
			state.world.for_each_nation([&](dcon::nation_id nid) {
				if(state.world.nation_get_active_inventions(nid, i_id)) {
					state.world.nation_get_unit_stats(nid, umod.type) += umod;
				}
			});
		}
	});
}

void apply_technology(sys::state& state, dcon::nation_id target_nation, dcon::technology_id t_id) {
	auto tech_id = fatten(state.world, t_id);

	state.world.nation_set_active_technologies(target_nation, t_id, true);

	auto tech_mod = tech_id.get_modifier();
	if(tech_mod) {
		auto& tech_nat_values = tech_mod.get_national_values();
		for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
			if(tech_nat_values.offsets[i] == 0)
				break; // no more modifier values attached to this tech

			auto fixed_offset = tech_nat_values.get_offet_at_index(i) - sys::provincial_mod_offsets::count;
			auto modifier_amount = tech_nat_values.values[i];

			state.world.nation_get_static_modifier_values(target_nation, fixed_offset) += modifier_amount;
		}
	}

	if(tech_id.get_increase_railroad()) {
		state.world.nation_get_max_railroad_level(target_nation) += 1;
	}
	if(tech_id.get_increase_fort()) {
		state.world.nation_get_max_fort_level(target_nation) += 1;
	}
	if(tech_id.get_increase_naval_base()) {
		state.world.nation_get_max_naval_base_level(target_nation) += 1;
	}

	state.world.for_each_factory_type([&](dcon::factory_type_id id) {
		if(tech_id.get_activate_building(id)) {
			state.world.nation_set_active_building(target_nation, id, true);
		}
	});
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		if(tech_id.get_activate_unit(uid)) {
			state.world.nation_set_active_unit(target_nation, uid, true);
		}
	}

	for(auto cmod : tech_id.get_rgo_goods_output()) {
		state.world.nation_get_rgo_goods_output(target_nation, cmod.type) += cmod.amount;
	}
	for(auto cmod : tech_id.get_factory_goods_output()) {
		state.world.nation_get_factory_goods_output(target_nation, cmod.type) += cmod.amount;
	}
	for(auto cmod : tech_id.get_rgo_size()) {
		state.world.nation_get_rgo_size(target_nation, cmod.type) += cmod.amount;
	}
	for(auto& umod : tech_id.get_modified_units()) {
		if(umod.type == state.military_definitions.base_army_unit) {
			for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
				dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
				if(state.military_definitions.unit_base_definitions[uid].is_land) {
					state.world.nation_get_unit_stats(target_nation, uid) += umod;
				}
			}
		} else if(umod.type == state.military_definitions.base_naval_unit) {
			for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
				dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
				if(!state.military_definitions.unit_base_definitions[uid].is_land) {
					state.world.nation_get_unit_stats(target_nation, uid) += umod;
				}
			}
		} else {
			state.world.nation_get_unit_stats(target_nation, umod.type) += umod;
		}
	}
}

void apply_invention(sys::state& state, dcon::nation_id target_nation, dcon::invention_id i_id) { //  TODO: shared prestige effect
	auto inv_id = fatten(state.world, i_id);

	state.world.nation_set_active_inventions(target_nation, i_id, true);

	// apply modifiers from active inventions
	auto inv_mod = inv_id.get_modifier();
	if(inv_mod) {
		auto& inv_nat_values = inv_mod.get_national_values();
		for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
			if(inv_nat_values.offsets[i] == 0)
				break; // no more modifier values attached to this tech

			auto fixed_offset = inv_nat_values.get_offet_at_index(i) - sys::provincial_mod_offsets::count;
			auto modifier_amount = inv_nat_values.values[i];

			state.world.nation_get_static_modifier_values(target_nation, fixed_offset) += modifier_amount;
		}
	}

	if(inv_id.get_enable_gas_attack()) {
		state.world.nation_set_has_gas_attack(target_nation, true);
	}
	if(inv_id.get_enable_gas_defence()) {
		state.world.nation_set_has_gas_defence(target_nation, true);
	}

	state.world.for_each_factory_type([&](dcon::factory_type_id id) {
		if(inv_id.get_activate_building(id)) {
			state.world.nation_set_active_building(target_nation, id, true);
		}
	});
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		if(inv_id.get_activate_unit(uid)) {
			state.world.nation_set_active_unit(target_nation, uid, true);
		}
	}
	for(uint32_t i = 0; i < state.culture_definitions.crimes.size(); ++i) {
		dcon::crime_id uid = dcon::crime_id{ dcon::crime_id::value_base_t(i) };
		if(inv_id.get_activate_crime(uid)) {
			state.world.nation_set_active_crime(target_nation, uid, true);
		}
	}

	for(auto cmod : inv_id.get_rgo_goods_output()) {
		state.world.nation_get_rgo_goods_output(target_nation, cmod.type) += cmod.amount;
	}
	for(auto cmod : inv_id.get_factory_goods_output()) {
		state.world.nation_get_factory_goods_output(target_nation, cmod.type) += cmod.amount;
	}
	for(auto cmod : inv_id.get_factory_goods_throughput()) {
		state.world.nation_get_factory_goods_throughput(target_nation, cmod.type) += cmod.amount;
	}
	for(auto cmod : inv_id.get_rebel_org()) {
		state.world.nation_get_rebel_org_modifier(target_nation, cmod.type) += cmod.amount;
	}
	for(auto& umod : inv_id.get_modified_units()) {
		if(umod.type == state.military_definitions.base_army_unit) {
			for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
				dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
				if(state.military_definitions.unit_base_definitions[uid].is_land) {
					state.world.nation_get_unit_stats(target_nation, uid) += umod;
				}
			}
		} else if(umod.type == state.military_definitions.base_naval_unit) {
			for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
				dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
				if(!state.military_definitions.unit_base_definitions[uid].is_land) {
					state.world.nation_get_unit_stats(target_nation, uid) += umod;
				}
			}
		} else {
			state.world.nation_get_unit_stats(target_nation, umod.type) += umod;
		}
	}
}

dcon::text_sequence_id get_current_flag_type(sys::state const& state, dcon::nation_id target_nation) {
	auto gov_type = state.world.nation_get_government_type(target_nation);
	return state.culture_definitions.governments[gov_type].flag;
}

}
