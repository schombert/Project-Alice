#include "construction.hpp"
#include "economy_stats.hpp"
#include "province_templates.hpp"
#include "text.hpp"
#include "money.hpp"
#include "province.hpp"
#include "economy.hpp"
#include "economy_templates.hpp"
#include "advanced_province_buildings.hpp"
#include "logistics.hpp"
#include "nations_templates.hpp"

namespace economy {

template<construction_completed completed, concepts::military_construction_type mil_construction_type>
void delete_unit_construction(sys::state& state, mil_construction_type c) {
	float cost_mult = construction_build_cost_multiplier(state, c);
	dcon::province_id location = construction_get_location(state, c);
	dcon::nation_id location_controller = construction_get_controller(state, c);
	auto state_inst = state.world.province_get_state_membership(location);
	auto market = state_inst.get_market_from_local_market();
	const auto& fufilled_goods = get_purchased_goods(state, c);
	const auto& base_build_cost = construction_get_base_build_cost(state, c);
	// Give back all of the purchased goods to the local stockpile if the construction was not completed. If it was completed, only give back the extra goods above what was needed
	if(location_controller) {
		for(uint32_t i = 0; i < base_build_cost.set_size; i++) {
			dcon::commodity_id com_id = base_build_cost.commodity_type[i];
			assert(base_build_cost.commodity_type[i] == fufilled_goods.commodity_type[i]);
			if(com_id) {
				float fufilled_amount = fufilled_goods.commodity_amounts[i];
				if constexpr(completed == construction_completed::no) {
					add_government_stockpile(state, location_controller, market, com_id, fufilled_amount);
				} else if constexpr(completed == construction_completed::yes) {
					float cost_amount = base_build_cost.commodity_amounts[i] * cost_mult;
					add_government_stockpile(state, location_controller, market, com_id, std::max(fufilled_amount - cost_amount, 0.0f));
				}
			}
			else {
				break;
			}
		}
	}
	else {
		for(uint32_t i = 0; i < base_build_cost.set_size; i++) {
			dcon::commodity_id com_id = base_build_cost.commodity_type[i];
			assert(base_build_cost.commodity_type[i] == fufilled_goods.commodity_type[i]);
			if(com_id) {
				float fufilled_amount = fufilled_goods.commodity_amounts[i];
				if constexpr(completed == construction_completed::no) {
					add_rebel_stockpile(state, market, com_id, fufilled_amount);
				} else if constexpr(completed == construction_completed::yes) {
					float cost_amount = base_build_cost.commodity_amounts[i] * cost_mult;
					add_rebel_stockpile(state, market, com_id, std::max(fufilled_amount - cost_amount, 0.0f));
				}
			} else {
				break;
			}
		}
	}
	if constexpr(std::is_same_v<mil_construction_type, dcon::province_land_construction_id>) {
		state.world.delete_province_land_construction(c);
	}
	else if constexpr(std::is_same_v<mil_construction_type, dcon::province_naval_construction_id>) {
		state.world.delete_province_naval_construction(c);
	}
}
template void delete_unit_construction<construction_completed::no>(sys::state& state, dcon::province_land_construction_id c);
template void delete_unit_construction<construction_completed::no>(sys::state& state, dcon::province_naval_construction_id c);
template void delete_unit_construction<construction_completed::yes>(sys::state& state, dcon::province_land_construction_id c);
template void delete_unit_construction<construction_completed::yes>(sys::state& state, dcon::province_naval_construction_id c);

void build_land_unit_construction_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	const dcon::province_land_construction_id conid
) {
	auto details = explain_land_unit_construction(state, conid);
	auto unit = state.world.province_land_construction_get_type(conid);
	const auto& goods = construction_get_actual_build_cost(state, conid);
	const auto& cgoods = get_purchased_goods(state, conid);

	{
		auto name = state.military_definitions.unit_base_definitions[unit].name;
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, name);
		text::close_layout_box(contents, box);
	}

	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(goods.commodity_type[i]) {
			auto box = text::open_layout_box(contents, 0);

			auto cid = goods.commodity_type[i];
			std::string padding = cid.index() < 10 ? "0" : "";
			std::string description = "@$" + padding + std::to_string(cid.index());
			text::add_unparsed_text_to_layout_box(state, contents, box, description);

			text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(goods.commodity_type[i]));
			text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ cgoods.commodity_amounts[i] });
			text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ goods.commodity_amounts[i] });
			text::close_layout_box(contents, box);
		}
	}
}

void build_naval_unit_construction_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	const dcon::province_naval_construction_id conid
) {
	auto details = explain_naval_unit_construction(state, conid);
	auto unit = state.world.province_naval_construction_get_type(conid);
	const auto& goods = construction_get_actual_build_cost(state, conid);
	const auto& cgoods = get_purchased_goods(state, conid);

	{
		auto name = state.military_definitions.unit_base_definitions[unit].name;
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, name);
		text::close_layout_box(contents, box);
	}

	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(goods.commodity_type[i]) {
			auto box = text::open_layout_box(contents, 0);

			auto cid = goods.commodity_type[i];
			std::string padding = cid.index() < 10 ? "0" : "";
			std::string description = "@$" + padding + std::to_string(cid.index());
			text::add_unparsed_text_to_layout_box(state, contents, box, description);

			text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(goods.commodity_type[i]));
			text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ cgoods.commodity_amounts[i] });
			text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ goods.commodity_amounts[i] });
			text::close_layout_box(contents, box);
		}
	}
}

economy::commodity_set calculate_factory_upgrade_goods_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::province_id pid,
	dcon::factory_type_id upgrade_target,
	bool is_pop_project
) {
	economy::commodity_set res{};
	auto& base_cost = state.world.factory_type_get_construction_costs(upgrade_target);
	float factory_mod = factory_build_cost_multiplier(state, n, pid, is_pop_project);


	for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
		if(base_cost.commodity_type[j]) {
			res.commodity_type[j] = base_cost.commodity_type[j];
			res.commodity_amounts[j] = base_cost.commodity_amounts[j] * factory_mod;
		} else {
			break;
		}
	}

	return res;
}

economy::commodity_set calculate_factory_refit_goods_cost(const sys::state& state, dcon::nation_id n, dcon::province_id pid, dcon::factory_type_id from, dcon::factory_type_id to) {
	auto& from_cost = state.world.factory_type_get_construction_costs(from);
	auto& to_cost = state.world.factory_type_get_construction_costs(to);

	float level = 1;

	for(auto f : state.world.province_get_factory_location(pid)) {
		if(f.get_factory().get_building_type() == from) {
			level = f.get_factory().get_size() / f.get_factory().get_building_type().get_base_workforce();
		}
	}


	// Refit cost = (to_cost) - (from_cost) + (0.1f * to_cost)
	float refit_mod = 1.0f + state.defines.alice_factory_refit_cost_modifier;

	economy::commodity_set res;

	// First take 110% of to_cost as a baseline
	if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
		for(uint32_t j = 0; j < commodity_set::set_size; ++j) {
			if(to_cost.commodity_type[j]) {
				res.commodity_type[j] = to_cost.commodity_type[j];
				res.commodity_amounts[j] = to_cost.commodity_amounts[j] * refit_mod * level;
			} else {
				break;
			}
		}
	}

	// Substract from_cost to represent refit discount
	if(!(n == state.local_player_nation && state.cheat_data.instant_industry)) {
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			if(!from_cost.commodity_type[i]) {
				break;
			}

			auto from_amount = from_cost.commodity_amounts[i] * level;
			auto from_commodity = from_cost.commodity_type[i];

			for(uint32_t j = 0; i < commodity_set::set_size; ++j) {
				if(!res.commodity_type[j]) {
					break;
				}

				if(res.commodity_type[j] == from_commodity) {
					res.commodity_amounts[j] = std::max(res.commodity_amounts[j] - from_amount, 0.f);
				}
			}
		}
	}

	return res;
}
float calculate_factory_refit_money_cost(sys::state& state, dcon::nation_id n, dcon::province_id pid, dcon::factory_type_id from, dcon::factory_type_id to) {
	auto goods_cost = calculate_factory_refit_goods_cost(state, n, pid, from, to);

	float admin_eff = state.world.province_get_control_ratio(pid);
	float admin_cost_factor = 2.0f - admin_eff;
	float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;

	auto total = 0.0f;
	for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
		if(goods_cost.commodity_type[i]) {
			total += economy::price(
				state,
				state.world.province_get_state_membership(pid),
				goods_cost.commodity_type[i]
			) * goods_cost.commodity_amounts[i]
				* factory_mod
				* admin_cost_factor;
		}
	}

	return total;
}

float global_province_construction_time_modifier(const sys::state& state) {
	return state.defines.alice_province_building_build_time_mult;
}
float global_land_construction_time_modifier(const sys::state& state) {
	return state.defines.alice_land_unit_build_time_mult;
}
float global_naval_construction_time_modifier(const sys::state& state) {
	return state.defines.alice_naval_unit_build_time_mult;
}

float global_factory_construction_time_modifier(const sys::state& state) {
	return state.defines.alice_factory_build_time_mult;
}

float location_build_cost_multiplier(const sys::state& state, dcon::province_id location, bool privately_owned) {
	float admin_eff = state.world.province_get_control_ratio(location);
	// make factories cheaper to make it a bit easier to get into industry and compensate for low control
	return (privately_owned ? 1.f : 2.0f - admin_eff) * 0.5f;
}

float factory_build_cost_multiplier(const sys::state& state, dcon::nation_id n, dcon::province_id location, bool privately_owned) {
	return
		location_build_cost_multiplier(state, location, privately_owned)
		* (std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost)) + 1.0f)
		* (std::max(0.1f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_owner_cost)));
}


template<concepts::construction_type con_type>
bool construction_is_privately_owned(const sys::state& state, con_type construction) {
	// military constructions are always considered not privately owned. Factories and province constructions depends on if it pop owned
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		return false; 
	} else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		return false;
	} else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		return state.world.factory_construction_get_is_pop_project(construction);
	} else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		return state.world.province_building_construction_get_is_pop_project(construction);
	}
}
template bool construction_is_privately_owned(const sys::state& state, dcon::province_land_construction_id construction);
template bool construction_is_privately_owned(const sys::state& state, dcon::province_naval_construction_id construction);
template bool construction_is_privately_owned(const sys::state& state, dcon::factory_construction_id construction);
template bool construction_is_privately_owned(const sys::state& state, dcon::province_building_construction_id construction);



