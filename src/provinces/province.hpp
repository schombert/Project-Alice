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
	dcon::modifier_id modifier_by_terrain_index[64] = {}; // these are the given mappings from the raw palette index to terrain type
	uint32_t color_by_terrain_index[64] = { 0 }; // these are the (packed) colors given for the terrain type modifier at the given palette index
};

}

