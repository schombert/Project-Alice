#include "gui_decision_window.hpp"

// -------------
// Decision Name
// -------------

void ui::decision_name::on_update(sys::state& state) noexcept {
	Cyto::Any payload = dcon::decision_id{};
	if(parent){
		parent->impl_get(state, payload);
		auto id = any_cast<dcon::decision_id>(payload);
		auto fat_id = dcon::fatten(state.world, id);
		auto name = text::produce_simple_string(state, fat_id.get_name());
		set_text(state, name);
	}
}

// -------------
// Decision Item
// -------------

std::unique_ptr<ui::element_base> ui::decision_item::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
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

ui::message_result ui::decision_item::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<dcon::decision_id>()) {
		payload.emplace<dcon::decision_id>(content);
		return message_result::consumed;
	} else {
		return listbox_row_element_base<dcon::decision_id>::get(state, payload);
	}
}

// ----------------
// Decision Listbox
// ----------------

std::string_view ui::decision_listbox::get_row_element_name() {
	return "decision_entry";
}

// ----------------
// Decision Window
// ----------------

std::vector<dcon::decision_id> ui::decision_window::get_decisions(sys::state& state) { 
	std::vector<dcon::decision_id> list;
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

void ui::decision_window::on_create(sys::state& state) noexcept {
	window_element_base::on_create(state);
	set_visible(state, false);
	
	decision_list->row_contents = get_decisions(state);
	decision_list->update(state);
}

void ui::decision_window::on_update(sys::state& state) noexcept {
	decision_list->row_contents = get_decisions(state);
	decision_list->update(state);
}

std::unique_ptr<ui::element_base> ui::decision_window::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "decision_listbox") {
		auto ptr = make_element_by_type<decision_listbox>(state, id);
		decision_list = ptr.get();
		return ptr;
	} else {
		return nullptr;
	}
}