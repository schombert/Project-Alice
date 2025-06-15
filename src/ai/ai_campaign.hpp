#pragma once
#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace ai {
void update_ai_campaign_strategy(sys::state& state);
void update_ai_general_status(sys::state& state);
void update_ai_research(sys::state& state);
void initialize_ai_tech_weights(sys::state& state);
}
