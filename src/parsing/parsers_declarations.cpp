#include "parsers_declarations.hpp"
#include "system_state.hpp"

namespace parsers {



void religion_def::icon(association_type, int32_t v, error_handler& err, int32_t line, religion_context& context) {
	context.outer_context.state.world.religion_set_icon(context.id, uint8_t(v));
}

void religion_def::color(color_from_3f v, error_handler& err, int32_t line, religion_context& context) {
	context.outer_context.state.world.religion_set_color(context.id, v.value);
}

void religion_def::pagan(association_type, bool v, error_handler& err, int32_t line, religion_context& context) {
	context.outer_context.state.world.religion_set_is_pagan(context.id, v);
}

void names_list::free_value(std::string_view text, error_handler& err, int32_t line, names_context& context) {
	auto new_id = context.outer_context.state.add_to_pool(text);
	if(context.first_names) {
		context.outer_context.state.world.culture_get_first_names(context.id).push_back(new_id);
	} else {
		context.outer_context.state.world.culture_get_last_names(context.id).push_back(new_id);
	}
}

void culture::color(color_from_3i v, error_handler& err, int32_t line, culture_context& context) {
	context.outer_context.state.world.culture_set_color(context.id, v.value);
}

void culture::radicalism(association_type, int32_t v, error_handler& err, int32_t line, culture_context& context) {
	context.outer_context.state.world.culture_set_radicalism(context.id, int8_t(v));
}

void culture_group::leader(association_type, std::string_view name, error_handler& err, int32_t line, culture_group_context& context) {
	if(name == "european")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::european));
	else if(name == "southamerican")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::southamerican));
	else if(name == "russian")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::russian));
	else if(name == "arab")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::arab));
	else if(name == "asian")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::asian));
	else if(name == "indian")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::indian));
	else if(name == "nativeamerican")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::nativeamerican));
	else if(name == "african")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::african));
	else if(name == "polar_bear")
		context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::polar_bear));
	else {
		err.accumulated_errors += "Unknown leader type " + std::string(name) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
	}
}

void culture_group::is_overseas(association_type, bool v, error_handler& err, int32_t line, culture_group_context& context) {
	context.outer_context.state.world.culture_group_set_is_overseas(context.id, v);
}

void culture_group::union_tag(association_type, uint32_t v, error_handler& err, int32_t line, culture_group_context& context) {
	auto nat_tag = context.outer_context.map_of_ident_names.find(v);
	if(nat_tag != context.outer_context.map_of_ident_names.end())
		context.outer_context.state.world.force_create_cultural_union_of(nat_tag->second, context.id);
	else
		err.accumulated_errors += "Unknown national tag in file " + err.file_name + " line " + std::to_string(line) + "\n";
}

void good::money(association_type, bool v, error_handler& err, int32_t line, good_context& context) {
	if(v) {
		auto money_id = dcon::commodity_id(0);
		context.outer_context.state.world.commodity_set_color(money_id, context.outer_context.state.world.commodity_get_color(context.id));
		context.outer_context.state.world.commodity_set_cost(money_id, context.outer_context.state.world.commodity_get_cost(context.id));

		for(auto& pr : context.outer_context.map_of_commodity_names) {
			if(pr.second == context.id) {
				pr.second = money_id;
				break;
			}
		}
		context.id = money_id;
		context.outer_context.state.world.pop_back_commodity();
	}
}

void good::color(color_from_3i v, error_handler& err, int32_t line, good_context& context) {
	context.outer_context.state.world.commodity_set_color(context.id, v.value);
}

void good::cost(association_type, float v, error_handler& err, int32_t line, good_context& context) {
	context.outer_context.state.world.commodity_set_cost(context.id, v);
}

void good::available_from_start(association_type, bool b, error_handler& err, int32_t line, good_context& context) {
	context.outer_context.state.world.commodity_set_is_available_from_start(context.id, b);
}

void issue::next_step_only(association_type, bool value, error_handler& err, int32_t line, issue_context& context) {
	context.outer_context.state.world.issue_set_is_next_step_only(context.id, value);
}

void issue::administrative(association_type, bool value, error_handler& err, int32_t line, issue_context& context) {
	context.outer_context.state.world.issue_set_is_administrative(context.id, value);
}

void government_type::election(association_type, bool value, error_handler& err, int32_t line, government_type_context& context) {
	context.outer_context.state.culture_definitions.governments[context.id].has_elections = value;
}

void government_type::appoint_ruling_party(association_type, bool value, error_handler& err, int32_t line, government_type_context& context) {
	context.outer_context.state.culture_definitions.governments[context.id].can_appoint_ruling_party = value;
}

void government_type::duration(association_type, int32_t value, error_handler& err, int32_t line, government_type_context& context) {
	context.outer_context.state.culture_definitions.governments[context.id].duration = int8_t(value);
}

void government_type::flagtype(association_type, std::string_view value, error_handler& err, int32_t line, government_type_context& context) {
	if(is_fixed_token_ci(value.data(), value.data() + value.length(), "communist"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::communist;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "fascist"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::fascist;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "monarchy"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::monarchy;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "republic"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::republic;
	// Non-vanilla flags
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "theocracy"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::theocracy;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "special"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::special;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "spare"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::spare;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "populist"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::populist;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "realm"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::realm;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "other"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::other;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "monarchy2"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::monarchy2;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "republic2"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::republic2;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "cosmetic_1"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::cosmetic_1;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "cosmetic_2"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::cosmetic_2;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "colonial"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::colonial;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "nationalist"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::nationalist;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "sectarian"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::sectarian;
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "socialist"))
		context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::socialist;
	else {
		err.accumulated_errors += "Unknown flag type " + std::string(value) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
	}
}

void government_type::any_value(std::string_view text, association_type, bool value, error_handler& err, int32_t line, government_type_context& context) {
	if(value) {
		auto found_ideology = context.outer_context.map_of_ideologies.find(std::string(text));
		if(found_ideology != context.outer_context.map_of_ideologies.end()) {
			context.outer_context.state.culture_definitions.governments[context.id].ideologies_allowed |= ::culture::to_bits(found_ideology->second.id);
		} else {
			err.accumulated_errors += "Unknown ideology " + std::string(text) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
		}
	}

}

void cb_list::free_value(std::string_view text, error_handler& err, int32_t line, scenario_building_context& context) {
	dcon::cb_type_id new_id = context.state.world.create_cb_type();
	context.map_of_cb_types.insert_or_assign(std::string(text), pending_cb_content{ token_generator{ }, new_id });
}

void trait::organisation(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
	context.outer_context.state.world.leader_trait_set_organisation(context.id, value);
}

void trait::morale(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
	context.outer_context.state.world.leader_trait_set_morale(context.id, value);
}

void trait::attack(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
	context.outer_context.state.world.leader_trait_set_attack(context.id, value);
}

void trait::defence(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
	context.outer_context.state.world.leader_trait_set_defence(context.id, value);
}

void trait::reconnaissance(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
	context.outer_context.state.world.leader_trait_set_reconnaissance(context.id, value);
}

void trait::speed(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
	context.outer_context.state.world.leader_trait_set_speed(context.id, value);
}

void trait::experience(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
	context.outer_context.state.world.leader_trait_set_experience(context.id, value);
}

void trait::reliability(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
	context.outer_context.state.world.leader_trait_set_reliability(context.id, value);
}

void sea_list::free_value(int32_t value, error_handler& err, int32_t line, scenario_building_context& context) {
	if(size_t(value - 1) > context.prov_id_to_original_id_map.size()) {
		err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
	} else {
		context.prov_id_to_original_id_map[dcon::province_id(dcon::province_id::value_base_t(value - 1))].is_sea = true;
	}
}

void state_definition::free_value(int32_t value, error_handler& err, int32_t line, state_def_building_context& context) {
	if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
		err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
	} else {
		auto province_id = context.outer_context.original_id_to_prov_id_map[value];
		context.outer_context.state.world.force_create_abstract_state_membership(province_id, context.id);
	}
}

void continent_provinces::free_value(int32_t value, error_handler& err, int32_t line, continent_building_context& context) {
	if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
		err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
	} else {
		auto province_id = context.outer_context.original_id_to_prov_id_map[value];
		context.outer_context.state.world.province_set_continent(province_id, context.id);
	}
}

