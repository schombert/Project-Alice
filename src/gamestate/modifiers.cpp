#include "modifiers.hpp"
#include "system_state.hpp"
#include "province.hpp"
#include "military.hpp"

namespace sys {

// NOTE: these functions do not add or remove a modifier from the list of modifiers for an entity
void apply_modifier_values_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		state.world.nation_get_modifier_values(target_nation, fixed_offset) += modifier_amount;
	}
}

void apply_scaled_modifier_values_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id, float scale) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		state.world.nation_get_modifier_values(target_nation, fixed_offset) += modifier_amount * scale;
	}
}

void apply_modifier_values_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	auto owner = state.world.province_get_nation_from_province_ownership(target_prov);
	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];
		state.world.province_get_modifier_values(target_prov, fixed_offset) += modifier_amount;
		
	}
	if(owner) {
		auto& nat_values = state.world.modifier_get_national_values(mod_id);
		for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
			if(!(nat_values.offsets[i]))
				break; // no more modifier values

			auto fixed_offset = nat_values.offsets[i];
			auto modifier_amount = nat_values.values[i];
			state.world.nation_get_modifier_values(owner, fixed_offset) += modifier_amount;
		}
	}
}

void add_modifier_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id, sys::date expiration) {
	state.world.nation_get_current_modifiers(target_nation).push_back(sys::dated_modifier{ expiration, mod_id });
}
void add_modifier_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id, sys::date expiration) {
	state.world.province_get_current_modifiers(target_prov).push_back(sys::dated_modifier{ expiration, mod_id });
}
void remove_modifier_from_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id) {
	auto modifiers_range = state.world.nation_get_current_modifiers(target_nation);
	auto count = modifiers_range.size();
	for(uint32_t i = count; i-- > 0; ) {
		if(modifiers_range.at(i).mod_id == mod_id) {
			modifiers_range.remove_at(i);
			return;
		}
	}
}
void remove_modifier_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id) {
	auto modifiers_range = state.world.province_get_current_modifiers(target_prov);
	auto count = modifiers_range.size();
	for(uint32_t i = count; i-- > 0; ) {
		if(modifiers_range.at(i).mod_id == mod_id) {
			modifiers_range.remove_at(i);
			return;
		}
	}
}

template<typename F>
void bulk_apply_masked_modifier_to_nations(sys::state& state, dcon::modifier_id m, F const& mask_functor) {
	auto& nat_values = state.world.modifier_get_national_values(m);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values attached

		state.world.execute_serial_over_nation([&,
			fixed_offset = nat_values.offsets[i],
			modifier_amount = nat_values.values[i]
		](auto nation_indices) {
			auto has_mod_mask = mask_functor(nation_indices);
			auto old_mod_value = state.world.nation_get_modifier_values(nation_indices, fixed_offset);
			state.world.nation_set_modifier_values(nation_indices, fixed_offset,
				ve::select(has_mod_mask, old_mod_value + modifier_amount, old_mod_value));
		});
	}
}

template<typename F>
void bulk_apply_scaled_modifier_to_nations(sys::state& state, dcon::modifier_id m, F const& scale_functor) {
	auto& nat_values = state.world.modifier_get_national_values(m);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values attached

		state.world.execute_serial_over_nation([&,
			fixed_offset = nat_values.offsets[i],
			modifier_amount = nat_values.values[i]
		](auto nation_indices) {
			auto scaling_factor = scale_functor(nation_indices);
			auto old_mod_value = state.world.nation_get_modifier_values(nation_indices, fixed_offset);
			state.world.nation_set_modifier_values(nation_indices, fixed_offset,
				old_mod_value + scaling_factor * modifier_amount);
		});
	}
}

