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

}
