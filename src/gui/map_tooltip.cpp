#include "map_tooltip.hpp"
#include "demographics.hpp"
#include "rebels.hpp"
#include "commands.hpp"
#include "unit_tooltip.hpp"
#include "economy_production.hpp"
#include "economy_stats.hpp"

namespace ui {

float selected_relative_attrition_amount(sys::state& state, dcon::nation_id n, std::vector<dcon::army_id>& list, dcon::province_id prov) {
	float total_army_weight = 0.f;
	for(auto army : list) {
		auto ar = dcon::fatten(state.world, army);
		if(ar.get_black_flag() == false && ar.get_is_retreating() == false && !bool(ar.get_navy_from_army_transport())) {
			for(auto rg : ar.get_army_membership()) {
				total_army_weight += 3.0f * rg.get_regiment().get_strength();
			}
		}
	}
	for(auto ar : state.world.province_get_army_location(prov)) {
		if(ar.get_army().get_black_flag() == false && ar.get_army().get_is_retreating() == false && !bool(ar.get_army().get_navy_from_army_transport())) {
			for(auto rg : ar.get_army().get_army_membership()) {
				total_army_weight += 3.0f * rg.get_regiment().get_strength();
			}
		}
	}
	auto prov_attrition_mod = state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::attrition);
	auto army_controller = dcon::fatten(state.world, n);
	auto supply_limit = military::supply_limit_in_province(state, army_controller, prov);
	auto attrition_mod = 1.0f + army_controller.get_modifier_values(sys::national_mod_offsets::land_attrition);
	float greatest_hostile_fort = 0.0f;
	for(auto adj : state.world.province_get_province_adjacency(prov)) {
		if((adj.get_type() & (province::border::impassible_bit | province::border::coastal_bit)) == 0) {
			auto other = adj.get_connected_provinces(0) != prov ? adj.get_connected_provinces(0) : adj.get_connected_provinces(1);
			if(other.get_building_level(uint8_t(economy::province_building_type::fort)) > 0) {
				if(military::are_at_war(state, army_controller, other.get_nation_from_province_control())) {
					greatest_hostile_fort = std::max(greatest_hostile_fort, float(other.get_building_level(uint8_t(economy::province_building_type::fort))));
				}
			}
		}
	}
	return total_army_weight * attrition_mod - (supply_limit + prov_attrition_mod + greatest_hostile_fort) > 0;
}

void country_name_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	auto owner = fat.get_nation_from_province_ownership();
	auto box = text::open_layout_box(contents);

	if(state.cheat_data.show_province_id_tooltip) {
		auto provid = state.world.province_get_provid(prov);
		auto sid = fat.get_state_membership();
		auto sdid = fat.get_state_from_abstract_state_membership();
		text::localised_format_box(state, contents, box, "province_id", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, prov.index());
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "state_id", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, sid.id.index());
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "state_definition_id", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, sdid.id.index());
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "provid", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, provid);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "state", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, sid.id);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "state_definition", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, sdid.id);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "nation_tag", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, nations::int_to_tag(owner.get_identity_from_identity_holder().get_identifying_int()));
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "nation_id", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, owner.id.value);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "province_sorting_distance", text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_four_places{ province::sorting_distance(state, state.map_state.selected_province, prov) });
		text::add_line_break_to_layout_box(state, contents, box);

		text::add_divider_to_layout_box(state, contents, box);
	}

	if(owner) {
		text::add_to_layout_box(state, contents, box, fat.get_name());
		text::add_to_layout_box(state, contents, box, std::string_view{ " (" });
		text::add_to_layout_box(state, contents, box, text::embedded_flag{ state.world.nation_get_identity_from_identity_holder(owner) });
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::get_name(state, owner.id));
		text::add_to_layout_box(state, contents, box, std::string_view{ ")" });
	} else {
		text::add_to_layout_box(state, contents, box, fat.get_name());
	}
	text::close_layout_box(contents, box);

	if(state.selected_armies.size() > 0) {
		text::add_line(state, contents, "alice_supply_limit_desc", text::variable_type::x, text::int_wholenum{ military::supply_limit_in_province(state, state.local_player_nation, fat) });
		ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::supply_limit, true);
		ui::active_modifiers_description(state, contents, fat, 0, sys::provincial_mod_offsets::supply_limit, true);

		// Supply in the target province tooltip
		for(const auto a : state.selected_armies) {
			auto controller = dcon::fatten(state.world, state.local_player_nation);
			ui::unitamounts amounts = ui::calc_amounts_from_army(state, dcon::fatten(state.world, a));
			text::substitution_map sub{};
			auto tag_str = std::string("@") + nations::int_to_tag(controller.get_identity_from_identity_holder().get_identifying_int()) + "@(A)";
			text::add_to_substitution_map(sub, text::variable_type::m, std::string_view{ tag_str });
			text::add_to_substitution_map(sub, text::variable_type::n, int64_t(amounts.type1));
			text::add_to_substitution_map(sub, text::variable_type::x, int64_t(amounts.type2));
			text::add_to_substitution_map(sub, text::variable_type::y, int64_t(amounts.type3));
			text::add_to_substitution_map(sub, text::variable_type::val, text::fp_two_places{ selected_relative_attrition_amount(state, state.local_player_nation, state.selected_armies, prov) });
			auto resolved = text::resolve_string_substitution(state, "alice_unit_relative_attrition", sub);
			box = text::open_layout_box(contents);
			text::add_unparsed_text_to_layout_box(state, contents, box, resolved);

			// Army arrival time tooltip
			auto army = dcon::fatten(state.world, a);
			auto path = command::calculate_army_path(state, state.local_player_nation, a, army.get_location_from_army_location(), prov);
			auto curprov = army.get_army_location().get_location().id;

			if (path.size() == 0) { /* No available route */ }

			else if(army.get_arrival_time() && *(army.get_path().end()) == prov) {
				sub = text::substitution_map{};
				text::add_to_substitution_map(sub, text::variable_type::date, army.get_arrival_time());
				resolved = " " + text::resolve_string_substitution(state, "unit_arrival_time_text", sub);
				text::add_to_layout_box(state, contents, box, resolved, text::text_color::white);
			}
			else {
				auto dt = state.current_date;

				for(const auto provonpath : path) {
					dt += military::movement_time_from_to(state, a, curprov, provonpath);
					curprov = provonpath;
				}

				sub = text::substitution_map{};
				text::add_to_substitution_map(sub, text::variable_type::date, dt);

				resolved = " " + text::resolve_string_substitution(state, "unit_arrival_time_text", sub);

				if(state.current_date + 15 > dt) {
					text::add_to_layout_box(state, contents, box, resolved, text::text_color::green);
				}
				else if(state.current_date + 30 < dt) {
					text::add_to_layout_box(state, contents, box, resolved, text::text_color::red);
				}
				else {
					text::add_to_layout_box(state, contents, box, resolved, text::text_color::yellow);
				}
			}

			text::close_layout_box(contents, box);
		}
	} else if(state.selected_navies.size() > 0) {
		text::add_line(state, contents, "alice_supply_limit_desc", text::variable_type::x, text::int_wholenum{ military::supply_limit_in_province(state, state.local_player_nation, fat) });
		ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::supply_limit, true);
		ui::active_modifiers_description(state, contents, fat, 0, sys::provincial_mod_offsets::supply_limit, true);

		for(const auto n : state.selected_navies) {
			auto navy = dcon::fatten(state.world, n);
			unitamounts amounts = calc_amounts_from_navy(state, navy);
			auto path = command::calculate_navy_path(state, state.local_player_nation, n, navy.get_location_from_navy_location(), prov);
			auto curprov = navy.get_navy_location().get_location().id;

			/* No available route */
			if(path.size() == 0 && prov != curprov) {
			}
			else {
				box = text::open_layout_box(contents);

				text::substitution_map sub{};
				auto unit_name = state.to_string_view(navy.get_name());
				text::add_to_substitution_map(sub, text::variable_type::m, unit_name);
				text::add_to_substitution_map(sub, text::variable_type::n, int64_t(amounts.type1));
				text::add_to_substitution_map(sub, text::variable_type::x, int64_t(amounts.type2));
				text::add_to_substitution_map(sub, text::variable_type::y, int64_t(amounts.type3));

				auto base_str = text::resolve_string_substitution(state, "ol_unit_standing_text", sub);
				text::add_to_layout_box(state, contents, box, base_str, text::text_color::white);

				// Navy arrival time tooltip
				// Zero arrival time to current province
				if(prov == curprov) {
				}
				else if(navy.get_arrival_time() && *(navy.get_path().end()) == prov) {
					sub = text::substitution_map{};
					text::add_to_substitution_map(sub, text::variable_type::date, navy.get_arrival_time());
					auto resolved = " " + text::resolve_string_substitution(state, "unit_arrival_time_text", sub);
					text::add_to_layout_box(state, contents, box, resolved, text::text_color::white);
				} else {
					auto dt = state.current_date;

					for(const auto provonpath : path) {
						dt += military::movement_time_from_to(state, n, curprov, provonpath);
						curprov = provonpath;
					}

					sub = text::substitution_map{};
					text::add_to_substitution_map(sub, text::variable_type::date, dt);

					auto resolved = " " + text::resolve_string_substitution(state, "unit_arrival_time_text", sub);

					if(state.current_date + 15 > dt) {
						text::add_to_layout_box(state, contents, box, resolved, text::text_color::green);
					} else if(state.current_date + 30 < dt) {
						text::add_to_layout_box(state, contents, box, resolved, text::text_color::red);
					} else {
						text::add_to_layout_box(state, contents, box, resolved, text::text_color::yellow);
					}
				}

				text::close_layout_box(contents, box);
			}
		}
	}
}

