#include "trigger_parsing.hpp"
#include "parsers_declarations.hpp"
#include "script_constants.hpp"
#include <algorithm>

namespace parsers {

void tr_scope_and(token_generator& gen, error_handler& err, trigger_building_context& context) {
	context.compiled_trigger.push_back(uint16_t(trigger::generic_scope));
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	parse_trigger_body(gen, err, context);

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}

void tr_scope_or(token_generator& gen, error_handler& err, trigger_building_context& context) {
	context.compiled_trigger.push_back(uint16_t(trigger::generic_scope | trigger::is_disjunctive_scope));
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	parse_trigger_body(gen, err, context);

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_not(token_generator& gen, error_handler& err, trigger_building_context& context) {
	context.compiled_trigger.push_back(uint16_t(trigger::placeholder_not_scope | trigger::is_disjunctive_scope));
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	parse_trigger_body(gen, err, context);

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_any_neighbor_province(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_neighbor_province_scope | trigger::is_existence_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_neighbor_province_scope_state | trigger::is_existence_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_neighbor_province trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_scope_any_neighbor_country(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_neighbor_country_scope_nation | trigger::is_existence_scope));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_neighbor_country_scope_pop | trigger::is_existence_scope));
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_neighbor_country trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_war_countries(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_war_countries_scope_nation));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_war_countries_scope_pop));
	} else {
		gen.discard_group();
		err.accumulated_errors += "war_countries trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_any_greater_power(token_generator& gen, error_handler& err, trigger_building_context& context) {
	context.compiled_trigger.push_back(uint16_t(trigger::x_greater_power_scope | trigger::is_existence_scope));
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_any_country(token_generator& gen, error_handler& err, trigger_building_context& context) {
	context.compiled_trigger.push_back(uint16_t(trigger::x_country_scope | trigger::is_existence_scope));
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_any_owned_province(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_owned_province_scope_nation | trigger::is_existence_scope));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_owned_province_scope_state | trigger::is_existence_scope));
	} else if(context.main_slot == trigger::slot_contents::province) { // any province in a province -> generic scope
		context.compiled_trigger.push_back(uint16_t(trigger::generic_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;
		parse_trigger_body(gen, err, context);
		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
		return;
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_owned_province trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::province;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_any_core(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_core_scope_nation | trigger::is_existence_scope));
	} else if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_core_scope_province | trigger::is_existence_scope));
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_core trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot =
			(context.main_slot == trigger::slot_contents::nation) ? trigger::slot_contents::province : trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_all_core(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_core_scope_nation));
	} else if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_core_scope_province));
	} else {
		gen.discard_group();
		err.accumulated_errors += "all_core trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot =
			(context.main_slot == trigger::slot_contents::nation) ? trigger::slot_contents::province : trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_any_state(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_state_scope | trigger::is_existence_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		auto old_main = context.main_slot;
		context.main_slot = trigger::slot_contents::state;
		parse_trigger_body(gen, err, context);
		context.main_slot = old_main;

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_state trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_scope_any_substate(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_substate_scope | trigger::is_existence_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_substate trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_scope_any_sphere_member(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_sphere_member_scope | trigger::is_existence_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_sphere_member trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_scope_any_pop(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_pop_scope_nation | trigger::is_existence_scope));
	} else if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_pop_scope_province | trigger::is_existence_scope));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_pop_scope_state | trigger::is_existence_scope));
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_pop trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::pop;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_owner(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::owner_scope_province));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::owner_scope_state));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::generic_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;
		parse_trigger_body(gen, err, context);
		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
		return;
	} else {
		gen.discard_group();
		err.accumulated_errors += "owner trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_controller(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::controller_scope));
	} else {
		gen.discard_group();
		err.accumulated_errors += "controller trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_location(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::location_scope));
	} else if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::generic_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
		return;
	} else {
		gen.discard_group();
		err.accumulated_errors += "location trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::province;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_country(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::country_scope_province));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::country_scope_state));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::country_scope_nation));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::country_scope_pop));
	} else {
		gen.discard_group();
		err.accumulated_errors += "country trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_capital_scope(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::capital_scope));
	} else if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::capital_scope_province));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::capital_scope_pop));
	} else {
		gen.discard_group();
		err.accumulated_errors += "capital_scope trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::province;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_this(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::this_scope_province));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::this_scope_state));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::this_scope_nation));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::this_scope_pop));
	} else {
		gen.discard_group();
		err.accumulated_errors += "'this' trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = context.this_slot;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_from(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::from_scope_province));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::from_scope_state));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::from_scope_nation));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::from_scope_pop));
	} else {
		gen.discard_group();
		err.accumulated_errors += "'from' trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = context.from_slot;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_sea_zone(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::sea_zone_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "sea_zone trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_scope_cultural_union(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::cultural_union_scope));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::cultural_union_scope_pop));
	} else {
		gen.discard_group();
		err.accumulated_errors += "cultural_union trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::nation;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_overlord(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::overlord_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "overlord trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_scope_sphere_owner(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::sphere_owner_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "sphere_owner trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_scope_independence(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.from_slot == trigger::slot_contents::rebel) {
		context.compiled_trigger.push_back(uint16_t(trigger::independence_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		auto old_main = context.main_slot;
		context.main_slot = trigger::slot_contents::nation;
		parse_trigger_body(gen, err, context);
		context.main_slot = old_main;

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "independence trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_flashpoint_tag_scope(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::flashpoint_tag_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		auto old_main = context.main_slot;
		context.main_slot = trigger::slot_contents::nation;
		parse_trigger_body(gen, err, context);
		context.main_slot = old_main;

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else {
		gen.discard_group();
		err.accumulated_errors += "flashpoint_tag_scope trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
}
void tr_crisis_state_scope(token_generator& gen, error_handler& err, trigger_building_context& context) {
	context.compiled_trigger.push_back(uint16_t(trigger::crisis_state_scope));
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::state;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_state_scope(token_generator& gen, error_handler& err, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_trigger.push_back(uint16_t(trigger::generic_scope));
		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		parse_trigger_body(gen, err, context);

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
		return;
	}

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_trigger.push_back(uint16_t(trigger::state_scope_province));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_trigger.push_back(uint16_t(trigger::state_scope_pop));
	} else {
		gen.discard_group();
		err.accumulated_errors += "state_scope trigger scope used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ")\n";
		return;
	}
	context.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = context.compiled_trigger.size() - 1;

	auto old_main = context.main_slot;
	context.main_slot = trigger::slot_contents::state;
	parse_trigger_body(gen, err, context);
	context.main_slot = old_main;

	context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
}
void tr_scope_variable(std::string_view name, token_generator& gen, error_handler& err, trigger_building_context& context) {
	std::string label_str{name};
	if(auto it = context.outer_context.map_of_state_names.find(label_str); it != context.outer_context.map_of_state_names.end()) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_provinces_in_variable_region));

		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		context.compiled_trigger.push_back(trigger::payload(it->second).value);

		auto old_main = context.main_slot;
		context.main_slot = trigger::slot_contents::province;
		parse_trigger_body(gen, err, context);
		context.main_slot = old_main;

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else if(auto itr = context.outer_context.map_of_region_names.find(label_str); itr != context.outer_context.map_of_region_names.end()) {
		context.compiled_trigger.push_back(uint16_t(trigger::x_provinces_in_variable_region_proper));

		context.compiled_trigger.push_back(uint16_t(1));
		auto payload_size_offset = context.compiled_trigger.size() - 1;

		context.compiled_trigger.push_back(trigger::payload(itr->second).value);

		auto old_main = context.main_slot;
		context.main_slot = trigger::slot_contents::province;
		parse_trigger_body(gen, err, context);
		context.main_slot = old_main;

		context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
	} else if(is_integer(name.data(), name.data() + name.length())) {
		auto int_value = parse_int(name, 0, err);
		if(0 <= int_value && size_t(int_value) < context.outer_context.original_id_to_prov_id_map.size()) {
			context.compiled_trigger.push_back(uint16_t(trigger::integer_scope));
			context.compiled_trigger.push_back(uint16_t(1));
			auto payload_size_offset = context.compiled_trigger.size() - 1;
			context.compiled_trigger.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[int_value]).value);

			auto old_main = context.main_slot;
			context.main_slot = trigger::slot_contents::province;
			parse_trigger_body(gen, err, context);
			context.main_slot = old_main;

			context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
		} else {
			err.accumulated_errors += "integer trigger scope given an invalid province id (" + err.file_name + ")\n";
			context.compiled_trigger.push_back(trigger::payload(dcon::province_id()).value);
		}

	} else if(name.length() == 3) {
		if(auto itb = context.outer_context.map_of_ident_names.find(nations::tag_to_int(name[0], name[1], name[2]));
				itb != context.outer_context.map_of_ident_names.end()) {
			context.compiled_trigger.push_back(uint16_t(trigger::tag_scope));
			context.compiled_trigger.push_back(uint16_t(1));
			auto payload_size_offset = context.compiled_trigger.size() - 1;
			context.compiled_trigger.push_back(trigger::payload(itb->second).value);

			auto old_main = context.main_slot;
			context.main_slot = trigger::slot_contents::nation;
			parse_trigger_body(gen, err, context);
			context.main_slot = old_main;

			context.compiled_trigger[payload_size_offset] = uint16_t(context.compiled_trigger.size() - payload_size_offset);
		} else {
			err.accumulated_errors += "unknown scope " + label_str + " introduced in a trigger (" + err.file_name + ")\n";
			gen.discard_group();
			return;
		}
	} else {
		err.accumulated_errors += "unknown scope " + label_str + " introduced in a trigger (" + err.file_name + ")\n";
		gen.discard_group();
		return;
	}
}

