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

inline uint8_t set_fort_level(uint32_t new_level, uint8_t old_value) {
	return uint8_t((new_level & 0x0F) | (old_value & 0xF0));
}
inline uint8_t set_naval_base_level(uint32_t new_level, uint8_t old_value) {
	return uint8_t(((new_level << 4) & 0xF0) | (old_value & 0x0F));
}
inline int32_t get_fort_level(uint8_t value) {
	return int32_t(value & 0x0F);
}
inline int32_t get_naval_base_level(uint8_t value) {
	return int32_t((value >> 4) & 0x0F);
}
bool nations_are_adjacent(sys::state& state, dcon::nation_id a, dcon::nation_id b);
void update_connected_regions(sys::state& state);
void restore_unsaved_values(sys::state& state);

template<typename T>
auto is_overseas(sys::state const& state, T ids);

}

