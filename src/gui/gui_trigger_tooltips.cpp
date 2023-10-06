#include <string_view>
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "script_constants.hpp"
#include "triggers.hpp"

namespace ui {

namespace trigger_tooltip {

inline std::string_view cmp_code_to_fixed_ui(uint16_t code) {
	switch(code & trigger::association_mask) {
	case trigger::association_eq:
		return "cmp_eq";
	case trigger::association_le:
		return "cmp_le";
	case trigger::association_lt:
		return "cmp_lt";
	case trigger::association_ge:
		return "cmp_ge";
	case trigger::association_gt:
		return "cmp_gt";
	case trigger::association_ne:
		return "cmp_ne";
	}
	return "cmp_eq";
}

inline std::string_view cmp_has_code_to_fixed_ui(uint16_t code) {
	switch(code & trigger::association_mask) {
	case trigger::association_eq:
		return "cmp_has_eq";
	case trigger::association_le:
		return "cmp_has_le";
	case trigger::association_lt:
		return "cmp_has_lt";
	case trigger::association_ge:
		return "cmp_has_ge";
	case trigger::association_gt:
		return "cmp_has_gt";
	case trigger::association_ne:
		return "cmp_has_ne";
	}
	return "cmp_has_eq";
}

inline std::string_view any_all_code_to_fixed_ui(uint16_t code) {
	if(code & trigger::is_disjunctive_scope)
		return "any_of_the_following";
	else
		return "all_of_the_following";
}

inline std::string_view every_any_code_to_fixed_ui(uint16_t code) {
	if(code & trigger::is_existence_scope)
		return "any";
	else
		return "every";
}

void display_with_comparison(uint16_t trigger_code, text::substitution left_label, text::substitution value, sys::state& ws,
		text::layout_base& layout, text::layout_box& box) {

	text::add_to_layout_box(ws, layout, box, left_label, text::text_color::white);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, cmp_code_to_fixed_ui(trigger_code)),
			text::text_color::white);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, value, text::text_color::white);
}

void display_with_has_comparison(uint16_t trigger_code, text::substitution left_label, text::substitution value, sys::state& ws,
		text::layout_base& layout, text::layout_box& box) {

	text::add_to_layout_box(ws, layout, box, left_label, text::text_color::white);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, cmp_has_code_to_fixed_ui(trigger_code)),
			text::text_color::white);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, value, text::text_color::white);
}

void display_with_comparison(uint16_t trigger_code, text::substitution value, sys::state& ws, text::layout_base& layout,
		text::layout_box& box) {

	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, cmp_code_to_fixed_ui(trigger_code)),
			text::text_color::white);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, value, text::text_color::white);
}

void display_with_has_comparison(uint16_t trigger_code, text::substitution value, sys::state& ws, text::layout_base& layout,
		text::layout_box& box) {

	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, cmp_has_code_to_fixed_ui(trigger_code)),
			text::text_color::white);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, value, text::text_color::white);
}

void make_trigger_description(sys::state& ws, text::layout_base& layout, uint16_t const* tval, int32_t primary_slot,
		int32_t this_slot, int32_t from_slot, int32_t indentation, bool show_condition);

inline constexpr int32_t indentation_amount = 15;

inline void display_subtriggers(uint16_t const* source, sys::state& ws, text::layout_base& layout, int32_t primary_slot,
		int32_t this_slot, int32_t from_slot, int32_t indentation, bool show_condition) {

	auto const source_size = 1 + trigger::get_trigger_payload_size(source);
	auto sub_units_start = source + 2 + trigger::trigger_scope_data_payload(source[0]);
	while(sub_units_start < source + source_size) {
		make_trigger_description(ws, layout, sub_units_start, primary_slot, this_slot, from_slot, indentation, show_condition);
		sub_units_start += 1 + trigger::get_trigger_payload_size(sub_units_start);
	}
}

#define TRIGGER_DISPLAY_PARAMS                                                                                                   \
	uint16_t const *tval, sys::state &ws, text::layout_base &layout, int32_t primary_slot, int32_t this_slot, int32_t from_slot,   \
			int32_t indentation, bool show_condition

void tf_none(TRIGGER_DISPLAY_PARAMS) { }

void make_condition(TRIGGER_DISPLAY_PARAMS, text::layout_box& box) {
	if(show_condition) {
		
			if(trigger::evaluate(ws, tval, primary_slot, this_slot, from_slot)) {
				text::add_to_layout_box(ws, layout, box, std::string_view("\x02"), text::text_color::green);
				text::add_space_to_layout_box(ws, layout, box);
			} else {
				text::add_to_layout_box(ws, layout, box, std::string_view("\x01"), text::text_color::red);
				text::add_space_to_layout_box(ws, layout, box);
			}
		
	} else {
		
		text::add_to_layout_box(ws, layout, box, std::string_view("\x95"), text::text_color::white);
		text::add_space_to_layout_box(ws, layout, box);
		
	}
}

void tf_generic_scope(TRIGGER_DISPLAY_PARAMS) {
	auto st_count = trigger::count_subtriggers(tval);
	if(st_count > 1) {
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}
	display_subtriggers(tval, ws, layout, primary_slot, this_slot, from_slot, indentation + (st_count > 1 ? indentation_amount : 0),
			show_condition);
}
void tf_x_neighbor_province_scope(TRIGGER_DISPLAY_PARAMS) {

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "neighboring_province"));
		text::close_layout_box(layout, box);
	}

	auto st_count = trigger::count_subtriggers(tval);
	if(st_count > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_neighbor_province_scope_state(TRIGGER_DISPLAY_PARAMS) {

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "neighboring_province"));
		text::close_layout_box(layout, box);
	}

	auto st_count = trigger::count_subtriggers(tval);
	if(st_count > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_neighbor_country_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "neighboring_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_neighbor_country_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "neighboring_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_war_countries_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "nation_at_war_against"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_war_countries_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "nation_at_war_against"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_greater_power_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "great_power"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_country_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_owned_province_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owned_province"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(primary_slot))
													 : text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_owned_province_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owned_province"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_core_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "core_in"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot)))
													 : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_core_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "core_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_state_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "state_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_substate_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "substate_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_sphere_member_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "nation_in_sphere"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_pop_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_pop_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_pop_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_provinces_in_variable_region(TRIGGER_DISPLAY_PARAMS) {
	auto region = trigger::payload(*(tval + 2)).state_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "province_in"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, region);
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_provinces_in_variable_region_proper(TRIGGER_DISPLAY_PARAMS) {
	auto region = trigger::payload(*(tval + 2)).reg_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "province_in"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, ws.world.region_get_name(region));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_owner_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(primary_slot))
													 : text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
			primary_slot != -1
					? trigger::to_generic(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot)))
					: -1,
			this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_owner_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot)))
													 : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
			primary_slot != -1
					? trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)))
					: -1,
			this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_controller_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "controller_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot)))
													 : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
			primary_slot != -1 ? trigger::to_generic(ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot)))
												 : -1,
			this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_location_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "location_of_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto loc =
			primary_slot != -1 ? trigger::to_generic(ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot))) : -1;
	display_subtriggers(tval, ws, layout, loc, this_slot, from_slot, indentation + indentation_amount,
			show_condition && primary_slot != -1);
}
void tf_country_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(primary_slot))
													 : text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
			primary_slot != -1
					? trigger::to_generic(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot)))
					: -1,
			this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_country_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "nation_of_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
			primary_slot != -1 ? trigger::to_generic(nations::owner_of_pop(ws, trigger::to_pop(primary_slot))) : -1, this_slot,
			from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_capital_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "capital_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto loc = primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_capital(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)))) : -1;
	display_subtriggers(tval, ws, layout, loc, this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_capital_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "capital_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(nations::owner_of_pop(ws, trigger::to_pop(primary_slot)))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto loc = primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_capital(nations::owner_of_pop(ws, trigger::to_pop(primary_slot)))) : -1;
	display_subtriggers(tval, ws, layout, loc, this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_capital_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "capital_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto loc = primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_capital(trigger::to_nation(primary_slot))) : -1;
	display_subtriggers(tval, ws, layout, loc, this_slot, from_slot, indentation + indentation_amount,
			show_condition && primary_slot != -1);
}
void tf_this_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box,
				this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(this_slot)))
												: text::produce_simple_string(ws, "this_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, this_slot, this_slot, from_slot, indentation + indentation_amount, this_slot != -1);
}
void tf_this_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box,
				this_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(this_slot))
												: text::produce_simple_string(ws, "this_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, this_slot, this_slot, from_slot, indentation + indentation_amount, this_slot != -1);
}
void tf_this_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box,
				this_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(this_slot)))
												: text::produce_simple_string(ws, "this_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, this_slot, this_slot, from_slot, indentation + indentation_amount, this_slot != -1);
}
void tf_this_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, this_slot, this_slot, from_slot, indentation + indentation_amount, this_slot != -1);
}
void tf_from_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box,
				from_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(from_slot)))
												: text::produce_simple_string(ws, "from_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, from_slot, this_slot, from_slot, indentation + indentation_amount, from_slot != -1);
}
void tf_from_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box,
				from_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(from_slot))
												: text::produce_simple_string(ws, "from_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, from_slot, this_slot, from_slot, indentation + indentation_amount, from_slot != -1);
}
void tf_from_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box,
				from_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(from_slot)))
												: text::produce_simple_string(ws, "from_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, from_slot, this_slot, from_slot, indentation + indentation_amount, from_slot != -1);
}
void tf_from_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, from_slot, this_slot, from_slot, indentation + indentation_amount, from_slot != -1);
}
void tf_sea_zone_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "adjacent_sea"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto sea_zone = primary_slot != -1 ? [&ws](int32_t p_slot) {
		auto pid = fatten(ws.world, trigger::to_prov(p_slot));
		for(auto adj : pid.get_province_adjacency()) {
			if(adj.get_connected_provinces(0).id.index() >= ws.province_definitions.first_sea_province.index()) {
				return adj.get_connected_provinces(0).id;
			} else if(adj.get_connected_provinces(1).id.index() >= ws.province_definitions.first_sea_province.index()) {
				return adj.get_connected_provinces(1).id;
			}
		}
		return dcon::province_id{};
	}(primary_slot)
									   : dcon::province_id{};

	if(sea_zone) {
		display_subtriggers(tval, ws, layout, trigger::to_generic(sea_zone), this_slot, from_slot, indentation + indentation_amount,
				show_condition);
	} else {
		display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
	}
}
void tf_cultural_union_scope(TRIGGER_DISPLAY_PARAMS) {
	auto prim_culture =
			primary_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "cultural_union_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
					 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto union_tag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holder = ws.world.national_identity_get_nation_from_identity_holder(union_tag);

	display_subtriggers(tval, ws, layout, trigger::to_generic(group_holder), this_slot, from_slot, indentation + indentation_amount,
			show_condition);
}
void tf_overlord_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "overlord_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto olr = ws.world.nation_get_overlord_as_subject(trigger::to_nation(primary_slot));
	display_subtriggers(tval, ws, layout, trigger::to_generic(ws.world.overlord_get_ruler(olr)), this_slot, from_slot,
			indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_sphere_owner_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "sphere_leader_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
			primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_in_sphere_of((trigger::to_nation(primary_slot)))) : -1,
			this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_independence_scope(TRIGGER_DISPLAY_PARAMS) {
	auto rtag =
			from_slot != -1 ? ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot)) : dcon::national_identity_id{};
	auto r_holder = ws.world.national_identity_get_nation_from_identity_holder(rtag);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "reb_independence_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, r_holder ? trigger::to_generic(r_holder) : -1, this_slot, from_slot,
			indentation + indentation_amount, show_condition && bool(r_holder));
}
void tf_flashpoint_tag_scope(TRIGGER_DISPLAY_PARAMS) {
	auto ctag = ws.world.state_instance_get_flashpoint_tag(trigger::to_state(primary_slot));
	auto fp_nation = ws.world.national_identity_get_nation_from_identity_holder(ctag);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "flashpoint_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, fp_nation ? trigger::to_generic(fp_nation) : -1, this_slot, from_slot,
			indentation + indentation_amount, show_condition && bool(fp_nation));
}
void tf_crisis_state_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "crisis_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto cstate = ws.crisis_state ? trigger::to_generic(ws.crisis_state) : -1;
	display_subtriggers(tval, ws, layout, cstate, this_slot, from_slot, indentation + indentation_amount,
			show_condition && cstate != -1);
}
void tf_state_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "containing_state"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot)))
													 : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto st =
			primary_slot != -1 ? ws.world.province_get_state_membership(trigger::to_prov(primary_slot)) : dcon::state_instance_id{};

	display_subtriggers(tval, ws, layout, st ? trigger::to_generic(st) : -1, this_slot, from_slot, indentation + indentation_amount,
			show_condition && st);
}
void tf_state_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "containing_state"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "singular_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto st = primary_slot != -1 ? ws.world.province_get_state_membership(
																		 ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot)))
															 : dcon::state_instance_id{};

	display_subtriggers(tval, ws, layout, st ? trigger::to_generic(st) : -1, this_slot, from_slot, indentation + indentation_amount,
			show_condition && st);
}
void tf_tag_scope(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[2]).tag_id;
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box,
				text::produce_simple_string(ws,
						tag_holder ? ws.world.nation_get_name(tag_holder) : ws.world.national_identity_get_name(tag)));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, tag_holder ? trigger::to_generic(tag_holder) : -1, this_slot, from_slot,
			indentation + indentation_amount, show_condition && tag_holder);
}
void tf_integer_scope(TRIGGER_DISPLAY_PARAMS) {
	auto p = trigger::payload(tval[2]).prov_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, ws.world.province_get_name(p)));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, trigger::to_generic(p), this_slot, from_slot, indentation + indentation_amount,
			show_condition);
}
void tf_country_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}
	display_subtriggers(tval, ws, layout, primary_slot, this_slot, from_slot, indentation + indentation_amount, show_condition);
}
void tf_country_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot)))
													 : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
			primary_slot != -1
					? trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)))
					: -1,
			this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_cultural_union_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	auto cultures = primary_slot != -1 ? ws.world.pop_get_culture(trigger::to_pop(primary_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(cultures);
	auto union_tags = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holders = ws.world.national_identity_get_nation_from_identity_holder(union_tags);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "cultural_union_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "singular_pop"));
		text::close_layout_box(layout, box);
	}
	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, group_holders ? trigger::to_generic(group_holders) : -1, this_slot, from_slot,
			indentation + indentation_amount, show_condition && bool(group_holders));
}

void tf_year(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "year"), int32_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_month(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "month"), int32_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}

void tf_port(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_port"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rank(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_rank"), int32_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_technology(TRIGGER_DISPLAY_PARAMS) {
	auto tech = trigger::payload(tval[1]).tech_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "technology"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.technology_get_name(tech));
	text::close_layout_box(layout, box);
}
void tf_invention(TRIGGER_DISPLAY_PARAMS) {
	auto tech = trigger::payload(tval[1]).invt_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "invention"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.invention_get_name(tech));
	text::close_layout_box(layout, box);
}
void tf_strata_rich(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "strata"), text::produce_simple_string(ws, "rich"), ws, layout,
			box);
	text::close_layout_box(layout, box);
}
void tf_strata_middle(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "strata"), text::produce_simple_string(ws, "middle"), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_strata_poor(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "strata"), text::produce_simple_string(ws, "poor"), ws, layout,
			box);
	text::close_layout_box(layout, box);
}
void tf_life_rating_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "life_rating"),
			int32_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_life_rating_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "minimum_state_life_rating"),
			int32_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_empty_adjacent_state_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "an_empty_adj_prov"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_empty_adjacent_state_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "an_empty_adj_prov"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_state_id_province(TRIGGER_DISPLAY_PARAMS) {
	auto it = (tval[0] & trigger::association_mask) == trigger::association_eq
								? ws.key_to_text_sequence.find(std::string_view("belongs_to_same_state"))
								: ws.key_to_text_sequence.find(std::string_view("does_not_belong_to_same_state"));
	if(it != ws.key_to_text_sequence.end()) {
		text::substitution_map map;
		text::add_to_substitution_map(map, text::variable_type::text, trigger::payload(tval[1]).prov_id);

		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, it->second, map);
		text::close_layout_box(layout, box);
	}
}
void tf_state_id_state(TRIGGER_DISPLAY_PARAMS) {
	auto it = (tval[0] & trigger::association_mask) == trigger::association_eq
								? ws.key_to_text_sequence.find(std::string_view("contains_blank"))
								: ws.key_to_text_sequence.find(std::string_view("does_not_contain_blank"));
	if(it != ws.key_to_text_sequence.end()) {
		text::substitution_map map;
		text::add_to_substitution_map(map, text::variable_type::text, trigger::payload(tval[1]).prov_id);

		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, it->second, map);
		text::close_layout_box(layout, box);
	}
}
void tf_cash_reserves(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "pop_savings"),
			text::fp_three_places{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "times_target_needs_spending"));
	text::close_layout_box(layout, box);
}
void tf_unemployment_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_slave_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"), text::produce_simple_string(ws, "allowed"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_slave_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_slave_state"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_slave_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_slave_state"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_slave_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_slave"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_independant(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "an_independent_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_national_minority_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "non_prim_culture_pops"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_national_minority_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "non_prim_culture_pops"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_national_minority_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "non_prim_culture_pops"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_government_nation(TRIGGER_DISPLAY_PARAMS) {
	auto gov = trigger::payload(tval[1]).gov_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "government"),
			text::produce_simple_string(ws, ws.world.government_type_get_name(gov)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_government_pop(TRIGGER_DISPLAY_PARAMS) {
	auto gov = trigger::payload(tval[1]).gov_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "government"),
			text::produce_simple_string(ws, ws.world.government_type_get_name(gov)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_capital(TRIGGER_DISPLAY_PARAMS) {
	auto prov = trigger::payload(tval[1]).prov_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "capital"),
			text::produce_simple_string(ws, ws.world.province_get_name(prov)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_tech_school(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "technology_school"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_primary_culture(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"),
			text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_primary_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture_pop"),
			text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}

void tf_primary_culture_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	std::string sub;
	if(from_slot != -1)
		sub = text::produce_simple_string(ws, ws.world.nation_get_primary_culture(trigger::to_nation(from_slot)).get_name());
	else
		sub = text::produce_simple_string(ws, "from_nation");
	display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), sub, ws, layout, box);
	text::close_layout_box(layout, box);
}

void tf_primary_culture_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	std::string sub;
	if(from_slot != -1)
		sub = text::produce_simple_string(ws, ws.world.nation_get_primary_culture(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot))).get_name());
	else
		sub = text::produce_simple_string(ws, "from_nation");
	display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), sub, ws, layout, box);
	text::close_layout_box(layout, box);
}

