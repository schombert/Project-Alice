#pragma once

#include "dcon_generated.hpp"
#include "system_state.hpp"

namespace politics {

float vote_total(sys::state& state, dcon::nation_id nation);
float get_weighted_vote_size(sys::state& state, dcon::nation_id nation, dcon::pop_id pop);

}
