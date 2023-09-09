#include "effect_parsing.hpp"
#include "parsers_declarations.hpp"

namespace parsers {

dcon::trigger_key ef_limit(token_generator& gen, error_handler& err, effect_building_context& context) {
	trigger_building_context tr_context{context.outer_context, context.main_slot, context.this_slot, context.from_slot};
	return make_trigger(gen, err, tr_context);
}

void ef_scope_hidden_tooltip(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;

	context.compiled_effect.push_back(uint16_t(effect::generic_scope | effect::scope_has_limit));
	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;
	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
}

void ef_scope_any_neighbor_province(token_generator& gen, error_handler& err, effect_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		auto old_limit_offset = context.limit_position;

		context.compiled_effect.push_back(uint16_t(effect::x_neighbor_province_scope | effect::scope_has_limit));
		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;
		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_neighbor_province effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}
}

void ef_scope_any_neighbor_country(token_generator& gen, error_handler& err, effect_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		auto old_limit_offset = context.limit_position;

		context.compiled_effect.push_back(uint16_t(effect::x_neighbor_country_scope | effect::scope_has_limit));
		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;
		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_neighbor_province effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}
}

void ef_scope_any_country(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	context.compiled_effect.push_back(uint16_t(effect::x_country_scope | effect::scope_has_limit));
	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::nation;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_random_country(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::nation)
		context.compiled_effect.push_back(uint16_t(effect::x_country_scope | effect::is_random_scope | effect::scope_has_limit));
	else
		context.compiled_effect.push_back(
				uint16_t(effect::x_country_scope_nation | effect::is_random_scope | effect::scope_has_limit));

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::nation;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_random_neighbor_province(token_generator& gen, error_handler& err, effect_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		auto old_limit_offset = context.limit_position;

		context.compiled_effect.push_back(
				uint16_t(effect::x_neighbor_province_scope | effect::is_random_scope | effect::scope_has_limit));
		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;
		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_neighbor_province effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}
}

void ef_scope_random_empty_neighbor_province(token_generator& gen, error_handler& err, effect_building_context& context) {
	if(context.main_slot == trigger::slot_contents::province) {
		auto old_limit_offset = context.limit_position;

		context.compiled_effect.push_back(
				uint16_t(effect::x_empty_neighbor_province_scope | effect::is_random_scope | effect::scope_has_limit));
		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;
		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_neighbor_province effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}
}

