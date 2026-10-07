#include "concept_declarations.hpp"
#include "system_state.hpp"
#include "military.hpp"
#include "economy.hpp"
#include "province.hpp"
#include "military_templates.hpp"
#include "construction.hpp"
#include "logistics_templates.hpp"
#include "logistics.hpp"
#include "economy_templates.hpp"
#include "military_templates.hpp"
#include "nations_templates.hpp"

namespace logistics {

constexpr float construction_route_transport_leeway = 0.001f; // The minimum % of the total cost a route will attempt to consume and transport per day as extra leeway. This makes it so that constructions arent "stuck" for abit due to supply loss. Should be low enough that it isn't noticable.

constexpr uint32_t supply_route_pathfinding_batches = 30; // The desired amount of batches for processing pathfinding in parallel. Less batches = better performance when under heavy load, but the pathfinding is not able to take other potentially overlapping paths into account in the pathfinding logic.

// union of either a army or navy. Used for type erasure when prioritizing supplies between military units
struct unit {
	bool is_army = false;
	union unit_type_union {
		dcon::army_id army;
		dcon::navy_id navy{};
		constexpr unit_type_union() = default;
	} content;

	template<concepts::military_unit unit_type>
	constexpr unit(unit_type unit) {
		if constexpr(std::is_same<unit_type, dcon::army_id>::value) {
			is_army = true;
			content.army = unit;
		} else if constexpr(std::is_same<unit_type, dcon::navy_id>::value) {
			is_army = false;
			content.navy = unit;
		}
	}
	constexpr unit() = default;
};



// local functions for accessing resuable buffers, as a way of aliasing them
template<concepts::any_dcon_id_type<dcon::market_id> market_type>
auto local_stockpile_available_goods_get(const sys::state& state, market_type market, dcon::commodity_id com_id) {
	return state.world.market_get_commodity_float_buffer_1(market, com_id);
}

template<concepts::any_dcon_id_type<dcon::market_id> market_type, concepts::regular_or_ve_value_type<float> float_type>
void local_stockpile_available_goods_set(sys::state& state, market_type market, dcon::commodity_id com_id, float_type val) {
	state.world.market_set_commodity_float_buffer_1(market, com_id, val);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_type>
auto nation_stockpile_available_goods_get(const sys::state& state, nation_type nation, dcon::commodity_id com_id) {
	return state.world.nation_get_commodity_float_buffer_1(nation, com_id);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_type, concepts::regular_or_ve_value_type<float> float_type>
void nation_stockpile_available_goods_set(sys::state& state, nation_type nation, dcon::commodity_id com_id, float_type val) {
	state.world.nation_set_commodity_float_buffer_1(nation, com_id, val);
}


template<typename unit_id_type>
auto unit_supply_need_get(const sys::state& state, unit_id_type unit, dcon::unit_supply_commodity_id com_id) {
	if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::army_id>) {
		return state.world.army_get_unit_supply_commodity_float_buffer_1(unit, com_id);
	}
	else if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::navy_id>) {
		return state.world.navy_get_unit_supply_commodity_float_buffer_1(unit, com_id);
	}
	else {
		static_assert(false, "Unsupported type");
	}
}

template<typename unit_id_type, concepts::regular_or_ve_value_type<float> float_type>
void unit_supply_need_set(sys::state& state, unit_id_type unit, dcon::unit_supply_commodity_id com_id, float_type val) {
	if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::army_id>) {
		state.world.army_set_unit_supply_commodity_float_buffer_1(unit, com_id, val);
	}
	else if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::navy_id>) {
		state.world.navy_set_unit_supply_commodity_float_buffer_1(unit, com_id, val);
	}
	else {
		static_assert(false, "Unsupported type");
	}
}


template<typename unit_id_type>
auto unit_reinforcement_need_get(const sys::state& state, unit_id_type unit, dcon::unit_build_commodity_id com_id) {
	if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::army_id>) {
		return state.world.army_get_unit_build_commodity_float_buffer_1(unit, com_id);
	} else if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::navy_id>) {
		return state.world.navy_get_unit_build_commodity_float_buffer_1(unit, com_id);
	} else {
		static_assert(false, "Unsupported type");
	}
}

template<typename unit_id_type, concepts::regular_or_ve_value_type<float> float_type>
void unit_reinforcement_need_set(sys::state& state, unit_id_type unit, dcon::unit_build_commodity_id com_id, float_type val) {
	if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::army_id>) {
		state.world.army_set_unit_build_commodity_float_buffer_1(unit, com_id, val);
	} else if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::navy_id>) {
		state.world.navy_set_unit_build_commodity_float_buffer_1(unit, com_id, val);
	} else {
		static_assert(false, "Unsupported type");
	}
}

template<military::unit_priority priority, concepts::any_dcon_id_type<dcon::nation_id> nation_id_type>
auto nation_unit_prio_need_get(const sys::state& state, nation_id_type n, dcon::unit_supply_and_build_commodity_id com_id) {
	if constexpr(priority == military::unit_priority::low_priority) {
		return state.world.nation_get_unit_supply_and_build_commodity_float_buffer_1(n, com_id);
	}
	else if constexpr(priority == military::unit_priority::normal_priority) {
		return state.world.nation_get_unit_supply_and_build_commodity_float_buffer_2(n, com_id);
	}
	else if constexpr(priority == military::unit_priority::high_priority) {
		return state.world.nation_get_unit_supply_and_build_commodity_float_buffer_3(n, com_id);
	}
}

template<military::unit_priority priority, concepts::any_dcon_id_type<dcon::nation_id> nation_id_type, concepts::regular_or_ve_value_type<float> float_type>
void nation_unit_prio_need_set(sys::state& state, nation_id_type n, dcon::unit_supply_and_build_commodity_id com_id, float_type val) {
	if constexpr(priority == military::unit_priority::low_priority) {
		state.world.nation_set_unit_supply_and_build_commodity_float_buffer_1(n, com_id, val);
	}
	else if constexpr(priority == military::unit_priority::normal_priority) {
		state.world.nation_set_unit_supply_and_build_commodity_float_buffer_2(n, com_id, val);
	}
	else if constexpr(priority == military::unit_priority::high_priority) {
		state.world.nation_set_unit_supply_and_build_commodity_float_buffer_3(n, com_id, val);
	}
}
template<concepts::construction_type construction_type>
const economy::commodity_amounts& constructions_need_get(const sys::state& state, construction_type con) {
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_commodity_amounts_buffer_1(con);
	}
	else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_commodity_amounts_buffer_1(con);
	}
	else if constexpr(std::is_same_v<construction_type, dcon::factory_construction_id>) {
		return state.world.factory_construction_get_commodity_amounts_buffer_1(con);
	}
	else if constexpr(std::is_same_v<construction_type, dcon::province_building_construction_id>) {
		return state.world.province_building_construction_get_commodity_amounts_buffer_1(con);
	}
}

template<concepts::construction_type construction_type>
economy::commodity_amounts& constructions_need_get(sys::state& state, construction_type con) {
	return const_cast<economy::commodity_amounts&>(constructions_need_get(static_cast<const sys::state&>(state), con));
}


template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type>
auto nation_construction_need_get(const sys::state& state, nation_id_type n, dcon::commodity_id com_id) {
	return state.world.nation_get_commodity_float_buffer_2(n, com_id);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type, concepts::regular_or_ve_value_type<float> float_type>
void nation_construction_need_set(sys::state& state, nation_id_type n, dcon::commodity_id com_id, float_type val) {
	state.world.nation_set_commodity_float_buffer_2(n, com_id, val);
}


template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type>
auto nation_unit_expected_satisfaction_get(const sys::state& state, nation_id_type n, dcon::unit_supply_and_build_commodity_id com_id) {
	return state.world.nation_get_unit_supply_and_build_commodity_float_buffer_4(n, com_id);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type, concepts::regular_or_ve_value_type<float> float_type>
void nation_unit_expected_satisfaction_set(sys::state& state, nation_id_type n, dcon::unit_supply_and_build_commodity_id com_id, float_type val) {
	state.world.nation_set_unit_supply_and_build_commodity_float_buffer_4(n, com_id, val);
}


template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type>
auto nation_construction_expected_satisfaction_get(const sys::state& state, nation_id_type n, dcon::commodity_id com_id) {
	return state.world.nation_get_commodity_float_buffer_3(n, com_id);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type, concepts::regular_or_ve_value_type<float> float_type>
void nation_construction_expected_satisfaction_set(sys::state& state, nation_id_type n, dcon::commodity_id com_id, float_type val) {
	state.world.nation_set_commodity_float_buffer_3(n, com_id, val);
}

template<concepts::military_unit unit_id_type>
auto unit_best_stockpiles_get(const sys::state& state, unit_id_type unit) {
	if constexpr(std::is_same_v<unit_id_type, dcon::army_id>) {
		return state.world.army_get_state_instance_vector_buffer_1(unit);
	}
	if constexpr(std::is_same_v<unit_id_type, dcon::navy_id>) {
		return state.world.navy_get_state_instance_vector_buffer_1(unit);
	}
}
template<concepts::military_unit unit_id_type>
auto unit_best_stockpiles_get(sys::state& state, unit_id_type unit) {
	if constexpr(std::is_same_v<unit_id_type, dcon::army_id>) {
		return state.world.army_get_state_instance_vector_buffer_1(unit);
	}
	if constexpr(std::is_same_v<unit_id_type, dcon::navy_id>) {
		return state.world.navy_get_state_instance_vector_buffer_1(unit);
	}
}

template<concepts::construction_type construction_type>
auto construction_best_stockpiles_get(const sys::state& state, construction_type con) {
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_state_instance_vector_buffer_1(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_state_instance_vector_buffer_1(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::factory_construction_id>) {
		return state.world.factory_construction_get_state_instance_vector_buffer_1(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_building_construction_id>) {
		return state.world.province_building_construction_get_state_instance_vector_buffer_1(con);
	}
}
template<concepts::construction_type construction_type>
auto construction_best_stockpiles_get(sys::state& state, construction_type con) {
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_state_instance_vector_buffer_1(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_state_instance_vector_buffer_1(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::factory_construction_id>) {
		return state.world.factory_construction_get_state_instance_vector_buffer_1(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_building_construction_id>) {
		return state.world.province_building_construction_get_state_instance_vector_buffer_1(con);
	}
}

template<concepts::construction_type construction_type>
auto construction_needs_construction_goods(const sys::state& state, construction_type con) {
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_needs_construction_goods(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_needs_construction_goods(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::factory_construction_id>) {
		return state.world.factory_construction_get_needs_construction_goods(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_building_construction_id>) {
		return state.world.province_building_construction_get_needs_construction_goods(con);
	}
}

template<concepts::construction_type construction_type>
void construction_set_needs_construction_goods(sys::state& state, construction_type con, bool val) {
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_set_needs_construction_goods(con, val);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_set_needs_construction_goods(con, val);
	}
	if constexpr(std::is_same_v<construction_type, dcon::factory_construction_id>) {
		return state.world.factory_construction_set_needs_construction_goods(con, val);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_building_construction_id>) {
		return state.world.province_building_construction_set_needs_construction_goods(con, val);
	}
}

template<concepts::military_unit unit_type>
bool unit_needs_supply_goods(const sys::state& state, unit_type unit) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		return state.world.army_get_needs_supply_goods(unit);
	}
	if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		return state.world.navy_get_needs_supply_goods(unit);
	}
}
template<concepts::military_unit unit_type>
void unit_set_needs_supply_goods(sys::state& state, unit_type unit, bool val) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		return state.world.army_set_needs_supply_goods(unit, val);
	}
	if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		return state.world.navy_set_needs_supply_goods(unit, val);
	}
}

template<concepts::military_unit unit_type>
bool unit_needs_reinforcement_goods(const sys::state& state, unit_type unit) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		return state.world.army_get_needs_reinforcement_goods(unit);
	}
	if constexpr(std::is_same_v < unit_type, dcon::navy_id>) {
		return state.world.navy_get_needs_reinforcement_goods(unit);
	}
}

template<concepts::military_unit unit_type>
void unit_set_needs_reinforcement_goods(sys::state& state, unit_type unit, bool val) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		return state.world.army_set_needs_reinforcement_goods(unit, val);
	}
	if constexpr(std::is_same_v < unit_type, dcon::navy_id>) {
		return state.world.navy_set_needs_reinforcement_goods(unit, val);
	}
}



dcon::dcon_vv_fat_id<dcon::supply_route_path_id> supply_paths_by_market_get(sys::state& state, dcon::market_id market) {
	return state.world.market_get_supply_route_path_vector_buffer_1(market);
}
dcon::dcon_vv_const_fat_id<dcon::supply_route_path_id> supply_paths_by_market_get(const sys::state& state, dcon::market_id market) {
	return state.world.market_get_supply_route_path_vector_buffer_1(market);
}




dcon::province_id supply_route_get_destination(const sys::state& state, dcon::army_supply_route_id route) {
	return state.world.army_get_location_from_army_location(state.world.army_supply_route_get_army(route));
}
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::navy_supply_route_id route) {
	return state.world.navy_get_location_from_navy_location(state.world.navy_supply_route_get_navy(route));
}
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::land_construction_supply_route_id route) {
	dcon::province_land_construction_id construction = state.world.land_construction_supply_route_get_construction(route);
	return state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(construction));
}
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::naval_construction_supply_route_id route) {
	dcon::province_naval_construction_id construction = state.world.naval_construction_supply_route_get_construction(route);
	return state.world.province_naval_construction_get_province(construction);
}
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::factory_construction_supply_route_id route) {
	dcon::factory_construction_id construction = state.world.factory_construction_supply_route_get_construction(route);
	return state.world.factory_construction_get_province(construction);
}

dcon::province_id supply_route_get_destination(const sys::state& state, dcon::building_construction_supply_route_id route) {
	dcon::province_building_construction_id construction = state.world.building_construction_supply_route_get_construction(route);
	return state.world.province_building_construction_get_province(construction);
}

template<concepts::supply_route_type route_type>
float supply_route_get_supply_loss(const sys::state& state, route_type route) {
	dcon::supply_route_path_id path = supply_route_get_path(state, route);
	return state.world.supply_route_path_get_supply_loss(path);
}
template float supply_route_get_supply_loss(const sys::state& state, dcon::army_supply_route_id route);
template float supply_route_get_supply_loss(const sys::state& state, dcon::navy_supply_route_id route);
template float supply_route_get_supply_loss(const sys::state& state, dcon::land_construction_supply_route_id route);
template float supply_route_get_supply_loss(const sys::state& state, dcon::naval_construction_supply_route_id route);
template float supply_route_get_supply_loss(const sys::state& state, dcon::factory_construction_supply_route_id route);
template float supply_route_get_supply_loss(const sys::state& state, dcon::building_construction_supply_route_id route);

template<concepts::supply_route_type route_type>
float supply_route_get_throughput(const sys::state& state, route_type route) {
	dcon::supply_route_path_id path = supply_route_get_path(state, route);
	return state.world.supply_route_path_get_throughput(path);
}
template float supply_route_get_throughput(const sys::state& state, dcon::army_supply_route_id route);
template float supply_route_get_throughput(const sys::state& state, dcon::navy_supply_route_id route);
template float supply_route_get_throughput(const sys::state& state, dcon::land_construction_supply_route_id route);
template float supply_route_get_throughput(const sys::state& state, dcon::naval_construction_supply_route_id route);
template float supply_route_get_throughput(const sys::state& state, dcon::factory_construction_supply_route_id route);
template float supply_route_get_throughput(const sys::state& state, dcon::building_construction_supply_route_id route);

dcon::province_id supply_route_path_get_origin_prov(const sys::state& state, dcon::supply_route_path_id path) {
	auto origin = state.world.supply_route_path_get_origin(path);
	auto state_inst = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_inst);
}

template<concepts::supply_route_type route_type>
uint8_t supply_route_get_inactive_days(const sys::state& state, route_type route) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		return state.world.land_construction_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		return state.world.naval_construction_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<route_type, dcon::factory_construction_supply_route_id>) {
		return state.world.factory_construction_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<route_type, dcon::building_construction_supply_route_id>) {
		return state.world.building_construction_supply_route_get_inactive_days(route);
	}
}


template<concepts::ve_supply_route_type route_type>
ve::value_to_vector_type<uint8_t> supply_route_get_inactive_days(const sys::state& state, route_type route) {
	using ID = route_type::wrapped_value;
	if constexpr(std::is_same_v<ID, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<ID, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<ID, dcon::land_construction_supply_route_id>) {
		return state.world.land_construction_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<ID, dcon::naval_construction_supply_route_id>) {
		return state.world.naval_construction_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<ID, dcon::factory_construction_supply_route_id>) {
		return state.world.factory_construction_supply_route_get_inactive_days(route);
	} else if constexpr(std::is_same_v<ID, dcon::building_construction_supply_route_id>) {
		return state.world.building_construction_supply_route_get_inactive_days(route);
	}
}

template<concepts::supply_route_type route_type>
void supply_route_set_inactive_days(sys::state& state, route_type route, uint8_t val) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		state.world.army_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		state.world.navy_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		state.world.land_construction_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		state.world.naval_construction_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::factory_construction_supply_route_id>) {
		state.world.factory_construction_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::building_construction_supply_route_id>) {
		state.world.building_construction_supply_route_set_inactive_days(route, val);
	}
	else {
		static_assert(false, "Unsupported type");
	}
}

