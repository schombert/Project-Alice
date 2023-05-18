#pragma once

#include <variant>
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

enum class outliner_filter : uint8_t {
    rebel_occupations,
    sieges,
    hostile_sieges,
    combat,
    naval_combat,
    armies,
    navies,
    factories,
    buildings,
    army_construction,
    navy_construction,
    gp_influence,
    national_focus,
    rally_points,
    count
};

typedef std::variant<
    outliner_filter,
    dcon::army_id,
    dcon::navy_id,
    dcon::gp_relationship_id,
    dcon::state_building_construction_id,
    dcon::province_building_construction_id,
    dcon::province_land_construction_id,
    dcon::province_naval_construction_id
> outliner_data;

class outliner_element_button : public generic_settable_element<button_element_base, outliner_data> {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
        if(std::holds_alternative<dcon::army_id>(content)) {
            auto aid = std::get<dcon::army_id>(content);

            auto box = text::open_layout_box(contents, 0);
		    text::substitution_map sub{};
            text::add_to_substitution_map(sub, text::variable_type::army_name, state.to_string_view(state.world.army_get_name(aid)));
            text::add_to_substitution_map(sub, text::variable_type::location, state.world.army_location_get_location(state.world.army_get_army_location(aid)));
            text::localised_format_box(state, contents, box, std::string_view("ol_armies_tooltip"), sub);
            text::close_layout_box(contents, box);
        } else if(std::holds_alternative<dcon::navy_id>(content)) {
            auto nid = std::get<dcon::navy_id>(content);

            auto box = text::open_layout_box(contents, 0);
		    text::substitution_map sub{};
            text::add_to_substitution_map(sub, text::variable_type::navy_name, state.to_string_view(state.world.navy_get_name(nid)));
            text::add_to_substitution_map(sub, text::variable_type::location, state.world.navy_location_get_location(state.world.navy_get_navy_location(nid)));
            text::localised_format_box(state, contents, box, std::string_view("ol_navies_tooltip"), sub);
            text::close_layout_box(contents, box);
        } else if(std::holds_alternative<dcon::gp_relationship_id>(content)) {
            auto grid = std::get<dcon::gp_relationship_id>(content);
            auto nid = state.world.gp_relationship_get_influence_target(grid);

        } else if(std::holds_alternative<dcon::state_building_construction_id>(content)) {
            auto sbcid = std::get<dcon::state_building_construction_id>(content);
            auto ftid = state.world.state_building_construction_get_type(sbcid);
            
        } else if(std::holds_alternative<dcon::province_building_construction_id>(content)) {
            auto pbcid = std::get<dcon::province_building_construction_id>(content);
            auto btid = state.world.province_building_construction_get_type(pbcid);
            auto name = province_building_type_get_name(economy::province_building_type(btid));
            
        } else if(std::holds_alternative<dcon::province_land_construction_id>(content)) {
            auto plcid = std::get<dcon::province_land_construction_id>(content);
            auto utid = state.world.province_land_construction_get_type(plcid);
            auto name = state.military_definitions.unit_base_definitions[utid].name;
            
        } else if(std::holds_alternative<dcon::province_naval_construction_id>(content)) {
            auto pncid = std::get<dcon::province_naval_construction_id>(content);
            auto utid = state.world.province_naval_construction_get_type(pncid);
            auto name = state.military_definitions.unit_base_definitions[utid].name;
            
        }
    }
};

class outliner_element : public listbox_row_element_base<outliner_data> {
    image_element_base* header_bg = nullptr;
    simple_text_element_base* header_text = nullptr;
    image_element_base* standard_bg = nullptr;
    image_element_base* combat_icon = nullptr;
    image_element_base* moving_icon = nullptr;
    image_element_base* rally_land_icon = nullptr;
    image_element_base* rally_navy_icon = nullptr;
    simple_text_element_base* entry_text = nullptr;
    simple_text_element_base* info_text = nullptr;