inline void invert_trigger_internal(uint16_t* source) {
	if((source[0] & trigger::code_mask) >= trigger::first_scope_code) {
		auto const neg_disjunctive_bit = trigger::is_disjunctive_scope & ~source[0];
		auto const neg_existence_bit =
				scope_has_any_all(source[0] & trigger::code_mask) ? (trigger::is_existence_scope & ~source[0]) : 0;
		auto const masked_source = source[0] & ~(trigger::is_disjunctive_scope | trigger::is_existence_scope);

		source[0] = uint16_t(masked_source | neg_disjunctive_bit | neg_existence_bit);
	} else {
		auto const inverse_association = invert_association(uint16_t(source[0] & trigger::association_mask));
		source[0] = uint16_t((source[0] & ~trigger::association_mask) | inverse_association);
	}
}

void invert_trigger(uint16_t* source) {
	trigger::recurse_over_triggers(source, invert_trigger_internal);
}

bool scope_is_empty(uint16_t const* source) {
	return trigger::get_trigger_scope_payload_size(source) <= 1 + trigger::trigger_scope_data_payload(source[0]);
}
// precondition: scope known to not be empty
bool scope_has_single_member(uint16_t const* source) {
	auto const data_offset = 2 + trigger::trigger_scope_data_payload(source[0]);
	return trigger::get_trigger_scope_payload_size(source) == data_offset + trigger::get_trigger_payload_size(source + data_offset);
}

