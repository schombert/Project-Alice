#pragma once
#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace ai {
void update_influence_priorities(sys::state& state);
void perform_influence_actions(sys::state& state);
}
