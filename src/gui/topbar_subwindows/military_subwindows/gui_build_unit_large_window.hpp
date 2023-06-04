#pragma once

#include "gui_element_types.hpp"

namespace ui {

class buildable_unit_entry_info {
public:
	dcon::pop_id pop_info;
	dcon::province_id province_info;
	// false == army
	// true == navy
	bool is_navy;
};

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
	bool is_navy = false;
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = Cyto::any_cast<dcon::nation_id>(n_payload);
			Cyto::Any u_payload = dcon::unit_type_id{};
			parent->impl_get(state, u_payload);
			dcon::unit_type_id utid = Cyto::any_cast<dcon::unit_type_id>(u_payload);
			Cyto::Any p_payload = dcon::province_id{};
			parent->impl_get(state, p_payload);
			dcon::province_id p = Cyto::any_cast<dcon::province_id>(p_payload);

			if(is_navy == false) {
				Cyto::Any c_payload = dcon::culture_id{};
				parent->impl_get(state, c_payload);
				dcon::culture_id c = Cyto::any_cast<dcon::culture_id>(c_payload);
				command::start_land_unit_construction(state, n, p, c, utid);
			} else {
				command::start_naval_unit_construction(state, n, p, utid);
			}
		}
	}
};

class unit_folder_button : public button_element_base {
public:
	dcon::unit_type_id unit_type{};

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<dcon::unit_type_id>{ unit_type };
			parent->impl_get(state, payload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id content = Cyto::any_cast<dcon::nation_id>(payload);
			disabled = !state.world.nation_get_active_unit(content, unit_type);
		}
	}
};

class buildable_units : public listbox_row_element_base<buildable_unit_entry_info> {
public:
	ui::unit_build_button* build_button;
	ui::simple_text_element_base* unit_name = nullptr;
	ui::image_element_base* unit_icon = nullptr;
	ui::simple_text_element_base* province_name = nullptr;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "build_button") {
			auto ptr = make_element_by_type<unit_build_button>(state, id);
			build_button = ptr.get();
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
		if(parent) {
			Cyto::Any payload = dcon::unit_type_id{};
			parent->impl_get(state, payload);
			dcon::unit_type_id utid = Cyto::any_cast<dcon::unit_type_id>(payload);
			unit_icon->frame = int32_t(state.military_definitions.unit_base_definitions[utid].icon - 1);

			build_button->is_navy = content.is_navy;
			if(content.is_navy == false) {
				auto culture_id = state.world.pop_get_culture(content.pop_info);
				auto culture_content = text::produce_simple_string(state, culture_id.get_name());
				auto unit_type_name = text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name);
				unit_name->set_text(state, culture_content + " " + unit_type_name);

				/* if(state.military_definitions.unit_base_definitions[dcon::unit_type_id(i)].primary_culture) {
					if(culture_id != state.world.nation_get_primary_culture(state.local_player_nation)) {
						int cult_it = 0;
						for(auto element : state.world.nation_get_accepted_cultures(state.local_player_nation)) {
							if(culture_id.id == element) {
								cult_it++;
							}
						}
						if(cult_it == 0) {
							set_visible(state, false);
						}
					}
				}*/
				province_name->set_text(state, text::produce_simple_string(state, state.world.province_get_name(state.world.pop_location_get_province(state.world.pop_get_pop_location(content.pop_info)))));
			} else {
				unit_name->set_text(state, text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name));
				province_name->set_text(state, text::produce_simple_string(state, state.world.province_get_name(content.province_info)));
			}
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::culture_id>()) {
			payload.emplace<dcon::culture_id>(state.world.pop_get_culture(content.pop_info).id);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(content.province_info);
			return message_result::consumed;
		}
		return listbox_row_element_base<buildable_unit_entry_info>::get(state, payload);
	}
};

