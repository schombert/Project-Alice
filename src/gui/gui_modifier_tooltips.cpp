#include "gui_modifier_tooltips.hpp"
#include "dcon_generated_ids.hpp"
#include "demographics.hpp"
#include "system_state.hpp"
#include "script_constants.hpp"
#include "text.hpp"
#include "triggers.hpp"
#include "ve_scalar_extensions.hpp"
#include "province.hpp"
#include "logistics.hpp"
#include "logistics_templates.hpp"
#include "advanced_province_buildings.hpp"

namespace ui {


std::string format_modifier_value(sys::state& state, float value, sys::modifier_display_type type) {
	switch(type) {
	case sys::modifier_display_type::integer:
		return (value >= 0.f ? "+" : "") + text::prettify(int64_t(value));
	case sys::modifier_display_type::percent:
		return (value >= 0.f ? "+" : "") + text::format_percentage(value, 1);
	case sys::modifier_display_type::percent_two_places:
		return (value >= 0.f ? "+" : "") + text::format_percentage(value, 2);
	case sys::modifier_display_type::fp_two_places:
		return(value >= 0.f ? "+" : "") + text::format_float(value, 2);
	case sys::modifier_display_type::fp_three_places:
		return (value >= 0.f ? "+" : "") + text::format_float(value, 3);
	case sys::modifier_display_type::yesno:
		return (value >= 0.f ? "yes" : "no");
	case sys::modifier_display_type::multiplier_two_places:
		return "x" + text::format_float(value, 2);
	}
	return "x%";
}

text::text_color get_modifier_value_color(sys::modifier_color_type modifier_color, float value) {
	switch(modifier_color) {
	case sys::modifier_color_type::positive_is_green:
		return (value >= 0.f ? text::text_color::green : text::text_color::red);
	case sys::modifier_color_type::positive_is_red:
		return (value >= 0.f ? text::text_color::red : text::text_color::green);
	case sys::modifier_color_type::less_than_one_is_red:
		return (value >= 1.f ? text::text_color::green : text::text_color::red);
	default:
		return text::text_color::black;
	}
}


void modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation, float scale) {
	auto fat_id = dcon::fatten(state.world, mid);

	auto const& prov_def = fat_id.get_province_values();
	for(uint32_t i = 0; i < prov_def.modifier_definition_size; ++i) {
		if(!bool(prov_def.offsets[i]))
			break;
		auto data = sys::province_modifier_metadata[prov_def.offsets[i].index()];
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, data.name), text::text_color::white);
		text::add_to_layout_box(state, layout, box, std::string_view{ ":" }, text::text_color::white);
		text::add_space_to_layout_box(state, layout, box);
		auto color = get_modifier_value_color(data.color_type, prov_def.values[i]);
		text::add_to_layout_box(state, layout, box, format_modifier_value(state, prov_def.values[i] * scale, data.type), color);
		
		// Special case since movement_cost is to show two modifiers: movement cost and trade attraction
		if(prov_def.offsets[i] == sys::provincial_mod_offsets::movement_cost) {
			text::close_layout_box(layout, box);
			box = text::open_layout_box(layout, indentation);

			text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, "alice_trade_attractiveness"), text::text_color::white);
			text::add_to_layout_box(state, layout, box, std::string_view{ ":" }, text::text_color::white);
			text::add_space_to_layout_box(state, layout, box);
			auto color2 = (prov_def.values[i] >= 0.f ? text::text_color::red : text::text_color::green);
			text::add_to_layout_box(state, layout, box, format_modifier_value(state, -1 * prov_def.values[i] * scale, data.type), color2);
		}

		text::close_layout_box(layout, box);
	}

	auto const& nat_def = fat_id.get_national_values();
	for(uint32_t i = 0; i < nat_def.modifier_definition_size; ++i) {
		if(!bool(nat_def.offsets[i]))
			break;
		auto data = sys::national_modifier_metadata[nat_def.offsets[i].index()];
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, data.name), text::text_color::white);
		text::add_to_layout_box(state, layout, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(state, layout, box);
		auto color = get_modifier_value_color(data.color_type, nat_def.values[i]);
		text::add_to_layout_box(state, layout, box, format_modifier_value(state, nat_def.values[i] * scale, data.type), color);
		text::close_layout_box(layout, box);
	}
}



