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
	good_context new_context{new_id, context.outer_context};
	parse_good(gen, err, new_context);
}
void make_goods_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	if(name == "military_goods") {
		good_group_context new_context{sys::commodity_group::military_goods, context};
		parse_goods_group(gen, err, new_context);
	} else if(name == "raw_material_goods") {
		good_group_context new_context{sys::commodity_group::raw_material_goods, context};
		parse_goods_group(gen, err, new_context);
	} else if(name == "industrial_goods") {
		good_group_context new_context{sys::commodity_group::industrial_goods, context};
		parse_goods_group(gen, err, new_context);
	} else if(name == "consumer_goods") {
		good_group_context new_context{sys::commodity_group::consumer_goods, context};
		parse_goods_group(gen, err, new_context);
	} else {
		err.accumulated_errors += "Unknown goods category " + std::string(name) + " found in " + err.file_name + "\n";
		good_group_context new_context{sys::commodity_group::military_goods, context};
		parse_goods_group(gen, err, new_context);
	}
}

void building_file::result(std::string_view name, building_definition&& res, error_handler& err, int32_t line,
	scenario_building_context& context) {
	res.goods_cost.data.safe_get(dcon::commodity_id(0)) = float(res.cost);
	switch(res.stored_type) {
	case building_type::factory: {
		auto factory_id = context.state.world.create_factory_type();
		context.map_of_factory_names.insert_or_assign(std::string(name), factory_id);

		auto desc_id = text::find_or_add_key(context.state, std::string(name) + "_desc");

		context.state.world.factory_type_set_name(factory_id, text::find_or_add_key(context.state, name));
		context.state.world.factory_type_set_description(factory_id, desc_id);
		context.state.world.factory_type_set_construction_time(factory_id, int16_t(res.time));
		context.state.world.factory_type_set_is_available_from_start(factory_id, res.default_enabled);
		/*for(uint32_t i = context.state.world.commodity_size(); i-- > 0; ) {
			dcon::commodity_id cid = dcon::commodity_id(dcon::commodity_id::value_base_t(i));
			context.state.world.factory_type_set_construction_costs(factory_id, cid, res.goods_cost.data[cid]);
		}*/
		uint32_t added = 0;
		auto& cc = context.state.world.factory_type_get_construction_costs(factory_id);
		context.state.world.for_each_commodity([&](dcon::commodity_id id) {
			auto amount = res.goods_cost.data.safe_get(id);
			if(amount > 0) {
				if(added >= economy::commodity_set::set_size) {
					err.accumulated_errors +=
						"Too many factory cost goods in " + std::string(name) + " (" + err.file_name + ")\n";
				} else {
					cc.commodity_type[added] = id;
					cc.commodity_amounts[added] = amount;
					++added;
				}
			}
		});
		if(res.production_type.length() > 0) {
			context.map_of_production_types.insert_or_assign(std::string(res.production_type), factory_id);
		}
	} break;
	case building_type::naval_base:
		for(uint32_t i = 0; i < 8 && i < res.colonial_points.data.size(); ++i)
			context.state.economy_definitions.naval_base_definition.colonial_points[i] = res.colonial_points.data[i];
		context.state.economy_definitions.naval_base_definition.colonial_range = res.colonial_range;
		{
			uint32_t added = 0;
			context.state.world.for_each_commodity([&](dcon::commodity_id id) {
				auto amount = res.goods_cost.data.safe_get(id);
				if(amount > 0) {
					if(added >= economy::commodity_set::set_size) {
						err.accumulated_errors +=
							"Too many naval_base cost goods in " + std::string(name) + " (" + err.file_name + ")\n";
					} else {
						context.state.economy_definitions.naval_base_definition.cost.commodity_type[added] = id;
						context.state.economy_definitions.naval_base_definition.cost.commodity_amounts[added] = amount;
						++added;
					}
				}
			});
		}
		context.state.economy_definitions.naval_base_definition.max_level = res.max_level;
		context.state.economy_definitions.naval_base_definition.naval_capacity = res.naval_capacity;
		context.state.economy_definitions.naval_base_definition.time = res.time;
		context.state.economy_definitions.naval_base_definition.name = text::find_or_add_key(context.state, name);
		if(res.next_to_add_p != 0) {
			context.state.economy_definitions.naval_base_definition.province_modifier = context.state.world.create_modifier();
			context.state.world.modifier_set_province_values(
				context.state.economy_definitions.naval_base_definition.province_modifier, res.peek_province_mod());
			context.state.world.modifier_set_national_values(
				context.state.economy_definitions.naval_base_definition.province_modifier, res.peek_national_mod());
			context.state.world.modifier_set_icon(context.state.economy_definitions.naval_base_definition.province_modifier,
				uint8_t(res.icon_index));
			context.state.world.modifier_set_name(context.state.economy_definitions.naval_base_definition.province_modifier,
				context.state.economy_definitions.naval_base_definition.name);
		}
		break;
	case building_type::fort: {
		uint32_t added = 0;
		context.state.world.for_each_commodity([&](dcon::commodity_id id) {
			auto amount = res.goods_cost.data.safe_get(id);
			if(amount > 0) {
				if(added >= economy::commodity_set::set_size) {
					err.accumulated_errors += "Too many fort cost goods in " + std::string(name) + " (" + err.file_name + ")\n";
				} else {
					context.state.economy_definitions.fort_definition.cost.commodity_type[added] = id;
					context.state.economy_definitions.fort_definition.cost.commodity_amounts[added] = amount;
					++added;
				}
			}
		});
	}
		context.state.economy_definitions.fort_definition.max_level = res.max_level;
		context.state.economy_definitions.fort_definition.time = res.time;
		context.state.economy_definitions.fort_definition.name = text::find_or_add_key(context.state, name);
		if(res.next_to_add_p != 0) {
			context.state.economy_definitions.fort_definition.province_modifier = context.state.world.create_modifier();
			context.state.world.modifier_set_province_values(context.state.economy_definitions.fort_definition.province_modifier,
				res.peek_province_mod());
			context.state.world.modifier_set_national_values(
				context.state.economy_definitions.naval_base_definition.province_modifier, res.peek_national_mod());
			context.state.world.modifier_set_icon(context.state.economy_definitions.fort_definition.province_modifier,
				uint8_t(res.icon_index));
			context.state.world.modifier_set_name(context.state.economy_definitions.fort_definition.province_modifier,
				context.state.economy_definitions.fort_definition.name);
		}
		break;
	case building_type::railroad: {
		uint32_t added = 0;
		context.state.world.for_each_commodity([&](dcon::commodity_id id) {
			auto amount = res.goods_cost.data.safe_get(id);
			if(amount > 0) {
				if(added >= economy::commodity_set::set_size) {
					err.accumulated_errors +=
						"Too many railroad cost goods in " + std::string(name) + " (" + err.file_name + ")\n";
				} else {
					context.state.economy_definitions.railroad_definition.cost.commodity_type[added] = id;
					context.state.economy_definitions.railroad_definition.cost.commodity_amounts[added] = amount;
					++added;
				}
			}
		});
	}
		context.state.economy_definitions.railroad_definition.infrastructure = res.infrastructure;
		context.state.economy_definitions.railroad_definition.max_level = res.max_level;
		context.state.economy_definitions.railroad_definition.time = res.time;
		context.state.economy_definitions.railroad_definition.name = text::find_or_add_key(context.state, name);
		if(res.next_to_add_p != 0) {
			context.state.economy_definitions.railroad_definition.province_modifier = context.state.world.create_modifier();
			context.state.world.modifier_set_province_values(
				context.state.economy_definitions.railroad_definition.province_modifier, res.peek_province_mod());
			context.state.world.modifier_set_national_values(
				context.state.economy_definitions.naval_base_definition.province_modifier, res.peek_national_mod());
			context.state.world.modifier_set_icon(context.state.economy_definitions.railroad_definition.province_modifier,
				uint8_t(res.icon_index));
			context.state.world.modifier_set_name(context.state.economy_definitions.railroad_definition.province_modifier,
				context.state.economy_definitions.railroad_definition.name);
		}
		break;
	}
}

