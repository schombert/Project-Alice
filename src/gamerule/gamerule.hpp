#pragma once
#include "dcon_generated.hpp"
#include "effects.hpp"

namespace sys {
	struct state;
}

namespace gamerule {

struct gamerule_option {
	dcon::text_key name;
	dcon::effect_key on_select;
	dcon::effect_key on_deselect;

};


enum class sphereling_declare_war_settings : uint8_t {
	no = 0,
	yes = 1
};

enum class partial_retreat_settings : uint8_t {
	disable = 0,
	enable = 1
};
enum class fog_of_war_settings : uint8_t {
	disable = 0,
	enable = 1
};
enum class auto_concession_peace_settings : uint8_t {
	cannot_reject = 0,
	can_reject = 1
};



struct hardcoded_gamerules {
	dcon::gamerule_id sphereling_can_declare_spherelord;
	dcon::gamerule_id allow_partial_retreat;
	dcon::gamerule_id fog_of_war;
	dcon::gamerule_id auto_concession_peace;

};

void load_hardcoded_gamerules(parsers::scenario_building_context& context);

void restore_gamerule_ui_settings(sys::state& state);

void set_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting);

bool check_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t setting);


}



