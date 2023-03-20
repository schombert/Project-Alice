#include "modifiers.hpp"
#include "system_state.hpp"

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
void remove_modifier_values_from_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		state.world.nation_get_modifier_values(target_nation, fixed_offset) -= modifier_amount;
	}
}
void remove_modifier_values_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	auto owner = state.world.province_get_nation_from_province_ownership(target_prov);
	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];
		state.world.province_get_modifier_values(target_prov, fixed_offset) -= modifier_amount;
	}
	if(owner) {
		auto& nat_values = state.world.modifier_get_national_values(mod_id);
		for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
			if(!(nat_values.offsets[i]))
				break; // no more modifier values

			auto fixed_offset = nat_values.offsets[i];
			auto modifier_amount = nat_values.values[i];
			state.world.nation_get_modifier_values(owner, fixed_offset) -= modifier_amount;
		}
	}
}

// restores values after loading a save
void repopulate_modifier_effects(sys::state& state) {
	state.world.for_each_province([&](dcon::province_id p) {
		apply_modifier_values_to_province(state, p, state.world.province_get_terrain(p));
		apply_modifier_values_to_province(state, p, state.world.province_get_climate(p));
		apply_modifier_values_to_province(state, p, state.world.province_get_continent(p));

		auto c = state.world.province_get_crime(p);
		if(c)
			apply_modifier_values_to_province(state, p, state.culture_definitions.crimes[c].modifier);

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

void add_modifier_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id, sys::date expiration) {
	state.world.nation_get_current_modifiers(target_nation).push_back(sys::dated_modifier{ expiration, mod_id });
	apply_modifier_values_to_nation(state, target_nation, mod_id);
}
void add_modifier_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id, sys::date expiration) {
	state.world.province_get_current_modifiers(target_prov).push_back(sys::dated_modifier{ expiration, mod_id });
	apply_modifier_values_to_province(state, target_prov, mod_id);
}
void remove_modifier_from_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id) {
	auto modifiers_range = state.world.nation_get_current_modifiers(target_nation);
	auto count = modifiers_range.size();
	for(uint32_t i = count; i-- > 0; ) {
		if(modifiers_range.at(i).mod_id == mod_id) {
			remove_modifier_values_from_nation(state, target_nation, mod_id);
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
			remove_modifier_values_from_province(state, target_prov, mod_id);
			modifiers_range.remove_at(i);
			return;
		}
	}
}
void remove_expired_modifiers_from_nation(sys::state& state, dcon::nation_id target_nation) {
	auto modifiers_range = state.world.nation_get_current_modifiers(target_nation);
	auto count = modifiers_range.size();
	for(uint32_t i = count; i-- > 0; ) {
		auto expiration = modifiers_range.at(i).expiration;
		if(expiration&& expiration < state.current_date) {
			remove_modifier_values_from_nation(state, target_nation, modifiers_range.at(i).mod_id);
			modifiers_range.remove_at(i);
		}
	}
}
void remove_expired_modifiers_from_province(sys::state& state, dcon::province_id target_prov) {
	auto modifiers_range = state.world.province_get_current_modifiers(target_prov);
	auto count = modifiers_range.size();
	for(uint32_t i = count; i-- > 0; ) {
		auto expiration = modifiers_range.at(i).expiration;
		if(expiration && expiration < state.current_date) {
			remove_modifier_values_from_province(state, target_prov, modifiers_range.at(i).mod_id);
			modifiers_range.remove_at(i);
		}
	}
}

}