float construction_build_cost_multiplier(const sys::state& state, dcon::province_land_construction_id con) {
	dcon::province_id location = construction_get_location(state, con);
	return location_build_cost_multiplier(state, location, true);
}
float construction_build_cost_multiplier(const sys::state& state, dcon::province_naval_construction_id con) {
	dcon::province_id location = construction_get_location(state, con);
	return location_build_cost_multiplier(state, location, true);
}
float construction_build_cost_multiplier(const sys::state& state, dcon::province_building_construction_id con) {
	dcon::province_id location = construction_get_location(state, con);
	return location_build_cost_multiplier(state, location, construction_is_privately_owned(state, con));
}

float construction_build_cost_multiplier(const sys::state& state, dcon::factory_construction_id con) {
	dcon::province_id location = construction_get_location(state, con);
	dcon::nation_id owner = construction_get_controller(state, con);
	return factory_build_cost_multiplier(state, owner, location, construction_is_privately_owned(state, con));
}


template<concepts::construction_type con_type>
const economy::commodity_set& construction_get_base_build_cost(const sys::state& state, con_type construction) {
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		dcon::unit_type_id type = state.world.province_land_construction_get_type(construction);
		return state.military_definitions.unit_base_definitions[type].build_cost;
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		dcon::unit_type_id type = state.world.province_naval_construction_get_type(construction);
		return state.military_definitions.unit_base_definitions[type].build_cost;
	}
	else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		dcon::factory_type_id type = state.world.factory_construction_get_type(construction);
		return state.world.factory_type_get_construction_costs(type);
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		uint8_t type = state.world.province_building_construction_get_type(construction);
		return state.economy_definitions.building_definitions[type].cost;
	}
}
template const economy::commodity_set& construction_get_base_build_cost(const sys::state& state, dcon::province_land_construction_id construction);
template const economy::commodity_set& construction_get_base_build_cost(const sys::state& state, dcon::province_naval_construction_id construction);
template const economy::commodity_set& construction_get_base_build_cost(const sys::state& state, dcon::factory_construction_id construction);
template const economy::commodity_set& construction_get_base_build_cost(const sys::state& state, dcon::province_building_construction_id construction);

template<concepts::construction_type con_type>
economy::commodity_set& construction_get_base_build_cost(sys::state& state, con_type construction) {
	return const_cast<economy::commodity_set&>( construction_get_base_build_cost(static_cast<const sys::state&>(state), construction));
}
template economy::commodity_set& construction_get_base_build_cost(sys::state& state, dcon::province_land_construction_id construction);
template economy::commodity_set& construction_get_base_build_cost(sys::state& state, dcon::province_naval_construction_id construction);
template economy::commodity_set& construction_get_base_build_cost(sys::state& state, dcon::factory_construction_id construction);
template economy::commodity_set& construction_get_base_build_cost(sys::state& state, dcon::province_building_construction_id construction);




template<concepts::construction_type con_type>
economy::commodity_set construction_get_actual_build_cost(const sys::state& state, con_type construction) {
	economy::commodity_set base_cost = [&]() -> economy::commodity_set {
		// Special case for factory constructions: If refitting, then a custom cost must be calculated
		if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
			if(state.world.factory_construction_get_refit_target(construction)) {
				dcon::nation_id owner = construction_get_controller(state, construction);
				dcon::province_id location = construction_get_location(state, construction);
				dcon::factory_type_id from = construction_get_type(state, construction);
				dcon::factory_type_id to = state.world.factory_construction_get_refit_target(construction);
				return calculate_factory_refit_goods_cost(state, owner, location, from, to );
			}
		}
		return construction_get_base_build_cost(state, construction);
	}();
	float cost_mult = construction_build_cost_multiplier(state, construction);
	base_cost.for_each_commodity([&](dcon::commodity_id, float& amount) {
		amount *= cost_mult;
	});
	return base_cost;
}
template economy::commodity_set construction_get_actual_build_cost(const sys::state& state, dcon::province_land_construction_id construction);
template economy::commodity_set construction_get_actual_build_cost(const sys::state& state, dcon::province_naval_construction_id construction);
template economy::commodity_set construction_get_actual_build_cost(const sys::state& state, dcon::factory_construction_id construction);
template economy::commodity_set construction_get_actual_build_cost(const sys::state& state, dcon::province_building_construction_id construction);


template<concepts::construction_type con_type>
const economy::commodity_set& get_purchased_goods(const sys::state& state, con_type construction) {
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_purchased_goods(construction);
	} else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_purchased_goods(construction);
	} else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		return state.world.factory_construction_get_purchased_goods(construction);
	} else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		return state.world.province_building_construction_get_purchased_goods(construction);
	}
}
template const economy::commodity_set& get_purchased_goods(const sys::state& state, dcon::province_land_construction_id construction);
template const economy::commodity_set& get_purchased_goods(const sys::state& state, dcon::province_naval_construction_id construction);
template const economy::commodity_set& get_purchased_goods(const sys::state& state, dcon::factory_construction_id construction);
template const economy::commodity_set& get_purchased_goods(const sys::state& state, dcon::province_building_construction_id construction);
template<concepts::construction_type con_type>
economy::commodity_set& get_purchased_goods(sys::state& state, con_type construction) {
	return const_cast<economy::commodity_set&>(get_purchased_goods(static_cast<const sys::state&>(state), construction));
}
template economy::commodity_set& get_purchased_goods(sys::state& state, dcon::province_land_construction_id construction);
template economy::commodity_set& get_purchased_goods(sys::state& state, dcon::province_naval_construction_id construction);
template economy::commodity_set& get_purchased_goods(sys::state& state, dcon::factory_construction_id construction);
template economy::commodity_set& get_purchased_goods(sys::state& state, dcon::province_building_construction_id construction);


dcon::internal::const_iterator_province_land_construction_foreach_land_construction_supply_route_as_construction_generator construction_get_supply_routes(const sys::state& state, dcon::province_land_construction_id con) {
	return state.world.province_land_construction_get_land_construction_supply_route(con);
}
dcon::internal::iterator_province_land_construction_foreach_land_construction_supply_route_as_construction_generator construction_get_supply_routes(sys::state& state, dcon::province_land_construction_id con) {
	return state.world.province_land_construction_get_land_construction_supply_route(con);
}
dcon::internal::const_iterator_province_naval_construction_foreach_naval_construction_supply_route_as_construction_generator construction_get_supply_routes(const sys::state& state, dcon::province_naval_construction_id con) {
	return state.world.province_naval_construction_get_naval_construction_supply_route(con);
}
dcon::internal::iterator_province_naval_construction_foreach_naval_construction_supply_route_as_construction_generator construction_get_supply_routes(sys::state& state, dcon::province_naval_construction_id con) {
	return state.world.province_naval_construction_get_naval_construction_supply_route(con);
}
dcon::internal::const_iterator_factory_construction_foreach_factory_construction_supply_route_as_construction_generator construction_get_supply_routes(const sys::state& state, dcon::factory_construction_id con) {
	return state.world.factory_construction_get_factory_construction_supply_route(con);
}
dcon::internal::iterator_factory_construction_foreach_factory_construction_supply_route_as_construction_generator construction_get_supply_routes(sys::state& state, dcon::factory_construction_id con) {
	return state.world.factory_construction_get_factory_construction_supply_route(con);
}
dcon::internal::const_iterator_province_building_construction_foreach_building_construction_supply_route_as_construction_generator construction_get_supply_routes(const sys::state& state, dcon::province_building_construction_id con) {
	return state.world.province_building_construction_get_building_construction_supply_route(con);
}
dcon::internal::iterator_province_building_construction_foreach_building_construction_supply_route_as_construction_generator construction_get_supply_routes(sys::state& state, dcon::province_building_construction_id con) {
	return state.world.province_building_construction_get_building_construction_supply_route(con);
}

template<concepts::construction_type con_type>
void construction_set_required_construction_base_cost(sys::state& state, con_type con, float val) {
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		state.world.province_land_construction_set_required_construction_base_cost(con, val);
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		state.world.province_naval_construction_set_required_construction_base_cost(con, val);
	}
	else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		state.world.factory_construction_set_required_construction_base_cost(con, val);
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		state.world.province_building_construction_set_required_construction_base_cost(con, val);
	}
	else {
		static_assert(false, "Unknown type");
	}
}
template void construction_set_required_construction_base_cost(sys::state& state, dcon::province_land_construction_id con, float val);
template void construction_set_required_construction_base_cost(sys::state& state, dcon::province_naval_construction_id con, float val);
template void construction_set_required_construction_base_cost(sys::state& state, dcon::factory_construction_id con, float val);
template void construction_set_required_construction_base_cost(sys::state& state, dcon::province_building_construction_id con, float val);

template<concepts::construction_type con_type>
float construction_get_required_construction_base_cost(const sys::state& state, con_type con) {
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_required_construction_base_cost(con);
	} else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_required_construction_base_cost(con);
	} else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		return state.world.factory_construction_get_required_construction_base_cost(con);
	} else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		return state.world.province_building_construction_get_required_construction_base_cost(con);
	} 
}
template float construction_get_required_construction_base_cost(const sys::state& state, dcon::province_land_construction_id con);
template float construction_get_required_construction_base_cost(const sys::state& state, dcon::province_naval_construction_id con);
template float construction_get_required_construction_base_cost(const sys::state& state, dcon::factory_construction_id con);
template float construction_get_required_construction_base_cost(const sys::state& state, dcon::province_building_construction_id con);


uint32_t land_unit_construction_time(
	const sys::state& state,
	dcon::unit_type_id utid,
	dcon::nation_id builder
) {
	return uint32_t(global_land_construction_time_modifier(state)
		* std::max(1, state.world.nation_get_unit_stats(builder, utid).build_time));
}

