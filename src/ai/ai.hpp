#pragma once
#include "dcon_generated.hpp"
#include "container_types.hpp"
#include "text.hpp"

namespace ai {

void take_ai_decisions(sys::state& state);
void update_ai_ruling_party(sys::state& state);
void update_ai_colonial_investment(sys::state& state);
void update_ai_colony_starting(sys::state& state);
void upgrade_colonies(sys::state& state);
void civilize(sys::state& state);
void take_reforms(sys::state& state);
void remove_ai_data(sys::state& state, dcon::nation_id n);
void update_ships(sys::state& state);
void build_ships(sys::state& state);
void refresh_home_ports(sys::state& state);
void daily_cleanup(sys::state& state);
void move_idle_guards(sys::state& state);
void update_land_constructions(sys::state& state);
void update_naval_transport(sys::state& state);
void move_gathered_attackers(sys::state& state);
void gather_to_battle(sys::state& state, dcon::nation_id n, dcon::province_id p);
void make_attacks(sys::state& state);
void make_defense(sys::state& state);
void general_ai_unit_tick(sys::state& state);

float estimate_army_offensive_strength(sys::state& state, dcon::army_id a);
float estimate_army_defensive_strength(sys::state& state, dcon::army_id a);
float estimate_rebel_strength(sys::state& state, dcon::province_id p);

void update_ai_embargoes(sys::state& state);

}