void climate_definition::free_value(int32_t value, error_handler& err, int32_t line, climate_building_context& context) {
	if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
		err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
	} else {
		auto province_id = context.outer_context.original_id_to_prov_id_map[value];
		context.outer_context.state.world.province_set_climate(province_id, context.id);
	}
}

void tech_folder_list::free_value(std::string_view name, error_handler& err, int32_t line, tech_group_context& context) {
	auto name_id = text::find_or_add_key(context.outer_context.state, name);
	auto cindex = context.outer_context.state.culture_definitions.tech_folders.size();
	context.outer_context.state.culture_definitions.tech_folders.push_back(::culture::folder_info{ name_id , context.category });
	context.outer_context.map_of_tech_folders.insert_or_assign(std::string(name), int32_t(cindex));
}

void commodity_set::any_value(std::string_view name, association_type, float value, error_handler& err, int32_t line, scenario_building_context& context) {
	auto found_commodity = context.map_of_commodity_names.find(std::string(name));
	if(found_commodity != context.map_of_commodity_names.end()) {
		if(num_added < int32_t(economy::commodity_set::set_size)) {
			commodity_amounts[num_added] = value;
			commodity_type[num_added] = found_commodity->second;
			++num_added;
		} else {
			err.accumulated_errors += "Too many items in a commodity set, in file " + err.file_name + " line " + std::to_string(line) + "\n";
		}
	} else {
		err.accumulated_errors += "Unknown commodity " + std::string(name) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
	}
}

void party::ideology(association_type, std::string_view text, error_handler& err, int32_t line, party_context& context) {
	if(auto it = context.outer_context.map_of_ideologies.find(std::string(text)); it != context.outer_context.map_of_ideologies.end()) {
		context.outer_context.state.world.political_party_set_ideology(context.id, it->second.id);
	} else {
		err.accumulated_errors += std::string(text) + " is not a valid ideology (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void party::name(association_type, std::string_view text, error_handler& err, int32_t line, party_context& context) {
	auto name_id = text::find_or_add_key(context.outer_context.state, text);
	context.outer_context.state.world.political_party_set_name(context.id, name_id);
}

void party::start_date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, party_context& context) {
	auto date_tag = sys::date(ymd, context.outer_context.state.start_date);
	context.outer_context.state.world.political_party_set_start_date(context.id, date_tag);
}

void party::end_date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, party_context& context) {
	auto date_tag = sys::date(ymd, context.outer_context.state.start_date);
	context.outer_context.state.world.political_party_set_end_date(context.id, date_tag);
}

void party::any_value(std::string_view issue, association_type, std::string_view option, error_handler& err, int32_t line, party_context& context) {
	if(auto it = context.outer_context.map_of_issues.find(std::string(issue)); it != context.outer_context.map_of_issues.end()) {
		if(it->second.index() < int32_t(context.outer_context.state.culture_definitions.party_issues.size())) {
			if(auto oit = context.outer_context.map_of_options.find(std::string(option)); oit != context.outer_context.map_of_options.end()) {
				context.outer_context.state.world.political_party_set_party_issues(context.id, it->second, oit->second.id);
			} else {
				err.accumulated_errors += std::string(option) + " is not a valid option name (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors += std::string(issue) + " is not a proper party issue (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
		//context.outer_context.state.world.political_party_set_ideology(context.id, it->second.id);
	} else {
		err.accumulated_errors += std::string(issue) + " is not a valid issue name (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void unit_names_list::free_value(std::string_view text, error_handler& err, int32_t line, unit_names_context& context) {
	if(text.length() <= 2)
		return;

	auto existing_count = context.outer_context.state.world.national_identity_get_unit_names_count(context.id, context.u_id);
	if(existing_count == 0) {
		auto first_id = context.outer_context.state.add_unit_name(text);
		context.outer_context.state.world.national_identity_set_unit_names_first(context.id, context.u_id, first_id);
		context.outer_context.state.world.national_identity_set_unit_names_count(context.id, context.u_id, uint8_t(1));
	} else {
		context.outer_context.state.add_unit_name(text);
		context.outer_context.state.world.national_identity_set_unit_names_count(context.id, context.u_id, uint8_t(existing_count + 1));
	}
}

void pop_history_definition::culture(association_type, std::string_view value, error_handler& err, int32_t line, pop_history_province_context& context) {
	if(auto it = context.outer_context.map_of_culture_names.find(std::string(value)); it != context.outer_context.map_of_culture_names.end()) {
		cul_id = it->second;
	} else {
		err.accumulated_errors += "Invalid culture " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void pop_history_definition::religion(association_type, std::string_view value, error_handler& err, int32_t line, pop_history_province_context& context) {
	if(auto it = context.outer_context.map_of_religion_names.find(std::string(value)); it != context.outer_context.map_of_religion_names.end()) {
		rel_id = it->second;
	} else {
		err.accumulated_errors += "Invalid religion " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void pop_history_definition::rebel_type(association_type, std::string_view value, error_handler& err, int32_t line, pop_history_province_context& context) {
	if(auto it = context.outer_context.map_of_rebeltypes.find(std::string(value)); it != context.outer_context.map_of_rebeltypes.end()) {
		reb_id = it->second.id;
	} else {
		err.accumulated_errors += "Invalid rebel type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void pop_province_list::any_group(std::string_view type, pop_history_definition const& def, error_handler& err, int32_t line, pop_history_province_context& context) {
	dcon::pop_type_id ptype;
	if(auto it = context.outer_context.map_of_poptypes.find(std::string(type)); it != context.outer_context.map_of_poptypes.end()) {
		ptype = it->second;
	} else {
		err.accumulated_errors += "Invalid pop type " + std::string(type) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
	for(auto pops_by_location : context.outer_context.state.world.province_get_pop_location(context.id)) {
		auto pop_id = pops_by_location.get_pop();
		if(pop_id.get_culture() == def.cul_id && pop_id.get_poptype() == ptype && pop_id.get_religion() == def.rel_id) {
			pop_id.get_size() += float(def.size);
			return; // done with this pop
		}
	}
	// no existing pop matched -- make a new pop
	auto new_pop = fatten(context.outer_context.state.world, context.outer_context.state.world.create_pop());
	new_pop.set_culture(def.cul_id);
	new_pop.set_religion(def.rel_id);
	new_pop.set_size(float(def.size));
	new_pop.set_poptype(ptype);
	new_pop.set_militancy(def.militancy);
	new_pop.set_rebel_group(def.reb_id);
	context.outer_context.state.world.force_create_pop_location(new_pop, context.id);
}

void poptype_file::sprite(association_type, int32_t value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_sprite(context.id, uint8_t(value));
}

void poptype_file::color(color_from_3i cvalue, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_color(context.id, cvalue.value);
}

void poptype_file::is_artisan(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
	if(value)
		context.outer_context.state.culture_definitions.artisans = context.id;
}

void poptype_file::strata(association_type, std::string_view value, error_handler& err, int32_t line, poptype_context& context) {
	if(is_fixed_token_ci(value.data(), value.data() + value.length(), "rich"))
		context.outer_context.state.world.pop_type_set_strata(context.id, uint8_t(::culture::pop_strata::rich));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "middle"))
		context.outer_context.state.world.pop_type_set_strata(context.id, uint8_t(::culture::pop_strata::middle));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "poor"))
		context.outer_context.state.world.pop_type_set_strata(context.id, uint8_t(::culture::pop_strata::poor));
	else {
		err.accumulated_errors += "Invalid pop strata " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void poptype_file::unemployment(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_has_unemployment(context.id, value);
}

void poptype_file::is_slave(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
	if(value)
		context.outer_context.state.culture_definitions.slaves = context.id;
}

void poptype_file::can_be_recruited(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
	if(value)
		context.outer_context.state.culture_definitions.soldiers = context.id;
}

void poptype_file::leadership(association_type, int32_t value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.culture_definitions.officer_leadership_points = value;
	context.outer_context.state.culture_definitions.officers = context.id;
}

void poptype_file::research_optimum(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_research_optimum(context.id, value);
}

void poptype_file::administrative_efficiency(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
	if(value)
		context.outer_context.state.culture_definitions.bureaucrat = context.id;
}

void poptype_file::tax_eff(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.culture_definitions.bureaucrat_tax_efficiency = value;
}

void poptype_file::can_build(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
	if(value)
		context.outer_context.state.culture_definitions.capitalists = context.id;
}

void poptype_file::research_points(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_research_points(context.id, value);
}

void poptype_file::can_reduce_consciousness(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
	if(value)
		context.outer_context.state.culture_definitions.clergy = context.id;
}

void poptype_file::workplace_input(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_workplace_input(context.id, value);
}

void poptype_file::workplace_output(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_workplace_output(context.id, value);
}

void poptype_file::equivalent(association_type, std::string_view value, error_handler& err, int32_t line, poptype_context& context) {
	if(value.length() > 0 && value[0] == 'f') {
		context.outer_context.state.culture_definitions.laborers = context.id;
	} else if(value.length() > 0 && value[0] == 'l') {
		context.outer_context.state.culture_definitions.farmers = context.id;
	}
}

void poptype_file::life_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id cid) {
		if(cid.index() < value.data.ssize())
			context.outer_context.state.world.pop_type_set_life_needs(context.id, cid, value.data[cid]);
		});
}

void poptype_file::everyday_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id cid) {
		if(cid.index() < value.data.ssize())
			context.outer_context.state.world.pop_type_set_everyday_needs(context.id, cid, value.data[cid]);
		});
}

void poptype_file::luxury_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id cid) {
		if(cid.index() < value.data.ssize())
			context.outer_context.state.world.pop_type_set_luxury_needs(context.id, cid, value.data[cid]);
		});
}

