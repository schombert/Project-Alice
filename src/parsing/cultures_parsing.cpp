#include "parsers_declarations.hpp"
#include "culture.hpp"
#include "trigger_parsing.hpp"

namespace parsers {

void make_religion(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto new_id = context.state.world.create_religion();
	auto name_id = text::find_or_add_key(context.state, name);
	context.state.world.religion_set_name(new_id, name_id);

	context.map_of_religion_names.insert_or_assign(std::string(name), new_id);

	religion_context new_context{new_id, context};
	parse_religion_def(gen, err, new_context);
}

void make_culture_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::culture_group_id new_id = context.state.world.create_culture_group();
	auto name_id = text::find_or_add_key(context.state, name);
	context.state.world.culture_group_set_name(new_id, name_id);
	context.state.world.culture_group_set_is_overseas(new_id, true);

	context.map_of_culture_group_names.insert_or_assign(std::string(name), new_id);
	culture_group_context new_context{new_id, context};
	parse_culture_group(gen, err, new_context);
}

void make_culture(std::string_view name, token_generator& gen, error_handler& err, culture_group_context& context) {
	dcon::culture_id new_id = context.outer_context.state.world.create_culture();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);
	context.outer_context.state.world.culture_set_name(new_id, name_id);
	context.outer_context.state.world.force_create_culture_group_membership(new_id, context.id);

	context.outer_context.map_of_culture_names.insert_or_assign(std::string(name), new_id);
	culture_context new_context{new_id, context.outer_context};
	parse_culture(gen, err, new_context);
}

void make_fn_list(token_generator& gen, error_handler& err, culture_context& context) {
	names_context new_context{context.id, true, context.outer_context};
	parse_names_list(gen, err, new_context);
}
void make_ln_list(token_generator& gen, error_handler& err, culture_context& context) {
	names_context new_context{context.id, false, context.outer_context};
	parse_names_list(gen, err, new_context);
}

void register_ideology(std::string_view name, token_generator& gen, error_handler& err, ideology_group_context& context) {

	dcon::ideology_id new_id = context.outer_context.state.world.create_ideology();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);

	context.outer_context.state.world.ideology_set_name(new_id, name_id);
	context.outer_context.map_of_ideologies.insert_or_assign(std::string(name), pending_ideology_content{gen, new_id});

	context.outer_context.state.world.force_create_ideology_group_membership(new_id, context.id);

	gen.discard_group();
}