void tf_accepted_culture(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(t)),
			text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_nation(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(t)),
			text::produce_simple_string(ws, "a_primary_or_accepted"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_pop_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			rc ? text::produce_simple_string(ws, ws.world.culture_get_name(rc)) : text::produce_simple_string(ws, "rebel_culture"), ws,
			layout, box);
}
void tf_culture_state_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			rc ? text::produce_simple_string(ws, ws.world.culture_get_name(rc)) : text::produce_simple_string(ws, "rebel_culture"), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_province_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			rc ? text::produce_simple_string(ws, ws.world.culture_get_name(rc)) : text::produce_simple_string(ws, "rebel_culture"), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_nation_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
			rc ? text::produce_simple_string(ws, ws.world.culture_get_name(rc)) : text::produce_simple_string(ws, "rebel_culture"),
			text::produce_simple_string(ws, "a_primary_or_accepted"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			text::produce_simple_string(ws, "this_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			text::produce_simple_string(ws, "from_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			text::produce_simple_string(ws, "this_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			text::produce_simple_string(ws, "this_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			text::produce_simple_string(ws, "this_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation(TRIGGER_DISPLAY_PARAMS) {
	auto cg = trigger::payload(tval[1]).culgrp_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture_group"),
			text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop(TRIGGER_DISPLAY_PARAMS) {
	auto cg = trigger::payload(tval[1]).culgrp_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_province(TRIGGER_DISPLAY_PARAMS) {
	auto cg = trigger::payload(tval[1]).culgrp_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture_group"),
			text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_state(TRIGGER_DISPLAY_PARAMS) {
	auto cg = trigger::payload(tval[1]).culgrp_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture_group"),
			text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_reb_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};
	auto rg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(rg))
				 : text::produce_simple_string(ws, "rebel_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_reb_pop(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};
	auto rg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(rg))
				 : text::produce_simple_string(ws, "rebel_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "this_nation_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "this_nation_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(from_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "from_nation_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(from_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "from_nation_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(
																	ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)))
														: dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "this_province_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(
																	ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)))
														: dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "this_province_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(
																	ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)))
														: dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "this_state_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(
																	ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)))
														: dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "this_state_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(nations::owner_of_pop(ws, trigger::to_pop(this_slot)))
														: dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "this_pop_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(nations::owner_of_pop(ws, trigger::to_pop(this_slot)))
														: dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"),
			cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
				 : text::produce_simple_string(ws, "this_pop_culture_group"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).rel_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			text::produce_simple_string(ws, ws.world.religion_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_religion(trigger::to_rebel(from_slot)) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			rc ? text::produce_simple_string(ws, ws.world.religion_get_name(rc)) : text::produce_simple_string(ws, "rebel_religion"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.nation_get_religion(trigger::to_nation(from_slot)) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			rc ? text::produce_simple_string(ws, ws.world.religion_get_name(rc))
				 : text::produce_simple_string(ws, "from_nation_religion"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_religion(trigger::to_nation(this_slot)) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			rc ? text::produce_simple_string(ws, ws.world.religion_get_name(rc))
				 : text::produce_simple_string(ws, "this_nation_religion"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_religion(
																	ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)))
														: dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			rc ? text::produce_simple_string(ws, ws.world.religion_get_name(rc))
				 : text::produce_simple_string(ws, "this_state_religion"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1
								? ws.world.nation_get_religion(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)))
								: dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			rc ? text::produce_simple_string(ws, ws.world.religion_get_name(rc))
				 : text::produce_simple_string(ws, "this_province_religion"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto rc =
			this_slot != -1 ? ws.world.nation_get_religion(nations::owner_of_pop(ws, trigger::to_pop(this_slot))) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			rc ? text::produce_simple_string(ws, ws.world.religion_get_name(rc)) : text::produce_simple_string(ws, "this_pop_religion"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_terrain_province(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "province_terrain"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_terrain_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "province_terrain"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_trade_goods(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).com_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rgo_production"),
			text::produce_simple_string(ws, ws.world.commodity_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}

void tf_is_secondary_power_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_secondary_power"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_secondary_power_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_secondary_power"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_faction_nation(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).reb_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.rebel_type_get_name(t)),
			text::produce_simple_string(ws, "an_active_rebel"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_faction_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).reb_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_member_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, ws.world.rebel_type_get_name(t)));
	text::close_layout_box(layout, box);
}
void tf_has_unclaimed_cores(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "unclaimed_cores"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_bool(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_cultural_union"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_self_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_pop"),
			text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_pop"),
			text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_rebel(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, "the_rebel_ind_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"),
			text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_have_core_in_nation_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_core_ob"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)));
	text::close_layout_box(layout, box);
}
void tf_have_core_in_nation_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_core_ob"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(this_slot))) : text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_have_core_in_nation_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_core_ob"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, from_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(from_slot))) : text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}

void tf_can_build_factory_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "capitalists_can_build"
																					: "capitalists_cannot_build"));
	text::close_layout_box(layout, box);
}
void tf_can_build_factory_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "capitalists_can_build"
																					: "capitalists_cannot_build"));
	text::close_layout_box(layout, box);
}
void tf_can_build_factory_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "capitalists_can_build"
																					: "capitalists_cannot_build"));
	text::close_layout_box(layout, box);
}
void tf_war_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_exhaustion_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "war_exhaustion"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_exhaustion_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "war_exhaustion"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_exhaustion_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "war_exhaustion"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_blockade(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "total_blockade"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_owns(TRIGGER_DISPLAY_PARAMS) {
	auto p = trigger::payload(tval[1]).prov_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws,
					(tval[0] & trigger::association_mask) == trigger::association_eq ? "owns" : "does_not_own"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, ws.world.province_get_name(p)));
	text::close_layout_box(layout, box);
}
void tf_controls(TRIGGER_DISPLAY_PARAMS) {
	auto p = trigger::payload(tval[1]).prov_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws,
					(tval[0] & trigger::association_mask) == trigger::association_eq ? "controls" : "does_not_control"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, ws.world.province_get_name(p)));
	text::close_layout_box(layout, box);
}
void tf_is_core_integer(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_core_in"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, ws.world.province_get_name(trigger::payload(tval[1]).prov_id)));
	text::close_layout_box(layout, box);
}
void tf_is_core_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.nation_get_name(trigger::to_nation(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.nation_get_name(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.nation_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.nation_get_name(nations::owner_of_pop(ws, trigger::to_pop(this_slot))));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.nation_get_name(trigger::to_nation(from_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_reb(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1) {
		auto tg = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
		auto h = ws.world.national_identity_get_nation_from_identity_holder(tg);
		text::add_to_layout_box(ws, layout, box, h ? ws.world.nation_get_name(h) : ws.world.national_identity_get_name(tg));
	} else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "the_rebel_ind_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tg = trigger::payload(tval[1]).tag_id;
	auto h = ws.world.national_identity_get_nation_from_identity_holder(tg);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, h ? ws.world.nation_get_name(h) : ws.world.national_identity_get_name(tg));
	text::close_layout_box(layout, box);
}
void tf_is_core_pop_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tg = trigger::payload(tval[1]).tag_id;
	auto h = ws.world.national_identity_get_nation_from_identity_holder(tg);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "located_in_a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, h ? ws.world.nation_get_name(h) : ws.world.national_identity_get_name(tg));
	text::close_layout_box(layout, box);
}
void tf_is_core_boolean(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of_owner"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_revolts(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_reb_control"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_revolt_percentage(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage_reb_control"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_int(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"),
			text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			from_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(from_slot)))
											: text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"),
			text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(this_slot)))
											: text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"),
			text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1
					? text::produce_simple_string(ws,
								ws.world.nation_get_name(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))
					: text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"),
			text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1
					? text::produce_simple_string(ws,
								ws.world.nation_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))
					: text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"),
			text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1
					? text::produce_simple_string(ws, ws.world.nation_get_name(nations::owner_of_pop(ws, trigger::to_pop(this_slot))))
					: text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_ports(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_ports_connected"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_allies(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_allies"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_vassals(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_vassals"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_owned_by_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_owned_by_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_owned_by_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_owned_by_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_owned_by_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_owned_by_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_exists_bool(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws,
					(tval[0] & trigger::association_mask) == trigger::association_eq ? "exists" : "does_not_exist"));
	text::close_layout_box(layout, box);
}
void tf_exists_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws,
					(tval[0] & trigger::association_mask) == trigger::association_eq ? "exists" : "does_not_exist"));
	text::close_layout_box(layout, box);
}
void tf_has_country_flag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
			text::produce_simple_string(ws, ws.national_definitions.flag_variable_names[trigger::payload(tval[1]).natf_id]),
			text::produce_simple_string(ws, "set"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_province(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "continent"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_state(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "continent"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_nation(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "continent"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "continent"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_nation_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_state_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_province_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_pop_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_nation_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_state_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_province_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_pop_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_casus_belli_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_military_access_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_value(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_prestige_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"),
			ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"),
			ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"),
			ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"),
			ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"),
			ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_badboy(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "infamy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_building_fort(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_fort"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_building_railroad(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_railroad"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_building_naval_base(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_naval_base"), ws, layout, box);
	text::close_layout_box(layout, box);
}

void tf_has_building_factory(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_factory"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_building_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0],
			text::produce_simple_string(ws, ws.world.factory_type_get_name(trigger::payload(tval[1]).fac_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_building_state_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0],
			text::produce_simple_string(ws, ws.world.factory_type_get_name(trigger::payload(tval[1]).fac_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_building_factory_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_factory"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_empty(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "empty"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_blockaded(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "blockaded"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_country_modifier(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "national_modifier"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
	text::close_layout_box(layout, box);
}
void tf_has_country_modifier_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "national_modifier"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
	text::close_layout_box(layout, box);
}
void tf_has_province_modifier(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "provincial_modifier"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
	text::close_layout_box(layout, box);
}
void tf_region(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "part_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).state_id);
	text::close_layout_box(layout, box);
}
void tf_region_proper(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "part_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.region_get_name(trigger::payload(tval[1]).reg_id));
	text::close_layout_box(layout, box);
}
void tf_owns_region(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "the_owner_of_region"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).state_id);
	text::close_layout_box(layout, box);
}
void tf_owns_region_proper(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "the_owner_of_region"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.region_get_name(trigger::payload(tval[1]).reg_id));
	text::close_layout_box(layout, box);
}
void tf_region_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "located_in"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).state_id);
	text::close_layout_box(layout, box);
}
void tf_region_proper_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "located_in"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.region_get_name(trigger::payload(tval[1]).reg_id));
	text::close_layout_box(layout, box);
}
void tf_tag_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], trigger::payload(tval[1]).tag_id, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_tag_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], trigger::to_nation(this_slot), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_tag_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)), ws,
				layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_tag_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], trigger::to_nation(from_slot), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "from_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_tag_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)), ws,
				layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "from_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_tag_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owner"), trigger::payload(tval[1]).tag_id, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_neighbour_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_border_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_neighbour_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_border_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_neighbour_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_border_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_neighbour_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_border_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_neighbour_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_border_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}

void tf_country_units_in_state_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_state"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_country_units_in_state_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_state"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_country_units_in_state_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_state"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_country_units_in_state_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_state"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_country_units_in_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_state"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_country_units_in_state_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_state"), ws, layout, box);
	text::close_layout_box(layout, box);
}

void tf_units_in_province_value(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "units_in_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_province"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_the_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_with_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_war_with_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_war_with_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_war_with_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_war_with_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_war_with_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_unit_in_battle(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "an_ongoing_battle"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_total_amount_of_divisions(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_divisions"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_money(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "treasury"),
			text::fp_currency{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_money_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "o_treasury"),
			text::fp_currency{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_lost_national(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage_unowned_cores"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_vassal(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_ruling_party_ideology_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "ruling_party_ideology"),
			text::produce_simple_string(ws, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_ruling_party(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "ruling_party"),
			text::produce_simple_string(ws, ws.world.political_party_get_name(trigger::payload(tval[1]).par_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_ideology_enabled(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id)),
			text::produce_simple_string(ws, "enabled"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_political_reform_want_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "political_reform_desire"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_political_reform_want_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "political_reform_desire"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_social_reform_want_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_reform_desire"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_social_reform_want_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_reform_desire"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_total_amount_of_ships(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_ships"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_plurality(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "plurality"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1) / 100.0f}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_plurality_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "plurality_pop"),
			text::fp_percentage{ trigger::read_float_from_payload(tval + 1) / 100.0f }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_corruption(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "crime_percent"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_state_religion_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "member_of_nat_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_state_religion_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
			text::produce_simple_string(ws, "dominant_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_state_religion_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
			text::produce_simple_string(ws, "dominant_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			text::produce_simple_string(ws, "owner_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, "owner_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, "owner_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"),
			text::produce_simple_string(ws, "this_pop_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_nation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"),
				text::produce_simple_string(ws,
						ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_nation_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"),
				text::produce_simple_string(ws,
						ws.world.culture_get_name(ws.world.nation_get_primary_culture(
								ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_nation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"),
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
																						ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws,
						ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
																						nations::owner_of_pop(ws, trigger::to_pop(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws,
						ws.world.culture_get_name(ws.world.nation_get_primary_culture(
								ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
																						ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws,
						ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
																						nations::owner_of_pop(ws, trigger::to_pop(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws,
						ws.world.culture_get_name(ws.world.nation_get_primary_culture(
								ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
																						ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws,
						ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
																						nations::owner_of_pop(ws, trigger::to_pop(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws,
						ws.world.culture_get_name(ws.world.nation_get_primary_culture(
								ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
					ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}


void tf_is_accepted_culture_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "this_pop_culture"),
			text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_nation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws,
			ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)))), text::produce_simple_string(ws, "an_accepted_culture"),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_nation_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws,
			ws.world.culture_get_name(ws.world.nation_get_primary_culture(
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))), text::produce_simple_string(ws, "an_accepted_culture"),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_nation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
			ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))),
			text::produce_simple_string(ws, "an_accepted_culture"),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_state_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws,
			ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)))),
			text::produce_simple_string(ws, "an_accepted_culture"),
			ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
			nations::owner_of_pop(ws, trigger::to_pop(this_slot))))),
			text::produce_simple_string(ws, "this_nation_primary_culture"),
			ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_state_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws,
			ws.world.culture_get_name(ws.world.nation_get_primary_culture(
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))),
			text::produce_simple_string(ws, "an_accepted_culture"),
			ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_state_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
			ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))),
			text::produce_simple_string(ws, "an_accepted_culture"),
			ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_province_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws,
			ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)))),
			text::produce_simple_string(ws, "an_accepted_culture"),
			ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
			nations::owner_of_pop(ws, trigger::to_pop(this_slot))))),
			text::produce_simple_string(ws, "an_accepted_culture"),
			ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_province_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws,
			ws.world.culture_get_name(ws.world.nation_get_primary_culture(
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))),
			text::produce_simple_string(ws, "an_accepted_culture"),
			ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_province_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.nation_get_primary_culture(
			ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))),
			text::produce_simple_string(ws, "an_accepted_culture"),
			ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation_primary_culture"),
				text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_pop_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	
	display_with_comparison(tval[0], text::produce_simple_string(ws, "this_pop_culture"),
		text::produce_simple_string(ws, "this_nation_accepted_culture"),
		ws, layout, box);
	
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	display_with_comparison(tval[0], text::produce_simple_string(ws, "this_pop_culture"),
		text::produce_simple_string(ws, "this_nation_accepted_culture"),
		ws, layout, box);

	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_pop_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	display_with_comparison(tval[0], text::produce_simple_string(ws, "this_pop_culture"),
		text::produce_simple_string(ws, "this_nation_accepted_culture"),
		ws, layout, box);

	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_pop_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	display_with_comparison(tval[0], text::produce_simple_string(ws, "this_pop_culture"),
		text::produce_simple_string(ws, "this_nation_accepted_culture"),
		ws, layout, box);

	text::close_layout_box(layout, box);
}

void tf_is_accepted_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "an_accepted_culture"), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, "an_accepted"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_coastal_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "coastal"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_coastal_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "coastal"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_in_sphere_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_in_sphere_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_sphere_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_sphere_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_sphere_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_sphere_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_produces_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_producer_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::close_layout_box(layout, box);
}
void tf_produces_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_producer_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::close_layout_box(layout, box);
}
void tf_produces_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_producer_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::close_layout_box(layout, box);
}