void poptype_file::life_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_life_needs_income_weight(context.id, value.weight);
	context.outer_context.state.world.pop_type_set_life_needs_income_type(context.id, uint8_t(value.itype));
}

void poptype_file::everyday_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_everyday_needs_income_weight(context.id, value.weight);
	context.outer_context.state.world.pop_type_set_everyday_needs_income_type(context.id, uint8_t(value.itype));
}

void poptype_file::luxury_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context) {
	context.outer_context.state.world.pop_type_set_luxury_needs_income_weight(context.id, value.weight);
	context.outer_context.state.world.pop_type_set_luxury_needs_income_type(context.id, uint8_t(value.itype));
}

void individual_ideology::can_reduce_militancy(association_type, bool value, error_handler& err, int32_t line, individual_ideology_context& context) {
	if(value)
		context.outer_context.state.culture_definitions.conservative = context.id;
}

void individual_ideology::uncivilized(association_type, bool value, error_handler& err, int32_t line, individual_ideology_context& context) {
	context.outer_context.state.world.ideology_set_is_civilized_only(context.id, !value);
}

void individual_ideology::color(color_from_3i cvalue, error_handler& err, int32_t line, individual_ideology_context& context) {
	context.outer_context.state.world.ideology_set_color(context.id, cvalue.value);
}

void individual_ideology::date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, individual_ideology_context& context) {
	auto date_tag = sys::date(ymd, context.outer_context.state.start_date);
	context.outer_context.state.world.ideology_set_activation_date(context.id, date_tag);
}

void individual_ideology::add_political_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
	context.outer_context.state.world.ideology_set_add_political_reform(context.id, value);
}

void individual_ideology::remove_political_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
	context.outer_context.state.world.ideology_set_remove_political_reform(context.id, value);
}

void individual_ideology::add_social_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
	context.outer_context.state.world.ideology_set_add_social_reform(context.id, value);
}

void individual_ideology::remove_social_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
	context.outer_context.state.world.ideology_set_remove_social_reform(context.id, value);
}

void individual_ideology::add_military_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
	context.outer_context.state.world.ideology_set_add_military_reform(context.id, value);
}

void individual_ideology::add_economic_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
	context.outer_context.state.world.ideology_set_add_economic_reform(context.id, value);
}

void cb_body::is_civil_war(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::is_civil_war;
		context.outer_context.state.military_definitions.standard_civil_war = context.id;
	}
}

void cb_body::months(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_months(context.id, uint8_t(value));
}

void cb_body::truce_months(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_truce_months(context.id, uint8_t(value));
}

void cb_body::sprite_index(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_sprite_index(context.id, uint8_t(value));
}

void cb_body::always(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::always;
	}
}

void cb_body::is_triggered_only(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::is_triggered_only;
	}
}

void cb_body::constructing_cb(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(!value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::is_not_constructing_cb;
	}
}

void cb_body::great_war_obligatory(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::great_war_obligatory;
		context.outer_context.state.military_definitions.standard_great_war = context.id;
	}
}

void cb_body::all_allowed_states(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::all_allowed_states;
	}
}

void cb_body::crisis(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(!value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::not_in_crisis;
	}
}

void cb_body::po_clear_union_sphere(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_clear_union_sphere;
	}
}

void cb_body::po_gunboat(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_gunboat;
	}
}

void cb_body::po_annex(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_annex;
	}
}

void cb_body::po_demand_state(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_demand_state;
	}
}

void cb_body::po_add_to_sphere(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_add_to_sphere;
	}
}

void cb_body::po_disarmament(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_disarmament;
	}
}

void cb_body::po_reparations(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_reparations;
	}
}

void cb_body::po_transfer_provinces(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_transfer_provinces;
	}
}

void cb_body::po_remove_prestige(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_remove_prestige;
	}
}

void cb_body::po_make_puppet(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_make_puppet;
	}
}

void cb_body::po_release_puppet(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_release_puppet;
	}
}

void cb_body::po_status_quo(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_status_quo;
	}
}

void cb_body::po_install_communist_gov_type(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_install_communist_gov_type;
	}
}

void cb_body::po_uninstall_communist_gov_type(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_uninstall_communist_gov_type;
	}
}

void cb_body::po_remove_cores(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_remove_cores;
	}
}

void cb_body::po_colony(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_colony;
	}
}

void cb_body::po_destroy_forts(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_destroy_forts;
	}
}

void cb_body::po_destroy_naval_bases(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
	if(value) {
		context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_destroy_naval_bases;
	}
}

void cb_body::war_name(association_type, std::string_view value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_war_name(context.id, text::find_or_add_key(context.outer_context.state, value));
}

void cb_body::badboy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_badboy_factor(context.id, value);
}

void cb_body::prestige_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_prestige_factor(context.id, value);
}

void cb_body::peace_cost_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_peace_cost_factor(context.id, value);
}

void cb_body::penalty_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_penalty_factor(context.id, value);
}

void cb_body::break_truce_prestige_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_break_truce_prestige_factor(context.id, value);
}

void cb_body::break_truce_infamy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_break_truce_infamy_factor(context.id, value);
}

void cb_body::break_truce_militancy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_break_truce_militancy_factor(context.id, value);
}

void cb_body::good_relation_prestige_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_good_relation_prestige_factor(context.id, value);
}

void cb_body::good_relation_infamy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_good_relation_infamy_factor(context.id, value);
}

void cb_body::good_relation_militancy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_good_relation_militancy_factor(context.id, value);
}

void cb_body::construction_speed(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_construction_speed(context.id, value);
}

void cb_body::tws_battle_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_tws_battle_factor(context.id, value);
}

void cb_body::allowed_states(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_allowed_states(context.id, value);
}

void cb_body::allowed_states_in_crisis(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_allowed_states_in_crisis(context.id, value);
}

void cb_body::allowed_substate_regions(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_allowed_substate_regions(context.id, value);
}

void cb_body::allowed_countries(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_allowed_countries(context.id, value);
}

void cb_body::can_use(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_can_use(context.id, value);
}

void cb_body::on_add(dcon::effect_key value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_on_add(context.id, value);
}

void cb_body::on_po_accepted(dcon::effect_key value, error_handler& err, int32_t line, individual_cb_context& context) {
	context.outer_context.state.world.cb_type_set_on_po_accepted(context.id, value);
}

void option_rules::build_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::build_factory;
}

void option_rules::expand_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::expand_factory;
}

void option_rules::open_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::open_factory;
}

void option_rules::destroy_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::destroy_factory;
}

void option_rules::factory_priority(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::factory_priority;
}

void option_rules::can_subsidise(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::can_subsidise;
}

void option_rules::pop_build_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_build_factory;
}

void option_rules::pop_expand_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_expand_factory;
}

void option_rules::pop_open_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_open_factory;
}

void option_rules::delete_factory_if_no_input(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::delete_factory_if_no_input;
}

void option_rules::build_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::build_factory_invest;
}

void option_rules::expand_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::expand_factory_invest;
}

void option_rules::open_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::open_factory_invest;
}

void option_rules::build_railway_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::build_railway_invest;
}

