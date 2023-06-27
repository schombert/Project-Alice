#include "map_tooltip.hpp"


namespace ui {

void country_name_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
    auto fat = dcon::fatten(state.world, prov);
    auto box = text::open_layout_box(contents);
	if(prov.value < state.province_definitions.first_sea_province.value) {
        text::substitution_map sub;
        text::add_to_substitution_map(sub, text::variable_type::state, fat.get_name());
        text::add_to_substitution_map(sub, text::variable_type::name, fat.get_nation_from_province_ownership().get_name());
        text::localised_format_box(state, contents, box, std::string_view("mtt_name"), sub);
    } else {
        text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, fat.get_name()));
    }
    text::close_layout_box(contents, box);
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
    //auto box = text::open_layout_box(contents);
    //text::close_layout_box(contents, box);
}

void revolt_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done 
    auto fat = dcon::fatten(state.world, prov);
    country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);
        text::localised_single_sub_box(state, contents, box, std::string_view("avg_mil_on_map"), text::variable_type::value, text::fp_one_place{state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::pop_militancy_modifier)});

        if(fat.get_crime()) {
            text::add_line_break_to_layout_box(state, contents, box);
		    text::add_to_layout_box(state, contents, box, state.culture_definitions.crimes[fat.get_crime()].name);
            text::add_divider_to_layout_box(state, contents, box);
        }
        text::close_layout_box(contents, box);

        ui::modifier_description(state, contents, state.culture_definitions.crimes[fat.get_crime()].modifier);
    }
}

void diplomatic_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done
    auto fat = dcon::fatten(state.world, prov);
    country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);
        dcon::nation_fat_id nationundermouse = fat.get_nation_from_province_ownership();
        dcon::nation_fat_id selectednation = (dcon::fatten(state.world, state.map_state.selected_province).get_nation_from_province_ownership().is_valid()) ?
                                            dcon::fatten(state.world, state.map_state.selected_province).get_nation_from_province_ownership() :
                                            dcon::fatten(state.world, state.local_player_nation);
        //if(selectednation.get_is_great_power() && fat.get_nation_from_province_ownership() != selectednation) {
        uint32_t level = nations::influence::get_level(state, selectednation.id, fat.get_nation_from_province_ownership().id);
        level = level & nations::influence::level_mask;

        if(nationundermouse.get_overlord_as_subject().get_ruler() == selectednation && nationundermouse.get_is_substate()) {
            text::localised_format_box(state, contents, box, std::string_view("diplo_they_substate"));
            text::add_line_break_to_layout_box(state, contents, box);

        } else if(selectednation.get_overlord_as_subject().get_ruler() == nationundermouse && selectednation.get_is_substate()){
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

            if(level == nations::influence::level_in_sphere) {
                text::localised_format_box(state, contents, box, std::string_view("rel_sphere_of_influence"));
            } else if(level == nations::influence::level_friendly) {
                text::localised_format_box(state, contents, box, std::string_view("rel_friendly"));
            } else if(level == nations::influence::level_cordial) {
                text::localised_format_box(state, contents, box, std::string_view("rel_cordial"));
            } else if(level == nations::influence::level_neutral) {
                text::localised_format_box(state, contents, box, std::string_view("rel_neutral"));
            } else if(level == nations::influence::level_opposed) {
                text::localised_format_box(state, contents, box, std::string_view("rel_opposed"));
            } else if(level == nations::influence::level_hostile) {
                text::localised_format_box(state, contents, box, std::string_view("rel_hostile"));
            }  else {
                text::add_to_layout_box(state, contents, box, std::string_view("map_tooltip.cpp:109 :: Send to a Programmer!"));
            }
            text::add_line_break_to_layout_box(state, contents, box);
        }
        text::localised_format_box(state, contents, box, std::string_view("following_have_claims"));
        text::add_line_break_to_layout_box(state, contents, box);
        for(auto c : fat.get_core()) {
            // TODO - replace these once flags get implemented
            text::add_to_layout_box(state, contents, box, std::string_view(" - "));
            text::add_to_layout_box(state, contents, box, c.get_identity().get_name());
            text::add_line_break_to_layout_box(state, contents, box);
        }
        

        if(state.use_debug_mode) {
            text::add_line_break_to_layout_box(state, contents, box);
            if(fat.get_province_rebel_control().is_valid()) {
                text::add_to_layout_box(state, contents, box, std::string_view("Province is Controlled by: "));
                text::add_to_layout_box(state, contents, box, fat.get_rebel_faction_from_province_rebel_control().get_type().get_name(), text::text_color::yellow);
                text::add_line_break_to_layout_box(state, contents, box);
            }
            if(fat.get_nation_from_province_control() != fat.get_nation_from_province_ownership()) {
                text::add_to_layout_box(state, contents, box, fat.id, text::text_color::yellow);
                text::add_to_layout_box(state, contents, box, std::string_view(" is currently controlled by "));
                text::add_to_layout_box(state, contents, box, fat.get_nation_from_province_control().id, text::text_color::yellow);
                text::add_line_break_to_layout_box(state, contents, box);
            }
            text::add_to_layout_box(state, contents, box, fat.id, text::text_color::yellow);
            text::add_to_layout_box(state, contents, box, std::string_view(" is owned by "));
            text::add_to_layout_box(state, contents, box, fat.get_nation_from_province_ownership().id, text::text_color::yellow);
        }
        text::close_layout_box(contents, box);
    }
}