template<concepts::ve_supply_route_type route_type>
void supply_route_set_inactive_days(sys::state& state, route_type route, ve::value_to_vector_type<uint8_t> val) {

	using ID = route_type::wrapped_value;
	if constexpr(std::is_same_v<ID, dcon::army_supply_route_id>) {
		state.world.army_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<ID, dcon::navy_supply_route_id>) {
		state.world.navy_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<ID, dcon::land_construction_supply_route_id>) {
		state.world.land_construction_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<ID, dcon::naval_construction_supply_route_id>) {
		state.world.naval_construction_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<ID, dcon::factory_construction_supply_route_id>) {
		state.world.factory_construction_supply_route_set_inactive_days(route, val);
	} else if constexpr(std::is_same_v<ID, dcon::building_construction_supply_route_id>) {
		state.world.building_construction_supply_route_set_inactive_days(route, val);
	}
	else {
		static_assert(false, "Unsupported type");
	}
}

template<concepts::supply_route_type route_type>
void supply_route_set_is_active(sys::state& state, route_type route, bool val) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		state.world.army_supply_route_set_is_active(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		state.world.navy_supply_route_set_is_active(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		state.world.land_construction_supply_route_set_is_active(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		state.world.naval_construction_supply_route_set_is_active(route, val);
	} else if constexpr(std::is_same_v<route_type, dcon::factory_construction_supply_route_id>) {
		state.world.factory_construction_supply_route_set_is_active(route, val);;
	} else if constexpr(std::is_same_v<route_type, dcon::building_construction_supply_route_id>) {
		state.world.building_construction_supply_route_set_is_active(route, val);
	}
	else {
		static_assert(false, "Unsupported type");
	}
}

template<concepts::supply_route_type route_type>
bool supply_route_is_active(const sys::state& state, route_type route) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_get_is_active(route);
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_get_is_active(route);
	} else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		return state.world.land_construction_supply_route_get_is_active(route);
	} else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		return state.world.naval_construction_supply_route_get_is_active(route);
	} else if constexpr(std::is_same_v<route_type, dcon::factory_construction_supply_route_id>) {
		return state.world.factory_construction_supply_route_get_is_active(route);;
	} else if constexpr(std::is_same_v<route_type, dcon::building_construction_supply_route_id>) {
		return state.world.building_construction_supply_route_get_is_active(route);
	}
}
template bool supply_route_is_active(const sys::state& state, dcon::army_supply_route_id route);
template bool supply_route_is_active(const sys::state& state, dcon::navy_supply_route_id route);
template bool supply_route_is_active(const sys::state& state, dcon::land_construction_supply_route_id route);
template bool supply_route_is_active(const sys::state& state, dcon::naval_construction_supply_route_id route);
template bool supply_route_is_active(const sys::state& state, dcon::factory_construction_supply_route_id route);
template bool supply_route_is_active(const sys::state& state, dcon::building_construction_supply_route_id route);


template<concepts::ve_supply_route_type route_type>
ve::vbitfield_type supply_route_is_active(const sys::state& state, route_type route) {
	using ID = route_type::wrapped_value;
	if constexpr(std::is_same_v<ID, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_get_is_active(route);
	} else if constexpr(std::is_same_v<ID, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_get_is_active(route);
	} else if constexpr(std::is_same_v<ID, dcon::land_construction_supply_route_id>) {
		return state.world.land_construction_supply_route_get_is_active(route);
	} else if constexpr(std::is_same_v<ID, dcon::naval_construction_supply_route_id>) {
		return state.world.naval_construction_supply_route_get_is_active(route);
	} else if constexpr(std::is_same_v<ID, dcon::factory_construction_supply_route_id>) {
		return state.world.factory_construction_supply_route_get_is_active(route);;
	} else if constexpr(std::is_same_v<ID, dcon::building_construction_supply_route_id>) {
		return state.world.building_construction_supply_route_get_is_active(route);
	}
}


dcon::province_id supply_route_get_origin(const sys::state& state, dcon::army_supply_route_id route) {
	auto origin = state.world.army_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::navy_supply_route_id route) {
	auto origin = state.world.navy_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::land_construction_supply_route_id route) {
	auto origin = state.world.land_construction_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::naval_construction_supply_route_id route) {
	auto origin = state.world.naval_construction_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}

dcon::province_id supply_route_get_origin(const sys::state& state, dcon::factory_construction_supply_route_id route) {
	auto origin = state.world.factory_construction_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::building_construction_supply_route_id route) {
	auto origin = state.world.building_construction_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}

dcon::army_supply_route_id get_supply_route_by_origin_dest_pair(const sys::state& state, dcon::army_id unit_k, dcon::market_id market_k) {
	return state.world.get_army_supply_route_by_origin_army_pair(unit_k, market_k);
}
dcon::navy_supply_route_id get_supply_route_by_origin_dest_pair(const sys::state& state, dcon::navy_id unit_k, dcon::market_id market_k) {
	return state.world.get_navy_supply_route_by_origin_navy_pair(unit_k, market_k);
}
dcon::land_construction_supply_route_id get_supply_route_by_origin_dest_pair(const sys::state& state, dcon::province_land_construction_id con_k, dcon::market_id market_k) {
	return state.world.get_land_construction_supply_route_by_origin_construction_pair(con_k, market_k);
}
dcon::naval_construction_supply_route_id get_supply_route_by_origin_dest_pair(const sys::state& state, dcon::province_naval_construction_id con_k, dcon::market_id market_k) {
	return state.world.get_naval_construction_supply_route_by_origin_construction_pair(con_k, market_k);
}
dcon::factory_construction_supply_route_id get_supply_route_by_origin_dest_pair(const sys::state& state, dcon::factory_construction_id con_k, dcon::market_id market_k) {
	return state.world.get_factory_construction_supply_route_by_origin_construction_pair(con_k, market_k);
}
dcon::building_construction_supply_route_id get_supply_route_by_origin_dest_pair(const sys::state& state, dcon::province_building_construction_id con_k, dcon::market_id market_k) {
	return state.world.get_building_construction_supply_route_by_origin_construction_pair(con_k, market_k);
}

bool supply_route_path_is_active(const sys::state& state, dcon::supply_route_path_id path) {
	return state.world.supply_route_path_get_is_active(path);
}

static bool adj_province_pair_valid(const sys::state& state, dcon::province_adjacency_id prov_adj, dcon::province_id prov_1, dcon::province_id prov_2) {
	dcon::province_id adj_prov_1 = state.world.province_adjacency_get_connected_provinces(prov_adj, 0);
	dcon::province_id adj_prov_2 = state.world.province_adjacency_get_connected_provinces(prov_adj, 1);
	return (prov_1 != prov_2) && (adj_prov_1 == prov_1 || adj_prov_1 == prov_2) && (adj_prov_2 == prov_1 || adj_prov_2 == prov_2);
}


template<concepts::unit_supply_or_build_commodity_type commodity_type, concepts::military_supply_route_type route_type>
float military_route_get_buffered_goods(const sys::state& state, route_type route, commodity_type commodity_id) {
	auto fat_route = fatten(state.world, route);
	if constexpr(std::is_same_v<commodity_type, dcon::unit_supply_commodity_id>) {
		return fat_route.get_buffered_supply_goods(commodity_id);
	}
	else if constexpr(std::is_same_v<commodity_type, dcon::unit_build_commodity_id>) {
		return fat_route.get_buffered_reinforcement_goods(commodity_id);
	}
	
}
template float military_route_get_buffered_goods(const sys::state& state, dcon::army_supply_route_id route, dcon::unit_supply_commodity_id sup_id);
template float military_route_get_buffered_goods(const sys::state& state, dcon::navy_supply_route_id route, dcon::unit_supply_commodity_id sup_id);
template float military_route_get_buffered_goods(const sys::state& state, dcon::army_supply_route_id route, dcon::unit_build_commodity_id sup_id);
template float military_route_get_buffered_goods(const sys::state& state, dcon::navy_supply_route_id route, dcon::unit_build_commodity_id sup_id);

dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::army_supply_route_id route) {
	return state.world.army_get_controller_from_army_control(state.world.army_supply_route_get_army(route));
}
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::navy_supply_route_id route) {
	return state.world.navy_get_controller_from_navy_control(state.world.navy_supply_route_get_navy(route));
}
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::land_construction_supply_route_id route) {
	dcon::province_land_construction_id construction = state.world.land_construction_supply_route_get_construction(route);
	return state.world.province_land_construction_get_nation(construction);
}
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::naval_construction_supply_route_id route) {
	dcon::province_naval_construction_id construction = state.world.naval_construction_supply_route_get_construction(route);
	return state.world.province_naval_construction_get_nation(construction);
}

dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::factory_construction_supply_route_id route) {
	dcon::factory_construction_id construction = state.world.factory_construction_supply_route_get_construction(route);
	return state.world.factory_construction_get_nation(construction);
}
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::building_construction_supply_route_id route) {
	dcon::province_building_construction_id construction = state.world.building_construction_supply_route_get_construction(route);
	return state.world.province_building_construction_get_nation(construction);
}

dcon::nation_id supply_route_path_get_owner(const sys::state& state, dcon::supply_route_path_id path) {
	auto src_market = state.world.supply_route_path_get_origin(path);
	auto src_state = state.world.market_get_zone_from_local_market(src_market);
	return state.world.state_instance_get_nation_from_state_control(src_state);
}

template<concepts::military_supply_route_type route_type>
float unit_supply_route_get_buffered_supply_goods(const sys::state& state, route_type route, dcon::unit_supply_commodity_id com_id) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_get_buffered_supply_goods(route, com_id);
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_get_buffered_supply_goods(route, com_id);
	} 
}

template<concepts::military_supply_route_type route_type>
void unit_supply_route_set_buffered_supply_goods(sys::state& state, route_type route, dcon::unit_supply_commodity_id com_id, float val) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_set_buffered_supply_goods(route, com_id, val);
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_set_buffered_supply_goods(route, com_id, val);
	}
}

template<concepts::military_supply_route_type route_type>
float unit_supply_route_get_buffered_reinforcement_goods(const sys::state& state, route_type route, dcon::unit_build_commodity_id com_id) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_get_buffered_reinforcement_goods(route, com_id);
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_get_buffered_reinforcement_goods(route, com_id);
	}
}

template<concepts::military_supply_route_type route_type>
void unit_supply_route_set_buffered_reinforcement_goods(sys::state& state, route_type route, dcon::unit_build_commodity_id com_id, float val) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_set_buffered_reinforcement_goods(route, com_id, val);
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_set_buffered_reinforcement_goods(route, com_id, val);
	}
}


template<concepts::construction_supply_route_type route_type>
const economy::commodity_amounts& construction_supply_route_get_buffered_goods(const sys::state& state, route_type route) {
    if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		return state.world.land_construction_supply_route_get_buffered_goods(route);
	} else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		return state.world.naval_construction_supply_route_get_buffered_goods(route);
	} else if constexpr(std::is_same_v<route_type, dcon::factory_construction_supply_route_id>) {
		return state.world.factory_construction_supply_route_get_buffered_goods(route);
	} else if constexpr(std::is_same_v<route_type, dcon::building_construction_supply_route_id>) {
		return state.world.building_construction_supply_route_get_buffered_goods(route);
	}
}

template<concepts::construction_supply_route_type route_type>
economy::commodity_amounts& construction_supply_route_get_buffered_goods(sys::state& state, route_type route) {
	return const_cast<economy::commodity_amounts&>(construction_supply_route_get_buffered_goods(static_cast<const sys::state&>(state), route));
}

template<concepts::construction_supply_route_type route_type>
economy::commodity_amounts_u64& construction_supply_route_get_buffered_goods_temp_buffer(sys::state& state, route_type route) {
	return const_cast<economy::commodity_amounts_u64&>(construction_supply_route_get_buffered_goods_temp_buffer(static_cast<const sys::state&>(state), route));
}

dcon::province_land_construction_id construction_supply_route_get_construction(const sys::state& state, dcon::land_construction_supply_route_id route) {
	return state.world.land_construction_supply_route_get_construction(route);
}
dcon::province_naval_construction_id construction_supply_route_get_construction(const sys::state& state, dcon::naval_construction_supply_route_id route) {
	return state.world.naval_construction_supply_route_get_construction(route);
}
dcon::factory_construction_id construction_supply_route_get_construction(const sys::state& state, dcon::factory_construction_supply_route_id route) {
	return state.world.factory_construction_supply_route_get_construction(route);
}
dcon::province_building_construction_id construction_supply_route_get_construction(const sys::state& state, dcon::building_construction_supply_route_id route) {
	return state.world.building_construction_supply_route_get_construction(route);
}


// Default values is: min = 0, max = 100. Can be changed later to accomondate triggers or other restrictions

int8_t army_supply_setting_min(const sys::state& state, dcon::nation_id nation) {
	return 0;
}
int8_t army_supply_setting_max(const sys::state& state, dcon::nation_id nation) {
	return 100;
}
int8_t army_reinforcement_setting_min(const sys::state& state, dcon::nation_id nation) {
	return 0;
}
int8_t army_reinforcement_setting_max(const sys::state& state, dcon::nation_id nation) {
	return 100;
}
int8_t navy_supply_setting_min(const sys::state& state, dcon::nation_id nation) {
	return 0;
}
int8_t navy_supply_setting_max(const sys::state& state, dcon::nation_id nation) {
	return 100;
}
int8_t navy_reinforcement_setting_min(const sys::state& state, dcon::nation_id nation) {
	return 0;
}
int8_t navy_reinforcement_setting_max(const sys::state& state, dcon::nation_id nation) {
	return 100;
}
int8_t army_construction_setting_min(const sys::state& state, dcon::nation_id nation) {
	return 0;
}
int8_t army_construction_setting_max(const sys::state& state, dcon::nation_id nation) {
	return 100;
}
int8_t navy_construction_setting_min(const sys::state& state, dcon::nation_id nation) {
	return 0;
}
int8_t navy_construction_setting_max(const sys::state& state, dcon::nation_id nation) {
	return 100;
}
int8_t factory_construction_setting_min(const sys::state& state, dcon::nation_id nation) {
	return 0;
}
int8_t factory_construction_setting_max(const sys::state& state, dcon::nation_id nation) {
	return 100;
}
int8_t building_construction_setting_min(const sys::state& state, dcon::nation_id nation) {
	return 0;
}
int8_t building_construction_setting_max(const sys::state& state, dcon::nation_id nation) {
	return 100;
}

float supply_loss_to_commodity_loss(const sys::state& state, float base_loss_rate, float commodity_loss_mult) {
	return std::min(base_loss_rate * commodity_loss_mult, state.defines.alice_max_supply_route_loss);
}

float supply_loss_to_loss_multiplier(const sys::state& state, float base_loss_rate, float commodity_loss_mult) {
	// The base loss rate us expressed as the percent of commodities which will be lost from 0.0 to 1.0f (the value stored in the supply paths themselves). Is multiplied with the commodity-specific loss multiplier first
	// The resulting value is the inverse multiplier which can be used to figure out the number of goods will be able to be transported after loss
	float loss_multiplier = 1.0f - supply_loss_to_commodity_loss(state, base_loss_rate, commodity_loss_mult);
	assert(loss_multiplier >= 0.0f);
	return loss_multiplier;
}

void schedule_immediate_supply_path_update(sys::state& state, dcon::supply_route_path_id path) {
	state.world.supply_route_path_set_path_out_of_date(path, true);
}

void schedule_immediate_supply_path_update_on_origin_market(sys::state& state, dcon::market_id path_origin) {
	for(auto path : state.world.market_get_supply_route_path(path_origin)) {
		path.set_path_out_of_date(true);
	}
}

void schedule_prov_specific_nation_supply_paths_update(sys::state& state, dcon::province_id to_update, dcon::nation_id nation) {
	auto nations_to_update = state.world.province_get_nation_routes_to_be_updated(to_update);
	auto found = std::find(nations_to_update.begin(), nations_to_update.end(), nation);
	if(found == nations_to_update.end()) {
		nations_to_update.push_back(nation);
	}
}

void schedule_prov_all_supply_paths_update(sys::state& state, dcon::province_id to_update) {
	state.world.province_set_supply_routes_requires_update(to_update, true);
}
void schedule_prov_enemy_supply_paths_update(sys::state& state, dcon::province_id to_update, dcon::nation_id nation) {
	// Add enemy nations to the vector of nations whose routes need to be updated
	for(auto wa : state.world.nation_get_war_participant(nation)) {
		auto is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_is_attacker() != is_attacker) {
				schedule_prov_specific_nation_supply_paths_update(state, to_update, o.get_nation());
			}
		}
	}
}

void schedule_prov_common_war_supply_paths_update(sys::state& state, dcon::province_id to_update, dcon::nation_id nation) {
	// Add common war nations to the vector of nations whose routes need to be updated
	for(auto wa : state.world.nation_get_war_participant(nation)) {
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_nation() != nation) {
				schedule_prov_specific_nation_supply_paths_update(state, to_update, o.get_nation());
			}
		}
	}
}

void schedule_nation_supply_paths_update(sys::state& state, dcon::nation_id nation) {
	state.world.nation_set_supply_routes_requires_path_update(nation, true);
}

constexpr float ineffective_supply_path_throughput_cutoff = 0.9f;
constexpr float ineffective_supply_path_loss_cutoff = 0.35f;

