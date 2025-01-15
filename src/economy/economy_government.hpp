#pragma once
#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace economy {
float estimate_spendings_administration_capital(sys::state& state, dcon::nation_id n, float budget_priority);
float estimate_spendings_administration_state(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid, float budget_priority);
float estimate_spendings_administration(sys::state& state, dcon::nation_id n, float budget_priority);
float full_spendings_administration(sys::state& state, dcon::nation_id n);
void update_consumption_administration(sys::state& state, dcon::nation_id n);
}
