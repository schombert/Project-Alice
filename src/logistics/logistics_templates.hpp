#pragma once
#include "system_state.hpp"
#include "logistics.hpp"

namespace logistics {

template<concepts::military_supply_route_type route_type>
float military_route_get_buffered_goods(const sys::state& state, route_type route, dcon::unit_supply_commodity_id commodity_id) {
	auto fat_route = fatten(state.world, route);
	return fat_route.get_buffered_supply_goods(commodity_id);
}
template<concepts::military_supply_route_type route_type>
float military_route_get_buffered_goods(const sys::state& state, route_type route, dcon::unit_build_commodity_id commodity_id) {
	auto fat_route = fatten(state.world, route);
	return fat_route.get_buffered_reinforcement_goods(commodity_id);
}

template<concepts::any_dcon_id_type<dcon::army_supply_route_id> ID>
auto supply_route_is_active(const sys::state& state, ID route) {
	return state.world.army_supply_route_get_is_active(route);
}
template<concepts::any_dcon_id_type<dcon::navy_supply_route_id> ID>
auto supply_route_is_active(const sys::state& state, ID route) {
	return state.world.navy_supply_route_get_is_active(route);
}
template<concepts::any_dcon_id_type<dcon::land_construction_supply_route_id> ID>
auto supply_route_is_active(const sys::state& state, ID route) {
	return state.world.land_construction_supply_route_get_is_active(route);
}
template<concepts::any_dcon_id_type<dcon::naval_construction_supply_route_id> ID>
auto supply_route_is_active(const sys::state& state, ID route) {
	return state.world.naval_construction_supply_route_get_is_active(route);
}
template<concepts::any_dcon_id_type<dcon::factory_construction_supply_route_id> ID>
auto supply_route_is_active(const sys::state& state, ID route) {
	return state.world.factory_construction_supply_route_get_is_active(route);
}
template<concepts::any_dcon_id_type<dcon::building_construction_supply_route_id> ID>
auto supply_route_is_active(const sys::state& state, ID route) {
	return state.world.building_construction_supply_route_get_is_active(route);
}

template<typename F>
void for_each_unit_supply_route(sys::state& state, F&& functor) {
	state.world.for_each_army_supply_route(functor);
	state.world.for_each_navy_supply_route(functor);
}

template<typename F>
void ve_for_each_unit_supply_route(sys::state& state, F&& functor) {
	state.world.execute_serial_over_army_supply_route(functor);
	state.world.execute_serial_over_navy_supply_route(functor);
}

template<typename F>
void parallel_for_each_unit_supply_route(sys::state& state, F&& functor) {
	concurrency::parallel_for(uint32_t(0), state.world.army_supply_route_size(), [&](uint32_t i) {
		dcon::army_supply_route_id route = dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i) };
		functor(route);
	});
	concurrency::parallel_for(uint32_t(0), state.world.navy_supply_route_size(), [&](uint32_t i) {
		dcon::navy_supply_route_id route = dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) };
		functor(route);
	});
}

template<typename F>
void ve_parallel_for_each_unit_supply_route(sys::state& state, F&& functor) {
	state.world.execute_parallel_over_army_supply_route(functor);
	state.world.execute_parallel_over_navy_supply_route(functor);
}

template<typename F>
void for_each_construction_supply_route(sys::state& state, F&& functor) {
	state.world.for_each_land_construction_supply_route(functor);
	state.world.for_each_naval_construction_supply_route(functor);
	state.world.for_each_factory_construction_supply_route(functor);
	state.world.for_each_building_construction_supply_route(functor);
}

template<typename F>
void ve_for_each_construction_supply_route(sys::state& state, F&& functor) {
	state.world.execute_serial_over_land_construction_supply_route(functor);
	state.world.execute_serial_over_naval_construction_supply_route(functor);
	state.world.execute_serial_over_factory_construction_supply_route(functor);
	state.world.execute_serial_over_building_construction_supply_route(functor);
}


template<typename F>
void parallel_for_each_construction_supply_route(sys::state& state, F&& functor) {
	concurrency::parallel_for(uint32_t(0), state.world.land_construction_supply_route_size(), [&](uint32_t i) {
		dcon::land_construction_supply_route_id route = dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) };
		functor(route);
	});
	concurrency::parallel_for(uint32_t(0), state.world.naval_construction_supply_route_size(), [&](uint32_t i) {
		dcon::naval_construction_supply_route_id route = dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) };
		functor(route);
	});
	concurrency::parallel_for(uint32_t(0), state.world.factory_construction_supply_route_size(), [&](uint32_t i) {
		dcon::factory_construction_supply_route_id route = dcon::factory_construction_supply_route_id{ dcon::factory_construction_supply_route_id::value_base_t(i) };
		functor(route);
	});
	concurrency::parallel_for(uint32_t(0), state.world.building_construction_supply_route_size(), [&](uint32_t i) {
		dcon::building_construction_supply_route_id route = dcon::building_construction_supply_route_id{ dcon::building_construction_supply_route_id::value_base_t(i) };
		functor(route);
	});
}

template<typename F>
void ve_parallel_for_each_construction_supply_route(sys::state& state, F&& functor) {
	state.world.execute_parallel_over_land_construction_supply_route(functor);
	state.world.execute_parallel_over_naval_construction_supply_route(functor);
	state.world.execute_parallel_over_factory_construction_supply_route(functor);
	state.world.execute_parallel_over_building_construction_supply_route(functor);
}

template<typename F>
void for_each_supply_route(sys::state& state, F&& functor) {
	for_each_unit_supply_route(state, functor);
	for_each_construction_supply_route(state, functor);
}
template<typename F>
void ve_for_each_supply_route(sys::state& state, F&& functor) {
	ve_for_each_unit_supply_route(state, functor);
	ve_for_each_construction_supply_route(state, functor);
}


