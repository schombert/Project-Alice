#pragma once

#include "dcon_generated_ids.hpp"
#include "commands.hpp"

namespace economy {
command::budget_settings_data budget_minimums(sys::state& state, dcon::nation_id n);
command::budget_settings_data budget_maximums(sys::state& state, dcon::nation_id n);
}
