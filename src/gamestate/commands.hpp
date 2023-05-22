#pragma once
#include "dcon_generated.hpp"
#include "common_types.hpp"

namespace command {

enum class command_type : uint8_t {
	invalid = 0,
	change_nat_focus = 1,
	start_research = 2,
};

struct national_focus_data {
	dcon::state_instance_id target_state;
	dcon::national_focus_id focus;
};

struct start_research_data {
	dcon::technology_id tech;
};

struct payload {
	union dtype {
		national_focus_data nat_focus;
		start_research_data start_research;
		dtype() {}
	} data;
	dcon::nation_id source;
	command_type type = command_type::invalid;

	payload() {}
};

void set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus);
bool can_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus);

void start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech);
bool can_start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech);

void execute_pending_commands(sys::state& state);

}