uint32_t naval_unit_construction_time(
	const sys::state& state,
	dcon::unit_type_id utid,
	dcon::nation_id builder
) {
	return uint32_t(global_naval_construction_time_modifier(state)
		* std::max(1, state.world.nation_get_unit_stats(builder, utid).build_time));
}

uint32_t province_building_construction_time(
	const sys::state& state,
	economy::province_building_type building_type
) {
	assert(0 <= int32_t(building_type) && int32_t(building_type) < int32_t(economy::max_building_types));
	return uint32_t(global_province_construction_time_modifier(state)
		* state.economy_definitions.building_definitions[int32_t(building_type)].time);
}

uint32_t factory_building_construction_time(
	const sys::state& state, dcon::factory_type_id ftid, bool is_upgrade
) {
	return uint32_t(global_factory_construction_time_modifier(state)
		* state.world.factory_type_get_construction_time(ftid)
		* (is_upgrade ? 0.5f : 1.0f));
}

template<concepts::construction_type con_type>
uint32_t construction_get_actual_construction_time(const sys::state& state, con_type con) {
	auto type = construction_get_type(state, con);
	dcon::nation_id owner = construction_get_controller(state, con);
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		return land_unit_construction_time(state, type, owner);
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		return naval_unit_construction_time(state, type, owner);
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		return province_building_construction_time(state, province_building_type(type));
	}
	else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		return factory_building_construction_time(state, type, state.world.factory_construction_get_is_upgrade(con));
	}
}


// govt construction demand needs to be added seperately as it is handled through national stockpiles
// it's registered as demand separately, do not add actual demand here
void register_government_construction_demand(sys::state& state, dcon::market_id s, dcon::commodity_id commodity_type, float amount) {
	auto& cur_demand = state.world.market_get_government_construction_demand(s, commodity_type);
	state.world.market_set_government_construction_demand(s, commodity_type, cur_demand + amount);
	assert(state.world.market_get_government_construction_demand(s, commodity_type) >= 0.f);
}

void reset_government_construction_demand(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_government_construction_demand(ids, cid, 0.0f);
		});
	}
}
void reset_private_construction_demand(sys::state& state) {
	uint32_t total_commodities = state.world.commodity_size();
	for(uint32_t i = 1; i < total_commodities; ++i) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
		state.world.execute_serial_over_market([&](auto ids) {
			state.world.market_set_private_construction_demand(ids, cid, 0.0f);
		});
	}
}

bool can_advance_construction(const sys::state& state, dcon::province_land_construction_id con ) {
	auto owner = construction_get_controller(state, con);
	auto location = construction_get_location(state, con);
	auto location_controller = state.world.province_get_nation_from_province_control(location);
	return owner && owner == location_controller;
}

bool can_advance_construction(const sys::state& state, dcon::province_naval_construction_id con) {
	auto construction = fatten(state.world, con);
	auto owner = construction_get_controller(state, con);
	auto location = construction_get_location(state, con);
	auto location_controller = state.world.province_get_nation_from_province_control(location);
	auto all_constructions = state.world.province_get_province_naval_construction(location);
	// Only the first naval construction in a province can be advanced at a time
	return owner && owner == location_controller && (*all_constructions.begin()) == con;
}
bool can_advance_construction(const sys::state& state, dcon::factory_construction_id con) {
	auto construction = fatten(state.world, con);
	auto owner = construction_get_controller(state, con);
	auto location = construction_get_location(state, con);
	auto location_controller = state.world.province_get_nation_from_province_control(location);
	// Can only advance the construction if you control the province it is being constructed in
	return owner && owner == location_controller;
}
bool can_advance_construction(const sys::state& state, dcon::province_building_construction_id con) {
	auto construction = fatten(state.world, con);
	auto owner = construction_get_controller(state, con);
	auto location = construction_get_location(state, con);
	auto location_controller = state.world.province_get_nation_from_province_control(location);
	// Can only advance the construction if you control the province it is being constructed in
	return owner && owner == location_controller;
}

unit_construction_data explain_land_unit_construction(
	sys::state& state,
	dcon::province_land_construction_id construction
) {
	auto province = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(construction));
	auto owner = state.world.province_get_nation_from_province_ownership(province);
	auto local_zone = state.world.province_get_state_membership(province);
	auto unit_type = state.world.province_land_construction_get_type(construction);
	unit_construction_data result = {
		.can_be_advanced = can_advance_construction(state, construction),
		.construction_time = land_unit_construction_time(state, unit_type, owner),
		.cost_multiplier = construction_build_cost_multiplier(state, construction),
		.owner = owner,
		.market = state.world.state_instance_get_market_from_local_market(local_zone),
		.province = province,
		.unit_type = unit_type
	};
	return result;
}


unit_construction_data explain_naval_unit_construction(
	sys::state& state,
	dcon::province_naval_construction_id construction
) {
	auto province = state.world.province_naval_construction_get_province(construction);
	auto owner = state.world.province_get_nation_from_province_ownership(province);
	auto local_zone = state.world.province_get_state_membership(province);
	auto unit_type = state.world.province_naval_construction_get_type(construction);
	unit_construction_data result = {
		.can_be_advanced = can_advance_construction(state, construction),
		.construction_time = naval_unit_construction_time(state, unit_type, owner),
		.cost_multiplier = construction_build_cost_multiplier(state, construction),
		.owner = owner,
		.market = state.world.state_instance_get_market_from_local_market(local_zone),
		.province = province,
		.unit_type = unit_type
	};
	return result;
}


struct province_building_construction_data {
	bool can_be_advanced;
	bool is_pop_project;
	bool is_upgrade;
	uint32_t construction_time;
	float cost_multiplier;
	dcon::nation_id owner;
	dcon::market_id market;
	dcon::province_id province;
	economy::province_building_type building_type;
};

province_building_construction_data explain_province_building_construction(
	sys::state& state,
	dcon::province_building_construction_id construction
) {
	auto owner = state.world.province_building_construction_get_nation(construction);
	auto province = state.world.province_building_construction_get_province(construction);
	auto local_zone = state.world.province_get_state_membership(province);
	auto raw_type = state.world.province_building_construction_get_type(construction);
	auto t = economy::province_building_type(raw_type);
	auto is_pop_project = state.world.province_building_construction_get_is_pop_project(construction);
	province_building_construction_data result = {
		.can_be_advanced = (owner && state.world.province_get_nation_from_province_control(province) == owner),
		.is_pop_project = is_pop_project,
		.is_upgrade = false,
		.construction_time = province_building_construction_time(state, t),
		.cost_multiplier = construction_build_cost_multiplier(state, construction),
		.owner = owner,
		.market = state.world.state_instance_get_market_from_local_market(local_zone),
		.province = province,
		.building_type = t,
	};
	return result;
}

//handles both private building constructions
void advance_private_province_building_construction(
	sys::state& state,
	dcon::province_building_construction_id construction
) {
	if(construction_is_privately_owned(state, construction)) {
		auto details = explain_province_building_construction(state, construction);
		assert(0 <= int32_t(details.building_type) && int32_t(details.building_type) < int32_t(economy::max_building_types));
		const auto& base_cost = state.economy_definitions.building_definitions[int32_t(details.building_type)].cost;
		assert(state.world.province_building_construction_is_valid(construction) && "Invalid write incoming!");
		auto& current_purchased = state.world.province_building_construction_get_purchased_goods(construction);

		// Rationale for not checking the building type:
		// Pop projects created for forts and naval bases should NOT happen in the first place, so checking against them
		// is a waste of resources
		// peter: i do not understand what the above comment means
		// but i guess it was an important piece of info during the ancient times

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid) break;
			auto current = current_purchased.commodity_amounts[i];
			auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
			if(current >= required)
				continue;
			auto amount = required / details.construction_time;
			auto source_private = state.world.market_get_private_construction_demand(details.market, base_cost.commodity_type[i]);
			auto delta = std::clamp(required / details.construction_time, 0.f, source_private);
			current_purchased.commodity_amounts[i] += delta;
			state.world.market_set_private_construction_demand(details.market, base_cost.commodity_type[i], source_private - delta);

		}
	}
}

// Internal function which takes the buffer as an out-param to smooth over accumulating a large amount of constructions
template<concepts::construction_type con_type>
void get_last_required_construction_need(const sys::state& state, con_type construction, tagged_vector<float, dcon::commodity_id>& vec_out) {

	if(construction_is_privately_owned(state, construction)) {
		return;
	}

	const economy::commodity_set& build_cost = construction_get_base_build_cost(state, construction);
	float required_base_cost = construction_get_required_construction_base_cost(state, construction);
	for(uint32_t i = 0; i < build_cost.set_size; ++i) {
		dcon::commodity_id base_com_id = build_cost.commodity_type[i];
		if(base_com_id) {
			vec_out[base_com_id] += build_cost.commodity_amounts[i] * required_base_cost;
		} else {
			break;
		}
	}
	
}

template<concepts::construction_type con_type>
tagged_vector<float, dcon::commodity_id> get_last_required_construction_need(const sys::state& state, con_type construction) {
	tagged_vector<float, dcon::commodity_id> result(state.world.commodity_size());
	get_last_required_construction_need(state, construction, result);
	return result;
}

template<concepts::construction_type con_type>
tagged_vector<float, dcon::commodity_id> nation_get_last_required_construction_need(const sys::state& state, dcon::nation_id nation) {
	tagged_vector<float, dcon::commodity_id> result(state.world.commodity_size());
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		state.world.nation_for_each_province_land_construction(nation, [&](dcon::province_land_construction_id con) {
			get_last_required_construction_need(state, con, result);
		});
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		state.world.nation_for_each_province_naval_construction(nation, [&](dcon::province_naval_construction_id con) {
			get_last_required_construction_need(state, con, result);
		});
	}
	else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		state.world.nation_for_each_factory_construction(nation, [&](dcon::factory_construction_id con) {
			get_last_required_construction_need(state, con, result);
		});
	}
	else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		state.world.nation_for_each_province_building_construction(nation, [&](dcon::province_building_construction_id con) {
			get_last_required_construction_need(state, con, result);
		});
	}
	return result;
}
template tagged_vector<float, dcon::commodity_id> nation_get_last_required_construction_need<dcon::province_land_construction_id>(const sys::state& state, dcon::nation_id nation);
template tagged_vector<float, dcon::commodity_id> nation_get_last_required_construction_need<dcon::province_naval_construction_id>(const sys::state& state, dcon::nation_id nation);
template tagged_vector<float, dcon::commodity_id> nation_get_last_required_construction_need<dcon::factory_construction_id>(const sys::state& state, dcon::nation_id nation);
template tagged_vector<float, dcon::commodity_id> nation_get_last_required_construction_need<dcon::province_building_construction_id>(const sys::state& state, dcon::nation_id nation);

