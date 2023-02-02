#include "system_state.hpp"
#include "nations.hpp"
#include "parsers_declarations.hpp"

namespace parsers {
void national_identity_file::any_value(std::string_view tag, association_type, std::string_view txt, error_handler& err, int32_t line, scenario_building_context& context) {
	if(tag.length() != 3) {
		err.accumulated_errors += err.file_name + " line " + std::to_string(line) + ": encountered a tag that was not three characters\n";
		return;
	}
	auto as_int = nations::tag_to_int(tag[0], tag[1], tag[2]);
	auto new_ident = context.state.world.create_national_identity();
	context.file_names_for_idents.resize(context.state.world.national_identity_size());
	context.file_names_for_idents[new_ident] = txt;
	context.map_of_ident_names.insert_or_assign(as_int, new_ident);

}

void triggered_modifier::finish(triggered_modifier_context& context) {
	auto name_id = text::find_or_add_key(context.outer_context.state, context.name);

	auto modifier_id = context.outer_context.state.world.create_modifier();

	context.outer_context.state.world.modifier_set_icon(modifier_id, uint8_t(icon_index));
	context.outer_context.state.world.modifier_set_name(modifier_id, name_id);
	this->convert_to_national_mod();
	context.outer_context.state.world.modifier_set_province_values(modifier_id, constructed_definition);

	context.outer_context.map_of_modifiers.insert_or_assign(std::string(context.name), modifier_id);

	context.outer_context.state.national_definitions.triggered_modifiers[context.index].linked_modifier = modifier_id;
}

void register_trigger(token_generator& gen, error_handler& err, triggered_modifier_context& context) {
	context.outer_context.set_of_triggered_modifiers.push_back(pending_triggered_modifier_content{gen, context.index});
}

void make_triggered_modifier(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto index = uint32_t(context.state.national_definitions.triggered_modifiers.size());
	context.state.national_definitions.triggered_modifiers.emplace_back();

	triggered_modifier_context new_context{context, index, name };
	parse_triggered_modifier(gen, err, new_context);
}

void make_national_value(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto name_id = text::find_or_add_key(context.state, name);

	auto parsed_modifier = parse_modifier_base(gen, err, context);
	parsed_modifier.convert_to_national_mod();

	auto new_modifier = context.state.world.create_modifier();

	context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
	context.state.world.modifier_set_name(new_modifier, name_id);
	context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.constructed_definition);

	context.map_of_modifiers.insert_or_assign(std::string(name), new_modifier);
}

}
