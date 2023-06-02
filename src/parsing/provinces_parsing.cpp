#include "parsers_declarations.hpp"
#include <algorithm>
#include <iterator>
#include "container_types.hpp"

namespace parsers {
void default_map_file::max_provinces(association_type, int32_t value, error_handler &err, int32_t line, scenario_building_context &context) {
	context.state.world.province_resize(int32_t(value - 1));
	context.original_id_to_prov_id_map.resize(value);
	context.prov_id_to_original_id_map.resize(value - 1);

	for (int32_t i = 1; i < value; ++i) {
		context.prov_id_to_original_id_map[dcon::province_id(dcon::province_id::value_base_t(i - 1))].id = i;
	}
}
void default_map_file::finish(scenario_building_context &context) {
	auto first_sea = std::stable_partition(context.prov_id_to_original_id_map.begin(), context.prov_id_to_original_id_map.end(),
	                                       [](province_data const &a) { return !(a.is_sea); });

	for (size_t i = context.prov_id_to_original_id_map.size(); i-- > 0;) {
		dcon::province_id id = dcon::province_id(dcon::province_id::value_base_t(i));
		auto old_id = context.prov_id_to_original_id_map[id].id;
		context.original_id_to_prov_id_map[old_id] = id;

		auto name = std::string("PROV") + std::to_string(old_id);
		auto name_id = text::find_or_add_key(context.state, name);

		context.state.world.province_set_name(id, name_id);
	}

	auto to_first_sea = std::distance(context.prov_id_to_original_id_map.begin(), first_sea);
	context.state.province_definitions.first_sea_province = dcon::province_id(dcon::province_id::value_base_t(to_first_sea));
}

void read_map_colors(char const *start, char const *end, error_handler &err, scenario_building_context &context) {
	char const *cpos = parsers::csv_advance_to_next_line(start, end); // first line is always useless
	while (cpos < end) {
		cpos = parsers::parse_fixed_amount_csv_values<4>(cpos, end, ';',
		                                                 [&](std::string_view const *values) {
			                                                 auto first_text = parsers::remove_surrounding_whitespace(values[0]);
			                                                 if (first_text.length() > 0) {
				                                                 auto first_value = parsers::parse_int(first_text, 0, err);
				                                                 if (first_value == 0) {
					                                                 // dead line
				                                                 } else if (size_t(first_value) >= context.original_id_to_prov_id_map.size()) {
					                                                 err.accumulated_errors += "Province id " + std::to_string(first_value) + " is too large (" + err.file_name + ")\n";
				                                                 } else {
					                                                 auto province_id = context.original_id_to_prov_id_map[first_value];
					                                                 auto color_value = sys::pack_color(
					                                                     parsers::parse_int(parsers::remove_surrounding_whitespace(values[1]), 0, err),
					                                                     parsers::parse_int(parsers::remove_surrounding_whitespace(values[2]), 0, err),
					                                                     parsers::parse_int(parsers::remove_surrounding_whitespace(values[3]), 0, err));
					                                                 context.map_color_to_province_id.insert_or_assign(color_value, province_id);
				                                                 }
			                                                 }
		                                                 });
	}
}

void read_map_adjacency(char const *start, char const *end, error_handler &err, scenario_building_context &context) {
	char const *cpos = parsers::csv_advance_to_next_line(start, end); // first line is always useless
	while (cpos < end) {
		// 2711;2721;canal;369;1
		cpos = parsers::parse_fixed_amount_csv_values<5>(cpos, end, ';',
		                                                 [&](std::string_view const *values) {
			                                                 auto first_text = parsers::remove_surrounding_whitespace(values[0]);
			                                                 auto second_text = parsers::remove_surrounding_whitespace(values[1]);
			                                                 if (first_text.length() > 0) {
				                                                 // 2204;2206;sea;3001;0;Panama strait
				                                                 auto first_value = parsers::parse_int(first_text, 0, err);
				                                                 auto second_value = parsers::parse_int(second_text, 0, err);
				                                                 if (first_value == 0 || second_value == 0) {
					                                                 // dead line
				                                                 } else if (size_t(first_value) >= context.original_id_to_prov_id_map.size()) {
					                                                 err.accumulated_errors += "Province id " + std::to_string(first_value) + " is too large (" + err.file_name + ")\n";
				                                                 } else if (size_t(second_value) >= context.original_id_to_prov_id_map.size()) {
					                                                 err.accumulated_errors += "Province id " + std::to_string(first_value) + " is too large (" + err.file_name + ")\n";
				                                                 } else {
					                                                 auto province_id_a = context.original_id_to_prov_id_map[first_value];
					                                                 auto province_id_b = context.original_id_to_prov_id_map[second_value];

					                                                 auto ttex = parsers::remove_surrounding_whitespace(values[2]);
					                                                 if (is_fixed_token_ci(ttex.data(), ttex.data() + ttex.length(), "sea")) {
						                                                 auto new_rel = context.state.world.force_create_province_adjacency(province_id_a, province_id_b);
						                                                 context.state.world.province_adjacency_set_type(new_rel, province::border::non_adjacent_bit);
					                                                 } else if (is_fixed_token_ci(ttex.data(), ttex.data() + ttex.length(), "impassable")) {
						                                                 auto new_rel = context.state.world.force_create_province_adjacency(province_id_a, province_id_b);
						                                                 context.state.world.province_adjacency_set_type(new_rel, province::border::impassible_bit);
					                                                 } else if (is_fixed_token_ci(ttex.data(), ttex.data() + ttex.length(), "canal")) {
						                                                 auto new_rel = context.state.world.force_create_province_adjacency(province_id_a, province_id_b);
						                                                 context.state.world.province_adjacency_set_type(new_rel, province::border::non_adjacent_bit | province::border::impassible_bit);

						                                                 auto canal_id = parsers::parse_uint(parsers::remove_surrounding_whitespace(values[4]), 0, err);

						                                                 if (context.state.province_definitions.canals.size() < canal_id) {
							                                                 context.state.province_definitions.canals.resize(canal_id);
						                                                 }
						                                                 context.state.province_definitions.canals[canal_id - 1] = new_rel;
					                                                 }
				                                                 }
			                                                 }
		                                                 });
	}
}

void palette_definition::finish(scenario_building_context &context) {
	if (color.free_value == 254) {
		auto it = context.map_of_terrain_types.find(std::string(type));
		if (it != context.map_of_terrain_types.end()) {
			context.ocean_terrain = it->second.id;
		}
	}

	if (color.free_value < 0 || color.free_value >= 64)
		return;

	auto it = context.map_of_terrain_types.find(std::string(type));
	if (it != context.map_of_terrain_types.end()) {
		context.color_by_terrain_index[color.free_value] = it->second.color;
		context.modifier_by_terrain_index[color.free_value] = it->second.id;
	}
}

void make_terrain_modifier(std::string_view name, token_generator &gen, error_handler &err, scenario_building_context &context) {
	auto name_id = text::find_or_add_key(context.state, name);

	auto parsed_modifier = parse_terrain_modifier(gen, err, context);

	auto new_modifier = context.state.world.create_modifier();

	std::string gfx_name = std::string("GFX_terrainimg_") + std::string(name); // GFX_terrainimg_XXX
	if (auto it = context.gfx_context.map_of_names.find(gfx_name); it != context.gfx_context.map_of_names.end()) {
		context.state.province_definitions.terrain_to_gfx_map.insert_or_assign(new_modifier, it->second);
	}

	context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
	context.state.world.modifier_set_name(new_modifier, name_id);

	context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.peek_province_mod());
	context.state.world.modifier_set_national_values(new_modifier, parsed_modifier.peek_national_mod());

