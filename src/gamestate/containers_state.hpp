#pragma once

#include "constants_state.hpp"
#include "game_scene_default.hpp"
#include "economy_common_api_containers.hpp"
#include "nations_state_containers.hpp"
#include "military_state_containers.hpp"
#include "diplomatic_messages_containers.hpp"
#include "events_containers.hpp"
#include "opengl_wrapper_containers.hpp"
#include "ui_state.hpp"
#include "commands_containers.hpp"

namespace province {
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
}
