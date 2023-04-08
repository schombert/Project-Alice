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
		case trigger::association_eq: return "cmp_eq";
		case trigger::association_le: return "cmp_le";
		case trigger::association_lt: return "cmp_lt";
		case trigger::association_ge: return "cmp_ge";
		case trigger::association_gt: return "cmp_gt";
		case trigger::association_ne: return "cmp_ne";
	}
	return "cmp_eq";
}

inline std::string_view cmp_has_code_to_fixed_ui(uint16_t code) {
	switch(code & trigger::association_mask) {
		case trigger::association_eq: return "cmp_has_eq";
		case trigger::association_le: return "cmp_has_le";
		case trigger::association_lt: return "cmp_has_lt";
		case trigger::association_ge: return "cmp_has_ge";
		case trigger::association_gt: return "cmp_has_gt";
		case trigger::association_ne: return "cmp_has_ne";
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

void display_with_comparison(
	uint16_t trigger_code,
	std::string_view left_label,
	text::substitution value,
	sys::state& ws,
	text::columnar_layout& layout,
	text::layout_box& box) {


	text::add_to_layout_box(layout, ws, box, left_label, text::text_color::white);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, cmp_code_to_fixed_ui(trigger_code)), text::text_color::white);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, value, text::text_color::white);
}

void display_with_has_comparison(
	uint16_t trigger_code,
	std::string_view left_label,
	text::substitution value,
	sys::state& ws,
	text::columnar_layout& layout,
	text::layout_box& box) {

	text::add_to_layout_box(layout, ws, box, left_label, text::text_color::white);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, cmp_has_code_to_fixed_ui(trigger_code)), text::text_color::white);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, value, text::text_color::white);
}

void display_with_comparison(
	uint16_t trigger_code,
	std::string_view value,
	sys::state& ws,
	text::columnar_layout& layout,
	text::layout_box& box) {

	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, cmp_code_to_fixed_ui(trigger_code)), text::text_color::white);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, value, text::text_color::white);
}

void display_with_has_comparison(
	uint16_t trigger_code,
	std::string_view value,
	sys::state& ws,
	text::columnar_layout& layout,
	text::layout_box& box) {

	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, cmp_has_code_to_fixed_ui(trigger_code)), text::text_color::white);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, value, text::text_color::white);
}

void make_trigger_description(
	sys::state& ws,
	text::columnar_layout& layout,
	uint16_t const* tval,
	int32_t primary_slot,
	int32_t this_slot,
	int32_t from_slot,
	int32_t indentation,
	bool show_condition
);

inline constexpr int32_t indentation_amount = 10;

inline void display_subtriggers(
	uint16_t const* source,
	sys::state& ws,
	text::columnar_layout& layout,
	int32_t primary_slot,
	int32_t this_slot,
	int32_t from_slot,
	int32_t indentation,
	bool show_condition) {

	const auto source_size = 1 + trigger::get_trigger_payload_size(source);
	auto sub_units_start = source + 2 + trigger::trigger_scope_data_payload(source[0]);
	while(sub_units_start < source + source_size) {
		make_trigger_description(ws, layout,
			sub_units_start, primary_slot, this_slot, from_slot, indentation, show_condition);
		sub_units_start += 1 + trigger::get_trigger_payload_size(sub_units_start);
	}
}

#define TRIGGER_DISPLAY_PARAMS uint16_t const* tval, sys::state& ws, text::columnar_layout& layout, \
			int32_t primary_slot, int32_t this_slot, int32_t from_slot, int32_t indentation, bool show_condition

