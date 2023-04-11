#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_common_elements.hpp"

namespace ui {

enum pop_listbox_item_type { nation, state, province };

class state_national_focus_icon : public button_element_base {
private:
	dcon::national_focus_id focus_id{};

public:
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::national_focus_id>()) {
			focus_id = any_cast<dcon::national_focus_id>(payload);
			auto fat_id = dcon::fatten(state.world, focus_id);
			frame = fat_id.get_icon() - 1;
			
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
        if(focus_id) {
            auto box = text::open_layout_box(contents, 0);
            text::add_to_layout_box(contents, state, box, state.world.national_focus_get_name(focus_id), text::substitution_map{ });
            text::close_layout_box(contents, box);
        }
    }
};

class state_focus_item : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "focus_icon") {
			return make_element_by_type<state_national_focus_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

class state_focus_category_list : public overlapping_listbox_element_base<state_focus_item, dcon::national_focus_id> {
public:
	std::string_view get_row_element_name() override {
		return "focus_item";
	}

	void update_subwindow(sys::state& state, state_focus_item* subwindow, dcon::national_focus_id content) override {
		Cyto::Any payload = content;
		subwindow->impl_set(state, payload);
	}
};

class state_focus_category : public window_element_base {
private:
	simple_text_element_base* category_label = nullptr;
	state_focus_category_list* focus_list = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			category_label = ptr.get();
			return ptr;
		} else if(name == "focus_icons") {
			auto ptr = make_element_by_type<state_focus_category_list>(state, id);
			focus_list = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<nations::focus_type>()) {
			auto category = any_cast<nations::focus_type>(payload);
			category_label->set_text(state, text::get_focus_category_name(state, category));

			focus_list->contents.clear();
			state.world.for_each_national_focus([&](dcon::national_focus_id focus_id) {
				auto fat_id = dcon::fatten(state.world, focus_id);
				if(fat_id.get_type() == uint8_t(category)) {
					focus_list->contents.push_back(focus_id);
				}
			});
			focus_list->update(state);

			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class state_close_button : public generic_close_button {
public:
	void button_action(sys::state& state) noexcept override {
		state.map_display.set_selected_province(dcon::province_id{});
        generic_close_button::button_action(state);
	}
};

class state_national_focus_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto start = make_element_by_type<window_element_base>(state, "focuscategory_start");
		auto current_pos = start->base_data.position;
		auto step = make_element_by_type<window_element_base>(state, "focuscategory_step");
		auto step_y = step->base_data.position.y;

		for(uint8_t i = 1; i <= uint8_t(nations::focus_type::party_loyalty_focus); i++) {
			auto ptr = make_element_by_type<state_focus_category>(state, "focus_category");
			ptr->base_data.position = current_pos;
			current_pos = xy_pair{current_pos.x, int16_t(current_pos.y + step_y)};

			Cyto::Any foc_type_payload = nations::focus_type(i);
			ptr->impl_set(state, foc_type_payload);

			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<state_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

enum class pop_list_filter : uint8_t {
    size,
    type,
    nationality,
    religion,
    location,
    mil,
    con,
    ideology,
    issues,
    unemployment,
    cash,
    subsistence,
    eve,
    luxury,
    revoltrisk,
    change,
    literacy
};

class pop_legend_info : public listbox_row_element_base<dcon::pop_id> {
private:
    simple_text_element_base *size = nullptr;
    image_element_base *type = nullptr;
    simple_text_element_base *nation = nullptr;
    image_element_base *religion = nullptr;
    simple_text_element_base *location = nullptr;
    simple_text_element_base *mil = nullptr;
    simple_text_element_base *con = nullptr;

public:
    void on_create(sys::state &state) noexcept override {
        listbox_row_element_base<dcon::pop_id>::on_create(state);
    }

    std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if (name == "pop_size") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            size = ptr.get();
            return ptr;
        } else if (name == "pop_nation") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            nation = ptr.get();
            return ptr;
        } else if (name == "pop_location") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            location = ptr.get();
            return ptr;
        } else if (name == "pop_mil") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            mil = ptr.get();
            return ptr;
        } else if (name == "pop_con") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            con = ptr.get();
            return ptr;
        } else if (name == "pop_type") {
            auto ptr = make_element_by_type<pop_type_icon>(state, id);
            type = ptr.get();
            return ptr;
        } else if (name == "pop_religion") {
            auto ptr = make_element_by_type<religion_type_icon>(state, id);
            religion = ptr.get();
            return ptr;
        } else {
            return nullptr;
        }
    };

    void update(sys::state& state) noexcept override {
        dcon::pop_fat_id fat_id = dcon::fatten(state.world, content);
        dcon::culture_fat_id cfat_id = dcon::fatten(state.world, fat_id.get_culture());
        dcon::province_fat_id lcfat_id = (dcon::fatten(state.world, fat_id.get_pop_location())).get_province();

        Cyto::Any tpayload = fat_id.get_poptype().id;
        type->impl_set(state, tpayload);
        type->impl_on_update(state);

        Cyto::Any rpayload = fat_id.get_religion().id;
        religion->impl_set(state, rpayload);
        religion->impl_on_update(state);


        size->set_text(state, std::to_string(int32_t(fat_id.get_size())));
        nation->set_text(state, text::produce_simple_string(state, cfat_id.get_name()));
        location->set_text(state, text::produce_simple_string(state, lcfat_id.get_name()));
        mil->set_text(state, text::format_float(fat_id.get_militancy()));
        con->set_text(state, text::format_float(fat_id.get_consciousness()));
    }

    message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
        if(payload.holds_type<dcon::nation_id>()) {
            payload.emplace<dcon::pop_id>(content);
            return message_result::consumed;
        } else if(payload.holds_type<button_press_notification>()) {
            Cyto::Any new_payload = content;
            return parent->impl_get(state, new_payload);
        } else {
            return listbox_row_element_base<dcon::pop_id>::get(state, payload);
        }
    }
};