void option_rules::can_invest_in_pop_projects(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::can_invest_in_pop_projects;
}

void option_rules::pop_build_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_build_factory_invest;
}

void option_rules::pop_expand_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_expand_factory_invest;
}

void option_rules::pop_open_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_open_factory_invest;
}

void option_rules::allow_foreign_investment(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::allow_foreign_investment;
}

void option_rules::slavery_allowed(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::slavery_allowed;
}

void option_rules::primary_culture_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::primary_culture_voting;
}

void option_rules::culture_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::culture_voting;
}

void option_rules::all_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::all_voting;
}

void option_rules::largest_share(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::largest_share;
}

void option_rules::dhont(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::dhont;
}

void option_rules::sainte_laque(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::sainte_laque;
}

void option_rules::same_as_ruling_party(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::same_as_ruling_party;
}

void option_rules::rich_only(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::rich_only;
}

void option_rules::state_vote(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::state_vote;
}

void option_rules::population_vote(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::population_vote;
}

void option_rules::build_railway(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::build_railway;
}

void issue_option_body::technology_cost(association_type, int32_t value, error_handler& err, int32_t line, individual_option_context& context) {
	context.outer_context.state.world.issue_option_set_technology_cost(context.id, value);
}

void issue_option_body::war_exhaustion_effect(association_type, float value, error_handler& err, int32_t line, individual_option_context& context) {
	context.outer_context.state.world.issue_option_set_war_exhaustion_effect(context.id, value);
}

void issue_option_body::administrative_multiplier(association_type, float value, error_handler& err, int32_t line, individual_option_context& context) {
	context.outer_context.state.world.issue_option_set_administrative_multiplier(context.id, value);
}

void issue_option_body::is_jingoism(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
	if(value)
		context.outer_context.state.culture_definitions.jingoism = context.id;
}

void issue_option_body::on_execute(on_execute_body const& value, error_handler& err, int32_t line, individual_option_context& context) {
	context.outer_context.state.world.issue_option_set_on_execute_trigger(context.id, value.trigger);
	context.outer_context.state.world.issue_option_set_on_execute_effect(context.id, value.effect);
}

void national_focus::railroads(association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
	context.outer_context.state.world.national_focus_set_railroads(context.id, value);
}

void national_focus::icon(association_type, int32_t value, error_handler& err, int32_t line, national_focus_context& context) {
	context.outer_context.state.world.national_focus_set_icon(context.id, uint8_t(value));
}

void national_focus::limit(dcon::trigger_key value, error_handler& err, int32_t line, national_focus_context& context) {
	context.outer_context.state.world.national_focus_set_limit(context.id, value);
}

void national_focus::has_flashpoint(association_type, bool value, error_handler& err, int32_t line, national_focus_context& context) {
	if(value)
		context.outer_context.state.national_definitions.flashpoint_focus = context.id;
}

void national_focus::flashpoint_tension(association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
	context.outer_context.state.national_definitions.flashpoint_amount = value;
}

void national_focus::ideology(association_type, std::string_view value, error_handler& err, int32_t line, national_focus_context& context) {
	if(auto it = context.outer_context.map_of_ideologies.find(std::string(value)); it != context.outer_context.map_of_ideologies.end()) {
		context.outer_context.state.world.national_focus_set_ideology(context.id, it->second.id);
	} else {
		err.accumulated_errors += "Invalid ideology " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void national_focus::loyalty_value(association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
	context.outer_context.state.world.national_focus_set_loyalty_value(context.id, value);
}

void national_focus::immigrant_attract(association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
	context.outer_context.state.world.national_focus_set_immigrant_attract(context.id, value);
}

void national_focus::any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
	std::string str_label{ label };
	if(auto it = context.outer_context.map_of_poptypes.find(str_label); it != context.outer_context.map_of_poptypes.end()) {
		context.outer_context.state.world.national_focus_set_promotion_type(context.id, it->second);
		context.outer_context.state.world.national_focus_set_promotion_amount(context.id, value);
	} else if(auto itb = context.outer_context.map_of_commodity_names.find(str_label); itb != context.outer_context.map_of_commodity_names.end()) {
		context.outer_context.state.world.national_focus_set_production_focus(context.id, itb->second, value);
	} else {
		err.accumulated_errors += "Invalid pop type / commodity " + str_label + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void main_pop_type_file::promotion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
	context.state.culture_definitions.promotion_chance = value;
}

void main_pop_type_file::demotion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
	context.state.culture_definitions.demotion_chance = value;
}

void main_pop_type_file::migration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
	context.state.culture_definitions.migration_chance = value;
}

void main_pop_type_file::colonialmigration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
	context.state.culture_definitions.colonialmigration_chance = value;
}

void main_pop_type_file::emigration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
	context.state.culture_definitions.emigration_chance = value;
}

void main_pop_type_file::assimilation_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
	context.state.culture_definitions.assimilation_chance = value;
}

void main_pop_type_file::conversion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
	context.state.culture_definitions.conversion_chance = value;
}

