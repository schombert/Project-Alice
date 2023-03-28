#pragma once

#include <vector>

#include "gui_element_types.hpp"
#include "triggers.hpp"

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
protected:
	dcon::nation_id current_nation{};
  std::string_view get_row_element_name() override {
    return "decision_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {}
};

class decision_window : public window_element_base {
private:
  decision_listbox* decision_list{nullptr};
	dcon::national_identity_id stored_identity{};

	std::vector<dcon::decision_id> get_decisions(sys::state& state) { 
		std::vector<dcon::decision_id> list{};
		auto n = state.local_player_nation;
		for(uint32_t i = state.world.decision_size(); i-- > 0; ) {
			dcon::decision_id did{ dcon::decision_id::value_base_t(i) };
			auto lim = state.world.decision_get_potential(did); 
			//if(!lim || trigger::evaluate_trigger(state, lim, trigger::to_generic(n), trigger::to_generic(n), 0)) { 
				list.push_back(did);
			//}
		}
		return list;
	}

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
		
		decision_list->row_contents = get_decisions(state);
		decision_list->update(state);
	}

	void on_update(sys::state& state) noexcept override {
		decision_list->row_contents = get_decisions(state);
		decision_list->update(state);
	}

	//TODO: add whenever else to update the list. set? get?

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
