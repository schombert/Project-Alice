#pragma once
#include "dcon_generated.hpp"
#include "text.hpp"

namespace ui {

void modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation);
void active_single_modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation,
		bool& header, dcon::national_modifier_value nmid, float scaled = 1.f);
void active_single_modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation,
		bool& header, dcon::provincial_modifier_value pmid, float scaled = 1.f);
void active_modifiers_description(sys::state& state, text::layout_base& layout, dcon::province_id p, int32_t identation,
		dcon::provincial_modifier_value nmid, bool have_header);
void active_modifiers_description(sys::state& state, text::layout_base& layout, dcon::nation_id n, int32_t identation,
		dcon::national_modifier_value nmid, bool have_header);
void display_battle_reinforcement_modifiers(sys::state& state, dcon::land_battle_id b, text::layout_base& contents, int32_t indent);

} // namespace ui
