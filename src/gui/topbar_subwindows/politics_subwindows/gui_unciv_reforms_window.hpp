#pragma once

#include "culture.hpp"
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "politics.hpp"
#include "system_state.hpp"
#include <cstdint>

namespace ui {

class unciv_reforms_westernize_button : public standard_nation_button {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		on_update(state);
	}

	void on_update(sys::state& state) noexcept override {
		disabled = state.world.nation_get_modifier_values(nation_id, sys::national_mod_offsets::civilization_progress_modifier) < 1.f;
	}
};

class unciv_reforms_reform_button : public standard_nation_reform_option_button {
public:
	void on_update(sys::state& state) noexcept override {
		standard_nation_reform_option_button::on_update(state);
		auto reform_type = state.world.reform_option_get_parent_reform(reform_option_id).get_reform_type();
		if(reform_type == uint8_t(culture::issue_type::military)) {
			disabled = !politics::can_enact_military_reform(state, nation_id, reform_option_id);
		} else {
			disabled = !politics::can_enact_economic_reform(state, nation_id, reform_option_id);
		}
	}
};

class unciv_reforms_option : public listbox_row_element_base<dcon::reform_option_id> {
public:
	void update(sys::state& state) noexcept override {
		Cyto::Any payload = content;
		impl_set(state, payload);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reform_name") {
			return make_element_by_type<generic_name_text<dcon::reform_option_id>>(state, id);
		} else if(name == "selected") {
			return make_element_by_type<reform_selected_icon>(state, id);
		} else if(name == "reform_option") {
			return make_element_by_type<unciv_reforms_reform_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class unciv_reforms_listbox : public listbox_element_base<unciv_reforms_option, dcon::reform_option_id> {
protected:
	std::string_view get_row_element_name() override {
        return "reform_option_window";
    }

public:
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::reform_id>()) {
			auto reform_id = any_cast<dcon::reform_id>(payload);
			row_contents.clear();
			auto fat_id = dcon::fatten(state.world, reform_id);
			for(auto& option : fat_id.get_options()) {
				if(option) {
					row_contents.push_back(option);
				}
			}
			update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class unciv_reforms_reform_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reform_name") {
			return make_element_by_type<generic_multiline_name_text<dcon::reform_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto reforms_listbox = std::make_unique<unciv_reforms_listbox>();
		reforms_listbox->base_data.size.x = 130;
		reforms_listbox->base_data.size.y = 100;
		make_size_from_graphics(state, reforms_listbox->base_data);
		reforms_listbox->on_create(state);
		add_child_to_front(std::move(reforms_listbox));
	}
};

class unciv_reforms_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "civ_progress") {
			return make_element_by_type<nation_westernization_progress_bar>(state, id);
		} else if(name == "westernize_button") {
			return make_element_by_type<unciv_reforms_westernize_button>(state, id);
		} else if(name == "research_points_val") {
			return make_element_by_type<nation_research_points_text>(state, id);
		} else if(auto reform_id = politics::get_reform_by_name(state, name); bool(reform_id)) {
			auto ptr = make_element_by_type<unciv_reforms_reform_window>(state, id);
			Cyto::Any payload = reform_id;
			ptr->impl_set(state, payload);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
