#pragma once

#include "dcon_generated.hpp"
#include "script_constants.hpp"
#include "system_state.hpp"

namespace event {

void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n);
void trigger_national_event(sys::state& state, dcon::free_national_event_id e, dcon::nation_id n);

}
