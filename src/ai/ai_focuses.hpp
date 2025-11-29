#pragma once
#include "dcon_generated_ids.hpp"

namespace sys {
struct state;
}

namespace ai {
void update_focuses(sys::state& state);
void identify_focuses(sys::state& state);
}