// yields new source size
int32_t simplify_trigger(uint16_t* source) {
	assert((0 <= (*source & trigger::code_mask) && (*source & trigger::code_mask) < trigger::first_invalid_code) ||
				 (*source & trigger::code_mask) == trigger::placeholder_not_scope);
	if((source[0] & trigger::code_mask) >= trigger::first_scope_code) {
		if(scope_is_empty(source)) {
			return 0; // simplify an empty scope to nothing
		}

		// simplify each member
		auto source_size = 1 + trigger::get_trigger_scope_payload_size(source);
		auto const first_member = source + 2 + trigger::trigger_scope_data_payload(source[0]);

		{
			auto sub_units_start = first_member;
			while(sub_units_start < source + source_size) {
				auto const old_size = 1 + trigger::get_trigger_payload_size(sub_units_start);
				auto const new_size = simplify_trigger(sub_units_start);

				if(new_size != old_size) { // has been simplified, assumes that new size always <= old size
					std::copy(sub_units_start + old_size, source + source_size, sub_units_start + new_size);
					source_size -= (old_size - new_size);
				}
				sub_units_start += new_size;
			}
		}

		source[1] = uint16_t(source_size - 1);

		if((source[0] & trigger::code_mask) == trigger::placeholder_not_scope) { // remove not scopes
			invert_trigger(source);
			source[0] &= ~trigger::code_mask;
			/*
			if((source[0] & trigger::is_disjunctive_scope) != 0) {
				source[0] &= ~trigger::is_disjunctive_scope;
			} else {
				source[0] |= trigger::is_disjunctive_scope;
			}
			*/
			source[0] |= trigger::generic_scope;
		}

		if(source[0] == trigger::generic_scope || source[0] == (trigger::generic_scope | trigger::is_disjunctive_scope)) {
			auto sub_units_start = first_member;
			while(sub_units_start < source + source_size) {
				auto const size = 1 + trigger::get_trigger_payload_size(sub_units_start);
				if(sub_units_start[0] == source[0]) {
					std::copy(sub_units_start + 2, source + source_size, sub_units_start);
					source_size -= 2;
				} else {
					sub_units_start += size;
				}
			}
			source[1] = uint16_t(source_size - 1);
		}

		if((source[0] & trigger::code_mask) >= trigger::first_scope_code && scope_has_single_member(source)) {
			if((source[0] & trigger::code_mask) == trigger::generic_scope) { // remove single-member generic scopes
				std::copy(source + 2, source + source_size, source);
				source_size -= 2;
			} else if((first_member[0] & trigger::code_mask) == trigger::generic_scope) {
				// scope contains single generic scope

				source[1] = uint16_t(first_member[1] + trigger::trigger_scope_data_payload(source[0]));
				source[0] = uint16_t((source[0] & ~trigger::is_disjunctive_scope) | (first_member[0] & trigger::is_disjunctive_scope));

				std::copy(first_member + 2, source + source_size, first_member);

				source_size -= 2;
			}
		}

		if((source[0] & trigger::code_mask) >= trigger::first_scope_code && scope_is_empty(source)) {
			return 0; // simplify an empty scope to nothing
		}
		return source_size;
	} else {
		return 1 + trigger::get_trigger_non_scope_payload_size(source); // non scopes cannot be simplified
	}
}