void tech_rgo_goods_output::any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context) {
	if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
		context.outer_context.state.world.technology_get_rgo_goods_output(context.id).push_back(sys::commodity_modifier{ value, it->second });
	} else {
		err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void tech_fac_goods_output::any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context) {
	if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
		context.outer_context.state.world.technology_get_factory_goods_output(context.id).push_back(sys::commodity_modifier{ value, it->second });
	} else {
		err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void tech_rgo_size::any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context) {
	if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
		context.outer_context.state.world.technology_get_rgo_size(context.id).push_back(sys::commodity_modifier{ value, it->second });
	} else {
		err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void technology_contents::any_group(std::string_view label, unit_modifier_body const& value, error_handler& err, int32_t line, tech_context& context) {
	if(auto it = context.outer_context.map_of_unit_types.find(std::string(label)); it != context.outer_context.map_of_unit_types.end()) {
		sys::unit_modifier temp = value;
		temp.type = it->second;
		context.outer_context.state.world.technology_get_modified_units(context.id).push_back(temp);
	} else {
		err.accumulated_errors += "Invalid unit type " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void technology_contents::ai_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, tech_context& context) {
	context.outer_context.state.world.technology_set_ai_chance(context.id, value);
}

void technology_contents::year(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
	context.outer_context.state.world.technology_set_year(context.id, int16_t(value));
}

void technology_contents::cost(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
	context.outer_context.state.world.technology_set_cost(context.id, value);
}

void technology_contents::area(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context) {
	if(auto it = context.outer_context.map_of_tech_folders.find(std::string(value)); it != context.outer_context.map_of_tech_folders.end()) {
		context.outer_context.state.world.technology_set_folder_index(context.id, uint8_t(it->second));
	} else {
		err.accumulated_errors += "Invalid technology folder name " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void technology_contents::max_fort(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
	if(value == 1) {
		context.outer_context.state.world.technology_set_increase_fort(context.id, true);
	} else {
		err.accumulated_errors += "max_fort may only be 1 (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void technology_contents::max_railroad(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
	if(value == 1) {
		context.outer_context.state.world.technology_set_increase_railroad(context.id, true);
	} else {
		err.accumulated_errors += "max_railroad may only be 1 (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void technology_contents::max_naval_base(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
	if(value == 1) {
		context.outer_context.state.world.technology_set_increase_naval_base(context.id, true);
	} else {
		err.accumulated_errors += "max_naval_base may only be 1 (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void technology_contents::colonial_points(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
	context.outer_context.state.world.technology_set_colonial_points(context.id, int16_t(value));
}

void technology_contents::activate_unit(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context) {
	if(auto it = context.outer_context.map_of_unit_types.find(std::string(value)); it != context.outer_context.map_of_unit_types.end()) {
		context.outer_context.state.world.technology_set_activate_unit(context.id, it->second, true);
	} else {
		err.accumulated_errors += "Invalid unit type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void technology_contents::activate_building(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context) {
	if(is_fixed_token_ci(value.data(), value.data() + value.length(), "fort")) {
		context.outer_context.state.world.technology_set_increase_fort(context.id, true);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "railroad")) {
		context.outer_context.state.world.technology_set_increase_railroad(context.id, true);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "naval_base")) {
		context.outer_context.state.world.technology_set_increase_naval_base(context.id, true);
	} else if(auto it = context.outer_context.map_of_factory_names.find(std::string(value)); it != context.outer_context.map_of_factory_names.end()) {
		context.outer_context.state.world.technology_set_activate_building(context.id, it->second, true);
	} else {
		err.accumulated_errors += "Invalid factory type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_rgo_goods_output::any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, invention_context& context) {
	if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
		context.outer_context.state.world.invention_get_rgo_goods_output(context.id).push_back(sys::commodity_modifier{ value, it->second });
	} else {
		err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_fac_goods_output::any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, invention_context& context) {
	if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
		context.outer_context.state.world.invention_get_factory_goods_output(context.id).push_back(sys::commodity_modifier{ value, it->second });
	} else {
		err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_fac_goods_throughput::any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, invention_context& context) {
	if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
		context.outer_context.state.world.invention_get_factory_goods_throughput(context.id).push_back(sys::commodity_modifier{ value, it->second });
	} else {
		err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_rebel_org_gain::faction(association_type, std::string_view v, error_handler& err, int32_t line, invention_context& context) {
	if(is_fixed_token_ci(v.data(), v.data() + v.size(), "all")) {
		// do nothing
	} else if(auto it = context.outer_context.map_of_rebeltypes.find(std::string(v)); it != context.outer_context.map_of_rebeltypes.end()) {
		faction_ = it->second.id;
	} else {
		err.accumulated_errors += "Invalid rebel type " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_effect::any_group(std::string_view label, unit_modifier_body const& value, error_handler& err, int32_t line, invention_context& context) {
	if(auto it = context.outer_context.map_of_unit_types.find(std::string(label)); it != context.outer_context.map_of_unit_types.end()) {
		sys::unit_modifier temp = value;
		temp.type = it->second;
		context.outer_context.state.world.invention_get_modified_units(context.id).push_back(temp);
	} else {
		err.accumulated_errors += "Invalid unit type " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_effect::activate_unit(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context) {
	if(auto it = context.outer_context.map_of_unit_types.find(std::string(value)); it != context.outer_context.map_of_unit_types.end()) {
		context.outer_context.state.world.invention_set_activate_unit(context.id, it->second, true);
	} else {
		err.accumulated_errors += "Invalid unit type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_effect::activate_building(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context) {
	if(auto it = context.outer_context.map_of_factory_names.find(std::string(value)); it != context.outer_context.map_of_factory_names.end()) {
		context.outer_context.state.world.invention_set_activate_building(context.id, it->second, true);
	} else {
		err.accumulated_errors += "Invalid factory type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_effect::shared_prestige(association_type, float value, error_handler& err, int32_t line, invention_context& context) {
	context.outer_context.state.world.invention_set_shared_prestige(context.id, value);
}

void inv_effect::enable_crime(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context) {
	if(auto it = context.outer_context.map_of_crimes.find(std::string(value)); it != context.outer_context.map_of_crimes.end()) {
		context.outer_context.state.world.invention_set_activate_crime(context.id, it->second.id, true);
	} else {
		err.accumulated_errors += "Invalid crime " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void inv_effect::gas_attack(association_type, bool value, error_handler& err, int32_t line, invention_context& context) {
	context.outer_context.state.world.invention_set_enable_gas_attack(context.id, value);
}

void inv_effect::gas_defence(association_type, bool value, error_handler& err, int32_t line, invention_context& context) {
	context.outer_context.state.world.invention_set_enable_gas_defence(context.id, value);
}

void inv_effect::rebel_org_gain(inv_rebel_org_gain const& value, error_handler& err, int32_t line, invention_context& context) {
	context.outer_context.state.world.invention_get_rebel_org(context.id).push_back(sys::rebel_org_modifier{ value.value, value.faction_ });
}

void invention_contents::limit(dcon::trigger_key value, error_handler& err, int32_t line, invention_context& context) {
	context.outer_context.state.world.invention_set_limit(context.id, value);
}

void invention_contents::chance(dcon::value_modifier_key value, error_handler& err, int32_t line, invention_context& context) {
	context.outer_context.state.world.invention_set_chance(context.id, value);
}

void invention_contents::effect(inv_effect const& value, error_handler& err, int32_t line, invention_context& context) {
	for(uint32_t i = 0; i < value.next_to_add; ++i) {
		if(next_to_add >= sys::modifier_definition_size) {
			err.accumulated_errors += "Too many modifiers attached to invention (" + err.file_name + " line " + std::to_string(line) + ")\n";
			break;
		}
		constructed_definition.offsets[next_to_add] = value.constructed_definition.offsets[i];
		constructed_definition.values[next_to_add] = value.constructed_definition.values[i];
		++next_to_add;
	}
}

void s_on_yearly_pulse::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_yearly_pulse.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_yearly_pulse.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_quarterly_pulse::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_quarterly_pulse.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_quarterly_pulse.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_battle_won::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_provincial_events.find(event); it != context.map_of_provincial_events.end()) {
		context.state.national_definitions.on_battle_won.push_back(nations::fixed_province_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_provincial_event();
		context.map_of_provincial_events.insert_or_assign(event, pending_prov_event { id, trigger::slot_contents::province, trigger::slot_contents::nation, trigger::slot_contents::nation });
		context.state.national_definitions.on_battle_won.push_back(nations::fixed_province_event{ int16_t(value), id });
	}
}

void s_on_battle_lost::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_provincial_events.find(event); it != context.map_of_provincial_events.end()) {
		context.state.national_definitions.on_battle_lost.push_back(nations::fixed_province_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_provincial_event();
		context.map_of_provincial_events.insert_or_assign(event, pending_prov_event { id, trigger::slot_contents::province, trigger::slot_contents::nation, trigger::slot_contents::nation });
		context.state.national_definitions.on_battle_lost.push_back(nations::fixed_province_event{ int16_t(value), id });
	}
}

void s_on_surrender::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_surrender.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
		context.state.national_definitions.on_surrender.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_new_great_nation::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_new_great_nation.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_new_great_nation.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_lost_great_nation::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_lost_great_nation.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_lost_great_nation.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_election_tick::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_election_tick.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_election_tick.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_colony_to_state::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_colony_to_state.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::state, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_colony_to_state.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_state_conquest::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_state_conquest.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::state, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_state_conquest.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_colony_to_state_free_slaves::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_colony_to_state_free_slaves.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::state, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_colony_to_state_free_slaves.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_debtor_default::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_debtor_default.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
		context.state.national_definitions.on_debtor_default.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_debtor_default_small::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_debtor_default_small.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
		context.state.national_definitions.on_debtor_default_small.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_debtor_default_second::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_debtor_default_second.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
		context.state.national_definitions.on_debtor_default_second.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_civilize::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_civilize.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_civilize.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_crisis_declare_interest::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_crisis_declare_interest.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
		context.state.national_definitions.on_crisis_declare_interest.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void s_on_my_factories_nationalized::any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
	int32_t value = parse_int(chance, line, err);
	if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
		context.state.national_definitions.on_my_factories_nationalized.push_back(nations::fixed_event{ int16_t(value), it->second.id });
	} else {
		auto id = context.state.world.create_national_event();
		context.map_of_national_events.insert_or_assign(event, pending_nat_event { id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
		context.state.national_definitions.on_my_factories_nationalized.push_back(nations::fixed_event{ int16_t(value), id });
	}
}

void rebel_gov_list::any_value(std::string_view from_gov, association_type, std::string_view to_gov, error_handler& err, int32_t line, rebel_context& context) {
	if(auto frit = context.outer_context.map_of_governments.find(std::string(from_gov)); frit != context.outer_context.map_of_governments.end()) {
		if(auto toit = context.outer_context.map_of_governments.find(std::string(to_gov)); toit != context.outer_context.map_of_governments.end()) {
			context.outer_context.state.world.rebel_type_set_government_change(context.id, frit->second, toit->second);
		} else {
			err.accumulated_errors += "Invalid government " + std::string(to_gov) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	} else {
		err.accumulated_errors += "Invalid government " + std::string(from_gov) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void rebel_body::icon(association_type, int32_t value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_icon(context.id, uint8_t(value));
}

void rebel_body::break_alliance_on_win(association_type, bool value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_break_alliance_on_win(context.id, value);
}

void rebel_body::area(association_type, std::string_view value, error_handler& err, int32_t line, rebel_context& context) {
	if(is_fixed_token_ci(value.data(), value.data() + value.length(), "none"))
		context.outer_context.state.world.rebel_type_set_area(context.id, uint8_t(::culture::rebel_area::none));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "nation"))
		context.outer_context.state.world.rebel_type_set_area(context.id, uint8_t(::culture::rebel_area::nation));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "culture"))
		context.outer_context.state.world.rebel_type_set_area(context.id, uint8_t(::culture::rebel_area::culture));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "nation_culture"))
		context.outer_context.state.world.rebel_type_set_area(context.id, uint8_t(::culture::rebel_area::nation_culture));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "nation_religion"))
		context.outer_context.state.world.rebel_type_set_area(context.id, uint8_t(::culture::rebel_area::nation_religion));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "religion"))
		context.outer_context.state.world.rebel_type_set_area(context.id, uint8_t(::culture::rebel_area::religion));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "culture_group"))
		context.outer_context.state.world.rebel_type_set_area(context.id, uint8_t(::culture::rebel_area::culture_group));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "all"))
		context.outer_context.state.world.rebel_type_set_area(context.id, uint8_t(::culture::rebel_area::all));
	else {
		err.accumulated_errors += "Invalid rebel area " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void rebel_body::defection(association_type, std::string_view value, error_handler& err, int32_t line, rebel_context& context) {
	if(is_fixed_token_ci(value.data(), value.data() + value.length(), "none"))
		context.outer_context.state.world.rebel_type_set_defection(context.id, uint8_t(::culture::rebel_defection::none));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "ideology"))
		context.outer_context.state.world.rebel_type_set_defection(context.id, uint8_t(::culture::rebel_defection::ideology));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "culture"))
		context.outer_context.state.world.rebel_type_set_defection(context.id, uint8_t(::culture::rebel_defection::culture));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "any"))
		context.outer_context.state.world.rebel_type_set_defection(context.id, uint8_t(::culture::rebel_defection::any));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "pan_nationalist"))
		context.outer_context.state.world.rebel_type_set_defection(context.id, uint8_t(::culture::rebel_defection::pan_nationalist));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "religion"))
		context.outer_context.state.world.rebel_type_set_defection(context.id, uint8_t(::culture::rebel_defection::religion));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "culture_group"))
		context.outer_context.state.world.rebel_type_set_defection(context.id, uint8_t(::culture::rebel_defection::culture_group));
	else {
		err.accumulated_errors += "Invalid rebel defection " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void rebel_body::independence(association_type, std::string_view value, error_handler& err, int32_t line, rebel_context& context) {
	if(is_fixed_token_ci(value.data(), value.data() + value.length(), "none"))
		context.outer_context.state.world.rebel_type_set_independence(context.id, uint8_t(::culture::rebel_independence::none));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "colonial"))
		context.outer_context.state.world.rebel_type_set_independence(context.id, uint8_t(::culture::rebel_independence::colonial));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "culture"))
		context.outer_context.state.world.rebel_type_set_independence(context.id, uint8_t(::culture::rebel_independence::culture));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "any"))
		context.outer_context.state.world.rebel_type_set_independence(context.id, uint8_t(::culture::rebel_independence::any));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "pan_nationalist"))
		context.outer_context.state.world.rebel_type_set_independence(context.id, uint8_t(::culture::rebel_independence::pan_nationalist));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "religion"))
		context.outer_context.state.world.rebel_type_set_independence(context.id, uint8_t(::culture::rebel_independence::religion));
	else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "culture_group"))
		context.outer_context.state.world.rebel_type_set_independence(context.id, uint8_t(::culture::rebel_independence::culture_group));
	else {
		err.accumulated_errors += "Invalid rebel independence " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void rebel_body::defect_delay(association_type, int32_t value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_defect_delay(context.id, uint8_t(value));
}

void rebel_body::ideology(association_type, std::string_view value, error_handler& err, int32_t line, rebel_context& context) {
	if(auto it = context.outer_context.map_of_ideologies.find(std::string(value)); it != context.outer_context.map_of_ideologies.end()) {
		context.outer_context.state.world.rebel_type_set_ideology(context.id, it->second.id);
	} else {
		err.accumulated_errors += "Invalid ideology " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void rebel_body::allow_all_cultures(association_type, bool value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_culture_restriction(context.id, !value);
}

void rebel_body::allow_all_culture_groups(association_type, bool value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_culture_group_restriction(context.id, !value);
}

void rebel_body::occupation_mult(association_type, float value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_occupation_multiplier(context.id, uint8_t(value));
}

void rebel_body::will_rise(dcon::value_modifier_key value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_will_rise(context.id, value);
}

void rebel_body::spawn_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_spawn_chance(context.id, value);
}

void rebel_body::movement_evaluation(dcon::value_modifier_key value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_movement_evaluation(context.id, value);
}

void rebel_body::siege_won_trigger(dcon::trigger_key value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_siege_won_trigger(context.id, value);
}

void rebel_body::demands_enforced_trigger(dcon::trigger_key value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_demands_enforced_trigger(context.id, value);
}

void rebel_body::siege_won_effect(dcon::effect_key value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_siege_won_effect(context.id, value);
}

void rebel_body::demands_enforced_effect(dcon::effect_key value, error_handler& err, int32_t line, rebel_context& context) {
	context.outer_context.state.world.rebel_type_set_demands_enforced_effect(context.id, value);
}

void decision::potential(dcon::trigger_key value, error_handler& err, int32_t line, decision_context& context) {
	context.outer_context.state.world.decision_set_potential(context.id, value);
}

void decision::allow(dcon::trigger_key value, error_handler& err, int32_t line, decision_context& context) {
	context.outer_context.state.world.decision_set_allow(context.id, value);
}

void decision::effect(dcon::effect_key value, error_handler& err, int32_t line, decision_context& context) {
	context.outer_context.state.world.decision_set_effect(context.id, value);
}

void decision::ai_will_do(dcon::value_modifier_key value, error_handler& err, int32_t line, decision_context& context) {
	context.outer_context.state.world.decision_set_ai_will_do(context.id, value);
}

void oob_leader::name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_context& context) {
	name_ = context.outer_context.state.add_unit_name(value);
}

void oob_leader::date(association_type, sys::year_month_day value, error_handler& err, int32_t line, oob_file_context& context) {
	date_ = sys::date(value, context.outer_context.state.start_date);
}

void oob_army::name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_army_context& context) {
	context.outer_context.state.world.army_set_name(context.id, context.outer_context.state.add_unit_name(value));
}

