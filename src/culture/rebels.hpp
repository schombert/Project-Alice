#pragma once
#include "dcon_generated.hpp"

namespace rebel {

dcon::movement_id get_movement_by_position(sys::state& state, dcon::nation_id n, dcon::issue_option_id o);
dcon::movement_id get_movement_by_independence(sys::state& state, dcon::nation_id n, dcon::national_identity_id i);

void update_movement_values(sys::state& state); // simply updates cached values
void turn_movement_into_rebels(sys::state& state, dcon::movement_id m);
void update_movements(sys::state& state); // updates cached values and then possibly turns movements into rebels
void add_pop_to_movement(sys::state& state, dcon::pop_id p, dcon::movement_id m);
void remove_pop_from_movement(sys::state& state, dcon::pop_id p);
void suppress_movement(sys::state& state, dcon::nation_id n, dcon::movement_id m);

void daily_update_rebel_organization(sys::state& state); // increases org based on pop income and militancy
void update_factions(sys::state& state);								 // adds pops to factions, deletes invalid factions
dcon::rebel_faction_id get_faction_by_type(sys::state& state, dcon::nation_id n, dcon::rebel_type_id r);
void remove_pop_from_rebel_faction(sys::state& state, dcon::pop_id p);

bool sphere_member_has_ongoing_revolt(sys::state& state, dcon::nation_id n);
int32_t get_faction_brigades_ready(sys::state& state, dcon::rebel_faction_id r);
int32_t get_faction_brigades_active(sys::state& state, dcon::rebel_faction_id r);
float get_faction_organization(sys::state& state, dcon::rebel_faction_id r);
float get_faction_revolt_risk(sys::state& state, dcon::rebel_faction_id r);
float get_suppression_point_cost(sys::state& state, dcon::movement_id m);

void execute_rebel_victories(sys::state& state);
void execute_province_defections(sys::state& state);

void trigger_revolt(sys::state& state, dcon::nation_id n, dcon::rebel_type_id t, dcon::ideology_id i, dcon::culture_id c,
		dcon::religion_id r);

} // namespace rebel
