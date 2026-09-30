#pragma once
#include "system_state_forward.hpp"
namespace supply_routes {


dcon::province_id supply_route_get_destination(const sys::state& state, dcon::army_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::navy_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::naval_construction_supply_route_id route);

dcon::province_id supply_route_get_origin(const sys::state& state, dcon::army_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::navy_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::naval_construction_supply_route_id route);

template<concepts::supply_route_type route_type>
float supply_route_get_supply_loss(const sys::state& state, route_type route);
template<concepts::supply_route_type route_type>
float supply_route_get_throughput(const sys::state& state, route_type route);

template<concepts::supply_route_type route_type>
bool supply_route_is_active(const sys::state& state, route_type route);

template<concepts::unit_supply_or_build_commodity_type commodity_type, concepts::military_supply_route_type route_type>
float military_route_get_buffered_goods(const sys::state& state, route_type route, commodity_type commodity_id);


dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::army_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::navy_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::naval_construction_supply_route_id route);

dcon::supply_route_path_id create_supply_route_path_no_pathing(sys::state& state, dcon::province_id destination, dcon::market_id origin, bool attempting_to_route);

template<concepts::supply_route_type route_type>
dcon::supply_route_path_id supply_route_get_path(const sys::state& state, route_type route);

template<concepts::supply_route_type route_type>
dcon::market_id supply_route_get_origin_market(const sys::state& state, route_type route);

// Gets the avg supply loss of all supply routes a nation owns
float nation_get_avg_supply_loss(const sys::state& state, dcon::nation_id nation);
// Gets the avg supply throughput of all supply routes a nation owns
float nation_get_avg_supply_throughput(const sys::state& state, dcon::nation_id nation);
// Gets the avg supply loss of all construction supply routes a nation owns
float nation_get_avg_construction_supply_loss(const sys::state& state, dcon::nation_id nation);
// Gets the avg supply throughput of all construction supply routes a nation owns
float nation_get_avg_construction_supply_throughput(const sys::state& state, dcon::nation_id nation);
// Get the avg supply loss for all construction routes of a certain type a nation owns
template<concepts::construction_type con_type>
float nation_get_avg_construction_supply_loss_by_type(const sys::state& state, dcon::nation_id nation);
// Get the avg supply throughput for all construction routes of a certain type a nation owns
template<concepts::construction_type con_type>
float nation_get_avg_construction_supply_throughput_by_type(const sys::state& state, dcon::nation_id nation);


// Gets the avg supply loss of military supply routes a nation owns. Either army routes or navy routes depending on template param
template<concepts::military_unit unit_type>
float nation_get_avg_military_supply_loss_by_type(const sys::state& state, dcon::nation_id nation);
// Gets the avg supply throughput of military supply routes a nation owns. Either army routes or navy routes depending on template param
template<concepts::military_unit unit_type>
float nation_get_avg_military_supply_throughput_by_type(const sys::state& state, dcon::nation_id nation);

// Gets the avg supply loss of military supply routes a nation owns. Either army routes or navy routes, and either supply or reinforcement depending on template params
template<military::unit_consumption_type consume_type, concepts::military_unit unit_type>
float nation_get_avg_military_supply_loss_by_type(const sys::state& state, dcon::nation_id nation);
// Gets the avg supply throughput of military supply routes a nation owns. Either army routes or navy routes, and either supply or reinforcement depending on template params
template<military::unit_consumption_type consume_type, concepts::military_unit unit_type>
float nation_get_avg_military_supply_throughput_by_type(const sys::state& state, dcon::nation_id nation);

int8_t army_supply_setting_min(const sys::state& state, dcon::nation_id nation);
int8_t army_supply_setting_max(const sys::state& state, dcon::nation_id nation);
int8_t army_reinforcement_setting_min(const sys::state& state, dcon::nation_id nation);
int8_t army_reinforcement_setting_max(const sys::state& state, dcon::nation_id nation);
int8_t navy_supply_setting_min(const sys::state& state, dcon::nation_id nation);
int8_t navy_supply_setting_max(const sys::state& state, dcon::nation_id nation);
int8_t navy_reinforcement_setting_min(const sys::state& state, dcon::nation_id nation);
int8_t navy_reinforcement_setting_max(const sys::state& state, dcon::nation_id nation);
int8_t army_construction_setting_min(const sys::state& state, dcon::nation_id nation);
int8_t army_construction_setting_max(const sys::state& state, dcon::nation_id nation);
int8_t navy_construction_setting_min(const sys::state& state, dcon::nation_id nation);
int8_t navy_construction_setting_max(const sys::state& state, dcon::nation_id nation);
int8_t factory_construction_setting_min(const sys::state& state, dcon::nation_id nation);
int8_t factory_construction_setting_max(const sys::state& state, dcon::nation_id nation);
int8_t building_construction_setting_min(const sys::state& state, dcon::nation_id nation);
int8_t building_construction_setting_max(const sys::state& state, dcon::nation_id nation);