class pop_legend_listbox : public listbox_element_base<pop_legend_info, dcon::pop_id>{
protected:
    std::string_view get_row_element_name() override {
        return "popinfomember_popview";
    }
};

class pop_state_expand_button : public button_element_base {
public:
    void button_action(sys::state& state) noexcept override {

    }
};

class pop_state_national_focus_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
};

class pop_listbox_nation_item : public window_element_base {
    simple_text_element_base* elem_name = nullptr;
public:
    dcon::nation_id nat_id;
    std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if (name == "poplistbutton") {
            auto ptr = make_element_by_type<button_element_base>(state, id);
            return ptr;
        } else if (name == "poplist_name") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            elem_name = ptr.get();
            return ptr;
        /*} else if (name == "poplist_numpops") {
            auto ptr = make_element_by_type<population_text>(state, id);
            num_pops_text = ptr.get();
            return ptr;*/
        } else if (name == "growth_indicator") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            return ptr;
        } else {
            return nullptr;
        }
    };
    void on_update(sys::state& state) noexcept override {
        auto fat_id = dcon::fatten(state.world, nat_id);
        auto name = text::produce_simple_string(state, fat_id.get_name());
        if(!name.empty())
            elem_name->set_text(state, name);
    }
};
class pop_state_population_text : public simple_text_element_base {
    dcon::state_instance_id state_id;
	std::string get_text(sys::state& state) noexcept {
        auto total_pop = 0.f;
        auto def_id = dcon::fatten(state.world, dcon::state_instance_id(state_id)).get_definition();
        state.world.for_each_province([&](dcon::province_id prov_id) {
            auto fat_id = dcon::fatten(state.world, prov_id);
            auto memb_fat_id = dcon::fatten(state.world, fat_id.get_abstract_state_membership());
            if(memb_fat_id.get_state() == def_id)
                total_pop += state.world.province_get_demographics(prov_id, demographics::total);
        });
        return text::prettify(int32_t(total_pop));
	}
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(auto k = state.key_to_text_sequence.find(std::string_view("num")); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{ });
			text::close_layout_box(contents, box);
		}
	}
	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::state_instance_id>()) {
			state_id = any_cast<dcon::state_instance_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};
