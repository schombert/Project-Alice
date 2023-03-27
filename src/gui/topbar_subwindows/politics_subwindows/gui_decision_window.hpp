#pragma once

#include "gui_element_types.hpp"

namespace ui {

// TODO: verify these types
class decision_name : public window_element_base {};
class decision_image : public image_element_base {};
class decision_desc : public window_element_base {};
class decision_requirements : public image_element_base {};
class ignore_checkbox : public checkbox_button {};
class make_decision : public  button_element_base {};

class decision_item : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "decision_name") {
			auto ptr = make_element_by_type<decision_name>(state, id);
			return ptr;
		} else if (name == "decision_image"){
			auto ptr = make_element_by_type<decision_image>(state, id);
			return ptr;
		} else if (name == "decision_desc") {
			auto ptr = make_element_by_type<decision_desc>(state, id);
			return ptr;
		} else if (name == "requirements") {
			auto ptr = make_element_by_type<decision_requirements>(state, id);
			return ptr;
		} else if (name == "ignore_checkbox") {
			auto ptr = make_element_by_type<ignore_checkbox>(state, id);
			return ptr;
		} else if (name == "make_decision") {
			auto ptr = make_element_by_type<make_decision>(state, id);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

class decision_listbox : public listbox_element_base<decision_item, dcon::decision_id> {
public:
	void on_create(sys::state& state) noexcept override {
		// grab the list of decisions
		// create & store a bunch of decision_item elements using those
		// TODO: then also do this when things get updated
	}
};

class decision_window : public window_element_base {
private:
  decision_listbox* decision_list{nullptr};

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "decision_listbox") {
			auto ptr = make_element_by_type<decision_listbox>(state, id);
			decision_list = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