// 19 out of 22 (2 cant be finished so 21 of 22) finished

void terrain_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) { // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	auto box = text::open_layout_box(contents);
	text::add_to_layout_box(state, contents, box, fat.get_terrain().get_name());
	text::add_divider_to_layout_box(state, contents, box);
	text::add_line_break_to_layout_box(state, contents, box);
	text::close_layout_box(contents, box);

	if(fat.get_terrain().id) {
		ui::modifier_description(state, contents, fat.get_terrain().id);
	}
}

void political_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {   // Done
	country_name_box(state, contents, prov);
	if(auto n = state.world.province_get_nation_from_province_control(prov); n && n != state.world.province_get_nation_from_province_ownership(prov)) {
		auto fat_id = dcon::fatten(state.world, n);
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::embedded_flag{ state.world.nation_get_identity_from_identity_holder(n) });
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::get_name(state, n));
		text::close_layout_box(contents, box);
	} else if(auto rf = state.world.province_get_rebel_faction_from_province_rebel_control(prov); rf) {
		auto fat_id = dcon::fatten(state.world, rf);
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::embedded_flag{ state.national_definitions.rebel_id });
		text::add_space_to_layout_box(state, contents, box);
		auto name = rebel::rebel_name(state, rf);
		text::add_to_layout_box(state, contents, box, std::string_view{ name });
		text::close_layout_box(contents, box);
	}
}

void militancy_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		text::add_line(state, contents, "avg_mil_on_map", text::variable_type::value, text::fp_one_place{ province::revolt_risk(state, prov) });
		ui::active_modifiers_description(state, contents, prov, 0, sys::provincial_mod_offsets::pop_militancy_modifier, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(prov)), 0, sys::national_mod_offsets::core_pop_militancy_modifier, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(prov)), 0, sys::national_mod_offsets::global_pop_militancy_modifier, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(prov)), 0, sys::national_mod_offsets::non_accepted_pop_militancy_modifier, true);
	}
}

void diplomatic_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		dcon::nation_fat_id nationundermouse = fat.get_nation_from_province_ownership();
		//if(selectednation.get_is_great_power() && fat.get_nation_from_province_ownership() != selectednation) {
		if(nationundermouse) {
			dcon::nation_fat_id selectednation = (dcon::fatten(state.world, state.map_state.selected_province).get_nation_from_province_ownership().is_valid()) ?
				dcon::fatten(state.world, state.map_state.selected_province).get_nation_from_province_ownership() :
				dcon::fatten(state.world, state.local_player_nation);
			uint8_t level = uint8_t(nations::influence::get_level(state, selectednation, fat.get_nation_from_province_ownership()));
			level = level & nations::influence::level_mask;
			if(nationundermouse.get_overlord_as_subject().get_ruler() == selectednation && nationundermouse.get_is_substate()) {
				text::localised_format_box(state, contents, box, std::string_view("diplo_they_substate"));
				text::add_line_break_to_layout_box(state, contents, box);
			} else if(selectednation.get_overlord_as_subject().get_ruler() == nationundermouse && selectednation.get_is_substate()) {
				text::localised_format_box(state, contents, box, std::string_view("diplo_we_substate"));
				text::add_line_break_to_layout_box(state, contents, box);
			} else if(nationundermouse.get_overlord_as_subject().get_ruler() == selectednation && !nationundermouse.get_is_substate()) {
				text::localised_format_box(state, contents, box, std::string_view("diplo_they_satellite"));
				text::add_line_break_to_layout_box(state, contents, box);
			} else if(selectednation.get_overlord_as_subject().get_ruler() == nationundermouse && !selectednation.get_is_substate()) {
				text::localised_format_box(state, contents, box, std::string_view("diplo_we_satellite"));
				text::add_line_break_to_layout_box(state, contents, box);
			} else if(selectednation.get_is_great_power() && nationundermouse != selectednation) {
				text::add_to_layout_box(state, contents, box, fat.get_nation_from_province_ownership().id);
				text::add_to_layout_box(state, contents, box, std::string_view(": "));
				auto str = text::get_influence_level_name(state, level);
				text::add_to_layout_box(state, contents, box, std::string_view(str));
				text::add_line_break_to_layout_box(state, contents, box);
			}
			text::localised_format_box(state, contents, box, std::string_view("following_have_claims"));
			text::add_line_break_to_layout_box(state, contents, box);
			for(auto c : fat.get_core()) {
				text::add_to_layout_box(state, contents, box, std::string_view("• "));
				text::add_to_layout_box(state, contents, box, text::embedded_flag{ c.get_identity().id });
				text::add_to_layout_box(state, contents, box, c.get_identity().get_name());
				text::add_line_break_to_layout_box(state, contents, box);
			}
			text::close_layout_box(contents, box);
		}
	}
}

