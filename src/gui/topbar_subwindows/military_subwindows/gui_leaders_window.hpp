#pragma once

#include "gui_element_types.hpp"

namespace ui {

class military_leaders : public listbox_row_element_base<dcon::general_id> {
public:
	dcon::general_id leader_id;
	ui::simple_text_element_base* leader_name;
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		this->base_data.position.y = 0;
		this->base_data.position.x = 0;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			leader_name = ptr.get();
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "background") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	void update_name(sys::state& state) {
		leader_name->set_text(state, std::string(state.to_string_view(state.world.general_get_name(this->leader_id))));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::general_id>()) {
			impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class military_leaders_listbox : public listbox_element_base<military_leaders, dcon::general_id> {
protected:
	std::string_view get_row_element_name() override {
		return "milview_leader_entry";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		int i = 0;
		for(const auto fat_id : state.world.nation_get_general_loyalty(state.local_player_nation)) {
			row_contents.push_back(fat_id.get_leader());
			if(size_t(i) < this->row_windows.size()) {
				this->row_windows[i]->leader_id = fat_id.get_leader();
				this->row_windows[i]->update_name(state);
				i++;
			}
		}
		update(state);
	}
};

class leaders_window : public window_element_base {
	military_leaders_listbox* leaders_listbox = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "sort_leader_prestige") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "sort_leader_type") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "sort_leader_name") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "leader_listbox") {
			auto ptr = make_element_by_type<military_leaders_listbox>(state, id);
			leaders_listbox = ptr.get();
			ptr->set_visible(state, true);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
