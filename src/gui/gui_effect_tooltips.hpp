#pragma once
#include "system_state_forward.hpp"
#include "text.hpp"
#include "dcon_generated_ids.hpp"

namespace ui {

void effect_description(sys::state& state, text::layout_base& layout, dcon::effect_key k, int32_t primary_slot, int32_t this_slot,
		int32_t from_slot, uint32_t r_lo, uint32_t r_hi);

} // namespace ui
