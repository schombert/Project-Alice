#pragma once

#include "gui_element_types.hpp"

namespace ui {

class build_unit_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.ui_state.unit_window_army->set_visible(state, true);
		state.ui_state.unit_window_navy->set_visible(state, true);

		state.ui_state.build_unit_window->set_visible(state, false);
	}
};

class build_unit_large_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "cancel_all_units") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		}
		//ARMY
		else if(name == "build_army_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_17") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_1") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_2") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_3") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_16") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_18") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_14") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_13") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_15") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_20") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_19") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		}
		//NAVY
		else if(name == "build_navy_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_6") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_5") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_4") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_7") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_8") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_9") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_10") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_11") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_21") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_12") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "close") {
			auto ptr = make_element_by_type<build_unit_close_button>(state, id);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void set_army_visible(sys::state& state) {
		for(auto element : army_elements) {
			element->set_visible(state, true);
		}
	}

	void set_army_invisible(sys::state& state) {
		for(auto element : army_elements) {
			element->set_visible(state, false);
		}
	}

	void set_navy_visible(sys::state& state) {
		for(auto element : navy_elements) {
			element->set_visible(state, true);
		}
	}

	void set_navy_invisible(sys::state& state) {
		for(auto element : navy_elements) {
			element->set_visible(state, false);
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::army_id>()) {
			set_army_visible(state);
			set_navy_invisible(state);

			return message_result::consumed;
		} else if(payload.holds_type<dcon::navy_id>()) {
			set_army_invisible(state);
			set_navy_visible(state);

			return message_result::consumed;
		}
		return message_result::unseen;
	}

	std::vector<element_base*> army_elements;
	std::vector<element_base*> navy_elements;
};

}
