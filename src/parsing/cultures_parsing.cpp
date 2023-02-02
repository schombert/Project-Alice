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

void make_issues_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {

	::culture::issue_category type = ::culture::issue_category::party;

	if(is_fixed_token_ci(name.data(), name.data() + name.length(), "party_issues")) {
		type = ::culture::issue_category::party;
	} else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "political_reforms")) {
		type = ::culture::issue_category::political;
	} else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "social_reforms")) {
		type = ::culture::issue_category::social;
	} else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "economic_reforms")) {
		type = ::culture::issue_category::economic;
	} else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "military_reforms")) {
		type = ::culture::issue_category::military;
	} else {
		err.accumulated_errors += "Unknown issue group " + std::string(name) + " in file " + err.file_name + "\n";
	}

	issue_group_context new_context(context, type);
	parse_issues_group(gen, err, new_context);
}

void make_issue(std::string_view name, token_generator& gen, error_handler& err, issue_group_context& context) {
	dcon::issue_id new_id = context.outer_context.state.world.create_issue();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);

	context.outer_context.state.world.issue_set_name(new_id, name_id);
	context.outer_context.map_of_issues.insert_or_assign(std::string(name), new_id);

	switch(context.issue_cat) {
		case ::culture::issue_category::party:
			context.outer_context.state.culture_definitions.party_issues.push_back(new_id);
			break;
		case ::culture::issue_category::economic:
			context.outer_context.state.culture_definitions.economic_issues.push_back(new_id);
			break;
		case ::culture::issue_category::social:
			context.outer_context.state.culture_definitions.social_issues.push_back(new_id);
			break;
		case ::culture::issue_category::political:
			context.outer_context.state.culture_definitions.political_issues.push_back(new_id);
			break;
		case ::culture::issue_category::military:
			context.outer_context.state.culture_definitions.military_issues.push_back(new_id);
			break;
	}

	issue_context new_context{ context.outer_context, new_id };
	parse_issue(gen, err, new_context);
}
void register_option(std::string_view name, token_generator& gen, error_handler& err, issue_context& context) {
	dcon::issue_option_id new_id = context.outer_context.state.world.create_issue_option();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);

	context.outer_context.state.world.issue_option_set_name(new_id, name_id);
	context.outer_context.map_of_options.insert_or_assign(std::string(name), pending_option_content{ gen, new_id });

	bool assigned = false;
	auto& existing_options = context.outer_context.state.world.issue_get_options(context.id);
	for(uint32_t i = 0; i < existing_options.size(); ++i) {
		if(!(existing_options[i])) {
			existing_options[i] = new_id;
			assigned = true;
			break;
		}
	}
	if(!assigned) {
		err.accumulated_errors += "Option " + std::string(name) + " in file " + err.file_name + " was the 7th or later option\n";
	}

	gen.discard_group();
}


void make_government(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::government_type_id new_id = dcon::government_type_id(dcon::government_type_id::value_base_t(context.state.culture_definitions.governments.size()));
	context.state.culture_definitions.governments.emplace_back();

	auto name_id = text::find_or_add_key(context.state, name);

	context.state.culture_definitions.governments[new_id].name = name_id;
	context.map_of_governments.insert_or_assign(std::string(name), new_id);

	government_type_context new_context{ context , new_id };
	parse_government_type(gen, err, new_context);
}

void register_crime(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::crime_id new_id = dcon::crime_id(dcon::crime_id::value_base_t(context.state.culture_definitions.crimes.size()));
	context.state.culture_definitions.crimes.emplace_back();

	auto name_id = text::find_or_add_key(context.state, name);
	context.state.culture_definitions.crimes[new_id].name = name_id;

	context.map_of_crimes.insert_or_assign(std::string(name), pending_crime_content{ gen, new_id });

	gen.discard_group();
}

}
