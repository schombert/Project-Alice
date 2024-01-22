#pragma once

#include "text.hpp"

namespace ui {

namespace trigger_tooltip {
void make_trigger_description(sys::state& ws, text::layout_base& layout, uint16_t const* tval, int32_t primary_slot,
		int32_t this_slot, int32_t from_slot, int32_t indentation, bool show_condition);
}

void trigger_description(sys::state& state, text::layout_base& layout, dcon::trigger_key k, int32_t primary_slot,
		int32_t this_slot, int32_t from_slot);
void multiplicative_value_modifier_description(sys::state& state, text::layout_base& layout, dcon::value_modifier_key modifier,
		int32_t primary_slot, int32_t this_slot, int32_t from_slot);
void additive_value_modifier_description(sys::state& state, text::layout_base& layout, dcon::value_modifier_key modifier,
		int32_t primary_slot, int32_t this_slot, int32_t from_slot);

} // namespace ui