void region_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		std::string state_name = text::get_province_state_name(state, prov);
		text::add_to_substitution_map(sub, text::variable_type::state, std::string_view{ state_name });
		text::add_to_substitution_map(sub, text::variable_type::continentname, fat.get_continent().get_name());
		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_state_entry"), sub);
		text::close_layout_box(contents, box);
	}
}

void infrastructure_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done, doesnt appear to produce same results as V2 though
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, fat.get_rgo().get_name(), text::text_color::yellow);
		int32_t current_rails_lvl = state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::railroad));
		int32_t max_local_rails_lvl = state.world.nation_get_max_building_level(fat.get_nation_from_province_ownership().id, uint8_t(economy::province_building_type::railroad));
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_single_sub_box(state, contents, box, std::string_view("infra_level_here"), text::variable_type::val, uint16_t(float(current_rails_lvl) / float(max_local_rails_lvl)));
		text::close_layout_box(contents, box);
	}
}

void colonial_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {    // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		if(fat.get_is_coast()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::val, std::string_view(text::format_float(province::direct_distance(state, dcon::fatten(state.world, state.local_player_nation).get_capital().id, prov), 0)));
			text::localised_format_box(state, contents, box, std::string_view("colonial_range"), sub);
		}
		text::add_line_break_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, fat.get_rgo().get_name(), text::text_color::yellow);
		text::close_layout_box(contents, box);

		auto distance = province::direct_distance(state, dcon::fatten(state.world, state.local_player_nation).get_capital().id, prov);
		if(!province::can_start_colony(state, state.local_player_nation, fat.get_state_from_abstract_state_membership().id)) {
			if(fat.get_life_rating() > state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::colonial_life_rating)) {
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::colonial_life_rating, false);
			}
			if(province::has_an_owner(state, prov)) {
				auto box3 = text::open_layout_box(contents);
				text::add_line_break_to_layout_box(state, contents, box3);
				text::localised_format_box(state, contents, box3, std::string_view("colonize_settled"));
				text::add_line_break_to_layout_box(state, contents, box3);
				text::close_layout_box(contents, box3);
			} else if(distance > state.economy_definitions.building_definitions[int32_t(economy::province_building_type::naval_base)].colonial_range) {
				auto box2 = text::open_layout_box(contents);
				text::add_line_break_to_layout_box(state, contents, box2);
				text::localised_format_box(state, contents, box2, std::string_view("colonize_closest_base_to_far"));
				text::close_layout_box(contents, box2);
			}
		}
		auto box4 = text::open_layout_box(contents);

		for(auto coloniser : fat.get_state_from_abstract_state_membership().get_colonization()) {
			text::add_line_break_to_layout_box(state, contents, box4);
			text::add_to_layout_box(state, contents, box4, coloniser.get_colonizer().id, text::text_color::yellow);
			text::add_to_layout_box(state, contents, box4, std::string_view(": "), text::text_color::white);
			text::add_to_layout_box(state, contents, box4, text::format_float(coloniser.get_level(), 0), text::text_color::yellow);
		}

		text::close_layout_box(contents, box4);
	}
}

void admin_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {   // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		text::localised_single_sub_box(state, contents, box, std::string_view("provinceview_admin"), text::variable_type::value, text::fp_one_place{ state.world.province_get_control_ratio(prov) * 100.0f });
		text::close_layout_box(contents, box);
		if(fat.get_crime()) {
			text::add_line_break_to_layout(state, contents);
			text::add_line(state, contents, state.culture_definitions.crimes[fat.get_crime()].name);
			ui::modifier_description(state, contents, state.culture_definitions.crimes[fat.get_crime()].modifier);
		}
	}
}

void recruitment_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) { // TODO
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		auto max_regiments = military::regiments_max_possible_from_province(state, prov);
		auto created_regiments = military::regiments_created_from_province(state, prov);
		created_regiments += military::regiments_under_construction_in_province(state, prov);

		if(fat.get_nation_from_province_ownership().id.value == state.local_player_nation.value) {
			if(max_regiments == 0) {
				text::localised_format_box(state, contents, box, std::string_view("solmp_no_soldier_pops"));
			} else if(created_regiments < max_regiments) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::current, created_regiments);
				text::add_to_substitution_map(sub, text::variable_type::allowed, max_regiments);
				text::localised_format_box(state, contents, box, std::string_view("solmp_details"), sub);
			} else {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::current, created_regiments);
				text::add_to_substitution_map(sub, text::variable_type::allowed, max_regiments);
				text::localised_format_box(state, contents, box, std::string_view("solmp_details"), sub);
			}
			//text::localised_format_box(state, contents, box, std::string_view("solmp_over_recruited"));
		}

		text::close_layout_box(contents, box);
	}
}

void nationalfocus_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {   // TODO - Need new
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		if(fat.get_state_membership().get_owner_focus()) {
			text::localised_format_box(state, contents, box, std::string_view("pw_national_focus"));
			text::add_to_layout_box(state, contents, box, fat.get_state_membership().get_owner_focus().get_name(), text::text_color::yellow);
			modifier_description(state, contents, fat.get_state_membership().get_owner_focus().get_modifier());
		}
		text::close_layout_box(contents, box);
	}
}

void rgooutput_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {   // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, fat.get_rgo().get_name(), text::text_color::yellow);
		text::close_layout_box(contents, box);
	}
}

void population_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		text::localised_single_sub_box(state, contents, box, std::string_view("population_density_at_province"), text::variable_type::prov, (
			text::produce_simple_string(state, fat.get_name()) +
			": " +
			text::format_float(fat.get_demographics(demographics::total), 0) +
			" (" +
			text::prettify(int64_t(fat.get_demographics(demographics::total))) +
			")"
			));
		text::close_layout_box(contents, box);
	}
}

void nationality_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) { // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_tooltip_culture_majorities"), text::variable_type::prov, prov);

		std::vector<dcon::culture_fat_id> cultures;
		for(auto pop : fat.get_pop_location()) {
			if(std::find(cultures.begin(), cultures.end(), pop.get_pop().get_culture()) == cultures.end()) {
				cultures.push_back(pop.get_pop().get_culture());
			}
		}
		std::sort(cultures.begin(), cultures.end(), [&](auto a, auto b) {return fat.get_demographics(demographics::to_key(state, a.id)) > fat.get_demographics(demographics::to_key(state, b.id)); });
		for(size_t i = 0; i < cultures.size(); i++) {
			text::add_line_break_to_layout_box(state, contents, box);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, cultures[i].get_name(), text::text_color::yellow);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::white);
			text::add_to_layout_box(state, contents, box, text::format_percentage(fat.get_demographics(demographics::to_key(state, cultures[i].id)) / fat.get_demographics(demographics::total)), text::text_color::white);
			text::add_to_layout_box(state, contents, box, std::string_view(")"), text::text_color::white);
		}

		text::close_layout_box(contents, box);
	}
}