// Internal function which takes the buffer as an out-param to smooth over accumulating a large amount of constructions
template<concepts::construction_type con_type>
void get_last_fufilled_construction_need(const sys::state& state, con_type construction, tagged_vector<float, dcon::commodity_id>& vec_out) {

	if(construction_is_privately_owned(state, construction)) {
		return;
	}

	auto routes = construction_get_supply_routes(state, construction);
	const economy::commodity_set& build_cost = construction_get_base_build_cost(state, construction);
	for(auto route : routes) {
		if(logistics::supply_route_is_active(state, route.id)) {
			float supply_loss = logistics::supply_route_get_supply_loss(state, route.id);
			const commodity_amounts& buffered_goods = route.get_buffered_goods();
			build_cost.for_each_valid_index([&](uint32_t idx) {
				dcon::commodity_id com_id = build_cost.commodity_type[idx];
				float com_supply_loss_mod = state.world.commodity_get_supply_loss_rate(com_id);;
				float loss_mult = logistics::supply_loss_to_loss_multiplier(supply_loss, com_supply_loss_mod);
				vec_out[com_id] += (buffered_goods[idx] * loss_mult * logistics::supply_route_get_throughput(state, route.id)); // take into account goods which will be lost to attrition and throughput

			});
		}
	};
}

template<concepts::construction_type con_type>
tagged_vector<float, dcon::commodity_id> get_last_fufilled_construction_need(const sys::state& state, con_type construction) {
	tagged_vector<float, dcon::commodity_id> result{ };
	get_last_fufilled_construction_need(state, construction, result);
	return result;
}

template<concepts::construction_type con_type>
tagged_vector<float, dcon::commodity_id> nation_get_last_fufilled_construction_need(const sys::state& state, dcon::nation_id nation) {
	tagged_vector<float, dcon::commodity_id> result(state.world.commodity_size());
	if constexpr(std::is_same_v<con_type, dcon::province_land_construction_id>) {
		state.world.nation_for_each_province_land_construction(nation, [&](dcon::province_land_construction_id con) {
			get_last_fufilled_construction_need(state, con, result);
		});
	} else if constexpr(std::is_same_v<con_type, dcon::province_naval_construction_id>) {
		state.world.nation_for_each_province_naval_construction(nation, [&](dcon::province_naval_construction_id con) {
			get_last_fufilled_construction_need(state, con, result);
		});
	} else if constexpr(std::is_same_v<con_type, dcon::factory_construction_id>) {
		state.world.nation_for_each_factory_construction(nation, [&](dcon::factory_construction_id con) {
			get_last_fufilled_construction_need(state, con, result);
		});
	} else if constexpr(std::is_same_v<con_type, dcon::province_building_construction_id>) {
		state.world.nation_for_each_province_building_construction(nation, [&](dcon::province_building_construction_id con) {
			get_last_fufilled_construction_need(state, con, result);
		});
	}
	return result;
}
template tagged_vector<float, dcon::commodity_id> nation_get_last_fufilled_construction_need<dcon::province_land_construction_id>(const sys::state& state, dcon::nation_id nation);
template tagged_vector<float, dcon::commodity_id> nation_get_last_fufilled_construction_need<dcon::province_naval_construction_id>(const sys::state& state, dcon::nation_id nation);
template tagged_vector<float, dcon::commodity_id> nation_get_last_fufilled_construction_need<dcon::factory_construction_id>(const sys::state& state, dcon::nation_id nation);
template tagged_vector<float, dcon::commodity_id> nation_get_last_fufilled_construction_need<dcon::province_building_construction_id>(const sys::state& state, dcon::nation_id nation);


//void populate_province_building_construction_demand(
//	sys::state& state,
//	dcon::province_building_construction_id construction,
//	float& budget,
//	float budget_limit
//) {
//	auto details = explain_province_building_construction(state, construction);
//	if(!details.can_be_advanced) return;
//	if(details.is_pop_project) return;
//
//	assert(0 <= int32_t(details.building_type) && int32_t(details.building_type) < int32_t(economy::max_building_types));
//	auto& base_cost = state.economy_definitions.building_definitions[int32_t(details.building_type)].cost;
//	auto& current_purchased = state.world.province_building_construction_get_purchased_goods(construction);
//
//	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
//		auto cid = base_cost.commodity_type[i];
//		if(!cid) break;
//		auto current = current_purchased.commodity_amounts[i];
//		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
//		if(current >= required) continue;
//		auto local_price = price(state, details.market, cid);
//		auto can_purchase_budget = std::min(budget_limit, budget) / (local_price + 0.001f);
//		auto can_purchase_construction = required / details.construction_time;
//		auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
//		auto satisfaction = state.world.market_get_actual_probability_to_buy(details.market, cid);
//		budget = std::max(0.f, budget - can_purchase * local_price * satisfaction);
//		register_construction_demand(state, details.market, cid, can_purchase);
//	}
//}

template<concepts::construction_type con_type>
float nation_average_construction_satisfaction_by_type(const sys::state& state, dcon::nation_id nation) {
	float total_goods_required = 0.0f;
	float total_goods_fufilled = 0.0f;


	tagged_vector<float, dcon::commodity_id> required_amounts = nation_get_last_required_construction_need<con_type>(state, nation);
	tagged_vector<float, dcon::commodity_id> fufilled_amounts = nation_get_last_fufilled_construction_need<con_type>(state, nation);

	for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
		total_goods_required += required_amounts[com_id];
		total_goods_fufilled += fufilled_amounts[com_id];
	});

	return (total_goods_required == 0.0f ? 1.0f : total_goods_fufilled / total_goods_required);

}
template float nation_average_construction_satisfaction_by_type<dcon::province_land_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_average_construction_satisfaction_by_type<dcon::province_naval_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_average_construction_satisfaction_by_type<dcon::factory_construction_id>(const sys::state& state, dcon::nation_id nation);
template float nation_average_construction_satisfaction_by_type<dcon::province_building_construction_id>(const sys::state& state, dcon::nation_id nation);

template<concepts::construction_type con_type>
float construction_progress(const sys::state& state, con_type c) {
	auto construction = fatten(state.world, c);
	uint32_t construction_time_needed = construction_get_actual_construction_time(state, c);

	uint32_t current_construction_days = construction.get_construction_days();
	assert(construction_time_needed > 0);
	float construction_days_progress = std::min(float(current_construction_days) / float(construction_time_needed), 1.0f);

	return construction_days_progress;
}

struct factory_construction_data {
	bool can_be_advanced;
	bool is_pop_project;
	bool is_upgrade;
	uint32_t construction_time;
	float cost_multiplier;
	dcon::nation_id owner;
	dcon::market_id market;
	dcon::province_id province;
	dcon::state_instance_id state_instance;
	dcon::factory_type_id building_type;
	dcon::factory_type_id refit_target;
};

factory_construction_data explain_factory_building_construction(
	sys::state& state,
	dcon::factory_construction_id construction
) {
	auto owner = state.world.factory_construction_get_nation(construction);
	auto province = state.world.factory_construction_get_province(construction);
	auto local_zone = state.world.province_get_state_membership(province);
	auto market = state.world.state_instance_get_market_from_local_market(local_zone);
	auto refit_target = state.world.factory_construction_get_refit_target(construction);
	auto building_type = state.world.factory_construction_get_type(construction);
	auto is_pop_project = state.world.factory_construction_get_is_pop_project(construction);
	auto is_upgrade = state.world.factory_construction_get_is_upgrade(construction);
	factory_construction_data result = {
		.can_be_advanced = (owner && state.world.province_get_nation_from_province_control(province) == owner),
		.is_pop_project = is_pop_project,
		.is_upgrade = is_upgrade,
		.construction_time = factory_building_construction_time(state, building_type, is_upgrade),
		.cost_multiplier = construction_build_cost_multiplier(state, construction),
		.owner = owner,
		.market = state.world.state_instance_get_market_from_local_market(local_zone),
		.province = province,
		.state_instance = local_zone,
		.building_type = building_type,
		.refit_target = refit_target
	};
	return result;
}


void factory_construction_tooltip(sys::state& state, text::columnar_layout& contents, dcon::factory_construction_id fcid) {
	auto fat_fcid = dcon::fatten(state.world, fcid);
	auto ftid = state.world.factory_construction_get_type(fcid);

	auto details = explain_factory_building_construction(state, fcid);
	auto base_cost =
		details.refit_target
		? calculate_factory_refit_goods_cost(
			state, details.owner, details.province, details.building_type, details.refit_target
		)
		: state.world.factory_type_get_construction_costs(details.building_type);
	auto& current_purchased = state.world.factory_construction_get_purchased_goods(fcid);

	float total = 0.0f;
	float purchased = 0.0f;

	float factory_mod = economy::factory_build_cost_multiplier(state, fat_fcid.get_nation(), fat_fcid.get_province(), fat_fcid.get_is_pop_project());
	float refit_discount = (fat_fcid.get_refit_target()) ? state.defines.alice_factory_refit_cost_modifier : 1.0f;
	auto market = state.world.state_instance_get_market_from_local_market(fat_fcid.get_province().get_state_membership());

	text::add_line(state, contents, state.world.factory_type_get_name(fat_fcid.get_type()));

	if(fat_fcid.get_is_pop_project()) {
		text::add_line(state, contents, "pop_project");
	} else {
		text::add_line(state, contents, "state_project");
	}

	text::add_line(state, contents, "alice_construction_cost");

	// List factory type construction costs
	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;

		auto commodity_price = state.world.market_get_price(market, cid);
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;

		total += required * commodity_price;
		purchased += std::min(current, required) * commodity_price;

		auto left = std::max(0.f, required - current);

		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::name, state.world.commodity_get_name(cid));
		text::add_to_substitution_map(m, text::variable_type::val, text::fp_currency{ commodity_price });
		text::add_to_substitution_map(m, text::variable_type::need, text::fp_four_places{ left });
		text::add_to_substitution_map(m, text::variable_type::cost, text::fp_currency{ commodity_price * left });
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, "alice_factory_input_item", m);
		text::close_layout_box(contents, box);
	}

	text::add_line_break_to_layout(state, contents);
	auto progress = total > 0.0f ? purchased / total : 0.0f;
	text::add_line(state, contents, "alice_factory_construction_explain_3", text::variable_type::x, text::fp_currency{ purchased });
	text::add_line(state, contents, "alice_factory_construction_explain_4", text::variable_type::x, text::fp_currency{ total });
	text::add_line(state, contents, "alice_factory_construction_explain_5", text::variable_type::x, text::fp_percentage{ progress });
};