void schedule_active_ineffective_supply_paths_update(sys::state& state) {
	state.world.for_each_supply_route_path([&](dcon::supply_route_path_id path_id) {
		float throughput = state.world.supply_route_path_get_throughput(path_id);
		float loss = state.world.supply_route_path_get_supply_loss(path_id);
		bool active = state.world.supply_route_path_get_is_active(path_id);
		// update it if throughput is less than 90%, or if loss is greater than 35%
		if(active && (throughput < ineffective_supply_path_throughput_cutoff || loss > ineffective_supply_path_loss_cutoff)) {
			schedule_immediate_supply_path_update(state, path_id);
		}
	});
}

float compute_efficiency(float consumed, float available) {
	if(consumed == 0.0f) {
		if(available == 0.0f) {
			return 0.0f;
		} else {
			return 1.0f;
		}
	} else {
		return std::min(available / consumed, 1.0f);
	}
}

void regenerate_unsaved_values(sys::state& state) {
	// Recreate adjacency paths from the stored province paths for all paths which are longer than 1 province
	parallel_for_each_supply_route_path_predicate(state, [&](dcon::supply_route_path_id path) { return state.world.supply_route_path_get_path(path).size() > 1; }, [&](dcon::supply_route_path_id path) {
		auto prov_path = state.world.supply_route_path_get_path(path);
		auto adj_path = state.world.supply_route_path_get_adjacency_path(path);
		province::make_adjacency_path_from_prov_path(state, prov_path, adj_path);
	});
}

float port_supply_capacity_mult_hostile_troops_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	float enemy_strength_present = military::army_strength_present<military::battle_included::yes, military::retreat_included::no, military::blackflag_included::no, military::participants_included::enemies>(state, prov, nation_as);
	return state.defines.alice_army_supply_throughput_blockade_threshold > 0.0f ? std::max((state.defines.alice_army_supply_throughput_blockade_threshold - enemy_strength_present) / state.defines.alice_army_supply_throughput_blockade_threshold, 0.f) : 1.0f;
}

float port_supply_capacity_mult_blockaded_modifier(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as) {
	assert(province::prov_is_coastal(state, port_prov));
	auto port_to_prov = state.world.province_get_port_to(port_prov);
	auto enemy_blockade_power = military::navy_strength_present<military::battle_included::yes, military::retreat_included::no, military::participants_included::enemies>(state, port_to_prov, nation_as);
	return state.defines.alice_navy_port_supply_capacity_blockade_threshold > 0.0f ? std::max((state.defines.alice_navy_port_supply_capacity_blockade_threshold - enemy_blockade_power) / state.defines.alice_navy_port_supply_capacity_blockade_threshold, 0.f) : 1.0f;
}
float port_supply_capacity_mult_supply_access_modifier(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as) {
	assert(province::prov_is_coastal(state, port_prov));
	bool has_access = province::has_supply_access_to_province(state, nation_as, port_prov);
	return has_access ? 1.0f : 0.0f;
}


float port_supply_capacity_in_province(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as) {
	assert(province::prov_is_coastal(state, port_prov));

	float access_mult = port_supply_capacity_mult_supply_access_modifier(state, port_prov, nation_as);
	if(access_mult != 0.0f) {
		float hostile_units_mult = port_supply_capacity_mult_hostile_troops_modifier(state, port_prov, nation_as);
		float capacity_add = state.world.province_get_modifier_values(port_prov, sys::provincial_mod_offsets::port_supply_capacity_add) + state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_port_supply_capacity_add);
		float capacity_percent = state.world.province_get_modifier_values(port_prov, sys::provincial_mod_offsets::port_supply_capacity_percent) + state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_port_supply_capacity_percent) + 1.0f;
		float capacity_mul = state.world.province_get_modifier_values(port_prov, sys::provincial_mod_offsets::port_supply_capacity_mul) * state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_port_supply_capacity_mul) * port_supply_capacity_mult_blockaded_modifier(state, port_prov, nation_as) * access_mult * hostile_units_mult;
		return std::max(capacity_add * capacity_percent * capacity_mul, 0.0f);
	}
	else {
		return 0.0f;
	}
}


float supply_throughput_mult_access_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	bool has_access = province::has_supply_access_to_province(state, nation_as, province);
	return has_access ? 1.0f : 0.0f;
}

float supply_throughput_mult_hostile_troops_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	assert(prov);
	assert(nation_as);

	if(province::is_sea(state, prov)) {
		return 1.0f; // Cannot have hostile troops in sea provinces. Blockades of ports are handled with a malus to port supply capacity, convoy raiding is handled as supply attrition
	} else {
		float enemy_strength_present = military::army_strength_present<military::battle_included::yes, military::retreat_included::no, military::blackflag_included::no, military::participants_included::enemies>(state, prov, nation_as);
		return state.defines.alice_army_supply_throughput_blockade_threshold > 0.0f ? std::max((state.defines.alice_army_supply_throughput_blockade_threshold - enemy_strength_present) / state.defines.alice_army_supply_throughput_blockade_threshold, 0.f) : 1.0f;
	}
}


float calculate_supply_throughput_in_province(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {

	float access_mul_mod = supply_throughput_mult_access_modifier(state, province, nation_as);
	if(access_mul_mod != 0.0f) {
		bool is_sea = province::is_sea(state, province);
		auto nation_add_mod = (is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_throughput_add) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_throughput_add));
		auto nation_percent_mod = (is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_throughput_percent) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_throughput_percent));
		auto nation_mul_mod = (is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_throughput_mul) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_throughput_mul));
		auto hostile_armies_mul_mod = (is_sea ? 1.0f : supply_throughput_mult_hostile_troops_modifier(state, province, nation_as));
		float add_modifiers = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_throughput_add) + nation_add_mod;
		float percent_modifiers = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_throughput_percent) + nation_percent_mod + 1.0f;
		float mult_modifiers = access_mul_mod * hostile_armies_mul_mod * state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_throughput_mul) * nation_mul_mod;
		return std::max(add_modifiers * percent_modifiers * mult_modifiers, 0.0f);
	}
	else {
		return 0.0f;
	}
}

float calculate_supply_throughput_in_noncoastal_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::province_id prov_1, dcon::province_id prov_2, float prov_1_throughput, float prov_2_throughput, dcon::nation_id nation) {
	assert(!province::adj_is_coastal(state, adj));
	assert(adj_province_pair_valid(state, adj, prov_1, prov_2));
	if(prov_1_throughput > 0.0f && prov_2_throughput > 0.0f) {
		return std::min(prov_1_throughput, prov_2_throughput);
	}
	// Special condition: Even if one edge of the adjacency has no throughput, allow it if there is a friendly army on the other side and the other edge has some throughput (balancing so that battles just inside enemy territory can be supplied if just near a friendly province)
	else if(prov_1_throughput > 0.0f && military::province_has_army<military::battle_included::yes, military::retreat_included::yes, military::blackflag_included::no, military::participants_included::ourselves>(state, prov_2, nation)) {
		return prov_1_throughput;
	} else if(prov_2_throughput > 0.0f && military::province_has_army<military::battle_included::yes, military::retreat_included::yes, military::blackflag_included::no, military::participants_included::ourselves>(state, prov_1, nation)) {
		return prov_2_throughput;
	} else {
		return 0.0f;
	}
}

float calculate_supply_throughput_in_noncoastal_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation) {
	float supply_throughput_1 = calculate_supply_throughput_in_province(state, prov_1, nation);
	float supply_throughput_2 = calculate_supply_throughput_in_province(state, prov_2, nation);
	return calculate_supply_throughput_in_noncoastal_adjacency(state, adj, prov_1, prov_2, supply_throughput_1, supply_throughput_2, nation);
}


float calculate_supply_throughput_in_coastal_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::province_id prov_1, dcon::province_id prov_2, float prov_1_capacity, float prov_2_capacity, dcon::nation_id nation) {
	assert(province::adj_is_coastal(state, adj));
	assert(adj_province_pair_valid(state, adj, prov_1, prov_2));
	auto port_1_port_to = state.world.province_get_port_to(prov_1);
	auto port_2_port_to = state.world.province_get_port_to(prov_2);
	bool port_1_is_port = (port_1_port_to == prov_2);
	bool port_2_is_port = (port_2_port_to == prov_1);
	if(port_1_is_port) {
		// use port supply capacity on prov 1 (the port prov)
		return prov_1_capacity;
	} else if(port_2_is_port) {
		// use port supply capacity on prov 2 (the port prov)
		return prov_2_capacity;
	} else {
		// There is no port connection between the two provinces
		return 0.0f;
	}
}

float calculate_supply_throughput_in_coastal_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation) {
	assert(province::adj_is_coastal(state, adj));
	assert(adj_province_pair_valid(state, adj, prov_1, prov_2));
	auto port_1_port_to = state.world.province_get_port_to(prov_1);
	auto port_2_port_to = state.world.province_get_port_to(prov_2);
	bool port_1_is_port = (port_1_port_to == prov_2);
	bool port_2_is_port = (port_2_port_to == prov_1);
	if(port_1_is_port) {
		// use port supply capacity on prov 1 (the port prov)
		return port_supply_capacity_in_province(state, prov_1, nation);
	} else if(port_2_is_port) {
		// use port supply capacity on prov 2 (the port prov)
		return port_supply_capacity_in_province(state, prov_2, nation);
	} else {
		// There is no port connection between the two provinces
		return 0.0f;
	}
}

float calculate_supply_throughput_in_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::nation_id nation) {
	bool coastal_adj = province::adj_is_coastal(state, adj);
	auto prov_1 = state.world.province_adjacency_get_connected_provinces(adj, 0);
	auto prov_2 = state.world.province_adjacency_get_connected_provinces(adj, 1);
	// Is coastal (land -> sea or sea -> land). Use port supply capacity compuation
	if(coastal_adj) {
		return calculate_supply_throughput_in_coastal_adjacency(state, adj, prov_1, prov_2, nation);
	}
	// Is not coastal (land -> land or sea -> sea). Use regular supply throughput compuation
	else {
		return calculate_supply_throughput_in_noncoastal_adjacency(state, adj, prov_1, prov_2, nation);
	}
}

float supply_throughput_efficiency(const sys::state& state, dcon::province_adjacency_id adj, dcon::nation_id nation_as) {
	assert(nation_as);
	auto prov_1 = state.world.province_adjacency_get_connected_provinces(adj, 0);
	auto prov_2 = state.world.province_adjacency_get_connected_provinces(adj, 1);
	float used_supply_throughput = state.world.province_adjacency_get_used_supply_throughput(adj);
	float throughput = calculate_supply_throughput_in_adjacency(state, adj, nation_as);
	return compute_efficiency(used_supply_throughput, throughput);
}

float supply_loss_add_hostile_armies(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	assert(province::is_land(state, province));
	return military::army_strength_present<military::battle_included::yes, military::retreat_included::no, military::blackflag_included::no, military::participants_included::enemies>(state, province, nation_as) * state.defines.alice_hostile_army_supply_loss;
}

float calculate_supply_loss_in_province(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	bool province_is_sea = province::is_sea(state, province);
	float national_add_mod = (province_is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_loss_add) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_loss_add));
	float national_percent_mod = (province_is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_loss_percent) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_loss_percent));
	float national_mul_mod = (province_is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_loss_mul) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_loss_mul));

	float hostile_units_add = (province_is_sea ? 0.0f : supply_loss_add_hostile_armies(state, province, nation_as));
	float add_mods = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_loss_add) + national_add_mod + hostile_units_add;
	float percent_mods = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_loss_percent) + national_percent_mod + 1.0f;
	float mul_mods = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_loss_mul) * national_mul_mod;
	return std::max(add_mods * percent_mods * mul_mods, 0.0f);
}

float calculate_adjacency_avg_supply_loss(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, float prov_1_loss, float prov_2_loss, dcon::nation_id nation_as) {
	auto avg_supply_attr = (prov_1_loss + prov_2_loss) / 2.0f;
	return avg_supply_attr;
}

float calculate_adjacency_avg_supply_loss(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as) {
	float prov_1_loss = logistics::calculate_supply_loss_in_province(state, prov_1, nation_as);
	float prov_2_loss = logistics::calculate_supply_loss_in_province(state, prov_2, nation_as);
	return calculate_adjacency_avg_supply_loss(state, prov_1, prov_2, prov_1_loss, prov_2_loss, nation_as);
}
float calculate_adjacency_avg_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as) {
	auto prov_1 = state.world.province_adjacency_get_connected_provinces(province_adj, 0);
	auto prov_2 = state.world.province_adjacency_get_connected_provinces(province_adj, 1);
	return calculate_adjacency_avg_supply_loss(state, prov_1, prov_2, nation_as);
}

float calculate_adjacency_net_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::province_id prov_1, dcon::province_id prov_2, float prov_1_loss, float prov_2_loss, dcon::nation_id nation_as) {
	assert(adj_province_pair_valid(state, province_adj, prov_1, prov_2));
	auto avg_supply_loss_per_km = calculate_adjacency_avg_supply_loss(state, prov_1, prov_2, prov_1_loss, prov_2_loss, nation_as) / state.map_state.map_data.world_circumference; // Get supply loss measured in loss per km
	auto distance = state.world.province_adjacency_get_distance_km(province_adj) * military::get_avg_movement_cost_modifier(state, nation_as, prov_1, prov_2);
	assert(std::isfinite(distance * avg_supply_loss_per_km));
	return distance * avg_supply_loss_per_km;
}

float calculate_adjacency_net_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as) {
	float prov_1_loss = logistics::calculate_supply_loss_in_province(state, prov_1, nation_as);
	float prov_2_loss = logistics::calculate_supply_loss_in_province(state, prov_2, nation_as);
	return calculate_adjacency_net_supply_loss(state, province_adj, prov_1, prov_2, prov_1_loss, prov_2_loss, nation_as);
}

float calculate_supply_route_throughput(const sys::state& state, std::span<const dcon::province_adjacency_id> adj_path, dcon::nation_id controller) {
	float smallest_supply_throughput = 1.0f;
	for(auto adj : adj_path) {
		float throughput_eff = supply_throughput_efficiency(state, adj, controller);
		smallest_supply_throughput = std::min(smallest_supply_throughput, throughput_eff);
	}
	return smallest_supply_throughput;
}

float calculate_supply_route_supply_loss(const sys::state& state, std::span<const dcon::province_adjacency_id> adj_path, dcon::province_id start_prov, dcon::nation_id controller) {
	assert(start_prov);

	float total_attrition_mod = 0.0f;
	// Keep track of the "last" and "next" province while iterating through the adjacencies, so that we need only compute 1+1*N supply loss computations, instead of 2*N
	dcon::province_id last_province = start_prov;
	float last_province_sup_loss = logistics::calculate_supply_loss_in_province(state, last_province, controller);
	for(auto adj : adj_path) {
		auto prov_1 = state.world.province_adjacency_get_connected_provinces(adj, 0);
		auto prov_2 = state.world.province_adjacency_get_connected_provinces(adj, 1);
		dcon::province_id next_province = (prov_1 == last_province ? prov_2 : prov_1);

		assert(adj_province_pair_valid(state, adj, last_province, next_province));

		float next_province_sup_loss = logistics::calculate_supply_loss_in_province(state, next_province, controller);
		total_attrition_mod += calculate_adjacency_net_supply_loss(state, adj, last_province, next_province, last_province_sup_loss, next_province_sup_loss, controller);
		last_province = next_province;
		last_province_sup_loss = next_province_sup_loss;
		assert(std::isfinite(total_attrition_mod));
	}
	return std::min(total_attrition_mod, state.defines.alice_max_supply_route_loss);
	
}


void add_used_supply_throughput(sys::state& state, std::span<const dcon::province_adjacency_id> adj_path, float volume) {
	for(auto adj : adj_path) {
		state.world.province_adjacency_set_used_supply_throughput(adj, state.world.province_adjacency_get_used_supply_throughput(adj) + volume);
	}
}


void update_supply_path_throughput_attrition(sys::state& state, dcon::supply_route_path_id path_handle, dcon::nation_id controller) {

	auto prov_path = state.world.supply_route_path_get_path(path_handle);
	assert(prov_path.size() > 0); // A valid province path should never be empty
	auto adj_path = state.world.supply_route_path_get_adjacency_path(path_handle);
	float old_throughput = state.world.supply_route_path_get_throughput(path_handle);
	float supply_loss;
	float new_throughput;
	// If adjacency path is greater than 0 then compute the throughput and loss. If size is 0 it must mean that the path is only a single province (and thus no edges). In that case there is no travel between edges needed, and throughput is always 100%, and loss 0%
	if(adj_path.size() > 0) {
		supply_loss = calculate_supply_route_supply_loss(state, adj_path, prov_path[0], controller);
		new_throughput = calculate_supply_route_throughput(state, adj_path, controller);
	}
	else {
		supply_loss = 0.0f;
		new_throughput = 1.0f; 
	}
	state.world.supply_route_path_set_expected_throughput(path_handle, (new_throughput + old_throughput) / 2.0f);
	state.world.supply_route_path_set_throughput(path_handle, new_throughput);
	state.world.supply_route_path_set_supply_loss(path_handle, supply_loss);
}