void sphere_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		if(fat.get_nation_from_province_ownership().get_is_great_power()) {
			text::localised_single_sub_box(state, contents, box, std::string_view("sphere_of_infl_great_power"), text::variable_type::country, fat.get_nation_from_province_ownership().id);
		} else {
			if(fat.get_nation_from_province_ownership().get_in_sphere_of().is_valid()) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::country1, fat.get_nation_from_province_ownership().id);
				text::add_to_substitution_map(sub, text::variable_type::country2, fat.get_nation_from_province_ownership().get_in_sphere_of());
				text::localised_format_box(state, contents, box, std::string_view("sphere_of_infl_is_in_sphere"), sub);
			}

			bool bHasDisplayedHeader = false;
			for(auto gpr : fat.get_nation_from_province_ownership().get_gp_relationship_as_influence_target()) {
				if(!bHasDisplayedHeader) {
					text::add_line_break_to_layout_box(state, contents, box);
					text::localised_single_sub_box(state, contents, box, std::string_view("sphere_of_infl_is_infl_by"), text::variable_type::country, fat.get_nation_from_province_ownership());
					text::add_line_break_to_layout_box(state, contents, box);
					bHasDisplayedHeader = true;
				}
				text::add_to_layout_box(state, contents, box, gpr.get_great_power().id, text::text_color::yellow);
				text::add_to_layout_box(state, contents, box, (
					" (" +
					text::format_float(gpr.get_influence(), 0) +
					")"
					), text::text_color::white);
				text::add_line_break_to_layout_box(state, contents, box);
			}
		}
		text::close_layout_box(contents, box);
	}
}

void supply_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "provinceview_supply_limit") + " ");
		text::add_to_layout_box(state, contents, box, military::supply_limit_in_province(state, fat.get_nation_from_province_ownership().id, prov), text::text_color::yellow);
		text::add_line_break_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "base"), text::text_color::white);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_float(state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::supply_limit), 1), text::text_color::yellow);
		text::add_line_break_to_layout_box(state, contents, box);

		// "Borrowed" from military.cpp
		auto prov_controller = state.world.province_get_nation_from_province_control(prov);
		auto self_controlled = prov_controller == state.local_player_nation;
		if(state.world.province_get_nation_from_province_ownership(prov) == state.local_player_nation && self_controlled) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_owner"), text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_three_places{ 2.5f }, text::text_color::green);
			text::add_line_break_to_layout_box(state, contents, box);
		} else if(auto dip_rel = state.world.get_diplomatic_relation_by_diplomatic_pair(prov_controller, state.local_player_nation); state.world.diplomatic_relation_get_are_allied(dip_rel) || fat.get_nation_from_province_control().get_overlord_as_subject().get_ruler() == state.local_player_nation) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_allied"), text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::format_float(2.0f, 0), text::text_color::green);
			text::add_line_break_to_layout_box(state, contents, box);
		} else if(auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(prov_controller, state.local_player_nation); state.world.unilateral_relationship_get_military_access(uni_rel)) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_access"), text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::format_float(2.0f, 0), text::text_color::green);
			text::add_line_break_to_layout_box(state, contents, box);
		} else if(bool(state.world.get_core_by_prov_tag_key(prov, state.world.nation_get_identity_from_identity_holder(state.local_player_nation)))) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_core"), text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::format_float(2.0f, 0), text::text_color::green);
			text::add_line_break_to_layout_box(state, contents, box);
		} else if(state.world.province_get_siege_progress(prov) > 0.0f) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_siege"), text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::format_float(2.0f, 0), text::text_color::red);
			text::add_line_break_to_layout_box(state, contents, box);
		}
		text::close_layout_box(contents, box);
	}
}

void party_loyalty_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {    // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		text::localised_single_sub_box(state, contents, box, std::string_view("party_loyalty_desc_header"), text::variable_type::prov, prov);
		std::vector<dcon::political_party_id> active_parties;
		nations::get_active_political_parties(state, fat.get_nation_from_province_ownership(), active_parties);
		for(auto party : active_parties) {
			text::add_line_break_to_layout_box(state, contents, box);
			auto loyalty = state.world.province_get_party_loyalty(prov, dcon::fatten(state.world, party).get_ideology().id);
			if(loyalty > 0) {
				text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::green);
				text::add_to_layout_box(state, contents, box, dcon::fatten(state.world, party).get_ideology().get_name(), text::text_color::green);
				text::add_to_layout_box(state, contents, box, std::string_view(") "), text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::yellow);
				text::add_to_layout_box(state, contents, box, dcon::fatten(state.world, party).get_ideology().get_name(), text::text_color::yellow);
				text::add_to_layout_box(state, contents, box, std::string_view(") "), text::text_color::yellow);
			}
			text::add_to_layout_box(state, contents, box, dcon::fatten(state.world, party).get_name(), text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ loyalty }, loyalty > 0 ? text::text_color::green : text::text_color::yellow);
		}
		text::close_layout_box(contents, box);
	}
}

void rank_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {    //  Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		switch(nations::get_status(state, fat.get_nation_from_province_ownership().id)) {
		case(nations::status::great_power):
		{
			text::localised_single_sub_box(state, contents, box, std::string_view("sphere_of_infl_great_power"), text::variable_type::country, fat.get_nation_from_province_ownership().id);
			text::add_line_break_to_layout_box(state, contents, box);
			break;
		}
		case(nations::status::secondary_power):
		{
			text::localised_single_sub_box(state, contents, box, std::string_view("world_ranking_secondary_power"), text::variable_type::country, fat.get_nation_from_province_ownership().id);
			text::add_line_break_to_layout_box(state, contents, box);
			break;
		}
		default:
			text::add_line_break_to_layout_box(state, contents, box);
			break;
		};
		text::localised_format_box(state, contents, box, std::string_view("world_ranking"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, fat.get_nation_from_province_ownership().get_rank(), text::text_color::yellow);
		text::close_layout_box(contents, box);
	}
}