void oob_army::location(association_type, int32_t value, error_handler& err, int32_t line, oob_file_army_context& context) {
	if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
		err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
	} else {
		auto province_id = context.outer_context.original_id_to_prov_id_map[value];
		context.outer_context.state.world.force_create_army_location(context.id, province_id);
	}
}

void oob_army::leader(oob_leader const& value, error_handler& err, int32_t line, oob_file_army_context& context) {
	if(value.is_general) {
		auto l_id = fatten(context.outer_context.state.world, context.outer_context.state.world.create_general());
		l_id.set_background(value.background_);
		l_id.set_personality(value.personality_);
		l_id.set_prestige(value.prestige);
		l_id.set_since(value.date_);
		l_id.set_name(value.name_);
		context.outer_context.state.world.force_create_general_loyalty(context.nation_for, l_id);
		context.outer_context.state.world.force_create_army_leadership(context.id, l_id);
	} else {
		err.accumulated_errors += "Cannot attach an admiral to an army (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void oob_navy::name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_navy_context& context) {
	context.outer_context.state.world.navy_set_name(context.id, context.outer_context.state.add_unit_name(value));
}

void oob_navy::location(association_type, int32_t value, error_handler& err, int32_t line, oob_file_navy_context& context) {
	if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
		err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
	} else {
		auto province_id = context.outer_context.original_id_to_prov_id_map[value];
		context.outer_context.state.world.force_create_navy_location(context.id, province_id);
	}
}

void oob_ship::name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_ship_context& context) {
	context.outer_context.state.world.ship_set_name(context.id, context.outer_context.state.add_unit_name(value));
}