//handles only private factory constructions
void advance_private_factory_construction(
	sys::state& state,
	dcon::factory_construction_id construction
) {
	if(construction_is_privately_owned(state, construction)) {
		auto details = explain_factory_building_construction(state, construction);
		auto base_cost =
			details.refit_target
			? calculate_factory_refit_goods_cost(
				state, details.owner, details.province, details.building_type, details.refit_target
			)
			: state.world.factory_type_get_construction_costs(details.building_type);
		assert(state.world.factory_construction_is_valid(construction) && "Invalid write incoming!");
		auto& current_purchased = state.world.factory_construction_get_purchased_goods(construction);

		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid) break;
			auto current = current_purchased.commodity_amounts[i];
			auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
			if(current >= required)	continue;
			auto source_private = state.world.market_get_private_construction_demand(details.market, base_cost.commodity_type[i]);
			auto delta = std::clamp(required / details.construction_time, 0.f, source_private);
			current_purchased.commodity_amounts[i] += delta;
			state.world.market_set_private_construction_demand(details.market, base_cost.commodity_type[i], source_private - delta);

		}
	}
}

tagged_vector<float, dcon::commodity_id> estimate_nation_construction_consumption(const sys::state& state, dcon::nation_id nation) {
	tagged_vector<float, dcon::commodity_id> consumption(state.world.commodity_size());

	nations::nation_for_each_construction(state, nation, [&](auto construction) {

		float construction_consumption = static_cast<float>(nations::get_nation_construction_consumption_setting_by_type<decltype(construction)>(state, nation)) / 100.0f;
		float construction_days = static_cast<float>(construction_get_actual_construction_time(state, construction));

		auto accumulate_func = [&](dcon::commodity_id com_id, float required_amount, float total_required) {
			float demand_amount = std::min(total_required / construction_days * construction_consumption, required_amount); // Tie the demand amount to add to the construction consumption setting of the nation. Low consumption setting -> we won't try to purchase as much
			consumption[com_id] += demand_amount;
		};

		accumulate_construction_good_requirements(state, construction, accumulate_func);

	});
	return consumption;
	
}

float estimate_construction_stockpile_spending(const sys::state& state, dcon::nation_id nation, float budget) {
	auto goods_consumption = estimate_nation_construction_consumption(state, nation);
	float total_expected_price = get_estimated_stockpile_total_purchase_price<price_estimation::capped_by_availability>(state, nation, goods_consumption);
	float can_afford_mult = (total_expected_price == 0 ? max_con_required_spend : std::min(budget / total_expected_price, max_con_required_spend));
	return total_expected_price * can_afford_mult;
}
tagged_vector<float, dcon::commodity_id> estimate_construction_stockpile_spending_by_commodity(const sys::state& state, dcon::nation_id nation, float budget) {
	auto goods_consumption = estimate_nation_construction_consumption(state, nation);
	float total_expected_price = get_estimated_stockpile_total_purchase_price<price_estimation::capped_by_availability>(state, nation, goods_consumption);
	auto total_expected_prices = get_estimated_stockpile_purchase_price_by_commodity<price_estimation::capped_by_availability>(state, nation, goods_consumption);
	float can_afford_mult = (total_expected_price == 0 ? max_con_required_spend : std::min(budget / total_expected_price, max_con_required_spend));
	for(auto& price : total_expected_prices) {
		price = price * can_afford_mult;
	}
	return total_expected_prices;
}





void populate_government_construction_consumption(sys::state& state) {

	// Nobody should be using this dcon buffer at this time, so we can re-use it.
	auto demand_buffer_set = [&]<concepts::any_dcon_id_type<dcon::nation_id> nation_type, concepts::regular_or_ve_value_type<float> float_type>(nation_type nation, dcon::commodity_id com_id, float_type val) {
		state.world.nation_set_commodity_float_buffer_2(nation, com_id, val);
	};
	auto demand_buffer_get = [&]<concepts::any_dcon_id_type<dcon::nation_id> nation_type>(nation_type nation, dcon::commodity_id com_id) {
		return state.world.nation_get_commodity_float_buffer_2(nation, com_id);
	};

	static auto current_budget = ve::vectorizable_buffer<float, dcon::nation_id>(uint32_t(1));
	static auto expected_total_price = ve::vectorizable_buffer<float, dcon::nation_id>(uint32_t(1));
	{
		static uint32_t old_count = 1;
		auto new_count = state.world.nation_size();
		if(new_count > old_count) {
			current_budget = state.world.nation_make_vectorizable_float_buffer();
			expected_total_price = state.world.nation_make_vectorizable_float_buffer();
			old_count = new_count;
		}
	}

	// Set nation budget values and reset buffer
	state.world.execute_serial_over_nation([&](auto ids) {
		auto base_budget = state.world.nation_get_last_base_budget(ids);
		auto construction_priority = ve::to_float(state.world.nation_get_construction_spending(ids)) / 100.f;
		current_budget.set(ids, ve::max(0.f, base_budget * construction_priority));
		expected_total_price.set(ids, 0.0f);
		state.world.for_each_commodity([&](dcon::commodity_id com_id) {
			demand_buffer_set(ids, com_id, ve::fp_vector{ 0.0f });
		});
	});
	// Accumulate total potential demand for government constructions. Also checks if the construction can be advanced before buffering the demand
	economy::for_each_construction(state, [&](auto con) {

		if(construction_is_privately_owned(state, con)) {
			return;
		}

		if(!can_advance_construction(state, con)) {
			return;
		}
		dcon::nation_id conc_owner = construction_get_controller(state, con);

		float construction_days = static_cast<float>(construction_get_actual_construction_time(state, con));
		float construction_consumption = static_cast<float>(nations::get_nation_construction_consumption_setting_by_type<decltype(con)>(state, conc_owner)) / 100.0f;


		auto accumulate_func = [&](dcon::commodity_id com_id, float required_amount, float total_required) {
			float demand_amount = std::min(total_required / construction_days * construction_consumption, required_amount); // Tie the demand amount to add to the construction consumption setting of the nation. Low consumption setting -> we won't try to purchase as much
			demand_buffer_set(conc_owner, com_id, demand_buffer_get(conc_owner, com_id) + demand_amount);
		};
		accumulate_construction_good_requirements(state, con, accumulate_func);
	});

	// Calculate the total expected price per nation, when taking into account demand weights in their controlled states.
	// We need to calculate this to be a modifier on demand later
	state.world.execute_serial_over_market([&](auto markets) {
		auto states = state.world.market_get_zone_from_local_market(markets);
		auto controllers = state.world.state_instance_get_nation_from_state_control(states);
		auto nations_valid = ve::apply([&](dcon::nation_id nation) {
			return state.world.nation_is_valid(nation);
		}, controllers);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id base_com_id) {
			auto nations_demand = demand_buffer_get(controllers, base_com_id);
			auto expected_stockpile_price = get_estimated_state_stockpile_purchase_price<price_estimation::theoretical_max>(state, markets, base_com_id, nations_demand);
			ve::apply([&](float expected_price, dcon::nation_id nation, bool valid) {
				if(valid) {
					expected_total_price.set(nation, expected_total_price.get(nation) + expected_price);
				}
			}, expected_stockpile_price, controllers, nations_valid);
		});
	});

	// Calculate the actual amount of demand to apply on each market, depending on the nation budget and how much we are expected to be able to afford
	state.world.execute_serial_over_market([&](auto markets) {
		auto states = state.world.market_get_zone_from_local_market(markets);
		auto controllers = state.world.state_instance_get_nation_from_state_control(states);
		auto controllers_valid = ve::apply([&](dcon::nation_id nation) {
			return state.world.nation_is_valid(nation);
		}, controllers);
		auto total_expected_price = expected_total_price.get(controllers);
		auto nation_budget = current_budget.get(controllers);
		auto can_afford_mult = ve::select(total_expected_price == 0, max_con_required_spend, ve::min(nation_budget / total_expected_price, max_con_required_spend));
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id base_commodity) {
			auto percentage_weight = state.world.market_get_government_stockpile_demand_weights(markets, base_commodity);
			auto total_demand = demand_buffer_get(controllers, base_commodity);
			auto can_purchase = total_demand * percentage_weight * can_afford_mult;
			state.world.market_set_government_construction_demand(markets, base_commodity, ve::select(controllers_valid, can_purchase, 0.0f));
		});
	});

}


