#pragma once
#include "dcon_generated.hpp"

namespace sys {
	struct state;
}

namespace gamerule {

inline constexpr uint32_t MAX_GAMERULE_SETTINGS = 30;


enum class sphereling_declare_war_settings : uint8_t {
	no = 0,
	yes = 1
};

struct hardcoded_gamerules {
	dcon::gamerule_id sphereling_can_declare_spherelord;

};

void load_hardcoded_gamerules(sys::state& state);

void restore_gamerule_ui_settings(sys::state& state);

void set_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting);

bool check_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t setting);


}



