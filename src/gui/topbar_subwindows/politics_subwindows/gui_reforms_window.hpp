#pragma once

#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

class reforms_reform_button : public standard_nation_issue_option_button {
public:
	void on_update(sys::state& state) noexcept override {
		standard_nation_issue_option_button::on_update(state);
		auto issue_type = state.world.issue_option_get_parent_issue(issue_option_id).get_issue_type();
		if(issue_type == uint8_t(culture::issue_type::political)) {
			disabled = !politics::can_enact_political_reform(state, nation_id, issue_option_id);
		} else {
			disabled = !politics::can_enact_social_reform(state, nation_id, issue_option_id);
		}
	}
};

class reforms_option : public listbox_row_element_base<dcon::issue_option_id> {
public:
	void update(sys::state& state) noexcept override {
		Cyto::Any payload = content;
		impl_set(state, payload);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reform_name") {
			return make_element_by_type<generic_name_text<dcon::issue_option_id>>(state, id);
		} else if(name == "selected") {
			return make_element_by_type<issue_selected_icon>(state, id);
		} else if(name == "reform_option") {
			return make_element_by_type<reforms_reform_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class reforms_listbox : public listbox_element_base<reforms_option, dcon::issue_option_id> {
protected:
	std::string_view get_row_element_name() override {
        return "reform_option_window";
    }

public:
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_id>()) {
			auto issue_id = any_cast<dcon::issue_id>(payload);
			row_contents.clear();
			auto fat_id = dcon::fatten(state.world, issue_id);
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

class reforms_reform_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reform_name") {
			return make_element_by_type<generic_multiline_name_text<dcon::issue_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto reforms_box = std::make_unique<reforms_listbox>();
		reforms_box->base_data.size.x = 130;
		reforms_box->base_data.size.y = 100;
		make_size_from_graphics(state, reforms_box->base_data);
		reforms_box->on_create(state);
		add_child_to_front(std::move(reforms_box));
	}
};

class reforms_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(auto issue_id = politics::get_issue_by_name(state, name); bool(issue_id)) {
			auto ptr = make_element_by_type<reforms_reform_window>(state, id);
			Cyto::Any payload = issue_id;
			ptr->impl_set(state, payload);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
