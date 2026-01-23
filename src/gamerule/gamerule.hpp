#pragma once
#include "dcon_generated_ids.hpp"
#include "effects.hpp"

namespace sys {
struct state;
}

namespace parsers {
struct scenario_building_context;
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
	enable = 1,
	disable_for_observer = 2,
};
enum class auto_concession_peace_settings : uint8_t {
	cannot_reject = 0,
	can_reject = 1
};

enum class command_units_settings : uint8_t {
	disabled = 0,
	enabled = 1
};


struct hardcoded_gamerules {
	dcon::gamerule_id sphereling_can_declare_spherelord;
	dcon::gamerule_id allow_partial_retreat;
	dcon::gamerule_id fog_of_war;
	dcon::gamerule_id auto_concession_peace;
	dcon::gamerule_id command_units;

};

void load_hardcoded_gamerules(parsers::scenario_building_context& context, parsers::error_handler& err);

void restore_gamerule_ui_settings(sys::state& state);

void set_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting);
void set_gamerule_no_lua_exec(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting);

bool check_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t setting);

dcon::gamerule_id get_gamerule_id_by_name(const sys::state& state, std::string_view gamerule_name);

// Returns the gamerule option id with the given name
uint8_t get_gamerule_option_id_by_name(const sys::state& state, std::string_view gamerule_option_name);

uint8_t get_active_gamerule_option(const sys::state& state, dcon::gamerule_id gamerule);


}



