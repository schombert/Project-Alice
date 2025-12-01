#pragma once

namespace game_scene {

enum class scene_id : uint8_t {
	pick_nation,
	in_game_basic,
	in_game_military,
	in_game_state_selector,
	in_game_national_identity_selector,
	in_game_military_selector,
	in_game_economy_viewer,
	end_screen,
	count
};

enum class borders_granularity : uint8_t {
	none, province, state, nation
};
}
