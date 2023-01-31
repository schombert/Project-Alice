#include "parsers_declarations.hpp"

namespace parsers {

void make_building(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::building_id new_id = context.state.world.create_building();
	auto name_id = text::find_or_add_key(context.state, name);
	context.state.world.building_set_name(new_id, name_id);

	context.map_of_building_names.insert_or_assign(std::string(name), new_id);
	
	building_context new_context{ new_id, context };
	parse_building(gen, err, new_context);
}

void make_colonial_points_list(token_generator& gen, error_handler& err, building_context& context) {
	colonial_points_context new_context{ context.id, context.outer_context };
	parse_colonial_points_list(gen, err, new_context);
}
}