template<typename F>
void bulk_apply_masked_modifier_to_provinces(sys::state& state, dcon::modifier_id mod_id, F const& mask_functor) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);

	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values


		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];

		province::ve_for_each_land_province(state, [&](auto ids) {
			auto has_mod_mask = mask_functor(ids);
			auto old_value = state.world.province_get_modifier_values(ids, fixed_offset);
			state.world.province_set_modifier_values(ids, fixed_offset,
				ve::select(has_mod_mask, old_value + modifier_amount, old_value));
		});
	}
	
	auto& nat_values = state.world.modifier_get_national_values(mod_id);

	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];

		province::ve_for_each_land_province(state, [&](auto ids) {
			auto owners = state.world.province_get_nation_from_province_ownership(ids);
			auto has_mod_mask = mask_functor(ids) && (owners != dcon::nation_id{});

			auto old_value = state.world.nation_get_modifier_values(owners, fixed_offset);
			state.world.nation_set_modifier_values(owners, fixed_offset,
				ve::select(has_mod_mask, old_value + modifier_amount, old_value));
		});
	}
}

void bulk_apply_modifier_to_provinces(sys::state& state, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);

	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];

		province::ve_for_each_land_province(state, [&](auto ids) {
			auto old_value = state.world.province_get_modifier_values(ids, fixed_offset);
			state.world.province_set_modifier_values(ids, fixed_offset, old_value + modifier_amount);
		});
	}
}

template<typename F>
void bulk_apply_scaled_modifier_to_provinces(sys::state& state, dcon::modifier_id mod_id, F const& scale_functor) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);

	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values


		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];

		province::ve_for_each_land_province(state, [&](auto ids) {
			auto scale = scale_functor(ids);
			auto old_value = state.world.province_get_modifier_values(ids, fixed_offset);
			state.world.province_set_modifier_values(ids, fixed_offset,
				old_value + modifier_amount * scale);
		});
	}

	auto& nat_values = state.world.modifier_get_national_values(mod_id);

	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];

		province::ve_for_each_land_province(state, [&](auto ids) {
			auto owners = state.world.province_get_nation_from_province_ownership(ids);
			auto scale = ve::select(owners != dcon::nation_id{}, scale_functor(ids), 0.0f);

			auto old_value = state.world.nation_get_modifier_values(owners, fixed_offset);
			state.world.nation_set_modifier_values(owners, fixed_offset,
				 old_value + modifier_amount * scale);
		});
	}
}

