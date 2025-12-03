#pragma once

namespace military {

enum special_army_order {
	none,
	move_to_siege,
	strategic_redeployment,
	pursue_to_engage
};

enum class unit_type : uint8_t {
	support, big_ship, cavalry, transport, light_ship, special, infantry
};

enum class crossing_type {
	none, river, sea
};

enum class apply_attrition_on_arrival {
	no, yes
};

enum class battle_is_ending {
	no, yes
};

enum class retreat_type : bool {
	automatic = 0,
	manual = 1,
};

}