void make_ideology_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::ideology_group_id new_id = context.state.world.create_ideology_group();
	auto name_id = text::find_or_add_key(context.state, name);

	context.state.world.ideology_group_set_name(new_id, name_id);
	context.map_of_ideology_groups.insert_or_assign(std::string(name), new_id);

	ideology_group_context new_context{context, new_id};
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

	switch(context.issue_cat) {
	case ::culture::issue_category::party: {
		dcon::issue_id new_id = context.outer_context.state.world.create_issue();
		auto name_id = text::find_or_add_key(context.outer_context.state, name);

		context.outer_context.state.world.issue_set_name(new_id, name_id);
		context.outer_context.map_of_iissues.insert_or_assign(std::string(name), new_id);

		context.outer_context.state.culture_definitions.party_issues.push_back(new_id);

		issue_context new_context{context.outer_context, new_id};
		parse_issue(gen, err, new_context);
	} break;
	case ::culture::issue_category::economic: {
		dcon::reform_id new_id = context.outer_context.state.world.create_reform();
		auto name_id = text::find_or_add_key(context.outer_context.state, name);

		context.outer_context.state.world.reform_set_name(new_id, name_id);
		context.outer_context.map_of_reforms.insert_or_assign(std::string(name), new_id);

		context.outer_context.state.culture_definitions.economic_issues.push_back(new_id);

		reform_context new_context{context.outer_context, new_id};
		parse_issue(gen, err, new_context);
	} break;
	case ::culture::issue_category::social: {
		dcon::issue_id new_id = context.outer_context.state.world.create_issue();
		auto name_id = text::find_or_add_key(context.outer_context.state, name);

		context.outer_context.state.world.issue_set_name(new_id, name_id);
		context.outer_context.map_of_iissues.insert_or_assign(std::string(name), new_id);

		context.outer_context.state.culture_definitions.social_issues.push_back(new_id);
		issue_context new_context{context.outer_context, new_id};
		parse_issue(gen, err, new_context);
	} break;
	case ::culture::issue_category::political: {
		dcon::issue_id new_id = context.outer_context.state.world.create_issue();
		auto name_id = text::find_or_add_key(context.outer_context.state, name);

		context.outer_context.state.world.issue_set_name(new_id, name_id);
		context.outer_context.map_of_iissues.insert_or_assign(std::string(name), new_id);

		context.outer_context.state.culture_definitions.political_issues.push_back(new_id);
		issue_context new_context{context.outer_context, new_id};
		parse_issue(gen, err, new_context);
	} break;
	case ::culture::issue_category::military: {
		dcon::reform_id new_id = context.outer_context.state.world.create_reform();
		auto name_id = text::find_or_add_key(context.outer_context.state, name);

		context.outer_context.state.world.reform_set_name(new_id, name_id);
		context.outer_context.map_of_reforms.insert_or_assign(std::string(name), new_id);

		context.outer_context.state.culture_definitions.military_issues.push_back(new_id);

		reform_context new_context{context.outer_context, new_id};
		parse_issue(gen, err, new_context);
	} break;
	}
}
void register_option(std::string_view name, token_generator& gen, error_handler& err, issue_context& context) {
	dcon::issue_option_id new_id = context.outer_context.state.world.create_issue_option();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);

	auto movement_name_id = text::find_or_add_key(context.outer_context.state, name);

	context.outer_context.state.world.issue_option_set_name(new_id, name_id);
	context.outer_context.map_of_ioptions.insert_or_assign(std::string(name), pending_option_content{gen, new_id});

	auto it = context.outer_context.state.key_to_text_sequence.find(lowercase_str(std::string("movement_") + std::string(name)));
	if(it != context.outer_context.state.key_to_text_sequence.end()) {
		context.outer_context.state.world.issue_option_set_movement_name(new_id, it->second);
	}

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
void register_option(std::string_view name, token_generator& gen, error_handler& err, reform_context& context) {
	dcon::reform_option_id new_id = context.outer_context.state.world.create_reform_option();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);

	context.outer_context.state.world.reform_option_set_name(new_id, name_id);
	context.outer_context.map_of_roptions.insert_or_assign(std::string(name), pending_roption_content{gen, new_id});

	bool assigned = false;
	auto& existing_options = context.outer_context.state.world.reform_get_options(context.id);
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
	dcon::government_type_id new_id =
			dcon::government_type_id(dcon::government_type_id::value_base_t(context.state.culture_definitions.governments.size()));
	context.state.culture_definitions.governments.emplace_back();

	auto name_id = text::find_or_add_key(context.state, name);

	context.state.culture_definitions.governments[new_id].name = name_id;
	context.state.culture_definitions.governments[new_id].ruler_name =
			text::find_or_add_key(context.state, std::string(name) + "_ruler");
	context.map_of_governments.insert_or_assign(std::string(name), new_id);

	government_type_context new_context{context, new_id};
	parse_government_type(gen, err, new_context);
}

void register_crime(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::crime_id new_id = dcon::crime_id(dcon::crime_id::value_base_t(context.state.culture_definitions.crimes.size()));
	context.state.culture_definitions.crimes.emplace_back();

	auto name_id = text::find_or_add_key(context.state, name);
	context.state.culture_definitions.crimes[new_id].name = name_id;

	context.map_of_crimes.insert_or_assign(std::string(name), pending_crime_content{gen, new_id});

	gen.discard_group();
}