void migration_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {   // TODO - Needs Migration Map
	auto fat = dcon::fatten(state.world, prov);
	auto owner = fat.get_nation_from_province_ownership();

	if(owner) {
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "im_em_header");
			text::add_to_layout_box(state, contents, box, text::embedded_flag{ state.world.nation_get_identity_from_identity_holder(owner) });
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::get_name(state, owner.id));
			text::close_layout_box(contents, box);
		}
		struct nation_and_value {
			float v;
			dcon::nation_id n;
		};

		static dcon::nation_id last_owner;
		static sys::date last_checked;
		static std::vector<float> last_value;
		static std::vector<nation_and_value> positive_vals;
		static std::vector<nation_and_value> neg_vals;
		static float total_pos = 0.0f;
		static float total_neg = 0.0f;

		auto d = state.ui_date;
		if(d != last_checked || last_owner != owner) {
			last_checked = d;
			last_owner = owner;
			total_pos = 0.0f;
			total_neg = 0.0f;

			demographics::estimate_directed_immigration(state, owner, last_value);
			positive_vals.clear();
			neg_vals.clear();
			for(uint32_t i = uint32_t(last_value.size()); i-- > 0; ) {
				dcon::nation_id in{ dcon::nation_id::value_base_t(i) };
				if(last_value[i] > 0.0f) {
					positive_vals.push_back(nation_and_value{ last_value[i],in });
					total_pos += last_value[i];
				} else if(last_value[i] < 0.0f) {
					neg_vals.push_back(nation_and_value{ last_value[i],in });
					total_neg += last_value[i];
				}
			}

			std::sort(positive_vals.begin(), positive_vals.end(), [](nation_and_value const& a, nation_and_value const& b) {
				return a.v > b.v;
			});
			std::sort(neg_vals.begin(), neg_vals.end(), [](nation_and_value const& a, nation_and_value const& b) {
				return a.v < b.v;
			});
		}

		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "monthly_immigration_lab");
		float total_accounted_for = 0.0f;
		for(uint32_t i = 0; i < positive_vals.size() && i < 10; ++i) {
			total_accounted_for += positive_vals[i].v;
			auto box = text::open_layout_box(contents);

			text::add_to_layout_box(state, contents, box, int64_t(positive_vals[i].v), text::text_color::green);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::embedded_flag{ state.world.nation_get_identity_from_identity_holder(positive_vals[i].n) });
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::get_name(state, positive_vals[i].n));

			text::close_layout_box(contents, box);
		}
		if(total_pos - total_accounted_for >= 1.0f) {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, int64_t(total_pos - total_accounted_for), text::text_color::green);
			text::add_space_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, "pop_other_cult");
			text::close_layout_box(contents, box);
		}

		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "monthly_emigration_lab");
		total_accounted_for = 0.0f;
		for(uint32_t i = 0; i < neg_vals.size() && i < 10; ++i) {
			total_accounted_for += neg_vals[i].v;
			auto box = text::open_layout_box(contents);

			text::add_to_layout_box(state, contents, box, int64_t(-neg_vals[i].v), text::text_color::red);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::embedded_flag{ state.world.nation_get_identity_from_identity_holder(neg_vals[i].n) });
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::get_name(state, neg_vals[i].n));

			text::close_layout_box(contents, box);
		}
		if(total_neg - total_accounted_for <= -1.0f) {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, int64_t(-(total_neg - total_accounted_for)), text::text_color::red);
			text::add_space_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, "pop_other_cult");
			text::close_layout_box(contents, box);
		}
	} else {

	}
}

void civilsation_level_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {    //Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		switch(nations::get_status(state, fat.get_nation_from_province_ownership().id)) {
		case(nations::status::great_power):
		{
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_greatnation_status"));
			break;
		}
		case(nations::status::secondary_power):
		{
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_colonialnation_status"));
			break;
		}
		case(nations::status::civilized):
		{
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_civilizednation_status"));
			// Civil
			break;
		}
		case(nations::status::westernizing):
		{
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_almost_western_nation_status"));
			text::add_line_break_to_layout_box(state, contents, box);
			auto civpro = state.world.nation_get_modifier_values(fat.get_nation_from_province_ownership().id, sys::national_mod_offsets::civilization_progress_modifier);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, std::string_view("modifier_civilization_progress")) + ": ");
			text::add_to_layout_box(state, contents, box, text::format_percentage(civpro, 2), text::text_color::green);
			// ???
			break;
		}
		case(nations::status::uncivilized):
		{
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_uncivilizednation_status"));
			text::add_line_break_to_layout_box(state, contents, box);
			auto civpro = state.world.nation_get_modifier_values(fat.get_nation_from_province_ownership().id, sys::national_mod_offsets::civilization_progress_modifier);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, std::string_view("modifier_civilization_progress")) + ": ");
			text::add_to_layout_box(state, contents, box, text::format_percentage(civpro, 2), text::text_color::green);
			// Nothing
			break;
		}
		case(nations::status::primitive):
		{
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_primitivenation_status"));
			text::add_line_break_to_layout_box(state, contents, box);
			auto civpro = state.world.nation_get_modifier_values(fat.get_nation_from_province_ownership().id, sys::national_mod_offsets::civilization_progress_modifier);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, std::string_view("modifier_civilization_progress")) + ": ");
			text::add_to_layout_box(state, contents, box, text::format_percentage(civpro, 2), text::text_color::green);
			// Nothing
			break;
		}
		default:
			break;
		};

		text::close_layout_box(contents, box);
	}
}

void relation_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {    //Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		if(dcon::fatten(state.world, state.map_state.selected_province).is_valid()
		&& dcon::fatten(state.world, state.map_state.selected_province).get_nation_from_province_ownership() != fat.get_nation_from_province_ownership()) {
			text::substitution_map sub;
			auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(fat.get_nation_from_province_ownership().id, dcon::fatten(state.world, state.map_state.selected_province).get_nation_from_province_ownership());
			auto fat_rel = dcon::fatten(state.world, rel);
			text::add_to_substitution_map(sub, text::variable_type::country1, dcon::fatten(state.world, state.map_state.selected_province).get_nation_from_province_ownership().id);
			text::add_to_substitution_map(sub, text::variable_type::country2, fat.get_nation_from_province_ownership().id);
			text::add_to_substitution_map(sub, text::variable_type::relation, int32_t(fat_rel.get_value()));
			text::localised_format_box(state, contents, box, std::string_view("relation_between"), sub);

		} else if(!dcon::fatten(state.world, state.map_state.selected_province).is_valid()
			&& dcon::fatten(state.world, state.local_player_nation) != fat.get_nation_from_province_ownership()) {
			text::substitution_map sub;
			auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(fat.get_nation_from_province_ownership().id, state.local_player_nation);
			auto fat_rel = dcon::fatten(state.world, rel);
			text::add_to_substitution_map(sub, text::variable_type::country1, state.local_player_nation);
			text::add_to_substitution_map(sub, text::variable_type::country2, fat.get_nation_from_province_ownership().id);
			text::add_to_substitution_map(sub, text::variable_type::relation, int32_t(fat_rel.get_value()));
			text::localised_format_box(state, contents, box, std::string_view("relation_between"), sub);
		}

		text::close_layout_box(contents, box);
	}
}

void crisis_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		{
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::value, text::fp_percentage_one_place{ fat.get_state_membership().get_flashpoint_tension() });
			text::localised_format_box(state, contents, box, std::string_view("flashpoint_tension"), sub);
		}
		{
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::val, text::fp_percentage_one_place{ fat.get_state_from_abstract_state_membership().get_colonization_temperature() / 100.f });
			text::localised_format_box(state, contents, box, std::string_view("province_colonisation_temperature"), sub);
		}
		text::close_layout_box(contents, box);
	}
}