void populate_construction_consumption(sys::state& state) {
	//reset_construction_demand(state);

	//static auto total_budget = ve::vectorizable_buffer<float, dcon::nation_id>(uint32_t(1));
	//static auto current_budget = ve::vectorizable_buffer<float, dcon::nation_id>(uint32_t(1));
	//static auto going_constructions = ve::vectorizable_buffer<int32_t, dcon::nation_id>(uint32_t(1));
	//{
	//	static uint32_t old_count = 1;
	//	auto new_count = state.world.nation_size();
	//	if(new_count > old_count) {
	//		total_budget = state.world.nation_make_vectorizable_float_buffer();
	//		current_budget = state.world.nation_make_vectorizable_float_buffer();
	//		going_constructions = state.world.nation_make_vectorizable_int_buffer();
	//		old_count = new_count;
	//	}
	//}

	////reset static data

	//state.world.execute_serial_over_nation([&](auto ids) {
	//	auto base_budget = state.world.nation_get_last_base_budget(ids);
	//	auto construction_priority = ve::to_float(state.world.nation_get_construction_spending(ids)) / 100.f;
	//	current_budget.set(ids, ve::max(0.f, base_budget * construction_priority));
	//	total_budget.set(ids, ve::max(0.f, base_budget * construction_priority));
	//	going_constructions.set(ids, 0);
	//});

	//// count ongoing constructions
	//// we need this number to limit amount of money going into individual constructions


	//for(auto c : state.world.in_province_building_construction) {
	//	auto owner = c.get_nation().id;
	//	if(owner && c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control() && !c.get_is_pop_project()) {
	//		going_constructions.get(owner) += 1;
	//	}
	//};
	//for(auto c : state.world.in_factory_construction) {
	//	auto owner = c.get_nation().id;
	//	if(owner && !c.get_is_pop_project()) {
	//		going_constructions.get(owner) += 1;
	//	}
	//};


	//for(auto c : state.world.in_province_building_construction) {
	//	auto owner = c.get_nation().id;
	//	float& base_budget = current_budget.get(owner);
	//	float budget_limit = total_budget.get(owner) / float(std::max(1, going_constructions.get(owner)));
	//	populate_province_building_construction_demand(state, c, base_budget, budget_limit);
	//}
	//for(auto c : state.world.in_factory_construction) {
	//	auto owner = c.get_nation().id;
	//	float& base_budget = current_budget.get(owner);
	//	float budget_limit = total_budget.get(owner) / float(std::max(1, going_constructions.get(owner)));
	//	populate_state_construction_demand(state, c, base_budget, budget_limit);
	//}
}
// Only non-unit constructions are counted here (factories, province buildings etc). Unit constructions are handled diffrently
int32_t count_ongoing_constructions(sys::state& state, dcon::nation_id n) {
	auto count = 0;

	for(auto c : state.world.in_province_building_construction) {
		auto owner = c.get_nation().id;
		if(owner != n) {
			continue;
		}
		auto spending_scale = state.world.nation_get_spending_level(owner);
		auto local_zone = c.get_province().get_state_membership();
		auto market = state.world.state_instance_get_market_from_local_market(local_zone);
		if(owner && c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control() && !c.get_is_pop_project()) {
			count++;
		}
	}
	for(auto c : state.world.in_factory_construction) {
		auto owner = c.get_nation().id;
		if(owner != n) {
			continue;
		}
		auto spending_scale = state.world.nation_get_spending_level(owner);
		auto market = state.world.state_instance_get_market_from_local_market(c.get_province().get_state_membership());
		if(owner && !c.get_is_pop_project()) {
			count++;
		}
	}

	return count;
}


//float populate_explanation_unit_construction(
//	sys::state& state,
//	dcon::nation_id n,
//	float& dedicated_budget,
//	float& estimated_spendings,
//	float budget_limit_per_project
//) {
//
//}



void populate_explanation_province_construction(
	sys::state& state,
	std::vector<province_construction_spending_entry>& data,
	dcon::nation_id n,
	float& dedicated_budget,
	float& estimated_spendings,
	float budget_limit_per_project
) {
	for(auto c : state.world.in_province_building_construction) {
		auto details = explain_province_building_construction(state, c);
		if(details.owner != n) continue;
		if(!details.can_be_advanced) continue;
		if(details.is_pop_project) continue;

		assert(0 <= int32_t(details.building_type) && int32_t(details.building_type) < int32_t(economy::max_building_types));
		auto& base_cost = state.economy_definitions.building_definitions[int32_t(details.building_type)].cost;
		auto& current_purchased = c.get_purchased_goods();
		float total_cost = 0.f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid) break;
			auto current = current_purchased.commodity_amounts[i];
			auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
			if(current >= required) continue;
			auto local_price = price(state, details.market, cid);
			auto can_purchase_budget = std::min(budget_limit_per_project, dedicated_budget) / (local_price + 0.001f);
			auto can_purchase_construction = required / details.construction_time;
			auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
			auto satisfaction = state.world.market_get_actual_probability_to_buy(details.market, cid);
			auto cost = std::min(dedicated_budget, can_purchase * satisfaction * local_price);
			dedicated_budget -= cost;
			estimated_spendings += cost;
			total_cost += cost;
		}
		province_construction_spending_entry to_add{
			.construction = c.id, .spending = total_cost
		};
		data.push_back(to_add);
	}
}
void populate_explanation_state_construction(
	sys::state& state,
	std::vector<state_construction_spending_entry>& data,
	dcon::nation_id n,
	float& dedicated_budget,
	float& estimated_spendings,
	float budget_limit_per_project
) {
	for(auto c : state.world.in_factory_construction) {
		auto details = explain_factory_building_construction(state, c);
		if(details.owner != n) continue;
		if(!details.can_be_advanced) continue;
		if(details.is_pop_project) continue;
		auto& base_cost = state.world.factory_type_get_construction_costs(details.building_type);
		auto& current_purchased = c.get_purchased_goods();
		float total_cost = 0.f;
		for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
			auto cid = base_cost.commodity_type[i];
			if(!cid) break;
			auto current = current_purchased.commodity_amounts[i];
			auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
			if(current >= required) continue;
			auto local_price = price(state, details.market, cid);
			auto can_purchase_budget = std::min(budget_limit_per_project, dedicated_budget) / (local_price + 0.001f);
			auto can_purchase_construction = required / details.construction_time;
			auto can_purchase = std::min(can_purchase_budget, can_purchase_construction);
			auto satisfaction = state.world.market_get_actual_probability_to_buy(details.market, cid);
			auto cost = std::min(dedicated_budget, can_purchase * satisfaction * local_price);
			dedicated_budget -= cost;
			estimated_spendings += cost;
			total_cost += cost;
		}
		state_construction_spending_entry to_add{
			.construction = c.id, .spending = total_cost
		};
		data.push_back(to_add);
	}
}

construction_spending_explanation explain_construction_spending(
	sys::state& state,
	dcon::nation_id n,
	float dedicated_budget
) {
	construction_spending_explanation result = {
		.ongoing_projects = count_ongoing_constructions(state, n),
		.budget_limit_per_project = 0.f,
		.estimated_spendings = 0.f,
		.province_buildings = { },
		.factories = { }
	};

	if(result.ongoing_projects == 0) return result;
	result.budget_limit_per_project = dedicated_budget / float(result.ongoing_projects);
	
	// Unit constructions are handled seperately as they work very diffrently (consume from stockpiles and distributed via military supply system)
	populate_explanation_province_construction(
		state, result.province_buildings,
		n, dedicated_budget, result.estimated_spendings, result.budget_limit_per_project
	);
	populate_explanation_state_construction(
		state, result.factories,
		n, dedicated_budget, result.estimated_spendings, result.budget_limit_per_project
	);

	return result;
}



construction_spending_explanation explain_construction_spending_now(sys::state& state, dcon::nation_id n) {
	auto treasury = state.world.nation_get_stockpiles(n, economy::money);
	auto priority = float(state.world.nation_get_construction_spending(n)) / 100.f;
	auto current_budget = std::max(0.f, treasury * priority);
	return explain_construction_spending(state, n, current_budget);
}

// TODO: write a lighter version which doesn't include all the current projects and calculates only costs
float estimate_construction_spending_from_budget(sys::state& state, dcon::nation_id n, float current_budget) {
	return explain_construction_spending(state, n, current_budget).estimated_spendings;
}

float estimate_construction_spending(sys::state& state, dcon::nation_id n) {
	auto priority = float(state.world.nation_get_construction_spending(n)) / 100.f;
	auto current_budget = std::max(0.f, economy::estimate_next_budget(state, n) * priority);
	return estimate_construction_spending_from_budget(state, n, current_budget);
}

float estimate_private_construction_spendings(sys::state& state, dcon::nation_id nid) {
	float total = 0.f;

	for(auto c : state.world.nation_get_province_building_construction(nid)) {
		auto market = state.world.state_instance_get_market_from_local_market(
			c.get_province().get_state_membership()
		);

		// Rationale for not checking building type: Its an invalid state; should not occur under normal circumstances
		if(nid == c.get_province().get_nation_from_province_control() && c.get_is_pop_project()) {
			auto t = economy::province_building_type(c.get_type());
			assert(0 <= int32_t(t) && int32_t(t) < int32_t(economy::max_building_types));
			auto& base_cost = state.economy_definitions.building_definitions[int32_t(t)].cost;
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_province_construction_time_modifier(state) *
				float(state.economy_definitions.building_definitions[int32_t(t)].time);
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i])
						total +=
						base_cost.commodity_amounts[i]
						* price(state, market, base_cost.commodity_type[i])
						/ construction_time;
				} else {
					break;
				}
			}
		}
	}

	for(auto c : state.world.nation_get_factory_construction(nid)) {
		auto location = c.get_province();
		auto sid = location.get_state_membership();
		auto market = state.world.state_instance_get_market_from_local_market(sid);
		if(c.get_is_pop_project()) {
			auto& base_cost = c.get_type().get_construction_costs();
			auto& current_purchased = c.get_purchased_goods();
			float construction_time = global_factory_construction_time_modifier(state) *
				float(c.get_type().get_construction_time()) * (c.get_is_upgrade() ? 0.1f : 1.0f);
			float factory_mod = construction_build_cost_multiplier(state, c.id);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(base_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < base_cost.commodity_amounts[i] * factory_mod)
						total +=
						base_cost.commodity_amounts[i]
						* price(state, market, base_cost.commodity_type[i])
						* factory_mod
						/ construction_time;
				} else {
					break;
				}
			}
		}
	}

	return total;
}

