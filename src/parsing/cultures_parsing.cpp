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

}