void recreate_national_modifiers(sys::state& state) {

	// purge expired triggered modifiers
	for(auto n : state.world.in_nation) {
		auto timed_modifiers = n.get_current_modifiers();
		for(uint32_t i = timed_modifiers.size(); i-- > 0;) {
			if(bool(timed_modifiers[i].expiration) && timed_modifiers[i].expiration >= state.current_date) {
				timed_modifiers.remove_at(i);
			}
		}
	}

	concurrency::parallel_for(uint32_t(0), sys::national_mod_offsets::count, [&](uint32_t i) {
		dcon::national_modifier_value mid{ dcon::national_modifier_value::value_base_t(i) };
		state.world.execute_serial_over_nation([&](auto ids) {
			state.world.nation_set_modifier_values(ids, mid, ve::fp_vector{});
		});
	});

	for(auto n : state.world.in_nation) {
		if(auto ts = n.get_tech_school(); ts)
			apply_modifier_values_to_nation(state, n, ts);
	}
	for(auto n : state.world.in_nation) {
		if(auto nv = n.get_national_value(); nv)
			apply_modifier_values_to_nation(state, n, nv);
	}
	for(auto n : state.world.in_nation) {
		for(auto mpr : state.world.nation_get_current_modifiers(n)) {
			apply_modifier_values_to_nation(state, n, mpr.mod_id);
		}
	}
	state.world.for_each_technology([&](dcon::technology_id t) {
		auto tmod = state.world.technology_get_modifier(t);
		if(tmod) {
			bulk_apply_masked_modifier_to_nations(state, tmod, [&](auto ids) { return state.world.nation_get_active_technologies(ids, t); });
		}
	});
	state.world.for_each_invention([&](dcon::invention_id i) {
		auto tmod = state.world.invention_get_modifier(i);
		if(tmod) {
			bulk_apply_masked_modifier_to_nations(state, tmod, [&](auto ids) { return state.world.nation_get_active_inventions(ids, i); });
		}
	});
	state.world.for_each_issue([&](dcon::issue_id i) {
		for(auto n : state.world.in_nation) {
			auto iopt = state.world.nation_get_issues(n, i);
			auto imod = state.world.issue_option_get_modifier(iopt);
			if(imod && (n.get_is_civilized() || state.world.issue_get_issue_type(i) == uint8_t(culture::issue_type::party))) {
				apply_modifier_values_to_nation(state, n, imod);
			}
		}
	});
	state.world.for_each_reform([&](dcon::reform_id i) {
		for(auto n : state.world.in_nation) {
			auto iopt = state.world.nation_get_reforms(n, i);
			auto imod = state.world.reform_option_get_modifier(iopt);
			if(imod && !n.get_is_civilized()) {
				apply_modifier_values_to_nation(state, n, imod);
			}
		}
	});
	for(auto n : state.world.in_nation) {
		auto in_wars = n.get_war_participant();
		if(in_wars.begin() != in_wars.end()) {
			if(state.national_definitions.war)
				apply_modifier_values_to_nation(state, n, state.national_definitions.war);
		} else {
			if(state.national_definitions.peace)
				apply_modifier_values_to_nation(state, n, state.national_definitions.peace);
		}
	}
	
	if(state.national_definitions.badboy) {
		bulk_apply_scaled_modifier_to_nations(state, state.national_definitions.badboy, [&](auto ids) { return state.world.nation_get_infamy(ids); });
	}
	if(state.national_definitions.plurality) {
		bulk_apply_scaled_modifier_to_nations(state, state.national_definitions.plurality, [&](auto ids) { return state.world.nation_get_plurality(ids); });
	}
	if(state.national_definitions.war_exhaustion) {
		bulk_apply_scaled_modifier_to_nations(state, state.national_definitions.war_exhaustion, [&](auto ids) { return state.world.nation_get_war_exhaustion(ids); });
	}
	if(state.national_definitions.average_literacy) {
		bulk_apply_scaled_modifier_to_nations(state, state.national_definitions.average_literacy, [&](auto ids) {
			auto total = state.world.nation_get_demographics(ids, demographics::total);
			return ve::select(total > 0, state.world.nation_get_demographics(ids, demographics::literacy) / total, 0.0f); });
	}
	if(state.national_definitions.total_blockaded) {
		bulk_apply_scaled_modifier_to_nations(state, state.national_definitions.total_blockaded, [&](auto ids) {
				auto bc = ve::to_float(state.world.nation_get_central_blockaded(ids));
				auto c = ve::to_float(state.world.nation_get_central_ports(ids));
				return ve::select(c > 0.0f, bc / c, 0.0f);
			});
	}
	if(state.national_definitions.total_occupation) {
		bulk_apply_scaled_modifier_to_nations(state, state.national_definitions.total_occupation, [&](auto ids) {
			return ve::apply([&](dcon::nation_id nid) {
				auto n = fatten(state.world, nid);
				auto cap_continent = n.get_capital().get_continent();
				float total = 0.0f;
				float occupied = 0.0f;
				for(auto owned : n.get_province_ownership()) {
					if(owned.get_province().get_continent() == cap_continent) {
						total += 1.0f;
						if(auto c = owned.get_province().get_nation_from_province_control().id; c && c != n.id) {
							occupied += 1.0f;
						}
					}
				}
				if(total > 0.0f)
					return occupied / total;
				else
					return 0.0f;
			}, ids);
		});
	}
	for(auto n : state.world.in_nation) {
		if(n.get_is_civilized() == false) {
			if(state.national_definitions.unciv_nation)
				apply_modifier_values_to_nation(state, n, state.national_definitions.unciv_nation);
		} else if(nations::is_great_power(state, n)) {
			if(state.national_definitions.great_power)
				apply_modifier_values_to_nation(state, n, state.national_definitions.great_power);
		} else if(n.get_rank() <= uint16_t(state.defines.colonial_rank)) {
			if(state.national_definitions.second_power)
				apply_modifier_values_to_nation(state, n, state.national_definitions.second_power);
		} else {
			if(state.national_definitions.civ_nation)
				apply_modifier_values_to_nation(state, n, state.national_definitions.civ_nation);
		}
	}
	if(state.national_definitions.disarming) {
		for(auto n : state.world.in_nation) {
			if(bool(n.get_disarmed_until()) && n.get_disarmed_until() > state.current_date) {
				apply_modifier_values_to_nation(state, n, state.national_definitions.disarming);
			}
		}
	}
	if(state.national_definitions.in_bankrupcy) {
		bulk_apply_masked_modifier_to_nations(state, state.national_definitions.in_bankrupcy, [&](auto ids) { return state.world.nation_get_is_bankrupt(ids); });
	}
	// TODO: debt

	for(auto tm : state.national_definitions.triggered_modifiers) {
		if(tm.trigger_condition && tm.linked_modifier) {
			auto& nat_values = state.world.modifier_get_national_values(tm.linked_modifier);
			auto size_used = state.world.nation_size();
			ve::execute_serial_fast<dcon::nation_id>(size_used, [&](auto nids) {
				auto trigger_condition_satisfied = trigger::evaluate(state, tm.trigger_condition, trigger::to_generic(nids), trigger::to_generic(nids), 0) || ve::apply([size_used](auto n) { return n.index() < int32_t(size_used); }, nids);
				auto compressed_res = ve::compress_mask(trigger_condition_satisfied);
				if(compressed_res.v == ve::vbitfield_type::storage(0)) {
					return;
				} else {
					for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
						if(!(nat_values.offsets[i]))
							break; // no more modifier values attached

						auto fixed_offset = nat_values.offsets[i];
						auto modifier_amount = nat_values.values[i];
						auto old_mod_value = state.world.nation_get_modifier_values(nids, fixed_offset);
						state.world.nation_set_modifier_values(nids, fixed_offset, ve::select(trigger_condition_satisfied, old_mod_value + modifier_amount, old_mod_value));
					}
				}
			});
		}
	}
}

