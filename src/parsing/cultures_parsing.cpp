#include "parsers_declarations.hpp"

namespace parsers {

void make_religion(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto new_id = context.state.world.create_religion();
	auto name_id = text::find_or_add_key(context.state, name);
	context.state.world.religion_set_name(new_id, name_id);

	context.map_of_religion_names.insert_or_assign(std::string(name), new_id);

	religion_context new_context{ new_id, context };
	parse_religion_def(gen, err, new_context);
}


void make_culture_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::culture_group_id new_id = context.state.world.create_culture_group();
	auto name_id = text::find_or_add_key(context.state, name);
	context.state.world.culture_group_set_name(new_id, name_id);
	context.state.world.culture_group_set_is_overseas(new_id, true);

	context.map_of_culture_group_names.insert_or_assign(std::string(name), new_id);
	culture_group_context new_context{ new_id, context };
	parse_culture_group(gen, err, new_context);
}

void make_culture(std::string_view name, token_generator& gen, error_handler& err, culture_group_context& context) {
	dcon::culture_id new_id = context.outer_context.state.world.create_culture();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);
	context.outer_context.state.world.culture_set_name(new_id, name_id);
	context.outer_context.state.world.force_create_culture_group_membership(new_id, context.id);

	context.outer_context.map_of_culture_names.insert_or_assign(std::string(name), new_id);
	culture_context new_context{ new_id, context.outer_context };
	parse_culture(gen, err, new_context);
}

void make_fn_list(token_generator& gen, error_handler& err, culture_context& context) {
	names_context new_context{ context.id, true, context.outer_context };
	parse_names_list(gen, err, new_context);
}
void make_ln_list(token_generator& gen, error_handler& err, culture_context& context) {
	names_context new_context{ context.id, false, context.outer_context };
	parse_names_list(gen, err, new_context);
}

void register_ideology(std::string_view name, token_generator& gen, error_handler& err, ideology_group_context& context) {

	dcon::ideology_id new_id = context.outer_context.state.world.create_ideology();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);

	context.outer_context.state.world.ideology_set_name(new_id, name_id);
	context.outer_context.map_of_ideologies.insert_or_assign(std::string(name), pending_ideology_content{ gen , new_id });


	context.outer_context.state.world.force_create_ideology_group_membership(new_id, context.id);

	gen.discard_group();
}

void make_ideology_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::ideology_group_id new_id = context.state.world.create_ideology_group();
	auto name_id = text::find_or_add_key(context.state, name);

	context.state.world.ideology_group_set_name(new_id, name_id);
	context.map_of_ideology_groups.insert_or_assign(std::string(name), new_id);

	ideology_group_context new_context{ context , new_id };
	parse_ideology_group(gen, err, new_context);
}

}