	context.map_of_modifiers.insert_or_assign(std::string(name), new_modifier);
	context.map_of_terrain_types.insert_or_assign(std::string(name), terrain_type{new_modifier, parsed_modifier.color.value});
}

void make_state_definition(std::string_view name, token_generator &gen, error_handler &err, scenario_building_context &context) {
	auto name_id = text::find_or_add_key(context.state, name);
	auto state_id = context.state.world.create_state_definition();

	context.map_of_state_names.insert_or_assign(std::string(name), state_id);
	context.state.world.state_definition_set_name(state_id, name_id);

	state_def_building_context new_context{context, state_id};

	parsers::parse_state_definition(gen, err, new_context);
}

void make_continent_definition(std::string_view name, token_generator &gen, error_handler &err, scenario_building_context &context) {
	auto name_id = text::find_or_add_key(context.state, name);
	auto new_modifier = context.state.world.create_modifier();

	context.map_of_modifiers.insert_or_assign(std::string(name), new_modifier);

	continent_building_context new_context{context, new_modifier};
	context.state.world.modifier_set_name(new_modifier, name_id);

	auto continent = parse_continent_definition(gen, err, new_context);

	context.state.world.modifier_set_icon(new_modifier, uint8_t(continent.icon_index));
	context.state.world.modifier_set_province_values(new_modifier, continent.peek_province_mod());
	context.state.world.modifier_set_national_values(new_modifier, continent.peek_national_mod());

	/* [...] "The game has special logic concerning which nations automatically count as
	   interested in a crisis that references the existing continents" [...] */
	if (is_fixed_token_ci(name.data(), name.data() + name.length(), "europe")) {
		context.state.province_definitions.europe = new_modifier;
	} else if (is_fixed_token_ci(name.data(), name.data() + name.length(), "asia")) {
		context.state.province_definitions.asia = new_modifier;
	} else if (is_fixed_token_ci(name.data(), name.data() + name.length(), "africa")) {
		context.state.province_definitions.africa = new_modifier;
	} else if (is_fixed_token_ci(name.data(), name.data() + name.length(), "north_america")) {
		context.state.province_definitions.north_america = new_modifier;
	} else if (is_fixed_token_ci(name.data(), name.data() + name.length(), "south_america")) {
		context.state.province_definitions.south_america = new_modifier;
	} else if (is_fixed_token_ci(name.data(), name.data() + name.length(), "oceania")) {
		context.state.province_definitions.oceania = new_modifier;
	}
}