class build_units_listbox : public listbox_element_base<buildable_units, buildable_unit_entry_info> {
protected:
	std::string_view get_row_element_name() override {
		return "build_unit_entry_wide";
	}

public:
	// false == army
	// true == navy
	bool is_navy = true;

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = dcon::unit_type_id{};
			parent->impl_get(state, payload);
			dcon::unit_type_id utid = Cyto::any_cast<dcon::unit_type_id>(payload);
			if(is_navy == false) {
				for(auto po : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
					auto p = po.get_province();
					state.world.for_each_culture([&](dcon::culture_id c) {
						if(command::can_start_land_unit_construction(state, state.local_player_nation, p, c, utid)) {
							buildable_unit_entry_info info;
							info.is_navy = false;
							for(auto pl : state.world.province_get_pop_location_as_province(p)) {
								if(pl.get_pop().get_culture() == c) {
									if(pl.get_pop().get_poptype() == state.culture_definitions.soldiers) {
										info.pop_info = pl.get_pop();
										break;
									}
								}
							}
							info.province_info = p;
							row_contents.push_back(info);
						}
					});
				}
			} else {
				for(auto po : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
					auto p = po.get_province();
					if(command::can_start_naval_unit_construction(state, state.local_player_nation, p, utid)) {
						buildable_unit_entry_info info;
						info.is_navy = true;
						info.pop_info = dcon::pop_id(1);
						info.province_info = p;
						row_contents.push_back(info);
					}
				}
			}
		}
		update(state);
	}
};

class build_unit_large_window : public window_element_base {
	dcon::unit_type_id unit_type{};
	dcon::unit_type_id first_land_type{};
	dcon::unit_type_id first_naval_type{};
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "cancel_all_units") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "build_army_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			return ptr;
		} else if(name == "build_navy_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			return ptr;
		} else if(name == "close") {
			auto ptr = make_element_by_type<build_unit_close_button>(state, id);
			return ptr;
		} else if(name == "list") {
			auto ptr = make_element_by_type<build_units_listbox>(state, id);
			buildable_units = ptr.get();
			return ptr;
		} else if(name == "external_scroll_slider_list") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "external_scroll_slider_queue") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name.substr(0, 12) == "unit_folder_") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			auto value = std::stoi(std::string(name.substr(12)));
			for(uint8_t i = 0; i < state.military_definitions.unit_base_definitions.size(); i++) {
				if(state.military_definitions.unit_base_definitions[dcon::unit_type_id(i)].icon == value) {
					ptr->unit_type = dcon::unit_type_id(i);
					break;
				}
			}
			switch(value) {
			case 1:
			case 2:
			case 3:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
				first_land_type = ptr->unit_type;
				army_elements.push_back(ptr.get());
				break;
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 21:
				first_naval_type = ptr->unit_type;
				navy_elements.push_back(ptr.get());
				break;
			default:
				break;
			}
			return ptr;
		} else {
			return nullptr;
		}
	}

	void set_army_visible(sys::state& state) {
		buildable_units->is_navy = false;
		for(auto element : army_elements)
			element->set_visible(state, true);
		buildable_units->impl_on_update(state);
	}

	void set_army_invisible(sys::state& state) {
		for(auto element : army_elements)
			element->set_visible(state, false);
	}

	void set_navy_visible(sys::state& state) {
		buildable_units->is_navy = true;
		for(auto element : navy_elements)
			element->set_visible(state, true);
		buildable_units->impl_on_update(state);
	}

	void set_navy_invisible(sys::state& state) {
		for(auto element : navy_elements)
			element->set_visible(state, false);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::army_id>()) {
			unit_type = first_land_type;
			set_navy_invisible(state);
			set_army_visible(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::navy_id>()) {
			unit_type = first_naval_type;
			set_army_invisible(state);
			set_navy_visible(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::unit_type_id>>()) {
			unit_type = Cyto::any_cast<element_selection_wrapper<dcon::unit_type_id>>(payload).data;
			buildable_units->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::unit_type_id>()) {
			payload.emplace<dcon::unit_type_id>(unit_type);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	std::vector<element_base*> army_elements;
	std::vector<element_base*> navy_elements;
	ui::build_units_listbox* buildable_units = nullptr;
};

} // namespace ui
