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

	// NOTE: these should not be referred to directly by the game mechanics
	//       they are, however, useful for the ui to filter provinces / nations by continent
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

bool nations_are_adjacent(sys::state& state, dcon::nation_id a, dcon::nation_id b);
void update_connected_regions(sys::state& state);
void restore_unsaved_values(sys::state& state);

template<typename T>
auto is_overseas(sys::state const& state, T ids);

// can we just do without this?
// void update_state_administrative_efficiency(sys::state& state);

float monthly_net_pop_growth(sys::state& state, dcon::province_id id);
float monthly_net_pop_promotion_and_demotion(sys::state& state, dcon::province_id id);
float monthly_net_pop_internal_migration(sys::state& state, dcon::province_id id);
float monthly_net_pop_external_migration(sys::state& state, dcon::province_id id);
float rgo_maximum_employment(sys::state& state, dcon::province_id id);
float rgo_employment(sys::state& state, dcon::province_id id);
float rgo_income(sys::state& state, dcon::province_id id);
float rgo_production_quantity(sys::state& state, dcon::province_id id);
float crime_fighting_efficiency(sys::state& state, dcon::province_id id);
float state_admin_efficiency(sys::state& state, dcon::state_instance_id id);

}

