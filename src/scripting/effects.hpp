#pragma once

#include "script_constants.hpp"
#include "dcon_generated.hpp"
#include "container_types.hpp"

namespace effect {

void execute(sys::state& state, dcon::effect_key key, int32_t primary, int32_t this_slot, int32_t from_slot, uint32_t r_lo,
	uint32_t r_hi);
void execute(sys::state& state, uint16_t const* data, int32_t primary, int32_t this_slot, int32_t from_slot, uint32_t r_lo,
	uint32_t r_hi);

} // namespace effect
