#include "dcon_generated.hpp"
#include "culture.hpp"
#include "system_state.hpp"
#include "triggers.hpp"

namespace culture {

void set_default_issue_and_reform_options(sys::state& state) {
	state.world.nation_resize_issues(state.world.issue_size());

	state.world.for_each_issue([&](dcon::issue_id i) {
		auto def_option = state.world.issue_get_options(i)[0];
		state.world.execute_serial_over_nation([&](auto ids) {
			state.world.nation_set_issues(ids, i, def_option);
		});
	});

	state.world.nation_resize_reforms(state.world.reform_size());

	state.world.for_each_reform([&](dcon::reform_id i) {
		auto def_option = state.world.reform_get_options(i)[0];
		state.world.execute_serial_over_nation([&](auto ids) {
			state.world.nation_set_reforms(ids, i, def_option);
		});
	});
}

void repopulate_technology_effects(sys::state& state) {
	state.world.for_each_technology([&](dcon::technology_id t_id) {
		auto tech_id = fatten(state.world, t_id);

		// apply modifiers from active technologies
		/*
		auto tech_mod = tech_id.get_modifier();
		if(tech_mod) {
			auto& tech_nat_values = tech_mod.get_national_values();
			for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
				if(!(tech_nat_values.offsets[i]))
					break; // no more modifier values attached to this tech

				state.world.execute_serial_over_nation([&,
					fixed_offset = tech_nat_values.offsets[i],
					modifier_amount = tech_nat_values.values[i]
				](auto nation_indices) {
					auto has_tech_mask = state.world.nation_get_active_technologies(nation_indices, t_id);
					auto old_mod_value = state.world.nation_get_modifier_values(nation_indices, fixed_offset);
					state.world.nation_set_modifier_values(nation_indices, fixed_offset,
						ve::select(has_tech_mask, old_mod_value + modifier_amount, old_mod_value));
				});
			}
		}
		*/

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
		/*
		auto inv_mod = inv_id.get_modifier();
		if(inv_mod) {
			auto& inv_nat_values = inv_mod.get_national_values();
			for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
				if(!(inv_nat_values.offsets[i]))
					break; // no more modifier values attached to this invention

				state.world.execute_serial_over_nation([&,
					fixed_offset = inv_nat_values.offsets[i],
					modifier_amount = inv_nat_values.values[i]
				](auto nation_indices) {
					auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
					auto old_mod_value = state.world.nation_get_modifier_values(nation_indices, fixed_offset);
					state.world.nation_set_modifier_values(nation_indices, fixed_offset,
						ve::select(has_inv_mask, old_mod_value + modifier_amount, old_mod_value));
				});
			}
		}
		*/

		if(inv_id.get_enable_gas_attack()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
				auto old_value = state.world.nation_get_has_gas_attack(nation_indices);
				state.world.nation_set_has_gas_attack(nation_indices, old_value | has_inv_mask);
			});
		}
		if(inv_id.get_enable_gas_defense()) {
			state.world.execute_serial_over_nation([&](auto nation_indices) {
				auto has_inv_mask = state.world.nation_get_active_inventions(nation_indices, i_id);
				auto old_value = state.world.nation_get_has_gas_defense(nation_indices);
				state.world.nation_set_has_gas_defense(nation_indices, old_value | has_inv_mask);
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
				if(cmod.type) {
					auto old_value = state.world.nation_get_rebel_org_modifier(nation_indices, cmod.type);
					state.world.nation_set_rebel_org_modifier(nation_indices, cmod.type,
						ve::select(has_inv_mask, old_value + cmod.amount, old_value));
				} else if(has_inv_mask.v != 0 ) {
					state.world.for_each_rebel_type([&](dcon::rebel_type_id rt) {
						auto old_value = state.world.nation_get_rebel_org_modifier(nation_indices, rt);
						state.world.nation_set_rebel_org_modifier(nation_indices, rt,
							ve::select(has_inv_mask, old_value + cmod.amount, old_value));
					});
				}
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
		for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
			if(!(tech_nat_values.offsets[i]))
				break; // no more modifier values attached to this tech

			auto fixed_offset = tech_nat_values.offsets[i];
			auto modifier_amount = tech_nat_values.values[i];

			state.world.nation_get_modifier_values(target_nation, fixed_offset) += modifier_amount;
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
		for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
			if(!(inv_nat_values.offsets[i]))
				break; // no more modifier values attached to this tech

			auto fixed_offset = inv_nat_values.offsets[i];
			auto modifier_amount = inv_nat_values.values[i];

			state.world.nation_get_modifier_values(target_nation, fixed_offset) += modifier_amount;
		}
	}

	if(inv_id.get_enable_gas_attack()) {
		state.world.nation_set_has_gas_attack(target_nation, true);
	}
	if(inv_id.get_enable_gas_defense()) {
		state.world.nation_set_has_gas_defense(target_nation, true);
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
		if(cmod.type) {
			state.world.nation_get_rebel_org_modifier(target_nation, cmod.type) += cmod.amount;
		} else {
			state.world.for_each_rebel_type([&](dcon::rebel_type_id rt) {
				state.world.nation_get_rebel_org_modifier(target_nation, rt) += cmod.amount;
			});
		}
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

uint32_t get_remapped_flag_type(sys::state const& state, flag_type type) {
	return state.flag_type_map[static_cast<size_t>(type)];
}

flag_type get_current_flag_type(sys::state const& state, dcon::nation_id target_nation) {
	auto gov_type = state.world.nation_get_government_type(target_nation);
	auto id = state.world.national_identity_get_government_flag_type(
		state.world.nation_get_identity_from_identity_holder(target_nation),
		gov_type);
	if(id != 0)
		return flag_type(id - 1);
	return state.culture_definitions.governments[gov_type].flag;
}

flag_type get_current_flag_type(sys::state const& state, dcon::national_identity_id identity) {
	auto holder = state.world.national_identity_get_nation_from_identity_holder(identity);
	if(holder && state.world.nation_get_owned_province_count(holder) > 0) {
		return get_current_flag_type(state, holder);
	} else {
		return flag_type::default_flag;
	}
}

void update_nation_issue_rules(sys::state& state, dcon::nation_id n_id) {
	uint32_t combined = 0;
	state.world.for_each_issue([&](dcon::issue_id i_id) {
		auto current_opt = state.world.nation_get_issues(n_id, i_id);
		auto rules_for_opt = state.world.issue_option_get_rules(current_opt);
		combined |= rules_for_opt;
	});
	state.world.for_each_reform([&](dcon::reform_id i_id) {
		auto current_opt = state.world.nation_get_reforms(n_id, i_id);
		auto rules_for_opt = state.world.reform_option_get_rules(current_opt);
		combined |= rules_for_opt;
	});
	state.world.nation_set_combined_issue_rules(n_id, combined);
}
void update_all_nations_issue_rules(sys::state& state) {
	state.world.execute_serial_over_nation([&](auto n_id) {
		ve::int_vector combined;
		state.world.for_each_issue([&](dcon::issue_id i_id) {
			auto current_opt = state.world.nation_get_issues(n_id, i_id);
			auto rules_for_opt = state.world.issue_option_get_rules(current_opt);
			combined = combined | rules_for_opt;
		});
		state.world.for_each_reform([&](dcon::reform_id i_id) {
			auto current_opt = state.world.nation_get_reforms(n_id, i_id);
			auto rules_for_opt = state.world.reform_option_get_rules(current_opt);
			combined = combined | rules_for_opt;
		});
		state.world.nation_set_combined_issue_rules(n_id, combined);
	});
}

void restore_unsaved_values(sys::state& state) {
	state.world.for_each_pop([&state](dcon::pop_id pid) {
		float total = 0.0f;
		float pol_sup = 0.0f;
		float soc_sup = 0.0f;
		state.world.for_each_issue_option([&](dcon::issue_option_id i) {
			auto sup = state.world.pop_get_demographics(pid, pop_demographics::to_key(state, i));
			total += sup;

			auto par = state.world.issue_option_get_parent_issue(i);
			if(state.world.issue_get_issue_type(par) == uint8_t(culture::issue_type::political)) {
				pol_sup += sup;
			} else if(state.world.issue_get_issue_type(par) == uint8_t(culture::issue_type::social)) {
				soc_sup += sup;
			}
		});
		if(total > 0) {
			state.world.pop_set_political_reform_desire(pid, pol_sup / total);
			state.world.pop_set_social_reform_desire(pid, soc_sup / total);
		}
	});
}

void create_initial_ideology_and_issues_distribution(sys::state& state) {
	state.world.for_each_pop([&state](dcon::pop_id pid) {
		auto ptype = state.world.pop_get_poptype(pid);
		auto owner = nations::owner_of_pop(state, pid);
		auto psize = state.world.pop_get_size(pid);
		if(psize <= 0)
			return;

		{ // ideologies
			float total = 0.0f;
			state.world.for_each_ideology([&](dcon::ideology_id iid) {
				if(state.world.ideology_get_enabled(iid) && (!state.world.ideology_get_is_civilized_only(iid) || state.world.nation_get_is_civilized(owner))) {
					auto ptrigger = state.world.pop_type_get_ideology(ptype, iid);
					if(ptrigger) {
						auto amount = trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(pid), trigger::to_generic(owner), 0);
						state.world.pop_set_demographics(pid, pop_demographics::to_key(state, iid), amount);
						total += amount;
					}
				}
			});
			if(total != 0) {
				float adjustment_factor = 1.0f / total;
				state.world.for_each_ideology([&state, pid, adjustment_factor](dcon::ideology_id iid) {
					auto normalized_amount = state.world.pop_get_demographics(pid, pop_demographics::to_key(state, iid)) * adjustment_factor;
					state.world.pop_set_demographics(pid, pop_demographics::to_key(state, iid), normalized_amount);
				});
			}
		}
		{ // issues
			
			float total = 0.0f;

			state.world.for_each_issue_option([&](dcon::issue_option_id iid) {
				auto opt = fatten(state.world, iid);
				auto allow = opt.get_allow();
				auto parent_issue = opt.get_parent_issue();
				if((state.world.nation_get_is_civilized(owner) || state.world.issue_get_issue_type(parent_issue) == uint8_t(issue_type::party))
					&& (!allow || trigger::evaluate(state, allow, trigger::to_generic(owner), trigger::to_generic(owner), 0))) {
					if(auto mtrigger = state.world.pop_type_get_issues(ptype, iid); mtrigger) {
						auto amount = trigger::evaluate_multiplicative_modifier(state, mtrigger, trigger::to_generic(pid), trigger::to_generic(owner), 0);
						state.world.pop_set_demographics(pid, pop_demographics::to_key(state, iid), amount);
						total += amount;
					}
				}
			});
			if(total != 0) {
				float adjustment_factor = 1.0f / total;
				state.world.for_each_issue_option([&state, pid, adjustment_factor](dcon::issue_option_id iid) {
					auto normalized_amount = state.world.pop_get_demographics(pid, pop_demographics::to_key(state, iid)) * adjustment_factor;
					state.world.pop_set_demographics(pid, pop_demographics::to_key(state, iid), normalized_amount);
				});
			}
		}
	});
}

}
