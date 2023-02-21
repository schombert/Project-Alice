#include "modifiers.hpp"
#include "system_state.hpp"

namespace sys {

// NOTE: these functions do not add or remove a modifier from the list of modifiers for an entity
void apply_modifier_values_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
		if(nat_values.offsets[i] == 0)
			break; // no more modifier values

		auto fixed_offset = nat_values.get_offet_at_index(i) - sys::provincial_mod_offsets::count;
		auto modifier_amount = nat_values.values[i];
		state.world.nation_get_static_modifier_values(target_nation, fixed_offset) += modifier_amount;
	}
}
void apply_modifier_values_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	auto owner = state.world.province_get_nation_from_province_ownership(target_prov);
	for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
		if(prov_values.offsets[i] == 0)
			break; // no more modifier values

		auto fixed_offset = prov_values.get_offet_at_index(i);
		auto modifier_amount = prov_values.values[i];
		if(fixed_offset < sys::provincial_mod_offsets::count) {
			state.world.province_get_modifier_values(target_prov, fixed_offset) += modifier_amount;
		} else if(owner) {
			state.world.nation_get_static_modifier_values(owner, fixed_offset - sys::provincial_mod_offsets::count) += modifier_amount;
		}
	}
}
void remove_modifier_values_from_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
		if(nat_values.offsets[i] == 0)
			break; // no more modifier values

		auto fixed_offset = nat_values.get_offet_at_index(i) - sys::provincial_mod_offsets::count;
		auto modifier_amount = nat_values.values[i];
		state.world.nation_get_static_modifier_values(target_nation, fixed_offset) -= modifier_amount;
	}
}
void remove_modifier_values_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	auto owner = state.world.province_get_nation_from_province_ownership(target_prov);
	for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
		if(prov_values.offsets[i] == 0)
			break; // no more modifier values

		auto fixed_offset = prov_values.get_offet_at_index(i);
		auto modifier_amount = prov_values.values[i];
		if(fixed_offset < sys::provincial_mod_offsets::count) {
			state.world.province_get_modifier_values(target_prov, fixed_offset) -= modifier_amount;
		} else if(owner) {
			state.world.nation_get_static_modifier_values(owner, fixed_offset - sys::provincial_mod_offsets::count) -= modifier_amount;
		}
	}
}
void apply_modifier_values_to_province_owner(sys::state& state, dcon::nation_id owner, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
		if(prov_values.offsets[i] == 0)
			break; // no more modifier values

		auto fixed_offset = prov_values.get_offet_at_index(i);
		auto modifier_amount = prov_values.values[i];
		if(fixed_offset >= sys::provincial_mod_offsets::count) {
			state.world.nation_get_static_modifier_values(owner, fixed_offset - sys::provincial_mod_offsets::count) += modifier_amount;
		}
	}
}
void remove_modifier_values_from_province_owner(sys::state& state, dcon::nation_id owner, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	for(uint32_t i = 0; i < sys::modifier_definition_size; ++i) {
		if(prov_values.offsets[i] == 0)
			break; // no more modifier values

		auto fixed_offset = prov_values.get_offet_at_index(i);
		auto modifier_amount = prov_values.values[i];
		if(fixed_offset >= sys::provincial_mod_offsets::count) {
			state.world.nation_get_static_modifier_values(owner, fixed_offset - sys::provincial_mod_offsets::count) -= modifier_amount;
		}
	}
}

// restores values after a save
void repopulate_modifier_effects(sys::state& state) {
	state.world.for_each_province([&](dcon::province_id p) {
		apply_modifier_values_to_province(state, p, state.world.province_get_terrain(p));
		apply_modifier_values_to_province(state, p, state.world.province_get_climate(p));
		apply_modifier_values_to_province(state, p, state.world.province_get_continent(p));

		for(auto mpr : state.world.province_get_current_modifiers(p)) {
			apply_modifier_values_to_province(state, p, mpr.mod_id);
		}
	});

	state.world.for_each_nation([&](dcon::nation_id n) {
		apply_modifier_values_to_nation(state, n, state.world.nation_get_tech_school(n));
		apply_modifier_values_to_nation(state, n, state.world.nation_get_national_value(n));

		for(auto mpr : state.world.nation_get_current_modifiers(n)) {
			apply_modifier_values_to_nation(state, n, mpr.mod_id);
		}
	});
}

}

