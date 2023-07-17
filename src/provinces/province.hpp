#pragma once

#include "dcon_generated.hpp"

namespace province {

inline constexpr uint16_t to_map_id(dcon::province_id id) {
	return uint16_t(id.index() + 1);
}
inline constexpr dcon::province_id from_map_id(uint16_t id) {
	if(id == 0)
		return dcon::province_id();
	else
		return dcon::province_id(id - 1);
}

struct global_provincial_state {
	std::vector<dcon::province_adjacency_id> canals;
	ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash> terrain_to_gfx_map;

	dcon::province_id first_sea_province;
	dcon::modifier_id europe;
	dcon::modifier_id asia;
	dcon::modifier_id africa;
	dcon::modifier_id north_america;
	dcon::modifier_id south_america;
	dcon::modifier_id oceania;
};

template<typename F>
void for_each_land_province(sys::state& state, F const& func);
template<typename F>
void for_each_sea_province(sys::state& state, F const& func);
template<typename F>
void for_each_province_in_state_instance(sys::state& state, dcon::state_instance_id s, F const& func);
template<typename F>
void ve_for_each_land_province(sys::state& state, F const& func);

bool nations_are_adjacent(sys::state& state, dcon::nation_id a, dcon::nation_id b);
void update_connected_regions(sys::state& state);
void update_cached_values(sys::state& state);
void restore_unsaved_values(sys::state& state);

template<typename T>
auto is_overseas(sys::state const& state, T ids);
bool is_overseas(sys::state const& state, dcon::province_id ids);
bool can_integrate_colony(sys::state& state, dcon::state_instance_id id);
dcon::province_id get_connected_province(sys::state& state, dcon::province_adjacency_id adj, dcon::province_id curr);
float colony_integration_cost(sys::state& state, dcon::state_instance_id id);
float state_accepted_bureaucrat_size(sys::state& state, dcon::state_instance_id id);

// can we just do without this?
// void update_state_administrative_efficiency(sys::state& state);

bool has_railroads_being_built(sys::state& state, dcon::province_id id);
bool can_build_railroads(sys::state& state, dcon::province_id id, dcon::nation_id n);
bool generic_can_build_railroads(sys::state& state, dcon::province_id id, dcon::nation_id n);
bool has_fort_being_built(sys::state& state, dcon::province_id id);
bool can_build_fort(sys::state& state, dcon::province_id id, dcon::nation_id n);
bool has_naval_base_being_built(sys::state& state, dcon::province_id id);
bool can_build_naval_base(sys::state& state, dcon::province_id id, dcon::nation_id n);
bool has_an_owner(sys::state& state, dcon::province_id id);
bool state_is_coastal(sys::state& state, dcon::state_instance_id s);

dcon::province_id pick_capital(sys::state& state, dcon::nation_id n);

float monthly_net_pop_growth(sys::state& state, dcon::province_id id);
float monthly_net_pop_promotion_and_demotion(sys::state& state, dcon::province_id id);
float monthly_net_pop_internal_migration(sys::state& state, dcon::province_id id);
float monthly_net_pop_external_migration(sys::state& state, dcon::province_id id);
float rgo_maximum_employment(sys::state& state, dcon::province_id id);
float rgo_employment(sys::state& state, dcon::province_id id);
float rgo_income(sys::state& state, dcon::province_id id);
float rgo_production_quantity(sys::state& state, dcon::province_id id);
float rgo_size(sys::state& state, dcon::province_id prov_id);
float state_admin_efficiency(sys::state& state, dcon::state_instance_id id);
float crime_fighting_efficiency(sys::state& state, dcon::province_id id);
float revolt_risk(sys::state& state, dcon::province_id id);

void change_province_owner(sys::state& state, dcon::province_id id, dcon::nation_id new_owner);
void conquer_province(sys::state& state, dcon::province_id id, dcon::nation_id new_owner);

void update_crimes(sys::state& state);
void update_nationalism(sys::state& state);

bool can_start_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d);
bool fast_can_start_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d, int32_t free_points, bool state_is_coastal, bool& adjacent);
bool can_invest_in_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d);
bool is_colonizing(sys::state& state, dcon::nation_id n, dcon::state_definition_id d);
void update_colonization(sys::state& state);
void increase_colonial_investment(sys::state& state, dcon::nation_id source, dcon::state_definition_id state_def);

void add_core(sys::state& state, dcon::province_id prov, dcon::national_identity_id tag);
void remove_core(sys::state& state, dcon::province_id prov, dcon::national_identity_id tag);
void set_rgo(sys::state& state, dcon::province_id prov, dcon::commodity_id c);
void enable_canal(sys::state& state, int32_t id);

// distance from a state to a given province (does not pathfind)
float state_distance(sys::state& state, dcon::state_instance_id state_id, dcon::province_id prov_id);
// distance between to adjacent provinces
float distance(sys::state& state, dcon::province_adjacency_id pair);
// direct distance between two provinces; does not pathfind
float direct_distance(sys::state& state, dcon::province_id a, dcon::province_id b);

// determines whether a land unit is allowed to move to / be in a province
bool has_access_to_province(sys::state& state, dcon::nation_id nation_as, dcon::province_id prov);
// whether a ship can dock at a land province
bool has_naval_access_to_province(sys::state& state, dcon::nation_id nation_as, dcon::province_id prov);

//
// when pathfinding, check that the destination province is valid on its own (i.e. accessible for normal, or embark-able for sea)
//

// normal pathfinding
std::vector<dcon::province_id> make_land_path(sys::state& state, dcon::province_id start, dcon::province_id end,
		dcon::nation_id nation_as, dcon::army_id a);
// used for rebel unit and black-flagged unit pathfinding
std::vector<dcon::province_id> make_unowned_land_path(sys::state& state, dcon::province_id start, dcon::province_id end);
// naval unit pathfinding; start and end provinces may be land provinces; function assumes you have naval access to both
std::vector<dcon::province_id> make_naval_path(sys::state& state, dcon::province_id start, dcon::province_id end);

std::vector<dcon::province_id> make_naval_retreat_path(sys::state& state, dcon::nation_id nation_as, dcon::province_id start);
std::vector<dcon::province_id> make_land_retreat_path(sys::state& state, dcon::nation_id nation_as, dcon::province_id start);

} // namespace province