void tf_produces_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_producer_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::close_layout_box(layout, box);
}
void tf_average_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_consciousness_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_consciousness_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_consciousness_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_next_reform_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "next_reform"),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_next_reform_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "next_reform"),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_next_rreform_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "next_reform"),
			text::produce_simple_string(ws, ws.world.reform_option_get_name(trigger::payload(tval[1]).ropt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_next_rreform_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "next_reform"),
			text::produce_simple_string(ws, ws.world.reform_option_get_name(trigger::payload(tval[1]).ropt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rebel_power_fraction(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ? "never" : "always"));
	text::close_layout_box(layout, box);
}
void tf_recruited_percentage_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage_recruited"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_recruited_percentage_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage_recruited"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_culture_core(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_cultural_core_prov"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_culture_core_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_cultural_core_prov"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_nationalism(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "nationalism"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_overseas(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "overseas"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_overseas_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "overseas_pop"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_controlled_by_rebels(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by_rebel"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_controlled_by_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_controlled_by_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_owner(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owner"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_reb(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by_rebel"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_canal_enabled(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, std::string("canal_") + std::to_string(tval[1])),
			text::produce_simple_string(ws, "enabled"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_state_capital(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "loc_state_capital"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_state_capital_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "state_capital"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_truce_with_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_truce_with_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_truce_with_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_truce_with_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_truce_with_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_truce_with_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_total_pops_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "population"),
			int64_t(trigger::read_float_from_payload(tval + 1)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_total_pops_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "population"),
			int64_t(trigger::read_float_from_payload(tval + 1)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_total_pops_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "population"),
			int64_t(trigger::read_float_from_payload(tval + 1)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_total_pops_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "population"),
			int64_t(trigger::read_float_from_payload(tval + 1)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_pop_type_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_of_type"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::close_layout_box(layout, box);
}
void tf_has_pop_type_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_of_type"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::close_layout_box(layout, box);
}
void tf_has_pop_type_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_of_type"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::close_layout_box(layout, box);
}
void tf_has_pop_type_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "of_type_plain"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::close_layout_box(layout, box);
}
void tf_has_empty_adjacent_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "an_empty_adj_prov"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_leader(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_leader_named"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.to_string_view(dcon::unit_name_id(dcon::unit_name_id::value_base_t(trigger::read_int32_t_from_payload(tval + 1)))));
	text::close_layout_box(layout, box);
}
void tf_ai(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "ai_controlled"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_can_create_vassals(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "able_to_create_vassals"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_possible_vassal(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], trigger::payload(tval[1]).tag_id, text::produce_simple_string(ws, "a_possible_vassal"), ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_province_id(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.province_get_name(trigger::payload(tval[1]).prov_id)),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_vassal_of_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_vassal_of_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_vassal_of_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_vassal_of_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_vassal_of_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_vassal_of_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}


void tf_vassal_of_province_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_vassal_of_province_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_vassal_of_province_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_vassal_of_province_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_vassal_of_province_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_vassal_of_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_vassal_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}

void tf_substate_of_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_substate_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_substate_of_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_substate_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_substate_of_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_substate_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_substate_of_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_substate_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_substate_of_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_substate_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_substate_of_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_substate_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_alliance_with_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "allied_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_alliance_with_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "allied_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_alliance_with_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "allied_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_alliance_with_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "allied_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_alliance_with_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "allied_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_alliance_with_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "allied_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_has_recently_lost_war(TRIGGER_DISPLAY_PARAMS) {
	if((tval[0] & trigger::association_mask) == trigger::association_eq) {
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "has_recently_lost_war"));
		text::close_layout_box(layout, box);
	} else {
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "has_not_recently_lost_war"));
		text::close_layout_box(layout, box);
	}
}
void tf_is_mobilised(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "mobilized"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_mobilisation_size(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "mobilization_size"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_crime_higher_than_education_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "admin_spending"),
			text::produce_simple_string(ws, "greater_than_edu_spending"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_crime_higher_than_education_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "admin_spending"),
			text::produce_simple_string(ws, "greater_than_edu_spending"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_crime_higher_than_education_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "admin_spending"),
			text::produce_simple_string(ws, "greater_than_edu_spending"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_crime_higher_than_education_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "admin_spending"),
			text::produce_simple_string(ws, "greater_than_edu_spending"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_agree_with_ruling_party(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support_for_rp"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_colonial_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "colonial"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_colonial_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "colonial"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_colonial_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "colonial_pop"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_factories_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_factory"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_factories_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_factory"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_in_default_bool(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_bnkrpt"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_in_default_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_default_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_in_default_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_default_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_default_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_default_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_default_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_default_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_default_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_default_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_default_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_default_to"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_total_num_of_ports(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_ports"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_always(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));

	text::close_layout_box(layout, box);
}
void tf_election(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "holding_an_election"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_global_flag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
			text::produce_simple_string(ws, ws.national_definitions.global_flag_variable_names[trigger::payload(tval[1]).glob_id]),
			text::produce_simple_string(ws, "set_globally"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_capital(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "capital"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_nationalvalue_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_value"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_nationalvalue_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_value"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_nationalvalue_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_value"),
			text::produce_simple_string(ws, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_industrial_score_value(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "industrial_score"),
			int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_industrial_score_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "industrial_score"),
			text::produce_simple_string(ws, "industrial_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_industrial_score_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "industrial_score"),
			text::produce_simple_string(ws, "industrial_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_industrial_score_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "industrial_score"),
			text::produce_simple_string(ws, "industrial_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_industrial_score_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "industrial_score"),
			text::produce_simple_string(ws, "industrial_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_industrial_score_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "industrial_score"),
			text::produce_simple_string(ws, "industrial_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_industrial_score_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "industrial_score"),
			text::produce_simple_string(ws, "industrial_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_score_value(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "alice_military_score"),
			int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_military_score_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "alice_military_score"),
			text::produce_simple_string(ws, "military_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_military_score_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "alice_military_score"),
			text::produce_simple_string(ws, "military_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_score_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "alice_military_score"),
			text::produce_simple_string(ws, "military_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_score_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "alice_military_score"),
			text::produce_simple_string(ws, "military_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_score_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "alice_military_score"),
			text::produce_simple_string(ws, "military_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_score_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "alice_military_score"),
			text::produce_simple_string(ws, "military_score_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_civilized_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "civilized"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_civilized_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "civilized"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_civilized_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "civilized"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_national_provinces_occupied(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percent_occupied"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_greater_power_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "great_power"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_greater_power_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "great_power"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_greater_power_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "great_power"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_tax(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_tax"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_tax(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_tax"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_tax(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_tax"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_social_spending_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_social_spending_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_social_spending_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_military_spending_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_military_spending_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "mil_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_military_spending_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "mil_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_military_spending_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "mil_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_administration_spending_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "admin_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_administration_spending_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "admin_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_administration_spending_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "admin_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_administration_spending_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "admin_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_education_spending_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "edu_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_education_spending_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "edu_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_education_spending_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "edu_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_education_spending_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "edu_spending"),
			text::int_percentage{trigger::payload(tval[1]).signed_value}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_colonial_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "colonies"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_religion_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_religion"),
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_religion_nation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_religion"),
			text::produce_simple_string(ws, ws.world.religion_get_name(ws.world.nation_get_religion(trigger::to_nation(this_slot)))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_religion"), text::produce_simple_string(ws, "same_rel_as_this"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_religion_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_religion"),
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_religion_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_religion"),
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_culture_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"),
			text::produce_simple_string(ws, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_issue_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_issue"),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_issue_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_issue"),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_issue_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_issue"),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_issue_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_issue"),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_ideology_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_ideology"),
			text::produce_simple_string(ws, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_ideology_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_ideology"),
			text::produce_simple_string(ws, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_ideology_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_ideology"),
			text::produce_simple_string(ws, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_majority_ideology_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_ideology"),
			text::produce_simple_string(ws, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_militancy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_militancy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_militancy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_tax_above_poor(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_tax"), text::produce_simple_string(ws, "gt_poor_tax"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_has_union_tag_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_cultural_union"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_has_union_tag_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_cultural_union"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"), trigger::payload(tval[1]).tag_id, ws,
			layout, box);
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"), trigger::to_nation(from_slot), ws, layout,
				box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"),
				text::produce_simple_string(ws, "from_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"), trigger::to_nation(this_slot), ws, layout,
				box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"),
				text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"),
				ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"),
				text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"),
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"),
				text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"),
				nations::owner_of_pop(ws, trigger::to_pop(this_slot)), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "cultural_union"),
				text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_this_union_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_cultural_union"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_this_union_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_cultural_union"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_this_union_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_cultural_union"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_this_culture_union_this_union_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_cultural_union"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_minorities_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "non_accepted_pops"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_minorities_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "non_accepted_pops"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_minorities_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "non_accepted_pops"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_revanchism_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "revanchism"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_revanchism_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "revanchism"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_crime(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "crime"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_substates(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_substates"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_vassals_no_substates(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_vassals_no_substates"), int64_t(tval[1]), ws, layout,
			box);
	text::close_layout_box(layout, box);
}
void tf_stronger_army_than_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
		text::produce_simple_string(ws, "num_regiments"),
		text::produce_simple_string(ws, "num_regiments_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_stronger_army_than_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
		text::produce_simple_string(ws, "num_regiments"),
		text::produce_simple_string(ws, "num_regiments_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_stronger_army_than_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
		text::produce_simple_string(ws, "num_regiments"),
		text::produce_simple_string(ws, "num_regiments_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_stronger_army_than_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
		text::produce_simple_string(ws, "num_regiments"),
		text::produce_simple_string(ws, "num_regiments_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_stronger_army_than_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
		text::produce_simple_string(ws, "num_regiments"),
		text::produce_simple_string(ws, "num_regiments_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_stronger_army_than_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
		text::produce_simple_string(ws, "num_regiments"),
		text::produce_simple_string(ws, "num_regiments_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_stronger_army_than_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
		text::produce_simple_string(ws, "num_regiments"),
		text::produce_simple_string(ws, "num_regiments_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_brigades_compare_this(TRIGGER_DISPLAY_PARAMS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	text::substitution_map s;
	text::add_to_substitution_map(s, text::variable_type::x, text::fp_one_place{ factor });
	auto factor_str = text::resolve_string_substitution(ws, "times_num_regiments_of", s);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_regiments"),
			factor_str, ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_brigades_compare_from(TRIGGER_DISPLAY_PARAMS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	text::substitution_map s;
	text::add_to_substitution_map(s, text::variable_type::x, text::fp_one_place{ factor });
	auto factor_str = text::resolve_string_substitution(ws, "times_num_regiments_of", s);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_regiments"),
			factor_str, ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_brigades_compare_province_this(TRIGGER_DISPLAY_PARAMS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	text::substitution_map s;
	text::add_to_substitution_map(s, text::variable_type::x, text::fp_one_place{ factor });
	auto factor_str = text::resolve_string_substitution(ws, "times_num_regiments_of", s);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owner_num_regiments"),
			factor_str, ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_brigades_compare_province_from(TRIGGER_DISPLAY_PARAMS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	text::substitution_map s;
	text::add_to_substitution_map(s, text::variable_type::x, text::fp_one_place{ factor });
	auto factor_str = text::resolve_string_substitution(ws, "times_num_regiments_of", s);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owner_num_regiments"),
			factor_str, ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "fabricating_on"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "fabricating_on"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "fabricating_on"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "fabricating_on"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "fabricating_on"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "fabricating_on"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_discovered(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "cb_discovered"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "cb_not_discovered"));
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_progress(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "cb_progress"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_civilization_progress(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "civ_progress"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_type(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "constructing_a"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "casus_belli"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], trigger::payload(tval[1]).tag_id, ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		display_with_has_comparison(tval[0], trigger::to_nation(from_slot), ws, layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "from_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_has_comparison(tval[0], trigger::to_nation(this_slot), ws, layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_has_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)), ws,
				layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_has_comparison(tval[0], ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)),
				ws, layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_has_comparison(tval[0], nations::owner_of_pop(ws, trigger::to_pop(this_slot)), ws, layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}


void tf_is_our_vassal_province_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_has_comparison(tval[0], trigger::payload(tval[1]).tag_id, ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_province_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		display_with_has_comparison(tval[0], trigger::to_nation(from_slot), ws, layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "from_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_province_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_has_comparison(tval[0], trigger::to_nation(this_slot), ws, layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_province_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_has_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)), ws,
				layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_province_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_has_comparison(tval[0], ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)),
				ws, layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}
void tf_is_our_vassal_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "o_of_prov"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_has_comparison(tval[0], nations::owner_of_pop(ws, trigger::to_pop(this_slot)), ws, layout, box);
	else
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "as_a_vassal"));
	text::close_layout_box(layout, box);
}

void tf_is_substate(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_substate"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_great_wars_enabled(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "gw_enabled"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "gw_not_enabled"));
	text::close_layout_box(layout, box);
}
void tf_can_nationalize(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "can_perform_nationalization"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "cannot_perform_nationalization"));
	text::close_layout_box(layout, box);
}
void tf_part_of_sphere(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_sphere_leader_of_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "sphere_leader_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_is_sphere_leader_of_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "sphere_leader_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_sphere_leader_of_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "sphere_leader_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_sphere_leader_of_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "sphere_leader_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_sphere_leader_of_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "sphere_leader_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box,
				ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_sphere_leader_of_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "sphere_leader_of"), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_number_of_states(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_states"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_score(TRIGGER_DISPLAY_PARAMS) {
	// stub for apparently unused trigger
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_is_releasable_vassal_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], trigger::to_nation(from_slot), text::produce_simple_string(ws, "a_releasable_vassal"), ws,
				layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "from_nation"),
				text::produce_simple_string(ws, "a_releasable_vassal"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_releasable_vassal_other(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_releasable_vassal"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_recent_imigration(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "recent_immigration"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_province_control_days(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_control_days"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_disarmed(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "disarmed"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_disarmed_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "disarmed_pop"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_big_producer(TRIGGER_DISPLAY_PARAMS) {
	// stub: used only rarely in ai chances and would be expensive to test
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	text::close_layout_box(layout, box);
}
void tf_someone_can_form_union_tag_from(TRIGGER_DISPLAY_PARAMS) {
	// stub: apparently unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	text::close_layout_box(layout, box);
}
void tf_someone_can_form_union_tag_other(TRIGGER_DISPLAY_PARAMS) {
	// stub: apparently unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	text::close_layout_box(layout, box);
}
void tf_social_movement_strength(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_mov_support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_political_movement_strength(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "political_mov_support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_can_build_factory_in_capital_state(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_social_movement(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_member_of_social_mov"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_political_movement(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_member_of_political_mov"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_political_movement_from_reb(TRIGGER_DISPLAY_PARAMS) {
	// stub: implementation unclear
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	text::close_layout_box(layout, box);
}
void tf_social_movement_from_reb(TRIGGER_DISPLAY_PARAMS) {
	// stub: implementation unclear
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	text::close_layout_box(layout, box);
}
void tf_has_cultural_sphere(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "cultural_sphere_members"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_world_wars_enabled(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "ww_enabled"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "ww_not_enabled"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_culture_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.pop_get_culture(trigger::to_pop(this_slot)))), ws,
				layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
				text::produce_simple_string(ws, "this_pop_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_pop_culture_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1) {
		display_with_has_comparison(tval[0],
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.pop_get_culture(trigger::to_pop(this_slot)))), ws,
				layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	} else {
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_with"), ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_culture"));
	}
	text::close_layout_box(layout, box);
}
void tf_has_pop_culture_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1) {
		display_with_has_comparison(tval[0],
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.pop_get_culture(trigger::to_pop(this_slot)))), ws,
				layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	} else {
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_with"), ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_culture"));
	}
	text::close_layout_box(layout, box);
}
void tf_has_pop_culture_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1) {
		display_with_has_comparison(tval[0],
				text::produce_simple_string(ws, ws.world.culture_get_name(ws.world.pop_get_culture(trigger::to_pop(this_slot)))), ws,
				layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	} else {
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_with"), ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_culture"));
	}
	text::close_layout_box(layout, box);
}
void tf_has_pop_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"),
			text::produce_simple_string(ws, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_pop_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0],
			text::produce_simple_string(ws, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id)), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0],
			text::produce_simple_string(ws, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id)), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_culture_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0],
			text::produce_simple_string(ws, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id)), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1) {
		display_with_has_comparison(tval[0],
				text::produce_simple_string(ws, ws.world.religion_get_name(ws.world.pop_get_religion(trigger::to_pop(this_slot)))), ws,
				layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	} else {
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_with"), ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_religion"));
	}
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1) {
		display_with_has_comparison(tval[0],
				text::produce_simple_string(ws, ws.world.religion_get_name(ws.world.pop_get_religion(trigger::to_pop(this_slot)))), ws,
				layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	} else {
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_with"), ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_religion"));
	}
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1) {
		display_with_has_comparison(tval[0],
				text::produce_simple_string(ws, ws.world.religion_get_name(ws.world.pop_get_religion(trigger::to_pop(this_slot)))), ws,
				layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	} else {
		display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_with"), ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_religion"));
	}
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"),
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0],
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0],
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0],
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_life_needs(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_everyday_needs(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_luxury_needs(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_consciousness_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_consciousness_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_consciousness_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_consciousness_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_literacy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "literacy"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_literacy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "literacy"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_literacy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "literacy"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_literacy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "literacy"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_militancy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_trade_goods_in_state_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id)),
			text::produce_simple_string(ws, "present_in_state"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_trade_goods_in_state_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id)),
			text::produce_simple_string(ws, "present_in_state"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_flashpoint(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "flashpoint"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_flashpoint_tension(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "flashpoint_tension"),
			text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_crisis_exist(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "there"), text::produce_simple_string(ws, "an_ongoing_crisis"),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_liberation_crisis(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "current_crisis"),
			text::produce_simple_string(ws, "liberation_crisis"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_claim_crisis(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "current_crisis"),
			text::produce_simple_string(ws, "claim_crisis"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_crisis_temperature(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "crisis_temperature_plain"),
			int64_t(trigger::read_float_from_payload(tval + 1)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_involved_in_crisis_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "involved_in_crisis"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_involved_in_crisis_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "involved_in_crisis"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_life_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_life_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_life_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_life_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_everyday_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_everyday_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_everyday_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_everyday_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_luxury_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_luxury_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_luxury_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_luxury_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_life_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_life_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_life_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_life_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_everyday_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_everyday_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_everyday_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_everyday_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_luxury_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_luxury_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_luxury_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_luxury_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_life_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_life_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_life_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_life_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_life_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_everyday_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_everyday_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_everyday_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_everyday_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_everyday_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_luxury_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_luxury_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_luxury_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_luxury_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_luxury_needs_satisfaction"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_diplomatic_influence_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "dip_influence_over"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], trigger::payload(tval[2]).tag_id, int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_diplomatic_influence_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "dip_influence_over"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], trigger::to_nation(this_slot), int64_t(tval[1]), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_diplomatic_influence_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "dip_influence_over"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)),
				int64_t(tval[1]), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_diplomatic_influence_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "dip_influence_over"));
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], trigger::to_nation(from_slot), int64_t(tval[1]), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "from_nation"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_diplomatic_influence_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "dip_influence_over"));
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)),
				int64_t(tval[1]), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "from_nation"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_unemployment_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[3]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_unemployment_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[3]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_unemployment_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[3]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_unemployment_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_unemployment_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(ws.world.pop_get_poptype(trigger::to_pop(this_slot))));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_type"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_unemployment_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(ws.world.pop_get_poptype(trigger::to_pop(this_slot))));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_type"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_unemployment_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(ws.world.pop_get_poptype(trigger::to_pop(this_slot))));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop_type"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_relation_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "relationship_with"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], trigger::payload(tval[2]).tag_id, int64_t(trigger::payload(tval[1]).signed_value), ws, layout,
			box);
	text::close_layout_box(layout, box);
}
void tf_relation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "relationship_with"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], trigger::to_nation(this_slot), int64_t(trigger::payload(tval[1]).signed_value), ws, layout,
				box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation"),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_relation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "relationship_with"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation"),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_relation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "relationship_with"));
	text::add_space_to_layout_box(ws, layout, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], nations::owner_of_pop(ws, trigger::to_pop(this_slot)),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "this_nation"),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_relation_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "relationship_with"));
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], trigger::to_nation(from_slot), int64_t(trigger::payload(tval[1]).signed_value), ws, layout,
				box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "from_nation"),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_relation_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "relationship_with"));
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "from_nation"),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_check_variable(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],
			text::produce_simple_string(ws, ws.national_definitions.variable_names[trigger::payload(tval[3]).natv_id]),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_upper_house(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, ws.world.ideology_get_name(trigger::payload(tval[3]).ideo_id)));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support_in_uh"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_by_type_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[3]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_by_type_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[3]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_by_type_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[3]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_by_type_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[3]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment_in_state"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_nation_province_id(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[3]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "support_in"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], trigger::payload(tval[1]).prov_id, int64_t(trigger::payload(tval[2]).signed_value), ws, layout,
			box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_from_nation_province_id(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[3]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "support_in"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], trigger::payload(tval[1]).prov_id, int64_t(trigger::payload(tval[2]).signed_value), ws, layout,
			box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_province_province_id(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[3]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "support_in"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], trigger::payload(tval[1]).prov_id, int64_t(trigger::payload(tval[2]).signed_value), ws, layout,
			box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_from_province_province_id(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[3]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "support_in"));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], trigger::payload(tval[1]).prov_id, int64_t(trigger::payload(tval[2]).signed_value), ws, layout,
			box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_nation_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[2]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "support_in"));
	text::add_space_to_layout_box(ws, layout, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], trigger::to_prov(from_slot), int64_t(trigger::payload(tval[1]).signed_value), ws, layout,
				box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "from_province"),
				int64_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_generic(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[2]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"), int64_t(trigger::payload(tval[1]).signed_value),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_from_nation_scope_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[2]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"), int64_t(trigger::payload(tval[1]).signed_value),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_from_province_scope_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);

	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[2]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"), int64_t(trigger::payload(tval[1]).signed_value),
			ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_railroad_no_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "can_build_railroad_here"
																					: "cannot_build_railroad_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_railroad_yes_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "can_build_railroad_here"
																					: "cannot_build_railroad_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_railroad_no_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "can_build_railroad_here"
																					: "cannot_build_railroad_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_railroad_yes_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "can_build_railroad_here"
																					: "cannot_build_railroad_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_fort_no_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws,
					(tval[0] & trigger::association_mask) == trigger::association_eq ? "can_build_fort_here" : "cannot_build_fort_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_fort_yes_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws,
					(tval[0] & trigger::association_mask) == trigger::association_eq ? "can_build_fort_here" : "cannot_build_fort_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_fort_no_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws,
					(tval[0] & trigger::association_mask) == trigger::association_eq ? "can_build_fort_here" : "cannot_build_fort_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_fort_yes_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws,
					(tval[0] & trigger::association_mask) == trigger::association_eq ? "can_build_fort_here" : "cannot_build_fort_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_naval_base_no_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "can_build_naval_base_here"
																					: "cannot_build_naval_base_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_naval_base_yes_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "can_build_naval_base_here"
																					: "cannot_build_naval_base_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_naval_base_no_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "can_build_naval_base_here"
																					: "cannot_build_naval_base_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_in_province_naval_base_yes_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(ws, layout, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box,
			text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq
																					? "can_build_naval_base_here"
																					: "cannot_build_naval_base_here"));
	text::close_layout_box(layout, box);
}
void tf_can_build_railway_in_capital_yes_whole_state_yes_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_can_build_railway_in_capital_yes_whole_state_no_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_can_build_railway_in_capital_no_whole_state_yes_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_can_build_railway_in_capital_no_whole_state_no_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_can_build_fort_in_capital_yes_whole_state_yes_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_can_build_fort_in_capital_yes_whole_state_no_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_can_build_fort_in_capital_no_whole_state_yes_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_can_build_fort_in_capital_no_whole_state_no_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if((tval[0] & trigger::association_mask) == trigger::association_eq)
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "always"));
	else
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "never"));
	text::close_layout_box(layout, box);
}
void tf_work_available_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "work"), text::produce_simple_string(ws, "available_for"), ws,
			layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_work_available_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "work"), text::produce_simple_string(ws, "available_for"), ws,
			layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_work_available_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "work"), text::produce_simple_string(ws, "available_for"), ws,
			layout, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_ideology_name_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_ideology_name_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_ideology_name_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_ideology_name_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_issue_name_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_issue_name_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_issue_name_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_issue_name_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_issue_group_name_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.issue_get_name(trigger::payload(tval[1]).iss_id)),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[2]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_issue_group_name_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.issue_get_name(trigger::payload(tval[1]).iss_id)),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[2]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_issue_group_name_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.issue_get_name(trigger::payload(tval[1]).iss_id)),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[2]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_issue_group_name_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.issue_get_name(trigger::payload(tval[1]).iss_id)),
			text::produce_simple_string(ws, ws.world.issue_option_get_name(trigger::payload(tval[2]).opt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_reform_group_name_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.reform_get_name(trigger::payload(tval[1]).ref_id)),
			text::produce_simple_string(ws, ws.world.reform_option_get_name(trigger::payload(tval[2]).ropt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_reform_group_name_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.reform_get_name(trigger::payload(tval[1]).ref_id)),
			text::produce_simple_string(ws, ws.world.reform_option_get_name(trigger::payload(tval[2]).ropt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_reform_group_name_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.reform_get_name(trigger::payload(tval[1]).ref_id)),
			text::produce_simple_string(ws, ws.world.reform_option_get_name(trigger::payload(tval[2]).ropt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_reform_group_name_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.reform_get_name(trigger::payload(tval[1]).ref_id)),
			text::produce_simple_string(ws, ws.world.reform_option_get_name(trigger::payload(tval[2]).ropt_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_pop_type_name_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage"),
			text::fp_percentage_one_place{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_pop_type_name_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage"),
			text::fp_percentage_one_place{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_pop_type_name_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage"),
			text::fp_percentage_one_place{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_pop_type_name_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "province_percentage"),
			text::fp_percentage_one_place{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_variable_good_name(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(ws, layout, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::add_space_to_layout_box(ws, layout, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "stockpile"),
			text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
			text::produce_simple_string(ws, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_nation_reb(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws,
						ws.world.religion_get_name(ws.world.rebel_faction_get_religion(trigger::to_rebel(from_slot)))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, "rebel_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_nation_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, ws.world.religion_get_name(ws.world.nation_get_religion(trigger::to_nation(from_slot)))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, "from_nat_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_nation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, ws.world.religion_get_name(ws.world.nation_get_religion(trigger::to_nation(this_slot)))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, "this_nat_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_nation_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws,
						ws.world.religion_get_name(ws.world.nation_get_religion(
								ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, "this_nat_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_nation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, ws.world.religion_get_name(ws.world.nation_get_religion(
																						ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, "this_nat_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws,
						ws.world.religion_get_name(ws.world.nation_get_religion(nations::owner_of_pop(ws, trigger::to_pop(this_slot))))),
				ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"),
				text::produce_simple_string(ws, "this_nat_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}

constexpr inline void (*trigger_functions[])(TRIGGER_DISPLAY_PARAMS) = {
		tf_none,
		tf_year,																			// constexpr inline uint16_t year = 0x0001;
		tf_month,																			// constexpr inline uint16_t month = 0x0002;
		tf_port,																			// constexpr inline uint16_t port = 0x0003;
		tf_rank,																			// constexpr inline uint16_t rank = 0x0004;
		tf_technology,																// constexpr inline uint16_t technology = 0x0005;
		tf_strata_rich,																// constexpr inline uint16_t strata_rich = 0x0006;
		tf_life_rating_province,											// constexpr inline uint16_t life_rating_province = 0x0007;
		tf_life_rating_state,													// constexpr inline uint16_t life_rating_state = 0x0008;
		tf_has_empty_adjacent_state_province,					// constexpr inline uint16_t has_empty_adjacent_state_province = 0x0009;
		tf_has_empty_adjacent_state_state,						// constexpr inline uint16_t has_empty_adjacent_state_state = 0x000A;
		tf_state_id_province,													// constexpr inline uint16_t state_id_province = 0x000B;
		tf_state_id_state,														// constexpr inline uint16_t state_id_state = 0x000C;
		tf_cash_reserves,															// constexpr inline uint16_t cash_reserves = 0x000D;
		tf_unemployment_nation,												// constexpr inline uint16_t unemployment_nation = 0x000E;
		tf_unemployment_state,												// constexpr inline uint16_t unemployment_state = 0x000F;
		tf_unemployment_province,											// constexpr inline uint16_t unemployment_province = 0x0010;
		tf_unemployment_pop,													// constexpr inline uint16_t unemployment_pop = 0x0011;
		tf_is_slave_nation,														// constexpr inline uint16_t is_slave_nation = 0x0012;
		tf_is_slave_state,														// constexpr inline uint16_t is_slave_state = 0x0013;
		tf_is_slave_province,													// constexpr inline uint16_t is_slave_province = 0x0014;
		tf_is_slave_pop,															// constexpr inline uint16_t is_slave_pop = 0x0015;
		tf_is_independant,														// constexpr inline uint16_t is_independant = 0x0016;
		tf_has_national_minority_province,						// constexpr inline uint16_t has_national_minority_province = 0x0017;
		tf_has_national_minority_state,								// constexpr inline uint16_t has_national_minority_state = 0x0018;
		tf_has_national_minority_nation,							// constexpr inline uint16_t has_national_minority_nation = 0x0019;
		tf_government_nation,													// constexpr inline uint16_t government_nation = 0x001A;
		tf_government_pop,														// constexpr inline uint16_t government_pop = 0x001B;
		tf_capital,																		// constexpr inline uint16_t capital = 0x001C;
		tf_tech_school,																// constexpr inline uint16_t tech_school = 0x001D;
		tf_primary_culture,														// constexpr inline uint16_t primary_culture = 0x001E;
		tf_accepted_culture,													// constexpr inline uint16_t accepted_culture = 0x001F;
		tf_culture_pop,																// constexpr inline uint16_t culture_pop = 0x0020;
		tf_culture_state,															// constexpr inline uint16_t culture_state = 0x0021;
		tf_culture_province,													// constexpr inline uint16_t culture_province = 0x0022;
		tf_culture_nation,														// constexpr inline uint16_t culture_nation = 0x0023;
		tf_culture_pop_reb,														// constexpr inline uint16_t culture_pop_reb = 0x0024;
		tf_culture_state_reb,													// constexpr inline uint16_t culture_state_reb = 0x0025;
		tf_culture_province_reb,											// constexpr inline uint16_t culture_province_reb = 0x0026;
		tf_culture_nation_reb,												// constexpr inline uint16_t culture_nation_reb = 0x0027;
		tf_culture_from_nation,												// constexpr inline uint16_t culture_from_nation = 0x0028;
		tf_culture_this_nation,												// constexpr inline uint16_t culture_this_nation = 0x0029;
		tf_culture_this_state,												// constexpr inline uint16_t culture_this_state = 0x002A;
		tf_culture_this_pop,													// constexpr inline uint16_t culture_this_pop = 0x002B;
		tf_culture_this_province,											// constexpr inline uint16_t culture_this_province = 0x002C;
		tf_culture_group_nation,											// constexpr inline uint16_t culture_group_nation = 0x002D;
		tf_culture_group_pop,													// constexpr inline uint16_t culture_group_pop = 0x002E;
		tf_culture_group_reb_nation,									// constexpr inline uint16_t culture_group_reb_nation = 0x002F;
		tf_culture_group_reb_pop,											// constexpr inline uint16_t culture_group_reb_pop = 0x0030;
		tf_culture_group_nation_from_nation,					// constexpr inline uint16_t culture_group_nation_from_nation = 0x0031;
		tf_culture_group_pop_from_nation,							// constexpr inline uint16_t culture_group_pop_from_nation = 0x0032;
		tf_culture_group_nation_this_nation,					// constexpr inline uint16_t culture_group_nation_this_nation = 0x0033;
		tf_culture_group_pop_this_nation,							// constexpr inline uint16_t culture_group_pop_this_nation = 0x0034;
		tf_culture_group_nation_this_province,				// constexpr inline uint16_t culture_group_nation_this_province = 0x0035;
		tf_culture_group_pop_this_province,						// constexpr inline uint16_t culture_group_pop_this_province = 0x0036;
		tf_culture_group_nation_this_state,						// constexpr inline uint16_t culture_group_nation_this_state = 0x0037;
		tf_culture_group_pop_this_state,							// constexpr inline uint16_t culture_group_pop_this_state = 0x0038;
		tf_culture_group_nation_this_pop,							// constexpr inline uint16_t culture_group_nation_this_pop = 0x0039;
		tf_culture_group_pop_this_pop,								// constexpr inline uint16_t culture_group_pop_this_pop = 0x003A;
		tf_religion,																	// constexpr inline uint16_t religion = 0x003B;
		tf_religion_reb,															// constexpr inline uint16_t religion_reb = 0x003C;
		tf_religion_from_nation,											// constexpr inline uint16_t religion_from_nation = 0x003D;
		tf_religion_this_nation,											// constexpr inline uint16_t religion_this_nation = 0x003E;
		tf_religion_this_state,												// constexpr inline uint16_t religion_this_state = 0x003F;
		tf_religion_this_province,										// constexpr inline uint16_t religion_this_province = 0x0040;
		tf_religion_this_pop,													// constexpr inline uint16_t religion_this_pop = 0x0041;
		tf_terrain_province,													// constexpr inline uint16_t terrain_province = 0x0042;
		tf_terrain_pop,																// constexpr inline uint16_t terrain_pop = 0x0043;
		tf_trade_goods,																// constexpr inline uint16_t trade_goods = 0x0044;
		tf_is_secondary_power_pop,										// constexpr inline uint16_t is_secondary_power_pop = 0x0045;
		tf_is_secondary_power_nation,									// constexpr inline uint16_t is_secondary_power_nation = 0x0046;
		tf_has_faction_nation,												// constexpr inline uint16_t has_faction_nation = 0x0047;
		tf_has_faction_pop,														// constexpr inline uint16_t has_faction_pop = 0x0048;
		tf_has_unclaimed_cores,												// constexpr inline uint16_t has_unclaimed_cores = 0x0049;
		tf_is_cultural_union_bool,										// constexpr inline uint16_t is_cultural_union_bool = 0x004A;
		tf_is_cultural_union_this_self_pop,						// constexpr inline uint16_t is_cultural_union_this_self_pop = 0x004B;
		tf_is_cultural_union_this_pop,								// constexpr inline uint16_t is_cultural_union_this_pop = 0x004C;
		tf_is_cultural_union_this_state,							// constexpr inline uint16_t is_cultural_union_this_state = 0x004D;
		tf_is_cultural_union_this_province,						// constexpr inline uint16_t is_cultural_union_this_province = 0x004E;
		tf_is_cultural_union_this_nation,							// constexpr inline uint16_t is_cultural_union_this_nation = 0x004F;
		tf_is_cultural_union_this_rebel,							// constexpr inline uint16_t is_cultural_union_this_rebel = 0x0050;
		tf_is_cultural_union_tag_nation,							// constexpr inline uint16_t is_cultural_union_tag_nation = 0x0051;
		tf_is_cultural_union_tag_this_pop,						// constexpr inline uint16_t is_cultural_union_tag_this_pop = 0x0052;
		tf_is_cultural_union_tag_this_state,					// constexpr inline uint16_t is_cultural_union_tag_this_state = 0x0053;
		tf_is_cultural_union_tag_this_province,				// constexpr inline uint16_t is_cultural_union_tag_this_province = 0x0054;
		tf_is_cultural_union_tag_this_nation,					// constexpr inline uint16_t is_cultural_union_tag_this_nation = 0x0055;
		tf_can_build_factory_pop,											// constexpr inline uint16_t can_build_factory_pop = 0x0056;
		tf_war_pop,																		// constexpr inline uint16_t war_pop = 0x0057;
		tf_war_nation,																// constexpr inline uint16_t war_nation = 0x0058;
		tf_war_exhaustion_nation,											// constexpr inline uint16_t war_exhaustion_nation = 0x0059;
		tf_blockade,																	// constexpr inline uint16_t blockade = 0x005A;
		tf_owns,																			// constexpr inline uint16_t owns = 0x005B;
		tf_controls,																	// constexpr inline uint16_t controls = 0x005C;
		tf_is_core_integer,														// constexpr inline uint16_t is_core_integer = 0x005D;
		tf_is_core_this_nation,												// constexpr inline uint16_t is_core_this_nation = 0x005E;
		tf_is_core_this_state,												// constexpr inline uint16_t is_core_this_state = 0x005F;
		tf_is_core_this_province,											// constexpr inline uint16_t is_core_this_province = 0x0060;
		tf_is_core_this_pop,													// constexpr inline uint16_t is_core_this_pop = 0x0061;
		tf_is_core_from_nation,												// constexpr inline uint16_t is_core_from_nation = 0x0062;
		tf_is_core_reb,																// constexpr inline uint16_t is_core_reb = 0x0063;
		tf_is_core_tag,																// constexpr inline uint16_t is_core_tag = 0x0064;
		tf_num_of_revolts,														// constexpr inline uint16_t num_of_revolts = 0x0065;
		tf_revolt_percentage,													// constexpr inline uint16_t revolt_percentage = 0x0066;
		tf_num_of_cities_int,													// constexpr inline uint16_t num_of_cities_int = 0x0067;
		tf_num_of_cities_from_nation,									// constexpr inline uint16_t num_of_cities_from_nation = 0x0068;
		tf_num_of_cities_this_nation,									// constexpr inline uint16_t num_of_cities_this_nation = 0x0069;
		tf_num_of_cities_this_state,									// constexpr inline uint16_t num_of_cities_this_state = 0x006A;
		tf_num_of_cities_this_province,								// constexpr inline uint16_t num_of_cities_this_province = 0x006B;
		tf_num_of_cities_this_pop,										// constexpr inline uint16_t num_of_cities_this_pop = 0x006C;
		tf_num_of_ports,															// constexpr inline uint16_t num_of_ports = 0x006D;
		tf_num_of_allies,															// constexpr inline uint16_t num_of_allies = 0x006E;
		tf_num_of_vassals,														// constexpr inline uint16_t num_of_vassals = 0x006F;
		tf_owned_by_tag,															// constexpr inline uint16_t owned_by_tag = 0x0070;
		tf_owned_by_from_nation,											// constexpr inline uint16_t owned_by_from_nation = 0x0071;
		tf_owned_by_this_nation,											// constexpr inline uint16_t owned_by_this_nation = 0x0072;
		tf_owned_by_this_province,										// constexpr inline uint16_t owned_by_this_province = 0x0073;
		tf_owned_by_this_state,												// constexpr inline uint16_t owned_by_this_state = 0x0074;
		tf_owned_by_this_pop,													// constexpr inline uint16_t owned_by_this_pop = 0x0075;
		tf_exists_bool,																// constexpr inline uint16_t exists_bool = 0x0076;
		tf_exists_tag,																// constexpr inline uint16_t exists_tag = 0x0077;
		tf_has_country_flag,													// constexpr inline uint16_t has_country_flag = 0x0078;
		tf_continent_nation,													// constexpr inline uint16_t continent_nation = 0x0079;
		tf_continent_state,														// constexpr inline uint16_t continent_state = 0x007A;
		tf_continent_province,												// constexpr inline uint16_t continent_province = 0x007B;
		tf_continent_pop,															// constexpr inline uint16_t continent_pop = 0x007C;
		tf_continent_nation_this,											// constexpr inline uint16_t continent_nation_this = 0x007D;
		tf_continent_state_this,											// constexpr inline uint16_t continent_state_this = 0x007E;
		tf_continent_province_this,										// constexpr inline uint16_t continent_province_this = 0x007F;
		tf_continent_pop_this,												// constexpr inline uint16_t continent_pop_this = 0x0080;
		tf_continent_nation_from,											// constexpr inline uint16_t continent_nation_from = 0x0081;
		tf_continent_state_from,											// constexpr inline uint16_t continent_state_from = 0x0082;
		tf_continent_province_from,										// constexpr inline uint16_t continent_province_from = 0x0083;
		tf_continent_pop_from,												// constexpr inline uint16_t continent_pop_from = 0x0084;
		tf_casus_belli_tag,														// constexpr inline uint16_t casus_belli_tag = 0x0085;
		tf_casus_belli_from,													// constexpr inline uint16_t casus_belli_from = 0x0086;
		tf_casus_belli_this_nation,										// constexpr inline uint16_t casus_belli_this_nation = 0x0087;
		tf_casus_belli_this_state,										// constexpr inline uint16_t casus_belli_this_state = 0x0088;
		tf_casus_belli_this_province,									// constexpr inline uint16_t casus_belli_this_province = 0x0089;
		tf_casus_belli_this_pop,											// constexpr inline uint16_t casus_belli_this_pop = 0x008A;
		tf_military_access_tag,												// constexpr inline uint16_t military_access_tag = 0x008B;
		tf_military_access_from,											// constexpr inline uint16_t military_access_from = 0x008C;
		tf_military_access_this_nation,								// constexpr inline uint16_t military_access_this_nation = 0x008D;
		tf_military_access_this_state,								// constexpr inline uint16_t military_access_this_state = 0x008E;
		tf_military_access_this_province,							// constexpr inline uint16_t military_access_this_province = 0x008F;
		tf_military_access_this_pop,									// constexpr inline uint16_t military_access_this_pop = 0x0090;
		tf_prestige_value,														// constexpr inline uint16_t prestige_value = 0x0091;
		tf_prestige_from,															// constexpr inline uint16_t prestige_from = 0x0092;
		tf_prestige_this_nation,											// constexpr inline uint16_t prestige_this_nation = 0x0093;
		tf_prestige_this_state,												// constexpr inline uint16_t prestige_this_state = 0x0094;
		tf_prestige_this_province,										// constexpr inline uint16_t prestige_this_province = 0x0095;
		tf_prestige_this_pop,													// constexpr inline uint16_t prestige_this_pop = 0x0096;
		tf_badboy,																		// constexpr inline uint16_t badboy = 0x0097;
		tf_has_building_state,												// constexpr inline uint16_t has_building_state = 0x0098;
		tf_has_building_fort,													// constexpr inline uint16_t has_building_fort = 0x0099;
		tf_has_building_railroad,											// constexpr inline uint16_t has_building_railroad = 0x009A;
		tf_has_building_naval_base,										// constexpr inline uint16_t has_building_naval_base = 0x009B;
		tf_empty,																			// constexpr inline uint16_t empty = 0x009C;
		tf_is_blockaded,															// constexpr inline uint16_t is_blockaded = 0x009D;
		tf_has_country_modifier,											// constexpr inline uint16_t has_country_modifier = 0x009E;
		tf_has_province_modifier,											// constexpr inline uint16_t has_province_modifier = 0x009F;
		tf_region,																		// constexpr inline uint16_t region = 0x00A0;
		tf_tag_tag,																		// constexpr inline uint16_t tag_tag = 0x00A1;
		tf_tag_this_nation,														// constexpr inline uint16_t tag_this_nation = 0x00A2;
		tf_tag_this_province,													// constexpr inline uint16_t tag_this_province = 0x00A3;
		tf_tag_from_nation,														// constexpr inline uint16_t tag_from_nation = 0x00A4;
		tf_tag_from_province,													// constexpr inline uint16_t tag_from_province = 0x00A5;
		tf_neighbour_tag,															// constexpr inline uint16_t neighbour_tag = 0x00A6;
		tf_neighbour_this,														// constexpr inline uint16_t neighbour_this = 0x00A7;
		tf_neighbour_from,														// constexpr inline uint16_t neighbour_from = 0x00A8;
		tf_units_in_province_value,										// constexpr inline uint16_t units_in_province_value = 0x00A9;
		tf_units_in_province_from,										// constexpr inline uint16_t units_in_province_from = 0x00AA;
		tf_units_in_province_this_nation,							// constexpr inline uint16_t units_in_province_this_nation = 0x00AB;
		tf_units_in_province_this_province,						// constexpr inline uint16_t units_in_province_this_province = 0x00AC;
		tf_units_in_province_this_state,							// constexpr inline uint16_t units_in_province_this_state = 0x00AD;
		tf_units_in_province_this_pop,								// constexpr inline uint16_t units_in_province_this_pop = 0x00AE;
		tf_war_with_tag,															// constexpr inline uint16_t war_with_tag = 0x00AF;
		tf_war_with_from,															// constexpr inline uint16_t war_with_from = 0x00B0;
		tf_war_with_this_nation,											// constexpr inline uint16_t war_with_this_nation = 0x00B1;
		tf_war_with_this_province,										// constexpr inline uint16_t war_with_this_province = 0x00B2;
		tf_war_with_this_state,												// constexpr inline uint16_t war_with_this_state = 0x00B3;
		tf_war_with_this_pop,													// constexpr inline uint16_t war_with_this_pop = 0x00B4;
		tf_unit_in_battle,														// constexpr inline uint16_t unit_in_battle = 0x00B5;
		tf_total_amount_of_divisions,									// constexpr inline uint16_t total_amount_of_divisions = 0x00B6;
		tf_money,																			// constexpr inline uint16_t money = 0x00B7;
		tf_lost_national,															// constexpr inline uint16_t lost_national = 0x00B8;
		tf_is_vassal,																	// constexpr inline uint16_t is_vassal = 0x00B9;
		tf_ruling_party_ideology_nation,									// constexpr inline uint16_t ruling_party_ideology_pop = 0x00BA;
		tf_ruling_party_ideology_nation,							// constexpr inline uint16_t ruling_party_ideology_nation = 0x00BB;
		tf_ruling_party,															// constexpr inline uint16_t ruling_party = 0x00BC;
		tf_is_ideology_enabled,												// constexpr inline uint16_t is_ideology_enabled = 0x00BD;
		tf_political_reform_want_nation,							// constexpr inline uint16_t political_reform_want_nation = 0x00BE;
		tf_political_reform_want_pop,									// constexpr inline uint16_t political_reform_want_pop = 0x00BF;
		tf_social_reform_want_nation,									// constexpr inline uint16_t social_reform_want_nation = 0x00C0;
		tf_social_reform_want_pop,										// constexpr inline uint16_t social_reform_want_pop = 0x00C1;
		tf_total_amount_of_ships,											// constexpr inline uint16_t total_amount_of_ships = 0x00C2;
		tf_plurality,																	// constexpr inline uint16_t plurality = 0x00C3;
		tf_corruption,																// constexpr inline uint16_t corruption = 0x00C4;
		tf_is_state_religion_pop,											// constexpr inline uint16_t is_state_religion_pop = 0x00C5;
		tf_is_state_religion_province,								// constexpr inline uint16_t is_state_religion_province = 0x00C6;
		tf_is_state_religion_state,										// constexpr inline uint16_t is_state_religion_state = 0x00C7;
		tf_is_primary_culture_pop,										// constexpr inline uint16_t is_primary_culture_pop = 0x00C8;
		tf_is_primary_culture_province,								// constexpr inline uint16_t is_primary_culture_province = 0x00C9;
		tf_is_primary_culture_state,									// constexpr inline uint16_t is_primary_culture_state = 0x00CA;
		tf_is_primary_culture_nation_this_pop,				// constexpr inline uint16_t is_primary_culture_nation_this_pop = 0x00CB;
		tf_is_primary_culture_nation_this_nation,			// constexpr inline uint16_t is_primary_culture_nation_this_nation = 0x00CC;
		tf_is_primary_culture_nation_this_state,			// constexpr inline uint16_t is_primary_culture_nation_this_state = 0x00CD;
		tf_is_primary_culture_nation_this_province,		// constexpr inline uint16_t is_primary_culture_nation_this_province = 0x00CE;
		tf_is_primary_culture_state_this_pop,					// constexpr inline uint16_t is_primary_culture_state_this_pop = 0x00CF;
		tf_is_primary_culture_state_this_nation,			// constexpr inline uint16_t is_primary_culture_state_this_nation = 0x00D0;
		tf_is_primary_culture_state_this_state,				// constexpr inline uint16_t is_primary_culture_state_this_state = 0x00D1;
		tf_is_primary_culture_state_this_province,		// constexpr inline uint16_t is_primary_culture_state_this_province = 0x00D2;
		tf_is_primary_culture_province_this_pop,			// constexpr inline uint16_t is_primary_culture_province_this_pop = 0x00D3;
		tf_is_primary_culture_province_this_nation,		// constexpr inline uint16_t is_primary_culture_province_this_nation = 0x00D4;
		tf_is_primary_culture_province_this_state,		// constexpr inline uint16_t is_primary_culture_province_this_state = 0x00D5;
		tf_is_primary_culture_province_this_province, // constexpr inline uint16_t is_primary_culture_province_this_province = 0x00D6;
		tf_is_primary_culture_pop_this_pop,						// constexpr inline uint16_t is_primary_culture_pop_this_pop = 0x00D7;
		tf_is_primary_culture_pop_this_nation,				// constexpr inline uint16_t is_primary_culture_pop_this_nation = 0x00D8;
		tf_is_primary_culture_pop_this_state,					// constexpr inline uint16_t is_primary_culture_pop_this_state = 0x00D9;
		tf_is_primary_culture_pop_this_province,			// constexpr inline uint16_t is_primary_culture_pop_this_province = 0x00DA;
		tf_is_accepted_culture_pop,										// constexpr inline uint16_t is_accepted_culture_pop = 0x00DB;
		tf_is_accepted_culture_province,							// constexpr inline uint16_t is_accepted_culture_province = 0x00DC;
		tf_is_accepted_culture_state,									// constexpr inline uint16_t is_accepted_culture_state = 0x00DD;
		tf_is_coastal_province,																// constexpr inline uint16_t is_coastal_province = 0x00DE;
		tf_in_sphere_tag,															// constexpr inline uint16_t in_sphere_tag = 0x00DF;
		tf_in_sphere_from,														// constexpr inline uint16_t in_sphere_from = 0x00E0;
		tf_in_sphere_this_nation,											// constexpr inline uint16_t in_sphere_this_nation = 0x00E1;
		tf_in_sphere_this_province,										// constexpr inline uint16_t in_sphere_this_province = 0x00E2;
		tf_in_sphere_this_state,											// constexpr inline uint16_t in_sphere_this_state = 0x00E3;
		tf_in_sphere_this_pop,												// constexpr inline uint16_t in_sphere_this_pop = 0x00E4;
		tf_produces_nation,														// constexpr inline uint16_t produces_nation = 0x00E5;
		tf_produces_state,														// constexpr inline uint16_t produces_state = 0x00E6;
		tf_produces_province,													// constexpr inline uint16_t produces_province = 0x00E7;
		tf_produces_pop,															// constexpr inline uint16_t produces_pop = 0x00E8;
		tf_average_militancy_nation,									// constexpr inline uint16_t average_militancy_nation = 0x00E9;
		tf_average_militancy_state,										// constexpr inline uint16_t average_militancy_state = 0x00EA;
		tf_average_militancy_province,								// constexpr inline uint16_t average_militancy_province = 0x00EB;
		tf_average_consciousness_nation,							// constexpr inline uint16_t average_consciousness_nation = 0x00EC;
		tf_average_consciousness_state,								// constexpr inline uint16_t average_consciousness_state = 0x00ED;
		tf_average_consciousness_province,						// constexpr inline uint16_t average_consciousness_province = 0x00EE;
		tf_is_next_reform_nation,											// constexpr inline uint16_t is_next_reform_nation = 0x00EF;
		tf_is_next_reform_pop,												// constexpr inline uint16_t is_next_reform_pop = 0x00F0;
		tf_rebel_power_fraction,											// constexpr inline uint16_t rebel_power_fraction = 0x00F1;
		tf_recruited_percentage_nation,								// constexpr inline uint16_t recruited_percentage_nation = 0x00F2;
		tf_recruited_percentage_pop,									// constexpr inline uint16_t recruited_percentage_pop = 0x00F3;
		tf_has_culture_core,													// constexpr inline uint16_t has_culture_core = 0x00F4;
		tf_nationalism,																// constexpr inline uint16_t nationalism = 0x00F5;
		tf_is_overseas,																// constexpr inline uint16_t is_overseas = 0x00F6;
		tf_controlled_by_rebels,											// constexpr inline uint16_t controlled_by_rebels = 0x00F7;
		tf_controlled_by_tag,													// constexpr inline uint16_t controlled_by_tag = 0x00F8;
		tf_controlled_by_from,												// constexpr inline uint16_t controlled_by_from = 0x00F9;
		tf_controlled_by_this_nation,									// constexpr inline uint16_t controlled_by_this_nation = 0x00FA;
		tf_controlled_by_this_province,								// constexpr inline uint16_t controlled_by_this_province = 0x00FB;
		tf_controlled_by_this_state,									// constexpr inline uint16_t controlled_by_this_state = 0x00FC;
		tf_controlled_by_this_pop,										// constexpr inline uint16_t controlled_by_this_pop = 0x00FD;
		tf_controlled_by_owner,												// constexpr inline uint16_t controlled_by_owner = 0x00FE;
		tf_controlled_by_reb,													// constexpr inline uint16_t controlled_by_reb = 0x00FF;
		tf_is_canal_enabled,													// constexpr inline uint16_t is_canal_enabled = 0x0100;
		tf_is_state_capital,													// constexpr inline uint16_t is_state_capital = 0x0101;
		tf_truce_with_tag,														// constexpr inline uint16_t truce_with_tag = 0x0102;
		tf_truce_with_from,														// constexpr inline uint16_t truce_with_from = 0x0103;
		tf_truce_with_this_nation,										// constexpr inline uint16_t truce_with_this_nation = 0x0104;
		tf_truce_with_this_province,									// constexpr inline uint16_t truce_with_this_province = 0x0105;
		tf_truce_with_this_state,											// constexpr inline uint16_t truce_with_this_state = 0x0106;
		tf_truce_with_this_pop,												// constexpr inline uint16_t truce_with_this_pop = 0x0107;
		tf_total_pops_nation,													// constexpr inline uint16_t total_pops_nation = 0x0108;
		tf_total_pops_state,													// constexpr inline uint16_t total_pops_state = 0x0109;
		tf_total_pops_province,												// constexpr inline uint16_t total_pops_province = 0x010A;
		tf_total_pops_pop,														// constexpr inline uint16_t total_pops_pop = 0x010B;
		tf_has_pop_type_nation,												// constexpr inline uint16_t has_pop_type_nation = 0x010C;
		tf_has_pop_type_state,												// constexpr inline uint16_t has_pop_type_state = 0x010D;
		tf_has_pop_type_province,											// constexpr inline uint16_t has_pop_type_province = 0x010E;
		tf_has_pop_type_pop,													// constexpr inline uint16_t has_pop_type_pop = 0x010F;
		tf_has_empty_adjacent_province,								// constexpr inline uint16_t has_empty_adjacent_province = 0x0110;
		tf_has_leader,																// constexpr inline uint16_t has_leader = 0x0111;
		tf_ai,																				// constexpr inline uint16_t ai = 0x0112;
		tf_can_create_vassals,												// constexpr inline uint16_t can_create_vassals = 0x0113;
		tf_is_possible_vassal,												// constexpr inline uint16_t is_possible_vassal = 0x0114;
		tf_province_id,																// constexpr inline uint16_t province_id = 0x0115;
		tf_vassal_of_tag,															// constexpr inline uint16_t vassal_of_tag = 0x0116;
		tf_vassal_of_from,														// constexpr inline uint16_t vassal_of_from = 0x0117;
		tf_vassal_of_this_nation,											// constexpr inline uint16_t vassal_of_this_nation = 0x0118;
		tf_vassal_of_this_province,										// constexpr inline uint16_t vassal_of_this_province = 0x0119;
		tf_vassal_of_this_state,											// constexpr inline uint16_t vassal_of_this_state = 0x011A;
		tf_vassal_of_this_pop,												// constexpr inline uint16_t vassal_of_this_pop = 0x011B;
		tf_alliance_with_tag,													// constexpr inline uint16_t alliance_with_tag = 0x011C;
		tf_alliance_with_from,												// constexpr inline uint16_t alliance_with_from = 0x011D;
		tf_alliance_with_this_nation,									// constexpr inline uint16_t alliance_with_this_nation = 0x011E;
		tf_alliance_with_this_province,								// constexpr inline uint16_t alliance_with_this_province = 0x011F;
		tf_alliance_with_this_state,									// constexpr inline uint16_t alliance_with_this_state = 0x0120;
		tf_alliance_with_this_pop,										// constexpr inline uint16_t alliance_with_this_pop = 0x0121;
		tf_has_recently_lost_war,											// constexpr inline uint16_t has_recently_lost_war = 0x0122;
		tf_is_mobilised,															// constexpr inline uint16_t is_mobilised = 0x0123;
		tf_mobilisation_size,													// constexpr inline uint16_t mobilisation_size = 0x0124;
		tf_crime_higher_than_education_nation,				// constexpr inline uint16_t crime_higher_than_education_nation = 0x0125;
		tf_crime_higher_than_education_state,					// constexpr inline uint16_t crime_higher_than_education_state = 0x0126;
		tf_crime_higher_than_education_province,			// constexpr inline uint16_t crime_higher_than_education_province = 0x0127;
		tf_crime_higher_than_education_pop,						// constexpr inline uint16_t crime_higher_than_education_pop = 0x0128;
		tf_agree_with_ruling_party,										// constexpr inline uint16_t agree_with_ruling_party = 0x0129;
		tf_is_colonial_state,													// constexpr inline uint16_t is_colonial_state = 0x012A;
		tf_is_colonial_province,											// constexpr inline uint16_t is_colonial_province = 0x012B;
		tf_has_factories_state,															// constexpr inline uint16_t has_factories_state = 0x012C;
		tf_in_default_tag,														// constexpr inline uint16_t in_default_tag = 0x012D;
		tf_in_default_from,														// constexpr inline uint16_t in_default_from = 0x012E;
		tf_in_default_this_nation,										// constexpr inline uint16_t in_default_this_nation = 0x012F;
		tf_in_default_this_province,									// constexpr inline uint16_t in_default_this_province = 0x0130;
		tf_in_default_this_state,											// constexpr inline uint16_t in_default_this_state = 0x0131;
		tf_in_default_this_pop,												// constexpr inline uint16_t in_default_this_pop = 0x0132;
		tf_total_num_of_ports,												// constexpr inline uint16_t total_num_of_ports = 0x0133;
		tf_always,																		// constexpr inline uint16_t always = 0x0134;
		tf_election,																	// constexpr inline uint16_t election = 0x0135;
		tf_has_global_flag,														// constexpr inline uint16_t has_global_flag = 0x0136;
		tf_is_capital,																// constexpr inline uint16_t is_capital = 0x0137;
		tf_nationalvalue_nation,											// constexpr inline uint16_t nationalvalue_nation = 0x0138;
		tf_industrial_score_value,										// constexpr inline uint16_t industrial_score_value = 0x0139;
		tf_industrial_score_from_nation,							// constexpr inline uint16_t industrial_score_from_nation = 0x013A;
		tf_industrial_score_this_nation,							// constexpr inline uint16_t industrial_score_this_nation = 0x013B;
		tf_industrial_score_this_pop,									// constexpr inline uint16_t industrial_score_this_pop = 0x013C;
		tf_industrial_score_this_state,								// constexpr inline uint16_t industrial_score_this_state = 0x013D;
		tf_industrial_score_this_province,						// constexpr inline uint16_t industrial_score_this_province = 0x013E;
		tf_military_score_value,											// constexpr inline uint16_t military_score_value = 0x013F;
		tf_military_score_from_nation,								// constexpr inline uint16_t military_score_from_nation = 0x0140;
		tf_military_score_this_nation,								// constexpr inline uint16_t military_score_this_nation = 0x0141;
		tf_military_score_this_pop,										// constexpr inline uint16_t military_score_this_pop = 0x0142;
		tf_military_score_this_state,									// constexpr inline uint16_t military_score_this_state = 0x0143;
		tf_military_score_this_province,							// constexpr inline uint16_t military_score_this_province = 0x0144;
		tf_civilized_nation,													// constexpr inline uint16_t civilized_nation = 0x0145;
		tf_civilized_pop,															// constexpr inline uint16_t civilized_pop = 0x0146;
		tf_civilized_province,												// constexpr inline uint16_t civilized_province = 0x0147;
		tf_national_provinces_occupied,								// constexpr inline uint16_t national_provinces_occupied = 0x0148;
		tf_is_greater_power_nation,										// constexpr inline uint16_t is_greater_power_nation = 0x0149;
		tf_is_greater_power_pop,											// constexpr inline uint16_t is_greater_power_pop = 0x014A;
		tf_rich_tax,																	// constexpr inline uint16_t rich_tax = 0x014B;
		tf_middle_tax,																// constexpr inline uint16_t middle_tax = 0x014C;
		tf_poor_tax,																	// constexpr inline uint16_t poor_tax = 0x014D;
		tf_social_spending_nation,										// constexpr inline uint16_t social_spending_nation = 0x014E;
		tf_social_spending_pop,												// constexpr inline uint16_t social_spending_pop = 0x014F;
		tf_social_spending_province,									// constexpr inline uint16_t social_spending_province = 0x0150;
		tf_colonial_nation,														// constexpr inline uint16_t colonial_nation = 0x0151;
		tf_pop_majority_religion_nation,							// constexpr inline uint16_t pop_majority_religion_nation = 0x0152;
		tf_pop_majority_religion_state,								// constexpr inline uint16_t pop_majority_religion_state = 0x0153;
		tf_pop_majority_religion_province,						// constexpr inline uint16_t pop_majority_religion_province = 0x0154;
		tf_pop_majority_culture_nation,								// constexpr inline uint16_t pop_majority_culture_nation = 0x0155;
		tf_pop_majority_culture_state,								// constexpr inline uint16_t pop_majority_culture_state = 0x0156;
		tf_pop_majority_culture_province,							// constexpr inline uint16_t pop_majority_culture_province = 0x0157;
		tf_pop_majority_issue_nation,									// constexpr inline uint16_t pop_majority_issue_nation = 0x0158;
		tf_pop_majority_issue_state,									// constexpr inline uint16_t pop_majority_issue_state = 0x0159;
		tf_pop_majority_issue_province,								// constexpr inline uint16_t pop_majority_issue_province = 0x015A;
		tf_pop_majority_issue_pop,										// constexpr inline uint16_t pop_majority_issue_pop = 0x015B;
		tf_pop_majority_ideology_nation,							// constexpr inline uint16_t pop_majority_ideology_nation = 0x015C;
		tf_pop_majority_ideology_state,								// constexpr inline uint16_t pop_majority_ideology_state = 0x015D;
		tf_pop_majority_ideology_province,						// constexpr inline uint16_t pop_majority_ideology_province = 0x015E;
		tf_pop_majority_ideology_pop,									// constexpr inline uint16_t pop_majority_ideology_pop = 0x015F;
		tf_poor_strata_militancy_nation,							// constexpr inline uint16_t poor_strata_militancy_nation = 0x0160;
		tf_poor_strata_militancy_state,								// constexpr inline uint16_t poor_strata_militancy_state = 0x0161;
		tf_poor_strata_militancy_province,						// constexpr inline uint16_t poor_strata_militancy_province = 0x0162;
		tf_poor_strata_militancy_pop,									// constexpr inline uint16_t poor_strata_militancy_pop = 0x0163;
		tf_middle_strata_militancy_nation,						// constexpr inline uint16_t middle_strata_militancy_nation = 0x0164;
		tf_middle_strata_militancy_state,							// constexpr inline uint16_t middle_strata_militancy_state = 0x0165;
		tf_middle_strata_militancy_province,					// constexpr inline uint16_t middle_strata_militancy_province = 0x0166;
		tf_middle_strata_militancy_pop,								// constexpr inline uint16_t middle_strata_militancy_pop = 0x0167;
		tf_rich_strata_militancy_nation,							// constexpr inline uint16_t rich_strata_militancy_nation = 0x0168;
		tf_rich_strata_militancy_state,								// constexpr inline uint16_t rich_strata_militancy_state = 0x0169;
		tf_rich_strata_militancy_province,						// constexpr inline uint16_t rich_strata_militancy_province = 0x016A;
		tf_rich_strata_militancy_pop,									// constexpr inline uint16_t rich_strata_militancy_pop = 0x016B;
		tf_rich_tax_above_poor,												// constexpr inline uint16_t rich_tax_above_poor = 0x016C;
		tf_culture_has_union_tag_pop,									// constexpr inline uint16_t culture_has_union_tag_pop = 0x016D;
		tf_culture_has_union_tag_nation,							// constexpr inline uint16_t culture_has_union_tag_nation = 0x016E;
		tf_this_culture_union_tag,										// constexpr inline uint16_t this_culture_union_tag = 0x016F;
		tf_this_culture_union_from,										// constexpr inline uint16_t this_culture_union_from = 0x0170;
		tf_this_culture_union_this_nation,						// constexpr inline uint16_t this_culture_union_this_nation = 0x0171;
		tf_this_culture_union_this_province,					// constexpr inline uint16_t this_culture_union_this_province = 0x0172;
		tf_this_culture_union_this_state,							// constexpr inline uint16_t this_culture_union_this_state = 0x0173;
		tf_this_culture_union_this_pop,								// constexpr inline uint16_t this_culture_union_this_pop = 0x0174;
		tf_this_culture_union_this_union_nation,			// constexpr inline uint16_t this_culture_union_this_union_nation = 0x0175;
		tf_this_culture_union_this_union_province,		// constexpr inline uint16_t this_culture_union_this_union_province = 0x0176;
		tf_this_culture_union_this_union_state,				// constexpr inline uint16_t this_culture_union_this_union_state = 0x0177;
		tf_this_culture_union_this_union_pop,					// constexpr inline uint16_t this_culture_union_this_union_pop = 0x0178;
		tf_minorities_nation,													// constexpr inline uint16_t minorities_nation = 0x0179;
		tf_minorities_state,													// constexpr inline uint16_t minorities_state = 0x017A;
		tf_minorities_province,												// constexpr inline uint16_t minorities_province = 0x017B;
		tf_revanchism_nation,													// constexpr inline uint16_t revanchism_nation = 0x017C;
		tf_revanchism_pop,														// constexpr inline uint16_t revanchism_pop = 0x017D;
		tf_has_crime,																	// constexpr inline uint16_t has_crime = 0x017E;
		tf_num_of_substates,													// constexpr inline uint16_t num_of_substates = 0x017F;
		tf_num_of_vassals_no_substates,								// constexpr inline uint16_t num_of_vassals_no_substates = 0x0180;
		tf_brigades_compare_this,											// constexpr inline uint16_t brigades_compare_this = 0x0181;
		tf_brigades_compare_from,											// constexpr inline uint16_t brigades_compare_from = 0x0182;
		tf_constructing_cb_tag,												// constexpr inline uint16_t constructing_cb_tag = 0x0183;
		tf_constructing_cb_from,											// constexpr inline uint16_t constructing_cb_from = 0x0184;
		tf_constructing_cb_this_nation,								// constexpr inline uint16_t constructing_cb_this_nation = 0x0185;
		tf_constructing_cb_this_province,							// constexpr inline uint16_t constructing_cb_this_province = 0x0186;
		tf_constructing_cb_this_state,								// constexpr inline uint16_t constructing_cb_this_state = 0x0187;
		tf_constructing_cb_this_pop,									// constexpr inline uint16_t constructing_cb_this_pop = 0x0188;
		tf_constructing_cb_discovered,								// constexpr inline uint16_t constructing_cb_discovered = 0x0189;
		tf_constructing_cb_progress,									// constexpr inline uint16_t constructing_cb_progress = 0x018A;
		tf_civilization_progress,											// constexpr inline uint16_t civilization_progress = 0x018B;
		tf_constructing_cb_type,											// constexpr inline uint16_t constructing_cb_type = 0x018C;
		tf_is_our_vassal_tag,													// constexpr inline uint16_t is_our_vassal_tag = 0x018D;
		tf_is_our_vassal_from,												// constexpr inline uint16_t is_our_vassal_from = 0x018E;
		tf_is_our_vassal_this_nation,									// constexpr inline uint16_t is_our_vassal_this_nation = 0x018F;
		tf_is_our_vassal_this_province,								// constexpr inline uint16_t is_our_vassal_this_province = 0x0190;
		tf_is_our_vassal_this_state,									// constexpr inline uint16_t is_our_vassal_this_state = 0x0191;
		tf_is_our_vassal_this_pop,										// constexpr inline uint16_t is_our_vassal_this_pop = 0x0192;
		tf_substate_of_tag,														// constexpr inline uint16_t substate_of_tag = 0x0193;
		tf_substate_of_from,													// constexpr inline uint16_t substate_of_from = 0x0194;
		tf_substate_of_this_nation,										// constexpr inline uint16_t substate_of_this_nation = 0x0195;
		tf_substate_of_this_province,									// constexpr inline uint16_t substate_of_this_province = 0x0196;
		tf_substate_of_this_state,										// constexpr inline uint16_t substate_of_this_state = 0x0197;
		tf_substate_of_this_pop,											// constexpr inline uint16_t substate_of_this_pop = 0x0198;
		tf_is_substate,																// constexpr inline uint16_t is_substate = 0x0199;
		tf_great_wars_enabled,												// constexpr inline uint16_t great_wars_enabled = 0x019A;
		tf_can_nationalize,														// constexpr inline uint16_t can_nationalize = 0x019B;
		tf_part_of_sphere,														// constexpr inline uint16_t part_of_sphere = 0x019C;
		tf_is_sphere_leader_of_tag,										// constexpr inline uint16_t is_sphere_leader_of_tag = 0x019D;
		tf_is_sphere_leader_of_from,									// constexpr inline uint16_t is_sphere_leader_of_from = 0x019E;
		tf_is_sphere_leader_of_this_nation,						// constexpr inline uint16_t is_sphere_leader_of_this_nation = 0x019F;
		tf_is_sphere_leader_of_this_province,					// constexpr inline uint16_t is_sphere_leader_of_this_province = 0x01A0;
		tf_is_sphere_leader_of_this_state,						// constexpr inline uint16_t is_sphere_leader_of_this_state = 0x01A1;
		tf_is_sphere_leader_of_this_pop,							// constexpr inline uint16_t is_sphere_leader_of_this_pop = 0x01A2;
		tf_number_of_states,													// constexpr inline uint16_t number_of_states = 0x01A3;
		tf_war_score,																	// constexpr inline uint16_t war_score = 0x01A4;
		tf_is_releasable_vassal_from,									// constexpr inline uint16_t is_releasable_vassal_from = 0x01A5;
		tf_is_releasable_vassal_other,								// constexpr inline uint16_t is_releasable_vassal_other = 0x01A6;
		tf_has_recent_imigration,											// constexpr inline uint16_t has_recent_imigration = 0x01A7;
		tf_province_control_days,											// constexpr inline uint16_t province_control_days = 0x01A8;
		tf_is_disarmed,																// constexpr inline uint16_t is_disarmed = 0x01A9;
		tf_big_producer,															// constexpr inline uint16_t big_producer = 0x01AA;
		tf_someone_can_form_union_tag_from,						// constexpr inline uint16_t someone_can_form_union_tag_from = 0x01AB;
		tf_someone_can_form_union_tag_other,					// constexpr inline uint16_t someone_can_form_union_tag_other = 0x01AC;
		tf_social_movement_strength,									// constexpr inline uint16_t social_movement_strength = 0x01AD;
		tf_political_movement_strength,								// constexpr inline uint16_t political_movement_strength = 0x01AE;
		tf_can_build_factory_in_capital_state,				// constexpr inline uint16_t can_build_factory_in_capital_state = 0x01AF;
		tf_social_movement,														// constexpr inline uint16_t social_movement = 0x01B0;
		tf_political_movement,												// constexpr inline uint16_t political_movement = 0x01B1;
		tf_has_cultural_sphere,												// constexpr inline uint16_t has_cultural_sphere = 0x01B2;
		tf_world_wars_enabled,												// constexpr inline uint16_t world_wars_enabled = 0x01B3;
		tf_has_pop_culture_pop_this_pop,							// constexpr inline uint16_t has_pop_culture_pop_this_pop = 0x01B4;
		tf_has_pop_culture_state_this_pop,						// constexpr inline uint16_t has_pop_culture_state_this_pop = 0x01B5;
		tf_has_pop_culture_province_this_pop,					// constexpr inline uint16_t has_pop_culture_province_this_pop = 0x01B6;
		tf_has_pop_culture_nation_this_pop,						// constexpr inline uint16_t has_pop_culture_nation_this_pop = 0x01B7;
		tf_has_pop_culture_pop,												// constexpr inline uint16_t has_pop_culture_pop = 0x01B8;
		tf_has_pop_culture_state,											// constexpr inline uint16_t has_pop_culture_state = 0x01B9;
		tf_has_pop_culture_province,									// constexpr inline uint16_t has_pop_culture_province = 0x01BA;
		tf_has_pop_culture_nation,										// constexpr inline uint16_t has_pop_culture_nation = 0x01BB;
		tf_has_pop_religion_pop_this_pop,							// constexpr inline uint16_t has_pop_religion_pop_this_pop = 0x01BC;
		tf_has_pop_religion_state_this_pop,						// constexpr inline uint16_t has_pop_religion_state_this_pop = 0x01BD;
		tf_has_pop_religion_province_this_pop,				// constexpr inline uint16_t has_pop_religion_province_this_pop = 0x01BE;
		tf_has_pop_religion_nation_this_pop,					// constexpr inline uint16_t has_pop_religion_nation_this_pop = 0x01BF;
		tf_has_pop_religion_pop,											// constexpr inline uint16_t has_pop_religion_pop = 0x01C0;
		tf_has_pop_religion_state,										// constexpr inline uint16_t has_pop_religion_state = 0x01C1;
		tf_has_pop_religion_province,									// constexpr inline uint16_t has_pop_religion_province = 0x01C2;
		tf_has_pop_religion_nation,										// constexpr inline uint16_t has_pop_religion_nation = 0x01C3;
		tf_life_needs,																// constexpr inline uint16_t life_needs = 0x01C4;
		tf_everyday_needs,														// constexpr inline uint16_t everyday_needs = 0x01C5;
		tf_luxury_needs,															// constexpr inline uint16_t luxury_needs = 0x01C6;
		tf_consciousness_pop,													// constexpr inline uint16_t consciousness_pop = 0x01C7;
		tf_consciousness_province,										// constexpr inline uint16_t consciousness_province = 0x01C8;
		tf_consciousness_state,												// constexpr inline uint16_t consciousness_state = 0x01C9;
		tf_consciousness_nation,											// constexpr inline uint16_t consciousness_nation = 0x01CA;
		tf_literacy_pop,															// constexpr inline uint16_t literacy_pop = 0x01CB;
		tf_literacy_province,													// constexpr inline uint16_t literacy_province = 0x01CC;
		tf_literacy_state,														// constexpr inline uint16_t literacy_state = 0x01CD;
		tf_literacy_nation,														// constexpr inline uint16_t literacy_nation = 0x01CE;
		tf_militancy_pop,															// constexpr inline uint16_t militancy_pop = 0x01CF;
		tf_militancy_province,												// constexpr inline uint16_t militancy_province = 0x01D0;
		tf_militancy_state,														// constexpr inline uint16_t militancy_state = 0x01D1;
		tf_militancy_nation,													// constexpr inline uint16_t militancy_nation = 0x01D2;
		tf_military_spending_pop,											// constexpr inline uint16_t military_spending_pop = 0x01D3;
		tf_military_spending_province,								// constexpr inline uint16_t military_spending_province = 0x01D4;
		tf_military_spending_state,										// constexpr inline uint16_t military_spending_state = 0x01D5;
		tf_military_spending_nation,									// constexpr inline uint16_t military_spending_nation = 0x01D6;
		tf_administration_spending_pop,								// constexpr inline uint16_t administration_spending_pop = 0x01D7;
		tf_administration_spending_province,					// constexpr inline uint16_t administration_spending_province = 0x01D8;
		tf_administration_spending_state,							// constexpr inline uint16_t administration_spending_state = 0x01D9;
		tf_administration_spending_nation,						// constexpr inline uint16_t administration_spending_nation = 0x01DA;
		tf_education_spending_pop,										// constexpr inline uint16_t education_spending_pop = 0x01DB;
		tf_education_spending_province,								// constexpr inline uint16_t education_spending_province = 0x01DC;
		tf_education_spending_state,									// constexpr inline uint16_t education_spending_state = 0x01DD;
		tf_education_spending_nation,									// constexpr inline uint16_t education_spending_nation = 0x01DE;
		tf_trade_goods_in_state_state,								// constexpr inline uint16_t trade_goods_in_state_state = 0x01DF;
		tf_trade_goods_in_state_province,							// constexpr inline uint16_t trade_goods_in_state_province = 0x01E0;
		tf_has_flashpoint,														// constexpr inline uint16_t has_flashpoint = 0x01E1;
		tf_flashpoint_tension,												// constexpr inline uint16_t flashpoint_tension = 0x01E2;
		tf_crisis_exist,															// constexpr inline uint16_t crisis_exist = 0x01E3;
		tf_is_liberation_crisis,											// constexpr inline uint16_t is_liberation_crisis = 0x01E4;
		tf_is_claim_crisis,														// constexpr inline uint16_t is_claim_crisis = 0x01E5;
		tf_crisis_temperature,												// constexpr inline uint16_t crisis_temperature = 0x01E6;
		tf_involved_in_crisis_pop,										// constexpr inline uint16_t involved_in_crisis_pop = 0x01E7;
		tf_involved_in_crisis_nation,									// constexpr inline uint16_t involved_in_crisis_nation = 0x01E8;
		tf_rich_strata_life_needs_nation,							// constexpr inline uint16_t rich_strata_life_needs_nation = 0x01E9;
		tf_rich_strata_life_needs_state,							// constexpr inline uint16_t rich_strata_life_needs_state = 0x01EA;
		tf_rich_strata_life_needs_province,						// constexpr inline uint16_t rich_strata_life_needs_province = 0x01EB;
		tf_rich_strata_life_needs_pop,								// constexpr inline uint16_t rich_strata_life_needs_pop = 0x01EC;
		tf_rich_strata_everyday_needs_nation,					// constexpr inline uint16_t rich_strata_everyday_needs_nation = 0x01ED;
		tf_rich_strata_everyday_needs_state,					// constexpr inline uint16_t rich_strata_everyday_needs_state = 0x01EE;
		tf_rich_strata_everyday_needs_province,				// constexpr inline uint16_t rich_strata_everyday_needs_province = 0x01EF;
		tf_rich_strata_everyday_needs_pop,						// constexpr inline uint16_t rich_strata_everyday_needs_pop = 0x01F0;
		tf_rich_strata_luxury_needs_nation,						// constexpr inline uint16_t rich_strata_luxury_needs_nation = 0x01F1;
		tf_rich_strata_luxury_needs_state,						// constexpr inline uint16_t rich_strata_luxury_needs_state = 0x01F2;
		tf_rich_strata_luxury_needs_province,					// constexpr inline uint16_t rich_strata_luxury_needs_province = 0x01F3;
		tf_rich_strata_luxury_needs_pop,							// constexpr inline uint16_t rich_strata_luxury_needs_pop = 0x01F4;
		tf_middle_strata_life_needs_nation,						// constexpr inline uint16_t middle_strata_life_needs_nation = 0x01F5;
		tf_middle_strata_life_needs_state,						// constexpr inline uint16_t middle_strata_life_needs_state = 0x01F6;
		tf_middle_strata_life_needs_province,					// constexpr inline uint16_t middle_strata_life_needs_province = 0x01F7;
		tf_middle_strata_life_needs_pop,							// constexpr inline uint16_t middle_strata_life_needs_pop = 0x01F8;
		tf_middle_strata_everyday_needs_nation,				// constexpr inline uint16_t middle_strata_everyday_needs_nation = 0x01F9;
		tf_middle_strata_everyday_needs_state,				// constexpr inline uint16_t middle_strata_everyday_needs_state = 0x01FA;
		tf_middle_strata_everyday_needs_province,			// constexpr inline uint16_t middle_strata_everyday_needs_province = 0x01FB;
		tf_middle_strata_everyday_needs_pop,					// constexpr inline uint16_t middle_strata_everyday_needs_pop = 0x01FC;
		tf_middle_strata_luxury_needs_nation,					// constexpr inline uint16_t middle_strata_luxury_needs_nation = 0x01FD;
		tf_middle_strata_luxury_needs_state,					// constexpr inline uint16_t middle_strata_luxury_needs_state = 0x01FE;
		tf_middle_strata_luxury_needs_province,				// constexpr inline uint16_t middle_strata_luxury_needs_province = 0x01FF;
		tf_middle_strata_luxury_needs_pop,						// constexpr inline uint16_t middle_strata_luxury_needs_pop = 0x0200;
		tf_poor_strata_life_needs_nation,							// constexpr inline uint16_t poor_strata_life_needs_nation = 0x0201;
		tf_poor_strata_life_needs_state,							// constexpr inline uint16_t poor_strata_life_needs_state = 0x0202;
		tf_poor_strata_life_needs_province,						// constexpr inline uint16_t poor_strata_life_needs_province = 0x0203;
		tf_poor_strata_life_needs_pop,								// constexpr inline uint16_t poor_strata_life_needs_pop = 0x0204;
		tf_poor_strata_everyday_needs_nation,					// constexpr inline uint16_t poor_strata_everyday_needs_nation = 0x0205;
		tf_poor_strata_everyday_needs_state,					// constexpr inline uint16_t poor_strata_everyday_needs_state = 0x0206;
		tf_poor_strata_everyday_needs_province,				// constexpr inline uint16_t poor_strata_everyday_needs_province = 0x0207;
		tf_poor_strata_everyday_needs_pop,						// constexpr inline uint16_t poor_strata_everyday_needs_pop = 0x0208;
		tf_poor_strata_luxury_needs_nation,						// constexpr inline uint16_t poor_strata_luxury_needs_nation = 0x0209;
		tf_poor_strata_luxury_needs_state,						// constexpr inline uint16_t poor_strata_luxury_needs_state = 0x020A;
		tf_poor_strata_luxury_needs_province,					// constexpr inline uint16_t poor_strata_luxury_needs_province = 0x020B;
		tf_poor_strata_luxury_needs_pop,							// constexpr inline uint16_t poor_strata_luxury_needs_pop = 0x020C;
		tf_diplomatic_influence_tag,									// constexpr inline uint16_t diplomatic_influence_tag = 0x020D;
		tf_diplomatic_influence_this_nation,					// constexpr inline uint16_t diplomatic_influence_this_nation = 0x020E;
		tf_diplomatic_influence_this_province,				// constexpr inline uint16_t diplomatic_influence_this_province = 0x020F;
		tf_diplomatic_influence_from_nation,					// constexpr inline uint16_t diplomatic_influence_from_nation = 0x0210;
		tf_diplomatic_influence_from_province,				// constexpr inline uint16_t diplomatic_influence_from_province = 0x0211;
		tf_pop_unemployment_nation,										// constexpr inline uint16_t pop_unemployment_nation = 0x0212;
		tf_pop_unemployment_state,										// constexpr inline uint16_t pop_unemployment_state = 0x0213;
		tf_pop_unemployment_province,									// constexpr inline uint16_t pop_unemployment_province = 0x0214;
		tf_pop_unemployment_pop,											// constexpr inline uint16_t pop_unemployment_pop = 0x0215;
		tf_pop_unemployment_nation_this_pop,					// constexpr inline uint16_t pop_unemployment_nation_this_pop = 0x0216;
		tf_pop_unemployment_state_this_pop,						// constexpr inline uint16_t pop_unemployment_state_this_pop = 0x0217;
		tf_pop_unemployment_province_this_pop,				// constexpr inline uint16_t pop_unemployment_province_this_pop = 0x0218;
		tf_relation_tag,															// constexpr inline uint16_t relation_tag = 0x0219;
		tf_relation_this_nation,											// constexpr inline uint16_t relation_this_nation = 0x021A;
		tf_relation_this_province,										// constexpr inline uint16_t relation_this_province = 0x021B;
		tf_relation_from_nation,											// constexpr inline uint16_t relation_from_nation = 0x021C;
		tf_relation_from_province,										// constexpr inline uint16_t relation_from_province = 0x021D;
		tf_check_variable,														// constexpr inline uint16_t check_variable = 0x021E;
		tf_upper_house,																// constexpr inline uint16_t upper_house = 0x021F;
		tf_unemployment_by_type_nation,								// constexpr inline uint16_t unemployment_by_type_nation = 0x0220;
		tf_unemployment_by_type_state,								// constexpr inline uint16_t unemployment_by_type_state = 0x0221;
		tf_unemployment_by_type_province,							// constexpr inline uint16_t unemployment_by_type_province = 0x0222;
		tf_unemployment_by_type_pop,									// constexpr inline uint16_t unemployment_by_type_pop = 0x0223;
		tf_party_loyalty_nation_province_id,					// constexpr inline uint16_t party_loyalty_nation_province_id = 0x0224;
		tf_party_loyalty_from_nation_province_id,			// constexpr inline uint16_t party_loyalty_from_nation_province_id = 0x0225;
		tf_party_loyalty_province_province_id,				// constexpr inline uint16_t party_loyalty_province_province_id = 0x0226;
		tf_party_loyalty_from_province_province_id,		// constexpr inline uint16_t party_loyalty_from_province_province_id = 0x0227;
		tf_party_loyalty_nation_from_province,				// constexpr inline uint16_t party_loyalty_nation_from_province = 0x0228;
		tf_party_loyalty_from_nation_scope_province,	// constexpr inline uint16_t party_loyalty_from_nation_scope_province = 0x0229;
		tf_can_build_in_province_railroad_no_limit_from_nation,		 // constexpr inline uint16_t
																															 // can_build_in_province_railroad_no_limit_from_nation = 0x022A;
		tf_can_build_in_province_railroad_yes_limit_from_nation,	 // constexpr inline uint16_t
																															 // can_build_in_province_railroad_yes_limit_from_nation = 0x022B;
		tf_can_build_in_province_railroad_no_limit_this_nation,		 // constexpr inline uint16_t
																															 // can_build_in_province_railroad_no_limit_this_nation = 0x022C;
		tf_can_build_in_province_railroad_yes_limit_this_nation,	 // constexpr inline uint16_t
																															 // can_build_in_province_railroad_yes_limit_this_nation = 0x022D;
		tf_can_build_in_province_fort_no_limit_from_nation,				 // constexpr inline uint16_t
																															 // can_build_in_province_fort_no_limit_from_nation = 0x022E;
		tf_can_build_in_province_fort_yes_limit_from_nation,			 // constexpr inline uint16_t
																															 // can_build_in_province_fort_yes_limit_from_nation = 0x022F;
		tf_can_build_in_province_fort_no_limit_this_nation,				 // constexpr inline uint16_t
																															 // can_build_in_province_fort_no_limit_this_nation = 0x0230;
		tf_can_build_in_province_fort_yes_limit_this_nation,			 // constexpr inline uint16_t
																															 // can_build_in_province_fort_yes_limit_this_nation = 0x0231;
		tf_can_build_in_province_naval_base_no_limit_from_nation,	 // constexpr inline uint16_t
																															 // can_build_in_province_naval_base_no_limit_from_nation = 0x0232;
		tf_can_build_in_province_naval_base_yes_limit_from_nation, // constexpr inline uint16_t
																															 // can_build_in_province_naval_base_yes_limit_from_nation = 0x0233;
		tf_can_build_in_province_naval_base_no_limit_this_nation,	 // constexpr inline uint16_t
																															 // can_build_in_province_naval_base_no_limit_this_nation = 0x0234;
		tf_can_build_in_province_naval_base_yes_limit_this_nation, // constexpr inline uint16_t
																															 // can_build_in_province_naval_base_yes_limit_this_nation = 0x0235;
		tf_can_build_railway_in_capital_yes_whole_state_yes_limit, // constexpr inline uint16_t
																															 // can_build_railway_in_capital_yes_whole_state_yes_limit = 0x0236;
		tf_can_build_railway_in_capital_yes_whole_state_no_limit,	 // constexpr inline uint16_t
																															 // can_build_railway_in_capital_yes_whole_state_no_limit = 0x0237;
		tf_can_build_railway_in_capital_no_whole_state_yes_limit,	 // constexpr inline uint16_t
																															 // can_build_railway_in_capital_no_whole_state_yes_limit = 0x0238;
		tf_can_build_railway_in_capital_no_whole_state_no_limit,	 // constexpr inline uint16_t
																															 // can_build_railway_in_capital_no_whole_state_no_limit = 0x0239;
		tf_can_build_fort_in_capital_yes_whole_state_yes_limit,		 // constexpr inline uint16_t
																															 // can_build_fort_in_capital_yes_whole_state_yes_limit = 0x023A;
		tf_can_build_fort_in_capital_yes_whole_state_no_limit,		 // constexpr inline uint16_t
																															 // can_build_fort_in_capital_yes_whole_state_no_limit = 0x023B;
		tf_can_build_fort_in_capital_no_whole_state_yes_limit,		 // constexpr inline uint16_t
																															 // can_build_fort_in_capital_no_whole_state_yes_limit = 0x023C;
		tf_can_build_fort_in_capital_no_whole_state_no_limit,			 // constexpr inline uint16_t
																															 // can_build_fort_in_capital_no_whole_state_no_limit = 0x023D;
		tf_work_available_nation,																	 // constexpr inline uint16_t work_available_nation = 0x023E;
		tf_work_available_state,																	 // constexpr inline uint16_t work_available_state = 0x023F;
		tf_work_available_province,																 // constexpr inline uint16_t work_available_province = 0x0240;
		tf_variable_ideology_name_nation,							 // constexpr inline uint16_t variable_ideology_name_nation = 0x0241;
		tf_variable_ideology_name_state,							 // constexpr inline uint16_t variable_ideology_name_state = 0x0242;
		tf_variable_ideology_name_province,						 // constexpr inline uint16_t variable_ideology_name_province = 0x0243;
		tf_variable_ideology_name_pop,								 // constexpr inline uint16_t variable_ideology_name_pop = 0x0244;
		tf_variable_issue_name_nation,								 // constexpr inline uint16_t variable_issue_name_nation = 0x0245;
		tf_variable_issue_name_state,									 // constexpr inline uint16_t variable_issue_name_state = 0x0246;
		tf_variable_issue_name_province,							 // constexpr inline uint16_t variable_issue_name_province = 0x0247;
		tf_variable_issue_name_pop,										 // constexpr inline uint16_t variable_issue_name_pop = 0x0248;
		tf_variable_issue_group_name_nation,					 // constexpr inline uint16_t variable_issue_group_name_nation = 0x0249;
		tf_variable_issue_group_name_state,						 // constexpr inline uint16_t variable_issue_group_name_state = 0x024A;
		tf_variable_issue_group_name_province,				 // constexpr inline uint16_t variable_issue_group_name_province = 0x024B;
		tf_variable_issue_group_name_pop,							 // constexpr inline uint16_t variable_issue_group_name_pop = 0x024C;
		tf_variable_pop_type_name_nation,							 // constexpr inline uint16_t variable_pop_type_name_nation = 0x024D;
		tf_variable_pop_type_name_state,							 // constexpr inline uint16_t variable_pop_type_name_state = 0x024E;
		tf_variable_pop_type_name_province,						 // constexpr inline uint16_t variable_pop_type_name_province = 0x024F;
		tf_variable_pop_type_name_pop,								 // constexpr inline uint16_t variable_pop_type_name_pop = 0x0250;
		tf_variable_good_name,												 // constexpr inline uint16_t variable_good_name = 0x0251;
		tf_strata_middle,															 // constexpr inline uint16_t strata_middle = 0x0252;
		tf_strata_poor,																 // constexpr inline uint16_t strata_poor = 0x0253;
		tf_party_loyalty_from_province_scope_province, // constexpr inline uint16_t party_loyalty_from_province_scope_province =
																									 // 0x0254;
		tf_can_build_factory_nation,									 // constexpr inline uint16_t can_build_factory_nation = 0x0255;
		tf_can_build_factory_province,								 // constexpr inline uint16_t can_build_factory_province = 0x0256;
		tf_nationalvalue_pop,													 // constexpr inline uint16_t nationalvalue_pop = 0x0257;
		tf_nationalvalue_province,										 // constexpr inline uint16_t nationalvalue_province = 0x0258;
		tf_war_exhaustion_pop,												 // constexpr inline uint16_t war_exhaustion_pop = 0x0259;
		tf_has_culture_core_province_this_pop,				 // constexpr inline uint16_t has_culture_core_province_this_pop = 0x025A;
		tf_tag_pop,																		 // constexpr inline uint16_t tag_pop = 0x025B;
		tf_has_country_flag,											 // constexpr inline uint16_t has_country_flag_pop = 0x025C;
		tf_has_country_flag,									 // constexpr inline uint16_t has_country_flag_province = 0x025D;
		tf_has_country_modifier_province,							 // constexpr inline uint16_t has_country_modifier_province = 0x025E;
		tf_religion_nation,														 // constexpr inline uint16_t religion_nation = 0x025F;
		tf_religion_nation_reb,												 // constexpr inline uint16_t religion_nation_reb = 0x0260;
		tf_religion_nation_from_nation,								 // constexpr inline uint16_t religion_nation_from_nation = 0x0261;
		tf_religion_nation_this_nation,								 // constexpr inline uint16_t religion_nation_this_nation = 0x0262;
		tf_religion_nation_this_state,								 // constexpr inline uint16_t religion_nation_this_state = 0x0263;
		tf_religion_nation_this_province,							 // constexpr inline uint16_t religion_nation_this_province = 0x0264;
		tf_religion_nation_this_pop,									 // constexpr inline uint16_t religion_nation_this_pop = 0x0265;
		tf_war_exhaustion_province,										 // constexpr inline uint16_t war_exhaustion_province = 0x0266;
		tf_is_greater_power_province,									 // constexpr inline uint16_t is_greater_power_province = 0x0267;
		tf_is_cultural_union_pop_this_pop,						 // constexpr inline uint16_t is_cultural_union_pop_this_pop = 0x0268;
		tf_has_building_factory,											 // constexpr inline uint16_t has_building_factory = 0x0269;
		tf_has_building_state_from_province,					 // constexpr inline uint16_t has_building_state_from_province = 0x026A;
		tf_has_building_factory_from_province,				 // constexpr inline uint16_t has_building_factory_from_province = 0x026B;
		tf_party_loyalty_generic,											 // constexpr inline uint16_t party_loyalty_generic = 0x026C;
		tf_invention,																	 // constexpr inline uint16_t invention = 0x026D;
		tf_political_movement_from_reb,								 // constexpr inline uint16_t political_movement_from_reb = 0x026E;
		tf_social_movement_from_reb,									 // constexpr inline uint16_t social_movement_from_reb = 0x026F;
		tf_is_next_rreform_nation,										 // constexpr inline uint16_t is_next_rreform_nation = 0x0270;
		tf_is_next_rreform_pop,												 // constexpr inline uint16_t is_next_rreform_pop = 0x0271;
		tf_variable_reform_group_name_nation,					 // constexpr inline uint16_t variable_reform_group_name_nation = 0x0272;
		tf_variable_reform_group_name_state,					 // constexpr inline uint16_t variable_reform_group_name_state = 0x0273;
		tf_variable_reform_group_name_province,				 // constexpr inline uint16_t variable_reform_group_name_province = 0x0274;
		tf_variable_reform_group_name_pop,						 // constexpr inline uint16_t variable_reform_group_name_pop = 0x0275;
		tf_is_disarmed_pop,  //constexpr inline uint16_t is_disarmed_pop = 0x0276;
		tf_owned_by_tag, //constexpr inline uint16_t owned_by_state_tag = 0x0277;
		tf_owned_by_from_nation, //constexpr inline uint16_t owned_by_state_from_nation = 0x0278;
		tf_owned_by_this_nation, //constexpr inline uint16_t owned_by_state_this_nation = 0x0279;
		tf_owned_by_this_province, //constexpr inline uint16_t owned_by_state_this_province = 0x027A;
		tf_owned_by_this_state, //constexpr inline uint16_t owned_by_state_this_state = 0x027B;
		tf_owned_by_this_pop, //constexpr inline uint16_t owned_by_state_this_pop = 0x027C;
		tf_units_in_province_tag, // constexpr inline uint16_t units_in_province_tag = 0x027D;
		tf_primary_culture_from_nation, //constexpr inline uint16_t primary_culture_from_nation = 0x027E;
		tf_primary_culture_from_province, //constexpr inline uint16_t primary_culture_from_province = 0x027F;
		tf_neighbour_this_province, //constexpr inline uint16_t neighbour_this_province = 0x0280;
		tf_neighbour_from_province, //constexpr inline uint16_t neighbour_from_province = 0x0281;
		tf_technology, //constexpr inline uint16_t technology_province = 0x0282;
		tf_invention, //constexpr inline uint16_t invention_province = 0x0283;
		tf_brigades_compare_province_this, //constexpr inline uint16_t brigades_compare_province_this = 0x0284;
		tf_brigades_compare_province_from, //constexpr inline uint16_t brigades_compare_province_from = 0x0285;
		tf_is_accepted_culture_nation_this_pop, //constexpr inline uint16_t is_accepted_culture_nation_this_pop = 0x0286;
		tf_is_accepted_culture_nation_this_nation, //constexpr inline uint16_t is_accepted_culture_nation_this_nation = 0x0287;
		tf_is_accepted_culture_nation_this_state, //constexpr inline uint16_t is_accepted_culture_nation_this_state = 0x0288;
		tf_is_accepted_culture_nation_this_province, //constexpr inline uint16_t is_accepted_culture_nation_this_province = 0x0289;
		tf_is_accepted_culture_state_this_pop, //constexpr inline uint16_t is_accepted_culture_state_this_pop = 0x028A;
		tf_is_accepted_culture_state_this_nation, //constexpr inline uint16_t is_accepted_culture_state_this_nation = 0x028B;
		tf_is_accepted_culture_state_this_state, //constexpr inline uint16_t is_accepted_culture_state_this_state = 0x028C;
		tf_is_accepted_culture_state_this_province, //constexpr inline uint16_t is_accepted_culture_state_this_province = 0x028D;
		tf_is_accepted_culture_province_this_pop, //constexpr inline uint16_t is_accepted_culture_province_this_pop = 0x028E;
		tf_is_accepted_culture_province_this_nation, //constexpr inline uint16_t is_accepted_culture_province_this_nation = 0x028F;
		tf_is_accepted_culture_province_this_state, //constexpr inline uint16_t is_accepted_culture_province_this_state = 0x0290;
		tf_is_accepted_culture_province_this_province, //constexpr inline uint16_t is_accepted_culture_province_this_province = 0x0291;
		tf_is_accepted_culture_pop_this_pop, //constexpr inline uint16_t is_accepted_culture_pop_this_pop = 0x0292;
		tf_is_accepted_culture_pop_this_nation, //constexpr inline uint16_t is_accepted_culture_pop_this_nation = 0x0293;
		tf_is_accepted_culture_pop_this_state, //constexpr inline uint16_t is_accepted_culture_pop_this_state = 0x0294;
		tf_is_accepted_culture_pop_this_province, //constexpr inline uint16_t is_accepted_culture_pop_this_province = 0x0295;
		tf_culture_group_province, //constexpr inline uint16_t culture_group_province = 0x0296;
		tf_culture_group_state, //constexpr inline uint16_t culture_group_state = 0x0297;
		tf_have_core_in_nation_tag, //constexpr inline uint16_t have_core_in_nation_tag = 0x0298;
		tf_have_core_in_nation_this, //constexpr inline uint16_t have_core_in_nation_this = 0x0299;
		tf_have_core_in_nation_from, //constexpr inline uint16_t have_core_in_nation_from = 0x029A;
		tf_owns, //constexpr inline uint16_t owns_province = 0x029B;
		tf_empty, //constexpr inline uint16_t empty_state = 0x029C;
		tf_is_overseas_pop, //constexpr inline uint16_t is_overseas_pop = 0x029D;
		tf_primary_culture_pop, //constexpr inline uint16_t primary_culture_pop = 0x029E;
		tf_plurality_pop, //constexpr inline uint16_t plurality_pop = 0x029F;
		tf_is_overseas, //constexpr inline uint16_t is_overseas_state = 0x02A0;
		tf_stronger_army_than_tag, //constexpr inline uint16_t stronger_army_than_tag = 0x02A1;
		tf_region, //constexpr inline uint16_t region_state = 0x02A2;
		tf_region_pop, //constexpr inline uint16_t region_pop = 0x02A3;
		tf_owns_region, //constexpr inline uint16_t owns_region = 0x02A4;
		tf_is_core_tag, //constexpr inline uint16_t is_core_state_tag = 0x02A5;
		tf_country_units_in_state_from, //constexpr inline uint16_t country_units_in_state_from = 0x02A6;
		tf_country_units_in_state_this_nation, //constexpr inline uint16_t country_units_in_state_this_nation = 0x02A7;
		tf_country_units_in_state_this_province, //constexpr inline uint16_t country_units_in_state_this_province = 0x02A8;
		tf_country_units_in_state_this_state, //constexpr inline uint16_t country_units_in_state_this_state = 0x02A9;
		tf_country_units_in_state_this_pop, //constexpr inline uint16_t country_units_in_state_this_pop = 0x02AA;
		tf_country_units_in_state_tag, //constexpr inline uint16_t country_units_in_state_tag = 0x02AB;
		tf_stronger_army_than_this_nation, //constexpr inline uint16_t stronger_army_than_this_nation = 0x02AC;
		tf_stronger_army_than_this_state, //constexpr inline uint16_t stronger_army_than_this_state = 0x02AD;
		tf_stronger_army_than_this_province, //constexpr inline uint16_t stronger_army_than_this_province = 0x02AE;
		tf_stronger_army_than_this_pop, //constexpr inline uint16_t stronger_army_than_this_pop = 0x02AF;
		tf_stronger_army_than_from_nation, //constexpr inline uint16_t stronger_army_than_from_nation = 0x02B0;
		tf_stronger_army_than_from_province, //constexpr inline uint16_t stronger_army_than_from_province = 0x02B1;
		tf_flashpoint_tension, //constexpr inline uint16_t flashpoint_tension_province = 0x02B2;
		tf_is_colonial_pop, //constexpr inline uint16_t is_colonial_pop = 0x02B3;
		tf_has_country_flag,	 // constexpr inline uint16_t has_country_flag_state = 0x02B4;
		tf_rich_tax, //constexpr inline uint16_t rich_tax_pop = 0x02B5;
		tf_middle_tax, //constexpr inline uint16_t middle_tax_pop = 0x02B6;
		tf_poor_tax, //constexpr inline uint16_t poor_tax_pop = 0x02B7;
		tf_is_core_pop_tag, //constexpr inline uint16_t is_core_pop_tag = 0x02B8;
		tf_is_core_boolean, //constexpr inline uint16_t is_core_boolean = 0x02B9;
		tf_is_core_this_nation, //constexpr inline uint16_t is_core_state_this_nation = 0x02BA;
		tf_is_core_this_province, //constexpr inline uint16_t is_core_state_this_province = 0x02BB;
		tf_is_core_this_pop, //constexpr inline uint16_t is_core_state_this_pop = 0x02BC;
		tf_is_core_from_nation, //constexpr inline uint16_t is_core_state_from_nation = 0x02BD;
		tf_ruling_party_ideology_nation, //constexpr inline uint16_t ruling_party_ideology_province = 0x02BE;
		tf_money_province, //constexpr inline uint16_t money_province = 0x02BF;
		tf_is_our_vassal_province_tag, //constexpr inline uint16_t is_our_vassal_province_tag = 0x02C0;
		tf_is_our_vassal_province_from, //constexpr inline uint16_t is_our_vassal_province_from = 0x02C1;
		tf_is_our_vassal_province_this_nation, //constexpr inline uint16_t is_our_vassal_province_this_nation = 0x02C2;
		tf_is_our_vassal_province_this_province, //constexpr inline uint16_t is_our_vassal_province_this_province = 0x02C3;
		tf_is_our_vassal_province_this_state, //constexpr inline uint16_t is_our_vassal_province_this_state = 0x02C4;
		tf_is_our_vassal_province_this_pop, //constexpr inline uint16_t is_our_vassal_province_this_pop = 0x02C5;
		tf_vassal_of_province_tag, //constexpr inline uint16_t vassal_of_province_tag = 0x02C6;
		tf_vassal_of_province_from, //constexpr inline uint16_t vassal_of_province_from = 0x02C7;
		tf_vassal_of_province_this_nation, //constexpr inline uint16_t vassal_of_province_this_nation = 0x02C8;
		tf_vassal_of_province_this_province, //constexpr inline uint16_t vassal_of_province_this_province = 0x02C9;
		tf_vassal_of_province_this_state, //constexpr inline uint16_t vassal_of_province_this_state = 0x02CA;
		tf_vassal_of_province_this_pop, //constexpr inline uint16_t vassal_of_province_this_pop = 0x02CB;
		tf_relation_this_pop, //constexpr inline uint16_t relation_this_pop = 0x02CC;
		tf_has_recently_lost_war, //constexpr inline uint16_t has_recently_lost_war_pop = 0x02CD;
		tf_technology, //constexpr inline uint16_t technology_pop = 0x02CE;
		tf_invention, //constexpr inline uint16_t invention_pop = 0x02CF;
		tf_in_default_bool, //constexpr inline uint16_t in_default_bool = 0x02D0;
		tf_is_state_capital_pop, //constexpr inline uint16_t is_state_capital_pop = 0x02D1;
		tf_region_proper, //constexpr inline uint16_t region_proper = 0x02D2;
		tf_region_proper, //constexpr inline uint16_t region_proper_state = 0x02D3;
		tf_region_proper_pop, //constexpr inline uint16_t region_proper_pop = 0x02D4;
		tf_owns_region_proper, //constexpr inline uint16_t owns_region_proper = 0x02D5;
		tf_pop_majority_religion_nation_this_nation, //constexpr inline uint16_t pop_majority_religion_nation_this_nation = 0x02D6;
		tf_military_score_tag, //constexpr inline uint16_t military_score_tag = 0x02D7;
		tf_industrial_score_tag, //constexpr inline uint16_t industrial_score_tag = 0x02D8;
		tf_has_factories_nation, // constexpr inline uint16_t has_factories_state = 0x02D9;
		tf_is_coastal_state, // constexpr inline uint16_t is_coastal_state = 0x02DA;

		//
		// scopes
		//

		tf_generic_scope,										// constexpr uint16_t generic_scope = 0x0000; // or & and
		tf_x_neighbor_province_scope,				// constexpr uint16_t x_neighbor_province_scope = 0x0001;
		tf_x_neighbor_country_scope_nation, // constexpr uint16_t x_neighbor_country_scope_nation = 0x0002;
		tf_x_neighbor_country_scope_pop,		// constexpr uint16_t x_neighbor_country_scope_pop = 0x0003;
		tf_x_war_countries_scope_nation,		// constexpr uint16_t x_war_countries_scope_nation = 0x0004;
		tf_x_war_countries_scope_pop,				// constexpr uint16_t x_war_countries_scope_pop = 0x0005;
		tf_x_greater_power_scope,						// constexpr uint16_t x_greater_power_scope = 0x0006;
		tf_x_owned_province_scope_state,		// constexpr uint16_t x_owned_province_scope_state = 0x0007;
		tf_x_owned_province_scope_nation,		// constexpr uint16_t x_owned_province_scope_nation = 0x0008;
		tf_x_core_scope_province,						// constexpr uint16_t x_core_scope_province = 0x0009;
		tf_x_core_scope_nation,							// constexpr uint16_t x_core_scope_nation = 0x000A;
		tf_x_state_scope,										// constexpr uint16_t x_state_scope = 0x000B;
		tf_x_substate_scope,								// constexpr uint16_t x_substate_scope = 0x000C;
		tf_x_sphere_member_scope,						// constexpr uint16_t x_sphere_member_scope = 0x000D;
		tf_x_pop_scope_province,						// constexpr uint16_t x_pop_scope_province = 0x000E;
		tf_x_pop_scope_state,								// constexpr uint16_t x_pop_scope_state = 0x000F;
		tf_x_pop_scope_nation,							// constexpr uint16_t x_pop_scope_nation = 0x0010;
		tf_x_provinces_in_variable_region,	// constexpr uint16_t x_provinces_in_variable_region = 0x0011; // variable name
		tf_owner_scope_state,								// constexpr uint16_t owner_scope_state = 0x0012;
		tf_owner_scope_province,						// constexpr uint16_t owner_scope_province = 0x0013;
		tf_controller_scope,								// constexpr uint16_t controller_scope = 0x0014;
		tf_location_scope,									// constexpr uint16_t location_scope = 0x0015;
		tf_country_scope_state,							// constexpr uint16_t country_scope_state = 0x0016;
		tf_country_scope_pop,								// constexpr uint16_t country_scope_pop = 0x0017;
		tf_capital_scope,										// constexpr uint16_t capital_scope = 0x0018;
		tf_this_scope_pop,									// constexpr uint16_t this_scope_pop = 0x0019;
		tf_this_scope_nation,								// constexpr uint16_t this_scope_nation = 0x001A;
		tf_this_scope_state,								// constexpr uint16_t this_scope_state = 0x001B;
		tf_this_scope_province,							// constexpr uint16_t this_scope_province = 0x001C;
		tf_from_scope_pop,									// constexpr uint16_t from_scope_pop = 0x001D;
		tf_from_scope_nation,								// constexpr uint16_t from_scope_nation = 0x001E;
		tf_from_scope_state,								// constexpr uint16_t from_scope_state = 0x001F;
		tf_from_scope_province,							// constexpr uint16_t from_scope_province = 0x0020;
		tf_sea_zone_scope,									// constexpr uint16_t sea_zone_scope = 0x0021;
		tf_cultural_union_scope,						// constexpr uint16_t cultural_union_scope = 0x0022;
		tf_overlord_scope,									// constexpr uint16_t overlord_scope = 0x0023;
		tf_sphere_owner_scope,							// constexpr uint16_t sphere_owner_scope = 0x0024;
		tf_independence_scope,							// constexpr uint16_t independence_scope = 0x0025;
		tf_flashpoint_tag_scope,						// constexpr uint16_t flashpoint_tag_scope = 0x0026;
		tf_crisis_state_scope,							// constexpr uint16_t crisis_state_scope = 0x0027;
		tf_state_scope_pop,									// constexpr uint16_t state_scope_pop = 0x0028;
		tf_state_scope_province,						// constexpr uint16_t state_scope_province = 0x0029;
		tf_tag_scope,												// constexpr uint16_t tag_scope = 0x002A; // variable name
		tf_integer_scope,										// constexpr uint16_t integer_scope = 0x002B; // variable name
		tf_country_scope_nation,						// constexpr uint16_t country_scope_nation = 0x002C;
		tf_country_scope_province,					// constexpr uint16_t country_scope_province = 0x002D;
		tf_cultural_union_scope_pop,				// constexpr uint16_t cultural_union_scope_pop = 0x002E;
		tf_capital_scope_province,    // constexpr uint16_t capital_scope_province = 0x002F;
		tf_capital_scope_pop,   //constexpr inline uint16_t capital_scope_pop = first_scope_code + 0x0030;
		tf_x_country_scope, //constexpr inline uint16_t x_country_scope = first_scope_code + 0x0031;
		tf_x_neighbor_province_scope_state, //constexpr inline uint16_t x_neighbor_province_scope_state = first_scope_code + 0x0032;
		tf_x_provinces_in_variable_region_proper, //constexpr inline uint16_t x_provinces_in_variable_region_proper = first_scope_code + 0x0033;
};

// #define TRIGGER_DISPLAY_PARAMS uint16_t const* tval, sys::state& ws, text::columnar_layout& layout,
//			int32_t primary_slot, int32_t this_slot, int32_t from_slot, int32_t indentation, bool show_condition

void make_trigger_description(sys::state& ws, text::layout_base& layout, uint16_t const* tval, int32_t primary_slot,
		int32_t this_slot, int32_t from_slot, int32_t indentation, bool show_condition) {
	trigger_functions[*tval & trigger::code_mask](tval, ws, layout, primary_slot, this_slot, from_slot, indentation,
			show_condition);
}

#undef TRIGGER_DISPLAY_PARAMS

} // namespace trigger_tooltip

void trigger_description(sys::state& state, text::layout_base& layout, dcon::trigger_key k, int32_t primary_slot,
		int32_t this_slot, int32_t from_slot) {
	if(!k)
		return;

	trigger_tooltip::make_trigger_description(state, layout, state.trigger_data.data() + state.trigger_data_indices[k.index() + 1],
			primary_slot, this_slot, from_slot, 0, true);
}

void multiplicative_value_modifier_description(sys::state& state, text::layout_base& layout, dcon::value_modifier_key modifier,
		int32_t primary_slot, int32_t this_slot, int32_t from_slot) {
	auto base = state.value_modifiers[modifier];

	{
		text::substitution_map map{};
		text::add_to_substitution_map(map, text::variable_type::val,
				text::fp_two_places{trigger::evaluate_multiplicative_modifier(state, modifier, primary_slot, this_slot, from_slot)});
		auto box = text::open_layout_box(layout, 0);
		text::localised_format_box(state, layout, box, std::string_view("value_mod_product"), map);
		text::close_layout_box(layout, box);
	}

	if(base.factor != 0.0f) {
		text::substitution_map map{};
		text::add_to_substitution_map(map, text::variable_type::val, text::fp_two_places{base.factor});
		auto box = text::open_layout_box(layout, trigger_tooltip::indentation_amount);
		text::localised_format_box(state, layout, box, std::string_view("comwid_base"), map);
		text::close_layout_box(layout, box);
	}

	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			auto box = text::open_layout_box(layout, trigger_tooltip::indentation_amount);

			if(trigger::evaluate(state, seg.condition, primary_slot, this_slot, from_slot)) {
				text::add_to_layout_box(state, layout, box, std::string_view("\x02"), text::text_color::green);
				text::add_space_to_layout_box(state, layout, box);
			} else {
				text::add_to_layout_box(state, layout, box, std::string_view("\x01"), text::text_color::red);
				text::add_space_to_layout_box(state, layout, box);
			}

			text::add_to_layout_box(state, layout, box, text::fp_two_places{seg.factor},
					seg.factor >= 0.f ? text::text_color::green : text::text_color::red);
			text::close_layout_box(layout, box);

			trigger_tooltip::make_trigger_description(state, layout,
					state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], primary_slot, this_slot, from_slot,
					trigger_tooltip::indentation_amount * 2, true);
		}
	}
}

void additive_value_modifier_description(sys::state& state, text::layout_base& layout, dcon::value_modifier_key modifier,
		int32_t primary_slot, int32_t this_slot, int32_t from_slot) {
	auto base = state.value_modifiers[modifier];

	if(base.factor == 1.0f) {
		text::substitution_map map{};
		text::add_to_substitution_map(map, text::variable_type::val,
				text::fp_two_places{trigger::evaluate_additive_modifier(state, modifier, primary_slot, this_slot, from_slot)});
		auto box = text::open_layout_box(layout, 0);
		text::localised_format_box(state, layout, box, std::string_view("value_mod_sum"), map);
		text::close_layout_box(layout, box);
	} else {
		text::substitution_map map{};
		text::add_to_substitution_map(map, text::variable_type::val,
				text::fp_two_places{ trigger::evaluate_additive_modifier(state, modifier, primary_slot, this_slot, from_slot) });
		text::add_to_substitution_map(map, text::variable_type::x,
				text::fp_two_places{ base.factor });
		auto box = text::open_layout_box(layout, 0);
		text::localised_format_box(state, layout, box, std::string_view("value_mod_sum_factor"), map);
		text::close_layout_box(layout, box);
	}

	{
		text::substitution_map map{};
		text::add_to_substitution_map(map, text::variable_type::val, text::fp_two_places{base.base});
		auto box = text::open_layout_box(layout, trigger_tooltip::indentation_amount);
		text::localised_format_box(state, layout, box, std::string_view("comwid_base"), map);
		text::close_layout_box(layout, box);
	}

	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			auto box = text::open_layout_box(layout, trigger_tooltip::indentation_amount);

			if(trigger::evaluate(state, seg.condition, primary_slot, this_slot, from_slot)) {
				text::add_to_layout_box(state, layout, box, std::string_view("\x02"), text::text_color::green);
				text::add_space_to_layout_box(state, layout, box);
			} else {
				text::add_to_layout_box(state, layout, box, std::string_view("\x01"), text::text_color::red);
				text::add_space_to_layout_box(state, layout, box);
			}

			text::add_to_layout_box(state, layout, box, text::fp_two_places{seg.factor},
					seg.factor >= 0.f ? text::text_color::green : text::text_color::red);
			text::close_layout_box(layout, box);

			trigger_tooltip::make_trigger_description(state, layout,
					state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], primary_slot, this_slot, from_slot,
					trigger_tooltip::indentation_amount * 2, true);
		}
	}
}

} // namespace ui