template<typename F>
void parallel_for_each_supply_route(sys::state& state, F&& functor) {
	parallel_for_each_unit_supply_route(state, functor);
	parallel_for_each_construction_supply_route(state, functor);
}

template<typename F>
void ve_parallel_for_each_supply_route(sys::state& state, F&& functor) {
	ve_parallel_for_each_unit_supply_route(state, functor);
	ve_parallel_for_each_construction_supply_route(state, functor);
}
template<typename F>
void supply_path_for_each_connected_route(const sys::state& state, dcon::supply_route_path_id path, F&& functor) {
	for(auto route : state.world.supply_route_path_get_army_route_path(path)) {
		functor(route.get_route().id);
	}
	for(auto route : state.world.supply_route_path_get_navy_route_path(path)) {
		functor(route.get_route().id);
	}
	for(auto route : state.world.supply_route_path_get_land_construction_route_path(path)) {
		functor(route.get_route().id);
	}
	for(auto route : state.world.supply_route_path_get_naval_construction_route_path(path)) {
		functor(route.get_route().id);
	}
	for(auto route : state.world.supply_route_path_get_factory_construction_route_path(path)) {
		functor(route.get_route().id);
	}
	for(auto route : state.world.supply_route_path_get_building_construction_route_path(path)) {
		functor(route.get_route().id);
	}
}


template<typename F>
void parallel_for_each_supply_route_path(sys::state& state, F&& functor) {
	concurrency::parallel_for(uint32_t(0), state.world.supply_route_path_size(), [&](uint32_t i) {
		dcon::supply_route_path_id route = dcon::supply_route_path_id{ dcon::supply_route_path_id::value_base_t(i) };
		functor(route);
	});
}


template<typename F, typename P>
void parallel_for_each_supply_route_path_predicate(sys::state& state, P&& pred, F&& functor) {
	static std::vector<dcon::supply_route_path_id> valid_paths;
	valid_paths.clear();
	state.world.for_each_supply_route_path([&](dcon::supply_route_path_id path) {
		if(pred(path)) {
			valid_paths.push_back(path);
		}
	});
	concurrency::parallel_for_each(valid_paths.begin(), valid_paths.end(), [&](dcon::supply_route_path_id i) {
		functor(i);
	});
}
template<typename F>
void construction_for_each_active_route(const sys::state& state, dcon::province_land_construction_id con, F&& func) {
	state.world.province_land_construction_for_each_land_construction_supply_route(con, [&](auto route) {
		if(logistics::supply_route_is_active(state, route)) {
			func(route);
		}
	});
}
template<typename F>
void construction_for_each_active_route(const sys::state& state, dcon::province_naval_construction_id con, F&& func) {
	state.world.province_naval_construction_for_each_naval_construction_supply_route(con, [&](auto route) {
		if(logistics::supply_route_is_active(state, route)) {
			func(route);
		}
	});
}
template<typename F>
void construction_for_each_active_route(const sys::state& state, dcon::factory_construction_id con, F&& func) {
	state.world.factory_construction_for_each_factory_construction_supply_route(con, [&](auto route) {
		if(logistics::supply_route_is_active(state, route)) {
			func(route);
		}
	});
}
template<typename F>
void construction_for_each_active_route(const sys::state& state, dcon::province_building_construction_id con, F&& func) {
	state.world.province_building_construction_for_each_building_construction_supply_route(con, [&](auto route) {
		if(logistics::supply_route_is_active(state, route)) {
			func(route);
		}
	});
}
template<typename F>
void unit_for_each_active_route(const sys::state& state, dcon::army_id unit, F&& func) {
	state.world.army_for_each_army_supply_route(unit, [&](dcon::army_supply_route_id route) {
		if(logistics::supply_route_is_active(state, route)) {
			func(route);
		}
	});
}
template<typename F>
void unit_for_each_active_route(const sys::state& state, dcon::navy_id unit, F&& func) {
	state.world.navy_for_each_navy_supply_route(unit, [&](dcon::navy_supply_route_id route) {
		if(logistics::supply_route_is_active(state, route)) {
			func(route);
		}
	});
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_type>
auto naval_supply_speed(const sys::state& state, nation_type nation_as) {
	if constexpr(concepts::dcon_id_type<nation_type>) {
		return std::max(state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_add) * state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_mul) * (state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_percent) + 1.0f), 0.0f);
	}
	else if constexpr(concepts::dcon_id_ve_type<nation_type, dcon::nation_id>) {
		return ve::max(state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_add) * state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_mul) * (state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_percent) + 1.0f), 0.0f);
	}
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_type>
auto land_supply_speed(const sys::state& state, nation_type nation_as) {
	if constexpr(concepts::dcon_id_type<nation_type>) {
		return std::max(state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_add) * state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_mul) * (state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_percent) + 1.0f), 0.0f);
	}
	else if constexpr(concepts::dcon_id_ve_type<nation_type, dcon::nation_id>) {
		return ve::max(state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_add) * state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_mul) * (state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_percent) + 1.0f), 0.0f);
	}
}

template<concepts::supply_route_type route_type>
float supply_route_get_supply_loss(const sys::state& state, route_type route) {
	dcon::supply_route_path_id path = supply_route_get_path(state, route);
	return state.world.supply_route_path_get_supply_loss(path);
}

template<concepts::supply_route_type route_type>
float supply_route_get_throughput(const sys::state& state, route_type route) {
	dcon::supply_route_path_id path = supply_route_get_path(state, route);
	return state.world.supply_route_path_get_throughput(path);
}

}