void active_single_hardcoded_modifier_description(sys::state& state, text::layout_base& layout, std::string_view mod_name, float value, int32_t indentation,
		bool& header, dcon::national_modifier_value nmid) {
	if(!header) {
		header = true;
		auto box = text::open_layout_box(layout, 0);
		text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, sys::national_modifier_metadata[nmid.index()].name),
				text::text_color::yellow);
		text::add_to_layout_box(state, layout, box, std::string_view(":"), text::text_color::yellow);
		text::close_layout_box(layout, box);
	}

	auto data = sys::national_modifier_metadata[nmid.index()];
	auto box = text::open_layout_box(layout, indentation);
	text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, mod_name), text::text_color::white);
	text::add_to_layout_box(state, layout, box, std::string_view{ ":" }, text::text_color::white);
	text::add_space_to_layout_box(state, layout, box);
	auto color = get_modifier_value_color(data.color_type, value);
	text::add_to_layout_box(state, layout, box, format_modifier_value(state, value, data.type), color);
	text::close_layout_box(layout, box);
	
}

void active_single_hardcoded_modifier_description(sys::state& state, text::layout_base& layout, std::string_view mod_name, float value, int32_t indentation,
		bool& header, dcon::provincial_modifier_value pmid) {

		if(!header) {
			header = true;
			auto box = text::open_layout_box(layout, 0);
			text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, sys::province_modifier_metadata[pmid.index()].name),
					text::text_color::yellow);
			text::add_to_layout_box(state, layout, box, std::string_view(":"), text::text_color::yellow);
			text::close_layout_box(layout, box);
		}

		auto data = sys::province_modifier_metadata[pmid.index()];
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, mod_name), text::text_color::white);
		text::add_to_layout_box(state, layout, box, std::string_view{ ":" }, text::text_color::white);
		text::add_space_to_layout_box(state, layout, box);
		auto color = get_modifier_value_color(data.color_type, value);
		text::add_to_layout_box(state, layout, box, format_modifier_value(state, value, data.type), color);
		text::close_layout_box(layout, box);
	
}



void active_single_modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation,
		bool& header, dcon::national_modifier_value nmid, float scaled) {
	if(scaled == 0.f)
		return;
	auto fat_id = dcon::fatten(state.world, mid);
	auto const& def = fat_id.get_national_values();
	for(uint32_t i = 0; i < def.modifier_definition_size; ++i) {
		if(!bool(def.offsets[i]))
			break;
		if(def.offsets[i] != nmid)
			continue;

		if(!header) {
			header = true;
			auto box = text::open_layout_box(layout, 0);
			text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, sys::national_modifier_metadata[nmid.index()].name),
					text::text_color::yellow);
			text::add_to_layout_box(state, layout, box, std::string_view(":"), text::text_color::yellow);
			text::close_layout_box(layout, box);
		}

		auto data = sys::national_modifier_metadata[nmid.index()];
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, fat_id.get_name()), text::text_color::white);
		text::add_to_layout_box(state, layout, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(state, layout, box);
		auto value = def.values[i] * scaled;
		auto color = get_modifier_value_color(data.color_type, value);
		text::add_to_layout_box(state, layout, box, format_modifier_value(state, value, data.type), color);
		text::close_layout_box(layout, box);
	}
}
void active_single_modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation,
		bool& header, dcon::provincial_modifier_value pmid, float scaled) {
	if(scaled == 0.f)
		return;
	auto fat_id = dcon::fatten(state.world, mid);
	auto const& def = fat_id.get_province_values();
	for(uint32_t i = 0; i < def.modifier_definition_size; ++i) {
		if(!bool(def.offsets[i]))
			break;
		if(def.offsets[i] != pmid)
			continue;

		if(!header) {
			header = true;
			auto box = text::open_layout_box(layout, 0);
			text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, sys::province_modifier_metadata[pmid.index()].name),
					text::text_color::yellow);
			text::add_to_layout_box(state, layout, box, std::string_view(":"), text::text_color::yellow);
			text::close_layout_box(layout, box);
		}

		auto data = sys::province_modifier_metadata[pmid.index()];
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(state, layout, box, text::produce_simple_string(state, fat_id.get_name()), text::text_color::white);
		text::add_to_layout_box(state, layout, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(state, layout, box);
		auto value = def.values[i] * scaled;
		auto color = get_modifier_value_color(data.color_type, value);
		text::add_to_layout_box(state, layout, box, format_modifier_value(state, value, data.type), color);
		text::close_layout_box(layout, box);
	}
}

