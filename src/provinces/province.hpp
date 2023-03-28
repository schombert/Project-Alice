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

#define CONTINENTS_LIST \
	CONTINENTS_LIST_ELEM(europe) \
	CONTINENTS_LIST_ELEM(asia) \
	CONTINENTS_LIST_ELEM(africa) \
	CONTINENTS_LIST_ELEM(north_america) \
	CONTINENTS_LIST_ELEM(south_america) \
	CONTINENTS_LIST_ELEM(oceania) \
	CONTINENTS_LIST_ELEM(mena) \
	CONTINENTS_LIST_ELEM(indochina) \
	CONTINENTS_LIST_ELEM(east_indies) \
	CONTINENTS_LIST_ELEM(central_africa) \
	CONTINENTS_LIST_ELEM(west_africa) \
	CONTINENTS_LIST_ELEM(east_africa) \
	CONTINENTS_LIST_ELEM(south_west_africa) \
	CONTINENTS_LIST_ELEM(australia_new_zealand) \
	CONTINENTS_LIST_ELEM(polynesia) \
	CONTINENTS_LIST_ELEM(isle_of_man) \
	CONTINENTS_LIST_ELEM(lakes) \
	CONTINENTS_LIST_ELEM(greenland) \
	CONTINENTS_LIST_ELEM(gondor) \
	CONTINENTS_LIST_ELEM(forodwaith) \
	CONTINENTS_LIST_ELEM(khand) \
	CONTINENTS_LIST_ELEM(harad) \
	CONTINENTS_LIST_ELEM(rhovanion) \
	CONTINENTS_LIST_ELEM(mordor) \
	CONTINENTS_LIST_ELEM(rhun) \
	CONTINENTS_LIST_ELEM(erebor) \
	CONTINENTS_LIST_ELEM(grey_mountains) \
	CONTINENTS_LIST_ELEM(mirkwood) \
	CONTINENTS_LIST_ELEM(anduin_vale) \
	CONTINENTS_LIST_ELEM(rohan) \
	CONTINENTS_LIST_ELEM(enedwaith) \
	CONTINENTS_LIST_ELEM(misty_mountains) \
	CONTINENTS_LIST_ELEM(eriador)

#define CONTINENTS_LIST_ELEM(x) x,
enum continent_list { none = 0, CONTINENTS_LIST count };
#undef CONTINENTS_LIST_ELEM

std::string_view get_continent_name(continent_list id) {
#define CONTINENTS_LIST_ELEM(x) \
	if(id == continent_list::x) { \
		return #x; \
	}
	CONTINENTS_LIST
#undef CONTINENTS_LIST_ELEM
	return "";
}

continent_list get_continent_id(std::string_view name) {
#define CONTINENTS_LIST_ELEM(x) \
	if(parsers::is_fixed_token_ci(name.data(), name.data() + name.length(), #x)) { \
		return continent_list::x; \
	}
	CONTINENTS_LIST
#undef CONTINENTS_LIST_ELEM
	return continent_list::none;
}

#undef CONTINENTS_LIST

struct global_provincial_state {
	std::vector<dcon::province_adjacency_id> canals;
	ankerl::unordered_dense::map<dcon::modifier_id, dcon::gfx_object_id, sys::modifier_hash> terrain_to_gfx_map;

	dcon::province_id first_sea_province;

	// NOTE: these should not be referred to directly by the game mechanics
	//       they are, however, useful for the ui to filter provinces / nations by continent
	ankerl::unordered_dense::map<continent_list, dcon::modifier_id> continents;
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

bool has_railroads_being_built(sys::state& state, dcon::province_id id);
bool can_build_railroads(sys::state& state, dcon::province_id id);

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
float revolt_risk(sys::state& state, dcon::province_id id);
}