void region_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done 
    auto fat = dcon::fatten(state.world, prov);
    country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);
        text::substitution_map sub;
        text::add_to_substitution_map(sub, text::variable_type::state, fat.get_state_from_abstract_state_membership().get_name());
        text::add_to_substitution_map(sub, text::variable_type::continentname, fat.get_continent().get_name());
        text::localised_format_box(state, contents, box, std::string_view("mtt_state_entry"), sub);
        text::close_layout_box(contents, box);
    }
}

void infrastructure_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {  // Done, doesnt appear to produce same results as V2 though
    auto fat = dcon::fatten(state.world, prov);
    country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);
        text::add_to_layout_box(state, contents, box, fat.get_rgo().get_name(), text::text_color::yellow);
		int32_t current_rails_lvl = state.world.province_get_railroad_level(prov);
		int32_t max_local_rails_lvl = state.world.nation_get_max_railroad_level(fat.get_nation_from_province_ownership().id);
        text::add_line_break_to_layout_box(state, contents, box);
        text::localised_single_sub_box(state, contents, box, std::string_view("infra_level_here"), text::variable_type::val, uint16_t(float(current_rails_lvl) / float(max_local_rails_lvl)));
        text::close_layout_box(contents, box);
    }
}

void colonial_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {    // Done
    auto fat = dcon::fatten(state.world, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);
        text::add_to_layout_box(state, contents, box, fat.get_abstract_state_membership().get_state().get_name(), text::text_color::white);
        if(fat.get_is_coast()) {
            text::add_to_layout_box(state, contents, box, std::string_view(" - "));
            text::substitution_map sub;
            text::add_to_substitution_map(sub, text::variable_type::val, 
            std::string_view(text::format_float(province::direct_distance(state, dcon::fatten(state.world, state.local_player_nation).get_capital().id, prov), 0))
            );
            //text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "colonial_range"), sub);
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
            if(distance > state.economy_definitions.naval_base_definition.colonial_range) {
                auto box2 = text::open_layout_box(contents);
                text::add_line_break_to_layout_box(state, contents, box2);
                text::localised_format_box(state, contents, box2, std::string_view("colonize_closest_base_to_far"));
                text::close_layout_box(contents, box2);
            }
            if(province::has_an_owner(state, prov)) {
                auto box3 = text::open_layout_box(contents);
                text::localised_format_box(state, contents, box, std::string_view("colonize_settled"));
                text::add_line_break_to_layout_box(state, contents, box3);
                text::close_layout_box(contents, box3);
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
    } else {
        country_name_box(state, contents, prov);
    }
}

void admin_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {   // Done 
    auto fat = dcon::fatten(state.world, prov);
    country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);
        text::localised_single_sub_box(state, contents, box, std::string_view("provinceview_admin"), text::variable_type::value, text::fp_one_place{province::state_admin_efficiency(state, fat.get_state_membership().id) * 100.0f});
        text::close_layout_box(contents, box);
    }
}