void make_climate_definition(std::string_view name, token_generator &gen, error_handler &err, scenario_building_context &context) {
	auto name_id = text::find_or_add_key(context.state, name);

	auto new_modifier = [&]() {
		if (auto it = context.map_of_modifiers.find(std::string(name)); it != context.map_of_modifiers.end())
			return it->second;

		auto new_id = context.state.world.create_modifier();
		context.map_of_modifiers.insert_or_assign(std::string(name), new_id);
		context.state.world.modifier_set_name(new_id, name_id);
		return new_id;
	}();

	climate_building_context new_context{context, new_modifier};
	auto climate = parse_climate_definition(gen, err, new_context);

	if (climate.icon_index != 0)
		context.state.world.modifier_set_icon(new_modifier, uint8_t(climate.icon_index));
	if (climate.next_to_add_p != 0) {
		context.state.world.modifier_set_province_values(new_modifier, climate.peek_province_mod());
	}
	if (climate.next_to_add_n != 0) {
		context.state.world.modifier_set_national_values(new_modifier, climate.peek_national_mod());
	}
}

void enter_dated_block(std::string_view name, token_generator &gen, error_handler &err, province_file_context &context) {
	auto ymd = parse_date(name, 0, err);
	auto block_date = sys::date(ymd, context.outer_context.state.start_date);
	if (block_date.to_raw_value() > 1) { // is after the start date
		gen.discard_group();
	} else {
		parse_province_history_file(gen, err, context);
	}
}

dcon::nation_id prov_parse_force_tag_owner(dcon::national_identity_id tag, dcon::data_container &world) {
	auto fat_tag = fatten(world, tag);
	auto tag_holder = fat_tag.get_nation_from_identity_holder();
	if (tag_holder)
		return tag_holder;

	tag_holder = world.create_nation();
	world.force_create_identity_holder(tag_holder, tag);

	return tag_holder;
}

void province_history_file::life_rating(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	context.outer_context.state.world.province_set_life_rating(context.id, uint8_t(value));
}

void province_history_file::fort(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	context.outer_context.state.world.province_set_fort_level(context.id, uint8_t(value));
}

void province_history_file::naval_base(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	context.outer_context.state.world.province_set_naval_base_level(context.id, uint8_t(value));
}

