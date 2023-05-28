#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

template<class T>
class unit_selection_panel : public window_element_base {
public:
    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "unitpanel_bg") {
            return make_element_by_type<draggable_target>(state, id);
        } else if(name == "leader_prestige_icon") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "leader_prestige_bar") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "prestige_bar_frame") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "unitname") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "only_unit_from_selection_button") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "remove_unit_from_selection_button") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "newunitbutton") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "splitinhalf") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "disbandbutton") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "disbandtoosmallbutton") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "str_bar") {
            return make_element_by_type<vertical_progress_bar>(state, id);
        } else if(name == "org_bar") {
            return make_element_by_type<vertical_progress_bar>(state, id);
        } else if(name == "unitattrition_icon") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "unitattrition") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "unitstrength") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "unitlocation") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "unit_location_button") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "unitleader") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "leader_button") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "unit_activity") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "leader_photo") {
            return make_element_by_type<image_element_base>(state, id);
        } else {
            return nullptr;
        }
    }
};

template<class T, std::size_t N>
class unit_details_type_item : public window_element_base {
public:
    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "unit_icon") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            if constexpr(std::is_same_v<T, dcon::army_id>) {
                ptr->frame = N;
            } else {
                ptr->frame = 3 + N;
            }
            return ptr;
		} else {
            return nullptr;
        }
    }
};

template<class T>
class subunit_details_name : public simple_text_element_base {
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = T{};
            parent->impl_get(state, payload);
            T content = any_cast<T>(payload);
            std::string str{ state.to_string_view(dcon::fatten(state.world, content).get_name()) };
            set_text(state, str);
        }
    }
};

template<class T>
class subunit_details_type_text : public simple_text_element_base {
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = T{};
            parent->impl_get(state, payload);
            T content = any_cast<T>(payload);
            dcon::unit_type_id utid = dcon::fatten(state.world, content).get_type();
            set_text(state, text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name));
        }
    }
};

template<class T>
class subunit_details_type_icon : public image_element_base {
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = T{};
            parent->impl_get(state, payload);
            T content = any_cast<T>(payload);
            dcon::unit_type_id utid = dcon::fatten(state.world, content).get_type();
            frame = state.military_definitions.unit_base_definitions[utid].icon - 1;
        }
    }
};

template<class T>
class subunit_organisation_progress_bar : public vertical_progress_bar {
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = T{};
            parent->impl_get(state, payload);
            // TODO: Organisation
            //T content = any_cast<T>(payload);
            progress = 0.5f;
        }
    }
};

template<class T>
class subunit_strength_progress_bar : public vertical_progress_bar {
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = T{};
            parent->impl_get(state, payload);
            // TODO: Strength
            //T content = any_cast<T>(payload);
            progress = 1.f;
        }
    }
};

class subunit_details_regiment_amount : public simple_text_element_base {
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::regiment_id{};
            parent->impl_get(state, payload);
            dcon::regiment_id content = any_cast<dcon::regiment_id>(payload);
            set_text(state, text::prettify(int32_t(state.world.regiment_get_strength(content) * state.defines.pop_size_per_regiment)));
        }
    }
};
class subunit_details_ship_amount : public simple_text_element_base {
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::ship_id{};
            parent->impl_get(state, payload);
            dcon::ship_id content = any_cast<dcon::ship_id>(payload);
            set_text(state, text::format_percentage(state.world.ship_get_strength(content), 0));
        }
    }
};

