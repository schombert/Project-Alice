#pragma once
#include "system_state.hpp"
#include "military.hpp"
#include "economy_constants.hpp"

namespace military {

template<typename T>
auto province_is_blockaded(sys::state const& state, T ids) {
	return state.world.province_get_is_blockaded(ids);
}

template<typename T>
auto province_is_under_siege(sys::state const& state, T ids) {
	return state.world.province_get_siege_progress(ids) > 0.0f;
}

template<typename T>
auto battle_is_ongoing_in_province(sys::state const& state, T ids) {
	ve::apply(
			[&](dcon::province_id p) {
				auto battles = state.world.province_get_land_battle_location(p);
				return battles.begin() != battles.end();
			},
			ids);
	return false;
}


enum class battle_included : uint8_t {
	no = 0,
	yes = 1
};
enum class retreat_included : uint8_t {
	no = 0,
	yes = 1
};
enum class blackflag_included : uint8_t {
	no = 0,
	yes = 1
};
enum class participants_included : uint8_t {
	none = 0,
	allies = 1,
	in_common_war = 2,
	enemies = 4,
	ourselves = 8
};
constexpr participants_included operator|(participants_included a, participants_included b) {
	return static_cast<participants_included>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

template<typename enum_type>
constexpr bool has_flag(enum_type to_check, enum_type flag) {
	using integer_type = std::underlying_type<enum_type>::type;
	return (static_cast<integer_type>(to_check) & static_cast<integer_type>(flag)) != integer_type(0);
}

template<battle_included battle_included, retreat_included retreat_included, participants_included participant_setting>
bool province_has_fleet(const sys::state& state, dcon::province_id location, dcon::nation_id our_nation) {
	auto navies = state.world.province_get_navy_location(location);
	if(navies.begin() == navies.end()) {
		return false; // no navies present
	}
	for(auto navy : navies) {
		if constexpr(battle_included == battle_included::no) {
			if(navy.get_navy().get_battle_from_navy_battle_participation()) {
				continue;
			}
		}
		if constexpr(retreat_included == retreat_included::no) {
			if(navy.get_navy().get_is_retreating()) {
				continue;
			}
		}
		auto controller = navy.get_navy().get_controller_from_navy_control();
		if constexpr(has_flag(participant_setting, participants_included::ourselves)) {
			if(our_nation == controller) {
				return true;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::allies)) {
			if(nations::are_allied(state, our_nation, controller)) {
				return true;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::in_common_war)) {
			if(military::are_allied_in_war(state, our_nation, controller)) {
				return true;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::enemies)) {
			if(military::are_at_war(state, our_nation, controller)) {
				return true;
			}
		}
	}
	return false;
}

template<battle_included battle_included, retreat_included retreat_included, blackflag_included blackflag_included, participants_included participant_setting>
bool province_has_army(const sys::state& state, dcon::province_id location, dcon::nation_id our_nation) {
	auto armies = state.world.province_get_army_location(location);
	if(armies.begin() == armies.end()) {
		return false; // no armies present
	}
	for(auto army : armies) {
		if constexpr(battle_included == battle_included::no) {
			if(army.get_army().get_battle_from_army_battle_participation()) {
				continue;
			}
		}
		if constexpr(retreat_included == retreat_included::no) {
			if(army.get_army().get_is_retreating()) {
				continue;
			}
		}
		if constexpr(blackflag_included == blackflag_included::no) {
			if(army.get_army().get_black_flag()) {
				continue;
			}
		}
		auto controller = army.get_army().get_controller_from_army_control();
		if constexpr(has_flag(participant_setting, participants_included::ourselves)) {
			if(our_nation == controller) {
				return true;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::allies)) {
			if(nations::are_allied(state, our_nation, controller)) {
				return true;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::in_common_war)) {
			if(military::are_allied_in_war(state, our_nation, controller)) {
				return true;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::enemies)) {
			if(military::are_enemies(state, our_nation, controller)) {
				return true;
			}
		}
	}
	return false;
}



template<battle_included battle_included, retreat_included retreat_included, blackflag_included blackflag_included, participants_included participant_setting>
float army_strength_present(const sys::state& state, dcon::province_id location, dcon::nation_id nation_as) {
	float total = 0.0f;
	for(auto a : state.world.province_get_army_location(location)) {
		auto army = a.get_army();
		auto controller = army.get_controller_from_army_control();
		if constexpr(battle_included == battle_included::no) {
			if(state.world.army_get_battle_from_army_battle_participation(army)) {
				continue;
			}
		}
		if constexpr(retreat_included == retreat_included::no) {
			if(state.world.army_get_is_retreating(army)) {
				continue;
			}
		}
		if constexpr(blackflag_included == blackflag_included::no) {
			if(state.world.army_get_black_flag(army)) {
				continue;
			}
		}

		if constexpr(has_flag(participant_setting, participants_included::ourselves)) {
			if(nation_as == controller) {
				total += army_get_strength(state, army);
				continue;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::allies)) {
			if(nations::are_allied(state, nation_as, controller)) {
				total += army_get_strength(state, army);
				continue;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::in_common_war)) {
			if(military::are_allied_in_war(state, nation_as, controller)) {
				total += army_get_strength(state, army);
				continue;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::enemies)) {
			if(military::are_enemies(state, nation_as, controller)) {
				total += army_get_strength(state, army);
				continue;
			}
		}
	}
	return total;
}

template<battle_included battle_included, retreat_included retreat_included, participants_included participant_setting>
float navy_strength_present(const sys::state& state, dcon::province_id location, dcon::nation_id nation_as) {
	float total = 0.0f;
	for(auto a : state.world.province_get_navy_location(location)) {
		auto navy = a.get_navy();
		auto controller = navy.get_controller_from_navy_control();
		if constexpr(battle_included == battle_included::no) {
			if(state.world.navy_get_battle_from_navy_battle_participation(navy)) {
				continue;
			}
		}
		if constexpr(retreat_included == retreat_included::no) {
			if(state.world.navy_get_is_retreating(navy)) {
				continue;
			}
		}

		if constexpr(has_flag(participant_setting, participants_included::ourselves)) {
			if(nation_as == controller) {
				total += navy_get_strength(state, navy);
				continue;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::allies)) {
			if(nations::are_allied(state, nation_as, controller)) {
				total += navy_get_strength(state, navy);
				continue;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::in_common_war)) {
			if(military::are_allied_in_war(state, nation_as, controller)) {
				total += navy_get_strength(state, navy);
				continue;
			}
		}
		if constexpr(has_flag(participant_setting, participants_included::enemies)) {
			if(military::are_at_war(state, nation_as, controller)) {
				total += navy_get_strength(state, navy);
				continue;
			}
		}
	}
	return total;
}


// Calculates whether province can support more regiments
// Considers existing regiments and construction as well
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, F&& filter) {

	float divisor = 0;

	if(state.world.province_get_is_colonial(p)) {
		divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
	}
	else if(!state.world.province_get_is_owner_core(p)) {
		divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
	}
	else {
		divisor = state.defines.pop_size_per_regiment;
	}

	for(auto pop : state.world.province_get_pop_location(p)) {
		if(filter(state, pop.get_pop())) {
			if(can_pop_form_regiment(state, pop.get_pop(), divisor)) {
				return pop.get_pop().id;

			}
		}
	}
	return dcon::pop_id{};
	
}



// Finds a pop which can support more regiments anywhere in the target nation
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier_anywhere(sys::state& state, dcon::nation_id nation, F&& filter) {
	if(!nation) {
		// can't find a pop for the invalid/rebel tag
		return dcon::pop_id{ };
	}

	for(auto p : state.world.nation_get_province_ownership(nation)) {
		auto prov = p.get_province();
		if(state.world.province_get_nation_from_province_control(prov) == nation) {
			auto pop = find_available_soldier(state, prov, filter);
			if(bool(pop)) {
				return pop;
			}
		}
	}
	return dcon::pop_id{};

}



// Calculates whether province can support more regiments when parsing OOBs
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier_parsing(sys::state& state, dcon::province_id province_id, F&& filter) {
	float divisor = state.defines.pop_size_per_regiment;

	for(auto pop : state.world.province_get_pop_location(province_id)) {
		if(filter(state, pop.get_pop())) {
			if(can_pop_form_regiment(state, pop.get_pop(), divisor)) {
				return pop.get_pop().id;

			}
		}
	}
	return dcon::pop_id{ };
};




// Estimates reinforcement for a regiment while using the passed-in modifiers
// interval_type: Do we estimate the reinforcement per day, or per month?
// supply_type: Do we assume we have full supply, or do we scale it based on current satisfaction?
// cap_rule: Do we want the uncapped reinforcement regain above max strength? For regiments, the queued potential reinforcements counts as the current strength
template<interval_estimation interval_type, supply_estimation supply_type, reinforcement_cap cap_rule>
float estimate_reinforcement(const sys::state& state, dcon::regiment_id regiment, float reinforcement_mods) {
	float reinf_fufillment;
	if constexpr(interval_type == interval_estimation::daily) {
		if constexpr(supply_type == supply_estimation::based_on_satisfaction) {
			reinf_fufillment = std::clamp(state.world.regiment_get_reinforcement_satisfaction(regiment) / economy::unit_reinforcement_demand_divisor, 0.f, 1.f);
		}
		// full supply always
		else {
			reinf_fufillment = std::clamp(1.0f / economy::unit_reinforcement_demand_divisor, 0.f, 1.f);
		}
	}
	// monthly
	else {
		if constexpr(supply_type == supply_estimation::based_on_satisfaction) {
			reinf_fufillment = state.world.regiment_get_reinforcement_satisfaction(regiment);
		}
		// full supply always
		else {
			reinf_fufillment = 1.0f;
		}
	}
	auto pop = state.world.regiment_get_pop_from_regiment_source(regiment);
	auto combined = reinf_fufillment * reinforcement_mods;
	float newstr;
	float curstr = state.world.regiment_get_strength(regiment);
	auto pop_size = state.world.pop_get_size(pop);
	float pending_reinf = state.world.regiment_get_total_pending_reinforcement(regiment);
	if constexpr(cap_rule == reinforcement_cap::capped_at_max_strength) {
		combined = std::min(combined, 1.0f - (curstr + pending_reinf)); // Can only reinforce up to the amount of missing strength
		auto limit_fraction = std::max(state.defines.alice_full_reinforce, std::min(1.0f, pop_size / state.defines.pop_size_per_regiment));
		newstr = std::min(curstr + combined, limit_fraction);
		return std::max(newstr - curstr, 0.0f); 
	} else if constexpr(cap_rule == reinforcement_cap::uncapped) {
		return combined;
	}

	else {
		static_assert(false, "Unknown enum value");
	}

}
// Estimates reinforcement for a regiment, and computes the modifiers on its own
// US14 Calculates reinforcement for a particular regiment
template<interval_estimation interval_type, supply_estimation supply_type, reinforcement_cap cap_rule>
float estimate_reinforcement(const sys::state& state, dcon::regiment_id regiment) {
	auto army = state.world.regiment_get_army_from_army_membership(regiment);
	auto mods = get_land_reinforcement_modifiers(state, army);
	return estimate_reinforcement<interval_type, supply_type, cap_rule>(state, regiment, mods);

}

// Estimates combined reinforcement for an entire army
template<interval_estimation interval_type, supply_estimation supply_type, reinforcement_cap cap_rule>
float estimate_reinforcement(const sys::state& state, dcon::army_id army) {
	float total_reinforcement = 0.0f;
	for(auto r : state.world.army_get_army_membership(army)) {
		auto regiment = r.get_regiment();
		total_reinforcement += estimate_reinforcement<interval_type, supply_type, cap_rule>(state, regiment);
	}
	return total_reinforcement;

}


// Estimates reinforcement for a ship while using the passed-in modifiers
// interval_type: Do we estimate the reinforcement per day, or per month?
// supply_type: Do we assume we have full supply, or do we scale it based on current satisfaction?
// cap_rule: Do we want the uncapped reinforcement regain above max strength?
template<interval_estimation interval_type, supply_estimation supply_type, reinforcement_cap cap_rule>
float estimate_reinforcement(const sys::state& state, dcon::ship_id ship, float reinforcement_mods) {
	float reinf_fufillment;
	if constexpr(interval_type == interval_estimation::daily) {
		if constexpr(supply_type == supply_estimation::based_on_satisfaction) {
			reinf_fufillment = std::clamp(state.world.ship_get_reinforcement_satisfaction(ship) / economy::unit_reinforcement_demand_divisor, 0.f, 1.f);
		}
		// full supply always
		else {
			reinf_fufillment = std::clamp(1.0f / economy::unit_reinforcement_demand_divisor, 0.f, 1.f);
		}
	}
	// monthly
	else {
		if constexpr(supply_type == supply_estimation::based_on_satisfaction) {
			reinf_fufillment = state.world.ship_get_reinforcement_satisfaction(ship);
		}
		// full supply always
		else {
			reinf_fufillment = 1.0f;
		}
	}
	auto combined = reinf_fufillment * reinforcement_mods;
	float curstr = state.world.ship_get_strength(ship);
	if constexpr(cap_rule == reinforcement_cap::capped_at_max_strength) {
		combined = std::min(combined, 1.0f - curstr); // Can only reinforce up to the amount of missing strength
		return std::max(combined, 0.0f);
	} else if constexpr(cap_rule == reinforcement_cap::uncapped) {
		return combined;
	}
	else {
		static_assert(false, "Unknown enum");
	}

}

// Estimates reinforcement for a regiment while computing the modifiers on its own
// interval_type: Do we estimate the reinforcement per day, or per month?
// supply_type: Do we assume we have full supply, or do we scale it based on current satisfaction?
// cap_rule: Do we want the uncapped reinforcement regain above max strength?
template<interval_estimation interval_type, supply_estimation supply_type, reinforcement_cap cap_rule>
float estimate_reinforcement(const sys::state& state, dcon::ship_id ship) {
	auto navy = state.world.ship_get_navy_from_navy_membership(ship);
	auto mods = get_naval_reinforcement_modifiers(state, navy);
	return estimate_reinforcement<interval_type, supply_type, cap_rule>(state, ship, mods);

}

// Estimates combined reinforcement for a whole navy
// interval_type: Do we estimate the reinforcement per day, or per month?
// supply_type: Do we assume we have full supply, or do we scale it based on current satisfaction?
// cap_rule: Do we want the uncapped reinforcement regain above max strength?
template<interval_estimation interval_type, supply_estimation supply_type, reinforcement_cap cap_rule>
float estimate_reinforcement(const sys::state& state, dcon::navy_id navy) {
	float total_reinforcement = 0.0f;
	for(auto r : state.world.navy_get_navy_membership(navy)) {
		auto ship = r.get_ship();
		total_reinforcement += estimate_reinforcement<interval_type, supply_type, cap_rule>(state, ship);
	}
	return total_reinforcement;

}
// Accumulates the consumption required for a subunit (regiment or ship) for full goods fufillment using the functors for supply consumption and reinforcement consumption respectively
// Functor signature is: (dcon::commodity_id, float)
template<concepts::military_subunit subunit_type, typename FSupply, typename FReinf>
void accumulate_subunit_consumption(const sys::state& state, dcon::nation_id owner, subunit_type u, FSupply&& supply_acc_func, FReinf&& reinf_acc_func) {
	assert(owner);
	auto subunit = fatten(state.world, u);
	dcon::unit_type_id type = subunit.get_type();

	float supply_mod = military::get_supply_cost_modifiers(state, subunit);

	const auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
	supply_cost.for_each_commodity([&](dcon::commodity_id com_id, float required_amounts) {
		supply_acc_func(com_id, required_amounts * supply_mod);
	});

	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	float reinforcement = military::estimate_reinforcement<military::interval_estimation::daily, military::supply_estimation::full_supply_always, reinforcement_cap::capped_at_max_strength>(state, subunit);

	build_cost.for_each_commodity([&](dcon::commodity_id com_id, float required_amounts) {
		reinf_acc_func(com_id, required_amounts * reinforcement);
	});
}

template<typename F>
void for_each_unit(const sys::state& state, F&& func) {
	state.world.for_each_army(func);
	state.world.for_each_navy(func);
}

template<typename F>
void ve_for_each_unit(const sys::state& state, F&& func) {
	state.world.execute_serial_over_army(func);
	state.world.execute_serial_over_navy(func);
}

template<typename F>
void parallel_for_each_unit(const sys::state& state, F&& func) {
	concurrency::parallel_for(uint32_t(0), state.world.army_size(), [&](uint32_t i) {
		dcon::army_id army{ dcon::army_id::value_base_t(i) };
		if(state.world.army_is_valid(army)) {
			func(army);
		}
	});
	concurrency::parallel_for(uint32_t(0), state.world.navy_size(), [&](uint32_t i) {
		dcon::navy_id navy{ dcon::navy_id::value_base_t(i) };
		if(state.world.navy_is_valid(navy)) {
			func(navy);
		}
	});
}

template<typename F>
void ve_parallel_for_each_unit(const sys::state& state, F&& func) {
	state.world.execute_parallel_over_army(func);
	state.world.execute_parallel_over_navy(func);
}

template<concepts::military_unit unit_type, typename F>
void unit_for_each_subunit(const sys::state& state, unit_type unit, F&& func) {
	auto membership = unit_get_membership(state, unit);
	for(auto u : membership) {
		auto subunit = [&]() {
			if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
				return u.get_regiment();
			}
			else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
				return u.get_ship();
			}
		}();
		func(subunit.id);
	}
}

// Accumulates the consumption required for a unit (army or navy) using the functors for Supply consumption and Reinforcement consumption respectively
// Functor signature is: (dcon::commodity_id, float)
template<concepts::military_unit unit_type, typename FSupply, typename FReinf>
void accumulate_unit_consumption(sys::state& state, unit_type unit, FSupply&& acc_supply_func, FReinf&& acc_reinf_func) {
	dcon::nation_id nation = military::unit_get_controller(state, unit);
	unit_for_each_subunit(state, unit, [&](auto subunit) {
		accumulate_subunit_consumption(state, nation, subunit, acc_supply_func, acc_reinf_func);
	});
}







} // namespace military