void oob_ship::type(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_ship_context& context) {
	if(auto it = context.outer_context.map_of_unit_types.find(std::string(value)); it != context.outer_context.map_of_unit_types.end()) {
		context.outer_context.state.world.ship_set_type(context.id, it->second);
	} else {
		err.accumulated_errors += "Invalid unit type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void oob_regiment::name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_regiment_context& context) {
	context.outer_context.state.world.regiment_set_name(context.id, context.outer_context.state.add_unit_name(value));
}

void oob_regiment::type(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_regiment_context& context) {
	if(auto it = context.outer_context.map_of_unit_types.find(std::string(value)); it != context.outer_context.map_of_unit_types.end()) {
		context.outer_context.state.world.regiment_set_type(context.id, it->second);
	} else {
		err.accumulated_errors += "Invalid unit type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void oob_regiment::home(association_type, int32_t value, error_handler& err, int32_t line, oob_file_regiment_context& context) {
	if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
		err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
	} else {
		auto province_id = context.outer_context.original_id_to_prov_id_map[value];
		for(auto pl : context.outer_context.state.world.province_get_pop_location(province_id)) {
			auto p = pl.get_pop();
			if(p.get_poptype() == context.outer_context.state.culture_definitions.soldiers) {
				context.outer_context.state.world.force_create_regiment_source(context.id, p);
				return;
			}
		}
		err.accumulated_errors += "No soldiers in province regiment comes from (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void oob_relationship::value(association_type, int32_t v, error_handler& err, int32_t line, oob_file_relation_context& context) {
	auto rel = context.outer_context.state.world.get_diplomatic_relation_by_diplomatic_pair(context.nation_for, context.nation_with);
	if(rel) {
		context.outer_context.state.world.diplomatic_relation_set_value(rel, v);
	} else {
		auto new_rel = context.outer_context.state.world.force_create_diplomatic_relation(context.nation_for, context.nation_with);
		context.outer_context.state.world.diplomatic_relation_set_value(new_rel, v);
	}
}

void oob_relationship::level(association_type, int32_t v, error_handler& err, int32_t line, oob_file_relation_context& context) {
	auto rel = context.outer_context.state.world.get_gp_relationship_by_gp_influence_pair(context.nation_with, context.nation_for);
	auto status_level = [&]() {
		switch(v) {
			case 0: return nations::influence::level_hostile;
			case 1: return nations::influence::level_opposed;
			case 2: return nations::influence::level_neutral;
			case 3: return nations::influence::level_cordial;
			case 4: return nations::influence::level_friendly;
			case 5: return nations::influence::level_in_sphere;
			default: return nations::influence::level_neutral;
		}
	}();
	if(rel) {
		context.outer_context.state.world.gp_relationship_set_status(rel, status_level);
	} else {
		auto new_rel = context.outer_context.state.world.force_create_gp_relationship(context.nation_with, context.nation_for);
		context.outer_context.state.world.gp_relationship_set_status(new_rel, status_level);
	}
}

void oob_relationship::influence_value(association_type, float v, error_handler& err, int32_t line, oob_file_relation_context& context) {
	auto rel = context.outer_context.state.world.get_gp_relationship_by_gp_influence_pair(context.nation_with, context.nation_for);
	if(rel) {
		context.outer_context.state.world.gp_relationship_set_influence(rel, v);
	} else {
		auto new_rel = context.outer_context.state.world.force_create_gp_relationship(context.nation_with, context.nation_for);
		context.outer_context.state.world.gp_relationship_set_influence(new_rel, v);
	}
}

void oob_file::leader(oob_leader const& value, error_handler& err, int32_t line, oob_file_context& context) {
	if(value.is_general) {
		auto l_id = fatten(context.outer_context.state.world, context.outer_context.state.world.create_general());
		l_id.set_background(value.background_);
		l_id.set_personality(value.personality_);
		l_id.set_prestige(value.prestige);
		l_id.set_since(value.date_);
		l_id.set_name(value.name_);
		context.outer_context.state.world.force_create_general_loyalty(context.nation_for, l_id);
	} else {
		auto l_id = fatten(context.outer_context.state.world, context.outer_context.state.world.create_admiral());
		l_id.set_background(value.background_);
		l_id.set_personality(value.personality_);
		l_id.set_prestige(value.prestige);
		l_id.set_since(value.date_);
		l_id.set_name(value.name_);
		context.outer_context.state.world.force_create_admiral_loyalty(context.nation_for, l_id);
	}
}

void production_employee::poptype(association_type, std::string_view v, error_handler& err, int32_t line, production_context& context) {
	if(is_fixed_token_ci(v.data(), v.data() + v.length(), "artisan")) {
		type = context.outer_context.state.culture_definitions.artisans;
	} else if(auto it = context.outer_context.map_of_poptypes.find(std::string(v)); it != context.outer_context.map_of_poptypes.end()) {
		type = it->second;
	} else {
		err.accumulated_errors += "Invalid pop type " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void alliance::first(association_type, std::string_view tag, error_handler& err, int32_t line, scenario_building_context& context) {
	if(tag.length() == 3) {
		if(auto it = context.map_of_ident_names.find(nations::tag_to_int(tag[0], tag[1], tag[2])); it != context.map_of_ident_names.end()) {
			first_ = context.state.world.national_identity_get_nation_from_identity_holder(it->second);
		} else {
			err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	} else {
		err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void alliance::second(association_type, std::string_view tag, error_handler& err, int32_t line, scenario_building_context& context) {
	if(tag.length() == 3) {
		if(auto it = context.map_of_ident_names.find(nations::tag_to_int(tag[0], tag[1], tag[2])); it != context.map_of_ident_names.end()) {
			second_ = context.state.world.national_identity_get_nation_from_identity_holder(it->second);
		} else {
			err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	} else {
		err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void vassal_description::first(association_type, std::string_view tag, error_handler& err, int32_t line, scenario_building_context& context) {
	if(tag.length() == 3) {
		if(auto it = context.map_of_ident_names.find(nations::tag_to_int(tag[0], tag[1], tag[2])); it != context.map_of_ident_names.end()) {
			first_ = context.state.world.national_identity_get_nation_from_identity_holder(it->second);
		} else {
			err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	} else {
		err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void vassal_description::second(association_type, std::string_view tag, error_handler& err, int32_t line, scenario_building_context& context) {
	if(tag.length() == 3) {
		if(auto it = context.map_of_ident_names.find(nations::tag_to_int(tag[0], tag[1], tag[2])); it != context.map_of_ident_names.end()) {
			second_ = context.state.world.national_identity_get_nation_from_identity_holder(it->second);
		} else {
			err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	} else {
		err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void vassal_description::start_date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, scenario_building_context& context) {
	if(context.state.start_date < sys::absolute_time_point(ymd))
		invalid = true;
}

void govt_flag_block::flag(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(auto it = context.outer_context.map_of_governments.find(std::string(value)); it != context.outer_context.map_of_governments.end()) {
		flag_ = context.outer_context.state.culture_definitions.governments[it->second].flag;
	} else {
		err.accumulated_errors += "invalid government type " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void upper_house_block::any_value(std::string_view value, association_type, float v, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;

	if(auto it = context.outer_context.map_of_ideologies.find(std::string(value)); it != context.outer_context.map_of_ideologies.end()) {
		context.outer_context.state.world.nation_set_upper_house(context.holder_id, it->second.id, v);
	} else {
		err.accumulated_errors += "invalid ideology " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void foreign_investment_block::any_value(std::string_view tag, association_type, float v, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;

	if(tag.length() == 3) {
		if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(tag[0], tag[1], tag[2])); it != context.outer_context.map_of_ident_names.end()) {
			auto other = context.outer_context.state.world.national_identity_get_nation_from_identity_holder(it->second);
			auto rel_id = context.outer_context.state.world.force_create_unilateral_relationship(other, context.holder_id);
			context.outer_context.state.world.unilateral_relationship_set_foreign_investment(rel_id, v);
		} else {
			err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	} else {
		err.accumulated_errors += "invalid tag " + std::string(tag) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void country_history_file::set_country_flag(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(auto it = context.outer_context.map_of_national_flags.find(std::string(value)); it != context.outer_context.map_of_national_flags.end()) {
		if(context.holder_id)
			context.outer_context.state.world.nation_set_flag_variables(context.holder_id, it->second, true);
	} else {
		// unused flag variable: ignore
	}
}

void country_history_file::capital(association_type, int32_t value, error_handler& err, int32_t line, country_history_context& context) {
	if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
		err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
	} else {
		auto province_id = context.outer_context.original_id_to_prov_id_map[value];
		context.outer_context.state.world.national_identity_set_capital(context.nat_ident, province_id);
		if(context.holder_id)
			context.outer_context.state.world.nation_set_capital(context.holder_id, province_id);
	}
}

void country_history_file::any_value(std::string_view label, association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;

	std::string str_label(label);
	if(auto it = context.outer_context.map_of_technologies.find(str_label); it != context.outer_context.map_of_technologies.end()) {
		auto v = parse_bool(value, line, err);
		context.outer_context.state.world.nation_set_active_technologies(context.holder_id, it->second.id, v);
	} else if(auto itb = context.outer_context.map_of_inventions.find(str_label); itb != context.outer_context.map_of_inventions.end()) {
		auto v = parse_bool(value, line, err);
		context.outer_context.state.world.nation_set_active_inventions(context.holder_id, itb->second.id, v);
	} else if(auto itc = context.outer_context.map_of_issues.find(str_label); itc != context.outer_context.map_of_issues.end()) {
		if(auto itd = context.outer_context.map_of_options.find(std::string(value)); itd != context.outer_context.map_of_options.end()) {
			context.outer_context.state.world.nation_set_reforms_and_issues(context.holder_id, itc->second, itd->second.id);
		} else {
			err.accumulated_errors += "invalid issue option name " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	} else {
		err.accumulated_errors += "invalid key " + str_label + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void country_history_file::primary_culture(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(auto it = context.outer_context.map_of_culture_names.find(std::string(value)); it != context.outer_context.map_of_culture_names.end()) {
		context.outer_context.state.world.national_identity_set_primary_culture(context.nat_ident, it->second);
		if(context.holder_id)
			context.outer_context.state.world.nation_set_primary_culture(context.holder_id, it->second);
	} else {
		err.accumulated_errors += "invalid culture " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void country_history_file::culture(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;

	if(auto it = context.outer_context.map_of_culture_names.find(std::string(value)); it != context.outer_context.map_of_culture_names.end()) {
		context.outer_context.state.world.nation_get_accepted_cultures(context.holder_id).push_back(it->second);
	} else {
		err.accumulated_errors += "invalid culture " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void country_history_file::religion(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(auto it = context.outer_context.map_of_religion_names.find(std::string(value)); it != context.outer_context.map_of_religion_names.end()) {
		context.outer_context.state.world.national_identity_set_religion(context.nat_ident, it->second);
		if(context.holder_id)
			context.outer_context.state.world.nation_set_religion(context.holder_id, it->second);
	} else {
		err.accumulated_errors += "invalid religion " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void country_history_file::government(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;

	if(auto it = context.outer_context.map_of_governments.find(std::string(value)); it != context.outer_context.map_of_governments.end()) {
		context.outer_context.state.world.nation_set_government_type(context.holder_id, it->second);
	} else {
		err.accumulated_errors += "invalid government type " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void country_history_file::plurality(association_type, float value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;
	context.outer_context.state.world.nation_set_plurality(context.holder_id, value);
}

void country_history_file::prestige(association_type, float value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;
	context.outer_context.state.world.nation_set_prestige(context.holder_id, value);
}

void country_history_file::nationalvalue(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;

	if(auto it = context.outer_context.map_of_modifiers.find(std::string(value)); it != context.outer_context.map_of_modifiers.end()) {
		context.outer_context.state.world.nation_set_national_value(context.holder_id, it->second);
	} else {
		err.accumulated_errors += "invalid modifier " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void country_history_file::schools(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;

	if(auto it = context.outer_context.map_of_modifiers.find(std::string(value)); it != context.outer_context.map_of_modifiers.end()) {
		context.outer_context.state.world.nation_set_tech_school(context.holder_id, it->second);
	} else {
		err.accumulated_errors += "invalid modifier " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void country_history_file::civilized(association_type, bool value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;
	context.outer_context.state.world.nation_set_is_civilized(context.holder_id, value);
}

void country_history_file::is_releasable_vassal(association_type, bool value, error_handler& err, int32_t line, country_history_context& context) {
	context.outer_context.state.world.national_identity_set_is_releasable(context.nat_ident, value);
}

void country_history_file::literacy(association_type, float value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;
	for(auto owned_prov : context.outer_context.state.world.nation_get_province_ownership(context.holder_id)) {
		for(auto prov_pop : owned_prov.get_province().get_pop_location()) {
			prov_pop.get_pop().set_literacy(value);
		}
	}
}

void country_history_file::non_state_culture_literacy(association_type, float value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;
	auto fh = fatten(context.outer_context.state.world, context.holder_id);
	for(auto owned_prov : context.outer_context.state.world.nation_get_province_ownership(context.holder_id)) {
		for(auto prov_pop : owned_prov.get_province().get_pop_location()) {
			bool non_accepted = [&]() {
				if(prov_pop.get_pop().get_culture() == fh.get_primary_culture())
					return false;
				for(auto c : fh.get_accepted_cultures()) {
					if(prov_pop.get_pop().get_culture() == c)
						return false;
				}
				return true;
			}();
			if(non_accepted)
				prov_pop.get_pop().set_literacy(value);
		}
	}
}

void country_history_file::consciousness(association_type, float value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;
	for(auto owned_prov : context.outer_context.state.world.nation_get_province_ownership(context.holder_id)) {
		for(auto prov_pop : owned_prov.get_province().get_pop_location()) {
			prov_pop.get_pop().set_consciousness(value);
		}
	}
}

void country_history_file::nonstate_consciousness(association_type, float value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;
	for(auto owned_prov : context.outer_context.state.world.nation_get_province_ownership(context.holder_id)) {
		if(owned_prov.get_province().get_is_colonial()) {
			for(auto prov_pop : owned_prov.get_province().get_pop_location()) {
				prov_pop.get_pop().set_consciousness(value);
			}
		}
	}
}

void country_history_file::govt_flag(const govt_flag_block& value, error_handler& err, int32_t line, country_history_context& context) {
	context.outer_context.state.world.national_identity_set_government_flag_type(context.nat_ident, value.government_, uint8_t(value.flag_) + uint8_t(1));
}

void country_history_file::ruling_party(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
	if(!context.holder_id)
		return;

	auto value_key = [&]() {
		auto it = context.outer_context.state.key_to_text_sequence.find(lowercase_str(value));
		if(it != context.outer_context.state.key_to_text_sequence.end()) {
			return it->second;
		}
		return dcon::text_sequence_id();
	}();

	auto first_party = context.outer_context.state.world.national_identity_get_political_party_first(context.nat_ident);
	auto party_count = context.outer_context.state.world.national_identity_get_political_party_count(context.nat_ident);
	for(uint32_t i = 0; i < party_count; ++i) {
		dcon::political_party_id pid{ dcon::political_party_id::value_base_t(first_party.id.index() + i) };
		auto name = context.outer_context.state.world.political_party_get_name(pid);
		if(name == value_key) {
			context.outer_context.state.world.nation_set_ruling_party(context.holder_id, pid);
			for(auto p_issue : context.outer_context.state.culture_definitions.party_issues) {
				context.outer_context.state.world.nation_set_reforms_and_issues(context.holder_id, p_issue,
					context.outer_context.state.world.political_party_get_party_issues(pid, p_issue)
				);
			}
			return;
		}
	}
	err.accumulated_errors += "invalid political party " + std::string(value) + " encountered  (" + err.file_name + " line " + std::to_string(line) + ")\n";
}

void commodity_array::finish(scenario_building_context& context) {
	data.resize(context.state.world.commodity_size());
}

void country_file::color(color_from_3i cvalue, error_handler& err, int32_t line, country_file_context& context) {
	context.outer_context.state.world.national_identity_set_color(context.id, cvalue.value);
}

void generic_event::title(association_type, std::string_view value, error_handler& err, int32_t line, event_building_context& context) {
	title_ = text::find_or_add_key(context.outer_context.state, value);
}

void generic_event::desc(association_type, std::string_view value, error_handler& err, int32_t line, event_building_context& context) {
	desc_ = text::find_or_add_key(context.outer_context.state, value);
}

void generic_event::option(sys::event_option const& value, error_handler& err, int32_t line, event_building_context& context) {
	if(last_option_added < sys::max_event_options) {
		options[last_option_added] = value;
		++last_option_added;
	} else {
		err.accumulated_errors += "Event given too many options (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void generic_event::picture(association_type, std::string_view value, error_handler& err, int32_t line, event_building_context& context) {
	picture_ = context.outer_context.state.add_unique_to_pool(std::string(value));
}

}

