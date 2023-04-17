#pragma once

#include "gui_element_types.hpp"

namespace ui {
template<class TypeId>
class military_unit_name_text : public simple_text_element_base {
protected:
	TypeId obj_id{};
public:
	void on_update(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, obj_id);
		set_text(state, std::string{ state.to_string_view(fat_id.get_name()) });
	}
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<TypeId>()) {
			obj_id = any_cast<TypeId>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};
template<class TypeId>
class military_unit_entry : public listbox_row_element_base<TypeId> {
    simple_text_element_base* unit_name = nullptr;
    image_element_base* unit_icon = nullptr;
    image_element_base* leader_icon = nullptr;
    button_element_base* cancel_button = nullptr;
    simple_text_element_base* eta_date_text = nullptr;
    simple_text_element_base* location_text = nullptr;
    progress_bar* unit_progress = nullptr;
    simple_text_element_base* unit_regiments_text = nullptr;
    simple_text_element_base* unit_men_text = nullptr;
    progress_bar* unit_morale_progress = nullptr;
    progress_bar* unit_strength_progress = nullptr;
    image_element_base* unit_moving_icon = nullptr;
    image_element_base* unit_digin_icon = nullptr;
    image_element_base* unit_combat_icon = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			auto ptr = make_element_by_type<military_unit_name_text<TypeId>>(state, id);
            unit_name = ptr.get();
            return ptr;
		} else if(name == "leader") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
            leader_icon = ptr.get();
            return ptr;
        } else if(name == "unit_strip") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
            unit_icon = ptr.get();
            return ptr;
        } else if(name == "military_cancel_unit") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
            cancel_button = ptr.get();
            return ptr;
        } else if(name == "unit_eta") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            eta_date_text = ptr.get();
            return ptr;
        } else if(name == "location") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            location_text = ptr.get();
            return ptr;
        } else if(name == "unit_progress") {
			auto ptr = make_element_by_type<progress_bar>(state, id);
            unit_progress = ptr.get();
            return ptr;
        } else if(name == "morale_progress") {
			auto ptr = make_element_by_type<standard_unit_progress_bar>(state, id);
            unit_morale_progress = ptr.get();
            return ptr;
        } else if(name == "strength_progress") {
			auto ptr = make_element_by_type<standard_unit_progress_bar>(state, id);
            unit_strength_progress = ptr.get();
            return ptr;
        } else if(name == "regiments") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            unit_regiments_text = ptr.get();
            return ptr;
        } else if(name == "men") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            unit_men_text = ptr.get();
            return ptr;
        } else if(name == "moving") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
            unit_moving_icon = ptr.get();
            return ptr;
        } else if(name == "digin") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
            unit_digin_icon = ptr.get();
            return ptr;
        } else if(name == "combat") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
            unit_combat_icon = ptr.get();
            return ptr;
		} else {
			return nullptr;
		}
	}
	void update(sys::state& state) noexcept override {
        bool is_building = false;
        bool is_moving = false;
        bool is_digin = false;
        bool is_combat = false;
        unit_icon->set_visible(state, is_building);
        cancel_button->set_visible(state, is_building);
        eta_date_text->set_visible(state, is_building);
        location_text->set_visible(state, is_building);
        unit_progress->set_visible(state, is_building);

        const auto& content = listbox_row_element_base<TypeId>::content;

        if(!is_building) {
            auto regiments = 0;
            auto strength = 0.f;
            auto full_strength = 0.f;
            // Armies
            if constexpr(std::is_same_v<TypeId, dcon::army_id>) {
                state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id amid) {
                    auto rid = state.world.army_membership_get_regiment(amid);
                    full_strength += 1.f * state.defines.pop_size_per_regiment;
                    strength += state.world.regiment_get_strength(rid) * state.defines.pop_size_per_regiment;
                    ++regiments;
                });
            }
            // Navies
            if constexpr(std::is_same_v<TypeId, dcon::navy_id>) {
                state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
                    auto sid = state.world.navy_membership_get_ship(nmid);
                    full_strength += 1.f;
                    strength += state.world.ship_get_strength(sid);
                    ++regiments;
                });
            }
            unit_strength_progress->progress = (strength && full_strength) ? strength / full_strength : 0.f;
            unit_men_text->set_text(state, text::prettify(int32_t(strength)));
            unit_regiments_text->set_text(state, std::to_string(regiments));
        }
        unit_name->set_visible(state, !is_building);
        leader_icon->set_visible(state, !is_building);
        unit_regiments_text->set_visible(state, !is_building);
        unit_men_text->set_visible(state, !is_building);
        unit_morale_progress->set_visible(state, !is_building);
        unit_strength_progress->set_visible(state, !is_building);
        unit_moving_icon->set_visible(state, !is_building && is_moving);
        unit_digin_icon->set_visible(state, !is_building && is_digin);
        unit_combat_icon->set_visible(state, !is_building && is_combat);

		Cyto::Any payload = content;
        listbox_row_element_base<TypeId>::impl_set(state, payload);
	}
};

template<class TypeId>
class military_units_listbox : public listbox_element_base<military_unit_entry<TypeId>, TypeId> {
protected:
	std::string_view get_row_element_name() override {
        return "unit_entry";
    }
public:
	void on_update(sys::state& state) noexcept override {
        auto& row_contents = listbox_element_base<military_unit_entry<TypeId>, TypeId>::row_contents;
        row_contents.clear();
        // Armies
        if constexpr(std::is_same_v<TypeId, dcon::army_id>) {
            state.world.nation_for_each_army_control_as_controller(state.local_player_nation, [&](dcon::army_control_id acid) {
                auto aid = state.world.army_control_get_army(acid);
                row_contents.push_back(aid);
            });
        // Navies
        }
        if constexpr(std::is_same_v<TypeId, dcon::navy_id>) {
            state.world.nation_for_each_navy_control_as_controller(state.local_player_nation, [&](dcon::navy_control_id ncid) {
                auto nid = state.world.navy_control_get_navy(ncid);
                row_contents.push_back(nid);
            });
        }
        listbox_element_base<military_unit_entry<TypeId>, TypeId>::update(state);
	}
};

class military_armies_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
        int32_t count = 0;
		state.world.nation_for_each_army_control_as_controller(state.local_player_nation, [&](dcon::army_control_id acid) {
			++count;
		});
		auto contents = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black }
		);
        auto box = text::open_layout_box(contents);
        text::substitution_map sub{};
        text::add_to_substitution_map(sub, text::variable_type::value, std::to_string(count));
        text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "military_army_count_tooltip"), text::text_color::white);
		text::close_layout_box(contents, box);
	}
};

class military_navies_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
        int32_t count = 0;
		state.world.nation_for_each_navy_control_as_controller(state.local_player_nation, [&](dcon::navy_control_id acid) {
			++count;
		});
		auto contents = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black }
		);
        auto box = text::open_layout_box(contents);
        text::substitution_map sub{};
        text::add_to_substitution_map(sub, text::variable_type::value, std::to_string(count));
        text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "military_navy_count_tooltip"), text::text_color::white);
		text::close_layout_box(contents, box);
	}
};

template<class TypeId>
class military_units_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_listbox") {
            return make_element_by_type<military_units_listbox<TypeId>>(state, id);
		} else if(name == "current_count") {
            if constexpr(std::is_same_v<TypeId, dcon::army_id>)
                return make_element_by_type<military_armies_text>(state, id);
            else
                return make_element_by_type<military_navies_text>(state, id);
        } else {
			return nullptr;
		}
	}
};

}