float supply_loss_to_loss_multiplier(float base_loss_rate, float commodity_loss_mult);

// Schedules a path update on the specific supply path as soon as possible
void schedule_immediate_supply_path_update(sys::state& state, dcon::supply_route_path_id path);
// Schedules a path updatr on all paths which have this market as their origin as soon as possible
void schedule_immediate_supply_path_update_on_origin_market(sys::state& state, dcon::market_id path_origin);
// Schedules a path update on all supply paths passing through the given province on next weekly tick
void schedule_prov_all_supply_paths_update(sys::state& state, dcon::province_id to_update);
// Schedules a path update on all supply paths passing through the given province controlled by enemy nations on next weekly tick
void schedule_prov_enemy_supply_paths_update(sys::state& state, dcon::province_id to_update, dcon::nation_id nation);
// Schedules a path update on all supply paths passing through the given province controlled by nation in common wars on next weekly tick
void schedule_prov_common_war_supply_paths_update(sys::state& state, dcon::province_id to_update, dcon::nation_id nation);
// Schedules a path update on all supply paths passing through the given province controlled by nation on next weekly tick
void schedule_prov_specific_nation_supply_paths_update(sys::state& state, dcon::province_id to_update, dcon::nation_id nation);
// Schedules a path update on all supply routes owned by the nation on next weekly tick
void schedule_nation_supply_paths_update(sys::state& state, dcon::nation_id nation);
// Schedules supply paths which are considered to be ineffective & are active to be updated as soon as possible. Can be expensive
void schedule_active_ineffective_supply_paths_update(sys::state& state);

// Computes the efficiency of a construct with has consumed vs available. Eg supply throughput
float compute_efficiency(float consumed, float available);

float port_supply_capacity_mult_hostile_troops_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

float port_supply_capacity_mult_blockaded_modifier(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as);

float port_supply_capacity_in_province(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as);

float port_supply_capacity_mult_supply_access_modifier(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as);

float supply_throughput_mult_access_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float supply_throughput_mult_hostile_troops_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

// Gets the max supply throughput modifier in goods volume for the specified nation in the specified province, with percentage-based modifiers taken into account
float calculate_supply_throughput_in_province(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);
float calculate_supply_throughput_in_noncoastal_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::province_id prov_1, dcon::province_id prov_2, float prov_1_throughput, float prov_2_throughput, dcon::nation_id nation);
float calculate_supply_throughput_in_noncoastal_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation);
float calculate_supply_throughput_in_coastal_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::province_id prov_1, dcon::province_id prov_2, float prov_1_capacity, float prov_2_capacity, dcon::nation_id nation);
float calculate_supply_throughput_in_coastal_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation);

// Calculates the actual supply throughput after taking into account both regular supply throughput and potential port supply capacity
float calculate_supply_throughput_in_adjacency(const sys::state& state, dcon::province_adjacency_id adj, dcon::nation_id nation);

// Also takes into account potential port supply capacity modifiers if relavent
float supply_throughput_efficiency(const sys::state& state, dcon::province_adjacency_id adj, dcon::nation_id nation_as);

float supply_loss_add_hostile_armies(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);

float calculate_supply_loss_in_province(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float calculate_adjacency_net_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as);
float calculate_adjacency_avg_supply_loss(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as);
float calculate_adjacency_avg_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as);
float calculate_adjacency_avg_supply_loss(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, float prov_1_loss, float prov_2_loss, dcon::nation_id nation_as);
void update_supply_routes_daily(sys::state& state);


template<command::actor actor>
bool can_change_army_supply_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template<command::actor actor>
void change_army_supply_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_army_reinforcement_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template<command::actor actor>
void change_army_reinforcement_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_navy_supply_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template<command::actor actor>
void change_navy_supply_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_navy_reinforcement_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template<command::actor actor>
void change_navy_reinforcement_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_army_construction_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template<command::actor actor>
void change_army_construction_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_navy_construction_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template<command::actor actor>
void change_navy_construction_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_factory_construction_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template<command::actor actor>
void change_factory_construction_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting);

template<command::actor actor>
bool can_change_building_construction_consumption_setting(const sys::state& state, dcon::nation_id source, int8_t new_setting);
template<command::actor actor>
void change_building_construction_consumption_setting(sys::state& state, dcon::nation_id source, int8_t new_setting);


}