dcon::trigger_key make_trigger(token_generator& gen, error_handler& err, trigger_building_context& context) {
	tr_scope_and(gen, err, context);

	auto const new_size = simplify_trigger(context.compiled_trigger.data());
	context.compiled_trigger.resize(static_cast<size_t>(new_size));
	
	return context.outer_context.state.commit_trigger_data(context.compiled_trigger);
}

void make_stored_trigger(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	trigger_building_context tcontext{ context , trigger::slot_contents::empty, trigger::slot_contents::empty , trigger::slot_contents::empty };

	tcontext.compiled_trigger.push_back(uint16_t(trigger::generic_scope));
	tcontext.compiled_trigger.push_back(uint16_t(1));
	auto payload_size_offset = tcontext.compiled_trigger.size() - 1;

	parse_stored_condition(gen, err, tcontext);

	tcontext.compiled_trigger[payload_size_offset] = uint16_t(tcontext.compiled_trigger.size() - payload_size_offset);

	auto const new_size = simplify_trigger(tcontext.compiled_trigger.data());
	tcontext.compiled_trigger.resize(static_cast<size_t>(new_size));

	auto by_name = context.map_of_stored_triggers.find(std::string(name));

	bool can_store = (by_name == context.map_of_stored_triggers.end());
	if(!can_store) {
		can_store = true;
		auto& prev_stored = by_name->second;
		for(auto e : prev_stored) {
			if(e.main_slot == tcontext.main_slot && e.from_slot == tcontext.from_slot && e.this_slot == tcontext.this_slot) {
				err.accumulated_errors += "two stored triggers with the name " + std::string(name) + " defined for the same types of parameters (" + err.file_name + ")\n";
				can_store = false;
			}
		}
	}

	if(can_store) {
		auto saved_trigger = context.state.commit_trigger_data(tcontext.compiled_trigger);
		auto name_id = text::find_or_add_key(context.state, name);

		auto stored_t_id = context.state.world.create_stored_trigger();
		context.state.world.stored_trigger_set_function(stored_t_id, saved_trigger);
		context.state.world.stored_trigger_set_name(stored_t_id, name_id);

		if(by_name == context.map_of_stored_triggers.end()) {
			context.map_of_stored_triggers.insert_or_assign(std::string(name), std::vector<saved_stored_condition>{ saved_stored_condition{ stored_t_id, tcontext.main_slot, tcontext.this_slot, tcontext.from_slot } });
		} else {
			by_name->second.push_back(saved_stored_condition{ stored_t_id, tcontext.main_slot, tcontext.this_slot, tcontext.from_slot });
		}
	}
}

