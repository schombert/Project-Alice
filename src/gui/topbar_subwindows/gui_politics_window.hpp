#pragma once

#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "gui_movements_window.hpp"
#include "gui_decision_window.hpp"
#include "gui_reforms_window.hpp"
#include "gui_release_nation_window.hpp"
#include "gui_unciv_reforms_window.hpp"
#include "system_state.hpp"
#include <vector>

namespace ui {

enum class politics_window_tab : uint8_t {
	reforms = 0x0,
	movements = 0x1,
	decisions = 0x2,
	releasables = 0x3
};

class politics_unciv_overlay : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		set_visible(state, !state.world.nation_get_is_civilized(nation_id));
		return 0;
	}
};

class politics_upper_house_entry : public listbox_row_element_base<dcon::ideology_id> {
private:
	dcon::nation_id nation_id{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "ideology_name") {
			return make_element_by_type<generic_name_text<dcon::ideology_id>>(state, id);
		} else if(name == "ideology_perc") {
			return make_element_by_type<nation_ideology_percentage_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			update(state);
			return message_result::consumed;
		} else {
			return listbox_row_element_base<dcon::ideology_id>::set(state, payload);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<wrapped_listbox_row_content<dcon::nation_id>>()) {
			Cyto::Any new_payload = any_cast<wrapped_listbox_row_content<dcon::nation_id>>(payload).content;
			impl_set(state, new_payload);
			return message_result::consumed;
		} else if(payload.holds_type<wrapped_listbox_row_content<dcon::ideology_id>>()) {
			Cyto::Any new_payload = any_cast<wrapped_listbox_row_content<dcon::ideology_id>>(payload).content;
			impl_set(state, new_payload);
			return message_result::consumed;
		} else {
			return listbox_row_element_base<dcon::ideology_id>::get(state, payload);
		}
	}
};

class politics_upper_house_listbox : public listbox_element_base<politics_upper_house_entry, dcon::ideology_id> {
private:
	dcon::nation_id nation_id{};

protected:
	std::string_view get_row_element_name() override {
        return "ideology_option_window";
    }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_ideology([&](dcon::ideology_id ideology_id) {
			if(state.world.nation_get_upper_house(nation_id, ideology_id) > 0.f) {
				row_contents.push_back(ideology_id);
			}
		});
		update(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			update(state);
			return message_result::consumed;
		} else {
			return listbox_element_base<politics_upper_house_entry, dcon::ideology_id>::set(state, payload);
		}
	}
};

class politics_window : public generic_tabbed_window<politics_window_tab> {
private:
	dcon::nation_id nation_id{};

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		set_visible(state, false);
		nation_id = state.local_player_nation;
		Cyto::Any payload = nation_id;
		impl_set(state, payload);
	}
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "reforms_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::reforms;
			return ptr;
		} else if(name == "movements_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::movements;
			return ptr;
		} else if(name == "decisions_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::decisions;
			return ptr;
		} else if(name == "release_nations_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::releasables;
			return ptr;
		} else if(name == "reforms_window") {
			auto ptr = make_element_by_type<reforms_window>(state, id);
			reform_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "movements_window") {
			auto ptr = make_element_by_type<movements_window>(state, id);
			movement_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "decision_window") {
			auto ptr = make_element_by_type<decision_window>(state, id);
			decision_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "release_nation") {
			auto ptr = make_element_by_type<release_nation_window>(state, id);
			release_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "unciv_reforms_window") {
			auto ptr = make_element_by_type<unciv_reforms_window>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "government_name") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "national_value") {
			return make_element_by_type<nation_national_value_icon>(state, id);
		} else if(name == "plurality_value") {
			return make_element_by_type<nation_plurality_text>(state, id);
		} else if(name == "revanchism_value") {
			return make_element_by_type<nation_revanchism_text>(state, id);
		} else if(name == "can_do_social_reforms") {
			return make_element_by_type<nation_can_do_social_reform_text>(state, id);
		} else if(name == "social_reforms_bock") {
			return make_element_by_type<nation_can_do_social_reform_icon>(state, id);
		} else if(name == "can_do_political_reforms") {
			return make_element_by_type<nation_can_do_political_reform_text>(state, id);
		} else if(name == "political_reforms_bock") {
			return make_element_by_type<nation_can_do_political_reform_icon>(state, id);
		} else if(name == "chart_upper_house") {
			return make_element_by_type<upper_house_piechart>(state, id);
		} else if(name == "chart_people_ideologies") {
			return make_element_by_type<ideology_piechart<dcon::nation_id>>(state, id);
		} else if(name == "upperhouse_ideology_listbox") {
			return make_element_by_type<politics_upper_house_listbox>(state, id);
		} else if(name == "unciv_overlay") {
			return make_element_by_type<politics_unciv_overlay>(state, id);
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		if(nation_id != state.local_player_nation) {
			nation_id = state.local_player_nation;
			Cyto::Any payload = nation_id;
			impl_set(state, payload);
		}
	}
	void hide_vector_elements(sys::state& state, std::vector<element_base*>& elements) {
		for(auto element : elements) {
			element->set_visible(state, false);
		}
	}

	void show_vector_elements(sys::state& state, std::vector<element_base*>& elements) {
		for(auto element : elements) {
			element->set_visible(state, true);
		}
	}
	void hide_sub_windows(sys::state& state) {
		hide_vector_elements(state, reform_elements);
		hide_vector_elements(state, movement_elements);
		hide_vector_elements(state, decision_elements);
		hide_vector_elements(state, release_elements);
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<politics_window_tab>()) {
			auto enum_val = any_cast<politics_window_tab>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
				case politics_window_tab::reforms:
					show_vector_elements(state, reform_elements);
					break;
				case politics_window_tab::movements:
					show_vector_elements(state, movement_elements);
					break;
				case politics_window_tab::decisions:
					show_vector_elements(state, decision_elements);
					break;
				case politics_window_tab::releasables:
					show_vector_elements(state, release_elements);
					break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(nation_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	std::vector<element_base*> reform_elements;
	std::vector<element_base*> movement_elements;
	std::vector<element_base*> decision_elements;
	std::vector<element_base*> release_elements;
};

}