class pop_listbox_state_item : public window_element_base {
    simple_text_element_base* elem_name = nullptr;
    pop_state_population_text* num_pops_text = nullptr;
public:
    dcon::state_instance_id state_id;
    std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if (name == "poplistbutton") {
            auto ptr = make_element_by_type<button_element_base>(state, id);
            return ptr;
        } else if (name == "poplist_name") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            elem_name = ptr.get();
            return ptr;
        } else if (name == "colonial_state_icon") {
            auto ptr = make_element_by_type<button_element_base>(state, id);
            return ptr;
        } else if (name == "state_focus") {
            auto ptr = make_element_by_type<pop_state_national_focus_button>(state, id);
            return ptr;
        } else if (name == "poplist_numpops") {
            auto ptr = make_element_by_type<pop_state_population_text>(state, id);
            num_pops_text = ptr.get();
            return ptr;
        } else if (name == "expand") {
            auto ptr = make_element_by_type<pop_state_expand_button>(state, id);
            return ptr;
        } else if (name == "growth_indicator") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            return ptr;
        } else {
            return nullptr;
        }
    };
    void on_update(sys::state& state) noexcept override {
        Cyto::Any c_state_id = state_id;
        num_pops_text->set(state, c_state_id);
        auto def_id = dcon::fatten(state.world, state_id).get_definition();
        auto fat_id = dcon::fatten(state.world, def_id);
        auto name = text::produce_simple_string(state, fat_id.get_name());
        if(!name.empty())
            elem_name->set_text(state, name);
    }
};
class pop_listbox_province_item : public window_element_base {
    simple_text_element_base* elem_name = nullptr;
public:
    dcon::province_id prov_id;
    std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if (name == "poplistbutton") {
            auto ptr = make_element_by_type<button_element_base>(state, id);
            return ptr;
        } else if (name == "poplist_name") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            elem_name = ptr.get();
            return ptr;
        /*} else if (name == "poplist_numpops") {
            auto ptr = make_element_by_type<population_text>(state, id);
            num_pops_text = ptr.get();
            return ptr;*/
        } else if (name == "growth_indicator") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            return ptr;
        } else {
            return nullptr;
        }
    };
    void on_update(sys::state& state) noexcept override {
        auto fat_id = dcon::fatten(state.world, prov_id);
        auto name = text::produce_simple_string(state, fat_id.get_name());
        if(!name.empty())
            elem_name->set_text(state, name);
    }
};

class pop_listbox_item_container : public listbox_row_element_base<std::pair<pop_listbox_item_type, int32_t>> {
    element_base* contained_item = nullptr;
public:
    void on_update(sys::state& state) noexcept override {
        if(contained_item == nullptr) {
            switch(content.first) {
                case pop_listbox_item_type::nation: {
                    auto new_it = make_element_by_type<pop_listbox_nation_item>(state, "poplistitem_country");
                    contained_item = new_it.get();
                    static_cast<pop_listbox_nation_item*>(contained_item)->nat_id = dcon::nation_id(content.second);
                    add_child_to_front(std::move(new_it));
                    break;
                } case pop_listbox_item_type::state: {
                    /*auto new_it = make_element_by_type<pop_listbox_state_item>(state, "poplistitem_state");
                    contained_item = new_it.get();
                    static_cast<pop_listbox_state_item*>(contained_item)->state_id = dcon::state_instance_id(content.second);
                    add_child_to_front(std::move(new_it));*/
                    break;
                } case pop_listbox_item_type::province: {
                    /*auto new_it = make_element_by_type<pop_listbox_province_item>(state, "poplistitem_province");
                    contained_item = new_it.get();
                    static_cast<pop_listbox_province_item*>(contained_item)->prov_id = dcon::province_id(content.second);
                    add_child_to_front(std::move(new_it));*/
                    break;
                }
            }
        }
    }
};

class pop_listbox : public listbox_element_base<pop_listbox_item_container, std::pair<pop_listbox_item_type, int32_t>> {
protected:
    std::string_view get_row_element_name() override {
        return "popinfomember_popview";
    }
};