void ef_scope_any_greater_power(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	context.compiled_effect.push_back(uint16_t(effect::x_greater_power_scope | effect::scope_has_limit));
	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::nation;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_poor_strata(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::poor_strata_scope_province | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::poor_strata_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::poor_strata_scope_nation | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "poor_strata effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::pop;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_middle_strata(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::middle_strata_scope_province | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::middle_strata_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::middle_strata_scope_nation | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "middle_strata effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::pop;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_rich_strata(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::rich_strata_scope_province | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::rich_strata_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::rich_strata_scope_nation | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "rich_strata effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::pop;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_random_pop(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::x_pop_scope_province | effect::is_random_scope | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::x_pop_scope_state | effect::is_random_scope | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::x_pop_scope_nation | effect::is_random_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "random_pop effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::pop;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_scope_random_owned(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::x_owned_scope_state | effect::is_random_scope | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::x_owned_scope_nation | effect::is_random_scope | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::province) { // we parse this as basically a do-nothing scope
		context.compiled_effect.push_back(uint16_t(effect::generic_scope | effect::scope_has_limit));
		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;
		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;
		return;
	} else {
		gen.discard_group();
		err.accumulated_errors += "random_owned effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::province;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_any_owned(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::x_owned_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::x_owned_scope_nation | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::province) { // we parse this as basically a do-nothing scope
		context.compiled_effect.push_back(uint16_t(effect::generic_scope | effect::scope_has_limit));
		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;
		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;
		return;
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_owned effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::province;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_scope_all_core(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::x_core_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "all_core effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::province;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_scope_any_state(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::x_state_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_state effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::state;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_any_substate(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;

	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::x_substate_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_substate effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
}

void ef_scope_random_state(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::x_state_scope | effect::is_random_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "random_state effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::state;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_any_pop(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::x_pop_scope_province | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::x_pop_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::x_pop_scope_nation | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "any_pop effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::pop;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_scope_owner(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::owner_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::owner_scope_province | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "owner effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::nation;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_scope_controller(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::controller_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "controller effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::nation;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_scope_location(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_effect.push_back(uint16_t(effect::location_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "location effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::province;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_scope_country(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::country_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_effect.push_back(uint16_t(effect::country_scope_pop | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "country effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::nation;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_capital_scope(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::capital_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "capital_scope effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::province;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_scope_this(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::this_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::this_scope_province | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_effect.push_back(uint16_t(effect::this_scope_pop | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::this_scope_nation | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "this effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = context.this_slot;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_from(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::from_scope_state | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::from_scope_province | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_effect.push_back(uint16_t(effect::from_scope_pop | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::from_scope_nation | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "from effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = context.from_slot;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_sea_zone(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::sea_zone_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "sea_zone effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
}

void ef_scope_cultural_union(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;

	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::cultural_union_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "cultural_union effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
}

void ef_scope_overlord(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;

	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::overlord_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "overlord effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
}

void ef_scope_sphere_owner(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;

	if(context.main_slot == trigger::slot_contents::nation) {
		context.compiled_effect.push_back(uint16_t(effect::sphere_owner_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "sphere_owner effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
}

void ef_scope_independence(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::rebel) {
		context.compiled_effect.push_back(uint16_t(effect::independence_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "independence effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::nation;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_flashpoint_tag_scope(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::state) {
		context.compiled_effect.push_back(uint16_t(effect::flashpoint_tag_scope | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "flashpoint_tag_scope effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::nation;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}
void ef_crisis_state_scope(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	context.compiled_effect.push_back(uint16_t(effect::crisis_state_scope | effect::scope_has_limit));

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::state;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_state_scope(token_generator& gen, error_handler& err, effect_building_context& context) {
	if(context.main_slot == trigger::slot_contents::state) { // for the case where state context is added within state context
		auto old_limit_offset = context.limit_position;

		context.compiled_effect.push_back(uint16_t(effect::generic_scope | effect::scope_has_limit));
		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;
		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;

		return;
	}

	auto old_limit_offset = context.limit_position;
	auto old_main = context.main_slot;

	if(context.main_slot == trigger::slot_contents::province) {
		context.compiled_effect.push_back(uint16_t(effect::state_scope_province | effect::scope_has_limit));
	} else if(context.main_slot == trigger::slot_contents::pop) {
		context.compiled_effect.push_back(uint16_t(effect::state_scope_pop | effect::scope_has_limit));
	} else {
		gen.discard_group();
		err.accumulated_errors += "state_scope effect scope used in an incorrect scope type (" + err.file_name + ")\n";
		return;
	}

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	context.main_slot = trigger::slot_contents::state;
	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;
	context.main_slot = old_main;
}

void ef_scope_random(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;

	context.compiled_effect.push_back(uint16_t(effect::random_scope | effect::scope_has_limit));

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
	context.compiled_effect.push_back(uint16_t(0));

	auto read_body = parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.compiled_effect[payload_size_offset + 2] = uint16_t(read_body.chance);
	context.limit_position = old_limit_offset;
}

int32_t add_to_random_list(std::string_view label, token_generator& gen, error_handler& err, effect_building_context& context) {
	auto ivalue = parse_int(label, 0, err);
	context.compiled_effect.push_back(uint16_t(ivalue));

	auto old_limit_offset = context.limit_position;
	context.compiled_effect.push_back(uint16_t(effect::generic_scope | effect::scope_has_limit));
	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;
	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	parse_effect_body(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.limit_position = old_limit_offset;

	return ivalue;
}

void ef_scope_random_list(token_generator& gen, error_handler& err, effect_building_context& context) {
	auto old_limit_offset = context.limit_position;

	context.compiled_effect.push_back(uint16_t(effect::random_list_scope));

	context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = context.compiled_effect.size() - 1;

	context.limit_position = context.compiled_effect.size();
	context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

	auto random_set = parse_ef_random_list(gen, err, context);

	context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
	context.compiled_effect[payload_size_offset + 1] = uint16_t(std::max(random_set.chances_sum, 1));

	context.limit_position = old_limit_offset;
}

void ef_scope_variable(std::string_view label, token_generator& gen, error_handler& err, effect_building_context& context) {
	std::string str_label{label};
	if(auto it = context.outer_context.map_of_state_names.find(str_label); it != context.outer_context.map_of_state_names.end()) {
		auto old_limit_offset = context.limit_position;
		auto old_main = context.main_slot;

		context.compiled_effect.push_back(uint16_t(effect::region_scope | effect::scope_has_limit));

		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;

		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		context.compiled_effect.push_back(trigger::payload(it->second).value);

		context.main_slot = trigger::slot_contents::province;
		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;
		context.main_slot = old_main;
	} else if(auto itb = context.outer_context.map_of_poptypes.find(str_label);
						itb != context.outer_context.map_of_poptypes.end()) {

		if(context.main_slot == trigger::slot_contents::nation)
			context.compiled_effect.push_back(uint16_t(effect::pop_type_scope_nation | effect::scope_has_limit));
		else if(context.main_slot == trigger::slot_contents::state)
			context.compiled_effect.push_back(uint16_t(effect::pop_type_scope_state | effect::scope_has_limit));
		else if(context.main_slot == trigger::slot_contents::province)
			context.compiled_effect.push_back(uint16_t(effect::pop_type_scope_province | effect::scope_has_limit));
		else {
			gen.discard_group();
			err.accumulated_errors += "pop type effect scope used in an incorrect scope type (" + err.file_name + ")\n";
			return;
		}

		auto old_limit_offset = context.limit_position;
		auto old_main = context.main_slot;

		context.compiled_effect.push_back(uint16_t(0));
		auto payload_size_offset = context.compiled_effect.size() - 1;

		context.limit_position = context.compiled_effect.size();
		context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);

		context.compiled_effect.push_back(trigger::payload(itb->second).value);

		context.main_slot = trigger::slot_contents::pop;
		parse_effect_body(gen, err, context);

		context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
		context.limit_position = old_limit_offset;
		context.main_slot = old_main;
	} else if(is_integer(label.data(), label.data() + label.length())) {
		auto value = parse_int(label, 0, err);
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {

			auto old_limit_offset = context.limit_position;
			auto old_main = context.main_slot;

			context.compiled_effect.push_back(uint16_t(effect::integer_scope | effect::scope_has_limit));

			context.compiled_effect.push_back(uint16_t(0));
			auto payload_size_offset = context.compiled_effect.size() - 1;

			context.limit_position = context.compiled_effect.size();
			context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
			context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[value]).value);

			context.main_slot = trigger::slot_contents::province;
			parse_effect_body(gen, err, context);

			context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
			context.limit_position = old_limit_offset;

			context.main_slot = old_main;
		} else {
			gen.discard_group();
			err.accumulated_errors += "Invalid province id used as an effect scope (" + err.file_name + ")\n";
			return;
		}
	} else if(label.length() == 3) {
		if(auto itc = context.outer_context.map_of_ident_names.find(nations::tag_to_int(label[0], label[1], label[2]));
				itc != context.outer_context.map_of_ident_names.end()) {

			auto old_limit_offset = context.limit_position;
			auto old_main = context.main_slot;

			context.compiled_effect.push_back(uint16_t(effect::tag_scope | effect::scope_has_limit));

			context.compiled_effect.push_back(uint16_t(0));
			auto payload_size_offset = context.compiled_effect.size() - 1;

			context.limit_position = context.compiled_effect.size();
			context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
			context.compiled_effect.push_back(trigger::payload(itc->second).value);

			context.main_slot = trigger::slot_contents::nation;
			parse_effect_body(gen, err, context);

			context.compiled_effect[payload_size_offset] = uint16_t(context.compiled_effect.size() - payload_size_offset);
			context.limit_position = old_limit_offset;

			context.main_slot = old_main;
		} else {
			err.accumulated_errors += "Invalid tag used as an effect scope (" + err.file_name + ")\n";
			return;
		}
	} else {
		gen.discard_group();
		err.accumulated_errors += "unknown effect scope " + str_label + " (" + err.file_name + ")\n";
		return;
	}
}

int32_t simplify_effect(uint16_t* source) {
	assert(0 <= (*source & effect::code_mask) && (*source & effect::code_mask) < effect::first_invalid_code);
	if((source[0] & effect::code_mask) >= effect::first_scope_code) {
		auto source_size = 1 + effect::get_effect_scope_payload_size(source);

		if((source[0] & effect::code_mask) == effect::random_list_scope) {
			auto sub_units_start = source + 4; // [code] + [payload size] + [chances total] + [first sub effect chance]

			while(sub_units_start < source + source_size) {
				auto const old_size = 1 + effect::get_generic_effect_payload_size(sub_units_start);
				auto const new_size = simplify_effect(sub_units_start);
				if(new_size > 0) {
					if(new_size != old_size) { // has been simplified, assumes that new size always <= old size
						std::copy(sub_units_start + old_size, source + source_size, sub_units_start + new_size);
						source_size -= (old_size - new_size);
					}
					sub_units_start += new_size + 1;
				} else {
					std::copy(sub_units_start + old_size, source + source_size, sub_units_start - 1);
					source_size -= (1 + old_size);
				}
			}
		} else {
			auto sub_units_start = source + 2 + effect::effect_scope_data_payload(source[0]);

			while(sub_units_start < source + source_size) {
				auto const old_size = 1 + effect::get_generic_effect_payload_size(sub_units_start);
				auto const new_size = simplify_effect(sub_units_start);

				if(new_size != old_size) { // has been simplified, assumes that new size always <= old size
					std::copy(sub_units_start + old_size, source + source_size, sub_units_start + new_size);
					source_size -= (old_size - new_size);
				}
				sub_units_start += new_size;
			}
		}

		source[1] = uint16_t(source_size - 1);

		if((source[0] & effect::scope_has_limit) != 0 && !bool(trigger::payload(source[2]).tr_id)) {
			std::copy(source + 3, source + source_size, source + 2);
			--source_size;
			source[0] = uint16_t(source[0] & ~effect::scope_has_limit);
			source[1] = uint16_t(source_size - 1);
		}

		if((source[0] & effect::code_mask) == effect::generic_scope) {
			if(source_size == 2) {
				return 0; // simplify empty scope to nothing
			} else if(((source[0] & effect::scope_has_limit) == 0) && effect::effect_scope_has_single_member(source)) {
				std::copy(source + 2, source + source_size, source);
				source_size -= 2;
			}
		}

		return source_size;
	} else {
		return 1 + effect::get_effect_non_scope_payload_size(source); // non scopes cannot be simplified
	}
}

template<typename T>
void recurse_over_effects(uint16_t* source, T const& f) {
	f(source);

	if((source[0] & effect::code_mask) >= effect::first_scope_code) {
		if((source[0] & effect::code_mask) == effect::random_list_scope) {
			auto const source_size = 1 + effect::get_generic_effect_payload_size(source);

			auto sub_units_start = source + 4; // [code] + [payload size] + [chances total] + [first sub effect chance]
			while(sub_units_start < source + source_size) {
				recurse_over_effects(sub_units_start, f);
				sub_units_start += 2 + effect::get_generic_effect_payload_size(sub_units_start); // each member preceded by uint16_t
			}
		} else {
			auto const source_size = 1 + effect::get_generic_effect_payload_size(source);

			auto sub_units_start = source + 2 + effect::effect_scope_data_payload(source[0]);
			while(sub_units_start < source + source_size) {
				recurse_over_effects(sub_units_start, f);
				sub_units_start += 1 + effect::get_generic_effect_payload_size(sub_units_start);
			}
		}
	}
}

dcon::effect_key make_effect(token_generator& gen, error_handler& err, effect_building_context& context) {
	ef_scope_hidden_tooltip(gen, err, context);

	if(context.compiled_effect.size() >= std::numeric_limits<uint16_t>::max()) {
		err.accumulated_errors += "effect is " + std::to_string(context.compiled_effect.size()) +
															" cells big, which exceeds 64 KB bytecode limit (" + err.file_name + ")";
		return dcon::effect_key{0};
	}

	auto const new_size = simplify_effect(context.compiled_effect.data());
	context.compiled_effect.resize(static_cast<size_t>(new_size));

	return context.outer_context.state.commit_effect_data(context.compiled_effect);
}

void ef_province_event::id(association_type t, int32_t value, error_handler& err, int32_t line,
		effect_building_context& context) {
	if(auto it = context.outer_context.map_of_provincial_events.find(value);
			it != context.outer_context.map_of_provincial_events.end()) {
		if(it->second.id) {
			id_ = it->second.id;
		} else {
			id_ = context.outer_context.state.world.create_provincial_event();
			it->second.id = id_;
			it->second.main_slot = trigger::slot_contents::province;
			it->second.this_slot = trigger::slot_contents::nation;
			it->second.from_slot = context.this_slot;
		}
	} else {
		id_ = context.outer_context.state.world.create_provincial_event();
		context.outer_context.map_of_provincial_events.insert_or_assign(value,
				pending_prov_event{id_, trigger::slot_contents::province, trigger::slot_contents::province, context.this_slot});
	}
}

void ef_country_event::id(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
	if(auto it = context.outer_context.map_of_national_events.find(value);
			it != context.outer_context.map_of_national_events.end()) {
		if(it->second.id) {
			id_ = it->second.id;
		} else {
			id_ = context.outer_context.state.world.create_national_event();
			it->second.id = id_;
			it->second.main_slot = trigger::slot_contents::nation;
			it->second.this_slot = trigger::slot_contents::nation;
			it->second.from_slot = context.this_slot;
		}
	} else {
		id_ = context.outer_context.state.world.create_national_event();
		context.outer_context.map_of_national_events.insert_or_assign(value,
				pending_nat_event{id_, trigger::slot_contents::nation, trigger::slot_contents::nation, context.this_slot});
	}
}

void effect_body::country_event(association_type t, int32_t value, error_handler& err, int32_t line,
		effect_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		if(context.this_slot == trigger::slot_contents::nation)
			context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_this_nation));
		else if(context.this_slot == trigger::slot_contents::province)
			context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_this_province));
		else if(context.this_slot == trigger::slot_contents::state)
			context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_this_state));
		else if(context.this_slot == trigger::slot_contents::pop)
			context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_this_pop));
		else {
			err.accumulated_errors +=
					"country_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(auto it = context.outer_context.map_of_national_events.find(value);
				it != context.outer_context.map_of_national_events.end()) {
			if(it->second.id) {
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				auto ev_id = context.outer_context.state.world.create_national_event();
				it->second.id = ev_id;
				it->second.main_slot = trigger::slot_contents::nation;
				it->second.this_slot = trigger::slot_contents::nation;
				it->second.from_slot = context.this_slot;
				context.compiled_effect.push_back(trigger::payload(ev_id).value);
			}
		} else {
			auto ev_id = context.outer_context.state.world.create_national_event();
			context.outer_context.map_of_national_events.insert_or_assign(value,
					pending_nat_event{ev_id, trigger::slot_contents::nation, trigger::slot_contents::nation, context.this_slot});
			context.compiled_effect.push_back(trigger::payload(ev_id).value);
		}
	} else if(context.main_slot == trigger::slot_contents::province) {
		if(context.this_slot == trigger::slot_contents::nation)
			context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_province_this_nation));
		else if(context.this_slot == trigger::slot_contents::province)
			context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_province_this_province));
		else if(context.this_slot == trigger::slot_contents::state)
			context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_province_this_state));
		else if(context.this_slot == trigger::slot_contents::pop)
			context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_province_this_pop));
		else {
			err.accumulated_errors +=
					"country_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(auto it = context.outer_context.map_of_national_events.find(value);
				it != context.outer_context.map_of_national_events.end()) {
			if(it->second.id) {
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				auto ev_id = context.outer_context.state.world.create_national_event();
				it->second.id = ev_id;
				it->second.main_slot = trigger::slot_contents::nation;
				it->second.this_slot = trigger::slot_contents::nation;
				it->second.from_slot = context.this_slot;
				context.compiled_effect.push_back(trigger::payload(ev_id).value);
			}
		} else {
			auto ev_id = context.outer_context.state.world.create_national_event();
			context.outer_context.map_of_national_events.insert_or_assign(value,
					pending_nat_event{ev_id, trigger::slot_contents::nation, trigger::slot_contents::nation, context.this_slot});
			context.compiled_effect.push_back(trigger::payload(ev_id).value);
		}
	} else {
		err.accumulated_errors +=
				"country_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		return;
	}
}

void effect_body::province_event(association_type t, int32_t value, error_handler& err, int32_t line,
		effect_building_context& context) {
	if(context.main_slot == trigger::slot_contents::nation) {
		if(context.this_slot == trigger::slot_contents::nation)
			context.compiled_effect.push_back(uint16_t(effect::province_event_immediate_this_nation));
		else if(context.this_slot == trigger::slot_contents::province)
			context.compiled_effect.push_back(uint16_t(effect::province_event_immediate_this_province));
		else if(context.this_slot == trigger::slot_contents::state)
			context.compiled_effect.push_back(uint16_t(effect::province_event_immediate_this_state));
		else if(context.this_slot == trigger::slot_contents::pop)
			context.compiled_effect.push_back(uint16_t(effect::province_event_immediate_this_pop));
		else {
			err.accumulated_errors +=
					"province_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(auto it = context.outer_context.map_of_provincial_events.find(value);
				it != context.outer_context.map_of_provincial_events.end()) {
			if(it->second.id) {
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				auto ev_id = context.outer_context.state.world.create_provincial_event();
				it->second.id = ev_id;
				it->second.main_slot = trigger::slot_contents::province;
				it->second.this_slot = trigger::slot_contents::nation;
				it->second.from_slot = context.this_slot;
				context.compiled_effect.push_back(trigger::payload(ev_id).value);
			}
		} else {
			auto ev_id = context.outer_context.state.world.create_provincial_event();
			context.outer_context.map_of_provincial_events.insert_or_assign(value,
					pending_prov_event{ev_id, trigger::slot_contents::province, trigger::slot_contents::province, context.this_slot});
			context.compiled_effect.push_back(trigger::payload(ev_id).value);
		}
	} else {
		err.accumulated_errors +=
				"province_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		return;
	}
}

void effect_body::define_general(ef_define_general const& value, error_handler& err, int32_t line,
		effect_building_context& context) {
	if(context.main_slot != trigger::slot_contents::nation) {
		err.accumulated_errors +=
				"define_general effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		return;
	}
	context.compiled_effect.push_back(effect::define_general);
	context.compiled_effect.push_back(trigger::payload(context.outer_context.state.add_unit_name(value.name)).value);
	context.compiled_effect.push_back(trigger::payload(value.personality_).value);
	context.compiled_effect.push_back(trigger::payload(value.background_).value);
}

void effect_body::define_admiral(ef_define_admiral const& value, error_handler& err, int32_t line,
		effect_building_context& context) {
	if(context.main_slot != trigger::slot_contents::nation) {
		err.accumulated_errors +=
				"define_admiral effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		return;
	}
	context.compiled_effect.push_back(effect::define_admiral);
	context.compiled_effect.push_back(trigger::payload(context.outer_context.state.add_unit_name(value.name)).value);
	context.compiled_effect.push_back(trigger::payload(value.personality_).value);
	context.compiled_effect.push_back(trigger::payload(value.background_).value);
}
void effect_body::enable_canal(association_type t, int32_t value, error_handler& err, int32_t line,
		effect_building_context& context) {
	if(1 <= value && value <= int32_t(context.outer_context.state.province_definitions.canals.size())) {
		context.compiled_effect.push_back(uint16_t(effect::enable_canal));
		context.compiled_effect.push_back(trigger::payload(uint16_t(value)).value);
	} else {
		err.accumulated_errors +=
				"canal index " + std::to_string(value) + " out of range (" + err.file_name + ", line " + std::to_string(line) + ")\n";
	}
}
void effect_body::any_value(std::string_view label, association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
	std::string str_label{ label };
	if(auto it = context.outer_context.map_of_commodity_names.find(str_label);
			it != context.outer_context.map_of_commodity_names.end()) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(effect::variable_good_name);
			context.compiled_effect.push_back(trigger::payload(it->second).value);
			context.add_float_to_payload(parse_float(value, line, err));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(effect::variable_good_name_province);
			context.compiled_effect.push_back(trigger::payload(it->second).value);
			context.add_float_to_payload(parse_float(value, line, err));
		} else {
			err.accumulated_errors += "variable commodity name effect used in an incorrect scope type (" + err.file_name + ", line " +
				std::to_string(line) + ")\n";
			return;
		}
	} else if(auto itb = context.outer_context.map_of_technologies.find(str_label); itb != context.outer_context.map_of_technologies.end()) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(parse_bool(value, line, err))
				context.compiled_effect.push_back(effect::variable_tech_name_yes);
			else
				context.compiled_effect.push_back(effect::variable_tech_name_no);
			context.compiled_effect.push_back(trigger::payload(itb->second.id).value);
			context.add_float_to_payload(parse_float(value, line, err));
		} else {
			err.accumulated_errors += "variable technology name effect used in an incorrect scope type (" + err.file_name +
				", line " + std::to_string(line) + ")\n";
			return;
		}
	} else if(auto itc = context.outer_context.map_of_inventions.find(str_label); itc != context.outer_context.map_of_inventions.end()) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(parse_bool(value, line, err))
				context.compiled_effect.push_back(effect::variable_invention_name_yes);
			else
				context.compiled_effect.push_back(effect::variable_invention_name_no);
			context.compiled_effect.push_back(trigger::payload(itc->second.id).value);
			context.add_float_to_payload(parse_float(value, line, err));
		} else {
			err.accumulated_errors += "variable invention name effect used in an incorrect scope type (" + err.file_name + ", line " +
				std::to_string(line) + ")\n";
			return;
		}
	} else if(auto itf = context.outer_context.map_of_iissues.find(str_label); itf != context.outer_context.map_of_iissues.end()) {
		if(auto itopt = context.outer_context.map_of_ioptions.find(std::string(value)); itopt != context.outer_context.map_of_ioptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				auto cat = context.outer_context.state.world.issue_get_issue_type(itf->second);
				if(cat == uint8_t(::culture::issue_category::political)) {
					context.compiled_effect.push_back(uint16_t(effect::political_reform));
					context.compiled_effect.push_back(trigger::payload(itopt->second.id).value);
				} else if(cat == uint8_t(::culture::issue_category::social)) {
					context.compiled_effect.push_back(uint16_t(effect::social_reform));
					context.compiled_effect.push_back(trigger::payload(itopt->second.id).value);
				} else {
					err.accumulated_errors += "named issue effect used with a party issue (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "named issue effect used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "named issue effect used with an invalid option name (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	} else if(auto ith = context.outer_context.map_of_reforms.find(str_label); ith != context.outer_context.map_of_reforms.end()) {
		if(auto itopt = context.outer_context.map_of_roptions.find(std::string(value));
				itopt != context.outer_context.map_of_roptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				auto cat = context.outer_context.state.world.reform_get_reform_type(ith->second);
				if(cat == uint8_t(::culture::issue_category::military)) {
					context.compiled_effect.push_back(uint16_t(effect::military_reform));
					context.compiled_effect.push_back(trigger::payload(itopt->second.id).value);
				} else if(cat == uint8_t(::culture::issue_category::economic)) {
					context.compiled_effect.push_back(uint16_t(effect::economic_reform));
					context.compiled_effect.push_back(trigger::payload(itopt->second.id).value);
				} else {
					err.accumulated_errors += "named reform effect used with an invalid issue type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "named reform effect used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "named reform effect used with an invalid option name (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	} else {
		err.accumulated_errors +=
			"unknown effect " + str_label + " encountered (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		return;
	}
}

} // namespace parsers