void update_single_nation_modifiers(sys::state& state, dcon::nation_id n) {

	for(uint32_t i = uint32_t(0); i < sys::national_mod_offsets::count; ++i) {
		dcon::national_modifier_value mid{ dcon::national_modifier_value::value_base_t(i) };
		state.world.nation_set_modifier_values(n, mid, 0.0f);
	}

	if(auto ts = state.world.nation_get_tech_school(n); ts)
		apply_modifier_values_to_nation(state, n, ts);
	
	if(auto nv = state.world.nation_get_national_value(n); nv)
		apply_modifier_values_to_nation(state, n, nv);

	for(auto mpr : state.world.nation_get_current_modifiers(n)) {
		apply_modifier_values_to_nation(state, n, mpr.mod_id);
	}
	
	state.world.for_each_technology([&](dcon::technology_id t) {
		auto tmod = state.world.technology_get_modifier(t);
		if(tmod && state.world.nation_get_active_technologies(n, t)) {
			apply_modifier_values_to_nation(state, n, tmod);
		}
	});
	state.world.for_each_invention([&](dcon::invention_id i) {
		auto tmod = state.world.invention_get_modifier(i);
		if(tmod && state.world.nation_get_active_inventions(n, i)) {
			apply_modifier_values_to_nation(state, n, tmod);
		}
	});
	state.world.for_each_issue([&](dcon::issue_id i) {
		auto iopt = state.world.nation_get_issues(n, i);
		auto imod = state.world.issue_option_get_modifier(iopt);
		if(imod && (state.world.nation_get_is_civilized(n) || state.world.issue_get_issue_type(i) == uint8_t(culture::issue_type::party))) {
			apply_modifier_values_to_nation(state, n, imod);
		}
		
	});
	if(!state.world.nation_get_is_civilized(n)) {
		state.world.for_each_reform([&](dcon::reform_id i) {
			auto iopt = state.world.nation_get_reforms(n, i);
			auto imod = state.world.reform_option_get_modifier(iopt);
			if(imod) {
				apply_modifier_values_to_nation(state, n, imod);
			}
		});
	}
	
	auto in_wars = state.world.nation_get_war_participant(n);
	if(in_wars.begin() != in_wars.end()) {
		if(state.national_definitions.war)
			apply_modifier_values_to_nation(state, n, state.national_definitions.war);
	} else {
		if(state.national_definitions.peace)
			apply_modifier_values_to_nation(state, n, state.national_definitions.peace);
	}
	

	if(state.national_definitions.badboy) {
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.badboy, state.world.nation_get_infamy(n));
	}
	if(state.national_definitions.plurality) {
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.plurality, state.world.nation_get_plurality(n));
	}
	if(state.national_definitions.war_exhaustion) {
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.war_exhaustion, state.world.nation_get_war_exhaustion(n));
	}
	if(state.national_definitions.average_literacy) {
		auto total = state.world.nation_get_demographics(n, demographics::total);
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.average_literacy, total > 0 ? state.world.nation_get_demographics(n, demographics::literacy) / total : 0.0f);
	}
	if(state.national_definitions.total_blockaded) {
		auto bc = ve::to_float(state.world.nation_get_central_blockaded(n));
		auto c = ve::to_float(state.world.nation_get_central_ports(n));

		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.total_blockaded, c > 0.0f ? bc / c : 0.0f);
	}
	if(state.national_definitions.total_occupation) {
		auto nid = fatten(state.world, n);
		auto cap_continent = nid.get_capital().get_continent();
		float total = 0.0f;
		float occupied = 0.0f;
		for(auto owned : nid.get_province_ownership()) {
			if(owned.get_province().get_continent() == cap_continent) {
				total += 1.0f;
				if(auto c = owned.get_province().get_nation_from_province_control().id; c && c != n) {
					occupied += 1.0f;
				}
			}
		}

		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.total_occupation, total > 0.0f ? occupied / total : 0.0f);
	}

	if(state.world.nation_get_is_civilized(n) == false) {
		if(state.national_definitions.unciv_nation)
			apply_modifier_values_to_nation(state, n, state.national_definitions.unciv_nation);
	} else if(nations::is_great_power(state, n)) {
		if(state.national_definitions.great_power)
			apply_modifier_values_to_nation(state, n, state.national_definitions.great_power);
	} else if(state.world.nation_get_rank(n) <= uint16_t(state.defines.colonial_rank)) {
		if(state.national_definitions.second_power)
			apply_modifier_values_to_nation(state, n, state.national_definitions.second_power);
	} else {
		if(state.national_definitions.civ_nation)
			apply_modifier_values_to_nation(state, n, state.national_definitions.civ_nation);
	}
	
	if(state.national_definitions.disarming) {
		if(bool(state.world.nation_get_disarmed_until(n)) && state.world.nation_get_disarmed_until(n) > state.current_date) {
			apply_modifier_values_to_nation(state, n, state.national_definitions.disarming);
		}
	}
	if(state.national_definitions.in_bankrupcy) {
		if(bool(state.world.nation_get_is_bankrupt(n))) {
			apply_modifier_values_to_nation(state, n, state.national_definitions.in_bankrupcy);
		}
	}
	// TODO: debt

	for(auto tm : state.national_definitions.triggered_modifiers) {
		if(tm.trigger_condition && tm.linked_modifier) {

			auto trigger_condition_satisfied = trigger::evaluate(state, tm.trigger_condition, trigger::to_generic(n), trigger::to_generic(n), 0);

			if(trigger_condition_satisfied) {
				apply_modifier_values_to_nation(state, n, tm.linked_modifier);
			}
		}
	}
}