void province_history_file::railroad(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	context.outer_context.state.world.province_set_railroad_level(context.id, uint8_t(value));
}

void province_history_file::colony(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	context.outer_context.state.world.province_set_is_colonial(context.id, value != 0);
}

void province_history_file::trade_goods(association_type, std::string_view text, error_handler &err, int32_t line, province_file_context &context) {
	if (auto it = context.outer_context.map_of_commodity_names.find(std::string(text)); it != context.outer_context.map_of_commodity_names.end()) {
		context.outer_context.state.world.province_set_rgo(context.id, it->second);
	} else {
		err.accumulated_errors += std::string(text) + " is not a valid commodity name (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void province_history_file::owner(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	if (auto it = context.outer_context.map_of_ident_names.find(value); it != context.outer_context.map_of_ident_names.end()) {
		auto holder = prov_parse_force_tag_owner(it->second, context.outer_context.state.world);
		context.outer_context.state.world.force_create_province_ownership(context.id, holder);
	} else {
		err.accumulated_errors += "Invalid tag (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}
void province_history_file::controller(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	if (auto it = context.outer_context.map_of_ident_names.find(value); it != context.outer_context.map_of_ident_names.end()) {
		auto holder = prov_parse_force_tag_owner(it->second, context.outer_context.state.world);
		context.outer_context.state.world.force_create_province_control(context.id, holder);
	} else {
		err.accumulated_errors += "Invalid tag (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void province_history_file::terrain(association_type, std::string_view text, error_handler &err, int32_t line, province_file_context &context) {
	if (auto it = context.outer_context.map_of_terrain_types.find(std::string(text)); it != context.outer_context.map_of_terrain_types.end()) {
		context.outer_context.state.world.province_set_terrain(context.id, it->second.id);
	} else {
		err.accumulated_errors += std::string(text) + " is not a valid commodity name (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void province_history_file::add_core(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	if (auto it = context.outer_context.map_of_ident_names.find(value); it != context.outer_context.map_of_ident_names.end()) {
		context.outer_context.state.world.try_create_core(context.id, it->second);
	} else {
		err.accumulated_errors += "Invalid tag (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void province_history_file::remove_core(association_type, uint32_t value, error_handler &err, int32_t line, province_file_context &context) {
	if (auto it = context.outer_context.map_of_ident_names.find(value); it != context.outer_context.map_of_ident_names.end()) {
		auto core = context.outer_context.state.world.get_core_by_prov_tag_key(context.id, it->second);
		context.outer_context.state.world.delete_core(core);
	} else {
		err.accumulated_errors += "Invalid tag (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
}

void province_history_file::party_loyalty(pv_party_loyalty const &value, error_handler &err, int32_t line, province_file_context &context) {
	if (value.id) {
		context.outer_context.state.world.province_set_party_loyalty(context.id, value.id, float(value.loyalty_value) / 100.0f);
	}
}

void province_history_file::state_building(pv_state_building const &value, error_handler &err, int32_t line, province_file_context &context) {
	if (value.id) {
		auto new_fac = context.outer_context.state.world.create_factory();
		context.outer_context.state.world.factory_set_building_type(new_fac, value.id);
		context.outer_context.state.world.factory_set_level(new_fac, uint8_t(value.level));
		context.outer_context.state.world.force_create_factory_location(new_fac, context.id);
	}
}

void province_history_file::is_slave(association_type, bool value, error_handler &err, int32_t line, province_file_context &context) {
	context.outer_context.state.world.province_set_is_slave(context.id, value);
}

void make_pop_province_list(std::string_view name, token_generator &gen, error_handler &err, scenario_building_context &context) {
	auto province_int = parse_int(name, 0, err);
	if (province_int < 0 || size_t(province_int) >= context.original_id_to_prov_id_map.size()) {
		err.accumulated_errors += "Province id " + std::string(name) + " is invalid (" + err.file_name + ")\n";
		gen.discard_group();
	} else {
		auto province_id = context.original_id_to_prov_id_map[province_int];
		pop_history_province_context new_context{context, province_id};
		parse_pop_province_list(gen, err, new_context);
	}
}
} // namespace parsers