void register_rebel_type(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::rebel_type_id new_id = context.state.world.create_rebel_type();

	auto name_id = text::find_or_add_key(context.state, std::string(name) + "_name");
	auto desc_id = text::find_or_add_key(context.state, std::string(name) + "_desc");
	auto army_id = text::find_or_add_key(context.state, std::string(name) + "_army");

	context.state.world.rebel_type_set_name(new_id, name_id);
	context.state.world.rebel_type_set_description(new_id, desc_id);
	context.state.world.rebel_type_set_army_name(new_id, army_id);

	context.map_of_rebeltypes.insert_or_assign(std::string(name), pending_rebel_type_content{gen, new_id});

	gen.discard_group();
}

void make_tech_folder_list(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {

	::culture::tech_category cat = ::culture::tech_category::army;
	if(is_fixed_token_ci(name.data(), name.data() + name.length(), "army_tech")) {
		cat = ::culture::tech_category::army;
	} else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "navy_tech")) {
		cat = ::culture::tech_category::navy;
	} else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "commerce_tech")) {
		cat = ::culture::tech_category::commerce;
	} else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "culture_tech")) {
		cat = ::culture::tech_category::culture;
	} else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "industry_tech")) {
		cat = ::culture::tech_category::industry;
	} else {
		err.accumulated_errors += "Unknown technology category " + std::string(name) + " in file " + err.file_name + "\n";
	}

	tech_group_context new_context{context, cat};
	parse_tech_folder_list(gen, err, new_context);

	// read sub file
	auto root = get_root(context.state.common_fs);
	auto tech_folder = open_directory(root, NATIVE("technologies"));
	auto tech_file = open_file(tech_folder, simple_fs::win1250_to_native(name) + NATIVE(".txt"));
	if(tech_file) {
		auto content = view_contents(*tech_file);
		err.file_name = std::string(name) + ".txt";
		token_generator file_gen(content.data, content.data + content.file_size);
		parse_technology_sub_file(file_gen, err, context);

		context.tech_and_invention_files.emplace_back(std::move(*tech_file));
	}
}
void read_school_modifier(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto name_id = text::find_or_add_key(context.state, name);
	auto new_modifier = context.state.world.create_modifier();

	context.map_of_modifiers.insert_or_assign(std::string(name), new_modifier);
	context.state.world.modifier_set_name(new_modifier, name_id);

	auto school = parse_modifier_base(gen, err, context);

	context.state.world.modifier_set_icon(new_modifier, uint8_t(school.icon_index));
	context.state.world.modifier_set_national_values(new_modifier, school.force_national_mod());
}

void register_technology(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto new_id = context.state.world.create_technology();

	auto name_id = text::find_or_add_key(context.state, name);
	context.state.world.technology_set_name(new_id, name_id);

	context.map_of_technologies.insert_or_assign(std::string(name), pending_tech_content{gen, new_id});

	auto root = get_root(context.state.common_fs);
	auto gfx = open_directory(root, NATIVE("gfx"));
	auto pictures = open_directory(gfx, NATIVE("pictures"));
	auto tech = open_directory(pictures, NATIVE("tech"));

	std::string file_name = simple_fs::remove_double_backslashes(std::string("gfx\\pictures\\tech\\") + [&]() {
		if(peek_file(tech, simple_fs::utf8_to_native(name) + NATIVE(".tga"))) {
			return std::string(name) + ".tga";
		} else {
			return std::string("noimage.tga");
		}
	}());

	if(auto it = context.gfx_context.map_of_names.find(file_name); it != context.gfx_context.map_of_names.end()) {
		context.state.world.technology_set_image(new_id, it->second);
	} else {
		auto gfxindex = context.state.ui_defs.gfx.size();
		context.state.ui_defs.gfx.emplace_back();
		ui::gfx_object& new_obj = context.state.ui_defs.gfx.back();
		auto new_gfx_id = dcon::gfx_object_id(uint16_t(gfxindex));

		context.gfx_context.map_of_names.insert_or_assign(file_name, new_gfx_id);

		new_obj.number_of_frames = uint8_t(1);

		if(auto itb = context.gfx_context.map_of_texture_names.find(file_name);
				itb != context.gfx_context.map_of_texture_names.end()) {
			new_obj.primary_texture_handle = itb->second;
		} else {
			auto index = context.state.ui_defs.textures.size();
			context.state.ui_defs.textures.emplace_back(context.state.add_to_pool(file_name));
			new_obj.primary_texture_handle = dcon::texture_id(uint16_t(index));
			context.gfx_context.map_of_texture_names.insert_or_assign(file_name, dcon::texture_id(uint16_t(index)));
		}
		new_obj.flags |= uint8_t(ui::object_type::generic_sprite);

		context.state.world.technology_set_image(new_id, new_gfx_id);
	}

	gen.discard_group();
}