void update_supply_route_path(sys::state& state, dcon::supply_route_path_id supply_path, float expected_volume) {

	static thread_local std::vector<dcon::province_id> path{};
	static thread_local std::vector<dcon::province_adjacency_id> adj_path{};
	path.clear();
	adj_path.clear();
	dcon::province_id destiation = state.world.supply_route_path_get_destination(supply_path);
	auto origin = state.world.supply_route_path_get_origin(supply_path);
	auto state_inst = state.world.market_get_zone_from_local_market(origin);
	auto capital = state.world.state_instance_get_capital(state_inst);
	dcon::nation_id controller = state.world.state_instance_get_nation_from_state_control(state_inst);
	bool valid = province::make_military_supply_path(state, capital, destiation, controller, expected_volume, path, adj_path);
	state.world.supply_route_path_get_path(supply_path).load_range(path.data(), path.data() + path.size());
	state.world.supply_route_path_get_adjacency_path(supply_path).load_range(adj_path.data(), adj_path.data() + adj_path.size());
	state.world.supply_route_path_set_path_out_of_date(supply_path, false); // Whether or not the pathing suceeded, it is no longer out of date
	state.world.supply_route_path_set_valid_path(supply_path, valid);
}

template<concepts::supply_route_type route_type>
void delete_supply_route(sys::state& state, route_type route) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		state.world.delete_army_supply_route(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		state.world.delete_navy_supply_route(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		state.world.delete_land_construction_supply_route(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		state.world.delete_naval_construction_supply_route(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::factory_construction_supply_route_id>) {
		state.world.delete_factory_construction_supply_route(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::building_construction_supply_route_id>) {
		state.world.delete_building_construction_supply_route(route);
	}
	else {
		static_assert(false, "Unsupported route type");
	}
}

dcon::supply_route_path_id create_supply_route_path_no_pathing(sys::state& state, dcon::province_id destination, dcon::market_id origin, bool attempting_to_route) {
	auto state_inst = state.world.market_get_zone_from_local_market(origin);
	auto capital = state.world.state_instance_get_capital(state_inst);
	auto handle = state.world.force_create_supply_route_path(destination, origin);
	schedule_immediate_supply_path_update(state, handle); // Path starts out of date since no pathing was done this must be updated whenever possible
	state.world.supply_route_path_set_attempting_to_route(handle, attempting_to_route);
	state.world.supply_route_path_set_recently_created(handle, true);
	state.world.supply_route_path_set_valid_path(handle, false);
	state.world.supply_route_path_set_inactive_days(handle, 0);
	state.world.supply_route_path_set_throughput(handle, 1.0f);
	state.world.supply_route_path_set_expected_throughput(handle, 1.0f);
	state.world.supply_route_path_set_supply_loss(handle, 0.0f);
	state.world.supply_route_path_set_volume(handle, 0.0f);
	return handle;
}


dcon::army_route_path_id create_supply_route_path_relation(sys::state& state, dcon::army_supply_route_id route, dcon::supply_route_path_id sup_path) {
	return state.world.force_create_army_route_path(route, sup_path);
}
dcon::navy_route_path_id create_supply_route_path_relation(sys::state& state, dcon::navy_supply_route_id route, dcon::supply_route_path_id sup_path) {
	return state.world.force_create_navy_route_path(route, sup_path);
}
dcon::land_construction_route_path_id create_supply_route_path_relation(sys::state& state, dcon::land_construction_supply_route_id route, dcon::supply_route_path_id sup_path) {
	return state.world.force_create_land_construction_route_path(route, sup_path);
}
dcon::naval_construction_route_path_id create_supply_route_path_relation(sys::state& state, dcon::naval_construction_supply_route_id route, dcon::supply_route_path_id sup_path) {
	return state.world.force_create_naval_construction_route_path(route, sup_path);
}
dcon::factory_construction_route_path_id create_supply_route_path_relation(sys::state& state, dcon::factory_construction_supply_route_id route, dcon::supply_route_path_id sup_path) {
	return state.world.force_create_factory_construction_route_path(route, sup_path);
}
dcon::building_construction_route_path_id create_supply_route_path_relation(sys::state& state, dcon::building_construction_supply_route_id route, dcon::supply_route_path_id sup_path) {
	return state.world.force_create_building_construction_route_path(route, sup_path);
}


dcon::army_supply_route_id create_supply_route(sys::state& state, dcon::army_id unit, dcon::market_id origin) {
	auto id = fatten(state.world, state.world.force_create_army_supply_route(unit, origin));
	id.set_inactive_days(0);
	return id.id;
}

dcon::navy_supply_route_id create_supply_route(sys::state& state, dcon::navy_id unit, dcon::market_id origin) {
	auto id = fatten(state.world, state.world.force_create_navy_supply_route(unit, origin));
	id.set_inactive_days(0);
	return id.id;
}

dcon::land_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_land_construction_id con, dcon::market_id origin) {
	auto id = fatten(state.world, state.world.force_create_land_construction_supply_route(con, origin));
	auto type = state.world.province_land_construction_get_type(con);
	id.set_inactive_days(0);
	return id.id;
}

dcon::naval_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_naval_construction_id con, dcon::market_id origin) {
	auto id = fatten(state.world, state.world.force_create_naval_construction_supply_route(con, origin));
	auto type = state.world.province_naval_construction_get_type(con);
	id.set_inactive_days(0);
	return id.id;
}

dcon::factory_construction_supply_route_id create_supply_route(sys::state& state, dcon::factory_construction_id con, dcon::market_id origin) {
	auto id = fatten(state.world, state.world.force_create_factory_construction_supply_route(con, origin));
	auto type = state.world.factory_construction_get_type(con);
	id.set_inactive_days(0);
	return id.id;
}
dcon::building_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_building_construction_id con, dcon::market_id origin) {
	auto id = fatten(state.world, state.world.force_create_building_construction_supply_route(con, origin));
	auto type = state.world.province_building_construction_get_type(con);
	id.set_inactive_days(0);
	return id.id;
}
template<concepts::supply_route_type route_type>
dcon::market_id supply_route_get_origin_market(const sys::state& state, route_type route) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_get_origin(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_get_origin(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		return state.world.land_construction_supply_route_get_origin(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		return state.world.naval_construction_supply_route_get_origin(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::factory_construction_supply_route_id>) {
		return state.world.factory_construction_supply_route_get_origin(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::building_construction_supply_route_id>) {
		return state.world.building_construction_supply_route_get_origin(route);
	}
}
template dcon::market_id supply_route_get_origin_market(const sys::state& state, dcon::army_supply_route_id route);
template dcon::market_id supply_route_get_origin_market(const sys::state& state, dcon::navy_supply_route_id route);
template dcon::market_id supply_route_get_origin_market(const sys::state& state, dcon::land_construction_supply_route_id route);
template dcon::market_id supply_route_get_origin_market(const sys::state& state, dcon::naval_construction_supply_route_id route);
template dcon::market_id supply_route_get_origin_market(const sys::state& state, dcon::factory_construction_supply_route_id route);
template dcon::market_id supply_route_get_origin_market(const sys::state& state, dcon::building_construction_supply_route_id route);

template<concepts::supply_route_type route_type>
dcon::supply_route_path_id supply_route_get_path(const sys::state& state, route_type route) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_supply_route_get_path_from_army_route_path(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_supply_route_get_path_from_navy_route_path(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		return state.world.land_construction_supply_route_get_path_from_land_construction_route_path(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		return state.world.naval_construction_supply_route_get_path_from_naval_construction_route_path(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::factory_construction_supply_route_id>) {
		return state.world.factory_construction_supply_route_get_path_from_factory_construction_route_path(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::building_construction_supply_route_id>) {
		return state.world.building_construction_supply_route_get_path_from_building_construction_route_path(route);
	}
}
template dcon::supply_route_path_id supply_route_get_path(const sys::state& state, dcon::army_supply_route_id route);
template dcon::supply_route_path_id supply_route_get_path(const sys::state& state, dcon::navy_supply_route_id route);
template dcon::supply_route_path_id supply_route_get_path(const sys::state& state, dcon::land_construction_supply_route_id route);
template dcon::supply_route_path_id supply_route_get_path(const sys::state& state, dcon::naval_construction_supply_route_id route);
template dcon::supply_route_path_id supply_route_get_path(const sys::state& state, dcon::factory_construction_supply_route_id route);
template dcon::supply_route_path_id supply_route_get_path(const sys::state& state, dcon::building_construction_supply_route_id route);



// Internal function to accumulate supply loss for military units to perform later computations.
// Functor signature: void (dcon::commodity_id commodity, float amount, float loss). If loss is 1.0f then it is 100% loss, if 0.0f then no loss.
template<military::unit_consumption_type consume_type, concepts::military_unit unit_type, typename F>
static void accumulate_military_unit_supply_loss(const sys::state& state, unit_type unit, F&& accumulate_func) {
	auto routes = military::unit_get_supply_routes(state, unit);
	for(auto route : routes) {
		if(logistics::supply_route_is_active(state, route.id)) {
			float supply_loss = logistics::supply_route_get_supply_loss(state, route.id);
			if constexpr(consume_type == military::unit_consumption_type::supply) {
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com) {
					dcon::commodity_id base_com = economy::unit_commodity_get_base_commodity(state, com);
					float com_loss_mult = state.world.commodity_get_supply_loss_rate(base_com);
					float commodity_loss = supply_loss_to_commodity_loss(state, supply_loss, com_loss_mult);
					float buffered_amount = logistics::military_route_get_buffered_goods(state, route.id, com);
					accumulate_func(base_com, buffered_amount, commodity_loss);
				});
			} else if constexpr(consume_type == military::unit_consumption_type::reinforcement) {
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com) {
					dcon::commodity_id base_com = economy::unit_commodity_get_base_commodity(state, com);
					float com_loss_mult = state.world.commodity_get_supply_loss_rate(base_com);
					float commodity_loss = supply_loss_to_commodity_loss(state, supply_loss, com_loss_mult);
					float buffered_amount = logistics::military_route_get_buffered_goods(state, route.id, com);
					accumulate_func(base_com, buffered_amount, commodity_loss);
				});
			}
		}
	}
}

// Internal function to accumulate supply loss for constructions to perform later computations.
// Functor signature: void (dcon::commodity_id commodity, float amount, float loss). If loss is 1.0f then it is 100% loss, if 0.0f then no loss.
template<concepts::construction_type con_type, typename F>
static void accumulate_construction_supply_loss(const sys::state& state, con_type construction, F&& accumulate_func) {

	// Privately owned constructions do not use the logistics system, and thus no supply loss
	if(economy::construction_is_privately_owned(state, construction)) {
		return;
	}
	auto routes = economy::construction_get_supply_routes(state, construction);
	for(auto route : routes) {
		if(logistics::supply_route_is_active(state, route.id)) {
			float supply_loss = logistics::supply_route_get_supply_loss(state, route.id);
			const economy::commodity_set& base_cost = economy::construction_get_base_build_cost(state, construction);
			base_cost.for_each_valid_index([&](uint32_t idx) {
				dcon::commodity_id base_com = base_cost.commodity_type[idx];
				float com_loss_mult = state.world.commodity_get_supply_loss_rate(base_com);
				float commodity_loss = supply_loss_to_commodity_loss(state, supply_loss,  com_loss_mult);
				accumulate_func(base_com, base_cost.commodity_amounts[idx], commodity_loss);
			});
		}
	}
}




// Internal function to accumulate supply throughput for military units to perform later computations.
// Functor signature: void (dcon::commodity_id commodity, float amount, float throughput). If throughput is 1.0f then it has 100% throughput, if 0.0f then 0% throughput.
template<military::unit_consumption_type consume_type, concepts::military_unit unit_type, typename F>
static void accumulate_military_unit_supply_throughput(const sys::state& state, unit_type unit, F&& accumulate_func) {
	auto routes = military::unit_get_supply_routes(state, unit);
	for(auto route : routes) {
		if(logistics::supply_route_is_active(state, route.id)) {
			float supply_throughput = logistics::supply_route_get_throughput(state, route.id);
			if constexpr(consume_type == military::unit_consumption_type::supply) {
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com) {
					dcon::commodity_id base_com = economy::unit_commodity_get_base_commodity(state, com);
					float buffered_amount = logistics::military_route_get_buffered_goods(state, route.id, com);
					accumulate_func(base_com, buffered_amount, supply_throughput);
				});
			} else if constexpr(consume_type == military::unit_consumption_type::reinforcement) {
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com) {
					dcon::commodity_id base_com = economy::unit_commodity_get_base_commodity(state, com);
					float buffered_amount = logistics::military_route_get_buffered_goods(state, route.id, com);
					accumulate_func(base_com, buffered_amount, supply_throughput);
				});
			}
		}
	}
}

// Internal function to accumulate supply throughput for constructions to perform later computations.
// Functor signature: void (dcon::commodity_id commodity, float amount, float throughput). If throughput is 1.0f then it has 100% throughput, if 0.0f then 0% throughput.
template<concepts::construction_type con_type, typename F>
static void accumulate_construction_supply_throughput(const sys::state& state, con_type construction, F&& accumulate_func) {
	// Privately owned constructions do not use the logistics system, and thus no supply throughput
	if(economy::construction_is_privately_owned(state, construction)) {
		return;
	}
	auto routes = economy::construction_get_supply_routes(state, construction);
	for(auto route : routes) {
		if(logistics::supply_route_is_active(state, route.id)) {
			float supply_throughput = logistics::supply_route_get_throughput(state, route.id);
			const economy::commodity_set& base_cost = economy::construction_get_base_build_cost(state, construction);
			base_cost.for_each_valid_index([&](uint32_t idx) {
				dcon::commodity_id base_com = base_cost.commodity_type[idx];
				accumulate_func(base_com, base_cost.commodity_amounts[idx], supply_throughput);
			});
		}
	}
}

float nation_get_avg_supply_loss(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_lost = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float loss) {
		goods_total += amount;
		goods_lost += (amount * loss);
	};

	nations::nation_for_each_construction(state, nation, [&](auto construction) {
		accumulate_construction_supply_loss(state, construction, accumulator);
	});
	nations::nation_for_each_unit(state, nation, [&](auto unit) {
		accumulate_military_unit_supply_loss<military::unit_consumption_type::supply>(state, unit, accumulator);
		accumulate_military_unit_supply_loss<military::unit_consumption_type::reinforcement>(state, unit, accumulator);
	});
	return (goods_total == 0.0f ? 0.0f : goods_lost / goods_total);
}

float nation_get_avg_supply_throughput(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_transported = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float throughput) {
		goods_total += amount;
		goods_transported += (amount * throughput);
	};

	nations::nation_for_each_construction(state, nation, [&](auto construction) {
		accumulate_construction_supply_throughput(state, construction, accumulator);
	});
	nations::nation_for_each_unit(state, nation, [&](auto unit) {
		accumulate_military_unit_supply_throughput<military::unit_consumption_type::supply>(state, unit, accumulator);
		accumulate_military_unit_supply_throughput<military::unit_consumption_type::reinforcement>(state, unit, accumulator);
	});
	return (goods_total == 0.0f ? 1.0f : goods_transported / goods_total);
}

float nation_get_avg_construction_supply_loss(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_lost = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float loss) {
		goods_total += amount;
		goods_lost += (amount * loss);
	};

	nations::nation_for_each_construction(state, nation, [&](auto construction) {
		accumulate_construction_supply_loss(state, construction, accumulator);
	});
	return (goods_total == 0.0f ? 0.0f : goods_lost / goods_total);
}
float nation_get_avg_construction_supply_throughput(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_transported = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float throughput) {
		goods_total += amount;
		goods_transported += (amount * throughput);
	};

	nations::nation_for_each_construction(state, nation, [&](auto construction) {
		accumulate_construction_supply_throughput(state, construction, accumulator);
	});
	return (goods_total == 0.0f ? 1.0f : goods_transported / goods_total);
}


template<concepts::construction_type con_type>
float nation_get_avg_construction_supply_loss_by_type(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_lost = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float loss) {
		goods_total += amount;
		goods_lost += (amount * loss);
	};

	nations::nation_for_each_construction_by_type<con_type>(state, nation, [&](con_type construction) {
		accumulate_construction_supply_loss(state, construction, accumulator);
	});
	return (goods_total == 0.0f ? 0.0f : goods_lost / goods_total);
}
template float nation_get_avg_construction_supply_loss_by_type<dcon::province_land_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_construction_supply_loss_by_type<dcon::province_naval_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_construction_supply_loss_by_type<dcon::factory_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_construction_supply_loss_by_type<dcon::province_building_construction_id>(const sys::state& state, dcon::nation_id nation);

template<concepts::construction_type con_type>
float nation_get_avg_construction_supply_throughput_by_type(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_transported = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float throughput) {
		goods_total += amount;
		goods_transported += (amount * throughput);
	};

	nations::nation_for_each_construction_by_type<con_type>(state, nation, [&](con_type construction) {
		accumulate_construction_supply_throughput(state, construction, accumulator);
	});
	return (goods_total == 0.0f ? 1.0f : goods_transported / goods_total);
}
template float nation_get_avg_construction_supply_throughput_by_type<dcon::province_land_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_construction_supply_throughput_by_type<dcon::province_naval_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_construction_supply_throughput_by_type<dcon::factory_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_construction_supply_throughput_by_type<dcon::province_building_construction_id>(const sys::state& state, dcon::nation_id nation);

