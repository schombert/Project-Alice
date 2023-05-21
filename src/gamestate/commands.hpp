#pragma once
#include "dcon_generated.hpp"
#include "common_types.hpp"

namespace command {

enum class command_type : uint8_t {
	invalid = 0,
	change_nat_focus = 1
};

struct national_focus_data {
	dcon::state_instance_id target_state;
	dcon::national_focus_id focus;
};

struct payload {
	union dtype {
		national_focus_data nat_focus;
		dtype() {}
	} data;
	dcon::nation_id source;
	command_type type = command_type::invalid;

	payload() {}
};

void set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus);
bool can_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus);

void execute_pending_commands(sys::state& state);

}