class population_window : public window_element_base {
private:
   pop_legend_listbox* country_pop_listbox = nullptr;
   pop_listbox *state_listbox = nullptr;
   dcon::nation_id nation_id;

    void filter_pop(sys::state& state, std::function<bool(dcon::nation_id)> filter_fun) {
        if(country_pop_listbox){
            country_pop_listbox->row_contents.clear();
            state.world.for_each_pop([&](dcon::pop_id id) {
                dcon::pop_fat_id fat_id = dcon::fatten(state.world, id);
                if(fat_id.get_pop_location().get_province().get_province_ownership().get_nation() == state.local_player_nation) {
                    country_pop_listbox->row_contents.push_back(id);
                }
            });
            std::sort(country_pop_listbox->row_contents.begin(), country_pop_listbox->row_contents.end(), [&](auto a, auto b) {
                dcon::pop_fat_id a_fat_id = dcon::fatten(state.world, a);
                dcon::pop_fat_id b_fat_id = dcon::fatten(state.world, b);

                //bool sortFilter = a_fat_id.get_poptype().id.value > b_fat_id.get_poptype().id.value;
                bool sortFilter = a_fat_id.get_size() > b_fat_id.get_size();
                return sortFilter;

            });
            country_pop_listbox->update(state);
        }
    }
public:
    state_national_focus_window *focus_win = nullptr;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
        set_visible(state, false);
        filter_pop(state, [](dcon::nation_id) { return true; });
        nation_id = state.local_player_nation;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if (name == "pop_list"){
            auto ptr = make_element_by_type<pop_legend_listbox>(state, id);
            country_pop_listbox = ptr.get();
            return ptr;
        } else if (name == "external_scroll_slider") {
            auto ptr = make_element_by_type<opaque_element_base>(state,id);
            return ptr;
        } else if (name == "pop_province_list") {
            auto ptr = make_element_by_type<pop_listbox>(state, id);
            state_listbox = ptr.get();
            return ptr;
        } else {
			return nullptr;
		}
	}

    void on_update(sys::state& state) noexcept override {
        if (nation_id != state.local_player_nation) {
            filter_pop(state, [](dcon::nation_id) { return true; });
            nation_id = state.local_player_nation;
        }

        std::vector<std::pair<pop_listbox_item_type, int32_t>> results{};
        state.world.for_each_nation([&](dcon::nation_id nat_id) {
            if(nat_id != nation_id)
                return;
            
            results.push_back(std::make_pair(pop_listbox_item_type::nation, nat_id.value));
            state.world.for_each_state_instance([&](dcon::state_instance_id state_id) {
                auto state_def = state.world.state_instance_get_definition(state_id);
                auto owner = state.world.state_instance_get_nation_from_state_ownership(state_id);
                if(owner != nat_id)
                    return;

                results.push_back(std::make_pair(pop_listbox_item_type::state, state_id.value));
                for(auto p : state.world.state_definition_get_abstract_state_membership(state_def)) {
                    auto prov_fat_id = p.get_province();
                    if(prov_fat_id.get_nation_from_province_ownership() == owner)
                        results.push_back(std::make_pair(pop_listbox_item_type::province, prov_fat_id.id.value));
                }
            });
        });
        state_listbox->row_contents = results;
        state_listbox->update(state);
    }

    message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
        if(payload.holds_type<dcon::pop_id>()) {
            return impl_set(state, payload);
        }
        return message_result::unseen;
    }
};

void pop_state_national_focus_button::button_action(sys::state& state) noexcept {
    // Button -> ListboxItem -> ItemContainer -> ListBox -> Window
    auto win = static_cast<population_window*>(parent->parent->parent->parent);
    if(win->focus_win == nullptr) {
        auto new_win = make_element_by_type<state_national_focus_window>(state, "state_focus_window");
        win->focus_win = static_cast<state_national_focus_window*>(new_win.get());
        win->add_child_to_front(std::move(new_win));
        win->focus_win->set_visible(state, false);
    }
    win->focus_win->set_visible(state, !win->focus_win->is_visible());
}

}