void recruitment_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) { // TODO
    auto fat = dcon::fatten(state.world, prov);
    country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);

	    auto max_regiments = military::regiments_max_possible_from_province(state, prov);
	    auto created_regiments = military::regiments_created_from_province(state, prov);

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
        if(fat.get_state_membership().get_owner_focus().is_valid()) {
            text::localised_format_box(state, contents, box, std::string_view("pw_national_focus"));
            text::add_to_layout_box(state, contents, box, fat.get_state_membership().get_owner_focus().get_name(), text::text_color::yellow);
            // TODO - nattional focus desc should go here
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
        if(state.user_settings.use_new_ui) {
            text::localised_single_sub_box(state, contents, box, std::string_view("mtt_culture_majorities"), text::variable_type::prov, prov);
        } else {
            text::localised_single_sub_box(state, contents, box, std::string_view("nationality_majority_at_province"), text::variable_type::prov, prov);
        }
        std::vector<dcon::culture_fat_id> cultures;
        for(auto pop : fat.get_pop_location()) {
            if(std::find(cultures.begin(), cultures.end(), pop.get_pop().get_culture()) == cultures.end()) {
                cultures.push_back(pop.get_pop().get_culture());
            }
        }
        std::sort(cultures.begin(), cultures.end(), [&](auto a, auto b){return fat.get_demographics(demographics::to_key(state, a.id)) > fat.get_demographics(demographics::to_key(state, b.id));});
        //for(size_t i = cultures.size(); i > 0; i--) {
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
            text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_owner") + " x", text::text_color::white);
            text::add_to_layout_box(state, contents, box, text::fp_three_places{2.5f}, text::text_color::green);
            text::add_line_break_to_layout_box(state, contents, box);
	    /*} else if(self_controlled ||
	    					bool(state.world.province_get_rebel_faction_from_province_rebel_control(p))) { // TODO: check for sieging

                            // This does not appear to be used
            text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_allied") + " x", text::text_color::white);
            text::add_to_layout_box(state, contents, box, text::format_float{2.0f, 0}, text::text_color::red);
            text::add_line_break_to_layout_box(state, contents, box);
        */
	    } else if(auto dip_rel = state.world.get_diplomatic_relation_by_diplomatic_pair(prov_controller, state.local_player_nation);
	    					state.world.diplomatic_relation_get_are_allied(dip_rel)
                || fat.get_nation_from_province_control().get_overlord_as_subject().get_ruler() == state.local_player_nation) {
            text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_allied") + " x", text::text_color::white);
            text::add_to_layout_box(state, contents, box, text::format_float(2.0f, 0), text::text_color::green);
            text::add_line_break_to_layout_box(state, contents, box);
	    } else if(auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(prov_controller, state.local_player_nation);
	    					state.world.unilateral_relationship_get_military_access(uni_rel)) {
            text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_access") + " x", text::text_color::white);
            text::add_to_layout_box(state, contents, box, text::format_float(2.0f, 0), text::text_color::green);
            text::add_line_break_to_layout_box(state, contents, box);
	    } else if(bool(state.world.get_core_by_prov_tag_key(prov, state.world.nation_get_identity_from_identity_holder(state.local_player_nation)))) {
            text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_core") + " x", text::text_color::white);
            text::add_to_layout_box(state, contents, box, text::format_float(2.0f, 0), text::text_color::green);
            text::add_line_break_to_layout_box(state, contents, box);
	    } else if(state.world.province_get_siege_progress(prov) > 0.0f) {
            text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "supply_siege") + " x", text::text_color::white);
            text::add_to_layout_box(state, contents, box, text::format_float(2.0f, 0), text::text_color::red);
            text::add_line_break_to_layout_box(state, contents, box);
	    }

        text::close_layout_box(contents, box);
    }
}

void partyloyalty_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {    // Done
    auto fat = dcon::fatten(state.world, prov);
    country_name_box(state, contents, prov);

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);
        text::localised_single_sub_box(state, contents, box, std::string_view("party_loyalty_desc_header"), text::variable_type::prov, prov);
        std::vector<dcon::political_party_id> activeparties;
		nations::get_active_political_parties(state, fat.get_nation_from_province_ownership().id).swap(activeparties);

        for(auto party : activeparties) {
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
            if(loyalty > 0) {
                text::add_to_layout_box(state, contents, box, text::fp_three_places{loyalty * 100}, text::text_color::green);
            } else if(loyalty == 0){
                text::localised_format_box(state, contents, box, std::string_view("party_loyalty_desc_neutral"));
            }
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

	if(prov.value < state.province_definitions.first_sea_province.value) {
        auto box = text::open_layout_box(contents);
        text::localised_single_sub_box(state, contents, box, std::string_view("no_migrations_found1_tooltip"), text::variable_type::country, dcon::fatten(state.world, prov).get_nation_from_province_ownership().id);
        text::add_line_break_to_layout_box(state, contents, box);
        text::add_line_break_to_layout_box(state, contents, box);
        text::localised_format_box(state, contents, box, std::string_view("no_migrations_found2_tooltip"));
        text::add_line_break_to_layout_box(state, contents, box);
        text::localised_format_box(state, contents, box, std::string_view("no_migrations_found3_tooltip"));
        text::close_layout_box(contents, box);
    } else {
        country_name_box(state, contents, prov);
    }
}

