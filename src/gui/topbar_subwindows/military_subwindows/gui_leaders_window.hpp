#pragma once

#include "gui_element_types.hpp"

namespace ui {

class military_leaders : public listbox_row_element_base<dcon::general_id> {
public:
	ui::simple_text_element_base* leader_name = nullptr;
	ui::simple_text_element_base* background = nullptr;
	ui::simple_text_element_base* personality = nullptr;
	ui::simple_text_element_base* army = nullptr;
	ui::simple_text_element_base* location = nullptr;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		base_data.position.y = base_data.position.x = 0;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			leader_name = ptr.get();
			return ptr;
		} else if(name == "background") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			background = ptr.get();
			return ptr;
		} else if(name == "personality") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			personality = ptr.get();
			return ptr;
		} else if(name == "use_leader") {
			auto ptr = make_element_by_type<checkbox_button>(state, id);
			return ptr;
		} else if(name == "army") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			army = ptr.get();
			return ptr;
		} else if(name == "location") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			location = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		auto name_id = state.world.general_get_name(content);
		auto name_content = state.to_string_view(name_id);
		leader_name->set_text(state, std::string(name_content));

		auto background_id = state.world.general_get_background(content).get_name();
		auto background_content = text::produce_simple_string(state, background_id);
		background->set_text(state, background_content);

		auto personality_id = state.world.general_get_personality(content).get_name();
		auto personality_content = text::produce_simple_string(state, personality_id);
		personality->set_text(state, personality_content);

		auto army_id = state.world.general_get_army_from_army_leadership(content);
		if(army_id.value == 0) {
			army->set_text(state, "Unassigned");
			location->set_text(state, "");
		} else {
			auto army_content = state.to_string_view(state.world.army_get_name(army_id));
			army->set_text(state, std::string(army_content));

			auto location_content = text::produce_simple_string(state, state.world.province_get_name(state.world.army_location_get_location(state.world.army_get_army_location(army_id))));
			location->set_text(state, std::string(location_content));
		}

		Cyto::Any payload = content;
		impl_set(state, payload);
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
		for(const auto fat_id : state.world.nation_get_general_loyalty(state.local_player_nation))
			row_contents.push_back(fat_id.get_leader());
		update(state);
	}
};

class leaders_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "sort_leader_prestige") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sort_leader_type") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sort_leader_name") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "leader_listbox") {
			auto ptr = make_element_by_type<military_leaders_listbox>(state, id);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