    static std::string_view get_filter_text_key(outliner_filter f) noexcept {
        switch(f) {
        case outliner_filter::rebel_occupations:
            return "ol_rebel_occupations";
        case outliner_filter::sieges:
            return "ol_sieges";
        case outliner_filter::hostile_sieges:
            return "ol_hostile_sieges";
        case outliner_filter::combat:
            return "ol_combat";
        case outliner_filter::naval_combat:
            return "ol_naval_combat";
        case outliner_filter::armies:
            return "ol_armies";
        case outliner_filter::navies:
            return "ol_navies";
        case outliner_filter::factories:
            return "ol_statebuilding_construction";
        case outliner_filter::buildings:
            return "ol_province_construction";
        case outliner_filter::army_construction:
            return "ol_army_construction";
        case outliner_filter::navy_construction:
            // Yes it's mispelt like this in the original game - get lucky
            return "ol_navy_construciton";
        case outliner_filter::gp_influence:
            return "ol_gp_influence";
        case outliner_filter::national_focus:
            return "ol_view_natfocus_header";
        case outliner_filter::rally_points:
            return "ol_view_rallypoints_header";
        default:
            return "???";
        }
    }
public:
    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "outliner_header") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            header_bg = ptr.get();
            return ptr;
        } else if(name == "header_text") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            header_text = ptr.get();
            return ptr;
        } else if(name == "outliner_standard") {
            auto ptr = make_element_by_type<outliner_element_button>(state, id);
            standard_bg = ptr.get();
            return ptr;
        } else if(name == "combat_icon") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            combat_icon = ptr.get();
            return ptr;
        } else if(name == "moving_icon") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            moving_icon = ptr.get();
            return ptr;
        } else if(name == "rally_land_icon") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            rally_land_icon = ptr.get();
            return ptr;
        } else if(name == "rally_navy_icon") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            rally_navy_icon = ptr.get();
            return ptr;
        } else if(name == "entry_text") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            entry_text = ptr.get();
            return ptr;
        } else if(name == "info_text") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            info_text = ptr.get();
            return ptr;
        } else {
            return nullptr;
        }
    }

    void update(sys::state& state) noexcept override {
        combat_icon->set_visible(state, false);
        moving_icon->set_visible(state, false);
        rally_land_icon->set_visible(state, false);
        rally_navy_icon->set_visible(state, false);
        if(std::holds_alternative<outliner_filter>(content)) {
            header_bg->set_visible(state, true);
            header_text->set_visible(state, true);
            standard_bg->set_visible(state, false);
            entry_text->set_visible(state, false);
            info_text->set_visible(state, false);

            auto filter = std::get<outliner_filter>(content);
            auto text = text::produce_simple_string(state, get_filter_text_key(filter));
            header_text->set_text(state, text);
        } else {
            header_bg->set_visible(state, false);
            header_text->set_visible(state, false);
            standard_bg->set_visible(state, true);
            entry_text->set_visible(state, true);
            info_text->set_visible(state, true);

            Cyto::Any payload = content;
            standard_bg->impl_set(state, payload);

            if(std::holds_alternative<dcon::army_id>(content)) {
                auto aid = std::get<dcon::army_id>(content);
                info_text->set_text(state, std::string{ state.to_string_view(state.world.army_get_name(aid)) });

                auto strength = 0.f;
                state.world.army_for_each_army_membership_as_army(aid, [&](dcon::army_membership_id amid) {
                    auto rid = state.world.army_membership_get_regiment(amid);
                    strength += state.world.regiment_get_strength(rid) * state.defines.pop_size_per_regiment;
                });
                entry_text->set_text(state, text::prettify(int32_t(strength)));
            } else if(std::holds_alternative<dcon::navy_id>(content)) {
                auto nid = std::get<dcon::navy_id>(content);
                info_text->set_text(state, std::string{ state.to_string_view(state.world.navy_get_name(nid)) });
                // Navy membership
                auto strength = 0.f;
                state.world.navy_for_each_navy_membership_as_navy(nid, [&](dcon::navy_membership_id nmid) {
                    auto sid = state.world.navy_membership_get_ship(nmid);
                    strength += state.world.ship_get_strength(sid);
                });
                entry_text->set_text(state, text::prettify(int32_t(strength)));
            } else if(std::holds_alternative<dcon::gp_relationship_id>(content)) {
                auto grid = std::get<dcon::gp_relationship_id>(content);
                auto nid = state.world.gp_relationship_get_influence_target(grid);
                info_text->set_text(state, text::produce_simple_string(state, state.world.nation_get_name(nid)));
                auto status = state.world.gp_relationship_get_status(grid);
                entry_text->set_text(state, text::produce_simple_string(state, nation_player_opinion_text::get_level_text_key(status)));
            } else if(std::holds_alternative<dcon::state_building_construction_id>(content)) {
                auto sbcid = std::get<dcon::state_building_construction_id>(content);
                auto ftid = state.world.state_building_construction_get_type(sbcid);
                info_text->set_text(state, text::produce_simple_string(state, state.world.factory_type_get_name(ftid)));
                // TODO: Entry displays time left to build factory
                entry_text->set_text(state, "");
            } else if(std::holds_alternative<dcon::province_building_construction_id>(content)) {
                auto pbcid = std::get<dcon::province_building_construction_id>(content);
                auto btid = state.world.province_building_construction_get_type(pbcid);
                auto name = province_building_type_get_name(economy::province_building_type(btid));
                info_text->set_text(state, text::produce_simple_string(state, name));
                // TODO: Entry displays time left to build building
                entry_text->set_text(state, "");
            } else if(std::holds_alternative<dcon::province_land_construction_id>(content)) {
                auto plcid = std::get<dcon::province_land_construction_id>(content);
                auto utid = state.world.province_land_construction_get_type(plcid);
                auto name = state.military_definitions.unit_base_definitions[utid].name;
                info_text->set_text(state, text::produce_simple_string(state, name));
                // TODO: Entry displays time left to build building
                entry_text->set_text(state, "");
            } else if(std::holds_alternative<dcon::province_naval_construction_id>(content)) {
                auto pncid = std::get<dcon::province_naval_construction_id>(content);
                auto utid = state.world.province_naval_construction_get_type(pncid);
                auto name = state.military_definitions.unit_base_definitions[utid].name;
                info_text->set_text(state, text::produce_simple_string(state, name));
                // TODO: Entry displays time left to build building
                entry_text->set_text(state, "");
            }
        }
    }
};