void civilsationlevel_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {    //Done 
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
                text::add_to_layout_box(state, contents, box, text::format_percentage(civpro ,2), text::text_color::green);
			    // ???
			    break;
            }
		    case(nations::status::uncivilized):
            {
			    text::localised_format_box(state, contents, box, std::string_view("diplomacy_uncivilizednation_status"));
                text::add_line_break_to_layout_box(state, contents, box);
                auto civpro = state.world.nation_get_modifier_values(fat.get_nation_from_province_ownership().id, sys::national_mod_offsets::civilization_progress_modifier);
                text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, std::string_view("modifier_civilization_progress")) + ": ");
                text::add_to_layout_box(state, contents, box, text::format_percentage(civpro ,2), text::text_color::green);
			    // Nothing
			    break;
            }
		    case(nations::status::primitive):
            {
			    text::localised_format_box(state, contents, box, std::string_view("diplomacy_primitivenation_status"));
                text::add_line_break_to_layout_box(state, contents, box);
                auto civpro = state.world.nation_get_modifier_values(fat.get_nation_from_province_ownership().id, sys::national_mod_offsets::civilization_progress_modifier);
                text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, std::string_view("modifier_civilization_progress")) + ": ");
                text::add_to_layout_box(state, contents, box, text::format_percentage(civpro ,2), text::text_color::green);
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
            && dcon::fatten(state.world, state.local_player_nation) != fat.get_nation_from_province_ownership()){
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
        text::localised_single_sub_box(state, contents, box, std::string_view("flashpoint_tension"), text::variable_type::value, text::format_percentage(fat.get_state_membership().get_flashpoint_tension(), 0));
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
        if(fat.get_naval_base_level() == 0) {
            dcon::province_id navalprov{};
            for(auto p : fat.get_state_from_abstract_state_membership().get_abstract_state_membership()) {
                if(dcon::fatten(state.world, p).get_province().get_naval_base_level() != 0) {
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
            text::localised_single_sub_box(state, contents, box, std::string_view("mapmode_naval_tooltip_level"), text::variable_type::lvl, fat.get_naval_base_level());
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

void debug_map_tt_box(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
    auto fat = dcon::fatten(state.world, prov);

    auto box = text::open_layout_box(contents);
    text::add_divider_to_layout_box(state, contents, box);
    {
        text::add_to_layout_box(state, contents, box, std::string_view("Nation: "));
        text::add_to_layout_box(state, contents, box, fat.get_nation_from_province_ownership(), text::text_color::yellow);
        text::add_to_layout_box(state, contents, box, std::string_view(" ("));
        text::add_to_layout_box(state, contents, box, fat.get_nation_from_province_ownership().id.value - 1);
        text::add_to_layout_box(state, contents, box, std::string_view(" | "));
        text::add_to_layout_box(state, contents, box, std::string_view(nations::int_to_tag(state.world.national_identity_get_identifying_int(fat.get_nation_from_province_ownership().get_identity_from_identity_holder().id))));
        text::add_to_layout_box(state, contents, box, std::string_view(")"));
    }
    text::add_line_break_to_layout_box(state, contents, box);
    {
        text::add_to_layout_box(state, contents, box, prov, text::text_color::yellow);
        text::add_to_layout_box(state, contents, box, std::string_view(" ("));
        text::add_to_layout_box(state, contents, box, prov.value);
        text::add_to_layout_box(state, contents, box, std::string_view(")"));
    }
    text::add_line_break_to_layout_box(state, contents, box);
    if(fat.get_nation_from_province_ownership().is_valid()) {
        text::add_to_layout_box(state, contents, box, std::string_view("Military Gas Tech: "));
        if(fat.get_nation_from_province_ownership().get_has_gas_attack()) {
            text::add_to_layout_box(state, contents, box, std::string_view("ATK"), text::text_color::green);
        } else {
            text::add_to_layout_box(state, contents, box, std::string_view("ATK"), text::text_color::red);
        }
        text::add_to_layout_box(state, contents, box, std::string_view("/"), text::text_color::white);
        if(fat.get_nation_from_province_ownership().get_has_gas_defense()) {
            text::add_to_layout_box(state, contents, box, std::string_view("DEF"), text::text_color::green);
        } else {
            text::add_to_layout_box(state, contents, box, std::string_view("DEF"), text::text_color::red);
        }
    }
    text::close_layout_box(contents, box);
}

void populate_map_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
    switch(state.map_state.active_map_mode) {
        case map_mode::mode::terrain:
            terrain_map_tt_box(state, contents, prov);
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
            partyloyalty_map_tt_box(state, contents, prov);
            break;
        case map_mode::mode::rank:
            rank_map_tt_box(state, contents, prov);
            break;
        case map_mode::mode::migration:
            migration_map_tt_box(state, contents, prov);
            break;
        case map_mode::mode::civilization_level:
            civilsationlevel_map_tt_box(state, contents, prov);
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
        default:
            break;
    };
    if(state.use_debug_mode) {
        debug_map_tt_box(state, contents, prov);
    }
}

}
