#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_common_elements.hpp"

namespace ui {

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
        }  else {
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

class population_window : public window_element_base {
private:
   pop_legend_listbox* country_pop_listbox = nullptr;
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
            auto ptr = make_element_by_type<opaque_element_base>(state, id);
            return ptr;
        } else if(name == "sortby_size_button" ||
                 name == "sortby_type_button" ||
                 name == "sortby_nationality_button" ||
                 name == "sortby_religion_button" ||
                 name == "sortby_location_button")
        { return nullptr;
        } else if(name.substr(0, 4) == "sort") {
            auto ptr = make_element_by_type<generic_image_checkbox_button<dcon::nation_id>>(state, id);
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
    }

    message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
        if(payload.holds_type<dcon::pop_id>()) {
            return impl_set(state, payload);
        }
        return message_result::unseen;
    }
};

}