void populate_province_building_construction_private_demand(
	sys::state& state,
	dcon::province_building_construction_id construction
) {
	auto details = explain_province_building_construction(state, construction);
	// Rationale for not checking building type: Its an invalid state; should not occur under normal circumstances
	if(!details.can_be_advanced) return;
	if(!details.is_pop_project) return;

	assert(0 <= int32_t(details.building_type) && int32_t(details.building_type) < int32_t(economy::max_building_types));
	auto& base_cost = state.economy_definitions.building_definitions[int32_t(details.building_type)].cost;
	auto& current_purchased = state.world.province_building_construction_get_purchased_goods(construction);
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required) continue;
		auto& cur_demand = state.world.market_get_private_construction_demand(details.market, cid);
		state.world.market_set_private_construction_demand(details.market, cid, cur_demand + required / details.construction_time);
	}
}

void populate_state_construction_private_demand(
	sys::state& state,
	dcon::factory_construction_id construction
) {
	auto details = explain_factory_building_construction(state, construction);
	if(!details.can_be_advanced) return;
	if(!details.is_pop_project)	return;
	auto base_cost = details.refit_target
		? calculate_factory_refit_goods_cost(
			state, details.owner, details.province, details.building_type, details.refit_target
		) : state.world.factory_type_get_construction_costs(details.building_type);
	auto& current_purchased = state.world.factory_construction_get_purchased_goods(construction);
	for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
		auto cid = base_cost.commodity_type[i];
		if(!cid) break;
		auto current = current_purchased.commodity_amounts[i];
		auto required = base_cost.commodity_amounts[i] * details.cost_multiplier;
		if(current >= required) continue;
		auto& cur_demand = state.world.market_get_private_construction_demand(details.market, cid);
		state.world.market_set_private_construction_demand(details.market, cid, cur_demand + required / details.construction_time);
	}
}

void populate_private_construction_consumption(sys::state& state) {
	reset_private_construction_demand(state);
	for(auto c : state.world.in_province_building_construction) {
		populate_province_building_construction_private_demand(state, c);
	}
	for(auto c : state.world.in_factory_construction) {
		populate_state_construction_private_demand(state, c);
	}
}


void advance_constructions_progress(sys::state& state) {

	parallel_for_each_construction(state, [&](auto c) {
		if(!economy::can_advance_construction(state, c)) {
			return;
		}
		auto construction = fatten(state.world, c);
		auto province = construction_get_location(state, construction.id);
		auto builder = construction_get_controller(state, construction.id);
		auto type = construction_get_type(state, c);
		// Prov building id are allowed by be zero
		if(!std::is_same_v<decltype(c), dcon::province_building_construction_id>) {
			assert(type);
		}

		const economy::commodity_set actual_build_cost = construction_get_actual_build_cost(state, c);
		const economy::commodity_set& purchased_goods = get_purchased_goods(state, c);

		float total = 0.0f;
		float purchased = 0.0f;

		for(uint32_t i = 0; i < actual_build_cost.set_size; ++i) {
			total += actual_build_cost.commodity_amounts[i];
			purchased += purchased_goods.commodity_amounts[i];
		}

		float construction_days_progress = economy::construction_progress(state, construction.id);
		float goods_progress = (total == 0.0f ? 1.0f : purchased / total);
		if(goods_progress > construction_days_progress) {
			construction.set_construction_days(construction.get_construction_days() + 1);
		}
	});
}

void resolve_constructions(sys::state& state) {

	for_each_construction(state, [&](auto con) {
		if(!economy::can_advance_construction(state, con)) {
			return;
		}
		auto construction = fatten(state.world, con);
		dcon::nation_id owner = construction_get_controller(state, con);
		uint32_t required_construction_time = construction_get_actual_construction_time(state, con);
		bool construction_ready = [&]() {
			// Handle possible enabled cheats
			if constexpr(std::is_same_v<decltype(con), dcon::province_land_construction_id>) {
				if(state.world.nation_get_is_player_controlled(owner) && state.cheat_data.instant_army) {
					return true;
				}
			}
			else if constexpr(std::is_same_v<decltype(con), dcon::province_naval_construction_id>) {
				if(state.world.nation_get_is_player_controlled(owner) && state.cheat_data.instant_navy) {
					return true;
				}
			}
			else if constexpr(std::is_same_v<decltype(con), dcon::factory_construction_id>) {
				if(state.world.nation_get_is_player_controlled(owner) && state.cheat_data.instant_industry) {
					return true;
				}
			}
			if(construction.get_construction_days() < required_construction_time) {
				return false;
			}

			economy::commodity_set actual_cost = construction_get_actual_build_cost(state, con);
			const economy::commodity_set& current_purchased = get_purchased_goods(state, con);

			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
				if(actual_cost.commodity_type[i]) {
					if(current_purchased.commodity_amounts[i] < actual_cost.commodity_amounts[i]) {
						return false;
					}
				} else {
					break;
				}
			}
			return true;
		}();
		if(construction_ready) {
			dcon::province_id location = construction_get_location(state, con);
			auto type = construction_get_type(state, con);
			// Statically dispatch the logic depending on the type of construction being processed
			if constexpr(std::is_same_v<decltype(con), dcon::province_land_construction_id>) {

				dcon::pop_id pop = state.world.province_land_construction_get_pop(con);
				dcon::province_id template_prov = state.world.province_land_construction_get_template_province(con);

				auto new_reg = military::create_new_regiment(state, owner, construction_get_type(state, con));
				auto a = fatten(state.world, state.world.create_army());

				a.set_controller_from_army_control(owner);
				state.world.try_create_army_membership(new_reg, a);
				state.world.try_create_regiment_source(new_reg, pop);
				military::army_arrives_in_province(state, a, location, military::crossing_type::none);
				military::move_land_to_merge(state, owner, a, location, template_prov);

				if(owner == state.local_player_nation) {
					notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "amsg_army_built");
						},
						"amsg_army_built",
						state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::army_built,
						dcon::province_id{ }
					});
				}
				economy::delete_unit_construction<economy::construction_completed::yes>(state, con);
			}
			else if constexpr(std::is_same_v<decltype(con), dcon::province_naval_construction_id>) {
				dcon::province_id template_prov = state.world.province_naval_construction_get_template_province(con);
				auto new_ship = military::create_new_ship(state, owner, construction_get_type(state, con));
				auto a = fatten(state.world, state.world.create_navy());
				a.set_controller_from_navy_control(owner);
				military::navy_arrives_in_province(state, a, location);
				state.world.try_create_navy_membership(new_ship, a);
				military::move_navy_to_merge(state, owner, a, location, template_prov);

				if(owner == state.local_player_nation) {
					notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "amsg_navy_built");
						},
						"amsg_navy_built",
						state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::navy_built,
						dcon::province_id{ }
					});
				}

				economy::delete_unit_construction<economy::construction_completed::yes>(state, con);
			}
			else if constexpr(std::is_same_v<decltype(con), dcon::province_building_construction_id>) {
				province_building_type building_type = province_building_type(type);
				dcon::nation_id location_owner = state.world.province_get_nation_from_province_ownership(location);
				if(state.world.province_get_building_level(location, type) < state.world.nation_get_max_building_level(location_owner, type)) {
					state.world.province_set_building_level(location, type, uint8_t(state.world.province_get_building_level(location, type) + 1));

					if(building_type == province_building_type::naval_base) {
						auto civilian = (uint8_t)(advanced_province_buildings::list::civilian_ports);
						auto local_civilian_port = state.world.province_get_advanced_province_building_max_private_size(location, civilian);
						state.world.province_set_advanced_province_building_max_private_size(location, civilian, local_civilian_port + 5000.f);

						auto town_size = state.world.province_get_advanced_province_building_max_private_size(location, advanced_province_buildings::list::local_cities_and_towns);
						state.world.province_set_advanced_province_building_max_private_size(location, advanced_province_buildings::list::local_cities_and_towns, town_size + 5000.f);
					}
					else if(building_type == province_building_type::railroad) {
						auto town_size = state.world.province_get_advanced_province_building_max_private_size(location, advanced_province_buildings::list::local_cities_and_towns);
						state.world.province_set_advanced_province_building_max_private_size(location, advanced_province_buildings::list::local_cities_and_towns, town_size + 2000.f);
						/* Notify the railroad mesh builder to update the railroads! */
						state.railroad_built.store(true, std::memory_order::release);
					}

					if(owner == state.local_player_nation) {
						switch(building_type) {
						case province_building_type::naval_base:
							notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
									text::add_line(state, contents, "amsg_naval_base_complete");
								},
								"amsg_naval_base_complete",
								state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
								sys::message_base_type::naval_base_complete,
								dcon::province_id{ }
							});
							break;
						case province_building_type::fort:
							notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
									text::add_line(state, contents, "amsg_fort_complete");
								},
								"amsg_fort_complete",
								state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
								sys::message_base_type::fort_complete,
								dcon::province_id{ }
							});
							break;
						case province_building_type::railroad:
							notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
									text::add_line(state, contents, "amsg_rr_complete");
								},
								"amsg_rr_complete",
								state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
								sys::message_base_type::rr_complete,
								dcon::province_id{ }
							});
							break;
						default:
							break;
						}
					}
				}
				state.world.delete_province_building_construction(con);
			}
			else if constexpr(std::is_same_v<decltype(con), dcon::factory_construction_id>) {
				dcon::factory_type_id refit_target = state.world.factory_construction_get_refit_target(con);
				if(refit_target) {
					change_factory_type_in_province(state, location, type, refit_target);
				}
				else {
					add_factory_level_to_province(state, location, type);

					if(owner == state.local_player_nation) {
						notification::post(state, notification::message{ [](sys::state& state, text::layout_base& contents) {
								text::add_line(state, contents, "amsg_factory_complete");
							},
							"amsg_factory_complete",
							state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
							sys::message_base_type::factory_complete,
							dcon::province_id{ }
						});
					}
				}
				state.world.delete_factory_construction(con);
			}
		}
	});
}