void naval_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {   // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value
	&& fat.get_nation_from_province_ownership().id.value == state.local_player_nation.value
	&& fat.get_is_coast()) {
		auto box = text::open_layout_box(contents);
		if(fat.get_building_level(uint8_t(economy::province_building_type::naval_base)) == 0) {
			dcon::province_id navalprov{};
			for(auto p : fat.get_state_from_abstract_state_membership().get_abstract_state_membership()) {
				if(dcon::fatten(state.world, p).get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)) != 0) {
					navalprov = p.get_province().id;
					break;
				}
			}
			if(dcon::fatten(state.world, navalprov).is_valid()) {
				text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_naval_tooltip_in_other"), text::variable_type::prov, navalprov);
			} else {
				if(province::has_naval_base_being_built(state, prov)) {
					text::localised_format_box(state, contents, box, std::string_view("mapmode_naval_tooltip_under_con"));
				} else {
					text::localised_format_box(state, contents, box, std::string_view("mapmode_naval_tooltip_no_base"));
				}
			}
		} else {
			text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_naval_tooltip_level"), text::variable_type::lvl, fat.get_building_level(uint8_t(economy::province_building_type::naval_base)));
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::cap, military::naval_supply_from_naval_base(state, prov, state.local_player_nation));
			text::add_to_substitution_map(sub, text::variable_type::tot, military::naval_supply_points(state, state.local_player_nation));
			text::add_line_break_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("mapmode_naval_tooltip_cap"), sub);
		}

		if(province::is_overseas(state, prov)) {
			text::add_line_break_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("naval_base_overseas_limit"));
		}

		text::close_layout_box(contents, box);
	}
}

void religion_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) { // Done
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_tooltip_religion_majorities"), text::variable_type::prov, prov);

		std::vector<dcon::religion_fat_id> religions;
		for(auto pop : fat.get_pop_location()) {
			if(std::find(religions.begin(), religions.end(), pop.get_pop().get_religion()) == religions.end()) {
				religions.push_back(pop.get_pop().get_religion());
			}
		}
		std::sort(religions.begin(), religions.end(), [&](auto a, auto b) {return fat.get_demographics(demographics::to_key(state, a.id)) > fat.get_demographics(demographics::to_key(state, b.id)); });
		//for(size_t i = religions.size(); i > 0; i--) {
		for(size_t i = 0; i < religions.size(); i++) {
			text::add_line_break_to_layout_box(state, contents, box);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, religions[i].get_name(), text::text_color::yellow);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::white);
			text::add_to_layout_box(state, contents, box, text::format_percentage(fat.get_demographics(demographics::to_key(state, religions[i].id)) / fat.get_demographics(demographics::total)), text::text_color::white);
			text::add_to_layout_box(state, contents, box, std::string_view(")"), text::text_color::white);
		}

		text::close_layout_box(contents, box);
	}
}

void issues_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_tooltip_dominant_issues"), text::variable_type::prov, prov);

		std::vector<dcon::issue_option_fat_id> issues;
		state.world.for_each_issue_option([&](dcon::issue_option_id id) {
			if(fat.get_demographics(demographics::to_key(state, id)) > 0) {
				issues.push_back(dcon::fatten(state.world, id));
			}
		});
		std::sort(issues.begin(), issues.end(), [&](auto a, auto b) {return fat.get_demographics(demographics::to_key(state, a)) > fat.get_demographics(demographics::to_key(state, b)); });
		for(size_t i = 0; i < std::min(static_cast<size_t>(5), issues.size()); i++) {
			text::add_line_break_to_layout_box(state, contents, box);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, issues[i].get_name(), text::text_color::yellow);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::white);
			text::add_to_layout_box(state, contents, box, text::format_percentage(fat.get_demographics(demographics::to_key(state, issues[i].id)) / fat.get_demographics(demographics::total)), text::text_color::white);
			text::add_to_layout_box(state, contents, box, std::string_view(")"), text::text_color::white);
		}

		text::close_layout_box(contents, box);
	}
}

void income_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		float savings = 0.f;
		for(auto pop : fat.get_pop_location()) {
			savings += pop.get_pop().get_savings();
		}
		text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_tooltip_total_income"), text::variable_type::prov, prov);
		text::add_to_layout_box(state, contents, box, text::prettify_currency(savings), text::text_color::yellow);
		text::close_layout_box(contents, box);
	}
}

void ideology_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_tooltip_dominant_ideology"), text::variable_type::prov, prov);

		std::vector<dcon::ideology_fat_id> ideologies;
		float total_pops = state.world.province_get_demographics(prov, demographics::total);
		state.world.for_each_ideology([&](dcon::ideology_id id) {
			if(fat.get_demographics(demographics::to_key(state, id)) > 0) {
				ideologies.push_back(dcon::fatten(state.world, id));
			}
		});
		std::sort(ideologies.begin(), ideologies.end(), [&](auto a, auto b) {return fat.get_demographics(demographics::to_key(state, a.id)) > fat.get_demographics(demographics::to_key(state, b.id)); });
		for(size_t i = 0; i < ideologies.size(); i++) {
			text::add_line_break_to_layout_box(state, contents, box);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, ideologies[i].get_name(), text::text_color::yellow);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::white);
			text::add_to_layout_box(state, contents, box, text::format_percentage(fat.get_demographics(demographics::to_key(state, ideologies[i].id)) / fat.get_demographics(demographics::total)), text::text_color::white);
			text::add_to_layout_box(state, contents, box, std::string_view(")"), text::text_color::white);
		}

		text::close_layout_box(contents, box);
	}
}

void con_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		float average_con = (fat.get_demographics(demographics::total) == 0.f ? 0.f : fat.get_demographics(demographics::consciousness) / fat.get_demographics(demographics::total));
		text::add_line(state, contents, "mapmode_tooltip_con_average", text::variable_type::val, text::fp_one_place{ average_con });
		ui::active_modifiers_description(state, contents, prov, 0, sys::provincial_mod_offsets::pop_consciousness_modifier, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(prov)), 0, sys::national_mod_offsets::core_pop_consciousness_modifier, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(prov)), 0, sys::national_mod_offsets::global_pop_consciousness_modifier, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(prov)), 0, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier, true);
	}
}

void employment_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		float employment_rate = fat.get_demographics(demographics::employable) == 0.f ? 0.f : (fat.get_demographics(demographics::employed) / fat.get_demographics(demographics::employable));

		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_total_employment"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ employment_rate }, text::text_color::yellow);

		text::close_layout_box(contents, box);
	}
}

void literacy_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		float literacy_rate = fat.get_demographics(demographics::total) == 0.f ? 0.f : (fat.get_demographics(demographics::literacy) / fat.get_demographics(demographics::total));

		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_literacy_rate"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ literacy_rate }, text::text_color::yellow);

		text::close_layout_box(contents, box);
	}
}