template<class T>
class subunit_details_entry : public listbox_row_element_base<T> {
public:
    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "select") {
            return make_element_by_type<button_element_base>(state, id);
        } else if(name == "select_naval") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "sunit_icon") {
            return make_element_by_type<subunit_details_type_icon<T>>(state, id);
        } else if(name == "subunit_name") {
            return make_element_by_type<subunit_details_name<T>>(state, id);
        } else if(name == "subunit_type") {
            return make_element_by_type<subunit_details_type_text<T>>(state, id);
        } else if(name == "subunit_amount") {
            if constexpr(std::is_same_v<T, dcon::regiment_id>) {
                return make_element_by_type<subunit_details_regiment_amount>(state, id);
            } else {
                auto ptr = make_element_by_type<simple_text_element_base>(state, id);
                ptr->set_visible(state, false);
                return ptr;
            }
        } else if(name == "subunit_amount_naval") {
            if constexpr(std::is_same_v<T, dcon::ship_id>) {
                return make_element_by_type<subunit_details_ship_amount>(state, id);
            } else {
                auto ptr = make_element_by_type<simple_text_element_base>(state, id);
                ptr->set_visible(state, false);
                return ptr;
            }
        } else if(name == "connected_pop") {
            auto ptr = make_element_by_type<button_element_base>(state, id);
            ptr->set_visible(state, false);
            return ptr;
        } else if(name == "rebel_faction") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            ptr->set_visible(state, false);
            return ptr;
        } else if(name == "unit_experience") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "org_bar") {
            return make_element_by_type<subunit_organisation_progress_bar<T>>(state, id);
        } else if(name == "str_bar") {
            return make_element_by_type<subunit_strength_progress_bar<T>>(state, id);
        } else {
            return nullptr;
        }
    }

    void update(sys::state& state) noexcept override {

    }
};
class unit_details_army_listbox : public listbox_element_base<subunit_details_entry<dcon::regiment_id>, dcon::regiment_id> {
protected:
    std::string_view get_row_element_name() override {
        return "subunit_entry";
    }
public:
    void on_update(sys::state& state) noexcept override {
        row_contents.clear();
        if(parent) {
            Cyto::Any payload = dcon::army_id{};
            parent->impl_get(state, payload);
            auto content = any_cast<dcon::army_id>(payload);
            state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id amid) {
                auto rid = state.world.army_membership_get_regiment(amid);
                row_contents.push_back(rid);
            });
        }
        update(state);
    }
};
class unit_details_navy_listbox : public listbox_element_base<subunit_details_entry<dcon::ship_id>, dcon::ship_id> {
protected:
    std::string_view get_row_element_name() override {
        return "subunit_entry";
    }
public:
    void on_update(sys::state& state) noexcept override {
        row_contents.clear();
        if(parent) {
            Cyto::Any payload = dcon::navy_id{};
            parent->impl_get(state, payload);
            auto content = any_cast<dcon::navy_id>(payload);
            state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
                auto sid = state.world.navy_membership_get_ship(nmid);
                row_contents.push_back(sid);
            });
        }
        update(state);
    }
};

class unit_details_load_button : public button_element_base {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::army_id{};
            parent->impl_get(state, payload);
            //auto content = any_cast<dcon::army_id>(payload);
        }
    }
};
class unit_details_unload_button : public button_element_base {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::army_id{};
            parent->impl_get(state, payload);
            //auto content = any_cast<dcon::army_id>(payload);
        }
    }
};
class unit_details_enable_rebel_button : public button_element_base {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::army_id{};
            parent->impl_get(state, payload);
            //auto content = any_cast<dcon::army_id>(payload);
        }
    }
};
class unit_details_disable_rebel_button : public button_element_base {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::army_id{};
            parent->impl_get(state, payload);
            //auto content = any_cast<dcon::army_id>(payload);
        }
    }
};
class unit_details_select_land_button : public button_element_base {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::navy_id{};
            parent->impl_get(state, payload);
            //auto content = any_cast<dcon::navy_id>(payload);
        }
    }
};
template<class T>
class unit_details_attach_button : public button_element_base {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = T{};
            parent->impl_get(state, payload);
            //auto content = any_cast<T>(payload);
        }
    }
};
template<class T>
class unit_details_dettach_button : public button_element_base {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = T{};
            parent->impl_get(state, payload);
            //auto content = any_cast<T>(payload);
        }
    }
};

