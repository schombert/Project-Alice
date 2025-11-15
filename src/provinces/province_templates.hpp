#pragma once
#include "system_state.hpp"

namespace province {

template<typename T>
auto is_overseas(sys::state const& state, T ids) {
	auto owners = state.world.province_get_nation_from_province_ownership(ids);
	auto owner_cap = state.world.nation_get_capital(owners);
	return (state.world.province_get_continent(ids) != state.world.province_get_continent(owner_cap)) &&
				 (state.world.province_get_connected_region_id(ids) != state.world.province_get_connected_region_id(owner_cap));
}

template<typename F>
void for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	for(int32_t i = 0; i < last; ++i) {
		dcon::province_id pid{dcon::province_id::value_base_t(i)};
		func(pid);
	}
}

template<typename F>
void ve_for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	ve::execute_serial<dcon::province_id>(uint32_t(last), func);
}

template<typename F>
void ve_parallel_for_each_land_province(sys::state& state, F const& func) {
	int32_t last = state.province_definitions.first_sea_province.index();
	ve::execute_parallel<dcon::province_id>(uint32_t(last), func);
}

template<typename F>
void for_each_sea_province(sys::state& state, F const& func) {
	int32_t first = state.province_definitions.first_sea_province.index();
	for(int32_t i = first; i < int32_t(state.world.province_size()); ++i) {
		dcon::province_id pid{dcon::province_id::value_base_t(i)};
		func(pid);
	}
}

template<typename F>
void for_each_province_in_state_instance(sys::state& state, dcon::state_instance_id s, F const& func) {
	auto d = state.world.state_instance_get_definition(s);
	auto o = state.world.state_instance_get_nation_from_state_ownership(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			func(p.get_province().id);
		}
	}
}

template<typename F>
void for_each_province_building(sys::state& state, F const& function) {
	for(auto t = economy::province_building_type::railroad; t != economy::province_building_type::last; t = economy::province_building_type(uint8_t(t) + 1)) {
		function(t);
	}
}

template<typename F>
void for_each_market_province_parallel_over_market(sys::state& state, F const& func) {
	concurrency::parallel_for((size_t)(0), (size_t)(state.world.market_size()), [&](auto raw_mid) {
		dcon::market_id mid{ (dcon::market_id::value_base_t) (raw_mid) };
		auto sid = state.world.market_get_zone_from_local_market(mid);
		province::for_each_province_in_state_instance(state, sid, [&](auto pid) {
			func(mid, sid, pid);
		});
	});
}

} // namespace province