void recreate_province_modifiers(sys::state& state) {
	// purge expired triggered modifiers
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto timed_modifiers = state.world.province_get_current_modifiers(p);
		for(uint32_t i = timed_modifiers.size(); i-- > 0;) {
			if(bool(timed_modifiers[i].expiration) && timed_modifiers[i].expiration >= state.current_date) {
				timed_modifiers.remove_at(i);
			}
		}
	});

	concurrency::parallel_for(uint32_t(0), sys::provincial_mod_offsets::count, [&](uint32_t i) {
		dcon::provincial_modifier_value mid{ dcon::provincial_modifier_value::value_base_t(i) };
		province::ve_for_each_land_province(state, [&](auto ids) {
			state.world.province_set_modifier_values(ids, mid, ve::fp_vector{});
		});
	});

	if(state.national_definitions.land_province)
		bulk_apply_modifier_to_provinces(state, state.national_definitions.land_province);

	province::for_each_land_province(state, [&](dcon::province_id p) {
		for(auto mpr : state.world.province_get_current_modifiers(p)) {
			apply_modifier_values_to_province(state, p, mpr.mod_id);
		}
	});

	province::for_each_land_province(state, [&](dcon::province_id p) {
		if(auto m = state.world.province_get_terrain(p); m)
			apply_modifier_values_to_province(state, p, m);
	});
	province::for_each_land_province(state, [&](dcon::province_id p) {
		if(auto m = state.world.province_get_climate(p); m)
			apply_modifier_values_to_province(state, p, m);
	});
	province::for_each_land_province(state, [&](dcon::province_id p) {
		if(auto m = state.world.province_get_continent(p); m)
			apply_modifier_values_to_province(state, p, m);
	});

	province::for_each_land_province(state, [&](dcon::province_id p) {
		if(auto c = state.world.province_get_crime(p); c) {
			if(auto m = state.culture_definitions.crimes[c].modifier; m)
				apply_modifier_values_to_province(state, p, m);
		}
	});

	if(state.economy_definitions.railroad_definition.province_modifier) {
		bulk_apply_scaled_modifier_to_provinces(state, state.economy_definitions.railroad_definition.province_modifier,
			[&](auto ids) {
				return ve::to_float(state.world.province_get_railroad_level(ids));
			});
	}
	if(state.national_definitions.infrastructure) {
		bulk_apply_scaled_modifier_to_provinces(state, state.national_definitions.infrastructure,
			[&](auto ids) {
				return ve::to_float(state.world.province_get_railroad_level(ids)) * state.economy_definitions.railroad_definition.infrastructure;
			});
	}
	if(state.economy_definitions.fort_definition.province_modifier) {
		bulk_apply_scaled_modifier_to_provinces(state, state.economy_definitions.fort_definition.province_modifier,
			[&](auto ids) {
				return ve::to_float(state.world.province_get_fort_level(ids));
			});
	}
	if(state.economy_definitions.naval_base_definition.province_modifier) {
		bulk_apply_scaled_modifier_to_provinces(state, state.economy_definitions.fort_definition.province_modifier,
			[&](auto ids) {
				return ve::to_float(state.world.province_get_naval_base_level(ids));
			});
	}
	if(state.national_definitions.nationalism) {
		bulk_apply_scaled_modifier_to_provinces(state, state.national_definitions.nationalism,
			[&](auto ids) {
				return ve::select(state.world.province_get_is_owner_core(ids), 0.0f, state.world.province_get_nationalism(ids));
			});
	}
	if(state.national_definitions.non_coastal) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.non_coastal,
			[&](auto ids) {
				return !state.world.province_get_is_coast(ids);
			});
	}
	if(state.national_definitions.coastal) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.coastal,
			[&](auto ids) {
				return state.world.province_get_is_coast(ids);
			});
	}
	if(state.national_definitions.overseas) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.overseas,
			[&](auto ids) {
				return province::is_overseas(state, ids);
			});
	}
	if(state.national_definitions.core) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.core,
			[&](auto ids) {
				return state.world.province_get_is_owner_core(ids);
			});
	}
	if(state.national_definitions.has_siege) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.has_siege,
			[&](auto ids) {
				return military::province_is_under_siege(state, ids);
			});
	}
	if(state.national_definitions.blockaded) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.blockaded,
			[&](auto ids) {
				return military::province_is_blockaded(state, ids);
			});
	}
}


// restores values after loading a save
void repopulate_modifier_effects(sys::state& state) {
	recreate_national_modifiers(state);
	recreate_province_modifiers(state);
}

void update_modifier_effects(sys::state& state) {
	recreate_national_modifiers(state);
	recreate_province_modifiers(state);
}

}