template<class T>
class unit_details_buttons : public window_element_base {
public:
    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "load_button" && std::is_same_v<T, dcon::army_id>) {
            return make_element_by_type<unit_details_load_button>(state, id);
        } else if(name == "unload_button" && std::is_same_v<T, dcon::army_id>) {
            return make_element_by_type<unit_details_unload_button>(state, id);
        } else if(name == "enable_rebel_button" && std::is_same_v<T, dcon::army_id>) {
            return make_element_by_type<unit_details_enable_rebel_button>(state, id);
        } else if(name == "disable_rebel_button" && std::is_same_v<T, dcon::army_id>) {
            return make_element_by_type<unit_details_disable_rebel_button>(state, id);
        } else if(name == "select_land" && std::is_same_v<T, dcon::navy_id>) {
            return make_element_by_type<unit_details_select_land_button>(state, id);
        } else if(name == "attach_unit_button") {
            return make_element_by_type<unit_details_attach_button<T>>(state, id);
        } else if(name == "detach_unit_button") {
            return make_element_by_type<unit_details_dettach_button<T>>(state, id);
		} else {
            return nullptr;
        }
    }
};

template<class T>
class unit_details_window : public window_element_base {
    T unit_id{};
public:
    void on_create(sys::state& state) noexcept override {
        window_element_base::on_create(state);
        base_data.position.y = 250;

        xy_pair base_position = { 20, 0 };//state.ui_defs.gui[state.ui_state.defs_by_name.find("unittype_item_start")->second.definition].position;
        xy_pair base_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("unittype_item_offset")->second.definition].position;

        {
			auto win = make_element_by_type<unit_details_type_item<T, 0>>(state, state.ui_state.defs_by_name.find("unittype_item")->second.definition);
			win->base_data.position.x = base_position.x + (0 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (0 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
        }
        {
			auto win = make_element_by_type<unit_details_type_item<T, 1>>(state, state.ui_state.defs_by_name.find("unittype_item")->second.definition);
			win->base_data.position.x = base_position.x + (1 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (1 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
        }
        {
			auto win = make_element_by_type<unit_details_type_item<T, 2>>(state, state.ui_state.defs_by_name.find("unittype_item")->second.definition);
			win->base_data.position.x = base_position.x + (2 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (2 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
        }

        const xy_pair item_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("unittype_item")->second.definition].position;
        if constexpr(std::is_same_v<T, dcon::army_id>) {
            auto ptr = make_element_by_type<unit_details_army_listbox>(state, state.ui_state.defs_by_name.find("sup_subunits")->second.definition);
            ptr->base_data.position.y = base_position.y + item_offset.y + (3 * base_offset.y) + 72 - 32;
            ptr->base_data.size.y += 32;
            add_child_to_front(std::move(ptr));
        } else {
            auto ptr = make_element_by_type<unit_details_navy_listbox>(state, state.ui_state.defs_by_name.find("sup_subunits")->second.definition);
            ptr->base_data.position.y = base_position.y + item_offset.y + (3 * base_offset.y) + 72 - 32;
            ptr->base_data.size.y += 32;
            add_child_to_front(std::move(ptr));
        }

        {
            auto ptr = make_element_by_type<unit_details_buttons<T>>(state, state.ui_state.defs_by_name.find("sup_buttons_window")->second.definition);
            ptr->base_data.position.y = base_data.size.y; // Nudge
            add_child_to_front(std::move(ptr));
        }

	{
		auto ptr = make_element_by_type<unit_selection_panel<bool>>(state, state.ui_state.defs_by_name.find("unitpanel")->second.definition);
		ptr->base_data.position.y -= 81;
		add_child_to_front(std::move(ptr));
	}
    }

    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "unit_bottom_bg") {
            return make_element_by_type<draggable_target>(state, id);
		} else {
            return nullptr;
        }
    }

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<T>()) {
			payload.emplace<T>(unit_id);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<T>>()) {
			unit_id = any_cast<element_selection_wrapper<T>>(payload).data;
            impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