template<military::unit_consumption_type consume_type, concepts::military_unit unit_type>
float nation_get_avg_military_supply_loss_by_type(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_lost = 0.0f;
	nations::nation_for_each_unit_by_type<unit_type>(state, nation, [&](unit_type unit) {
		accumulate_military_unit_supply_loss<consume_type>(state, unit, [&](dcon::commodity_id, float amount, float loss) {
			goods_total += amount;
			goods_lost += (amount * loss);
		});
	});
	return (goods_total == 0.0f ? 0.0f : goods_lost / goods_total);
}
template float nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::supply, dcon::army_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::reinforcement, dcon::army_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::supply, dcon::navy_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::reinforcement, dcon::navy_id>(const sys::state& state, dcon::nation_id nation);

template<military::unit_consumption_type consume_type, concepts::military_unit unit_type>
float nation_get_avg_military_supply_throughput_by_type(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_transported = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float loss) {
		goods_total += amount;
		goods_transported += (amount * loss);
	};
	nations::nation_for_each_unit_by_type<unit_type>(state, nation, [&](unit_type unit) {
		accumulate_military_unit_supply_throughput<consume_type>(state, unit, accumulator);
	});
	return (goods_total == 0.0f ? 1.0f : goods_transported / goods_total);
}
template float nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::supply, dcon::army_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::reinforcement, dcon::army_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::supply, dcon::navy_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::reinforcement, dcon::navy_id>(const sys::state& state, dcon::nation_id nation);


template<concepts::military_unit unit_type>
float nation_get_avg_military_supply_loss_by_type(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_lost = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float loss) {
		goods_total += amount;
		goods_lost += (amount * loss);
	};
	nations::nation_for_each_unit_by_type<unit_type>(state, nation, [&](unit_type unit) {
		accumulate_military_unit_supply_loss<military::unit_consumption_type::supply>(state, unit, accumulator);
		accumulate_military_unit_supply_loss<military::unit_consumption_type::reinforcement>(state, unit, accumulator);
	});
	return (goods_total == 0.0f ? 0.0f : goods_lost / goods_total);
}
template float nation_get_avg_military_supply_loss_by_type<dcon::army_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_military_supply_loss_by_type<dcon::navy_id>(const sys::state& state, dcon::nation_id nation);

template<concepts::military_unit unit_type>
float nation_get_avg_military_supply_throughput_by_type(const sys::state& state, dcon::nation_id nation) {
	float goods_total = 0.0f;
	float goods_transported = 0.0f;

	auto accumulator = [&](dcon::commodity_id, float amount, float loss) {
		goods_total += amount;
		goods_transported += (amount * loss);
	};
	nations::nation_for_each_unit_by_type<unit_type>(state, nation, [&](unit_type unit) {
		accumulate_military_unit_supply_throughput<military::unit_consumption_type::supply>(state, unit, accumulator);
		accumulate_military_unit_supply_throughput<military::unit_consumption_type::reinforcement>(state, unit, accumulator);
	});
	return (goods_total == 0.0f ? 1.0f : goods_transported / goods_total);
}
template float nation_get_avg_military_supply_throughput_by_type<dcon::army_id>(const sys::state& state, dcon::nation_id nation);
template float nation_get_avg_military_supply_throughput_by_type<dcon::navy_id>(const sys::state& state, dcon::nation_id nation);

// Updates the supply route buffered goods, volume and subtracts the goods consumed from the stockpile buffers of all potential supply routes connected to the military unit.
// Supply type decides whether to update supply, or reinforcement goods
// It is assumed that the unit requires atleast some amount of this commodity before calling
template<concepts::military_unit unit_type, concepts::unit_supply_or_build_commodity_type unit_commodity_type>
void update_military_unit_routes_satisfaction(sys::state& state, unit_type unit,  dcon::nation_id nation, unit_commodity_type unit_com_id, dcon::commodity_id commodity) {

	auto get_remaining_goods_required = [&]() -> float{
		if constexpr(std::is_same_v<unit_commodity_type, dcon::unit_supply_commodity_id>) {
			return unit_supply_need_get(state, unit, unit_com_id);
		} else if constexpr(std::is_same_v<unit_commodity_type, dcon::unit_build_commodity_id>) {
			return unit_reinforcement_need_get(state, unit, unit_com_id);
		}
	};
	float com_supply_weight = state.world.commodity_get_supply_weight(commodity);
	dcon::province_id unit_location = military::unit_get_location(state, unit);

	auto stockpiles_buffer = unit_best_stockpiles_get(state, unit);
	for(auto stockpile_state : stockpiles_buffer) {

		dcon::market_id market = state.world.state_instance_get_market_from_local_market(stockpile_state);
		auto origin_prov = state.world.state_instance_get_capital(stockpile_state);

		float available_stockpile_amount = local_stockpile_available_goods_get(state, market, commodity);

		if(available_stockpile_amount == 0.f) {
			continue;
		}

		auto route = get_supply_route_by_origin_dest_pair(state, unit, market);

		if(!route) {
			route = create_supply_route(state, unit, market);
		}
		dcon::supply_route_path_id path = supply_route_get_path(state, route);
		if(!path) {
			// First, try to find a path which already exists here
			path = state.world.get_supply_route_path_by_origin_destination_pair(unit_location, market);
			if(!path) {
				// otherwise, create one
				path = create_supply_route_path_no_pathing(state, unit_location, market, true);
			}
			create_supply_route_path_relation(state, route, path);
		}
		state.world.supply_route_path_set_attempting_to_route(path, true);  // We are attempting to move goods through this path, whether its valid or not
		bool path_is_valid = state.world.supply_route_path_get_valid_path(path);
		bool recently_created = state.world.supply_route_path_get_recently_created(path);
		// We do want to reserve goods for invalid paths ONLY if it is a new path (as it may very well be valid later). If its an old path which is invalid, don't bother and wait till it becomes valid
		// The path may have been created today earlier
		if(!recently_created && !path_is_valid) {
			continue;
		}
		float remaining_goods_required = get_remaining_goods_required();
		float to_consume = std::min(remaining_goods_required, available_stockpile_amount) * state.world.supply_route_path_get_expected_throughput(path); // Consume less if the expected throughput of this route is lower than 100%

		// The amount to consume is the minimum of the desired amount or the amount available in stockpile
		// Compute how much to consume to compensate for the expected loss on the route.
		assert(available_stockpile_amount - to_consume >= 0.0f);
		// Update stockpile buffer to reflect the amount that will be subtracted later
		local_stockpile_available_goods_set(state, market, commodity, available_stockpile_amount - to_consume);
		float total_stockpile_buffer_count = nation_stockpile_available_goods_get(state, nation, commodity);
		nation_stockpile_available_goods_set(state, nation, commodity, std::max(total_stockpile_buffer_count - to_consume, 0.0f));
		auto fat_route = fatten(state.world, route);

		// Add to the buffered goods, and subtract from route need
		if constexpr(std::is_same_v<unit_commodity_type, dcon::unit_supply_commodity_id>) {
			fat_route.set_buffered_supply_goods(unit_com_id, fat_route.get_buffered_supply_goods(unit_com_id) + to_consume);
			unit_supply_need_set(state, unit, unit_com_id, remaining_goods_required - to_consume);
		}
		else if constexpr(std::is_same_v<unit_commodity_type, dcon::unit_build_commodity_id>) {
			fat_route.set_buffered_reinforcement_goods(unit_com_id, fat_route.get_buffered_reinforcement_goods(unit_com_id) + to_consume);
			unit_reinforcement_need_set(state, unit, unit_com_id, remaining_goods_required - to_consume);
		}
		state.world.supply_route_path_set_volume(path, state.world.supply_route_path_get_volume(path) + (to_consume * com_supply_weight));
		supply_route_set_is_active(state, route, true);
		state.world.supply_route_path_set_is_active(path, true);
		if(get_remaining_goods_required() == 0.0f) {
			break; // Check remaining goods required now after subtraction. If its zero then we can move on
		}
	}

}
uint32_t supply_route_path_get_connected_routes(const sys::state& state, dcon::supply_route_path_id path) {
	auto army_routes = state.world.supply_route_path_get_army_route_path(path);
	auto navy_routes = state.world.supply_route_path_get_navy_route_path(path);
	auto land_construction_routes = state.world.supply_route_path_get_land_construction_route_path(path);
	auto naval_construction_routes = state.world.supply_route_path_get_naval_construction_route_path(path);
	auto factory_construction_routes = state.world.supply_route_path_get_factory_construction_route_path(path);
	auto building_construction_routes = state.world.supply_route_path_get_building_construction_route_path(path);
	return uint32_t((army_routes.end() - army_routes.begin()) + (navy_routes.end() - navy_routes.begin()) + (land_construction_routes.end() - land_construction_routes.begin()) + (naval_construction_routes.end() - naval_construction_routes.begin()) + (factory_construction_routes.end() - factory_construction_routes.begin()) + (building_construction_routes.end() - building_construction_routes.begin()));
}

constexpr uint32_t supply_route_inactive_days_before_deletion = 10;
constexpr uint32_t supply_path_inactive_days_before_deletion = 50;

template<concepts::supply_route_type route_type>
bool should_delete_route(const sys::state& state, route_type route) {
	auto fat_route = fatten(state.world, route);
	dcon::nation_id route_owner = supply_route_get_owner(state, route);
	dcon::province_id route_origin = supply_route_get_origin(state, route);
	dcon::nation_id route_origin_controller = state.world.province_get_nation_from_province_control(route_origin);
	// A supply route shall be deleted if it has been inactive for some days or more, OR if the route owner does not control the stockpile the route is connected to
	return fat_route.get_inactive_days() >= supply_route_inactive_days_before_deletion || route_owner != route_origin_controller;
}
bool should_delete_path(const sys::state& state, dcon::supply_route_path_id path) {
	uint8_t inactive_days = state.world.supply_route_path_get_inactive_days(path);
	// A supply route shall be deleted if it has been inactive for some days or more
	return inactive_days >= supply_path_inactive_days_before_deletion;
}




// Updates the supply route buffered goods, volume and subtracts the goods consumed from the stockpile buffers of all potential supply routes connected to the military unit.
// Supply type decides whether to update supply, or reinforcement goods
// It is assumed that the construction requires atleast some amount of this commodity before calling
template<concepts::construction_type construction_type>
void update_construction_routes_satisfaction(sys::state& state, construction_type conc, dcon::nation_id nation, uint32_t set_index, dcon::commodity_id commodity) {

	economy::commodity_amounts& construction_need = constructions_need_get(state, conc);

	dcon::province_id con_location = economy::construction_get_location(state, conc);

	float com_supply_weight = state.world.commodity_get_supply_weight(commodity);

	auto stockpiles_buffer = construction_best_stockpiles_get(state, conc);
	for(auto stockpile_state : stockpiles_buffer) {

		dcon::market_id market = state.world.state_instance_get_market_from_local_market(stockpile_state);
		dcon::province_id origin_prov = state.world.state_instance_get_capital(stockpile_state);

		float available_stockpile_amount = local_stockpile_available_goods_get(state, market, commodity);
		// Is there anything to consume here?
		if(available_stockpile_amount == 0.0f) {
			continue;
		}

		auto route = get_supply_route_by_origin_dest_pair(state, conc, market);

		if(!route) {
			route = create_supply_route(state, conc, market);
		}
		dcon::supply_route_path_id path = supply_route_get_path(state, route);
		if(!path) {
			// First, try to find a path which already exists here
			path = state.world.get_supply_route_path_by_origin_destination_pair(con_location, market);
			if(!path) {
				// otherwise, create one
				path = create_supply_route_path_no_pathing(state, con_location, market, true);
			}
			create_supply_route_path_relation(state, route, path);
		}
		state.world.supply_route_path_set_attempting_to_route(path, true);  // We are attempting to move goods through this path, whether its valid or not
		bool path_is_valid = state.world.supply_route_path_get_valid_path(path);
		bool recently_created = state.world.supply_route_path_get_recently_created(path);
		// We do want to reserve goods for invalid paths ONLY if it is a new path (as it may very well be valid later). If its an old path which is invalid, don't bother and wait till it becomes valid
		// The path may have been created today earlier
		if(!recently_created && !path_is_valid) {
			continue;
		}
		float remaining_goods_required = construction_need[set_index];
		float to_consume = std::min(remaining_goods_required, available_stockpile_amount) * state.world.supply_route_path_get_expected_throughput(path); // Consume less if the expected throughput of this route is lower than 100%

		// The amount to consume is the minimum of the desired amount or the amount available in stockpile
		// Compute how much to consume to compensate for the expected loss on the route.
		assert(available_stockpile_amount - to_consume >= 0.0f);
		// Update stockpile buffer to reflect the amount that will be subtracted later
		local_stockpile_available_goods_set(state, market, commodity, available_stockpile_amount - to_consume);
		float total_stockpile_buffer_count = nation_stockpile_available_goods_get(state, nation, commodity);
		nation_stockpile_available_goods_set(state, nation, commodity, std::max(total_stockpile_buffer_count - to_consume, 0.0f));
		auto fat_route = fatten(state.world, route);

		economy::commodity_amounts& buffered_goods = fat_route.get_buffered_goods();
		buffered_goods[set_index] += to_consume;

		// Subtract from route need
		construction_need[set_index] -= to_consume;

		state.world.supply_route_path_set_volume(path, state.world.supply_route_path_get_volume(path) + (to_consume * com_supply_weight));
		supply_route_set_is_active(state, route, true);
		state.world.supply_route_path_set_is_active(path, true);
		if(construction_need[set_index] == 0.0f) {
			break; // Dont need any more of this commodity
		}

	}
}

