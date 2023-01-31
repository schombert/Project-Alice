#include "parsers_declarations.hpp"
#include "text.hpp"

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

void building_file::result(std::string_view name, building_definition&& res, error_handler& err, int32_t line, scenario_building_context& context) {
	res.goods_cost.data.safe_get(dcon::commodity_id(0)) = float(res.cost);
	switch(res.stored_type) {
		case building_type::factory:
		{
			auto factory_id = context.state.world.create_factory_type();
			context.state.world.factory_type_set_name(factory_id, text::find_or_add_key(context.state, name));
			context.state.world.factory_type_set_construction_time(factory_id, int16_t(res.time));
			context.state.world.factory_type_set_is_available_from_start(factory_id, res.default_enabled);
			for(uint32_t i = context.state.world.commodity_size(); i-- > 0; ) {
				dcon::commodity_id cid = dcon::commodity_id(dcon::commodity_id::value_base_t(i));
				context.state.world.factory_type_set_construction_costs(factory_id, cid, res.goods_cost.data[cid]);
			}
			if(res.production_type.length() > 0) {
				context.map_of_production_types.insert_or_assign(std::string(res.production_type), factory_id);
			}
		}
			break;
		case building_type::naval_base:
			for(uint32_t i = 0; i < 8 && i < res.colonial_points.data.size(); ++i)
				context.state.economy.naval_base_definition.colonial_points[i] = res.colonial_points.data[i];
			context.state.economy.naval_base_definition.colonial_range = res.colonial_range;
			context.state.economy.naval_base_definition.cost = std::move(res.goods_cost.data);
			context.state.economy.naval_base_definition.max_level = res.max_level;
			context.state.economy.naval_base_definition.naval_capacity = res.naval_capacity;
			context.state.economy.naval_base_definition.time = res.time;
			context.state.economy.naval_base_definition.name = text::find_or_add_key(context.state, name);
			if(res.next_to_add != 0) {
				res.convert_to_province_mod();
				context.state.economy.naval_base_definition.province_modifier = context.state.world.create_modifier();
				context.state.world.modifier_set_province_values(context.state.economy.naval_base_definition.province_modifier, res.constructed_definition);
				context.state.world.modifier_set_icon(context.state.economy.naval_base_definition.province_modifier, uint8_t(res.icon_index));
			}
			break;
		case building_type::fort:
			context.state.economy.fort_definition.cost = std::move(res.goods_cost.data);
			context.state.economy.fort_definition.max_level = res.max_level;
			context.state.economy.fort_definition.time = res.time;
			context.state.economy.fort_definition.name = text::find_or_add_key(context.state, name);
			if(res.next_to_add != 0) {
				res.convert_to_province_mod();
				context.state.economy.fort_definition.province_modifier = context.state.world.create_modifier();
				context.state.world.modifier_set_province_values(context.state.economy.fort_definition.province_modifier, res.constructed_definition);
				context.state.world.modifier_set_icon(context.state.economy.fort_definition.province_modifier, uint8_t(res.icon_index));
			}
			break;
		case building_type::railroad:
			context.state.economy.railroad_definition.cost = std::move(res.goods_cost.data);
			context.state.economy.railroad_definition.infrastructure = res.infrastructure;
			context.state.economy.railroad_definition.max_level = res.max_level;
			context.state.economy.railroad_definition.time = res.time;
			context.state.economy.railroad_definition.name = text::find_or_add_key(context.state, name);
			if(res.next_to_add != 0) {
				res.convert_to_province_mod();
				context.state.economy.railroad_definition.province_modifier = context.state.world.create_modifier();
				context.state.world.modifier_set_province_values(context.state.economy.railroad_definition.province_modifier, res.constructed_definition);
				context.state.world.modifier_set_icon(context.state.economy.railroad_definition.province_modifier, uint8_t(res.icon_index));
			}
			break;
	}
}

}