void register_invention(std::string_view name, token_generator& gen, error_handler& err, tech_group_context& context) {
	auto new_id = context.outer_context.state.world.create_invention();

	auto name_id = text::find_or_add_key(context.outer_context.state, name);
	context.outer_context.state.world.invention_set_name(new_id, name_id);
	context.outer_context.state.world.invention_set_technology_type(new_id, uint8_t(context.category));

	context.outer_context.map_of_inventions.insert_or_assign(std::string(name), pending_invention_content{gen, new_id});
	gen.discard_group();
}

void read_promotion_target(std::string_view name, token_generator& gen, error_handler& err, poptype_context& context) {
	if(auto it = context.outer_context.map_of_poptypes.find(std::string(name)); it != context.outer_context.map_of_poptypes.end()) {
		trigger_building_context t_context{context.outer_context, trigger::slot_contents::pop, trigger::slot_contents::nation,
				trigger::slot_contents::empty};
		auto result = make_value_modifier(gen, err, t_context);
		context.outer_context.state.world.pop_type_set_promotion(context.id, it->second, result);
	} else {
		err.accumulated_errors += "Unknown pop type " + std::string(name) + " in file " + err.file_name + "\n";
	}
}
void read_pop_ideology(std::string_view name, token_generator& gen, error_handler& err, poptype_context& context) {
	if(auto it = context.outer_context.map_of_ideologies.find(std::string(name));
			it != context.outer_context.map_of_ideologies.end()) {
		trigger_building_context t_context{context.outer_context, trigger::slot_contents::pop, trigger::slot_contents::nation,
				trigger::slot_contents::empty};
		auto result = make_value_modifier(gen, err, t_context);
		context.outer_context.state.world.pop_type_set_ideology(context.id, it->second.id, result);
	} else {
		err.accumulated_errors += "Unknown ideology " + std::string(name) + " in file " + err.file_name + "\n";
	}
}
void read_pop_issue(std::string_view name, token_generator& gen, error_handler& err, poptype_context& context) {
	if(auto it = context.outer_context.map_of_ioptions.find(std::string(name)); it != context.outer_context.map_of_ioptions.end()) {
		trigger_building_context t_context{context.outer_context, trigger::slot_contents::pop, trigger::slot_contents::nation,
				trigger::slot_contents::empty};
		auto result = make_value_modifier(gen, err, t_context);
		context.outer_context.state.world.pop_type_set_issues(context.id, it->second.id, result);
	} else {
		err.accumulated_errors += "Unknown issue option " + std::string(name) + " in file " + err.file_name + "\n";
	}
}
void read_c_migration_target(token_generator& gen, error_handler& err, poptype_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::pop,
			trigger::slot_contents::empty};
	auto result = make_value_modifier(gen, err, t_context);
	context.outer_context.state.world.pop_type_set_country_migration_target(context.id, result);
}
void read_migration_target(token_generator& gen, error_handler& err, poptype_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::province, trigger::slot_contents::pop,
			trigger::slot_contents::empty};
	auto result = make_value_modifier(gen, err, t_context);
	context.outer_context.state.world.pop_type_set_migration_target(context.id, result);
}

