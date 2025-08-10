#pragma once

#include "dcon_generated.hpp"
#include "constants.hpp"

namespace province {

inline constexpr float naval_range_distance_mult = 0.391f; // multiplier applied to regular direct distances to compute the naval range distance. This is so default naval range values match up with the expected values from vic2.

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
	std::vector<dcon::province_id> canal_provinces;
	ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash> terrain_to_gfx_map;
	std::vector<bool> connected_region_is_coastal;
	dcon::province_id first_sea_province;
	dcon::modifier_id europe;
	dcon::modifier_id asia;
	dcon::modifier_id africa;
	dcon::modifier_id north_america;
	dcon::modifier_id south_america;
	dcon::modifier_id oceania;
};

struct naval_range_data {
	float distance;
	bool is_reachable;
};

bool province_is_deep_waters(sys::state& state, dcon::province_id prov);
bool sea_province_is_adjacent_to_accessible_coast(sys::state& state, dcon::province_id prov, dcon::nation_id nation);

bool nations_are_adjacent(sys::state& state, dcon::nation_id a, dcon::nation_id b);
void update_connected_regions(sys::state& state);
void update_cached_values(sys::state& state);
void update_blockaded_cache(sys::state& state);
void restore_unsaved_values(sys::state& state);
void restore_distances(sys::state& state);

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
bool has_province_building_being_built(sys::state& state, dcon::province_id id, economy::province_building_type t);
bool can_build_province_building(sys::state& state, dcon::province_id id, dcon::nation_id n, economy::province_building_type t);
bool has_an_owner(sys::state& state, dcon::province_id id);
float effective_life_rating_growth(sys::state& state, dcon::province_id prov); // returns the effective life rating for popgrowth for a province
dcon::province_id state_get_coastal_capital(sys::state& state, dcon::state_instance_id s);
bool state_is_coastal(sys::state& state, dcon::state_instance_id s);
bool state_is_coastal_non_core_nb(sys::state& state, dcon::state_instance_id s);
bool state_borders_nation(sys::state& state, dcon::nation_id n, dcon::state_instance_id si);

float get_province_modifier_without_hostile_buildings(sys::state& state, dcon::nation_id as_nation, dcon::province_id prov, dcon::provincial_modifier_value prov_mod_val);

dcon::province_id pick_capital(sys::state& state, dcon::nation_id n);

float state_admin_efficiency(sys::state& state, dcon::state_instance_id id);
float crime_fighting_efficiency(sys::state& state, dcon::province_id id);
float revolt_risk(sys::state& state, dcon::province_id id);

void change_province_owner(sys::state& state, dcon::province_id id, dcon::nation_id new_owner);
bool is_strait_blocked(sys::state& state, dcon::nation_id thisnation, dcon::province_id from, dcon::province_id to);
bool is_strait_blocked(sys::state& state, dcon::nation_id thisnation, dcon::province_adjacency_id adjacency);
bool is_adjacency_impassable(sys::state& state, dcon::nation_id thisnation, dcon::province_adjacency_id adj);
bool is_canal_adjacency_passable(sys::state& state, dcon::nation_id thisnation, dcon::province_adjacency_id adj);
void conquer_province(sys::state& state, dcon::province_id id, dcon::nation_id new_owner);

void update_crimes(sys::state& state);
void update_nationalism(sys::state& state);

bool can_start_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d);
bool fast_can_start_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d, int32_t free_points, dcon::province_id coastal_target, bool& adjacent);
bool can_invest_in_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d);
bool is_colonizing(sys::state& state, dcon::nation_id n, dcon::state_definition_id d);
void update_colonization(sys::state& state);
void increase_colonial_investment(sys::state& state, dcon::nation_id source, dcon::state_definition_id state_def);