void active_hardcoded_modifiers_description(sys::state& state, text::layout_base& layout, dcon::nation_id n, int32_t identation,
		dcon::national_modifier_value nmid, bool& header) {
	auto fat_nation = fatten(state.world, n);
	if(nmid == sys::national_mod_offsets::national_land_supply_throughput_add) {
		float land_supply_speed = logistics::land_supply_speed(state, n);
		active_single_hardcoded_modifier_description(state, layout, "modifier_land_supply_speed", land_supply_speed * logistics::supply_throughput_per_km_land_supply_speed, identation, header, sys::national_mod_offsets::national_land_supply_throughput_add);
	}
	else if(nmid == sys::national_mod_offsets::national_naval_supply_throughput_add) {
		float naval_supply_speed = logistics::naval_supply_speed(state, n);
		active_single_hardcoded_modifier_description(state, layout, "modifier_naval_supply_speed", naval_supply_speed * logistics::supply_throughput_per_km_naval_supply_speed, identation, header, sys::national_mod_offsets::national_naval_supply_throughput_add);
	}
}

void active_hardcoded_modifiers_description(sys::state& state, text::layout_base& layout, dcon::province_id prov, int32_t identation,
		dcon::provincial_modifier_value pmid, bool& header) {
	auto fat_prov = fatten(state.world, prov);
	auto local_nation = state.local_player_nation;
	switch(pmid.value) {
	case sys::provincial_mod_offsets::supply_throughput_percent.value:
	{
		auto movement_cost = province::movement_cost(state, prov);
		float percent_mod = std::max((1.0f - movement_cost) * logistics::supply_throughput_from_movement_cost_mult, logistics::supply_throughput_from_movement_cost_max_penalty);
		if(percent_mod != 0.0f) {
			active_single_hardcoded_modifier_description(state, layout, "modifier_movement_cost", percent_mod, identation, header, sys::provincial_mod_offsets::supply_throughput_percent);
		}
		break;
	}

	case sys::provincial_mod_offsets::supply_throughput_mul.value:
	{
		float blockade_mod = logistics::supply_throughput_mult_hostile_troops_modifier(state, prov, local_nation);
		float access_mod = logistics::supply_throughput_mult_access_modifier(state, prov, local_nation);
		if(access_mod != 1.0f) {
			ui::active_single_hardcoded_modifier_description(state, layout, "supply_throughput_mult_access_modifier", access_mod, 8, header, sys::provincial_mod_offsets::supply_throughput_mul);
		}
		if(blockade_mod != 1.0f) {
			ui::active_single_hardcoded_modifier_description(state, layout, "supply_throughput_mult_hostile_units_modifier", blockade_mod, 8, header, sys::provincial_mod_offsets::supply_throughput_mul);
		}

		break;
	}
	case sys::provincial_mod_offsets::supply_loss_add.value:
	{
		bool is_sea = province::is_sea(state, prov);
		
		if(!is_sea) {
			float hostile_armies_add = logistics::supply_loss_add_hostile_armies(state, prov, local_nation);
			if(hostile_armies_add != 0.0f) {
				ui::active_single_hardcoded_modifier_description(state, layout, "supply_loss_add_hostile_armies_modifier", hostile_armies_add, 8, header, sys::provincial_mod_offsets::supply_loss_add);
			}
		}
		break;
	}
	case sys::provincial_mod_offsets::port_supply_capacity_mul.value:
	{
		float blockaded_mult = logistics::port_supply_capacity_mult_blockaded_modifier(state, prov, local_nation);
		if(blockaded_mult != 1.0f) {
			ui::active_single_hardcoded_modifier_description(state, layout, "port_supply_capacity_mul_blockaded_modifier", blockaded_mult, 8, header, sys::provincial_mod_offsets::port_supply_capacity_mul);
		}
		float access_mult = logistics::port_supply_capacity_mult_supply_access_modifier(state, prov, local_nation);
		if(access_mult != 1.0f) {
			ui::active_single_hardcoded_modifier_description(state, layout, "port_supply_capacity_mul_access_modifier", access_mult, 8, header, sys::provincial_mod_offsets::port_supply_capacity_mul);
		}
		float hostile_units_mult = logistics::port_supply_capacity_mult_hostile_troops_modifier(state, prov, local_nation);
		if(hostile_units_mult != 1.0f) {
			ui::active_single_hardcoded_modifier_description(state, layout, "port_supply_capacity_mul_hostile_units_modifier", hostile_units_mult, 8, header, sys::provincial_mod_offsets::port_supply_capacity_mul);
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

template<typename T>
void acting_modifiers_description_province(sys::state& state, text::layout_base& layout, dcon::province_id p, int32_t identation,
		bool& header, T nmid) {
	// Land province-only modifiers
	if(province::is_land(state, p)) {
		if(state.national_definitions.land_province) {
			active_single_modifier_description(state, layout, state.national_definitions.land_province, identation, header, nmid);
		}
		if(auto c = state.world.province_get_crime(p); c) {
			if(auto m = state.culture_definitions.crimes[c].modifier; m)
				active_single_modifier_description(state, layout, m, identation, header, nmid);
		}
		for(auto t = economy::province_building_type::railroad; t != economy::province_building_type::last; t = economy::province_building_type(uint8_t(t) + 1)) {
			if(state.economy_definitions.building_definitions[int32_t(t)].province_modifier) {
				active_single_modifier_description(state, layout, state.economy_definitions.building_definitions[int32_t(t)].province_modifier, identation,
						header, nmid, state.world.province_get_building_level(p, uint8_t(t)));
			}
		}
		if(state.national_definitions.infrastructure) {
			active_single_modifier_description(state, layout, state.national_definitions.infrastructure, identation, header, nmid,
					state.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad)) * state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure);
		}
		if(state.national_definitions.nationalism) {
			active_single_modifier_description(state, layout, state.national_definitions.nationalism, identation, header, nmid,
					(state.world.province_get_is_owner_core(p) ? 1.f : 0.f) * state.world.province_get_nationalism(p));
		}
		if(state.national_definitions.non_coastal) {
			active_single_modifier_description(state, layout, state.national_definitions.non_coastal, identation, header, nmid,
					!state.world.province_get_is_coast(p) ? 1.f : 0.f);
		}
		if(state.national_definitions.coastal) {
			active_single_modifier_description(state, layout, state.national_definitions.coastal, identation, header, nmid,
					state.world.province_get_is_coast(p) ? 1.f : 0.f);
		}
		if(state.national_definitions.overseas) {
			active_single_modifier_description(state, layout, state.national_definitions.overseas, identation, header, nmid,
					province::is_overseas(state, p) ? 1.f : 0.f);
		}
		if(state.national_definitions.core) {
			active_single_modifier_description(state, layout, state.national_definitions.core, identation, header, nmid,
					state.world.province_get_is_owner_core(p) ? 1.f : 0.f);
		}
		if(state.national_definitions.has_siege) {
			active_single_modifier_description(state, layout, state.national_definitions.has_siege, identation, header, nmid,
					military::province_is_under_siege(state, p) ? 1.f : 0.f);
		}
		if(state.national_definitions.blockaded) {
			active_single_modifier_description(state, layout, state.national_definitions.blockaded, identation, header, nmid,
					military::province_is_blockaded(state, p) ? 1.f : 0.f);
		}
		if(state.national_definitions.province_militancy) {
			float total_militancy = state.world.province_get_demographics(p, demographics::militancy);
			float total_pop = state.world.province_get_demographics(p, demographics::total);
			float avg_militancy = (total_pop == 0.0f ? 0.0f : total_militancy / total_pop);
			active_single_modifier_description(state, layout, state.national_definitions.province_militancy, identation, header, nmid, avg_militancy / 10.f);
		}
		if(state.national_definitions.province_control) {
			float control_level = state.world.province_get_control_ratio(p);
			active_single_modifier_description(state, layout, state.national_definitions.province_control, identation, header, nmid, control_level);
		}
		if(state.national_definitions.civilian_port) {
			float civilian_port = state.world.province_get_advanced_province_building_max_private_size(p, advanced_province_buildings::list::civilian_ports) / 1000.0f;
			active_single_modifier_description(state, layout, state.national_definitions.civilian_port, identation, header, nmid, civilian_port);
		}

	}
	// sea province-only modifiers
	else {
		if(state.national_definitions.sea_zone) {
			active_single_modifier_description(state, layout, state.national_definitions.sea_zone, identation, header, nmid);
		}
	}
	if(state.national_definitions.province_base) {
		active_single_modifier_description(state, layout, state.national_definitions.province_base, identation, header, nmid);
	}
	for(auto mpr : state.world.province_get_current_modifiers(p))
		active_single_modifier_description(state, layout, mpr.mod_id, identation, header, nmid);
	if(auto m = state.world.province_get_terrain(p); m)
		active_single_modifier_description(state, layout, m, identation, header, nmid);
	if(auto m = state.world.province_get_climate(p); m)
		active_single_modifier_description(state, layout, m, identation, header, nmid);
	if(auto m = state.world.province_get_continent(p); m)
		active_single_modifier_description(state, layout, m, identation, header, nmid);
	if constexpr(std::is_same_v<T, dcon::provincial_modifier_value>) {
		active_hardcoded_modifiers_description(state, layout, p, identation, nmid, header);
	}

}

void active_modifiers_description(sys::state& state, text::layout_base& layout, dcon::province_id p, int32_t identation,
		dcon::provincial_modifier_value nmid, bool have_header) {
	bool header = !have_header;
	acting_modifiers_description_province(state, layout, p, identation, header, nmid);
}

void active_modifiers_description(sys::state& state, text::layout_base& layout, dcon::nation_id n, int32_t identation,
		dcon::national_modifier_value nmid, bool have_header) {
	bool header = !have_header;
	if(auto ts = state.world.nation_get_tech_school(n); ts)
		active_single_modifier_description(state, layout, ts, identation, header, nmid);
	if(auto nv = state.world.nation_get_national_value(n); nv)
		active_single_modifier_description(state, layout, nv, identation, header, nmid);
	for(auto mpr : state.world.nation_get_current_modifiers(n))
		active_single_modifier_description(state, layout, mpr.mod_id, identation, header, nmid);
	state.world.for_each_technology([&](dcon::technology_id t) {
		auto tmod = state.world.technology_get_modifier(t);
		if(tmod && state.world.nation_get_active_technologies(n, t))
			active_single_modifier_description(state, layout, tmod, identation, header, nmid);
	});
	state.world.for_each_invention([&](dcon::invention_id i) {
		auto tmod = state.world.invention_get_modifier(i);
		if(tmod && state.world.nation_get_active_inventions(n, i))
			active_single_modifier_description(state, layout, tmod, identation, header, nmid);
	});
	state.world.for_each_issue([&](dcon::issue_id i) {
		auto iopt = state.world.nation_get_issues(n, i);
		auto imod = state.world.issue_option_get_modifier(iopt);
		if(imod &&
				(state.world.nation_get_is_civilized(n) || state.world.issue_get_issue_type(i) == uint8_t(culture::issue_type::party)))
			active_single_modifier_description(state, layout, imod, identation, header, nmid);
	});
	if(!state.world.nation_get_is_civilized(n)) {
		state.world.for_each_reform([&](dcon::reform_id i) {
			auto iopt = state.world.nation_get_reforms(n, i);
			auto imod = state.world.reform_option_get_modifier(iopt);
			if(imod)
				active_single_modifier_description(state, layout, imod, identation, header, nmid);
		});
	}

	if(state.national_definitions.nation_base) {
		active_single_modifier_description(state, layout, state.national_definitions.nation_base, identation, header, nmid);
	}

	auto in_wars = state.world.nation_get_war_participant(n);
	if(in_wars.begin() != in_wars.end()) {
		if(state.national_definitions.war)
			active_single_modifier_description(state, layout, state.national_definitions.war, identation, header, nmid);
	} else {
		if(state.national_definitions.peace)
			active_single_modifier_description(state, layout, state.national_definitions.peace, identation, header, nmid);
	}

	if(state.national_definitions.badboy) {
		active_single_modifier_description(state, layout, state.national_definitions.badboy, identation, header, nmid,
				state.world.nation_get_infamy(n));
	}
	if(state.national_definitions.plurality) {
		active_single_modifier_description(state, layout, state.national_definitions.plurality, identation, header, nmid,
				state.world.nation_get_plurality(n));
	}
	if(state.national_definitions.war_exhaustion) {
		active_single_modifier_description(state, layout, state.national_definitions.war_exhaustion, identation, header, nmid,
				state.world.nation_get_war_exhaustion(n));
	}
	if(state.national_definitions.average_literacy) {
		auto literacy = nations::get_avg_non_colonial_literacy(state, n);
		active_single_modifier_description(state, layout, state.national_definitions.average_literacy, identation, header, nmid, literacy);
	}
	if(state.national_definitions.total_blockaded) {
		auto bc = ve::to_float(state.world.nation_get_central_blockaded(n));
		auto c = ve::to_float(state.world.nation_get_central_ports(n));
		active_single_modifier_description(state, layout, state.national_definitions.total_blockaded, identation, header, nmid,
				c > 0.0f ? bc / c : 0.0f);
	}
	if(state.national_definitions.total_occupation) {
		auto nid = fatten(state.world, n);
		auto cap_continent = nid.get_capital().get_continent();
		float total = 0.0f;
		float occupied = 0.0f;
		for(auto owned : nid.get_province_ownership()) {
			if(owned.get_province().get_continent() == cap_continent) {
				total += 1.0f;
				if(auto c = owned.get_province().get_nation_from_province_control().id; c && c != n) {
					occupied += 1.0f;
				}
			}
		}
		active_single_modifier_description(state, layout, state.national_definitions.total_occupation, identation, header, nmid,
				total > 0.0f ? occupied / total : 0.0f);
	}

	if(state.world.nation_get_is_civilized(n) == false) {
		if(state.national_definitions.unciv_nation)
			active_single_modifier_description(state, layout, state.national_definitions.unciv_nation, identation, header, nmid);
	} else if(nations::is_great_power(state, n)) {
		if(state.national_definitions.great_power)
			active_single_modifier_description(state, layout, state.national_definitions.great_power, identation, header, nmid);
	} else if(state.world.nation_get_rank(n) <= uint16_t(state.defines.colonial_rank)) {
		if(state.national_definitions.second_power)
			active_single_modifier_description(state, layout, state.national_definitions.second_power, identation, header, nmid);
	} else {
		if(state.national_definitions.civ_nation)
			active_single_modifier_description(state, layout, state.national_definitions.civ_nation, identation, header, nmid);
	}

	if(state.national_definitions.disarming) {
		if(bool(state.world.nation_get_disarmed_until(n)) && state.world.nation_get_disarmed_until(n) > state.current_date)
			active_single_modifier_description(state, layout, state.national_definitions.disarming, identation, header, nmid);
	}
	if(state.national_definitions.in_bankrupcy) {
		if(bool(state.world.nation_get_is_bankrupt(n)))
			active_single_modifier_description(state, layout, state.national_definitions.in_bankrupcy, identation, header, nmid);
	}
	// TODO: debt

	for(auto tm : state.national_definitions.triggered_modifiers) {
		if(tm.trigger_condition && tm.linked_modifier) {
			auto trigger_condition_satisfied =
					trigger::evaluate(state, tm.trigger_condition, trigger::to_generic(n), trigger::to_generic(n), 0);
			if(trigger_condition_satisfied)
				active_single_modifier_description(state, layout, tm.linked_modifier, identation, header, nmid);
		}
	}
	if(state.national_definitions.fastest_land_unit_speed) {
		auto fastest_land_unit = state.world.nation_get_fastest_unlocked_land_unit(n);
		auto spd = state.world.nation_get_unit_stats(n, fastest_land_unit).maximum_speed;
		active_single_modifier_description(state, layout, state.national_definitions.fastest_land_unit_speed, identation, header, nmid, spd);
	}
	if(state.national_definitions.fastest_transport_unit_speed) {
		auto fastest_transport_unit = state.world.nation_get_fastest_unlocked_transport_unit(n);
		auto spd = state.world.nation_get_unit_stats(n, fastest_transport_unit).maximum_speed;
		active_single_modifier_description(state, layout, state.national_definitions.fastest_transport_unit_speed, identation, header, nmid, spd);
	}

	// Provinces of this nation
	for(auto pc : state.world.nation_get_province_ownership_as_nation(n)) {
		auto p = pc.get_province().id;
		acting_modifiers_description_province<dcon::national_modifier_value>(state, layout, p, identation, header, nmid);
	}

	// Hardcoded modifiers applied from the code are shown here
	active_hardcoded_modifiers_description(state, layout, n, identation, nmid, header);
	
}
void display_land_battle_supply_satisfaction(sys::state& state, dcon::land_battle_id b, text::layout_base& contents, int32_t indent, bool attacker) {
	uint32_t reserve_count = military::get_reserves_count_by_side(state, b, attacker);
	//top header displaying how many brigades are currently in reserve on that side
	//text::add_line(state, contents, "battle_reinforcement_header_tooltip", text::variable_type::x, text::format_wholenum(reserve_count), indent);


	float total_reinf_fufilled = 0.0f;
	float total_reinf_requied = 0.0f;
	float total_supply_fufilled = 0.0f;
	float total_supply_requied = 0.0f;
	for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
		auto army = a.get_army();
		bool attacker_army = military::is_attacker_in_battle(state, army);
		if((attacker && attacker_army) || (!attacker && !attacker_army)) {
			auto fufilled_reinf_goods = military::unit_get_last_fufilled_goods_need<military::unit_consumption_type::reinforcement>(state, army.id);
			auto required_reinf_goods = military::unit_get_last_required_goods_need<military::unit_consumption_type::reinforcement>(state, army.id);
			auto fufilled_supply_goods = military::unit_get_last_fufilled_goods_need<military::unit_consumption_type::supply>(state, army.id);
			auto required_supply_goods = military::unit_get_last_required_goods_need<military::unit_consumption_type::supply>(state, army.id);
			std::for_each(fufilled_reinf_goods.begin(), fufilled_reinf_goods.end(), [&](float amount) { total_reinf_fufilled += amount; });
			std::for_each(required_reinf_goods.begin(), required_reinf_goods.end(), [&](float amount) { total_reinf_requied += amount; });
			std::for_each(fufilled_supply_goods.begin(), fufilled_supply_goods.end(), [&](float amount) { total_supply_fufilled += amount; });
			std::for_each(required_supply_goods.begin(), required_supply_goods.end(), [&](float amount) { total_supply_requied += amount; });

		}
	}
	float reinforcement_satisfaction = (total_reinf_requied == 0.0f ? 1.0f : total_reinf_fufilled / total_reinf_requied);
	float supply_satisfaction = (total_supply_requied == 0.0f ? 1.0f : total_supply_fufilled / total_supply_requied);
	text::add_line(state, contents, "battle_supply_satisfaction_tooltip", text::variable_type::val, text::fp_percentage{ supply_satisfaction }, indent + 20);
	text::add_line(state, contents, "battle_reinforcement_satisfaction_tooltip", text::variable_type::val, text::fp_percentage{ reinforcement_satisfaction }, indent + 20);
}

void display_unit_stats(sys::state& state, text::columnar_layout& contents, dcon::nation_id controller, dcon::unit_type_id unit_type) {

	text::add_line(state, contents, state.military_definitions.unit_base_definitions[unit_type].name);
	const auto& stats = state.world.nation_get_unit_stats(controller, unit_type);
	if(state.military_definitions.unit_base_definitions[unit_type].is_land) {
		if(stats.reconnaissance_or_fire_range > 0) {
			text::add_line(state, contents, "unit_recon", text::variable_type::x, text::format_float(stats.reconnaissance_or_fire_range, 2));
		}
		if(stats.siege_or_torpedo_attack > 0) {
			text::add_line(state, contents, "unit_siege", text::variable_type::x, text::format_float(stats.siege_or_torpedo_attack, 2));
		}
		text::add_line(state, contents, "unit_attack", text::variable_type::x, text::format_float(stats.attack_or_gun_power, 2));
		text::add_line(state, contents, "unit_defence", text::variable_type::x, text::format_float(stats.defence_or_hull, 2));
		text::add_line(state, contents, "unit_discipline", text::variable_type::x, text::format_percentage(stats.discipline_or_evasion, 0));
		if(stats.support > 0) {
			text::add_line(state, contents, "unit_support", text::variable_type::x, text::format_percentage(stats.support, 0));
		}
		text::add_line(state, contents, "unit_maneuver", text::variable_type::x, text::format_float(stats.maneuver, 0));
		text::add_line(state, contents, "unit_max_speed", text::variable_type::x, text::format_float(stats.maximum_speed, 2));
		text::add_line(state, contents, "unit_supply_consumption", text::variable_type::x, text::format_percentage(stats.supply_consumption, 0));
	} else {
		text::add_line(state, contents, "unit_max_speed", text::variable_type::x, text::format_float(stats.maximum_speed, 2));
		text::add_line(state, contents, "unit_attack", text::variable_type::x, text::format_float(stats.attack_or_gun_power, 2));
		if(stats.siege_or_torpedo_attack > 0) {
			text::add_line(state, contents, "unit_torpedo_attack", text::variable_type::x, text::format_float(stats.siege_or_torpedo_attack, 2));
		}
		text::add_line(state, contents, "unit_hull", text::variable_type::x, text::format_float(stats.defence_or_hull, 2));
		text::add_line(state, contents, "unit_fire_range", text::variable_type::x, text::format_float(stats.reconnaissance_or_fire_range, 2));
		if(stats.discipline_or_evasion > 0) {
			text::add_line(state, contents, "unit_evasion", text::variable_type::x, text::format_percentage(stats.discipline_or_evasion, 0));
		}
		text::add_line(state, contents, "unit_supply_consumption", text::variable_type::x, text::format_percentage(stats.supply_consumption, 0));
		text::add_line(state, contents, "unit_supply_load", text::variable_type::x, state.military_definitions.unit_base_definitions[unit_type].supply_consumption_score);
	}

}



} // namespace ui
