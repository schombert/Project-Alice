#pragma once

#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"

namespace economy {
int32_t factory_priority(sys::state const& state, dcon::factory_id f);
void set_factory_priority(sys::state& state, dcon::factory_id f, int32_t priority);
bool factory_is_profitable(sys::state const& state, dcon::factory_id f);
bool factory_is_closed(sys::state const& state, dcon::factory_id f);
float factory_total_employment_score(sys::state const& state, dcon::factory_id f);
}