class outliner_listbox : public listbox_element_base<outliner_element, outliner_data> {
protected:
    std::string_view get_row_element_name() override {
        return "outliner_entry";
    }

    bool get_filter(sys::state& state, outliner_filter filter) noexcept {
        if(parent) {
            Cyto::Any payload = filter;
            parent->impl_get(state, payload);
            return any_cast<bool>(payload);
        }
		return false;
    }
public:
    void on_update(sys::state& state) noexcept override {
        row_contents.clear();
        // TODO: rebel_occupations,
        if(get_filter(state, outliner_filter::rebel_occupations)) {
            row_contents.push_back(outliner_filter::rebel_occupations);
            auto old_size = row_contents.size();
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        // TODO: sieges,
        if(get_filter(state, outliner_filter::sieges)) {
            row_contents.push_back(outliner_filter::sieges);
            auto old_size = row_contents.size();
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        // TODO: hostile_sieges,
        if(get_filter(state, outliner_filter::hostile_sieges)) {
            row_contents.push_back(outliner_filter::hostile_sieges);
            auto old_size = row_contents.size();
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        // TODO: combat,
        if(get_filter(state, outliner_filter::combat)) {
            row_contents.push_back(outliner_filter::combat);
            auto old_size = row_contents.size();
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        // TODO: naval_combat,
        if(get_filter(state, outliner_filter::naval_combat)) {
            row_contents.push_back(outliner_filter::naval_combat);
            auto old_size = row_contents.size();
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        if(get_filter(state, outliner_filter::armies)) {
            row_contents.push_back(outliner_filter::armies);
            auto old_size = row_contents.size();
            state.world.nation_for_each_army_control_as_controller(state.local_player_nation, [&](dcon::army_control_id acid) {
                auto aid = state.world.army_control_get_army(acid);
                row_contents.push_back(aid);
            });
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        if(get_filter(state, outliner_filter::navies)) {
            row_contents.push_back(outliner_filter::navies);
            auto old_size = row_contents.size();
            state.world.nation_for_each_navy_control_as_controller(state.local_player_nation, [&](dcon::navy_control_id ncid) {
                auto nid = state.world.navy_control_get_navy(ncid);
                row_contents.push_back(nid);
            });
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        if(get_filter(state, outliner_filter::factories)) {
            row_contents.push_back(outliner_filter::factories);
            auto old_size = row_contents.size();
            state.world.nation_for_each_state_building_construction(state.local_player_nation, [&](dcon::state_building_construction_id sbcid) {
                row_contents.push_back(sbcid);
            });
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        if(get_filter(state, outliner_filter::buildings)) {
            row_contents.push_back(outliner_filter::buildings);
            auto old_size = row_contents.size();
            state.world.nation_for_each_province_building_construction(state.local_player_nation, [&](dcon::province_building_construction_id pbcid) {
                row_contents.push_back(pbcid);
            });
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        if(get_filter(state, outliner_filter::army_construction)) {
            row_contents.push_back(outliner_filter::army_construction);
            auto old_size = row_contents.size();
            state.world.nation_for_each_province_land_construction(state.local_player_nation, [&](dcon::province_land_construction_id plcid) {
                row_contents.push_back(plcid);
            });
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        if(get_filter(state, outliner_filter::navy_construction)) {
            row_contents.push_back(outliner_filter::navy_construction);
            auto old_size = row_contents.size();
            state.world.nation_for_each_province_naval_construction(state.local_player_nation, [&](dcon::province_naval_construction_id pncid) {
                row_contents.push_back(pncid);
            });
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        // TODO: gp_influence,
        if(get_filter(state, outliner_filter::gp_influence)) {
            row_contents.push_back(outliner_filter::gp_influence);
            auto old_size = row_contents.size();
            state.world.nation_for_each_gp_relationship_as_great_power(state.local_player_nation, [&](dcon::gp_relationship_id grid) {
                row_contents.push_back(grid);
            });
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        // TODO: national_focus,
        if(get_filter(state, outliner_filter::national_focus)) {
            row_contents.push_back(outliner_filter::national_focus);
            auto old_size = row_contents.size();
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }
        // TODO: rally_points
        if(get_filter(state, outliner_filter::rally_points)) {
            row_contents.push_back(outliner_filter::rally_points);
            auto old_size = row_contents.size();
            if(old_size == row_contents.size())
                row_contents.pop_back();
        }

        update(state);
    }
};

class outliner_minmax_button : public button_element_base {
public:
};

template<outliner_filter Filter>
class outliner_filter_checkbox : public checkbox_button {
    static std::string_view get_filter_text_key(outliner_filter f) noexcept {
        switch(f) {
        case outliner_filter::rebel_occupations:
            return "ol_view_rebel_occupations";
        case outliner_filter::sieges:
            return "remove_ol_view_sieges";
        case outliner_filter::hostile_sieges:
            return "remove_ol_view_hostile_sieges";
        case outliner_filter::combat:
            return "ol_view_combat";
        case outliner_filter::naval_combat:
            return "ol_view_navalcombat";
        case outliner_filter::armies:
            return "ol_view_armies";
        case outliner_filter::navies:
            return "ol_view_navies";
        case outliner_filter::factories:
            return "ol_view_factories";
        case outliner_filter::buildings:
            return "ol_view_buildings";
        case outliner_filter::army_construction:
            return "ol_view_army_construction";
        case outliner_filter::navy_construction:
            return "ol_view_navy_construction";
        case outliner_filter::gp_influence:
            return "ol_view_gp_influence";
        case outliner_filter::national_focus:
            return "ol_view_natfocus";
        case outliner_filter::rally_points:
            return "ol_view_rallypoints";
        default:
            return "???";
        }
    }
public:
	bool is_active(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = Filter;
            parent->impl_get(state, payload);
            return any_cast<bool>(payload);
        }
		return false;
	}

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = Filter;
            parent->impl_set(state, payload);
            state.ui_state.outliner_window->impl_on_update(state);
        }
    }

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto name = get_filter_text_key(Filter);
        if(auto k = state.key_to_text_sequence.find(name); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{ });
			text::close_layout_box(contents, box);
		}
	}
};

class outliner_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(state.ui_state.outliner_window)
			frame = state.ui_state.outliner_window->is_visible() ? 1 : 0;
	}

	void button_action(sys::state& state) noexcept override {
		if(state.ui_state.outliner_window) {
			state.ui_state.outliner_window->set_visible(state, !state.ui_state.outliner_window->is_visible());
			on_update(state);
		}
	}
};

class outliner_window : public window_element_base {
    outliner_listbox* listbox = nullptr;
    image_element_base* bottom_image = nullptr;
public:
    void on_create(sys::state& state) noexcept override {
        window_element_base::on_create(state);

        auto ptr = make_element_by_type<image_element_base>(state, state.ui_state.defs_by_name.find("outliner_bottom")->second.definition);
        ptr->set_visible(state, true);
        bottom_image = ptr.get();
        add_child_to_front(std::move(ptr));
    }

    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "outliner_top") {
            return make_element_by_type<outliner_minmax_button>(state, id);
        } else if(name == "outliner_list") {
            auto ptr = make_element_by_type<outliner_listbox>(state, id);
            listbox = ptr.get();
            return ptr;
        } else if(name == "outliner_view_rebel_occupations") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::rebel_occupations>>(state, id);
        } else if(name == "outliner_view_sieges") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::sieges>>(state, id);
        } else if(name == "outliner_view_hostile_sieges") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::hostile_sieges>>(state, id);
        } else if(name == "outliner_view_combat") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::combat>>(state, id);
        } else if(name == "outliner_view_navalcombat") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::naval_combat>>(state, id);
        } else if(name == "outliner_view_armies") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::armies>>(state, id);
        } else if(name == "outliner_view_navies") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::navies>>(state, id);
        } else if(name == "outliner_view_factories") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::factories>>(state, id);
        } else if(name == "outliner_view_buildings") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::buildings>>(state, id);
        } else if(name == "outliner_view_army_construction") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::army_construction>>(state, id);
        } else if(name == "outliner_view_navy_construction") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::navy_construction>>(state, id);
        } else if(name == "outliner_view_gp_influence") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::gp_influence>>(state, id);
        } else if(name == "outliner_view_natfocus") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::national_focus>>(state, id);
        } else if(name == "outliner_view_rallypoints") {
            return make_element_by_type<outliner_filter_checkbox<outliner_filter::rally_points>>(state, id);
        } else {
            return nullptr;
        }
    }

    void on_update(sys::state& state) noexcept override {
        bottom_image->base_data.position.y = listbox->base_data.position.y;
        uint32_t offset = uint32_t(listbox->row_contents.size()) * 16;
        if(offset >= uint32_t(listbox->base_data.size.y))
            offset = uint32_t(listbox->base_data.size.y);
        bottom_image->base_data.position.y += int16_t(offset);
    }

    message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
        if(payload.holds_type<outliner_filter>()) {
            auto filter = any_cast<outliner_filter>(payload);

            state.user_settings.outliner_views[uint8_t(filter)] = !state.user_settings.outliner_views[uint8_t(filter)];
            state.save_user_settings();

            listbox->on_update(state);
            return message_result::consumed;
        }
        return message_result::unseen;
    }

    message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
        if(payload.holds_type<outliner_filter>()) {
            auto filter = any_cast<outliner_filter>(payload);
            payload.emplace<bool>(state.user_settings.outliner_views[uint8_t(filter)]);
            return message_result::consumed;
        }
        return message_result::unseen;
    }
};
}