commodity_array stub_commodity_array(token_generator& gen, error_handler& err, poptype_context& context) {
	return parse_commodity_array(gen, err, context.outer_context);
}

dcon::value_modifier_key ideology_condition(token_generator& gen, error_handler& err, individual_ideology_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_value_modifier(gen, err, t_context);
}

dcon::trigger_key make_crime_trigger(token_generator& gen, error_handler& err, scenario_building_context& context) {
	trigger_building_context t_context{context, trigger::slot_contents::province, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_trigger(gen, err, t_context);
}

void read_pending_crime(dcon::crime_id id, token_generator& gen, error_handler& err, scenario_building_context& context) {
	crime_modifier crime_body = parse_crime_modifier(gen, err, context);
	context.state.culture_definitions.crimes[id].available_by_default = crime_body.active;
	context.state.culture_definitions.crimes[id].trigger = crime_body.trigger;

	if(crime_body.next_to_add_p != 0) {
		auto new_modifier = context.state.world.create_modifier();
		context.state.world.modifier_set_name(new_modifier, context.state.culture_definitions.crimes[id].name);
		context.state.world.modifier_set_icon(new_modifier, uint8_t(crime_body.icon_index));

		context.state.world.modifier_set_province_values(new_modifier, crime_body.peek_province_mod());
		context.state.world.modifier_set_national_values(new_modifier, crime_body.peek_national_mod());
		context.state.culture_definitions.crimes[id].modifier = new_modifier;
	}
}

void make_opt_allow(token_generator& gen, error_handler& err, individual_option_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	context.outer_context.state.world.issue_option_set_allow(context.id, make_trigger(gen, err, t_context));
}
dcon::trigger_key make_execute_trigger(token_generator& gen, error_handler& err, individual_option_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_trigger(gen, err, t_context);
}
dcon::effect_key make_execute_effect(token_generator& gen, error_handler& err, individual_option_context& context) {
	effect_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_effect(gen, err, t_context);
}

void make_opt_allow(token_generator& gen, error_handler& err, individual_roption_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	context.outer_context.state.world.reform_option_set_allow(context.id, make_trigger(gen, err, t_context));
}
dcon::trigger_key make_execute_trigger(token_generator& gen, error_handler& err, individual_roption_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_trigger(gen, err, t_context);
}
dcon::effect_key make_execute_effect(token_generator& gen, error_handler& err, individual_roption_context& context) {
	effect_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_effect(gen, err, t_context);
}

void read_pending_option(dcon::issue_option_id id, token_generator& gen, error_handler& err, scenario_building_context& context) {
	individual_option_context new_context{context, id};
	issue_option_body opt = parse_issue_option_body(gen, err, new_context);

	if(opt.next_to_add_n != 0 || opt.next_to_add_p != 0) {
		auto new_modifier = context.state.world.create_modifier();
		context.state.world.modifier_set_name(new_modifier, context.state.world.issue_option_get_name(id));
		context.state.world.modifier_set_icon(new_modifier, uint8_t(opt.icon_index));
		context.state.world.modifier_set_national_values(new_modifier, opt.force_national_mod());
		context.state.world.issue_option_set_modifier(id, new_modifier);
	}
}
void read_pending_reform(dcon::reform_option_id id, token_generator& gen, error_handler& err,
		scenario_building_context& context) {
	individual_roption_context new_context{context, id};
	issue_option_body opt = parse_issue_option_body(gen, err, new_context);

	if(opt.next_to_add_n != 0 || opt.next_to_add_p != 0) {
		auto new_modifier = context.state.world.create_modifier();
		context.state.world.modifier_set_name(new_modifier, context.state.world.reform_option_get_name(id));
		context.state.world.modifier_set_icon(new_modifier, uint8_t(opt.icon_index));
		context.state.world.modifier_set_national_values(new_modifier, opt.force_national_mod());
		context.state.world.reform_option_set_modifier(id, new_modifier);
	}
}

dcon::value_modifier_key make_poptype_pop_chance(token_generator& gen, error_handler& err, scenario_building_context& context) {
	trigger_building_context t_context{context, trigger::slot_contents::pop, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_value_modifier(gen, err, t_context);
}

dcon::value_modifier_key make_ai_chance(token_generator& gen, error_handler& err, tech_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_value_modifier(gen, err, t_context);
}
void read_pending_technology(dcon::technology_id id, token_generator& gen, error_handler& err,
		scenario_building_context& context) {
	tech_context new_context{context, id};
	auto modifier = parse_technology_contents(gen, err, new_context);

	if(modifier.next_to_add_n != 0 || modifier.next_to_add_p != 0) {
		auto new_modifier = context.state.world.create_modifier();
		context.state.world.modifier_set_name(new_modifier, context.state.world.technology_get_name(id));
		context.state.world.modifier_set_icon(new_modifier, uint8_t(modifier.icon_index));
		context.state.world.modifier_set_national_values(new_modifier, modifier.force_national_mod());
		context.state.world.technology_set_modifier(id, new_modifier);
	}
}

dcon::value_modifier_key make_inv_chance(token_generator& gen, error_handler& err, invention_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_value_modifier(gen, err, t_context);
}
dcon::trigger_key make_inv_limit(token_generator& gen, error_handler& err, invention_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::empty};
	return make_trigger(gen, err, t_context);
}

void read_pending_invention(dcon::invention_id id, token_generator& gen, error_handler& err, scenario_building_context& context) {
	invention_context new_context{context, id};
	auto modifier = parse_invention_contents(gen, err, new_context);

	if(modifier.next_to_add_n != 0 || modifier.next_to_add_p != 0) {
		auto new_modifier = context.state.world.create_modifier();
		context.state.world.modifier_set_name(new_modifier, context.state.world.invention_get_name(id));
		context.state.world.modifier_set_icon(new_modifier, uint8_t(modifier.icon_index));
		context.state.world.modifier_set_national_values(new_modifier, modifier.force_national_mod());
		context.state.world.invention_set_modifier(id, new_modifier);
	}
}

dcon::value_modifier_key make_reb_will_rise(token_generator& gen, error_handler& err, rebel_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::rebel};
	return make_value_modifier(gen, err, t_context);
}
dcon::value_modifier_key make_reb_spawn_chance(token_generator& gen, error_handler& err, rebel_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::pop, trigger::slot_contents::nation,
			trigger::slot_contents::rebel};
	return make_value_modifier(gen, err, t_context);
}
dcon::value_modifier_key make_reb_movement_eval(token_generator& gen, error_handler& err, rebel_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::province, trigger::slot_contents::nation,
			trigger::slot_contents::rebel};
	return make_value_modifier(gen, err, t_context);
}
dcon::trigger_key make_reb_s_won_trigger(token_generator& gen, error_handler& err, rebel_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::province, trigger::slot_contents::nation,
			trigger::slot_contents::rebel};
	return make_trigger(gen, err, t_context);
}
dcon::trigger_key make_reb_enforced_trigger(token_generator& gen, error_handler& err, rebel_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::rebel};
	return make_trigger(gen, err, t_context);
}
dcon::effect_key make_reb_s_won_effect(token_generator& gen, error_handler& err, rebel_context& context) {
	effect_building_context e_context{context.outer_context, trigger::slot_contents::province, trigger::slot_contents::nation,
			trigger::slot_contents::rebel};
	return make_effect(gen, err, e_context);
}
dcon::effect_key make_reb_enforce_effect(token_generator& gen, error_handler& err, rebel_context& context) {
	effect_building_context e_context{context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::nation,
			trigger::slot_contents::rebel};
	return make_effect(gen, err, e_context);
}
void read_pending_rebel_type(dcon::rebel_type_id id, token_generator& gen, error_handler& err,
		scenario_building_context& context) {
	rebel_context new_context{context, id};
	parse_rebel_body(gen, err, new_context);
}

} // namespace parsers
