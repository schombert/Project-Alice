#include "parsers_declarations.hpp"

namespace parsers {

void make_good(std::string_view name, token_generator& gen, error_handler& err, good_group_context& context) {
	dcon::commodity_id new_id = context.outer_context.state.world.create_commodity();
	auto name_id = text::find_or_add_key(context.outer_context.state, name);
	context.outer_context.state.world.commodity_set_name(new_id, name_id);
	context.outer_context.state.world.commodity_set_commodity_group(new_id, uint8_t(context.group));
	context.outer_context.state.world.commodity_set_is_available_from_start(new_id, true);

	context.outer_context.map_of_commodity_names.insert_or_assign(std::string(name), new_id);
	good_context new_context{ new_id, context.outer_context };
	parse_good(gen, err, new_context);
}
void make_goods_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	if(name == "military_goods") {
		good_group_context new_context{ sys::commodity_group::military_goods, context };
		parse_goods_group(gen, err, new_context);
	} else if(name == "raw_material_goods") {
		good_group_context new_context{ sys::commodity_group::raw_material_goods, context };
		parse_goods_group(gen, err, new_context);
	} else if(name == "industrial_goods") {
		good_group_context new_context{ sys::commodity_group::industrial_goods, context };
		parse_goods_group(gen, err, new_context);
	} else if(name == "consumer_goods") {
		good_group_context new_context{ sys::commodity_group::consumer_goods, context };
		parse_goods_group(gen, err, new_context);
	} else {
		err.accumulated_errors += "Unknown goods category " + std::string(name) + " found in " + err.file_name + "\n";
		good_group_context new_context{ sys::commodity_group::military_goods, context };
		parse_goods_group(gen, err, new_context);
	}
}

}
