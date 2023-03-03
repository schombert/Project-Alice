#include "military.hpp"
#include "dcon_generated.hpp"

namespace military {

void reset_unit_stats(sys::state& state) {
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		state.world.for_each_nation([&](dcon::nation_id nid) {
			state.world.nation_get_unit_stats(nid, uid) = state.military_definitions.unit_base_definitions[uid];
		});
	}
}

void apply_base_unit_stat_modifiers(sys::state& state) {
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		auto base_id = state.military_definitions.unit_base_definitions[uid].is_land ? state.military_definitions.base_army_unit : state.military_definitions.base_naval_unit;
		state.world.for_each_nation([&](dcon::nation_id nid) {
			state.world.nation_get_unit_stats(nid, uid) += state.world.nation_get_unit_stats(nid, base_id);
		});
	}
}

void restore_unsaved_values(sys::state& state) {
	state.world.for_each_nation([&](dcon::nation_id n) {
		auto w = state.world.nation_get_war_attacker(n);
		if(w.begin() != w.end()) {
			state.world.nation_set_is_at_war(n, true);
			return;
		}
		auto w2 = state.world.nation_get_war_defender(n);
		if(w2.begin() != w2.end()) {
			state.world.nation_set_is_at_war(n, true);
			return;
		}
	});
}

bool can_use_cb_against(sys::state const& state, dcon::nation_id from, dcon::nation_id target) {
	// TODO: implement function
	// return true if nation from has any cb to use against target
	return false;
}

template<typename T>
auto province_is_blockaded(sys::state const& state, T ids) {
	// TODO: implement function
	return false;
}

template<typename T>
auto battle_is_ongoing_in_province(sys::state const& state, T ids) {
	// TODO: implement function
	return false;
}

float recruited_pop_fraction(sys::state const& state, dcon::nation_id n) {
	// TODO: implement function
	return 0.0f;
}

bool state_has_naval_base(sys::state const& state, dcon::state_instance_id si) {
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	auto def = state.world.state_instance_get_definition(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(def)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			if(p.get_province().get_naval_base_level() > 0)
				return true;
		}
	}
	return false;
}

bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	for(auto wa : state.world.nation_get_war_attacker(a)) {
		for(auto wd : wa.get_war().get_war_defender()) {
			if(wd.get_nation() == b)
				return true;
		}
	}

	for(auto wd : state.world.nation_get_war_defender(a)) {
		for(auto wa : wd.get_war().get_war_attacker()) {
			if(wa.get_nation() == b)
				return true;
		}
	}

	return false;
}

}
