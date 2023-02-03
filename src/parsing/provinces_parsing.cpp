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

}