dcon::trigger_key make_production_bonus_trigger(token_generator& gen, error_handler& err, production_context& context) {
	trigger_building_context t_context{context.outer_context, trigger::slot_contents::state, trigger::slot_contents::nation,
		trigger::slot_contents::empty};
	return make_trigger(gen, err, t_context);
}

void make_production_type(std::string_view name, token_generator& gen, error_handler& err, production_context& context) {
	auto pt = parse_production_type(gen, err, context);

	if(!bool(pt.output_goods_)) { // must be a template
		if(pt.type_ == production_type_enum::factory && !context.found_worker_types && pt.employees.employees.size() >= 2) {
			context.outer_context.state.culture_definitions.primary_factory_worker = pt.employees.employees[0].type;
			context.outer_context.state.culture_definitions.secondary_factory_worker = pt.employees.employees[1].type;
			context.outer_context.state.economy_definitions.craftsmen_fraction = pt.employees.employees[0].amount;
			context.found_worker_types = true;
		}
		context.templates.insert_or_assign(std::string(name), std::move(pt));
		if(pt.type_ == production_type_enum::rgo && bool(pt.owner.type)) {
			context.outer_context.state.culture_definitions.aristocrat = pt.owner.type;
		}
	} else if(pt.type_ == production_type_enum::rgo) {
		context.outer_context.state.world.commodity_set_is_mine(pt.output_goods_, pt.mine);
		context.outer_context.state.world.commodity_set_rgo_amount(pt.output_goods_, pt.value);
		context.outer_context.state.world.commodity_set_rgo_workforce(pt.output_goods_, pt.workforce);
	} else if(pt.type_ == production_type_enum::artisan) {
		economy::commodity_set cset;
		uint32_t added = 0;
		context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id id) {
			auto amount = pt.input_goods.data.safe_get(id);
			if(amount > 0) {
				if(added >= economy::commodity_set::set_size) {
					err.accumulated_errors +=
						"Too many artisan input goods in" + std::string(name) + " (" + err.file_name + ")\n";
				} else {
					cset.commodity_type[added] = id;
					cset.commodity_amounts[added] = amount;
					++added;
				}
			}
		});
		context.outer_context.state.world.commodity_set_artisan_inputs(pt.output_goods_, cset);
		context.outer_context.state.world.commodity_set_artisan_output_amount(pt.output_goods_, pt.value);
	} else if(pt.type_ == production_type_enum::factory) {
		if(auto it = context.outer_context.map_of_production_types.find(std::string(name));
			it != context.outer_context.map_of_production_types.end()) {
			auto factory_handle = fatten(context.outer_context.state.world, it->second);

			economy::commodity_set cset;
			uint32_t added = 0;
			context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id id) {
				auto amount = pt.input_goods.data.safe_get(id);
				if(amount > 0) {
					if(added >= economy::commodity_set::set_size) {
						err.accumulated_errors +=
							"Too many factory input goods in " + std::string(name) + " (" + err.file_name + ")\n";
					} else {
						cset.commodity_type[added] = id;
						cset.commodity_amounts[added] = amount;
						++added;
					}
				}
			});

			economy::small_commodity_set sm_cset;
			uint32_t sm_added = 0;
			context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id id) {
				auto amount = pt.efficiency.data.safe_get(id);
				if(amount > 0) {
					if(sm_added >= economy::small_commodity_set::set_size) {
						err.accumulated_errors +=
							"Too many factory efficiency goods in " + std::string(name) + " (" + err.file_name + ")\n";
					} else {
						sm_cset.commodity_type[sm_added] = id;
						sm_cset.commodity_amounts[sm_added] = amount;
						++sm_added;
					}
				}
			});

			factory_handle.set_inputs(cset);
			factory_handle.set_efficiency_inputs(sm_cset);
			factory_handle.set_output(pt.output_goods_);
			factory_handle.set_output_amount(pt.value);
			factory_handle.set_is_coastal(pt.is_coastal);
			factory_handle.set_base_workforce(pt.workforce);

			if(pt.bonuses.size() >= 1) {
				factory_handle.set_bonus_1_amount(pt.bonuses[0].value);
				factory_handle.set_bonus_1_trigger(pt.bonuses[0].trigger);
			}
			if(pt.bonuses.size() >= 2) {
				factory_handle.set_bonus_2_amount(pt.bonuses[1].value);
				factory_handle.set_bonus_2_trigger(pt.bonuses[1].trigger);
			}
			if(pt.bonuses.size() >= 3) {
				factory_handle.set_bonus_3_amount(pt.bonuses[2].value);
				factory_handle.set_bonus_3_trigger(pt.bonuses[2].trigger);
			}
			if(pt.bonuses.size() >= 4) {
				err.accumulated_errors += "Too many factory bonuses for " + std::string(name) + " (" + err.file_name + ")\n";
			}
		} else {
			err.accumulated_warnings += "Unused factory production type: " + std::string(name) + "\n";
		}
	}
}

commodity_array make_prod_commodity_array(token_generator& gen, error_handler& err, production_context& context) {
	return parse_commodity_array(gen, err, context.outer_context);
}

} // namespace parsers