template<concepts::military_unit unit_type>
void update_unit_commodity_satisfaction(sys::state& state, unit_type u) {
	auto unit = fatten(state.world, u);
	auto unit_membership = military::unit_get_membership(state, u);
	dcon::nation_id nation = military::unit_get_controller(state, unit.id);
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		// Rebels always have no supply or reinforcement for now
		if(!nation) {
			for(auto r : unit_membership) {
				auto reg = r.get_regiment();
				reg.set_supply_satisfaction(0.0f);
				reg.set_reinforcement_satisfaction(0.0f);
				reg.set_total_pending_reinforcement(0.0f);
			}
			return;
		}
	}
	auto routes = military::unit_get_supply_routes(state, unit.id);

	static thread_local ve::vectorizable_buffer<float, dcon::unit_supply_commodity_id> available_supply_goods_buffer(state.world.unit_supply_commodity_size());
	{
		static thread_local uint32_t old_count = state.world.unit_supply_commodity_size();
		auto new_count = state.world.unit_supply_commodity_size();
		if(new_count > old_count) {
			available_supply_goods_buffer = state.world.unit_supply_commodity_make_vectorizable_float_buffer();
			old_count = new_count;
		}
	}
	static thread_local ve::vectorizable_buffer<float, dcon::unit_build_commodity_id> available_reinforcement_goods_buffer(state.world.unit_build_commodity_size());
	{
		static thread_local uint32_t old_count = state.world.unit_build_commodity_size();
		auto new_count = state.world.unit_build_commodity_size();
		if(new_count > old_count) {
			available_reinforcement_goods_buffer = state.world.unit_build_commodity_make_vectorizable_float_buffer();
			old_count = new_count;
		}
	}
	state.world.execute_serial_over_unit_supply_commodity([&](auto ids) {
		available_supply_goods_buffer.set(ids, 0.0f);
	});
	state.world.execute_serial_over_unit_build_commodity([&](auto ids) {
		available_reinforcement_goods_buffer.set(ids, 0.0f);
	});

	// Sum up available supply and reinforcement goods from routes
	for(auto route : routes) {
		if(supply_route_is_active(state, route.id)) {
			float throughput = supply_route_get_throughput(state, route.id);
			float supply_loss = supply_route_get_supply_loss(state, route.id);
			state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
				dcon::commodity_id base_commodity = economy::unit_commodity_get_base_commodity(state, com_id);
				float com_supply_loss_mod = state.world.commodity_get_supply_loss_rate(base_commodity);
				float loss_mult = supply_loss_to_loss_multiplier(state, supply_loss, com_supply_loss_mod);
				float current_avail = available_supply_goods_buffer.get(com_id);
				float buffered_amount = route.get_buffered_supply_goods(com_id);
				available_supply_goods_buffer.set(com_id, current_avail + (buffered_amount * throughput * loss_mult));
			});
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				dcon::commodity_id base_commodity = economy::unit_commodity_get_base_commodity(state, com_id);
				float com_supply_loss_mod = state.world.commodity_get_supply_loss_rate(base_commodity);
				float loss_mult = supply_loss_to_loss_multiplier(state, supply_loss, com_supply_loss_mod);
				float current_avail = available_reinforcement_goods_buffer.get(com_id);
				float buffered_amount = route.get_buffered_reinforcement_goods(com_id);
				available_reinforcement_goods_buffer.set(com_id, current_avail + (buffered_amount * throughput * loss_mult));
			});
		}
	}

	// Consume supplies or reinforcement goods
	for(auto r : unit_membership) {
		auto subunit = [&]() {
			if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
				return r.get_regiment();
			} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
				return r.get_ship();
			}
		}();
		// Compute supply satisfaction
		dcon::unit_type_id type = subunit.get_type();
		{
			// Compute supply satisfaction
			float supply_goods_cost_mod = military::subunit_get_last_required_supply_percent_base_cost(state, subunit.id);
			float supply_consumption_setting = static_cast<float>(nations::get_nation_military_consumption_setting_by_type<decltype(u), military::unit_consumption_type::supply>(state, nation)) / 100.0f;
			float desired_supply_mult = supply_goods_cost_mod * supply_consumption_setting;

			const economy::commodity_set& supply_goods_cost = military::unit_type_get_commodity_costs<military::unit_consumption_type::supply>(state, type);
			float total_supply_goods_required = 0.0f;
			float total_supply_goods_consumed = 0.0f;
			// required_amount is what we need to get 100% supply satisfaction, desired amount is what we actually wish to consume decided by national consumption setting
			supply_goods_cost.for_each_commodity([&](dcon::commodity_id com_id, float base_amount) {
				auto sup_com_id = state.world.commodity_get_unit_supply_commodity(com_id);
				assert(sup_com_id);
				float required_amount = base_amount * supply_goods_cost_mod;
				float desired_amount = base_amount * desired_supply_mult;
				float max_available = available_supply_goods_buffer.get(sup_com_id);
				float to_consume = std::min(max_available, desired_amount);
				assert(max_available - to_consume >= 0.0f);
				available_supply_goods_buffer.set(sup_com_id, max_available - to_consume);
				total_supply_goods_required += required_amount;
				total_supply_goods_consumed += to_consume;
			});
			float supply_satisfaction = (total_supply_goods_required == 0.0f ? 1.0f : total_supply_goods_consumed / total_supply_goods_required);
			subunit.set_supply_satisfaction(supply_satisfaction);
		}
		// Compute reinforcement satisfaction
		{
			// And then compute reinforcement satisfaction
			const economy::commodity_set& reinf_goods_cost = military::unit_type_get_commodity_costs<military::unit_consumption_type::reinforcement>(state, type);
			// The reinforcement amount (ranges from 0.0-1.0) is also the cost modifier, as the faster it can reinforce, the more goods we need to fufill it at optimal speed. Here we get the reinforcement amount under perfect conditions (100% fufillment)
			float reinf_goods_cost_mod = military::subunit_get_last_required_reinforcement_percent_base_cost(state, subunit.id);
			float reinf_consumption_setting = static_cast<float>(nations::get_nation_military_consumption_setting_by_type<decltype(u), military::unit_consumption_type::reinforcement>(state, nation)) / 100.0f;
			float desired_reinf_mult = reinf_goods_cost_mod * reinf_consumption_setting;

			float total_reinf_goods_required = 0.0f;
			float total_reinf_goods_consumed = 0.0f;
			reinf_goods_cost.for_each_commodity([&](dcon::commodity_id com_id, float base_amount) {
				auto build_com_id = state.world.commodity_get_unit_build_commodity(com_id);
				assert(build_com_id);
				float required_amount = base_amount * reinf_goods_cost_mod;
				float desired_amount = base_amount * desired_reinf_mult;
				float max_available = available_reinforcement_goods_buffer.get(build_com_id);
				float to_consume = std::min(max_available, desired_amount);
				assert(max_available - to_consume >= 0.0f);
				available_reinforcement_goods_buffer.set(build_com_id, max_available - to_consume);
				total_reinf_goods_required += required_amount;
				total_reinf_goods_consumed += to_consume;
			});
			float reinf_satisfaction = (total_reinf_goods_required == 0.0f ? 1.0f : total_reinf_goods_consumed / total_reinf_goods_required);
			subunit.set_reinforcement_satisfaction(reinf_satisfaction);
			if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
				// For armies, we accumulate the reinforcement first and then apply it monthly (for balance reasons), whereas navies repair once a day, so navies can skip this section
				float added_pending_reinforcement = reinf_satisfaction * reinf_goods_cost_mod;
				subunit.set_total_pending_reinforcement(subunit.get_total_pending_reinforcement() + added_pending_reinforcement);
				assert(std::isfinite(subunit.get_total_pending_reinforcement()));
			}
		}
	}
}
template<concepts::construction_type construction_type>
void update_construction_commodity_satisfaction(sys::state& state, construction_type c) {
	dcon::province_id location = economy::construction_get_location(state, c);
	auto routes = economy::construction_get_supply_routes(state, c);
	dcon::nation_id nation = economy::construction_get_controller(state, c);
	economy::commodity_set& current_fufilled = economy::construction_get_purchased_goods(state, c);
	const economy::commodity_set& build_costs = economy::construction_get_base_build_cost(state, c);
	for(auto route : routes) {
		if(supply_route_is_active(state, route.id)) {
			float throughput = logistics::supply_route_get_throughput(state, route.id);
			float supply_loss = logistics::supply_route_get_supply_loss(state, route.id);
			const economy::commodity_amounts& route_goods = route.get_buffered_goods();
			for(uint32_t j = 0; j < build_costs.set_size; j++) {
				dcon::commodity_id com_id = build_costs.commodity_type[j];
				assert(build_costs.commodity_type[j] == current_fufilled.commodity_type[j]);
				if(com_id) {
					float com_supply_loss_mult = state.world.commodity_get_supply_loss_rate(com_id);
					float loss_mult = supply_loss_to_loss_multiplier(state, supply_loss, com_supply_loss_mult);
					float& current_amount = current_fufilled.commodity_amounts[j];
					float route_amount = route_goods[j] * throughput * loss_mult;
					current_amount += route_amount;
				} else {
					break;
				}
			}
		}
	}
}

void setup_spread_supply_path_batches(sys::state& state, uint32_t num_path_batches, std::vector<std::vector<dcon::supply_route_path_id>>& path_batches) {
	// Buffer all paths which are active and require an update, and index them by origin market
	uint32_t total_updatable_paths = 0;
	state.world.for_each_supply_route_path([&](dcon::supply_route_path_id path) {
		bool attempting_to_route_goods = state.world.supply_route_path_get_attempting_to_route(path);
		bool path_out_of_date = state.world.supply_route_path_get_path_out_of_date(path);
		if(attempting_to_route_goods && path_out_of_date) {
			dcon::market_id origin = state.world.supply_route_path_get_origin(path);
			auto paths_by_market = supply_paths_by_market_get(state, origin);
			paths_by_market.push_back(path);
			total_updatable_paths++;
		}
	});
	// Compute count per batch, aswell as remainder and if an extra remainder-batch is needed
	uint32_t count_per_batch;
	uint32_t remainder;
	// Always atleast one per batch minimum
	if(total_updatable_paths < num_path_batches) {
		count_per_batch = 1;
		remainder = 0;
	}
	else {
		count_per_batch = total_updatable_paths / num_path_batches;
		remainder = total_updatable_paths % num_path_batches;
	}
	if(remainder != 0) {
		// Create extra batches for remainders
		num_path_batches += (remainder / count_per_batch + 1);
	}
	// Compute count per batch, aswell as remainder and if an extra remainder-batch is needed
	path_batches.resize(num_path_batches);
	std::for_each(path_batches.begin(), path_batches.end(), [&](std::vector<dcon::supply_route_path_id>& vec) { vec.clear(); });

	// Populate each path batch, and try to keep paths with the same origin in seperate batches, to lessen the impact on pathfinding quality
	uint32_t first_avail_batch = 0;
	state.world.for_each_market([&](dcon::market_id market) {
		auto market_paths = supply_paths_by_market_get(state, market);
		uint32_t current_batch = first_avail_batch;
		for(auto path : market_paths) {
			path_batches[current_batch].push_back(path);
			if(path_batches[current_batch].size() >= count_per_batch) {
				first_avail_batch++;
				if(first_avail_batch >= num_path_batches) {
					return; // We are now done
				}
			}
			current_batch = (current_batch >= num_path_batches - 1 ? first_avail_batch : current_batch + 1);
		}
	});
}



