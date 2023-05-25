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

class unit_build_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		
	}
};

class unit_folder_button : public button_element_base {
public:
	int unit_type;
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = unit_type;
		impl_get(state, payload);

		Cyto::Any deactivate = true;
		impl_get(state, deactivate);
		frame = 1;
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<bool>()) {
			frame = 0;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class buildable_units : public listbox_row_element_base<dcon::pop_id> {
public:
	int unit_type = 17;
	ui::simple_text_element_base* unit_name = nullptr;
	ui::image_element_base* unit_icon = nullptr;
	ui::simple_text_element_base* province_name = nullptr;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "build_button") {
			auto ptr = make_element_by_type<unit_build_button>(state, id);
			return ptr;
		} else if(name == "build_button_group") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unit_name = ptr.get();
			return ptr;
		} else if(name == "unit_strip") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = 16;
			unit_icon = ptr.get();
			return ptr;
		} else if(name == "province") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			province_name = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		/*auto name_id = state.world.leader_get_name(content);
		auto name_content = state.to_string_view(name_id);
		leader_name->set_text(state, std::string(name_content));

		auto background_id = state.world.leader_get_background(content).get_name();
		auto background_content = text::produce_simple_string(state, background_id);
		background->set_text(state, background_content);

		auto personality_id = state.world.leader_get_personality(content).get_name();
		auto personality_content = text::produce_simple_string(state, personality_id);
		personality->set_text(state, personality_content);

		auto army_id = state.world.leader_get_army_from_army_leadership(content);
		if(army_id.value == 0) {
			army->set_text(state, "Unassigned");
			location->set_text(state, "");
		} else {
			auto army_content = state.to_string_view(state.world.army_get_name(army_id));
			army->set_text(state, std::string(army_content));

			auto location_content = text::produce_simple_string(state, state.world.province_get_name(state.world.army_location_get_location(state.world.army_get_army_location(army_id))));
			location->set_text(state, std::string(location_content));
		}*/

		//state.military_definitions.unit_base_definitions[dcon::unit_type_id(1)]

		//unit_icon->frame = 16;
		for(uint8_t i = 0; i < state.military_definitions.unit_base_definitions.size(); i++) {
			if(state.military_definitions.unit_base_definitions[dcon::unit_type_id(i)].icon == unit_type) {
				auto culture_id = state.world.pop_get_culture(content);
				auto culture_content = text::produce_simple_string(state, culture_id.get_name());
				auto unit_type_name = text::produce_simple_string(state, state.military_definitions.unit_base_definitions[dcon::unit_type_id(i)].name);
				unit_name->set_text(state, culture_content+" "+unit_type_name);
			}
		}
		province_name->set_text(state, text::produce_simple_string(state, state.world.province_get_name(state.world.pop_location_get_province(state.world.pop_get_pop_location(content)))));

		Cyto::Any payload = content;
		impl_set(state, payload);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<int>()) {
			unit_type = Cyto::any_cast<int>(payload);
			unit_icon->frame = unit_type-1;
			for(uint8_t i = 0; i < state.military_definitions.unit_base_definitions.size(); i++) {
				if(state.military_definitions.unit_base_definitions[dcon::unit_type_id(i)].icon == unit_type) {
					auto culture_id = state.world.pop_get_culture(content);
					auto culture_content = text::produce_simple_string(state, culture_id.get_name());
					auto unit_type_name = text::produce_simple_string(state, state.military_definitions.unit_base_definitions[dcon::unit_type_id(i)].name);
					unit_name->set_text(state, culture_content + " " + unit_type_name);
				}
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class build_units_listbox : public listbox_element_base<buildable_units, dcon::pop_id> {
protected:
	std::string_view get_row_element_name() override {
		return "build_unit_entry_wide";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		for(const auto fat_id : state.world.nation_get_province_ownership(state.local_player_nation)) {
			if(fat_id.get_province().get_province_control().get_nation() == fat_id.get_nation()) {
				for(const auto fat_id2 : state.world.province_get_pop_location(fat_id.get_province().id)) {
					if(fat_id2.get_pop().get_poptype().id == state.culture_definitions.soldiers) {
						if(state.world.province_get_is_colonial(fat_id2.get_province())) {
							if(fat_id2.get_pop().get_size() >= state.defines.pop_size_per_regiment*state.defines.pop_min_size_for_regiment_colony_multiplier) {
								int32_t total = int32_t(((fat_id2.get_pop().get_size() / (state.defines.pop_size_per_regiment*state.defines.pop_min_size_for_regiment_colony_multiplier))) + 1);
									for(const auto fat_id3 : fat_id2.get_pop().get_regiment_source()) {
										total--;
									}
								for(int32_t i = 0; i < total; i++) {
									row_contents.push_back(fat_id2.get_pop().id);
								}
							} else if(fat_id2.get_pop().get_size() >= state.defines.pop_min_size_for_regiment) {
								int32_t total = int32_t(1);
									for(const auto fat_id3 : fat_id2.get_pop().get_regiment_source()) {
										total--;
									}
								for(int32_t i = 0; i < total; i++) {
									row_contents.push_back(fat_id2.get_pop().id);
								}
							}
						} else if(!state.world.province_get_is_owner_core(fat_id2.get_province())) {
							if(fat_id2.get_pop().get_size() >= state.defines.pop_size_per_regiment*state.defines.pop_min_size_for_regiment_noncore_multiplier) {
								int32_t total = int32_t(((fat_id2.get_pop().get_size() / (state.defines.pop_size_per_regiment*state.defines.pop_min_size_for_regiment_noncore_multiplier))) + 1);
								for(const auto fat_id3 : fat_id2.get_pop().get_regiment_source()) {
									total--;
								}
								for(int32_t i = 0; i < total; i++) {
									row_contents.push_back(fat_id2.get_pop().id);
								}
							} else if(fat_id2.get_pop().get_size() >= state.defines.pop_min_size_for_regiment) {
								int32_t total = int32_t(1);
								for(const auto fat_id3 : fat_id2.get_pop().get_regiment_source()) {
									total--;
								}
								for(int32_t i = 0; i < total; i++) {
									row_contents.push_back(fat_id2.get_pop().id);
								}
							}
						} else {
							if(fat_id2.get_pop().get_size() >= state.defines.pop_size_per_regiment) {
								int32_t total = int32_t(((fat_id2.get_pop().get_size() / state.defines.pop_size_per_regiment)) + 1);
								for(const auto fat_id3 : fat_id2.get_pop().get_regiment_source()) {
									total--;
								}
								for(int32_t i = 0; i < total; i++) {
									row_contents.push_back(fat_id2.get_pop().id);
								}
							} else if(fat_id2.get_pop().get_size() >= state.defines.pop_min_size_for_regiment) {
								int32_t total = int32_t(1);
								for(const auto fat_id3 : fat_id2.get_pop().get_regiment_source()) {
									total--;
								}
								for(int32_t i = 0; i < total; i++) {
									row_contents.push_back(fat_id2.get_pop().id);
								}
							}
						}
					}
				}
			}
		}
		//for(uint16_t i = 0; i < state.world.nation_get_recruitable_regiments(state.local_player_nation); i++){

			//row_contents.push_back(state.world.nation_get_recruitable_regiments(state.local_player_nation));

		//}

		update(state);
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
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->frame = 1;
			ptr->unit_type = 17;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_1") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 1;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_2") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 2;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_3") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 3;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_16") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 16;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_18") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 18;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_14") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 14;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_13") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 13;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_15") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 15;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_20") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 20;
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "unit_folder_19") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			ptr->unit_type = 19;
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
		} else if(name == "list") {
			auto ptr = make_element_by_type<build_units_listbox>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "external_scroll_slider_list") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "external_scroll_slider_queue") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
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

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<int>()) {
			impl_set(state, payload);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	std::vector<element_base*> army_elements;
	std::vector<element_base*> navy_elements;
};

}