/*
* 
void tf_none(TRIGGER_DISPLAY_PARAMS) {
}


void make_condition(TRIGGER_DISPLAY_PARAMS, text::layout_box& box) {
	if(show_condition) {
		//evaluate_trigger(sys::state& state, uint16_t const* data, int32_t primary, int32_t this_slot, int32_t from_slot)
		if(trigger::evaluate_trigger(ws, tval, primary_slot, this_slot, from_slot)) {
			text::add_to_layout_box(layout, ws, box, std::string_view("("), text::text_color::white);
			text::add_to_layout_box(layout, ws, box, std::string_view("*"), text::text_color::green);
			text::add_to_layout_box(layout, ws, box, std::string_view(")"), text::text_color::white);
			text::add_space_to_layout_box(layout, ws, box);
		} else {
			text::add_to_layout_box(layout, ws, box, std::string_view("("), text::text_color::white);
			text::add_to_layout_box(layout, ws, box, std::string_view("x"), text::text_color::red);
			text::add_to_layout_box(layout, ws, box, std::string_view(")"), text::text_color::white);
			text::add_space_to_layout_box(layout, ws, box);
		}
	}
}

void tf_generic_scope(TRIGGER_DISPLAY_PARAMS) {
	auto st_count = trigger::count_subtriggers(tval);
	if(st_count > 1) {
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}
	display_subtriggers(tval, ws, layout, primary_slot, this_slot, from_slot, indentation + (st_count > 1 ? indentation_amount : 0), show_condition);
}
void tf_x_neighbor_province_scope(TRIGGER_DISPLAY_PARAMS) {

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "neighboring_province"));
		text::close_layout_box(layout, box);
	}

	auto st_count = trigger::count_subtriggers(tval);
	if(st_count > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	 display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_neighbor_country_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "neighboring_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_neighbor_country_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "neighboring_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_war_countries_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "nation_at_war_against"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_war_countries_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "nation_at_war_against"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_greater_power_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "great_power"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_owned_province_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "owned_province"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(primary_slot)) : text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_owned_province_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "owned_province"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_core_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "core_in"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot))) : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_core_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "core_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_state_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "state_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_substate_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "substate_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_sphere_member_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "nation_in_sphere"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_pop_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_pop_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_pop_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_x_provinces_in_variable_region(TRIGGER_DISPLAY_PARAMS) {
	auto region = trigger::payload(*(tval + 2)).state_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, every_any_code_to_fixed_ui(tval[0])));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "province_in"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, ws.world.state_definition_get_name(region)));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
}
void tf_owner_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(primary_slot)) : text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		primary_slot != -1 ? trigger::to_generic(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot))) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_owner_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot))) : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		primary_slot != -1 ? trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))) : -1, this_slot, from_slot, indentation + indentation_amount,
		show_condition && primary_slot != -1);
}
void tf_controller_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "controller_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot))) : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		primary_slot != -1 ? trigger::to_generic(ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot))) : -1, this_slot, from_slot, indentation + indentation_amount,
		show_condition && primary_slot != -1);
}
void tf_location_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "location_of_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto loc = primary_slot != -1 ? trigger::to_generic(ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot))) : -1;
	display_subtriggers(tval, ws, layout,
		 loc, this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_country_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(primary_slot)) : text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		primary_slot != -1 ? trigger::to_generic(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot))) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_country_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "nation_of_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		primary_slot != -1 ? trigger::to_generic(nations::owner_of_pop(ws, trigger::to_pop(primary_slot))) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_capital_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "capital_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto loc = primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_capital(trigger::to_nation(primary_slot))) : -1;
	display_subtriggers(tval, ws, layout,
		loc, this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_this_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(this_slot))) : text::produce_simple_string(ws, "this_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		this_slot, this_slot, from_slot, indentation + indentation_amount, this_slot != -1);
}
void tf_this_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, this_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(this_slot)) : text::produce_simple_string(ws, "this_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		this_slot, this_slot, from_slot, indentation + indentation_amount, this_slot != -1);
}
void tf_this_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, this_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(this_slot))) : text::produce_simple_string(ws, "this_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		this_slot, this_slot, from_slot, indentation + indentation_amount, this_slot != -1);
}
void tf_this_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		this_slot, this_slot, from_slot, indentation + indentation_amount, this_slot != -1);
}
void tf_from_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, from_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(from_slot))) : text::produce_simple_string(ws, "from_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		from_slot, this_slot, from_slot, indentation + indentation_amount, from_slot != -1);
}
void tf_from_scope_state(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, from_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(from_slot)) : text::produce_simple_string(ws, "from_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		from_slot, this_slot, from_slot, indentation + indentation_amount, from_slot != -1);
}
void tf_from_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, from_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(from_slot))) : text::produce_simple_string(ws, "from_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, from_slot, this_slot, from_slot, indentation + indentation_amount, from_slot != -1);
}
void tf_from_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout, from_slot, this_slot, from_slot, indentation + indentation_amount, from_slot != -1);
}
void tf_sea_zone_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "adjacent_sea"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
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
	}(primary_slot) : dcon::province_id{};

	if(sea_zone) {
		display_subtriggers(tval, ws, layout, trigger::to_generic(sea_zone), this_slot, from_slot, indentation + indentation_amount, show_condition);
	} else {
		display_subtriggers(tval, ws, layout, -1, this_slot, from_slot, indentation + indentation_amount, false);
	}
}
void tf_cultural_union_scope(TRIGGER_DISPLAY_PARAMS) {
	auto prim_culture = primary_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "cultural_union_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto union_tag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holder = ws.world.national_identity_get_nation_from_identity_holder(union_tag);

	display_subtriggers(tval, ws, layout, trigger::to_generic(group_holder), this_slot, from_slot, indentation + indentation_amount, show_condition);
}
void tf_overlord_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "overlord_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto olr = ws.world.nation_get_overlord_as_subject(trigger::to_nation(primary_slot));
	display_subtriggers(tval, ws, layout, trigger::to_generic(ws.world.overlord_get_ruler(olr)), this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_sphere_owner_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "sphere_leader_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot))) : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_in_sphere_of(
		(trigger::to_nation(primary_slot)))) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && primary_slot != -1);
}
void tf_independence_scope(TRIGGER_DISPLAY_PARAMS) {
	auto rtag = from_slot != -1 ? ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot)) : dcon::national_identity_id{};
	auto r_holder = ws.world.national_identity_get_nation_from_identity_holder(rtag);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "reb_independence_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		r_holder ? trigger::to_generic(r_holder) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && bool(r_holder));
}
void tf_flashpoint_tag_scope(TRIGGER_DISPLAY_PARAMS) {
	auto ctag = ws.world.state_instance_get_flashpoint_tag(trigger::to_state(primary_slot));
	auto fp_nation = ws.world.national_identity_get_nation_from_identity_holder(ctag);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "flashpoint_nation"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		fp_nation ? trigger::to_generic(fp_nation) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && bool(fp_nation));
}
void tf_crisis_state_scope(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "crisis_state"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto cstate = ws.crisis_state ? trigger::to_generic(ws.crisis_state) : -1;
	display_subtriggers(tval, ws, layout,
		cstate, this_slot, from_slot, indentation + indentation_amount, show_condition && cstate != -1);
}
void tf_state_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "containing_state"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot))) : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto st = primary_slot != -1 ? ws.world.province_get_state_membership(trigger::to_prov(primary_slot)) : dcon::state_instance_id{};

	display_subtriggers(tval, ws, layout, st ? trigger::to_generic(st) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && st);
}
void tf_state_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "containing_state"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "singular_pop"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	auto st = primary_slot != -1 ? ws.world.province_get_state_membership(ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot))) : dcon::state_instance_id{};

	display_subtriggers(tval, ws, layout,
		st ? trigger::to_generic(st) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && st);
}
void tf_tag_scope(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[2]).tag_id;
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, tag_holder ? ws.world.nation_get_name(tag_holder) : ws.world.national_identity_get_name(tag)));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		tag_holder ? trigger::to_generic(tag_holder) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && tag_holder);

}
void tf_integer_scope(TRIGGER_DISPLAY_PARAMS) {
	auto p = trigger::payload(tval[2]).prov_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, ws.world.province_get_name(p)));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		trigger::to_generic(p), this_slot, from_slot, indentation + indentation_amount, show_condition);
}
void tf_country_scope_nation(TRIGGER_DISPLAY_PARAMS) {
	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}
	display_subtriggers(tval, ws, layout, primary_slot, this_slot, from_slot, indentation + indentation_amount, show_condition);
}
void tf_country_scope_province(TRIGGER_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot))) : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	display_subtriggers(tval, ws, layout,
		primary_slot != -1 ? trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition&& primary_slot != -1);
}
void tf_cultural_union_scope_pop(TRIGGER_DISPLAY_PARAMS) {
	auto cultures = primary_slot != -1 ? ws.world.pop_get_culture(trigger::to_pop(primary_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(cultures);
	auto union_tags = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holders = ws.world.national_identity_get_nation_from_identity_holder(union_tags);

	{
		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "cultural_union_of"));
		text::add_space_to_layout_box(layout, ws, box);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "singular_pop"));
		text::close_layout_box(layout, box);
	}
	if(trigger::count_subtriggers(tval) > 1) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, any_all_code_to_fixed_ui(tval[0])));
		text::close_layout_box(layout, box);
	}

	
	display_subtriggers(tval, ws, layout,group_holders ? trigger::to_generic(group_holders) : -1, this_slot, from_slot, indentation + indentation_amount, show_condition && bool(group_holders));
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
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, tech, text::substitution_map{});
	text::close_layout_box(layout, box);
}
void tf_invention(TRIGGER_DISPLAY_PARAMS) {
	auto tech = trigger::payload(tval[1]).invt_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "invention"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, tech, text::substitution_map{});
	text::close_layout_box(layout, box);
}
void tf_strata_rich(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "strata"), text::produce_simple_string(ws, "rich"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_strata_middle(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "strata"), text::produce_simple_string(ws, "middle"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_strata_poor(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "strata"), text::produce_simple_string(ws, "poor"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_life_rating_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "life_rating"), int32_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_life_rating_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "minimum_state_life_rating"), int32_t(trigger::payload(tval[1]).signed_value), ws, layout, box);
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
	auto it = ws.key_to_text_sequence.find((tval[0] & trigger::association_mask) == trigger::association_eq ? "belongs_to_same_state" : "does_not_belong_to_same_state");
	if(it != ws.key_to_text_sequence.end()) {
		text::substitution_map map;
		text::add_to_substitution_map(map, text::variable_type::text, trigger::payload(tval[1]).prov_id);

		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, it->second, map);
		text::close_layout_box(layout, box);
	}
}
void tf_state_id_state(TRIGGER_DISPLAY_PARAMS) {
	auto it = ws.key_to_text_sequence.find((tval[0] & trigger::association_mask) == trigger::association_eq ? "contains_blank" : "does_not_contain_blank");
	if(it != ws.key_to_text_sequence.end()) {
		text::substitution_map map;
		text::add_to_substitution_map(map, text::variable_type::text, trigger::payload(tval[1]).prov_id);

		auto box = text::open_layout_box(layout, indentation);
		make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
		text::add_to_layout_box(layout, ws, box, it->second, map);
		text::close_layout_box(layout, box);
	}
}
void tf_cash_reserves(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "pop_savings"), text::fp_three_places{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "times_target_needs_spending"));
	text::close_layout_box(layout, box);
}
void tf_unemployment_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_unemployment_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_slave_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"), text::produce_simple_string(ws, "allowed"), ws, layout, box);
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, "government"), text::produce_simple_string(ws, ws.culture_definitions.governments[gov].name), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_government_pop(TRIGGER_DISPLAY_PARAMS) {
	auto gov = trigger::payload(tval[1]).gov_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "government"), text::produce_simple_string(ws, ws.culture_definitions.governments[gov].name), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_capital(TRIGGER_DISPLAY_PARAMS) {
	auto prov = trigger::payload(tval[1]).prov_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "capital"), text::produce_simple_string(ws, ws.world.province_get_name(prov)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_tech_school(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "technology_school"), text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_primary_culture(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_accepted_culture(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(t)), text::produce_simple_string(ws, "an_accepted_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
}
void tf_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, ws.world.culture_get_name(t)), ws, layout, box);
}
void tf_culture_nation(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).cul_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.culture_get_name(t)), text::produce_simple_string(ws, "a_primary_or_accepted"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_pop_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), rc ? text::produce_simple_string(ws, ws.world.culture_get_name(rc)) : text::produce_simple_string(ws, "rebel_culture"), ws, layout, box);
}
void tf_culture_state_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), rc ? text::produce_simple_string(ws, ws.world.culture_get_name(rc)) : text::produce_simple_string(ws, "rebel_culture"), ws, layout, box);
}
void tf_culture_province_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), rc ? text::produce_simple_string(ws, ws.world.culture_get_name(rc)) : text::produce_simple_string(ws, "rebel_culture"), ws, layout, box);
}
void tf_culture_nation_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], rc ? text::produce_simple_string(ws, ws.world.culture_get_name(rc)) : text::produce_simple_string(ws, "rebel_culture"), text::produce_simple_string(ws, "a_primary_or_accepted"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "this_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "from_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "this_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "this_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "this_nation_prim_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation(TRIGGER_DISPLAY_PARAMS) {
	auto cg = trigger::payload(tval[1]).culgrp_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)), ws, layout, box);
}
void tf_culture_group_pop(TRIGGER_DISPLAY_PARAMS) {
	auto cg = trigger::payload(tval[1]).culgrp_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)), ws, layout, box);
}
void tf_culture_group_reb_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};
	auto rg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(rg)) : text::produce_simple_string(ws, "rebel_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_reb_pop(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_primary_culture(trigger::to_rebel(from_slot)) : dcon::culture_id{};
	auto rg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(rg)) : text::produce_simple_string(ws, "rebel_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "this_nation_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "this_nation_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(from_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "from_nation_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(from_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "from_nation_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "this_province_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "this_province_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "this_state_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "this_state_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(nations::owner_of_pop(ws, trigger::to_pop(this_slot))) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "this_pop_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_culture_group_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_primary_culture(nations::owner_of_pop(ws, trigger::to_pop(this_slot))) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(rc);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture_group"), rg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg)) : text::produce_simple_string(ws, "this_pop_culture_group"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).rel_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"), text::produce_simple_string(ws, ws.world.religion_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.rebel_faction_get_religion(trigger::to_rebel(from_slot)) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"), rc ? text::produce_simple_string(ws, ws.world.religion_get_name(t)) : text::produce_simple_string(ws, "rebel_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = from_slot != -1 ? ws.world.nation_get_religion(trigger::to_nation(from_slot)) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"), rc ? text::produce_simple_string(ws, ws.world.religion_get_name(t)) : text::produce_simple_string(ws, "from_nation_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_religion(trigger::to_nation(this_slot)) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"), rc ? text::produce_simple_string(ws, ws.world.religion_get_name(t)) : text::produce_simple_string(ws, "this_nation_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_religion(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"), rc ? text::produce_simple_string(ws, ws.world.religion_get_name(t)) : text::produce_simple_string(ws, "this_state_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_religion(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"), rc ? text::produce_simple_string(ws, ws.world.religion_get_name(t)) : text::produce_simple_string(ws, "this_province_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_religion_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto rc = this_slot != -1 ? ws.world.nation_get_religion(nations::owner_of_pop(ws, trigger::to_pop(this_slot))) : dcon::religion_id{};
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "religion"), rc ? text::produce_simple_string(ws, ws.world.religion_get_name(t)) : text::produce_simple_string(ws, "this_pop_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_terrain_province(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "province_terrain"), text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_terrain_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "province_terrain"), text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_trade_goods(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).com_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rgo_production"), text::produce_simple_string(ws, ws.world.commodity_get_name(t)), ws, layout, box);
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.world.rebel_type_get_name(t)), text::produce_simple_string(ws, "an_active_rebel"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_faction_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).reb_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_member_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, ws.world.rebel_type_get_name(t)));
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_pop"), text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_pop"), text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, "this_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_this_rebel(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, "the_rebel_ind_nation"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_cultural_union_tag_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "union_for_nation"), text::produce_simple_string(ws, holder ? ws.world.nation_get_name(holder) : ws.world.national_identity_get_name(tag)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_can_build_factory_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ?
		"capitalists_can_build" : "capitalists_cannot_build"));
	text::close_layout_box(layout, box);
}
void tf_can_build_factory_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ?
		"capitalists_can_build" : "capitalists_cannot_build"));
	text::close_layout_box(layout, box);
}
void tf_can_build_factory_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ?
		"capitalists_can_build" : "capitalists_cannot_build"));
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, "war_exhaustion"), text::fp_percentage{trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_exhaustion_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "war_exhaustion"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_exhaustion_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "war_exhaustion"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_blockade(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "total_blockade"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_owns(TRIGGER_DISPLAY_PARAMS) {
	auto p = trigger::payload(tval[1]).prov_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ?
		"owns" : "does_not_own"));
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, ws.world.province_get_name(p)));
	text::close_layout_box(layout, box);
}
void tf_controls(TRIGGER_DISPLAY_PARAMS) {
	auto p = trigger::payload(tval[1]).prov_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ?
		"controls" : "does_not_control"));
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, ws.world.province_get_name(p)));
	text::close_layout_box(layout, box);
}
void tf_is_core_integer(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_core_in"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, ws.world.province_get_name(trigger::payload(tval[1]).prov_id)));
	text::close_layout_box(layout, box);
}
void tf_is_core_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.nation_get_name(trigger::to_nation(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.nation_get_name(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.nation_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.nation_get_name(nations::owner_of_pop(ws, trigger::to_pop(this_slot))));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.nation_get_name(trigger::to_nation(from_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_reb(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1) {
		auto tg = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
		auto h = ws.world.national_identity_get_nation_from_identity_holder(tg);
		text::add_to_layout_box(layout, ws, box, h ? ws.world.nation_get_name(h) : ws.world.national_identity_get_name(tg));
	} else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "the_rebel_ind_nation"));
	text::close_layout_box(layout, box);
}
void tf_is_core_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tg = trigger::payload(tval[1]).tag_id;
	auto h = ws.world.national_identity_get_nation_from_identity_holder(tg);

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_core_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, h ? ws.world.nation_get_name(h) : ws.world.national_identity_get_name(tg));
	text::close_layout_box(layout, box);
}
void tf_num_of_revolts(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_reb_control"), int64_t(tval[2]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_revolt_percentage(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage_reb_control"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_int(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"), int64_t(tval[2]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"), text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, from_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(from_slot))) : text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"), text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(this_slot))) : text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"), text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)))) : text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"), text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)))) : text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_cities_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_owned_provinces"), text::produce_simple_string(ws, "num_provinces_owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(nations::owner_of_pop(ws, trigger::to_pop(this_slot)))) : text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_num_of_ports(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_ports_connected"), int64_t( trigger::read_float_from_payload(tval[1])), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_allies(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_allies"), int64_t(trigger::read_float_from_payload(tval[1])), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_num_of_vassals(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "num_of_vassals"), int64_t(trigger::read_float_from_payload(tval[1])), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_owned_by_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_owned_by_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_owned_by_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_owned_by_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_owned_by_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_owned_by_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "owned_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_exists_bool(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ? "exists" : "does_not_exist"));
	text::close_layout_box(layout, box);
}
void tf_exists_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ? "exists" : "does_not_exist"));
	text::close_layout_box(layout, box);
}
void tf_has_country_flag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.national_definitions.flag_variable_names[trigger::payload(tval[1]).natf_id]), text::produce_simple_string(ws, "set"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_country_flag_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.national_definitions.flag_variable_names[trigger::payload(tval[1]).natf_id]), text::produce_simple_string(ws, "set"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_country_flag_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, ws.national_definitions.flag_variable_names[trigger::payload(tval[1]).natf_id]), text::produce_simple_string(ws, "set"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_province(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "continent"), text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_state(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "continent"), text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_nation(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "continent"), text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_pop(TRIGGER_DISPLAY_PARAMS) {
	auto t = trigger::payload(tval[1]).mod_id;
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "continent"), text::produce_simple_string(ws, ws.world.modifier_get_name(t)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_continent_nation_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_state_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_province_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_pop_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_nation_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_state_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_province_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_continent_pop_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "same_continent"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_casus_belli_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != - 1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_casus_belli_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_casus_belli_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_military_access_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_military_access_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "military_access_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_value(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_prestige_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_prestige_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "prestige"), text::produce_simple_string(ws, "prestige_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_badboy(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "infamy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
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
	display_with_has_comparison(tval[0], ws.world.factory_type_get_name(trigger::payload(tval[1]).fac_id), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_building_state_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], ws.world.factory_type_get_name(trigger::payload(tval[1]).fac_id), ws, layout, box);
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
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
	text::close_layout_box(layout, box);
}
void tf_has_country_modifier_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "national_modifier"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
	text::close_layout_box(layout, box);
}
void tf_has_province_modifier(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "provincial_modifier"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
	text::close_layout_box(layout, box);
}
void tf_region(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "part_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.state_definition_get_name(trigger::payload(tval[1]).state_id));
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
		display_with_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)), ws, layout, box);
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
		display_with_comparison(tval[0], ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)), ws, layout, box);
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
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_neighbour_this(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_border_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_neighbour_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_border_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_units_in_province_value(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "units_in_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::add_space_to_layout_box(layout, ws, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(layout, ws, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(layout, ws, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(layout, ws, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_units_in_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::add_space_to_layout_box(layout, ws, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "units_in_province"), int64_t(tval[1]), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_war_with_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_war_with_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_war_with_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_war_with_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_war_with_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_war_with_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "at_war_against"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, "treasury"), text::fp_currency{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_lost_national(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage_unowned_cores"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, "ruling_party_ideology"), ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_ruling_party_ideology_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "ruling_party_ideology"), ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_ruling_party(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "ruling_party"), ws.world.political_party_get_name(trigger::payload(tval[1]).par_id), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_ideology_enabled(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0],ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id), text::produce_simple_string(ws, "enabled"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_political_reform_want_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "political_reform_desire"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_political_reform_want_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "political_reform_desire"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_social_reform_want_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_reform_desire"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_social_reform_want_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_reform_desire"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, "plurality"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) / 100.0f }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_corruption(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "crime_percent"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"), text::produce_simple_string(ws, "dominant_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_state_religion_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "national_religion"), text::produce_simple_string(ws, "dominant_religion"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "owner_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "owner_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "owner_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), text::produce_simple_string(ws, "this_pop_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_nation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_nation_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), ws.world.culture_get_name(ws.world.nation_get_primary_culture(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_nation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), ws.world.culture_get_name(ws.world.nation_get_primary_culture(ws.world.province_get_nation_from_province_ownership(trigger::to_province(this_slot)))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "primary_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), ws.world.culture_get_name(nations::owner_of_pop(ws, trigger::to_pop(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), ws.world.culture_get_name(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_state_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), ws.world.culture_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), ws.world.culture_get_name(nations::owner_of_pop(ws, trigger::to_pop(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), ws.world.culture_get_name(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_province_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), ws.world.culture_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), ws.world.culture_get_name(ws.world.nation_get_primary_culture(trigger::to_nation(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), ws.world.culture_get_name(nations::owner_of_pop(ws, trigger::to_pop(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), ws.world.culture_get_name(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_primary_culture_pop_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	if(this_slot != -1)
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), ws.world.culture_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))), ws, layout, box);
	else
		display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "this_nation_primary_culture"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "culture"), text::produce_simple_string(ws, "an_accepted"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "an_accepted"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_accepted_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "dominant_culture"), text::produce_simple_string(ws, "an_accepted"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_coastal(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "coastal"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_in_sphere_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_in_sphere_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_sphere_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_sphere_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_sphere_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_in_sphere_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "in_sphere_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_produces_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_producer_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::close_layout_box(layout, box);
}
void tf_produces_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_producer_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::close_layout_box(layout, box);
}
void tf_produces_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_producer_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::close_layout_box(layout, box);
}

void tf_produces_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "a_producer_of"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
	text::close_layout_box(layout, box);
}
void tf_average_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_consciousness_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_consciousness_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_average_consciousness_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_next_reform_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "next_reform"), ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_next_reform_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "next_reform"), ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_next_rreform_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "next_reform"), ws.world.reform_option_get_name(trigger::payload(tval[1]).ropt_id), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_next_rreform_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "next_reform"), ws.world.reform_option_get_name(trigger::payload(tval[1]).ropt_id), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rebel_power_fraction(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, (tval[0] & trigger::association_mask) == trigger::association_eq ? "never" : "always"));
	text::close_layout_box(layout, box);
}
void tf_recruited_percentage_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage_recruited"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_recruited_percentage_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percentage_recruited"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
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
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_controlled_by_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_owner(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "owner"));
	text::close_layout_box(layout, box);
}
void tf_controlled_by_reb(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "controlled_by_rebel"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_canal_enabled(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		std::get<2>(ws.s.province_m.canals[tval[2] - 1]), scenario::fixed_ui::enabled, ws, container, cursor_in, lm, fmt);
}
void tf_is_state_capital(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "state_capital"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_truce_with_tag(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, trigger::payload(tval[1]).tag_id);
	text::close_layout_box(layout, box);
}
void tf_truce_with_from(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(from_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(from_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "from_nation"));
	text::close_layout_box(layout, box);
}
void tf_truce_with_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, trigger::to_nation(this_slot));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_truce_with_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_truce_with_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_truce_with_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "a_truce_with"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	if(this_slot != -1)
		text::add_to_layout_box(layout, ws, box, nations::owner_of_pop(ws, trigger::to_pop(this_slot)));
	else
		text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "this_nation"));
	text::close_layout_box(layout, box);
}
void tf_total_pops_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "population"), int64_t( trigger::read_float_from_payload(tval + 1) ), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_total_pops_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "population"), int64_t(trigger::read_float_from_payload(tval + 1)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_total_pops_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "population"), int64_t(trigger::read_float_from_payload(tval + 1)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_total_pops_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "population"), int64_t(trigger::read_float_from_payload(tval + 1)), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_pop_type_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_of_type"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::close_layout_box(layout, box);
}
void tf_has_pop_type_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_of_type"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::close_layout_box(layout, box);
}
void tf_has_pop_type_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "pops_of_type"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::close_layout_box(layout, box);
}
void tf_has_pop_type_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "of_type_plain"), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
	text::close_layout_box(layout, box);
}
void tf_has_empty_adjacent_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, "an_empty_adj_prov"), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_leader(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = display_with_has_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_leader_named], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, trigger_payload(tval[2]).text, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_ai(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::ai_controlled], ws, container, cursor_in, lm, fmt);
}
void tf_can_create_vassals(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_is_possible_vassal(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	return display_with_comparison(tval[0],
		name, scenario::fixed_ui::a_possible_vassal, ws, container, cursor_in, lm, fmt);
}
void tf_province_id(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.w.province_s.province_state_container.get<province_state::name>(provinces::province_tag(tval[2])), ws, container, cursor_in, lm, fmt);
}
void tf_vassal_of_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_vassal_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_vassal_of_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_vassal_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_vassal_of_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_vassal_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_vassal_of_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_vassal_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_vassal_of_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_vassal_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_vassal_of_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_vassal_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_substate_of_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_substate_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_substate_of_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_substate_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_substate_of_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_substate_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_substate_of_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_substate_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_substate_of_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_substate_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_substate_of_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_substate_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_alliance_with_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::allied_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_alliance_with_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::allied_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_alliance_with_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::allied_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_alliance_with_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::allied_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_alliance_with_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::allied_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_alliance_with_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::allied_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_has_recently_lost_war(TRIGGER_DISPLAY_PARAMS) {
	if((tval[0] & trigger_codes::association_mask) == trigger_codes::association_eq) {
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::has_recently_lost_war], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	} else {
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::has_not_recently_lost_war], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	}
}
void tf_is_mobilised(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::mobilized], ws, container, cursor_in, lm, fmt);
}
void tf_mobilisation_size(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "mobilization_size"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_crime_higher_than_education_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::admin_spending, ws.s.fixed_ui_text[scenario::fixed_ui::greater_than_edu_spending], ws, container, cursor_in, lm, fmt);
}
void tf_crime_higher_than_education_state(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::admin_spending, ws.s.fixed_ui_text[scenario::fixed_ui::greater_than_edu_spending], ws, container, cursor_in, lm, fmt);
}
void tf_crime_higher_than_education_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::admin_spending, ws.s.fixed_ui_text[scenario::fixed_ui::greater_than_edu_spending], ws, container, cursor_in, lm, fmt);
}
void tf_crime_higher_than_education_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::admin_spending, ws.s.fixed_ui_text[scenario::fixed_ui::greater_than_edu_spending], ws, container, cursor_in, lm, fmt);
}
void tf_agree_with_ruling_party(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "support_for_rp"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_colonial_state(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::colonial], ws, container, cursor_in, lm, fmt);
}
void tf_is_colonial_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::colonial], ws, container, cursor_in, lm, fmt);
}
void tf_has_factories(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_factory], ws, container, cursor_in, lm, fmt);
}
void tf_in_default_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::in_default_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_in_default_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::in_default_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_in_default_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::in_default_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_in_default_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::in_default_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_in_default_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::in_default_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_in_default_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::in_default_to], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_total_num_of_ports(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::num_of_ports, tval[2], display_type::integer,
		ws, container, cursor_in, lm, fmt);
}
void tf_always(TRIGGER_DISPLAY_PARAMS) {
	if((tval[0] & trigger_codes::association_mask) == trigger_codes::association_eq)
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::always], fmt, ws, container, lm);
	else
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::never], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_election(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::holding_an_election],
		ws, container, cursor_in, lm, fmt);
}
void tf_has_global_flag(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.variables_m.global_variable_to_name[trigger_payload(tval[2]).global_var], scenario::fixed_ui::set_globally, ws, container, cursor_in, lm, fmt);
}
void tf_is_capital(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::capital],
		ws, container, cursor_in, lm, fmt);
}
void tf_nationalvalue_nation(TRIGGER_DISPLAY_PARAMS) {
	auto mod = trigger_payload(tval[2]).nat_mod;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_value, ws.s.modifiers_m.national_modifiers[mod].name, ws, container, cursor_in, lm, fmt);
}
void tf_nationalvalue_pop(TRIGGER_DISPLAY_PARAMS) {
	auto mod = trigger_payload(tval[2]).nat_mod;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_value, ws.s.modifiers_m.national_modifiers[mod].name, ws, container, cursor_in, lm, fmt);
}
void tf_nationalvalue_province(TRIGGER_DISPLAY_PARAMS) {
	auto mod = trigger_payload(tval[2]).nat_mod;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_value, ws.s.modifiers_m.national_modifiers[mod].name, ws, container, cursor_in, lm, fmt);
}
void tf_industrial_score_value(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::industrial_score, trigger_payload(tval[2]).signed_value, display_type::integer,
		ws, container, cursor_in, lm, fmt);
}
void tf_industrial_score_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::industrial_score, ws.s.fixed_ui_text[scenario::fixed_ui::industrial_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_industrial_score_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::industrial_score, ws.s.fixed_ui_text[scenario::fixed_ui::industrial_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_industrial_score_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::industrial_score, ws.s.fixed_ui_text[scenario::fixed_ui::industrial_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_industrial_score_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::industrial_score, ws.s.fixed_ui_text[scenario::fixed_ui::industrial_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_industrial_score_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::industrial_score, ws.s.fixed_ui_text[scenario::fixed_ui::industrial_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_military_score_value(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::military_score, trigger_payload(tval[2]).signed_value, display_type::integer,
		ws, container, cursor_in, lm, fmt);
}
void tf_military_score_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::military_score, ws.s.fixed_ui_text[scenario::fixed_ui::military_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_military_score_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::military_score, ws.s.fixed_ui_text[scenario::fixed_ui::military_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_military_score_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::military_score, ws.s.fixed_ui_text[scenario::fixed_ui::military_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_military_score_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::military_score, ws.s.fixed_ui_text[scenario::fixed_ui::military_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_military_score_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::military_score, ws.s.fixed_ui_text[scenario::fixed_ui::military_score_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_civilized_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::civilized],
		ws, container, cursor_in, lm, fmt);
}
void tf_civilized_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::civilized],
		ws, container, cursor_in, lm, fmt);
}
void tf_civilized_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::civilized],
		ws, container, cursor_in, lm, fmt);
}
void tf_national_provinces_occupied(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "percent_occupied"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_is_greater_power_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::great_power],
		ws, container, cursor_in, lm, fmt);
}
void tf_is_greater_power_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::great_power],
		ws, container, cursor_in, lm, fmt);
}
void tf_is_greater_power_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::great_power],
		ws, container, cursor_in, lm, fmt);
}
void tf_rich_tax(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::rich_tax, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_middle_tax(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::middle_tax, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_poor_tax(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::poor_tax, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_social_spending_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::social_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_social_spending_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::social_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_social_spending_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::social_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_military_spending_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::mil_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_military_spending_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::mil_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_military_spending_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::mil_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_military_spending_state(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::mil_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_administration_spending_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::admin_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_administration_spending_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::admin_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_administration_spending_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::admin_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_administration_spending_state(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::admin_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_education_spending_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::edu_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_education_spending_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::edu_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_education_spending_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::edu_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_education_spending_state(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::edu_spending, float(trigger_payload(tval[2]).signed_value) / 100.0f, display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_colonial_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0], ws.s.fixed_ui_text[scenario::fixed_ui::colonies],
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_religion_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rel = trigger_payload(tval[2]).small.values.religion;
	return display_with_comparison(tval[0],
		ws.s.culture_m.religions[rel].name, scenario::fixed_ui::dominant_religion,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_religion_state(TRIGGER_DISPLAY_PARAMS) {
	auto rel = trigger_payload(tval[2]).small.values.religion;
	return display_with_comparison(tval[0],
		ws.s.culture_m.religions[rel].name, scenario::fixed_ui::dominant_religion,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_religion_province(TRIGGER_DISPLAY_PARAMS) {
	auto rel = trigger_payload(tval[2]).small.values.religion;
	return display_with_comparison(tval[0],
		ws.s.culture_m.religions[rel].name, scenario::fixed_ui::dominant_religion,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_culture_nation(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).culture;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_culture, ws.s.culture_m.culture_container[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).culture;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_culture, ws.s.culture_m.culture_container[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).culture;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_culture, ws.s.culture_m.culture_container[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_issue_nation(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).small.values.option;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_issue, ws.s.issues_m.options[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_issue_state(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).small.values.option;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_issue, ws.s.issues_m.options[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_issue_province(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).small.values.option;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_issue, ws.s.issues_m.options[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_issue_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).small.values.option;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_issue, ws.s.issues_m.options[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_ideology_nation(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).small.values.ideology;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_issue, ws.s.ideologies_m.ideology_container[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_ideology_state(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).small.values.ideology;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_issue, ws.s.ideologies_m.ideology_container[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_ideology_province(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).small.values.ideology;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_issue, ws.s.ideologies_m.ideology_container[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_majority_ideology_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).small.values.ideology;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::dominant_issue, ws.s.ideologies_m.ideology_container[c].name,
		ws, container, cursor_in, lm, fmt);
}
void tf_poor_strata_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_militancy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_militancy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_militancy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_tax_above_poor(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::rich_tax, ws.s.fixed_ui_text[scenario::fixed_ui::gt_poor_tax],
		ws, container, cursor_in, lm, fmt);
}
void tf_culture_has_union_tag_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_cultural_union],
		ws, container, cursor_in, lm, fmt);
}
void tf_culture_has_union_tag_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_cultural_union],
		ws, container, cursor_in, lm, fmt);
}
void tf_this_culture_union_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	return display_with_comparison(tval[0],
		scenario::fixed_ui::cultural_union, name, ws, container, cursor_in, lm, fmt);
}
void tf_this_culture_union_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::cultural_union, tname, ws, container, cursor_in, lm, fmt);
}
void tf_this_culture_union_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::cultural_union, tname, ws, container, cursor_in, lm, fmt);
}
void tf_this_culture_union_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::cultural_union, tname, ws, container, cursor_in, lm, fmt);
}
void tf_this_culture_union_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::cultural_union, tname, ws, container, cursor_in, lm, fmt);
}
void tf_this_culture_union_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::cultural_union, tname, ws, container, cursor_in, lm, fmt);
}
void tf_this_culture_union_this_union_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::cultural_union, ws.s.fixed_ui_text[scenario::fixed_ui::same_cultural_union], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_this_culture_union_this_union_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::cultural_union, ws.s.fixed_ui_text[scenario::fixed_ui::same_cultural_union], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_this_culture_union_this_union_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::cultural_union, ws.s.fixed_ui_text[scenario::fixed_ui::same_cultural_union], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_this_culture_union_this_union_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::cultural_union, ws.s.fixed_ui_text[scenario::fixed_ui::same_cultural_union], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_minorities_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::non_accepted_pops],
		ws, container, cursor_in, lm, fmt);
}
void tf_minorities_state(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::non_accepted_pops],
		ws, container, cursor_in, lm, fmt);
}
void tf_minorities_province(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::non_accepted_pops],
		ws, container, cursor_in, lm, fmt);
}
void tf_revanchism_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "revanchism"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_revanchism_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "revanchism"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_has_crime(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::crime],
		ws, container, cursor_in, lm, fmt);
}
void tf_num_of_substates(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::num_substates, tval[2], display_type::integer,
		ws, container, cursor_in, lm, fmt);
}
void tf_num_of_vassals_no_substates(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0], scenario::fixed_ui::num_vassals_no_substates, tval[2], display_type::integer,
		ws, container, cursor_in, lm, fmt);
}
void tf_brigades_compare_this(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::num_regiments, ws.s.fixed_ui_text[scenario::fixed_ui::num_regiments_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_brigades_compare_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		scenario::fixed_ui::num_regiments, ws.s.fixed_ui_text[scenario::fixed_ui::num_regiments_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_constructing_cb_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::fabricating_on], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_constructing_cb_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::fabricating_on], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_constructing_cb_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::fabricating_on], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_constructing_cb_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::fabricating_on], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_constructing_cb_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::fabricating_on], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_constructing_cb_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::fabricating_on], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_constructing_cb_discovered(TRIGGER_DISPLAY_PARAMS) {
	if((tval[0] & trigger_codes::association_mask) == trigger_codes::association_eq)
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::cb_discovered], fmt, ws, container, lm);
	else
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::cb_not_discovered], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_constructing_cb_progress(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "cb_progress"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_civilization_progress(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "civ_progress"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_constructing_cb_type(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[2]).small.values.cb_type;

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::constructing_a], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.military_m.cb_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::casus_belli], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_our_vassal_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;
	auto scope_name = bool(to_nation(primary_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::nation_in_scope];

	cursor_in = display_with_comparison_no_newline(tval[0],
		name, scenario::fixed_ui::a_vassal_of, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, scope_name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_our_vassal_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];
	auto scope_name = bool(to_nation(primary_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::nation_in_scope];

	cursor_in = display_with_comparison_no_newline(tval[0],
		tname, scenario::fixed_ui::a_vassal_of, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, scope_name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_our_vassal_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	auto scope_name = bool(to_nation(primary_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::nation_in_scope];

	cursor_in = display_with_comparison_no_newline(tval[0],
		tname, scenario::fixed_ui::a_vassal_of, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, scope_name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_our_vassal_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	auto scope_name = bool(to_nation(primary_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::nation_in_scope];

	cursor_in = display_with_comparison_no_newline(tval[0],
		tname, scenario::fixed_ui::a_vassal_of, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, scope_name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_our_vassal_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	auto scope_name = bool(to_nation(primary_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::nation_in_scope];

	cursor_in = display_with_comparison_no_newline(tval[0],
		tname, scenario::fixed_ui::a_vassal_of, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, scope_name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_our_vassal_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	auto scope_name = bool(to_nation(primary_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::nation_in_scope];

	cursor_in = display_with_comparison_no_newline(tval[0],
		tname, scenario::fixed_ui::a_vassal_of, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, scope_name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_substate(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_substate],
		ws, container, cursor_in, lm, fmt);
}
void tf_great_wars_enabled(TRIGGER_DISPLAY_PARAMS) {
	if((tval[0] & trigger_codes::association_mask) == trigger_codes::association_eq)
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::gw_enabled], fmt, ws, container, lm);
	else
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::gw_not_enabled], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_can_nationalize(TRIGGER_DISPLAY_PARAMS) {
	if((tval[0] & trigger_codes::association_mask) == trigger_codes::association_eq)
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::can_perform_nationalization], fmt, ws, container, lm);
	else
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::cannot_perform_nationalization], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_part_of_sphere(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::in_sphere],
		ws, container, cursor_in, lm, fmt);
}
void tf_is_sphere_leader_of_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[2]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::sphere_leader_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_sphere_leader_of_from(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::sphere_leader_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_sphere_leader_of_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::sphere_leader_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_sphere_leader_of_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::sphere_leader_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_sphere_leader_of_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::sphere_leader_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_is_sphere_leader_of_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = display_with_comparison_no_newline(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::sphere_leader_of], ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_number_of_states(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::num_states, tval[2], display_type::integer,
		ws, container, cursor_in, lm, fmt);
}
void tf_war_score(TRIGGER_DISPLAY_PARAMS) {
	//stub for apparently unused trigger
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_is_releasable_vassal_from(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation], scenario::fixed_ui::a_releasable_vassal, ws, container, cursor_in, lm, fmt);
}
void tf_is_releasable_vassal_other(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_releasable_vassal], ws, container, cursor_in, lm, fmt);
}
void tf_has_recent_imigration(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::recent_immigration], ws, container, cursor_in, lm, fmt);
}
void tf_province_control_days(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::num_control_days, tval[2], display_type::integer,
		ws, container, cursor_in, lm, fmt);
}
void tf_is_disarmed(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::disarmed], ws, container, cursor_in, lm, fmt);
}
void tf_big_producer(TRIGGER_DISPLAY_PARAMS) {
	// stub: used only rarely in ai chances and would be expensive to test
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::never], ws, container, cursor_in, lm, fmt);
}
void tf_someone_can_form_union_tag_from(TRIGGER_DISPLAY_PARAMS) {
	// stub: apparently unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::never], ws, container, cursor_in, lm, fmt);
}
void tf_someone_can_form_union_tag_other(TRIGGER_DISPLAY_PARAMS) {
	// stub: apparently unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::never], ws, container, cursor_in, lm, fmt);
}
void tf_social_movement_strength(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "social_mov_support"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_political_movement_strength(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "political_mov_support"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_can_build_factory_in_capital_state(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_social_movement(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_member_of_social_mov], ws, container, cursor_in, lm, fmt);
}
void tf_political_movement(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::a_member_of_political_mov], ws, container, cursor_in, lm, fmt);
}
void tf_has_cultural_sphere(TRIGGER_DISPLAY_PARAMS) {
	return display_with_has_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::cultural_sphere_members], ws, container, cursor_in, lm, fmt);
}
void tf_world_wars_enabled(TRIGGER_DISPLAY_PARAMS) {
	if((tval[0] & trigger_codes::association_mask) == trigger_codes::association_eq)
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::ww_enabled], fmt, ws, container, lm);
	else
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::ww_not_enabled], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_has_pop_culture_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::culture>(to_pop(this_slot)) : cultures::culture_tag();
	auto cn = is_valid_index(c) ? ws.s.culture_m.culture_container[c].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_culture];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::culture, cn, ws, container, cursor_in, lm, fmt);
}
void tf_has_pop_culture_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::culture>(to_pop(this_slot)) : cultures::culture_tag();
	if(is_valid_index(c)) {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.culture_m.culture_container[c].name, ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	} else {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.fixed_ui_text[scenario::fixed_ui::pops_with], ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_culture], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	}
}
void tf_has_pop_culture_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::culture>(to_pop(this_slot)) : cultures::culture_tag();
	if(is_valid_index(c)) {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.culture_m.culture_container[c].name, ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	} else {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.fixed_ui_text[scenario::fixed_ui::pops_with], ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_culture], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	}
}
void tf_has_pop_culture_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::culture>(to_pop(this_slot)) : cultures::culture_tag();
	if(is_valid_index(c)) {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.culture_m.culture_container[c].name, ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	} else {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.fixed_ui_text[scenario::fixed_ui::pops_with], ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_culture], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	}
}
void tf_has_pop_culture_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).culture;
	return display_with_comparison(tval[0],
		scenario::fixed_ui::culture, ws.s.culture_m.culture_container[c].name, ws, container, cursor_in, lm, fmt);
}
void tf_has_pop_culture_state(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).culture;
	cursor_in = display_with_has_comparison_no_newline(tval[0],
		ws.s.culture_m.culture_container[c].name, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_has_pop_culture_province(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).culture;
	cursor_in = display_with_has_comparison_no_newline(tval[0],
		ws.s.culture_m.culture_container[c].name, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_has_pop_culture_nation(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger_payload(tval[2]).culture;
	cursor_in = display_with_has_comparison_no_newline(tval[0],
		ws.s.culture_m.culture_container[c].name, ws, container, cursor_in, lm, fmt);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_has_pop_religion_pop_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::religion>(to_pop(this_slot)) : cultures::religion_tag();
	auto cn = is_valid_index(c) ? ws.s.culture_m.religions[c].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_religion];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::religion, cn, ws, container, cursor_in, lm, fmt);
}
void tf_has_pop_religion_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::religion>(to_pop(this_slot)) : cultures::religion_tag();
	if(is_valid_index(c)) {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.culture_m.religions[c].name, ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	} else {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.fixed_ui_text[scenario::fixed_ui::pops_with], ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_religion], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	}
}
void tf_has_pop_religion_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::religion>(to_pop(this_slot)) : cultures::religion_tag();
	if(is_valid_index(c)) {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.culture_m.religions[c].name, ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	} else {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.fixed_ui_text[scenario::fixed_ui::pops_with], ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_religion], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	}
}
void tf_has_pop_religion_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::religion>(to_pop(this_slot)) : cultures::religion_tag();
	if(is_valid_index(c)) {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.culture_m.religions[c].name, ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::pops], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	} else {
		cursor_in = display_with_has_comparison_no_newline(tval[0],
			ws.s.fixed_ui_text[scenario::fixed_ui::pops_with], ws, container, cursor_in, lm, fmt);
		cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
		cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_religion], fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
		return cursor_in;
	}
}
void tf_has_pop_religion_pop(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger::payload(tval[1]).rel_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, ws.world.religion_get_name(c)), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_state(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger::payload(tval[1]).rel_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, ws.world.religion_get_name(c)), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_province(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger::payload(tval[1]).rel_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, ws.world.religion_get_name(c)), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_has_pop_religion_nation(TRIGGER_DISPLAY_PARAMS) {
	auto c = trigger::payload(tval[1]).rel_id;

	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_has_comparison(tval[0], text::produce_simple_string(ws, ws.world.religion_get_name(c)), ws, layout, box);
	text::add_space_to_layout_box(layout, ws, box);
	text::add_to_layout_box(layout, ws, box, text::produce_simple_string(ws, "pops"));
	text::close_layout_box(layout, box);
}
void tf_life_needs(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_everyday_needs(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_luxury_needs(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_consciousness_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_consciousness_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_consciousness_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_consciousness_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "consciousness"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_literacy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "literacy"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_literacy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "literacy"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_literacy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "literacy"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_literacy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "literacy"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_militancy_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_militancy_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_militancy_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_militancy_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "militancy"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_trade_goods_in_state_state(TRIGGER_DISPLAY_PARAMS) {
	auto g = trigger_payload(tval[2]).small.values.good;
	return display_with_comparison(tval[0],
		ws.s.economy_m.goods[g].name, scenario::fixed_ui::present_in_state,
		ws, container, cursor_in, lm, fmt);
}
void tf_trade_goods_in_state_province(TRIGGER_DISPLAY_PARAMS) {
	auto g = trigger_payload(tval[2]).small.values.good;
	return display_with_comparison(tval[0],
		ws.s.economy_m.goods[g].name, scenario::fixed_ui::present_in_state,
		ws, container, cursor_in, lm, fmt);
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
	display_with_comparison(tval[0], text::produce_simple_string(ws, "flashpoint_tension"), text::fp_one_place{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_crisis_exist(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::there, ws.s.fixed_ui_text[scenario::fixed_ui::an_ongoing_crisis],
		ws, container, cursor_in, lm, fmt);
}
void tf_is_liberation_crisis(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::current_crisis, ws.s.fixed_ui_text[scenario::fixed_ui::liberation_crisis],
		ws, container, cursor_in, lm, fmt);
}
void tf_is_claim_crisis(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::current_crisis, ws.s.fixed_ui_text[scenario::fixed_ui::claim_crisis],
		ws, container, cursor_in, lm, fmt);
}
void tf_crisis_temperature(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "crisis_temperature_plain"), int64_t( trigger::read_float_from_payload(tval + 1) ), ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_involved_in_crisis_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::involved_in_crisis],
		ws, container, cursor_in, lm, fmt);
}
void tf_involved_in_crisis_pop(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::involved_in_crisis],
		ws, container, cursor_in, lm, fmt);
}
void tf_rich_strata_life_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_life_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_life_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_life_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_everyday_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_everyday_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_everyday_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_everyday_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_luxury_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_luxury_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_luxury_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_rich_strata_luxury_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "rich_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_life_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_life_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_life_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_life_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_everyday_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_everyday_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_everyday_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_everyday_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_luxury_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_luxury_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_luxury_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_middle_strata_luxury_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "middle_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_life_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_life_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_life_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_life_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_life_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_everyday_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_everyday_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_everyday_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_everyday_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_everyday_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_luxury_needs_nation(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_luxury_needs_state(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_luxury_needs_province(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_poor_strata_luxury_needs_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "poor_luxury_needs_satisfaction"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_diplomatic_influence_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[3]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::dip_influence_over], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, text_data::integer{ tval[2] }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_diplomatic_influence_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::dip_influence_over], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ tval[2] }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_diplomatic_influence_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::dip_influence_over], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ tval[2] }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_diplomatic_influence_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::dip_influence_over], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ tval[2] }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_diplomatic_influence_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(from_slot)) ? provinces::province_owner(ws, to_prov(from_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::dip_influence_over], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ tval[2] }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_pop_unemployment_nation(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[4]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_unemployment_state(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[4]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_unemployment_province(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[4]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_unemployment_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_pop_unemployment_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto type = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::type>(to_pop(this_slot)) : population::pop_type_tag();
	auto tname = is_valid_index(type) ? ws.s.population_m.pop_types[type].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_type];

	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_unemployment_state_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto type = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::type>(to_pop(this_slot)) : population::pop_type_tag();
	auto tname = is_valid_index(type) ? ws.s.population_m.pop_types[type].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_type];

	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_pop_unemployment_province_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto type = bool(to_pop(this_slot)) ? ws.w.population_s.pops.get<pop::type>(to_pop(this_slot)) : population::pop_type_tag();
	auto tname = is_valid_index(type) ? ws.s.population_m.pop_types[type].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_pop_type];

	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_relation_tag(TRIGGER_DISPLAY_PARAMS) {
	auto tag = trigger_payload(tval[3]).tag;
	auto holder = ws.w.culture_s.tags_to_holders[tag];
	auto name = bool(holder) ? ws.w.nation_s.nations.get<nation::name>(holder) : ws.s.culture_m.national_tags[tag].default_name.name;

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::relationship_with], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ trigger_payload(tval[2]).signed_value }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_relation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::relationship_with], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ trigger_payload(tval[2]).signed_value }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_relation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::relationship_with], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ trigger_payload(tval[2]).signed_value }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_relation_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::relationship_with], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ trigger_payload(tval[2]).signed_value }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_relation_from_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(from_slot)) ? provinces::province_owner(ws, to_prov(from_slot)) : nations::country_tag();
	auto tname = bool(owner) ? ws.w.nation_s.nations.get<nation::name>(owner) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];

	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::relationship_with], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, tname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::integer{ trigger_payload(tval[2]).signed_value }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_check_variable(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.variables_m.national_variable_to_name[triggers::trigger_payload(tval[3]).nat_var], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::nat_variable, read_float_from_payload(tval + 2), display_type::fp_one_place,
		ws, container, cursor_in, lm, fmt);
}
void tf_upper_house(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[triggers::trigger_payload(tval[3]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support_in_uh, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_unemployment_by_type_nation(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[4]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_unemployment_by_type_state(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[4]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_unemployment_by_type_province(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[4]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::unemployment, read_float_from_payload(tval + 2), display_type::percent,
		ws, container, cursor_in, lm, fmt);
}
void tf_unemployment_by_type_pop(TRIGGER_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	make_condition(tval, ws, layout, primary_slot, this_slot, from_slot, indentation, show_condition, box);
	display_with_comparison(tval[0], text::produce_simple_string(ws, "unemployment"), text::fp_percentage{ trigger::read_float_from_payload(tval + 1) }, ws, layout, box);
	text::close_layout_box(layout, box);
}
void tf_party_loyalty_nation_province_id(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[5]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::support_in], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.w.province_s.province_state_container.get<province_state::name>(provinces::province_tag(tval[2])), fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, text_data::percent{ read_float_from_payload(tval + 3) }, fmt, ws, container, lm);

	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_party_loyalty_from_nation_province_id(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[5]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::support_in], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.w.province_s.province_state_container.get<province_state::name>(provinces::province_tag(tval[2])), fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::percent{ read_float_from_payload(tval + 3) }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_party_loyalty_province_province_id(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[5]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::support_in], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.w.province_s.province_state_container.get<province_state::name>(provinces::province_tag(tval[2])), fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::percent{ read_float_from_payload(tval + 3) }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_party_loyalty_from_province_province_id(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[5]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::support_in], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.w.province_s.province_state_container.get<province_state::name>(provinces::province_tag(tval[2])), fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::percent{ read_float_from_payload(tval + 3) }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_party_loyalty_nation_from_province(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[4]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::support_in], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	auto pname = bool(to_prov(from_slot)) ? ws.w.province_s.province_state_container.get<province_state::name>(to_prov(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_province];
	cursor_in = ui::add_text(cursor_in, pname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::percent{ read_float_from_payload(tval + 2) }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_party_loyalty_generic(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[4]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::support_in], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	auto pname = is_valid_index(to_prov(primary_slot)) ? ws.w.province_s.province_state_container.get<province_state::name>(to_prov(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::scope_province];
	cursor_in = ui::add_text(cursor_in, pname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::percent{ read_float_from_payload(tval + 2) }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_party_loyalty_from_nation_scope_province(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[4]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::support_in], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	auto pname = is_valid_index(to_prov(primary_slot)) ? ws.w.province_s.province_state_container.get<province_state::name>(to_prov(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::scope_province];
	cursor_in = ui::add_text(cursor_in, pname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::percent{ read_float_from_payload(tval + 2) }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_party_loyalty_from_province_scope_province(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[4]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::support_in], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	auto pname = is_valid_index(to_prov(primary_slot)) ? ws.w.province_s.province_state_container.get<province_state::name>(to_prov(primary_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::scope_province];
	cursor_in = ui::add_text(cursor_in, pname, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);

	cursor_in = ui::add_text(cursor_in, text_data::percent{ read_float_from_payload(tval + 2) }, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_can_build_in_province_railroad_no_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_rr,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_railroad_yes_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_rr,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_railroad_no_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_rr,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_railroad_yes_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_rr,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_fort_no_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_fort,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_fort_yes_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_fort,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_fort_no_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_fort,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_fort_yes_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_fort,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_naval_base_no_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_naval_base,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_naval_base_yes_limit_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(from_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::from_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_naval_base,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_naval_base_no_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_naval_base,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_in_province_naval_base_yes_limit_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto tname = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::name>(to_nation(this_slot)) : ws.s.fixed_ui_text[scenario::fixed_ui::this_nation];
	return display_with_comparison(tval[0],
		tname, scenario::fixed_ui::able_build_naval_base,
		ws, container, cursor_in, lm, fmt);
}
void tf_can_build_railway_in_capital_yes_whole_state_yes_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_can_build_railway_in_capital_yes_whole_state_no_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_can_build_railway_in_capital_no_whole_state_yes_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_can_build_railway_in_capital_no_whole_state_no_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_can_build_fort_in_capital_yes_whole_state_yes_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_can_build_fort_in_capital_yes_whole_state_no_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_can_build_fort_in_capital_no_whole_state_yes_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_can_build_fort_in_capital_no_whole_state_no_limit(TRIGGER_DISPLAY_PARAMS) {
	// stub: virtually unused
	return display_with_comparison(tval[0],
		ws.s.fixed_ui_text[scenario::fixed_ui::always], ws, container, cursor_in, lm, fmt);
}
void tf_work_available_nation(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = display_with_comparison(tval[0],
		scenario::fixed_ui::work, ws.s.fixed_ui_text[scenario::fixed_ui::available_for], ws, container, cursor_in, lm, fmt);

	lm.increase_indent(1);
	auto count_workers = tval[1] - 1;
	for(int32_t i = 0; i < count_workers; ++i) {
		auto type = trigger_payload(tval[2 + i]).small.values.pop_type;
		cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
	}
	lm.decrease_indent(1);
	return cursor_in;
}
void tf_work_available_state(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = display_with_comparison(tval[0],
		scenario::fixed_ui::work, ws.s.fixed_ui_text[scenario::fixed_ui::available_for], ws, container, cursor_in, lm, fmt);

	lm.increase_indent(1);
	auto count_workers = tval[1] - 1;
	for(int32_t i = 0; i < count_workers; ++i) {
		auto type = trigger_payload(tval[2 + i]).small.values.pop_type;
		cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
	}
	lm.decrease_indent(1);
	return cursor_in;
}
void tf_work_available_province(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = display_with_comparison(tval[0],
		scenario::fixed_ui::work, ws.s.fixed_ui_text[scenario::fixed_ui::available_for], ws, container, cursor_in, lm, fmt);

	lm.increase_indent(1);
	auto count_workers = tval[1] - 1;
	for(int32_t i = 0; i < count_workers; ++i) {
		auto type = trigger_payload(tval[2 + i]).small.values.pop_type;
		cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
		cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
		lm.finish_current_line();
	}
	lm.decrease_indent(1);
	return cursor_in;
}
void tf_variable_ideology_name_nation(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[2]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support, read_float_from_payload(tval + 3), display_type::percent, ws, container, cursor_in, lm, fmt);
}
void tf_variable_ideology_name_state(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[2]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support, read_float_from_payload(tval + 3), display_type::percent, ws, container, cursor_in, lm, fmt);
}
void tf_variable_ideology_name_province(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[2]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support, read_float_from_payload(tval + 3), display_type::percent, ws, container, cursor_in, lm, fmt);
}
void tf_variable_ideology_name_pop(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.ideologies_m.ideology_container[trigger_payload(tval[2]).small.values.ideology].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support, read_float_from_payload(tval + 3), display_type::percent, ws, container, cursor_in, lm, fmt);
}
void tf_variable_issue_name_nation(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.options[trigger_payload(tval[2]).small.values.option].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support, read_float_from_payload(tval + 3), display_type::percent, ws, container, cursor_in, lm, fmt);
}
void tf_variable_issue_name_state(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.options[trigger_payload(tval[2]).small.values.option].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support, read_float_from_payload(tval + 3), display_type::percent, ws, container, cursor_in, lm, fmt);
}
void tf_variable_issue_name_province(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.options[trigger_payload(tval[2]).small.values.option].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support, read_float_from_payload(tval + 3), display_type::percent, ws, container, cursor_in, lm, fmt);
}
void tf_variable_issue_name_pop(TRIGGER_DISPLAY_PARAMS) {
	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.options[trigger_payload(tval[2]).small.values.option].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::support, read_float_from_payload(tval + 3), display_type::percent, ws, container, cursor_in, lm, fmt);
}
void tf_variable_issue_group_name_nation(TRIGGER_DISPLAY_PARAMS) {
	auto option = trigger_payload(tval[2]).small.values.option;
	auto issue = ws.s.issues_m.options[option].parent_issue;

	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.issues_container[issue].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	//cursor_in = ui::add_linear_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::reform], fmt, ws.s.gui_m, ws.w.gui_m, container, lm);
	//cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.options[option].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_variable_issue_group_name_state(TRIGGER_DISPLAY_PARAMS) {
	auto option = trigger_payload(tval[2]).small.values.option;
	auto issue = ws.s.issues_m.options[option].parent_issue;

	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.issues_container[issue].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	//cursor_in = ui::add_linear_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::reform], fmt, ws.s.gui_m, ws.w.gui_m, container, lm);
	//cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.options[option].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_variable_issue_group_name_province(TRIGGER_DISPLAY_PARAMS) {
	auto option = trigger_payload(tval[2]).small.values.option;
	auto issue = ws.s.issues_m.options[option].parent_issue;

	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.issues_container[issue].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	//cursor_in = ui::add_linear_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::reform], fmt, ws.s.gui_m, ws.w.gui_m, container, lm);
	//cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.options[option].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_variable_issue_group_name_pop(TRIGGER_DISPLAY_PARAMS) {
	auto option = trigger_payload(tval[2]).small.values.option;
	auto issue = ws.s.issues_m.options[option].parent_issue;

	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.issues_container[issue].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	//cursor_in = ui::add_linear_text(cursor_in, ws.s.fixed_ui_text[scenario::fixed_ui::reform], fmt, ws.s.gui_m, ws.w.gui_m, container, lm);
	//cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.fixed_ui_text[cmp_code_to_fixed_ui(tval[0])], fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	cursor_in = ui::add_text(cursor_in, ws.s.issues_m.options[option].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_to_newline(cursor_in, ws.s.gui_m, fmt);
	lm.finish_current_line();
	return cursor_in;
}
void tf_variable_pop_type_name_nation(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[2]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::percentage, read_float_from_payload(tval + 3), display_type::percent_fp_one_place, ws, container, cursor_in, lm, fmt);
}
void tf_variable_pop_type_name_state(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[2]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::percentage, read_float_from_payload(tval + 3), display_type::percent_fp_one_place, ws, container, cursor_in, lm, fmt);
}
void tf_variable_pop_type_name_province(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[2]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::percentage, read_float_from_payload(tval + 3), display_type::percent_fp_one_place, ws, container, cursor_in, lm, fmt);
}
void tf_variable_pop_type_name_pop(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[2]).small.values.pop_type;
	cursor_in = ui::add_text(cursor_in, ws.s.population_m.pop_types[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::percentage, read_float_from_payload(tval + 3), display_type::percent_fp_one_place, ws, container, cursor_in, lm, fmt);
}
void tf_variable_good_name(TRIGGER_DISPLAY_PARAMS) {
	auto type = trigger_payload(tval[2]).small.values.good;
	cursor_in = ui::add_text(cursor_in, ws.s.economy_m.goods[type].name, fmt, ws, container, lm);
	cursor_in = ui::advance_cursor_by_space(cursor_in, ws.s.gui_m, fmt);
	return display_with_comparison(tval[0],
		scenario::fixed_ui::stockpile, read_float_from_payload(tval + 3), display_type::integer, ws, container, cursor_in, lm, fmt);
}
void tf_religion_nation(TRIGGER_DISPLAY_PARAMS) {
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_religion, ws.s.culture_m.religions[trigger_payload(tval[2]).small.values.religion].name, ws, container, cursor_in, lm, fmt);
}
void tf_religion_nation_reb(TRIGGER_DISPLAY_PARAMS) {
	auto rtag = population::rebel_faction_tag_to_national_tag(to_rebel(from_slot));
	auto rholder = rtag ? ws.w.culture_s.tags_to_holders[rtag] : nations::country_tag();
	auto rr = ws.w.nation_s.nations.get<nation::national_religion>(rholder);

	auto rn = is_valid_index(rr) ? ws.s.culture_m.religions[rr].name : ws.s.fixed_ui_text[scenario::fixed_ui::rebel_religion];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_religion, rn, ws, container, cursor_in, lm, fmt);
}
void tf_religion_nation_from_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rr = bool(to_nation(from_slot)) ? ws.w.nation_s.nations.get<nation::national_religion>(to_nation(from_slot)) : cultures::religion_tag();
	auto rn = is_valid_index(rr) ? ws.s.culture_m.religions[rr].name : ws.s.fixed_ui_text[scenario::fixed_ui::from_nat_religion];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_religion, rn, ws, container, cursor_in, lm, fmt);
}
void tf_religion_nation_this_nation(TRIGGER_DISPLAY_PARAMS) {
	auto rr = bool(to_nation(this_slot)) ? ws.w.nation_s.nations.get<nation::national_religion>(to_nation(this_slot)) : cultures::religion_tag();
	auto rn = is_valid_index(rr) ? ws.s.culture_m.religions[rr].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_nat_religion];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_religion, rn, ws, container, cursor_in, lm, fmt);
}
void tf_religion_nation_this_state(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_state(this_slot)) ? nations::state_owner(ws, to_state(this_slot)) : nations::country_tag();
	auto rr = bool(owner) ? ws.w.nation_s.nations.get<nation::national_religion>(owner) : cultures::religion_tag();
	auto rn = is_valid_index(rr) ? ws.s.culture_m.religions[rr].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_nat_religion];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_religion, rn, ws, container, cursor_in, lm, fmt);
}
void tf_religion_nation_this_province(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_prov(this_slot)) ? provinces::province_owner(ws, to_prov(this_slot)) : nations::country_tag();
	auto rr = bool(owner) ? ws.w.nation_s.nations.get<nation::national_religion>(owner) : cultures::religion_tag();
	auto rn = is_valid_index(rr) ? ws.s.culture_m.religions[rr].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_nat_religion];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_religion, rn, ws, container, cursor_in, lm, fmt);
}
void tf_religion_nation_this_pop(TRIGGER_DISPLAY_PARAMS) {
	auto owner = bool(to_pop(this_slot)) ? population::get_pop_owner(ws, to_pop(this_slot)) : nations::country_tag();
	auto rr = bool(owner) ? ws.w.nation_s.nations.get<nation::national_religion>(owner) : cultures::religion_tag();
	auto rn = is_valid_index(rr) ? ws.s.culture_m.religions[rr].name : ws.s.fixed_ui_text[scenario::fixed_ui::this_nat_religion];
	return display_with_comparison(tval[0],
		scenario::fixed_ui::national_religion, rn, ws, container, cursor_in, lm, fmt);
}



void tf_none_b(TRIGGER_DISPLAY_PARAMS) {
	return cursor_in;
}

ui::xy_pair(*trigger_functions[])(uint16_t const*, world_state&, ui::tagged_gui_object, ui::xy_pair, ui::unlimited_line_manager&, ui::text_format const&,
	const_parameter, const_parameter, const_parameter) = {
	tf_none_b,
	tf_year,
	tf_month,
	tf_port,
	tf_rank,
	tf_technology,
	tf_strata_rich,
	tf_life_rating_province,
	tf_life_rating_state,
	tf_has_empty_adjacent_state_province,
	tf_has_empty_adjacent_state_state,
	tf_state_id_province,
	tf_state_id_state,
	tf_cash_reserves,
	tf_unemployment_nation,
	tf_unemployment_state,
	tf_unemployment_province,
	tf_unemployment_pop,
	tf_is_slave_nation,
	tf_is_slave_state,
	tf_is_slave_province,
	tf_is_slave_pop,
	tf_is_independant,
	tf_has_national_minority_province,
	tf_has_national_minority_state,
	tf_has_national_minority_nation,
	tf_government_nation,
	tf_government_pop,
	tf_capital,
	tf_tech_school,
	tf_primary_culture,
	tf_accepted_culture,
	tf_culture_pop,
	tf_culture_state,
	tf_culture_province,
	tf_culture_nation,
	tf_culture_pop_reb,
	tf_culture_state_reb,
	tf_culture_province_reb,
	tf_culture_nation_reb,
	tf_culture_from_nation,
	tf_culture_this_nation,
	tf_culture_this_state,
	tf_culture_this_pop,
	tf_culture_this_province,
	tf_culture_group_nation,
	tf_culture_group_pop,
	tf_culture_group_reb_nation,
	tf_culture_group_reb_pop,
	tf_culture_group_nation_from_nation,
	tf_culture_group_pop_from_nation,
	tf_culture_group_nation_this_nation,
	tf_culture_group_pop_this_nation,
	tf_culture_group_nation_this_province,
	tf_culture_group_pop_this_province,
	tf_culture_group_nation_this_state,
	tf_culture_group_pop_this_state,
	tf_culture_group_nation_this_pop,
	tf_culture_group_pop_this_pop,
	tf_religion,
	tf_religion_reb,
	tf_religion_from_nation,
	tf_religion_this_nation,
	tf_religion_this_state,
	tf_religion_this_province,
	tf_religion_this_pop,
	tf_terrain_province,
	tf_terrain_pop,
	tf_trade_goods,
	tf_is_secondary_power_pop,
	tf_is_secondary_power_nation,
	tf_has_faction_nation,
	tf_has_faction_pop,
	tf_has_unclaimed_cores,
	tf_is_cultural_union_bool,
	tf_is_cultural_union_this_self_pop,
	tf_is_cultural_union_this_pop,
	tf_is_cultural_union_this_state,
	tf_is_cultural_union_this_province,
	tf_is_cultural_union_this_nation,
	tf_is_cultural_union_this_rebel,
	tf_is_cultural_union_tag_nation,
	tf_is_cultural_union_tag_this_pop,
	tf_is_cultural_union_tag_this_state,
	tf_is_cultural_union_tag_this_province,
	tf_is_cultural_union_tag_this_nation,
	tf_can_build_factory_pop,
	tf_war_pop,
	tf_war_nation,
	tf_war_exhaustion_nation,
	tf_blockade,
	tf_owns,
	tf_controls,
	tf_is_core_integer,
	tf_is_core_this_nation,
	tf_is_core_this_state,
	tf_is_core_this_province,
	tf_is_core_this_pop,
	tf_is_core_from_nation,
	tf_is_core_reb,
	tf_is_core_tag,
	tf_num_of_revolts,
	tf_revolt_percentage,
	tf_num_of_cities_int,
	tf_num_of_cities_from_nation,
	tf_num_of_cities_this_nation,
	tf_num_of_cities_this_state,
	tf_num_of_cities_this_province,
	tf_num_of_cities_this_pop,
	tf_num_of_ports,
	tf_num_of_allies,
	tf_num_of_vassals,
	tf_owned_by_tag,
	tf_owned_by_from_nation,
	tf_owned_by_this_nation,
	tf_owned_by_this_province,
	tf_owned_by_this_state,
	tf_owned_by_this_pop,
	tf_exists_bool,
	tf_exists_tag,
	tf_has_country_flag,
	tf_continent_nation,
	tf_continent_state,
	tf_continent_province,
	tf_continent_pop,
	tf_continent_nation_this,
	tf_continent_state_this,
	tf_continent_province_this,
	tf_continent_pop_this,
	tf_continent_nation_from,
	tf_continent_state_from,
	tf_continent_province_from,
	tf_continent_pop_from,
	tf_casus_belli_tag,
	tf_casus_belli_from,
	tf_casus_belli_this_nation,
	tf_casus_belli_this_state,
	tf_casus_belli_this_province,
	tf_casus_belli_this_pop,
	tf_military_access_tag,
	tf_military_access_from,
	tf_military_access_this_nation,
	tf_military_access_this_state,
	tf_military_access_this_province,
	tf_military_access_this_pop,
	tf_prestige_value,
	tf_prestige_from,
	tf_prestige_this_nation,
	tf_prestige_this_state,
	tf_prestige_this_province,
	tf_prestige_this_pop,
	tf_badboy,
	tf_has_building_state,
	tf_has_building_fort,
	tf_has_building_railroad,
	tf_has_building_naval_base,
	tf_empty,
	tf_is_blockaded,
	tf_has_country_modifier,
	tf_has_province_modifier,
	tf_region,
	tf_tag_tag,
	tf_tag_this_nation,
	tf_tag_this_province,
	tf_tag_from_nation,
	tf_tag_from_province,
	tf_neighbour_tag,
	tf_neighbour_this,
	tf_neighbour_from,
	tf_units_in_province_value,
	tf_units_in_province_from,
	tf_units_in_province_this_nation,
	tf_units_in_province_this_province,
	tf_units_in_province_this_state,
	tf_units_in_province_this_pop,
	tf_war_with_tag,
	tf_war_with_from,
	tf_war_with_this_nation,
	tf_war_with_this_province,
	tf_war_with_this_state,
	tf_war_with_this_pop,
	tf_unit_in_battle,
	tf_total_amount_of_divisions,
	tf_money,
	tf_lost_national,
	tf_is_vassal,
	tf_ruling_party_ideology_pop,
	tf_ruling_party_ideology_nation,
	tf_ruling_party,
	tf_is_ideology_enabled,
	tf_political_reform_want_nation,
	tf_political_reform_want_pop,
	tf_social_reform_want_nation,
	tf_social_reform_want_pop,
	tf_total_amount_of_ships,
	tf_plurality,
	tf_corruption,
	tf_is_state_religion_pop,
	tf_is_state_religion_province,
	tf_is_state_religion_state,
	tf_is_primary_culture_pop,
	tf_is_primary_culture_province,
	tf_is_primary_culture_state,
	tf_is_primary_culture_nation_this_pop,
	tf_is_primary_culture_nation_this_nation,
	tf_is_primary_culture_nation_this_state,
	tf_is_primary_culture_nation_this_province,
	tf_is_primary_culture_state_this_pop,
	tf_is_primary_culture_state_this_nation,
	tf_is_primary_culture_state_this_state,
	tf_is_primary_culture_state_this_province,
	tf_is_primary_culture_province_this_pop,
	tf_is_primary_culture_province_this_nation,
	tf_is_primary_culture_province_this_state,
	tf_is_primary_culture_province_this_province,
	tf_is_primary_culture_pop_this_pop,
	tf_is_primary_culture_pop_this_nation,
	tf_is_primary_culture_pop_this_state,
	tf_is_primary_culture_pop_this_province,
	tf_is_accepted_culture_pop,
	tf_is_accepted_culture_province,
	tf_is_accepted_culture_state,
	tf_is_coastal,
	tf_in_sphere_tag,
	tf_in_sphere_from,
	tf_in_sphere_this_nation,
	tf_in_sphere_this_province,
	tf_in_sphere_this_state,
	tf_in_sphere_this_pop,
	tf_produces_nation,
	tf_produces_state,
	tf_produces_province,
	tf_produces_pop,
	tf_average_militancy_nation,
	tf_average_militancy_state,
	tf_average_militancy_province,
	tf_average_consciousness_nation,
	tf_average_consciousness_state,
	tf_average_consciousness_province,
	tf_is_next_reform_nation,
	tf_is_next_reform_pop,
	tf_rebel_power_fraction,
	tf_recruited_percentage_nation,
	tf_recruited_percentage_pop,
	tf_has_culture_core,
	tf_nationalism,
	tf_is_overseas,
	tf_controlled_by_rebels,
	tf_controlled_by_tag,
	tf_controlled_by_from,
	tf_controlled_by_this_nation,
	tf_controlled_by_this_province,
	tf_controlled_by_this_state,
	tf_controlled_by_this_pop,
	tf_controlled_by_owner,
	tf_controlled_by_reb,
	tf_is_canal_enabled,
	tf_is_state_capital,
	tf_truce_with_tag,
	tf_truce_with_from,
	tf_truce_with_this_nation,
	tf_truce_with_this_province,
	tf_truce_with_this_state,
	tf_truce_with_this_pop,
	tf_total_pops_nation,
	tf_total_pops_state,
	tf_total_pops_province,
	tf_total_pops_pop,
	tf_has_pop_type_nation,
	tf_has_pop_type_state,
	tf_has_pop_type_province,
	tf_has_pop_type_pop,
	tf_has_empty_adjacent_province,
	tf_has_leader,
	tf_ai,
	tf_can_create_vassals,
	tf_is_possible_vassal,
	tf_province_id,
	tf_vassal_of_tag,
	tf_vassal_of_from,
	tf_vassal_of_this_nation,
	tf_vassal_of_this_province,
	tf_vassal_of_this_state,
	tf_vassal_of_this_pop,
	tf_alliance_with_tag,
	tf_alliance_with_from,
	tf_alliance_with_this_nation,
	tf_alliance_with_this_province,
	tf_alliance_with_this_state,
	tf_alliance_with_this_pop,
	tf_has_recently_lost_war,
	tf_is_mobilised,
	tf_mobilisation_size,
	tf_crime_higher_than_education_nation,
	tf_crime_higher_than_education_state,
	tf_crime_higher_than_education_province,
	tf_crime_higher_than_education_pop,
	tf_agree_with_ruling_party,
	tf_is_colonial_state,
	tf_is_colonial_province,
	tf_has_factories,
	tf_in_default_tag,
	tf_in_default_from,
	tf_in_default_this_nation,
	tf_in_default_this_province,
	tf_in_default_this_state,
	tf_in_default_this_pop,
	tf_total_num_of_ports,
	tf_always,
	tf_election,
	tf_has_global_flag,
	tf_is_capital,
	tf_nationalvalue_nation,
	tf_industrial_score_value,
	tf_industrial_score_from_nation,
	tf_industrial_score_this_nation,
	tf_industrial_score_this_pop,
	tf_industrial_score_this_state,
	tf_industrial_score_this_province,
	tf_military_score_value,
	tf_military_score_from_nation,
	tf_military_score_this_nation,
	tf_military_score_this_pop,
	tf_military_score_this_state,
	tf_military_score_this_province,
	tf_civilized_nation,
	tf_civilized_pop,
	tf_civilized_province,
	tf_national_provinces_occupied,
	tf_is_greater_power_nation,
	tf_is_greater_power_pop,
	tf_rich_tax,
	tf_middle_tax,
	tf_poor_tax,
	tf_social_spending_nation,
	tf_social_spending_pop,
	tf_social_spending_province,
	tf_colonial_nation,
	tf_pop_majority_religion_nation,
	tf_pop_majority_religion_state,
	tf_pop_majority_religion_province,
	tf_pop_majority_culture_nation,
	tf_pop_majority_culture_state,
	tf_pop_majority_culture_province,
	tf_pop_majority_issue_nation,
	tf_pop_majority_issue_state,
	tf_pop_majority_issue_province,
	tf_pop_majority_issue_pop,
	tf_pop_majority_ideology_nation,
	tf_pop_majority_ideology_state,
	tf_pop_majority_ideology_province,
	tf_pop_majority_ideology_pop,
	tf_poor_strata_militancy_nation,
	tf_poor_strata_militancy_state,
	tf_poor_strata_militancy_province,
	tf_poor_strata_militancy_pop,
	tf_middle_strata_militancy_nation,
	tf_middle_strata_militancy_state,
	tf_middle_strata_militancy_province,
	tf_middle_strata_militancy_pop,
	tf_rich_strata_militancy_nation,
	tf_rich_strata_militancy_state,
	tf_rich_strata_militancy_province,
	tf_rich_strata_militancy_pop,
	tf_rich_tax_above_poor,
	tf_culture_has_union_tag_pop,
	tf_culture_has_union_tag_nation,
	tf_this_culture_union_tag,
	tf_this_culture_union_from,
	tf_this_culture_union_this_nation,
	tf_this_culture_union_this_province,
	tf_this_culture_union_this_state,
	tf_this_culture_union_this_pop,
	tf_this_culture_union_this_union_nation,
	tf_this_culture_union_this_union_province,
	tf_this_culture_union_this_union_state,
	tf_this_culture_union_this_union_pop,
	tf_minorities_nation,
	tf_minorities_state,
	tf_minorities_province,
	tf_revanchism_nation,
	tf_revanchism_pop,
	tf_has_crime,
	tf_num_of_substates,
	tf_num_of_vassals_no_substates,
	tf_brigades_compare_this,
	tf_brigades_compare_from,
	tf_constructing_cb_tag,
	tf_constructing_cb_from,
	tf_constructing_cb_this_nation,
	tf_constructing_cb_this_province,
	tf_constructing_cb_this_state,
	tf_constructing_cb_this_pop,
	tf_constructing_cb_discovered,
	tf_constructing_cb_progress,
	tf_civilization_progress,
	tf_constructing_cb_type,
	tf_is_our_vassal_tag,
	tf_is_our_vassal_from,
	tf_is_our_vassal_this_nation,
	tf_is_our_vassal_this_province,
	tf_is_our_vassal_this_state,
	tf_is_our_vassal_this_pop,
	tf_substate_of_tag,
	tf_substate_of_from,
	tf_substate_of_this_nation,
	tf_substate_of_this_province,
	tf_substate_of_this_state,
	tf_substate_of_this_pop,
	tf_is_substate,
	tf_great_wars_enabled,
	tf_can_nationalize,
	tf_part_of_sphere,
	tf_is_sphere_leader_of_tag,
	tf_is_sphere_leader_of_from,
	tf_is_sphere_leader_of_this_nation,
	tf_is_sphere_leader_of_this_province,
	tf_is_sphere_leader_of_this_state,
	tf_is_sphere_leader_of_this_pop,
	tf_number_of_states,
	tf_war_score,
	tf_is_releasable_vassal_from,
	tf_is_releasable_vassal_other,
	tf_has_recent_imigration,
	tf_province_control_days,
	tf_is_disarmed,
	tf_big_producer,
	tf_someone_can_form_union_tag_from,
	tf_someone_can_form_union_tag_other,
	tf_social_movement_strength,
	tf_political_movement_strength,
	tf_can_build_factory_in_capital_state,
	tf_social_movement,
	tf_political_movement,
	tf_has_cultural_sphere,
	tf_world_wars_enabled,
	tf_has_pop_culture_pop_this_pop,
	tf_has_pop_culture_state_this_pop,
	tf_has_pop_culture_province_this_pop,
	tf_has_pop_culture_nation_this_pop,
	tf_has_pop_culture_pop,
	tf_has_pop_culture_state,
	tf_has_pop_culture_province,
	tf_has_pop_culture_nation,
	tf_has_pop_religion_pop_this_pop,
	tf_has_pop_religion_state_this_pop,
	tf_has_pop_religion_province_this_pop,
	tf_has_pop_religion_nation_this_pop,
	tf_has_pop_religion_pop,
	tf_has_pop_religion_state,
	tf_has_pop_religion_province,
	tf_has_pop_religion_nation,
	tf_life_needs,
	tf_everyday_needs,
	tf_luxury_needs,
	tf_consciousness_pop,
	tf_consciousness_province,
	tf_consciousness_state,
	tf_consciousness_nation,
	tf_literacy_pop,
	tf_literacy_province,
	tf_literacy_state,
	tf_literacy_nation,
	tf_militancy_pop,
	tf_militancy_province,
	tf_militancy_state,
	tf_militancy_nation,
	tf_military_spending_pop,
	tf_military_spending_province,
	tf_military_spending_state,
	tf_military_spending_nation,
	tf_administration_spending_pop,
	tf_administration_spending_province,
	tf_administration_spending_state,
	tf_administration_spending_nation,
	tf_education_spending_pop,
	tf_education_spending_province,
	tf_education_spending_state,
	tf_education_spending_nation,
	tf_trade_goods_in_state_state,
	tf_trade_goods_in_state_province,
	tf_has_flashpoint,
	tf_flashpoint_tension,
	tf_crisis_exist,
	tf_is_liberation_crisis,
	tf_is_claim_crisis,
	tf_crisis_temperature,
	tf_involved_in_crisis_pop,
	tf_involved_in_crisis_nation,
	tf_rich_strata_life_needs_nation,
	tf_rich_strata_life_needs_state,
	tf_rich_strata_life_needs_province,
	tf_rich_strata_life_needs_pop,
	tf_rich_strata_everyday_needs_nation,
	tf_rich_strata_everyday_needs_state,
	tf_rich_strata_everyday_needs_province,
	tf_rich_strata_everyday_needs_pop,
	tf_rich_strata_luxury_needs_nation,
	tf_rich_strata_luxury_needs_state,
	tf_rich_strata_luxury_needs_province,
	tf_rich_strata_luxury_needs_pop,
	tf_middle_strata_life_needs_nation,
	tf_middle_strata_life_needs_state,
	tf_middle_strata_life_needs_province,
	tf_middle_strata_life_needs_pop,
	tf_middle_strata_everyday_needs_nation,
	tf_middle_strata_everyday_needs_state,
	tf_middle_strata_everyday_needs_province,
	tf_middle_strata_everyday_needs_pop,
	tf_middle_strata_luxury_needs_nation,
	tf_middle_strata_luxury_needs_state,
	tf_middle_strata_luxury_needs_province,
	tf_middle_strata_luxury_needs_pop,
	tf_poor_strata_life_needs_nation,
	tf_poor_strata_life_needs_state,
	tf_poor_strata_life_needs_province,
	tf_poor_strata_life_needs_pop,
	tf_poor_strata_everyday_needs_nation,
	tf_poor_strata_everyday_needs_state,
	tf_poor_strata_everyday_needs_province,
	tf_poor_strata_everyday_needs_pop,
	tf_poor_strata_luxury_needs_nation,
	tf_poor_strata_luxury_needs_state,
	tf_poor_strata_luxury_needs_province,
	tf_poor_strata_luxury_needs_pop,
	tf_diplomatic_influence_tag,
	tf_diplomatic_influence_this_nation,
	tf_diplomatic_influence_this_province,
	tf_diplomatic_influence_from_nation,
	tf_diplomatic_influence_from_province,
	tf_pop_unemployment_nation,
	tf_pop_unemployment_state,
	tf_pop_unemployment_province,
	tf_pop_unemployment_pop,
	tf_pop_unemployment_nation_this_pop,
	tf_pop_unemployment_state_this_pop,
	tf_pop_unemployment_province_this_pop,
	tf_relation_tag,
	tf_relation_this_nation,
	tf_relation_this_province,
	tf_relation_from_nation,
	tf_relation_from_province,
	tf_check_variable,
	tf_upper_house,
	tf_unemployment_by_type_nation,
	tf_unemployment_by_type_state,
	tf_unemployment_by_type_province,
	tf_unemployment_by_type_pop,
	tf_party_loyalty_nation_province_id,
	tf_party_loyalty_from_nation_province_id,
	tf_party_loyalty_province_province_id,
	tf_party_loyalty_from_province_province_id,
	tf_party_loyalty_nation_from_province,
	tf_party_loyalty_from_nation_scope_province,
	tf_can_build_in_province_railroad_no_limit_from_nation,
	tf_can_build_in_province_railroad_yes_limit_from_nation,
	tf_can_build_in_province_railroad_no_limit_this_nation,
	tf_can_build_in_province_railroad_yes_limit_this_nation,
	tf_can_build_in_province_fort_no_limit_from_nation,
	tf_can_build_in_province_fort_yes_limit_from_nation,
	tf_can_build_in_province_fort_no_limit_this_nation,
	tf_can_build_in_province_fort_yes_limit_this_nation,
	tf_can_build_in_province_naval_base_no_limit_from_nation,
	tf_can_build_in_province_naval_base_yes_limit_from_nation,
	tf_can_build_in_province_naval_base_no_limit_this_nation,
	tf_can_build_in_province_naval_base_yes_limit_this_nation,
	tf_can_build_railway_in_capital_yes_whole_state_yes_limit,
	tf_can_build_railway_in_capital_yes_whole_state_no_limit,
	tf_can_build_railway_in_capital_no_whole_state_yes_limit,
	tf_can_build_railway_in_capital_no_whole_state_no_limit,
	tf_can_build_fort_in_capital_yes_whole_state_yes_limit,
	tf_can_build_fort_in_capital_yes_whole_state_no_limit,
	tf_can_build_fort_in_capital_no_whole_state_yes_limit,
	tf_can_build_fort_in_capital_no_whole_state_no_limit,
	tf_work_available_nation,
	tf_work_available_state,
	tf_work_available_province,
	tf_variable_ideology_name_nation,
	tf_variable_ideology_name_state,
	tf_variable_ideology_name_province,
	tf_variable_ideology_name_pop,
	tf_variable_issue_name_nation,
	tf_variable_issue_name_state,
	tf_variable_issue_name_province,
	tf_variable_issue_name_pop,
	tf_variable_issue_group_name_nation,
	tf_variable_issue_group_name_state,
	tf_variable_issue_group_name_province,
	tf_variable_issue_group_name_pop,
	tf_variable_pop_type_name_nation,
	tf_variable_pop_type_name_state,
	tf_variable_pop_type_name_province,
	tf_variable_pop_type_name_pop,
	tf_variable_good_name,
	tf_strata_middle,
	tf_strata_poor,
	tf_party_loyalty_from_province_scope_province,
	tf_can_build_factory_nation,
	tf_can_build_factory_province,
	tf_nationalvalue_pop,
	tf_nationalvalue_province,
	tf_war_exhaustion_pop,
	tf_has_culture_core_province_this_pop,
	tf_tag_pop,
	tf_has_country_flag_pop,
	tf_has_country_flag_province,
	tf_has_country_modifier_province,
	tf_religion_nation,
	tf_religion_nation_reb,
	tf_religion_nation_from_nation,
	tf_religion_nation_this_nation,
	tf_religion_nation_this_state,
	tf_religion_nation_this_province,
	tf_religion_nation_this_pop,
	tf_war_exhaustion_province,
	tf_is_greater_power_province,
	tf_is_cultural_union_pop_this_pop,
	tf_has_building_factory,
	tf_has_building_state_from_province,
	tf_has_building_factory_from_province,
	tf_party_loyalty_generic,
};

void make_trigger_description(
	sys::state& ws,
	text::columnar_layout& layout,
	uint16_t const* tval,
	int32_t primary_slot,
	int32_t this_slot,
	int32_t from_slot,
	int32_t indentation,
	bool show_condition
) {
	

	if((*tval & trigger_codes::is_scope) != 0) {
		return scope_functions[*tval & trigger_codes::code_mask](tval, ws, container, cursor_in, lm, fmt, primary_slot, this_slot, from_slot, show_condition);
	} else {
		return trigger_functions[*tval & trigger_codes::code_mask](tval, ws, container, cursor_in, lm, fmt, primary_slot, this_slot, from_slot);
	}
}

*/

}

}