// after usage of this function, construction demand becomes a stockpile for construction projects by finding the amount of goods which were able to be satisfied and bought
void update_private_construction_demand_satisfaction(sys::state& state, dcon::nation_id n) {
	uint32_t total_commodities = state.world.commodity_size();
	float p_spending = state.world.nation_get_private_investment_effective_fraction(n);
	//float refund_amount = 0.0f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id c{ dcon::commodity_id::value_base_t(i) };
			//auto nat_demand = state.world.market_get_construction_demand(market, c);
			//auto com_price = price(state, market, c);
			auto d_sat = state.world.market_get_actual_probability_to_buy(market, c);
			/*refund_amount +=
				nat_demand
				* (1.0f - d_sat)
				* com_price;
			assert(refund_amount >= 0.0f);*/

			//state.world.market_set_construction_demand(market, c, nat_demand * d_sat);

			auto& private_demand = state.world.market_get_private_construction_demand(market, c);

			state.world.market_set_private_construction_demand(market, c, private_demand * p_spending * d_sat);
		}
	});

	/*auto& cur_money = state.world.nation_get_stockpiles(n, economy::money);
	state.world.nation_set_stockpiles(n, economy::money, cur_money + std::min(refund_amount, total_spent_on_construction));*/
}

void advance_nation_private_constructions(sys::state& state, dcon::nation_id n, float total_spent_on_construction) {
	update_private_construction_demand_satisfaction(state, n);
	for(auto c : state.world.nation_get_province_building_construction(n)) {
		if(c.get_province().get_nation_from_province_ownership() == c.get_province().get_nation_from_province_control()) {
			advance_private_province_building_construction(state, c);
		}
	}
	for(auto c : state.world.nation_get_factory_construction(n)) {
		advance_private_factory_construction(state, c);
	}
}

// this function partly emulates demand generated by nations
void emulate_construction_demand(sys::state& state, dcon::nation_id n) {
	// phase 1:
	// simulate spending on construction of units
	// useful to help the game start with some production of artillery and small arms

	float income_to_build_units = 2'000.f;

	if(state.world.nation_get_owned_province_count(n) == 0) {
		return;
	}

	// we build the best infantry and artillery, and the best light ship and transport ship:
	auto infantry = military::get_best_infantry(state, n, false, false);
	auto artillery = military::get_best_artillery(state, n, false, false);

	auto light_ship = military::get_best_light_ship(state, n, false, false);

	auto transport = military::get_best_transport(state, n, false, false);

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		float daily_cost = 0.f;
		bool state_is_coastal = province::state_is_coastal(state, local_state);

		if(state_is_coastal) {
			if(light_ship) {
				auto& light_ship_def = state.military_definitions.unit_base_definitions[light_ship];
				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(light_ship_def.build_cost.commodity_type[i]) {
						auto p = price(state, market, light_ship_def.build_cost.commodity_type[i]);
						daily_cost += light_ship_def.build_cost.commodity_amounts[i] / std::max(1, light_ship_def.build_time) * p;
					} else {
						break;
					}
				}
			}
			if(transport) {
				auto& transport_def = state.military_definitions.unit_base_definitions[transport];
				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(transport_def.build_cost.commodity_type[i]) {
						auto p = price(state, market, transport_def.build_cost.commodity_type[i]);
						daily_cost += transport_def.build_cost.commodity_amounts[i] / std::max(1, transport_def.build_time) * p;
					} else {
						break;
					}
				}
			}
		}

		if(infantry) {
			auto& infantry_def = state.military_definitions.unit_base_definitions[infantry];
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(infantry_def.build_cost.commodity_type[i]) {
					auto p = price(state, market, infantry_def.build_cost.commodity_type[i]);
					daily_cost += infantry_def.build_cost.commodity_amounts[i] / infantry_def.build_time * p;
				} else {
					break;
				}
			}
		}

		if(artillery) {
			auto& artillery_def = state.military_definitions.unit_base_definitions[artillery];
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(artillery_def.build_cost.commodity_type[i]) {
					auto p = price(state, market, artillery_def.build_cost.commodity_type[i]);
					daily_cost += artillery_def.build_cost.commodity_amounts[i] / artillery_def.build_time * p;
				} else {
					break;
				}
			}
		}
		

		auto pairs_to_build = std::max(0.f, income_to_build_units / (daily_cost + 1.f) - 0.1f);

		if(state_is_coastal) {
			if(light_ship) {
				auto& light_ship_def = state.military_definitions.unit_base_definitions[light_ship];
				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(light_ship_def.build_cost.commodity_type[i]) {
						auto daily_amount = light_ship_def.build_cost.commodity_amounts[i] / light_ship_def.build_time;
						register_demand(state, market, light_ship_def.build_cost.commodity_type[i], daily_amount * pairs_to_build);
						auto& current = state.world.market_get_stockpile(market, light_ship_def.build_cost.commodity_type[i]);
						state.world.market_set_stockpile(market, light_ship_def.build_cost.commodity_type[i], current + daily_amount * pairs_to_build * 0.05f);
					} else {
						break;
					}
				}
			}

			if(transport) {
				auto& transport_def = state.military_definitions.unit_base_definitions[transport];
				for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
					if(transport_def.build_cost.commodity_type[i]) {
						auto daily_amount = transport_def.build_cost.commodity_amounts[i] / transport_def.build_time;
						register_demand(state, market, transport_def.build_cost.commodity_type[i], daily_amount * pairs_to_build);
						auto& current = state.world.market_get_stockpile(market, transport_def.build_cost.commodity_type[i]);
						state.world.market_set_stockpile(market, transport_def.build_cost.commodity_type[i], current + daily_amount * pairs_to_build * 0.05f);
					} else {
						break;
					}
				}
			}
		}

		if(infantry) {
			auto& infantry_def = state.military_definitions.unit_base_definitions[infantry];
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(infantry_def.build_cost.commodity_type[i]) {
					auto daily_amount = infantry_def.build_cost.commodity_amounts[i] / infantry_def.build_time;
					register_demand(state, market, infantry_def.build_cost.commodity_type[i], daily_amount * pairs_to_build);
					auto& current = state.world.market_get_stockpile(market, infantry_def.build_cost.commodity_type[i]);
					state.world.market_set_stockpile(market, infantry_def.build_cost.commodity_type[i], current + daily_amount * pairs_to_build * 0.05f);
				} else {
					break;
				}
			}
		}

		if(artillery) {
			auto& artillery_def = state.military_definitions.unit_base_definitions[artillery];
			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(artillery_def.build_cost.commodity_type[i]) {
					auto daily_amount = artillery_def.build_cost.commodity_amounts[i] / artillery_def.build_time;
					register_demand(state, market, artillery_def.build_cost.commodity_type[i], daily_amount * pairs_to_build);
					auto& current = state.world.market_get_stockpile(market, artillery_def.build_cost.commodity_type[i]);
					state.world.market_set_stockpile(market, artillery_def.build_cost.commodity_type[i], current + daily_amount * pairs_to_build * 0.05f);
				} else {
					break;
				}
			}
		}
		
	});


	// simulate spending on construction of factories
	// helps with machine tools and cement

	float income_to_build_factories = 1'000.f;

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);

		// iterate over all factory types available from the start and find "average" daily construction cost:
		float sum_of_build_times = 0.f;
		float cost_factory_set = 0.f;
		float count = 0.f;

		state.world.for_each_factory_type([&](dcon::factory_type_id factory_type) {
			if(!state.world.factory_type_get_is_available_from_start(factory_type)) {
				return;
			}

			auto build_time = state.world.factory_type_get_construction_time(factory_type);
			auto& build_cost = state.world.factory_type_get_construction_costs(factory_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto pr = price(state, market, build_cost.commodity_type[i]);
					cost_factory_set += pr * build_cost.commodity_amounts[i] / build_time;
				} else {
					break;
				}
			}
			count++;
		});


		// calculate amount of factory sets we are building:
		auto num_of_factory_sets = std::max(0.f, income_to_build_factories / (cost_factory_set + 1.f) - 0.1f);

		// emulate construction demand
		state.world.for_each_factory_type([&](dcon::factory_type_id factory_type) {
			if(!state.world.factory_type_get_is_available_from_start(factory_type)) {
				return;
			}

			auto build_time = state.world.factory_type_get_construction_time(factory_type);
			auto& build_cost = state.world.factory_type_get_construction_costs(factory_type);

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				if(build_cost.commodity_type[i]) {
					auto amount = build_cost.commodity_amounts[i];
					register_demand(
						state,
						market,
						build_cost.commodity_type[i], amount / build_time * num_of_factory_sets
					);
					auto& current = state.world.market_get_stockpile(market, build_cost.commodity_type[i]);
					state.world.market_set_stockpile(market, build_cost.commodity_type[i], current + amount / build_time * num_of_factory_sets / 100.f);
				} else {
					break;
				}
			}
			count++;
		});
	});
}

bool is_colony(sys::state& state, dcon::province_id p) {
	return state.world.province_get_is_colonial(p);
}

bool is_colony(sys::state& state, dcon::state_instance_id s) {
	return state.world.province_get_is_colonial(state.world.state_instance_get_capital(s));
}

// Check rules for factories in colonies: can a factory be built in provided province
bool can_build_factory_in_colony(sys::state& state, dcon::province_id p) {
	if(state.world.province_get_is_colonial(p) && state.defines.alice_allow_factories_in_colonies == 0.f) {
		return false;
	}

	return true;
}

// Check rules for factories in colonies: can a factory be built in provided state
bool can_build_factory_in_colony(sys::state& state, dcon::state_instance_id s) {
	auto p = state.world.state_instance_get_capital(s);
	return can_build_factory_in_colony(state, p);
}

// Check rules for factories in colonies: can this factory type be built in provided state
bool can_build_factory_type_in_colony(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft) {
	if(!is_colony(state, s)) return true;
	return can_build_factory_in_colony(state, s) && state.world.factory_type_get_can_be_built_in_colonies(ft);
}

// Check rules for factories in colonies: : can this factory type be built in provided province
bool can_build_factory_type_in_colony(sys::state& state, dcon::province_id p, dcon::factory_type_id ft) {
	if(!is_colony(state, p)) return true;
	return can_build_factory_in_colony(state, p) && state.world.factory_type_get_can_be_built_in_colonies(ft);
}

}