void factory_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value && fat.get_nation_from_province_ownership()) {
		auto box = text::open_layout_box(contents);

		auto region = fat.get_state_membership();

		text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_tooltip_factory_count"), text::variable_type::state, text::get_province_state_name(state, prov));

		std::vector<dcon::factory_fat_id> factories;
		float totallevels = 0;
		for(auto f : state.world.province_get_factory_location(prov)) {
			factories.push_back(f.get_factory());
			totallevels += economy::get_factory_level(state, f.get_factory());
		}
		std::sort(factories.begin(), factories.end(), [&](auto a, auto b) {return economy::get_factory_level(state, a) > economy::get_factory_level(state, b); });

		text::add_to_layout_box(state, contents, box, text::prettify(int64_t(factories.size())), text::text_color::yellow);

		text::add_line_break_to_layout_box(state, contents, box);

		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_factory_size"));
		text::add_to_layout_box(state, contents, box, text::format_float(totallevels, 2), text::text_color::yellow);

		for(size_t i = 0; i < factories.size(); i++) {
			text::add_line_break_to_layout_box(state, contents, box);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, factories[i].get_building_type().get_name(), text::text_color::yellow);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::white);
			text::add_to_layout_box(state, contents, box, text::format_float(economy::get_factory_level(state, factories[i]), 2), text::text_color::white);
			text::add_to_layout_box(state, contents, box, std::string_view(")"), text::text_color::white);
		}

		text::close_layout_box(contents, box);
	}
}

void fort_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);


	if(prov.value < state.province_definitions.first_sea_province.value) {
		if(fat.get_building_level(uint8_t(economy::province_building_type::fort)) > 0) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_fort_level"));
			text::add_to_layout_box(state, contents, box, fat.get_building_level(uint8_t(economy::province_building_type::fort)), text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
		if(province::has_fort_being_built(state, fat.id)) {
			auto box3 = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box3, std::string_view("mapmode_tooltip_fort_being_built"));
			text::close_layout_box(contents, box3);
		}
		if(province::can_build_fort(state, fat.id, state.local_player_nation)) {
			auto box3 = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box3, std::string_view("mapmode_tooltip_can_build_fort"));
			text::close_layout_box(contents, box3);
		}
	}
}

void growth_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);

		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_population_change"));
		text::add_to_layout_box(state, contents, box, text::format_float(float(demographics::get_monthly_pop_increase(state, fat.id)), 0), text::text_color::yellow);

		text::close_layout_box(contents, box);
	}
}

void revolt_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto total_rebels = 0.f;
		std::vector<std::pair<dcon::rebel_faction_id, float>> rebel_factions;
		for(auto pop : fat.get_pop_location()) {
			if(pop.get_pop().get_pop_rebellion_membership().get_rebel_faction()) {
				auto fid = pop.get_pop().get_pop_rebellion_membership().get_rebel_faction().id;
				auto rebel_pop = pop.get_pop().get_size();
				auto f = std::find_if(rebel_factions.begin(), rebel_factions.end(), [fid](const auto& pair) {
					return pair.first == fid;
				});
				if(f != rebel_factions.end()) {
					f->second += rebel_pop;
				} else {
					rebel_factions.push_back(std::make_pair(fid, rebel_pop));
				}
				total_rebels += pop.get_pop().get_size();
			}
		}
		std::sort(rebel_factions.begin(), rebel_factions.end(), [&](auto a, auto b) {return a.second > b.second; });

		auto box = text::open_layout_box(contents);

		if(total_rebels <= 0.f) {
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_rebels_none"));
		} else {
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_rebels_amount"));
			text::add_to_layout_box(state, contents, box, text::prettify(int64_t(total_rebels)), text::text_color::yellow);

			for(size_t i = 0; i < rebel_factions.size(); i++) {
				text::add_line_break_to_layout_box(state, contents, box);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, rebel::rebel_name(state, rebel_factions[i].first), text::text_color::yellow);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::white);
				text::add_to_layout_box(state, contents, box, text::prettify(int64_t(rebel_factions[i].second)), text::text_color::white);
				text::add_to_layout_box(state, contents, box, std::string_view(")"), text::text_color::white);
			}
		}

		text::close_layout_box(contents, box);
	}
}

void players_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto n = state.world.province_get_nation_from_province_ownership(prov);
		if(n) {
			auto box = text::open_layout_box(contents);
			text::substitution_map sub;

			auto p = network::find_country_player(state, n);
			auto nickname = state.world.mp_player_get_nickname(p);
			text::add_to_substitution_map(sub, text::variable_type::x, sys::player_name{nickname }.to_string_view());
			if(n == state.local_player_nation) {
				if(state.network_mode == sys::network_mode_type::single_player) {
					text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_34_you_sp"), sub);
				} else {
					text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_34_you"), sub);
				}
			} else if(state.world.nation_get_is_player_controlled(n)) {
				text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_34"), sub);
			}
			if(!state.world.nation_get_is_player_controlled(n)) {
				text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_34_ai"), sub);
			}
			text::close_layout_box(contents, box);
		}
	}
}
void life_needs_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		float value = 0.f;
		float total = 0.f;
		for(const auto pl : state.world.province_get_pop_location_as_province(prov)) {
			value += pop_demographics::get_life_needs(state, pl.get_pop()) * pl.get_pop().get_size();
			total += pl.get_pop().get_size();
		}
		if(total > 0.f) {
			float ratio = value / total;
			auto box = text::open_layout_box(contents);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_percentage_one_place{ ratio });
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_35"), sub);
			text::close_layout_box(contents, box);
		}
	}
}
void everyday_needs_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		float value = 0.f;
		float total = 0.f;
		for(const auto pl : state.world.province_get_pop_location_as_province(prov)) {
			value += pop_demographics::get_everyday_needs(state, pl.get_pop()) * pl.get_pop().get_size();
			total += pl.get_pop().get_size();
		}
		if(total > 0.f) {
			float ratio = value / total;
			auto box = text::open_layout_box(contents);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_percentage_one_place{ ratio });
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_36"), sub);
			text::close_layout_box(contents, box);
		}
	}
}
void luxury_needs_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		float value = 0.f;
		float total = 0.f;
		for(const auto pl : state.world.province_get_pop_location_as_province(prov)) {
			value += pop_demographics::get_luxury_needs(state, pl.get_pop()) * pl.get_pop().get_size();
			total += pl.get_pop().get_size();
		}
		if(total > 0.f) {
			float ratio = value / total;
			auto box = text::open_layout_box(contents);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_percentage_one_place{ ratio });
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_37"), sub);
			text::close_layout_box(contents, box);
		}
	}
}
void officers_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		float value = state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.officers));
		float total = state.world.province_get_demographics(prov, demographics::total);
		float ratio = value / std::max(1.f, total);
		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::x, text::pretty_integer{ int32_t(value) });
		text::add_to_substitution_map(sub, text::variable_type::y, text::fp_percentage_one_place{ ratio });
		text::add_to_substitution_map(sub, text::variable_type::value, text::fp_percentage_one_place{ 0.02f });
		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_38"), sub);
		text::close_layout_box(contents, box);
	}
}
void ctc_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		float total_pw = state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		float total_sw = state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
		float ratio = total_pw / std::max(1.f, total_pw + total_sw);
		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::x, text::pretty_integer{ int32_t(total_pw) });
		text::add_to_substitution_map(sub, text::variable_type::y, text::pretty_integer{ int32_t(total_sw) });
		text::add_to_substitution_map(sub, text::variable_type::fraction, text::fp_percentage_one_place{ ratio });
		text::add_to_substitution_map(sub, text::variable_type::value, text::fp_percentage_one_place{ state.economy_definitions.craftsmen_fraction });
		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_39"), sub);
		text::close_layout_box(contents, box);
	}
}
void life_rating_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::x, text::pretty_integer{ int32_t(state.world.province_get_life_rating(prov)) });
		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_40"), sub);
		text::close_layout_box(contents, box);
	}
}
void crime_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value && state.world.province_get_crime(prov)) {
		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::x, state.culture_definitions.crimes[state.world.province_get_crime(prov)].name);
		text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_41"), sub);
		text::close_layout_box(contents, box);
		ui::modifier_description(state, contents, state.culture_definitions.crimes[state.world.province_get_crime(prov)].modifier);
	}
}
void rally_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		text::add_line_with_condition(state, contents, "mapmode_tooltip_42_1", state.world.province_get_land_rally_point(prov));
		text::add_line_with_condition(state, contents, "mapmode_tooltip_42_2", state.world.province_get_naval_rally_point(prov));
	}
}
void workforce_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_43"));
			text::close_layout_box(contents, box);
		}
		float total = state.world.province_get_demographics(prov, demographics::total);
		for(const auto pt : state.world.in_pop_type) {
			float value = state.world.province_get_demographics(prov, demographics::to_key(state, pt));
			if(value > 0.f) {
				auto box = text::open_layout_box(contents);
				text::add_to_layout_box(state, contents, box, pt.get_name(), text::text_color::white);
				text::add_to_layout_box(state, contents, box, std::string_view(":"), text::text_color::white);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::pretty_integer{ int32_t(value) }, text::text_color::yellow);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, std::string_view("("), text::text_color::yellow);
				text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ value / total }, text::text_color::yellow);
				text::add_to_layout_box(state, contents, box, std::string_view(")"), text::text_color::yellow);
				text::close_layout_box(contents, box);
			}
		}
	}
}
void mobilization_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	country_name_box(state, contents, prov);
	if(prov.value < state.province_definitions.first_sea_province.value) {
		auto max = military::mobilized_regiments_possible_from_province(state, prov);
		auto used = military::mobilized_regiments_created_from_province(state, prov);
		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::x, text::pretty_integer{ max });
		text::add_to_substitution_map(sub, text::variable_type::y, text::pretty_integer{ used });
		if(max == 0) {
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_44_2"), sub);
		} else {
			text::localised_format_box(state, contents, box, std::string_view("mapmode_tooltip_44"), sub);
		}
		text::close_layout_box(contents, box);
	}
}

