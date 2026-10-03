#pragma once
#include "ve_scalar_extensions.hpp"
#include "system_state.hpp"

namespace nations {

// returns whether a culture is on the accepted list OR is the primary culture
template<typename T, typename U>
auto nation_accepts_culture(sys::state const& state, T ids, U cul_ids) {
	auto is_accepted = ve::apply(
			[&state](dcon::nation_id n, dcon::culture_id c) {
				if(n)
					return state.world.nation_get_accepted_cultures(n, c);
				else
					return false;
			},
			ids, cul_ids);
	return (state.world.nation_get_primary_culture(ids) == cul_ids) || is_accepted;
}

template<typename T>
auto primary_culture_group(sys::state const& state, T ids) {
	auto cultures = state.world.nation_get_primary_culture(ids);
	return state.world.culture_get_group_from_culture_group_membership(cultures);
}

template<typename T>
auto owner_of_pop(sys::state const& state, T pop_ids) {
	auto location = state.world.pop_get_province_from_pop_location(pop_ids);
	return state.world.province_get_nation_from_province_ownership(location);
}

template<typename T>
auto central_blockaded_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_ports(ids));
	auto b_count = ve::to_float(state.world.nation_get_central_blockaded(ids));
	auto ret = decltype(cpc){};
	return ve::select(cpc != 0.0f, b_count / cpc, ret);
}

template<typename T>
auto central_reb_controlled_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_province_count(ids));
	auto reb_count = ve::to_float(state.world.nation_get_central_rebel_controlled(ids));
	return ve::select(cpc != 0.0f, reb_count / cpc, decltype(cpc)());
}

template<typename T>
auto central_has_crime_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_central_province_count(ids));
	auto crim_count = ve::to_float(state.world.nation_get_central_crime_count(ids));
	return ve::select(cpc != 0.0f, crim_count / cpc, decltype(cpc)());
}

template<typename T>
auto occupied_provinces_fraction(sys::state const& state, T ids) {
	auto cpc = ve::to_float(state.world.nation_get_owned_province_count(ids));
	auto occ_count = ve::to_float(state.world.nation_get_occupied_count(ids));
	return ve::select(cpc != 0.0f, occ_count / cpc, decltype(cpc)());
}
template<typename F>
dcon::nation_id get_random_nation(const sys::state& state, uint32_t random_hi, uint32_t random_lo, F&& condition_func) {
	static thread_local std::vector<dcon::nation_id> rlist;
	rlist.clear();
	state.world.for_each_nation([&](dcon::nation_id nation) {
		if(condition_func(nation)) {
			rlist.push_back(nation);
		}
	});
	if(rlist.size() != 0) {
		auto r = rng::get_random(state, random_hi, random_lo) % rlist.size();
		return rlist[r];
	}
	return dcon::nation_id{};
}

template<concepts::dcon_id_ve_type<dcon::nation_id> ve_id_type>
ve::mask_vector exists(sys::state& state, ve_id_type nations) {
	return state.world.nation_get_owned_province_count(nations) > 0;
}

template<concepts::dcon_id_ve_type<dcon::nation_id> ve_id_type>
ve::mask_vector exists_or_is_utility_tag(sys::state& state, ve_id_type nations) {
	return exists(state, nations) || state.world.nation_get_utility_tag(nations);
}
// Calls the functor on all constructions a nation owns (all construction types)
template<typename F>
void nation_for_each_construction(const sys::state& state, dcon::nation_id nation, F&& func) {
	for(auto lc : state.world.nation_get_province_land_construction(nation)) {
		func(lc.id);
	}
	for(auto nc : state.world.nation_get_province_naval_construction(nation)) {
		func(nc.id);
	}
	for(auto fc : state.world.nation_get_factory_construction(nation)) {
		func(fc.id);
	}
	for(auto pc : state.world.nation_get_province_building_construction(nation)) {
		func(pc.id);
	}
}

// Calls the functor on all nation-owned constructions of the type passed as template param 
template<concepts::construction_type con_type, typename F>
void nation_for_each_construction_by_type(const sys::state& state, dcon::nation_id nation, F&& func) {
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		for(auto lc : state.world.nation_get_province_land_construction(nation)) {
			func(lc.id);
		}
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		for(auto nc : state.world.nation_get_province_naval_construction(nation)) {
			func(nc.id);
		}
	}
	else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		for(auto fc : state.world.nation_get_factory_construction(nation)) {
			func(fc.id);
		}
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		for(auto pc : state.world.nation_get_province_building_construction(nation)) {
			func(pc.id);
		}
	}
	else {
		static_assert(false, "Unknown type");
	}
}

// Calls the functor on all units a nation owns (both navies & armies)
template<typename F>
void nation_for_each_unit(const sys::state& state, dcon::nation_id nation, F&& func) {
	auto army_it = state.world.nation_get_army_control(nation);
	auto navy_it = state.world.nation_get_navy_control(nation);
	for(auto a : army_it) {
		func(a.get_army().id);
	}
	for(auto n : navy_it) {
		func(n.get_navy().id);
	}
}
// Calls the functor on either all navies or all armies a nation owns depending on the "unit_type" template param 
template<concepts::military_unit unit_type, typename F>
void nation_for_each_unit_by_type(const sys::state& state, dcon::nation_id nation, F&& func) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		auto army_it = state.world.nation_get_army_control(nation);
		for(auto a : army_it) {
			func(a.get_army().id);
		}
	}
	else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		auto navy_it = state.world.nation_get_navy_control(nation);
		for(auto n : navy_it) {
			func(n.get_navy().id);
		}
	}
	else {
		static_assert(false, "Unknown type");
	}
}
template<typename F>
void parallel_for_each_existing_nation(const sys::state& state, F&& func) {
	static std::vector<dcon::nation_id> existing_nations{ };
	existing_nations.clear();
	get_existing_nations(state, existing_nations);
	concurrency::parallel_for_each(existing_nations.begin(), existing_nations.end(), func);
}

} // namespace nations