void update_supply_routes_daily(sys::state& state) {

	// STEP 1: initialize buffers in parallel
	auto begin = std::chrono::steady_clock::now();
	concurrency::parallel_for(0, 16, [&](uint32_t i) {
		switch(i) {
		case 0:
		{
			state.world.execute_serial_over_army_supply_route([&](auto route_ids) {
				state.world.army_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
					state.world.army_supply_route_set_buffered_supply_goods(route_ids, com_id, 0.0f);
				});
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
					state.world.army_supply_route_set_buffered_reinforcement_goods(route_ids, com_id, 0.0f);
				});
			});
			break;
		}
		case 1:
		{
			state.world.execute_serial_over_navy_supply_route([&](auto route_ids) {
				state.world.navy_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
					state.world.navy_supply_route_set_buffered_supply_goods(route_ids, com_id, 0.0f);
				});
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
					state.world.navy_supply_route_set_buffered_reinforcement_goods(route_ids, com_id, 0.0f);
				});
			});
			break;
		}
		case 2:
		{
			state.world.execute_serial_over_land_construction_supply_route([&](auto route_ids) {
				state.world.land_construction_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
				state.world.for_each_commodity([&](dcon::commodity_id com_id) {
					ve::apply([&](dcon::land_construction_supply_route_id route) {
						economy::commodity_amounts& goods = state.world.land_construction_supply_route_get_buffered_goods(route);
						std::fill(goods.begin(), goods.end(), 0.0f);
					}, route_ids);
				});
			});
			break;
		}
		case 3:
		{
			state.world.execute_serial_over_naval_construction_supply_route([&](auto route_ids) {
				state.world.naval_construction_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
				state.world.for_each_commodity([&](dcon::commodity_id com_id) {
					ve::apply([&](dcon::naval_construction_supply_route_id route) {
						economy::commodity_amounts& goods = state.world.naval_construction_supply_route_get_buffered_goods(route);
						std::fill(goods.begin(), goods.end(), 0.0f);
					}, route_ids);
				});
			});
			break;
		}
		case 4:
		{
			state.world.execute_serial_over_factory_construction_supply_route([&](auto route_ids) {
				state.world.factory_construction_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
				state.world.for_each_commodity([&](dcon::commodity_id com_id) {
					ve::apply([&](dcon::factory_construction_supply_route_id route) {
						economy::commodity_amounts& goods = state.world.factory_construction_supply_route_get_buffered_goods(route);
						std::fill(goods.begin(), goods.end(), 0.0f);
					}, route_ids);
				});
			});
			break;
		}
		case 5:
		{
			state.world.execute_serial_over_building_construction_supply_route([&](auto route_ids) {
				state.world.building_construction_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
				state.world.for_each_commodity([&](dcon::commodity_id com_id) {
					ve::apply([&](dcon::building_construction_supply_route_id route) {
						economy::commodity_amounts& goods = state.world.building_construction_supply_route_get_buffered_goods(route);
						std::fill(goods.begin(), goods.end(), 0.0f);
					}, route_ids);
				});
			});
			break;
		}
		case 6:
		{
			state.world.execute_serial_over_market([&](auto markets) {
				state.world.for_each_commodity([&](dcon::commodity_id com_id) {
					auto to_apply = state.world.market_get_government_stockpile(markets, com_id);
					local_stockpile_available_goods_set(state, markets, com_id, to_apply);
					ve::apply([&](dcon::market_id market) {
						if(state.world.market_is_valid(market)) {
							supply_paths_by_market_get(state, market).clear();
						}
					}, markets);
				});
			});
			break;
		}
		case 7:
		{
			state.world.execute_serial_over_army([&](auto armies) {
				state.world.army_set_needs_supply_goods(armies, ve::vbitfield_type{ 0 });
				state.world.army_set_needs_reinforcement_goods(armies, ve::vbitfield_type{ 0 });
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
					unit_supply_need_set(state, armies, com_id, ve::fp_vector{ 0.0f });
				});
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
					unit_reinforcement_need_set(state, armies, com_id, ve::fp_vector{ 0.0f });
				});
				ve::apply([&](dcon::army_id army) {
					if(state.world.army_is_valid(army)) {
						auto buffer = unit_best_stockpiles_get(state, army);
						buffer.clear();
					}
				}, armies);
			});
			break;
		}
		case 8:
		{
			state.world.execute_serial_over_navy([&](auto navies) {
				state.world.navy_set_needs_supply_goods(navies, ve::vbitfield_type{ 0 });
				state.world.navy_set_needs_reinforcement_goods(navies, ve::vbitfield_type{ 0 });
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
					unit_supply_need_set(state, navies, com_id, ve::fp_vector{ 0.0f });
				});
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
					unit_reinforcement_need_set(state, navies, com_id, ve::fp_vector{ 0.0f });
				});
				ve::apply([&](dcon::navy_id navy) {
					if(state.world.navy_is_valid(navy)) {
						auto buffer = unit_best_stockpiles_get(state, navy);
						buffer.clear();
					}
				}, navies);
			});
			break;
		}
		case 9:
		{
			state.world.execute_serial_over_province_land_construction([&](auto cons) {
				state.world.province_land_construction_set_needs_construction_goods(cons, ve::vbitfield_type{ 0 });
				ve::apply([&](dcon::province_land_construction_id con) {
					if(state.world.province_land_construction_is_valid(con)) {
						auto& constructions_need = constructions_need_get(state, con);
						std::fill(constructions_need.begin(), constructions_need.end(), 0.0f);
						auto stockpile_buffer = construction_best_stockpiles_get(state, con);
						stockpile_buffer.clear();
					}
				}, cons);
			});
			break;
		}
		case 10:
		{
			state.world.execute_serial_over_province_naval_construction([&](auto cons) {
				state.world.province_naval_construction_set_needs_construction_goods(cons, ve::vbitfield_type{ 0 });
				ve::apply([&](dcon::province_naval_construction_id con) {
					if(state.world.province_naval_construction_is_valid(con)) {
						auto& constructions_need = constructions_need_get(state, con);
						std::fill(constructions_need.begin(), constructions_need.end(), 0.0f);
						auto stockpile_buffer = construction_best_stockpiles_get(state, con);
						stockpile_buffer.clear();
					}
				}, cons);
			});
			break;
		}
		case 11:
		{
			state.world.execute_serial_over_factory_construction([&](auto cons) {
				state.world.factory_construction_set_needs_construction_goods(cons, ve::vbitfield_type{ 0 });
				ve::apply([&](dcon::factory_construction_id con) {
					if(state.world.factory_construction_is_valid(con)) {
						auto& constructions_need = constructions_need_get(state, con);
						std::fill(constructions_need.begin(), constructions_need.end(), 0.0f);
						auto stockpile_buffer = construction_best_stockpiles_get(state, con);
						stockpile_buffer.clear();
					}
				}, cons);
			});
			break;
		}
		case 12:
		{
			state.world.execute_serial_over_province_building_construction([&](auto cons) {
				state.world.province_building_construction_set_needs_construction_goods(cons, ve::vbitfield_type{ 0 });
				ve::apply([&](dcon::province_building_construction_id con) {
					if(state.world.province_building_construction_is_valid(con)) {
						auto& constructions_need = constructions_need_get(state, con);
						std::fill(constructions_need.begin(), constructions_need.end(), 0.0f);
						auto stockpile_buffer = construction_best_stockpiles_get(state, con);
						stockpile_buffer.clear();
					}
				}, cons);
			});
			break;
		}
		case 13:
		{
			state.world.execute_serial_over_nation([&](auto nations) {
				state.world.for_each_commodity([&](dcon::commodity_id com_id) {
					nation_construction_need_set(state, nations, com_id, ve::fp_vector{ 0.0f });
					nation_construction_expected_satisfaction_set(state, nations, com_id, ve::fp_vector{ 0.0f });
				});
				state.world.for_each_unit_supply_and_build_commodity([&](dcon::unit_supply_and_build_commodity_id com_id) {
					nation_unit_expected_satisfaction_set(state, nations, com_id, ve::fp_vector{ 0.0f });
					nation_unit_prio_need_set<military::unit_priority::low_priority>(state, nations, com_id, ve::fp_vector{ 0.0f });
					nation_unit_prio_need_set<military::unit_priority::normal_priority>(state, nations, com_id, ve::fp_vector{ 0.0f });
					nation_unit_prio_need_set<military::unit_priority::high_priority>(state, nations, com_id, ve::fp_vector{ 0.0f });
				});
				state.world.for_each_commodity([&](dcon::commodity_id com_id) {
					auto to_apply = state.world.nation_get_total_stockpiles(nations, com_id);
					nation_stockpile_available_goods_set(state, nations, com_id, to_apply);
				});
			});
			break;
		}
		case 14:
		{
			state.world.execute_serial_over_province_adjacency([&](auto adj) {
				state.world.province_adjacency_set_used_supply_throughput(adj, 0.0f);
			});
			break;
		}
		case 15:
		{
			state.world.execute_serial_over_supply_route_path([&](auto path) {
				state.world.supply_route_path_set_volume(path, ve::fp_vector{0.0f });
				state.world.supply_route_path_set_attempting_to_route(path, ve::vbitfield_type{ 0 });
				state.world.supply_route_path_set_is_active(path, ve::vbitfield_type{ 0 });
			});
			break;
		}
		}
	});

	auto end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 1 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();


	// STEP 2: delete unused routes and paths serially. Iterate from the end to compact as we go
	for(uint32_t i = state.world.army_supply_route_size(); i-- > 0;) {
		auto route = dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.navy_supply_route_size(); i-- > 0;) {
		auto route = dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.land_construction_supply_route_size(); i-- > 0;) {
		auto route = dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.naval_construction_supply_route_size(); i-- > 0;) {
		auto route = dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.factory_construction_supply_route_size(); i-- > 0;) {
		auto route = dcon::factory_construction_supply_route_id{ dcon::factory_construction_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.building_construction_supply_route_size(); i-- > 0;) {
		auto route = dcon::building_construction_supply_route_id{ dcon::building_construction_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.supply_route_path_size(); i-- > 0;) {
		auto path = dcon::supply_route_path_id{ dcon::supply_route_path_id::value_base_t(i) };
		if(should_delete_path(state, path)) {
			state.world.delete_supply_route_path(path);
		}
	}

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 2 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	// STEP 3: Compute the closest stockpile states to each military unit and construction, and accumulate all the goods required by each of them into buffers. Run them in parallel

	military::parallel_for_each_unit(state, [&](auto unit) {
		dcon::nation_id nation = military::unit_get_controller(state, unit);
		if constexpr(std::is_same_v<decltype(unit), dcon::army_id>) {
			// Only armies can have rebels. 
			if(!nation) {
				military::unit_for_each_subunit(state, unit, [&](auto subunit) {
					military::subunit_set_last_required_supply_percent_base_cost(state, subunit, 0.0f);
					military::subunit_set_last_required_reinforcement_percent_base_cost(state, subunit, 0.0f);
				});
				return;
			}
		}
		dcon::province_id location = military::unit_get_location(state, unit);

		auto stockpile_buffer = unit_best_stockpiles_get(state, unit);

		float supply_consumption_setting = static_cast<float>(nations::get_nation_military_consumption_setting_by_type<decltype(unit), military::unit_consumption_type::supply>(state, nation)) / 100.0f;
		float reinf_consumption_setting = static_cast<float>(nations::get_nation_military_consumption_setting_by_type<decltype(unit), military::unit_consumption_type::reinforcement>(state, nation)) / 100.0f;

		economy::get_closest_available_market_states(state, stockpile_buffer, nation, location);

		// Multiply the supply we want to dispatch to each unit by the consumption setting. We only want to fufill that percentage
		auto accumulate_supply = [&](dcon::commodity_id com_id, float amount) {
			dcon::unit_supply_commodity_id supply_id = state.world.commodity_get_unit_supply_commodity(com_id);
			unit_supply_need_set(state, unit, supply_id, unit_supply_need_get(state, unit, supply_id) + (amount * supply_consumption_setting));
			assert(supply_id);
			unit_set_needs_supply_goods(state, unit, unit_needs_supply_goods(state, unit) || amount > 0.0f); // set bool flag if this unit now needs more than 0 supply goods
		};
		auto accumulate_reinf = [&](dcon::commodity_id com_id, float amount) {
			dcon::unit_build_commodity_id build_id = state.world.commodity_get_unit_build_commodity(com_id);
			assert(build_id);
			unit_reinforcement_need_set(state, unit, build_id, unit_reinforcement_need_get(state, unit, build_id) + (amount * reinf_consumption_setting));
			unit_set_needs_reinforcement_goods(state, unit, unit_needs_reinforcement_goods(state, unit) || amount > 0.0f); // set bool flag if this unit now needs more than 0 reinforcement goods
		};


		military::unit_for_each_subunit(state, unit, [&](auto subunit) {
			// Compute the "required_x_of_base_cost" meaning how much of the base cost is required to reach 100% satisfaction for supply and reinforcement need respectively. These are used later
			float required_supply_goods_of_base_cost = military::get_supply_cost_modifiers(state, subunit);
			military::subunit_set_last_required_supply_percent_base_cost(state, subunit, required_supply_goods_of_base_cost);
			float required_reinf_goods_of_base_cost = military::estimate_reinforcement<military::interval_estimation::daily, military::supply_estimation::full_supply_always, military::reinforcement_cap::capped_at_max_strength>(state, subunit); // The reinforcement available (from 0.0-1.0f) requires that % of the base build cost to fufill
			military::subunit_set_last_required_reinforcement_percent_base_cost(state, subunit, required_reinf_goods_of_base_cost);
			// Then accumulate each commodity required, which will call the prev. lambdas with the commodity and amount required
			military::accumulate_subunit_daily_goods_requirements<military::unit_consumption_type::supply>(state, nation, subunit, accumulate_supply);
			military::accumulate_subunit_daily_goods_requirements<military::unit_consumption_type::reinforcement>(state, nation, subunit, accumulate_reinf);
		});
	});
	economy::parallel_for_each_construction(state, [&](auto construction) {
		if(economy::construction_is_privately_owned(state, construction)) {
			return;
		}
		if(!economy::can_advance_construction(state, construction)) {
			return;
		}

		dcon::nation_id nation = economy::construction_get_controller(state, construction);
		dcon::province_id location = economy::construction_get_location(state, construction);
		auto stockpile_buffer = construction_best_stockpiles_get(state, construction);

		economy::get_closest_available_market_states(state, stockpile_buffer, nation, location);

		economy::commodity_amounts& required_buffer = constructions_need_get(state, construction);
		float consumption_rate = static_cast<float>(nations::get_nation_construction_consumption_setting_by_type<decltype(construction)>(state, nation)) / 100.0f;

		float construction_days = static_cast<float>(economy::construction_get_actual_construction_time(state, construction));

		auto accumulate_func = [&](uint32_t set_indx, float required, float total_cost) {
			// Tie the demand amount to add to the construction consumption setting of the nation. Low consumption setting -> we won't try to dispatch as much goods
			float actual_demanded = std::min(required, total_cost / construction_days * consumption_rate);
			float minimum_demand = std::max(actual_demanded, total_cost * construction_route_transport_leeway);
			actual_demanded = (actual_demanded == 0.0f ? actual_demanded : minimum_demand); // If some amount of goods are still required, then at minimum attempt to consume some percent of the total cost (default: 0.1%) to avoid slowdowns due to potential loss
			required_buffer[set_indx] += actual_demanded;
			construction_set_needs_construction_goods(state, construction, construction_needs_construction_goods(state, construction) || actual_demanded > 0.0f ); // set bool flag if this construction now needs more than 0 goods
		};
		// Compute the "required_x_of_base_cost" meaning how much of the base cost is required to reach 100% satisfaction for construction requirements. These are used later
		float required_construction_goods_of_base_cost = economy::construction_build_cost_multiplier(state, construction) / construction_days;
		economy::construction_set_last_required_percent_base_cost(state, construction, required_construction_goods_of_base_cost);
		economy::accumulate_construction_daily_goods_requirements(state, construction, accumulate_func);
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 3 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	begin = std::chrono::steady_clock::now();

	// STEP 4: Split the total goods required by units in each of the priority brackets (high, medium, low) per-nation, and accumulate total goods requried for constructions per-nation by using the previously buffered data

	static std::vector<unit> low_prio_units;
	static std::vector<unit> normal_prio_units;
	static std::vector<unit> high_prio_units;
	low_prio_units.clear();
	normal_prio_units.clear();
	high_prio_units.clear();

	// Lambda for accumulating each military unit
	auto accumulate_prioritized_unit_supply = [&]<military::unit_priority priority, concepts::military_unit unit_type>(unit_type u, dcon::nation_id controller) {
		auto unit = fatten(state.world, u);
		state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
			auto sup_and_build_id = state.world.unit_supply_commodity_get_supply_and_build_commodity(com_id);
			nation_unit_prio_need_set<priority>(state, controller, sup_and_build_id, nation_unit_prio_need_get<priority>(state, controller, sup_and_build_id) + unit_supply_need_get(state, u, com_id));
			
		});
		state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
			auto sup_and_build_id = state.world.unit_build_commodity_get_supply_and_build_commodity(com_id);
			nation_unit_prio_need_set<priority>(state, controller, sup_and_build_id, nation_unit_prio_need_get<priority>(state, controller, sup_and_build_id) + unit_reinforcement_need_get(state, u, com_id));
		});
	};

	// Lambda for accumulating each construction
	auto accumulate_nation_construction_supply = [&]<concepts::construction_type construction_type>(construction_type construction) {
		dcon::nation_id nation = economy::construction_get_controller(state, construction);
		assert(nation);
		const economy::commodity_set& build_costs = economy::construction_get_base_build_cost(state, construction);
		for(uint32_t j = 0; j < build_costs.set_size; j++) {
			auto com_id = build_costs.commodity_type[j];
			if(com_id) {
				float construction_need = constructions_need_get(state, construction)[j];
				nation_construction_need_set(state, nation, com_id, nation_construction_need_get(state, nation, com_id) + construction_need);
			} else {
				break;
			}
		}
	};

	// Accumulating military units and military constructions can be done in two seperate threads as they don't overlap
	concurrency::parallel_invoke(
		[&]() {
			military::for_each_unit(state, [&](auto mil_unit) {
				auto fat_unit = fatten(state.world, mil_unit);
				dcon::nation_id controller = military::unit_get_controller(state, mil_unit);
				if constexpr(std::is_same_v<decltype(mil_unit), dcon::army_id>) {
					if(!controller) {
						return;
					}
				}
				military::unit_priority supply_prio = military::get_effective_unit_supply_priority(state, fat_unit.id, controller);
				switch(supply_prio) {
				case military::unit_priority::low_priority:
					accumulate_prioritized_unit_supply.template operator() < military::unit_priority::low_priority > (mil_unit, controller);
					low_prio_units.emplace_back(unit{ mil_unit });
					break;
				case military::unit_priority::normal_priority:
					accumulate_prioritized_unit_supply.template operator() < military::unit_priority::normal_priority > (mil_unit, controller);
					normal_prio_units.emplace_back(unit{ mil_unit });
					break;
				case military::unit_priority::high_priority:
					accumulate_prioritized_unit_supply.template operator() < military::unit_priority::high_priority > (mil_unit, controller);
					high_prio_units.emplace_back(unit{ mil_unit });
					break;
				}
			});
		},
		[&]() {
			economy::for_each_construction(state, [&](auto con) {
				accumulate_nation_construction_supply(con);
			});
		}
	);

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 4 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	


	// Lambda to compute the expected satisfaction of military units of a priority. Will be used to spread out the commodities abit more
	auto compute_nations_unit_expected_satisfaction = [&]<military::unit_priority priority>() {
		state.world.execute_parallel_over_nation([&](auto nations) {
			auto nation_valid = nations::exists(state, nations);
			// compute military units' expected satisfaction for a priority category for all nations
			state.world.for_each_unit_supply_and_build_commodity([&](dcon::unit_supply_and_build_commodity_id com_id) {
				dcon::commodity_id base_com_id = state.world.unit_supply_and_build_commodity_get_base_commodity(com_id);
				ve::fp_vector supply_required = nation_unit_prio_need_get<priority>(state, nations, com_id);
				auto expected_sat = ve::select(supply_required == 0.0f, 1.0f, ve::min(nation_stockpile_available_goods_get(state, nations, base_com_id) / supply_required, 1.0f));
				expected_sat = ve::select(nation_valid, expected_sat, 0.0f);
				nation_unit_expected_satisfaction_set(state, nations, com_id, expected_sat);
			});
		});
		const std::vector<unit>& unit_vec = [&]() -> const std::vector<unit>& {
			if constexpr(priority == military::unit_priority::low_priority) {
				return low_prio_units;
			} else if constexpr(priority == military::unit_priority::normal_priority) {
				return normal_prio_units;
			} else if constexpr(priority == military::unit_priority::high_priority) {
				return high_prio_units;
			}
		}();
		// update the supply&reinf needs for each military unit, using the computed buffered values in parallel
		concurrency::parallel_for_each(unit_vec.begin(), unit_vec.end(), [&](unit unit) {
			if(unit.is_army) {
				dcon::army_id army = unit.content.army;
				dcon::nation_id controller = state.world.army_get_controller_from_army_control(army);
				assert(controller);
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
					auto supply_build_id = state.world.unit_supply_commodity_get_supply_and_build_commodity(com_id);
					float expected_sat = nation_unit_expected_satisfaction_get(state, controller, supply_build_id);
					float current_req_supply = unit_supply_need_get(state, army, com_id);
					unit_supply_need_set(state, army, com_id, current_req_supply * expected_sat);
				});
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
					auto supply_build_id = state.world.unit_build_commodity_get_supply_and_build_commodity(com_id);
					float expected_sat = nation_unit_expected_satisfaction_get(state, controller, supply_build_id);
					float current_req_supply = unit_reinforcement_need_get(state, army, com_id);
					unit_reinforcement_need_set(state, army, com_id, current_req_supply * expected_sat);
				});
			}
			else {
				dcon::navy_id navy = unit.content.navy;
				dcon::nation_id controller = state.world.navy_get_controller_from_navy_control(navy);
				assert(controller);
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
					auto supply_build_id = state.world.unit_supply_commodity_get_supply_and_build_commodity(com_id);
					float expected_sat = nation_unit_expected_satisfaction_get(state, controller, supply_build_id);
					float current_req_supply = unit_supply_need_get(state, navy, com_id);
					unit_supply_need_set(state, navy, com_id, current_req_supply * expected_sat);
				});
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
					auto supply_build_id = state.world.unit_build_commodity_get_supply_and_build_commodity(com_id);
					float expected_sat = nation_unit_expected_satisfaction_get(state, controller, supply_build_id);
					float current_req_supply = unit_reinforcement_need_get(state, navy, com_id);
					unit_reinforcement_need_set(state, navy, com_id, current_req_supply * expected_sat);
				});
			}
		});

	};
	// Lambda to compute the expected satisfaction of constructions. Will be used to spread out the commodities abit more evenly
	auto compute_nations_construction_expected_satisfaction = [&]() {
		state.world.execute_parallel_over_nation([&](auto nations) {
			auto nation_valid = nations::exists(state, nations);
			// compute military constructions expected satisfaction for all nations
			economy::for_each_commodity_no_money(state, [&](dcon::commodity_id base_com_id) {
				auto supply_required = nation_construction_need_get(state, nations, base_com_id);
				auto expected_sat = ve::select(supply_required == 0.0f, 1.0f, ve::min(nation_stockpile_available_goods_get(state, nations, base_com_id) / supply_required, 1.0f));
				expected_sat = ve::select(nation_valid, expected_sat, 0.0f);
				nation_construction_expected_satisfaction_set(state, nations, base_com_id, expected_sat);
			});
		});
		// Clamp down the constructions need depending on the expected satisfaction, so that the first few constructions won't hoard the goods if there arent many available
		economy::parallel_for_each_construction(state, [&](auto con) {
			if(!economy::construction_is_privately_owned(state, con) && economy::can_advance_construction(state, con)) {
				economy::commodity_amounts& current_req_goods = constructions_need_get(state, con);
				auto controller = economy::construction_get_controller(state, con);
				const economy::commodity_set& build_cost = economy::construction_get_base_build_cost(state, con);
				for(uint32_t i = 0; i < build_cost.set_size; i++) {
					auto com_id = build_cost.commodity_type[i];
					if(com_id) {
						float expected_sat = nation_construction_expected_satisfaction_get(state, controller, com_id);
						current_req_goods[i] *= expected_sat;
					} else {
						break;
					}
				}
			}
		});

	};



	// STEP 5: Update the actual goods satisfaction for all units and constructions. It will remove goods presumed to be consumed from the buffer if a valid route and path exists.
	// If a route or path does not exist already between the stockpile and the unit/construction, it will add to to the pending queue of routes/paths to be created and skip.
	// Units must be done in order of reinforcement priority, via 3 vectors of differing priorities
	// Each unit vector and constructions are done in parallel over the commodities, as that allows it to be deterministic

	begin = std::chrono::steady_clock::now();


	// Lambda to process each prioitized unit vector seperately
	auto process_prioritized_unit_vector = [&](std::span<const unit> units_to_process) {
		for(auto unit : units_to_process) {
			if(unit.is_army) {
				auto army = unit.content.army;
				if(unit_needs_supply_goods(state, army)) {
					state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id sup_com) {
						float amount_required = unit_supply_need_get(state, army, sup_com);
						if(amount_required > 0.0f) {
							auto base_commodity = state.world.unit_supply_commodity_get_base_commodity(sup_com);
							update_military_unit_routes_satisfaction(state, army, state.world.army_get_controller_from_army_control(army), sup_com, base_commodity);
						}
					});
				}
				if(unit_needs_reinforcement_goods(state, army)) {
					state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id reinf_com) {
						float amount_required = unit_reinforcement_need_get(state, army, reinf_com);
						if(amount_required > 0.0f) {
							auto base_commodity = state.world.unit_build_commodity_get_base_commodity(reinf_com);
							update_military_unit_routes_satisfaction(state, army, state.world.army_get_controller_from_army_control(army), reinf_com, base_commodity);
						}
					});
				}
			} else {
				auto navy = unit.content.navy;
				if(unit_needs_supply_goods(state, navy)) {
					state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id sup_com) {
						auto base_commodity = state.world.unit_supply_commodity_get_base_commodity(sup_com);
						update_military_unit_routes_satisfaction(state, navy, state.world.navy_get_controller_from_navy_control(navy), sup_com, base_commodity);
					});
				}
				if(unit_needs_reinforcement_goods(state, navy)) {
					state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id reinf_com) {
						auto base_commodity = state.world.unit_build_commodity_get_base_commodity(reinf_com);
						update_military_unit_routes_satisfaction(state, navy, state.world.navy_get_controller_from_navy_control(navy), reinf_com, base_commodity);
					});
				}
			}
		}
	};
	// Lambda to process all constructions
	auto process_constructions = [&]() {
		economy::for_each_construction(state, [&](auto construction) {

			if(!construction_needs_construction_goods(state, construction)) {
				return;
			}

			// Skip constructions which are not govt owned
			if(economy::construction_is_privately_owned(state, construction)) {
				return;
			}
			if(!economy::can_advance_construction(state, construction)) {
				return;
			}
			dcon::nation_id controller = economy::construction_get_controller(state, construction);
			const economy::commodity_set& build_cost = economy::construction_get_base_build_cost(state, construction);
			const economy::commodity_amounts& construction_need = constructions_need_get(state, construction);
			for(uint32_t idx = 0; idx < build_cost.set_size; idx++) {
				dcon::commodity_id commodity = build_cost.commodity_type[idx];
				if(commodity) {
					if(construction_need[idx] > 0.0f) {
						update_construction_routes_satisfaction(state, construction, controller, idx, commodity);
					}
				}
				else {
					break;
				}
			}
		});
	};


	// Pricess units first, in order of priority
	if(!high_prio_units.empty()) {
		compute_nations_unit_expected_satisfaction.template operator() < military::unit_priority::high_priority > ();
		process_prioritized_unit_vector(high_prio_units);
	}
	if(!normal_prio_units.empty()) {
		compute_nations_unit_expected_satisfaction.template operator() < military::unit_priority::normal_priority > ();
		process_prioritized_unit_vector(normal_prio_units);
	}
	if(!low_prio_units.empty()) {
		compute_nations_unit_expected_satisfaction.template operator() < military::unit_priority::low_priority > ();
		process_prioritized_unit_vector(low_prio_units);
	}
	// Afterwards, process constructions. Constructions are always prioitized lower than unit consumption to prevent accidential kneecapping of the military
	compute_nations_construction_expected_satisfaction();
	process_constructions();

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 5 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();




	// STEP 6: update the paths of any supply routes which are deemed out-of-date or has no path. They are deemed out of date if anything significant happens to disrupt it and an update is scheduled. 
	// Weekly province updates are issued once per week at diffrent intervals, ie army supply routes are done at the 1st day of the week, navy supply routes on the 2nd day etc
	// Updates triggered by unit movement is done daily 

	auto day_of_week = state.current_date.value % 7;
	if(day_of_week == 0) {

		// Update paths which are flagged to require an update once per week
		parallel_for_each_supply_route_path(state, [&](dcon::supply_route_path_id path_handle) {
			bool path_is_valid = state.world.supply_route_path_get_valid_path(path_handle);
			bool attempting_to_route = state.world.supply_route_path_get_attempting_to_route(path_handle);
			// If the path is not valid, and the route is attempted to be used then update it so that it can check again if pathing is possible. We only do this weekly to prevent lots of daily pathing attempts
			if(!path_is_valid && attempting_to_route) {
				schedule_immediate_supply_path_update(state, path_handle);
				return; // Leave loop iteration 
			}
			auto path = state.world.supply_route_path_get_path(path_handle);
			dcon::nation_id owner = supply_route_path_get_owner(state, path_handle);
			for(dcon::province_id prov : path) {
				// Check if the province is flagged to update ALL routes which pass through it
				if(state.world.province_get_supply_routes_requires_update(prov)) {
					schedule_immediate_supply_path_update(state, path_handle);
					return; // Leave loop iteration 
				}
				// Check if the nation is flagged to have all of its' supply routes updated
				if(state.world.nation_get_supply_routes_requires_path_update(owner)) {
					schedule_immediate_supply_path_update(state, path_handle);
					return;
				}
				// Check which nations' routes should be updated. Only routes which belong to one of those nations will be updated
				auto nations_to_update = state.world.province_get_nation_routes_to_be_updated(prov);
				if(auto found = std::find(nations_to_update.begin(), nations_to_update.end(), owner); found != nations_to_update.end()) {
					schedule_immediate_supply_path_update(state, path_handle);
					return;
				}
			}

		});
		// Reset all of of the "to-update" values. Run concurrently
		concurrency::parallel_invoke(
		[&]() {
			state.world.execute_serial_over_nation([&](auto nations) {
				state.world.nation_set_supply_routes_requires_path_update(nations, ve::vbitfield_type{ 0 });
			});
		},
		[&]() {
			state.world.execute_serial_over_province([&](auto prov_ids) {
				state.world.province_set_supply_routes_requires_update(prov_ids, ve::vbitfield_type{ 0 });
				ve::apply([&](dcon::province_id prov) {
					state.world.province_get_nation_routes_to_be_updated(prov).clear();
				}, prov_ids);
			});
		});
	}

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 6 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();


	// step 7: Increment the amount of days a route or path has been inactive, add the volume used by some supply routes unto each adjacency, and finally set the recently_created flag to false.
	// Also setup the path batch container in the meantime as it can also run in parallel with these things

	static std::vector<std::vector<dcon::supply_route_path_id>> path_batches;

	concurrency::parallel_invoke(
		[&]() {
			ve_for_each_supply_route(state, [&](auto routes) {
				auto cur_inactive_days = supply_route_get_inactive_days(state, routes);
				auto new_inactive_days = ve::select(supply_route_is_active(state, routes), cur_inactive_days, cur_inactive_days + uint8_t(1));
				supply_route_set_inactive_days(state, routes, new_inactive_days);
			});
		},
		[&]() {
			state.world.execute_serial_over_supply_route_path([&](auto paths) {
				auto cur_inactive_days = state.world.supply_route_path_get_inactive_days(paths);
				auto new_inactive_days = ve::select(state.world.supply_route_path_get_is_active(paths), cur_inactive_days, cur_inactive_days + uint8_t(1));
				state.world.supply_route_path_set_inactive_days(paths, new_inactive_days);
				state.world.supply_route_path_set_recently_created(paths, ve::vbitfield_type{ 0 });
			});
		},
		[&]() {
			// Add the used supply throughput from paths unto each province adjacency. Might be able to parallelize this
			state.world.for_each_supply_route_path([&](dcon::supply_route_path_id path) {
				bool path_valid = state.world.supply_route_path_get_valid_path(path);
				bool path_out_of_date = state.world.supply_route_path_get_path_out_of_date(path);
				bool is_active = supply_route_path_is_active(state, path);
				// Don't apply volume for paths which are out of date. The volume for those will be applied as the paths are updated later
				if(is_active && !path_out_of_date && path_valid) {
					auto adj_path = state.world.supply_route_path_get_adjacency_path(path);
					float volume = state.world.supply_route_path_get_volume(path);
					add_used_supply_throughput(state, adj_path, volume);
				}
			});
		},
		[&]() {
			// Setup the batch container
			setup_spread_supply_path_batches(state, supply_route_pathfinding_batches, path_batches);
		}
	);

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 7 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	// STEP 8: Update paths requiring an update by processing it in batches. The total amount of paths requiring an update will be divided into x batches, done by the previous loop. The logic will attempt to keep paths with the same origin point in diffrent batches to lessen the impact on patching quality


	// Process the batches. The application of used supply throughput is done once per batch, and has to be done serially as it modifies arbitrary province adjacency data
	for(const auto& paths_batch : path_batches) {
		concurrency::parallel_for_each(paths_batch.begin(), paths_batch.end(), [&](dcon::supply_route_path_id path_handle) {
			update_supply_route_path(state, path_handle, state.world.supply_route_path_get_volume(path_handle));
		});
		std::for_each(paths_batch.begin(), paths_batch.end(), [&](dcon::supply_route_path_id path_handle) {
			bool valid_path = state.world.supply_route_path_get_valid_path(path_handle);
			// If the path is invalid, then set all connected routes to be inactive (the routes might have assumed that the new path handle would get a valid path and reserved goods and become active. If the pathing failed,it will get fixed here)
			if(!valid_path) {
				state.world.supply_route_path_set_is_active(path_handle, false);
				supply_path_for_each_connected_route(state, path_handle, [&](auto route) {
					supply_route_set_is_active(state, route, false);
				});
			}
			bool is_active = supply_route_path_is_active(state, path_handle);
			if(is_active) {
				auto adj_path = state.world.supply_route_path_get_adjacency_path(path_handle);
				float volume = state.world.supply_route_path_get_volume(path_handle);
				add_used_supply_throughput(state, adj_path, volume);
			}
		});
	}

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 8 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	// STEP 9: update throughput rate and supply loss on paths which are active and valid in parallel 

	parallel_for_each_supply_route_path_predicate(state, [&](dcon::supply_route_path_id p) { return supply_route_path_is_active(state, p) && state.world.supply_route_path_get_valid_path(p);  }, [&](dcon::supply_route_path_id path_handle) {
		update_supply_path_throughput_attrition(state, path_handle, supply_route_path_get_owner(state, path_handle));
	});


	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 9 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();


	// STEP 10: subtract from stockpiles the actual buffered amount which each route has taken
	// Must take into account the actual throughput, and only subtract the percentage of goods which were possible to move

	for_each_unit_supply_route(state, [&](auto r) {
		auto route = fatten(state.world, r);
		if(supply_route_is_active(state, route.id)) {
			dcon::nation_id route_owner = supply_route_get_owner(state, route);
			dcon::market_id origin_market = supply_route_get_origin_market(state, route.id);
			float throughput = supply_route_get_throughput(state, route.id);
			state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
				auto base_com_id = economy::unit_commodity_get_base_commodity(state, com_id);
				float amount_to_sub = route.get_buffered_supply_goods(com_id) * throughput;
				economy::subtract_government_stockpile(state, route_owner, origin_market, base_com_id, amount_to_sub);
			});
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				auto base_com_id = economy::unit_commodity_get_base_commodity(state, com_id);
				float amount_to_sub = route.get_buffered_reinforcement_goods(com_id) * throughput;
				economy::subtract_government_stockpile(state, route_owner, origin_market, base_com_id, amount_to_sub);
			});
		}
	});

	for_each_construction_supply_route(state, [&](auto r) {
		auto route = fatten(state.world, r);
		if(supply_route_is_active(state, route.id)) {
			dcon::nation_id route_owner = supply_route_get_owner(state, route);
			dcon::market_id origin_market = supply_route_get_origin_market(state, route.id);
			auto construction = route.get_construction();
			float throughput = supply_route_get_throughput(state, route.id);
			const economy::commodity_set& unit_build_goods = economy::construction_get_base_build_cost(state, construction.id);
			const economy::commodity_amounts& route_goods_amounts = route.get_buffered_goods();
			for(uint32_t i = 0; i < unit_build_goods.set_size; i++) {
				auto com_id = unit_build_goods.commodity_type[i];
				if(com_id) {
					economy::subtract_government_stockpile(state, route_owner, origin_market, com_id, route_goods_amounts[i] * throughput);
				}
				else {
					break;
				}
			}
		}
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 10 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	// STEP 11: Update each army/navy supply & reinforcement satisfaction and advance constructions, by computing how much of their required commodities they were able to receive from all supply routes

	// Start processing each army/navy and applying reinforcement/supply satisfaction
	// Do military units
	military::parallel_for_each_unit(state, [&](auto unit) {
		update_unit_commodity_satisfaction(state, unit);
	});
	// Then constructions
	economy::parallel_for_each_construction(state, [&](auto construction) {
		update_construction_commodity_satisfaction(state, construction);
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 11 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

}

template<command::actor actor>
bool can_change_army_supply_consumption_setting(const sys::state & state, dcon::nation_id source, int8_t new_setting) {
	if constexpr(actor == command::actor::player) {
		if(!state.current_scene.game_in_progress) {
			return false;
		}
		if(new_setting > army_supply_setting_max(state, source) || army_supply_setting_min(state, source) > new_setting) {
			return false;
		}
	}
	return true;
}
template bool can_change_army_supply_consumption_setting<command::actor::player>(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template bool can_change_army_supply_consumption_setting<command::actor::ai>(const sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
void change_army_supply_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting) {
	state.world.nation_set_land_supply_consumption(source, new_setting);
}
template void change_army_supply_consumption_setting<command::actor::player>(sys::state& state, dcon::nation_id source, int8_t new_setting);
template void change_army_supply_consumption_setting<command::actor::ai>(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_army_reinforcement_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting) {
	if constexpr(actor == command::actor::player) {
		if(!state.current_scene.game_in_progress) {
			return false;
		}
		if(new_setting > army_reinforcement_setting_max(state, source) || army_reinforcement_setting_min(state, source) > new_setting) {
			return false;
		}
	}
	return true;
}
template bool can_change_army_reinforcement_consumption_setting<command::actor::player>(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template bool can_change_army_reinforcement_consumption_setting<command::actor::ai>(const sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
void change_army_reinforcement_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting) {
	state.world.nation_set_land_reinforcement_consumption(source, new_setting);
}
template void change_army_reinforcement_consumption_setting<command::actor::player>(sys::state& state, dcon::nation_id source, int8_t new_setting);
template void change_army_reinforcement_consumption_setting<command::actor::ai>(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_navy_supply_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting) {
	if constexpr(actor == command::actor::player) {
		if(!state.current_scene.game_in_progress) {
			return false;
		}
		if(new_setting > navy_supply_setting_max(state, source) || navy_supply_setting_min(state, source) > new_setting) {
			return false;
		}
	}
	return true;
}
template bool can_change_navy_supply_consumption_setting<command::actor::player>(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template bool can_change_navy_supply_consumption_setting<command::actor::ai>(const sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
void change_navy_supply_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting) {
	state.world.nation_set_naval_supply_consumption(source, new_setting);
}
template void change_navy_supply_consumption_setting<command::actor::player>(sys::state& state, dcon::nation_id source, int8_t new_setting);
template void change_navy_supply_consumption_setting<command::actor::ai>(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_navy_reinforcement_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting) {
	if constexpr(actor == command::actor::player) {
		if(!state.current_scene.game_in_progress) {
			return false;
		}
		if(new_setting > navy_reinforcement_setting_max(state, source) || navy_reinforcement_setting_min(state, source) > new_setting) {
			return false;
		}
	}
	return true;
}
template bool can_change_navy_reinforcement_consumption_setting<command::actor::player>(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template bool can_change_navy_reinforcement_consumption_setting<command::actor::ai>(const sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
void change_navy_reinforcement_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting) {
	state.world.nation_set_naval_reinforcement_consumption(source, new_setting);
}
template void change_navy_reinforcement_consumption_setting<command::actor::player>(sys::state& state, dcon::nation_id source, int8_t new_setting);
template void change_navy_reinforcement_consumption_setting<command::actor::ai>(sys::state& state, dcon::nation_id source, int8_t new_setting);


template<command::actor actor>
bool can_change_army_construction_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting) {
	if constexpr(actor == command::actor::player) {
		if(!state.current_scene.game_in_progress) {
			return false;
		}
		if(new_setting > army_construction_setting_max(state, source) || army_construction_setting_min(state, source) > new_setting) {
			return false;
		}
	}
	return true;
}
template bool can_change_army_construction_consumption_setting<command::actor::player>(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template bool can_change_army_construction_consumption_setting<command::actor::ai>(const sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
void change_army_construction_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting) {
	state.world.nation_set_army_construction_consumption(source, new_setting);
}
template void change_army_construction_consumption_setting<command::actor::player>(sys::state& state, dcon::nation_id source, int8_t new_setting);
template void change_army_construction_consumption_setting<command::actor::ai>(sys::state& state, dcon::nation_id source, int8_t new_setting);


template<command::actor actor>
bool can_change_navy_construction_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting) {
	if constexpr(actor == command::actor::player) {
		if(!state.current_scene.game_in_progress) {
			return false;
		}
		if(new_setting > navy_construction_setting_max(state, source) || navy_construction_setting_min(state, source) > new_setting) {
			return false;
		}
	}
	return true;
}
template bool can_change_navy_construction_consumption_setting<command::actor::player>(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template bool can_change_navy_construction_consumption_setting<command::actor::ai>(const sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
void change_navy_construction_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting) {
	state.world.nation_set_navy_construction_consumption(source, new_setting);
}
template void change_navy_construction_consumption_setting<command::actor::player>(sys::state& state, dcon::nation_id source, int8_t new_setting);
template void change_navy_construction_consumption_setting<command::actor::ai>(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_factory_construction_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting) {
	if constexpr(actor == command::actor::player) {
		if(!state.current_scene.game_in_progress) {
			return false;
		}
		if(new_setting > factory_construction_setting_max(state, source) || factory_construction_setting_min(state, source) > new_setting) {
			return false;
		}
	}
	return true;
}
template bool can_change_factory_construction_consumption_setting<command::actor::player>(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template bool can_change_factory_construction_consumption_setting<command::actor::ai>(const sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
void change_factory_construction_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting) {
	state.world.nation_set_factory_construction_consumption(source, new_setting);
}
template void change_factory_construction_consumption_setting<command::actor::player>(sys::state& state, dcon::nation_id source, int8_t new_setting);
template void change_factory_construction_consumption_setting<command::actor::ai>(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_building_construction_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting) {
	if constexpr(actor == command::actor::player) {
		if(!state.current_scene.game_in_progress) {
			return false;
		}
		if(new_setting > building_construction_setting_max(state, source) || building_construction_setting_min(state, source) > new_setting) {
			return false;
		}
	}
	return true;
}
template bool can_change_building_construction_consumption_setting<command::actor::player>(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template bool can_change_building_construction_consumption_setting<command::actor::ai>(const sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
void change_building_construction_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting) {
	state.world.nation_set_building_construction_consumption(source, new_setting);
}
template void change_building_construction_consumption_setting<command::actor::player>(sys::state& state, dcon::nation_id source, int8_t new_setting);
template void change_building_construction_consumption_setting<command::actor::ai>(sys::state& state, dcon::nation_id source, int8_t new_setting);


}
