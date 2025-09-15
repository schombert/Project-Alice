#pragma once
#include "dcon_generated.hpp"


namespace gamerule {

enum class sphereling_declare_war_settings : uint8_t {
	no = 0,
	yes = 1
};

struct hardcoded_gamerules {
	dcon::gamerule_id sphereling_can_declare_spherelord;

};

void load_hardcoded_gamerules(sys::state& state);
}