void picking_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {   // Done
	country_name_box(state, contents, prov);
	if(auto n = state.world.province_get_nation_from_province_ownership(prov); n) {
		auto fat_id = dcon::fatten(state.world, n);
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::get_name(state, n), text::text_color::yellow);
		text::add_line_break_to_layout_box(state, contents, box);
		auto total = fat_id.get_demographics(demographics::total);
		if(total > 0.f) {
			{
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::x, text::pretty_integer{ int32_t(total) });
				text::localised_format_box(state, contents, box, std::string_view("alice_pnt_pops"), sub);
				text::add_line_break_to_layout_box(state, contents, box);
			}
			{
				auto value = fat_id.get_demographics(demographics::rich_total);
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::x, text::pretty_integer{ int32_t(value) });
				text::add_to_substitution_map(sub, text::variable_type::y, text::fp_percentage_one_place{ value / total });
				text::localised_format_box(state, contents, box, std::string_view("alice_pnt_rpops"), sub);
				text::add_line_break_to_layout_box(state, contents, box);
			}
			{
				auto value = fat_id.get_demographics(demographics::middle_total);
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::x, text::pretty_integer{ int32_t(value) });
				text::add_to_substitution_map(sub, text::variable_type::y, text::fp_percentage_one_place{ value / total });
				text::localised_format_box(state, contents, box, std::string_view("alice_pnt_mpops"), sub);
				text::add_line_break_to_layout_box(state, contents, box);
			}
			{
				auto value = fat_id.get_demographics(demographics::poor_total);
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::x, text::pretty_integer{ int32_t(value) });
				text::add_to_substitution_map(sub, text::variable_type::y, text::fp_percentage_one_place{ value / total });
				text::localised_format_box(state, contents, box, std::string_view("alice_pnt_ppops"), sub);
				text::add_line_break_to_layout_box(state, contents, box);
			}
		}
		if(fat_id.get_primary_culture()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::x, fat_id.get_primary_culture().get_name());
			text::localised_format_box(state, contents, box, std::string_view("alice_pnt_culture"), sub);
			text::add_line_break_to_layout_box(state, contents, box);
		}
		if(fat_id.get_religion()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::x, fat_id.get_religion().get_name());
			text::localised_format_box(state, contents, box, std::string_view("alice_pnt_religion"), sub);
			text::add_line_break_to_layout_box(state, contents, box);
		}
		if(fat_id.get_dominant_religion()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::x, fat_id.get_dominant_religion().get_name());
			text::localised_format_box(state, contents, box, std::string_view("alice_pnt_dominant_religion"), sub);
			text::add_line_break_to_layout_box(state, contents, box);
		}
		if(fat_id.get_dominant_ideology()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::x, fat_id.get_dominant_ideology().get_name());
			text::localised_format_box(state, contents, box, std::string_view("alice_pnt_dominant_ideology"), sub);
			text::add_line_break_to_layout_box(state, contents, box);
		}
		text::add_line_break_to_layout_box(state, contents, box);
		text::close_layout_box(contents, box);
	}
}

void populate_map_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	if(state.current_scene.overwrite_map_tooltip) {
		picking_map_tt_box(state, contents, prov);
		return;
	}

	switch(state.map_state.active_map_mode) {
	case map_mode::mode::terrain:
		terrain_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::state_select:
		political_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::political:
		political_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::revolt:
		revolt_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::diplomatic:
		diplomatic_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::region:
		region_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::infrastructure:
		infrastructure_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::colonial:
		colonial_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::admin:
		admin_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::recruitment:
		recruitment_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::national_focus:
		nationalfocus_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::rgo_output:
		rgooutput_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::population:
		population_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::nationality:
		nationality_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::sphere:
		sphere_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::supply:
		supply_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::party_loyalty:
		party_loyalty_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::rank:
		rank_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::migration:
		migration_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::civilization_level:
		civilsation_level_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::relation:
		relation_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::crisis:
		crisis_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::naval:
		naval_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::religion:
		religion_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::issues:
		issues_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::income:
		income_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::ideology:
		ideology_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::conciousness:
		con_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::employment:
		employment_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::literacy:
		literacy_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::factories:
		factory_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::fort:
		fort_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::growth:
		growth_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::militancy:
		militancy_map_tt_box(state, contents, prov);
		break;
	//even newer mapmodes
	case map_mode::mode::players:
		players_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::life_needs:
		life_needs_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::everyday_needs:
		everyday_needs_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::luxury_needs:
		luxury_needs_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::life_rating:
		life_rating_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::clerk_to_craftsmen_ratio:
		ctc_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::mobilization:
		mobilization_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::officers:
		officers_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::crime:
		crime_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::rally:
		rally_map_tt_box(state, contents, prov);
		break;
	case map_mode::mode::workforce:
		workforce_map_tt_box(state, contents, prov);
		break;
	default:
		break;
	};
}

}
