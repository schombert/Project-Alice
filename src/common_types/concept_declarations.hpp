#pragma once
#include <type_traits>
#include "dcon_generated_ids.hpp"
namespace concepts {


template<typename T>
concept military_unit = std::is_same_v<T, dcon::army_id> || std::is_same_v<T, dcon::navy_id>;

template<typename T>
concept military_subunit = std::is_same_v<T, dcon::regiment_id> || std::is_same_v<T, dcon::ship_id>;


template<typename T>
concept military_supply_route_type = std::is_same_v<T, dcon::army_supply_route_id> ||
									std::is_same_v<T, dcon::navy_supply_route_id>;

template<typename T>
concept construction_supply_route_type = std::is_same_v<T, dcon::land_construction_supply_route_id> ||
										 std::is_same_v<T, dcon::naval_construction_supply_route_id> ||
										 std::is_same_v<T, dcon::factory_construction_supply_route_id> ||
										 std::is_same_v<T, dcon::building_construction_supply_route_id>;

template<typename T>
concept supply_route_type = military_supply_route_type<T> || construction_supply_route_type<T>;


template<typename T>
concept military_construction_type = std::is_same_v<T, dcon::province_land_construction_id> ||
									 std::is_same_v<T, dcon::province_naval_construction_id>;

template<typename T>
concept economy_construction_type = std::is_same_v<T, dcon::factory_construction_id> ||
									std::is_same_v<T, dcon::province_building_construction_id>;

template<typename T>
concept construction_type = economy_construction_type<T> || military_construction_type<T>;


template<typename T>
concept unit_supply_or_build_commodity_type = std::is_same_v<T, dcon::unit_supply_commodity_id> ||
											  std::is_same_v<T, dcon::unit_build_commodity_id>;

template<typename T>
concept unit_commodity_type = std::is_same_v<T, dcon::unit_supply_commodity_id> || std::is_same_v<T, dcon::unit_build_commodity_id> || std::is_same_v<T, dcon::unit_supply_and_build_commodity_id>;

template<typename T>
concept any_commodity_type = unit_commodity_type<T> || std::is_same_v<T, dcon::commodity_id>;

template<typename T>
concept cvref_integral = std::integral<std::remove_cvref_t<T>>;

// Is the type a regular (non-ve) dcon id type?
template <typename T>
concept dcon_id_type =
	requires (T t) { { t.index() }-> cvref_integral; }
&&
	requires (T t) { { t.value }-> cvref_integral; };

// Is the type a ve-dcon-id type?
template<typename T, typename ID>
concept dcon_id_ve_type = dcon_id_type<ID> &&
						  (std::is_same_v<T, ve::contiguous_tags<ID>> ||
							  std::is_same_v<T, ve::tagged_vector<ID>>||
						  std::is_same_v <T, ve::partial_contiguous_tags<ID>> ||
						  std::is_same_v <T, ve::unaligned_contiguous_tags<ID>>);




template<typename T>
concept ve_military_supply_route_type = dcon_id_ve_type<T, dcon::army_supply_route_id> ||
										dcon_id_ve_type<T, dcon::navy_supply_route_id>;

template<typename T>
concept ve_military_subunit_type =  dcon_id_ve_type<T, dcon::regiment_id> ||
									dcon_id_ve_type<T, dcon::ship_id>;

template<typename T>
concept ve_or_regular_military_subunit_type = ve_military_subunit_type<T> || military_subunit<T>;

template<typename T>
concept ve_construction_supply_route_type = dcon_id_ve_type<T, dcon::land_construction_supply_route_id> ||
											dcon_id_ve_type<T, dcon::naval_construction_supply_route_id> ||
									        dcon_id_ve_type<T, dcon::factory_construction_supply_route_id> ||
											dcon_id_ve_type<T, dcon::building_construction_supply_route_id>;

template<typename T>
concept ve_supply_route_type = ve_military_supply_route_type<T> || ve_construction_supply_route_type<T>;


// Is this type any dcon id type. Can be either regular or ve
template<typename T, typename ID>
concept any_dcon_id_type = (dcon_id_type<ID> && std::is_same_v<T, ID>) || dcon_id_ve_type<T, ID>;



template<typename T, typename value_type>
concept ve_value_type = std::is_same_v<T, ve::value_to_vector_type<value_type>>;

template<typename T, typename value_type>
concept regular_or_ve_value_type = std::is_same_v<T, value_type> || ve_value_type<T, value_type>;



}