void add_core(sys::state& state, dcon::province_id prov, dcon::national_identity_id tag);
void remove_core(sys::state& state, dcon::province_id prov, dcon::national_identity_id tag);
void set_rgo(sys::state& state, dcon::province_id prov, dcon::commodity_id c);
void enable_canal(sys::state& state, int32_t id);
void upgrade_colonial_state(sys::state& state, dcon::nation_id owner, dcon::state_instance_id si);

// distance from a state to a given province (does not pathfind)
float state_distance(sys::state& state, dcon::state_instance_id state_id, dcon::province_id prov_id);
// distance between to adjacent provinces
float distance(sys::state& state, dcon::province_adjacency_id pair);

// distance in kilometers between to adjacent provinces
float distance_km(sys::state& state, dcon::province_adjacency_id pair);

// direct distance between two provinces; does not pathfind
float direct_distance(sys::state& state, dcon::province_id a, dcon::province_id b);

float direct_distance_km(sys::state& state, dcon::province_id a, dcon::province_id b);

// naval range distance between a port and a sea province. Must take the distance of a naval path instead of direct distance
naval_range_data naval_range_distance(sys::state& state, dcon::province_id port_prov, dcon::province_id sea_prov, dcon::nation_id nation_as);
// sorting distance returns values such that a smaller sorting distance between two provinces
// means that they are closer, but does not translate 1 to 1 to actual distances (i.e. is the negative dot product)
float sorting_distance(sys::state& state, dcon::province_id a, dcon::province_id b);
float state_sorting_distance(sys::state& state, dcon::state_instance_id state_id, dcon::province_id prov_id);

// determines whether a land unit is allowed to move to / be in a province
bool has_access_to_province(sys::state& state, dcon::nation_id nation_as, dcon::province_id prov);
// whether a ship can dock at a land province
bool has_naval_access_to_province(sys::state& state, dcon::nation_id nation_as, dcon::province_id prov);
// determines whether a land unit is allowed to move to / be in a province that isn't an active enemy
bool has_safe_access_to_province(sys::state& state, dcon::nation_id nation_as, dcon::province_id prov);

//
// when pathfinding, check that the destination province is valid on its own (i.e. accessible for normal, or embark-able for sea)
//

// normal pathfinding
std::vector<dcon::province_id> make_land_path(sys::state& state, dcon::province_id start, dcon::province_id end, dcon::nation_id nation_as, dcon::army_id a);
// pathfind through non-enemy controlled, not under siege provinces
std::vector<dcon::province_id> make_safe_land_path(sys::state& state, dcon::province_id start, dcon::province_id end, dcon::nation_id nation_as);
std::vector<dcon::province_id> make_unowned_path(sys::state& state, dcon::province_id start, dcon::province_id end);
// used for rebel unit and black-flagged unit pathfinding
std::vector<dcon::province_id> make_unowned_land_path(sys::state& state, dcon::province_id start, dcon::province_id end);
// naval unit pathfinding; start and end provinces may be land provinces; function assumes you have naval access to both
std::vector<dcon::province_id> make_naval_path(sys::state& state, dcon::province_id start, dcon::province_id end, dcon::nation_id nation_as);
//for sea trade routes
std::vector<dcon::province_id> make_unowned_naval_path(sys::state& state, dcon::province_id start, dcon::province_id end);

std::vector<dcon::province_id> make_naval_retreat_path(sys::state& state, dcon::nation_id nation_as, dcon::province_id start);
std::vector<dcon::province_id> make_land_retreat_path(sys::state& state, dcon::nation_id nation_as, dcon::province_id start);

std::vector<dcon::province_id> make_path_to_nearest_coast(sys::state& state, dcon::nation_id nation_as, dcon::province_id start);
std::vector<dcon::province_id> make_unowned_path_to_nearest_coast(sys::state& state, dcon::province_id start);

void set_province_controller(sys::state& state, dcon::province_id p, dcon::nation_id n);
void set_province_controller(sys::state& state, dcon::province_id p, dcon::rebel_faction_id rf);

} // namespace province
