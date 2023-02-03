#include "parsers_declarations.hpp"
#include <algorithm>
#include <iterator>
#include "container_types.hpp"

namespace parsers {

void default_map_file::finish(scenario_building_context& context) {
	auto first_sea = std::stable_partition(context.prov_id_to_original_id_map.begin(), context.prov_id_to_original_id_map.end(),
		[](province_data const& a) { return !(a.is_sea); });

	for(size_t i = context.prov_id_to_original_id_map.size(); i-- > 0; ) {
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


void read_map_colors(char const* start, char const* end, error_handler& err, scenario_building_context& context) {
	char const* cpos = parsers::csv_advance_to_next_line(start, end); // first line is always useless
	while(cpos < end) {
		cpos = parsers::parse_fixed_amount_csv_values<4>(cpos, end, ';',
			[&](std::string_view const* values) {
				auto first_text = parsers::remove_surrounding_whitespace(values[0]);
				if(first_text.length() > 0) {
					auto first_value = parsers::parse_int(first_text, 0, err);
					if(first_value == 0) {
						// dead line
					} else if(size_t(first_value) >= context.original_id_to_prov_id_map.size()) {
						err.accumulated_errors += "Province id " + std::to_string(first_value) + " is too large (" + err.file_name + ")\n";
					} else {
						auto province_id = context.original_id_to_prov_id_map[first_value];
						auto color_value = sys::pack_color(
							parsers::parse_int(parsers::remove_surrounding_whitespace(values[1]), 0, err),
							parsers::parse_int(parsers::remove_surrounding_whitespace(values[2]), 0, err),
							parsers::parse_int(parsers::remove_surrounding_whitespace(values[3]), 0, err)
						);
						context.map_color_to_province_id.insert_or_assign(color_value, province_id);
					}
				}
			});
	}
}


void palette_definition::finish(scenario_building_context& context) {
	if(color.free_value < 0 || color.free_value >= 64)
		return;

	auto it = context.map_of_terrain_types.find(std::string(type));
	if(it != context.map_of_terrain_types.end()) {
		context.state.province_definitions.color_by_terrain_index[color.free_value] = it->second.color;
		context.state.province_definitions.modifier_by_terrain_index[color.free_value] = it->second.id;
	}
}

void make_terrain_modifier(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto name_id = text::find_or_add_key(context.state, name);

	auto parsed_modifier = parse_terrain_modifier(gen, err, context);

	auto new_modifier = context.state.world.create_modifier();

	context.state.world.modifier_set_icon(new_modifier, uint8_t(parsed_modifier.icon_index));
	context.state.world.modifier_set_name(new_modifier, name_id);

	parsed_modifier.convert_to_province_mod();
	context.state.world.modifier_set_province_values(new_modifier, parsed_modifier.constructed_definition);

	context.map_of_modifiers.insert_or_assign(std::string(name), new_modifier);
	context.map_of_terrain_types.insert_or_assign(std::string(name), terrain_type{ new_modifier, parsed_modifier.color.value });
}

void make_state_definition(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto name_id = text::find_or_add_key(context.state, name);
	auto state_id = context.state.world.create_state_definition();

	context.state.world.state_definition_set_name(state_id, name_id);

	state_def_building_context new_context{ context, state_id };

	parsers::parse_state_definition(gen, err, new_context);
}

}