void make_value_modifier_segment(token_generator& gen, error_handler& err, trigger_building_context& context) {
	auto old_factor = context.factor;
	context.factor = 0.0f;

	tr_scope_and(gen, err, context);

	auto new_factor = context.factor;
	context.factor = old_factor;

	auto const new_size = simplify_trigger(context.compiled_trigger.data());
	context.compiled_trigger.resize(static_cast<size_t>(new_size));

	auto tkey = context.outer_context.state.commit_trigger_data(context.compiled_trigger);
	context.compiled_trigger.clear();

	context.outer_context.state.value_modifier_segments.push_back(sys::value_modifier_segment{new_factor, tkey});
}

dcon::value_modifier_key make_value_modifier(token_generator& gen, error_handler& err, trigger_building_context& context) {
	auto old_count = context.outer_context.state.value_modifier_segments.size();
	value_modifier_definition result = parse_value_modifier_definition(gen, err, context);

	auto multiplier = result.factor ? *result.factor : 1.0f;
	auto overall_factor = result.base;
	auto new_count = context.outer_context.state.value_modifier_segments.size();

	return context.outer_context.state.value_modifiers.push_back(
			sys::value_modifier_description{ multiplier, overall_factor, uint16_t(old_count), uint16_t(new_count - old_count)});
}

void trigger_body::is_canal_enabled(association_type a, int32_t value, error_handler& err, int32_t line,
		trigger_building_context& context) {
	if(1 <= value && value <= int32_t(context.outer_context.state.province_definitions.canals.size())) {
		context.compiled_trigger.push_back(uint16_t(trigger::is_canal_enabled | association_to_bool_code(a)));
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	} else {
		err.accumulated_errors +=
				"canal index " + std::to_string(value) + " out of range (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		return;
	}
}

void trigger_body::badboy(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::badboy | association_to_trigger_code(a)));
	} else {
		err.accumulated_errors += "badboy trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
															"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
		return;
	}
	context.add_float_to_payload(value * context.outer_context.state.defines.badboy_limit);
}

void trigger_body::ruling_party(association_type a, std::string_view value, error_handler& err, int32_t line,
		trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {

		context.compiled_trigger.push_back(uint16_t(trigger::ruling_party | association_to_bool_code(a)));
	} else {
		err.accumulated_errors += "ruling_party trigger used in an incorrect scope type " +
															slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
															std::to_string(line) + ")\n";
		return;
	}
	auto name_id = text::find_or_add_key(context.outer_context.state, value);
	context.add_int32_t_to_payload(name_id.index());
}

void trigger_body::has_leader(association_type a, std::string_view value, error_handler& err, int32_t line,
		trigger_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_trigger.push_back(uint16_t(trigger::has_leader | association_to_bool_code(a)));
	} else {
		err.accumulated_errors += "has_leader trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
															"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
		return;
	}
	auto name_id = context.outer_context.state.add_unit_name(value);
	context.add_int32_t_to_payload(name_id.index());
}

} // namespace parsers
