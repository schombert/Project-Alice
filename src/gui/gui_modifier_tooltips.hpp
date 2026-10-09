#pragma once
#include "dcon_generated_ids.hpp"
#include "text.hpp"

namespace ui {
void active_single_hardcoded_modifier_description(sys::state& state, text::layout_base& layout, std::string_view mod_name, float value, int32_t indentation, bool& header, dcon::national_modifier_value nmid);
void active_single_hardcoded_modifier_description(sys::state& state, text::layout_base& layout, std::string_view mod_name, float value, int32_t indentation, bool& header, dcon::provincial_modifier_value pmid);

void modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation, float scale);
void active_single_modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation,
		bool& header, dcon::national_modifier_value nmid, float scaled = 1.f);
void active_single_modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation,
		bool& header, dcon::provincial_modifier_value pmid, float scaled = 1.f);
void active_modifiers_description(sys::state& state, text::layout_base& layout, dcon::province_id p, int32_t identation,
		dcon::provincial_modifier_value nmid, bool have_header);
void active_modifiers_description(sys::state& state, text::layout_base& layout, dcon::nation_id n, int32_t identation,
		dcon::national_modifier_value nmid, bool have_header);
void display_land_battle_supply_satisfaction(sys::state& state, dcon::land_battle_id b, text::layout_base& contents, int32_t indent, bool attacker);
void display_unit_stats(sys::state& state, text::columnar_layout& contents, dcon::nation_id controller, dcon::unit_type_id unit_type);

} // namespace ui
