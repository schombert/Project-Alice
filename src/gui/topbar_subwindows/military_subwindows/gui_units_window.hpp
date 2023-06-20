#pragma once

#include "gui_element_types.hpp"

namespace ui {

template<typename T>
struct military_unit_info : public std::variant<T, dcon::province_land_construction_id, dcon::province_naval_construction_id> { };

template<typename T>
class military_unit_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = military_unit_info<T>{};
			parent->impl_get(state, payload);
			auto content = Cyto::any_cast<military_unit_info<T>>(payload);
			if(std::holds_alternative<T>(content)) {
				auto fat_id = dcon::fatten(state.world, std::get<T>(content));
				set_text(state, std::string{state.to_string_view(fat_id.get_name())});
			}
		}
	}
};

class military_unit_building_progress_bar : public progress_bar {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box,
				std::string_view("UwU")); // TODO - this should only display if the unit is being built,
																	// it needs to display the goods that are needed before contruction can
																	// begin, once said goods are found it doesnt display anything
		text::close_layout_box(contents, box);
	}
};

class military_unit_morale_progress_bar : public vertical_progress_bar {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_single_sub_box(state, contents, box, std::string_view("military_morale_tooltip"), text::variable_type::value,
				uint8_t(progress * 100));
		text::close_layout_box(contents, box);
	}
};

class military_unit_strength_progress_bar : public vertical_progress_bar {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		// This should change to use "military_shipstrength_tooltip" instead for naval units
		text::localised_single_sub_box(state, contents, box, std::string_view("military_strength_tooltip2"),
				text::variable_type::percent, uint8_t(progress * 100));
		text::close_layout_box(contents, box);
	}
};

template<typename T>
class military_unit_entry : public listbox_row_element_base<military_unit_info<T>> {
	simple_text_element_base* unit_name = nullptr;
	image_element_base* unit_icon = nullptr;
	image_element_base* leader_icon = nullptr;
	button_element_base* cancel_button = nullptr;
	simple_text_element_base* eta_date_text = nullptr;
	simple_text_element_base* location_text = nullptr;
	military_unit_building_progress_bar* unit_building_progress = nullptr;
	simple_text_element_base* unit_regiments_text = nullptr;
	simple_text_element_base* unit_men_text = nullptr;
	military_unit_morale_progress_bar* unit_morale_progress = nullptr;
	military_unit_strength_progress_bar* unit_strength_progress = nullptr;
	image_element_base* unit_moving_icon = nullptr;
	image_element_base* unit_digin_icon = nullptr;
	image_element_base* unit_combat_icon = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "military_unit_entry_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "unit_progress") {
			auto ptr = make_element_by_type<military_unit_building_progress_bar>(state, id);
			unit_building_progress = ptr.get();
			return ptr;

		} else if(name == "leader") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			leader_icon = ptr.get();
			return ptr;
		} else if(name == "unit_strip") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			unit_icon = ptr.get();
			return ptr;
		} else if(name == "name") {
			auto ptr = make_element_by_type<military_unit_name_text<T>>(state, id);
			unit_name = ptr.get();
			return ptr;
		} else if(name == "location") {
			auto ptr = make_element_by_type<generic_name_text<dcon::province_id>>(state, id);
			location_text = ptr.get();
			return ptr;
		} else if(name == "unit_eta") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			eta_date_text = ptr.get();
			return ptr;
		} else if(name == "military_cancel_unit") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			cancel_button = ptr.get();
			return ptr;
		} else if(name == "regiments") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unit_regiments_text = ptr.get();
			return ptr;
		} else if(name == "men") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unit_men_text = ptr.get();
			return ptr;
		} else if(name == "morale_progress") {
			auto ptr = make_element_by_type<military_unit_morale_progress_bar>(state, id);
			unit_morale_progress = ptr.get();
			return ptr;
		} else if(name == "strength_progress") {
			auto ptr = make_element_by_type<military_unit_strength_progress_bar>(state, id);
			unit_strength_progress = ptr.get();
			return ptr;
		} else if(name == "moving") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			unit_moving_icon = ptr.get();
			return ptr;
		} else if(name == "digin") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			unit_digin_icon = ptr.get();
			return ptr;
		} else if(name == "combat") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			unit_combat_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		auto const& content = listbox_row_element_base<military_unit_info<T>>::content;

		bool is_building = !std::holds_alternative<T>(content);
		bool is_moving = false;
		bool is_digin = false;
		bool is_combat = false;

		if(is_building) {
			if(std::holds_alternative<dcon::province_land_construction_id>(content)) {
				auto c = std::get<dcon::province_land_construction_id>(content);
				unit_icon->frame =
						state.military_definitions.unit_base_definitions[state.world.province_land_construction_get_type(c)].icon - 1;
				unit_building_progress->progress = economy::unit_construction_progress(state, c);
			} else if(std::holds_alternative<dcon::province_naval_construction_id>(content)) {
				auto c = std::get<dcon::province_naval_construction_id>(content);
				unit_icon->frame =
						state.military_definitions.unit_base_definitions[state.world.province_naval_construction_get_type(c)].icon - 1;
				unit_building_progress->progress = economy::unit_construction_progress(state, c);
			}
		} else {
			auto regiments = 0;
			auto strength = 0.f;
			auto full_strength = 0.f;
			// Armies
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				state.world.army_for_each_army_membership_as_army(std::get<dcon::army_id>(content), [&](dcon::army_membership_id amid) {
					auto rid = state.world.army_membership_get_regiment(amid);
					full_strength += 1.f * state.defines.pop_size_per_regiment;
					strength += state.world.regiment_get_strength(rid) * state.defines.pop_size_per_regiment;
					++regiments;
				});
			}
			// Navies
			if constexpr(std::is_same_v<T, dcon::navy_id>) {
				state.world.navy_for_each_navy_membership_as_navy(std::get<dcon::navy_id>(content), [&](dcon::navy_membership_id nmid) {
					auto sid = state.world.navy_membership_get_ship(nmid);
					full_strength += 1.f;
					strength += state.world.ship_get_strength(sid);
					++regiments;
				});
			}
			unit_strength_progress->progress = (strength && full_strength) ? strength / full_strength : 0.f;
			unit_men_text->set_text(state, text::prettify(int32_t(strength)));
			unit_regiments_text->set_text(state, std::to_string(regiments));
		}

		unit_icon->set_visible(state, is_building);
		cancel_button->set_visible(state, is_building);
		eta_date_text->set_visible(state, is_building);
		location_text->set_visible(state, is_building);
		unit_building_progress->set_visible(state, is_building);

		unit_name->set_visible(state, !is_building);
		leader_icon->set_visible(state, !is_building);
		unit_regiments_text->set_visible(state, !is_building);
		unit_men_text->set_visible(state, !is_building);
		unit_morale_progress->set_visible(state, !is_building);
		unit_strength_progress->set_visible(state, !is_building);
		unit_moving_icon->set_visible(state, !is_building && is_moving);
		unit_digin_icon->set_visible(state, !is_building && is_digin);
		unit_combat_icon->set_visible(state, !is_building && is_combat);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		auto const& content = listbox_row_element_base<military_unit_info<T>>::content;
		if(payload.holds_type<dcon::province_id>()) {
			dcon::province_id p{};
			if(std::holds_alternative<dcon::province_land_construction_id>(content)) {
				auto c = std::get<dcon::province_land_construction_id>(content);
				p = state.world.pop_location_get_province(
						state.world.pop_get_pop_location_as_pop(state.world.province_land_construction_get_pop(c)));
			} else if(std::holds_alternative<dcon::province_naval_construction_id>(content)) {
				auto c = std::get<dcon::province_naval_construction_id>(content);
				p = state.world.province_naval_construction_get_province(c);
			}
			payload.emplace<dcon::province_id>(p);
			return message_result::consumed;
		}
		return listbox_row_element_base<military_unit_info<T>>::get(state, payload);
	}
};

template<typename T>
class military_units_listbox : public listbox_element_base<military_unit_entry<T>, military_unit_info<T>> {
protected:
	std::string_view get_row_element_name() override {
		return "unit_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		auto& row_contents = listbox_element_base<military_unit_entry<T>, military_unit_info<T>>::row_contents;
		row_contents.clear();
		if(listbox_element_base<military_unit_entry<T>, military_unit_info<T>>::parent) {
			Cyto::Any payload = dcon::nation_id{};
			listbox_element_base<military_unit_entry<T>, military_unit_info<T>>::parent->impl_get(state, payload);
			dcon::nation_id n = Cyto::any_cast<dcon::nation_id>(payload);
			// Armies
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				state.world.nation_for_each_army_control_as_controller(n, [&](dcon::army_control_id acid) {
					auto aid = state.world.army_control_get_army(acid);
					row_contents.push_back(military_unit_info<T>{aid});
				});
				state.world.nation_for_each_province_land_construction_as_nation(n,
						[&](dcon::province_land_construction_id p) { row_contents.push_back(military_unit_info<T>{p}); });
			}
			// Navies
			if constexpr(std::is_same_v<T, dcon::navy_id>) {
				state.world.nation_for_each_navy_control_as_controller(n, [&](dcon::navy_control_id ncid) {
					auto nid = state.world.navy_control_get_navy(ncid);
					row_contents.push_back(military_unit_info<T>{nid});
				});
				state.world.nation_for_each_province_naval_construction_as_nation(n,
						[&](dcon::province_naval_construction_id p) { row_contents.push_back(military_unit_info<T>{p}); });
			}
		}
		listbox_element_base<military_unit_entry<T>, military_unit_info<T>>::update(state);
	}
};

template<class T>
class build_unit_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.ui_state.unit_window_army->set_visible(state, false);
		state.ui_state.unit_window_navy->set_visible(state, false);

		state.ui_state.build_unit_window->set_visible(state, true);
		state.ui_state.root->move_child_to_front(state.ui_state.build_unit_window);

		if constexpr(std::is_same_v<T, dcon::army_id>) {
			Cyto::Any payload = dcon::army_id(1);
			state.ui_state.build_unit_window->impl_set(state, payload);
		} else if constexpr(std::is_same_v<T, dcon::navy_id>) {
			Cyto::Any payload = dcon::navy_id(1);
			state.ui_state.build_unit_window->impl_set(state, payload);
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			text::localised_format_box(state, contents, box, std::string_view("military_build_army_tooltip"));
		} else if constexpr(std::is_same_v<T, dcon::navy_id>) {
			text::localised_format_box(state, contents, box, std::string_view("military_build_navy_tooltip"));
		}
		text::close_layout_box(contents, box);
	}	
};

class military_armies_text : public nation_armies_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_single_sub_box(state, contents, box, std::string_view("military_army_count_tooltip"),
				text::variable_type::value, get_num_armies(state, state.local_player_nation));
		text::close_layout_box(contents, box);
	}
};

class military_navies_text : public nation_navies_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_single_sub_box(state, contents, box, std::string_view("military_navy_count_tooltip"),
				text::variable_type::value, get_num_navies(state, state.local_player_nation));
		text::close_layout_box(contents, box);
	}
};

class military_units_construction_text : public simple_text_element_base {
public:
};

class military_units_sortby_name : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("military_sort_by_name_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class military_units_sortby_strength : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("military_sort_by_strength_tooltip"));
		text::close_layout_box(contents, box);
	}
};

template<class T>
class military_units_window : public window_element_base {
private:
	image_element_base* cdts_icon = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "current_count") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<military_armies_text>(state, id);
			} else {
				return make_element_by_type<military_navies_text>(state, id);
			}
		} else if(name == "under_construction") {
			return make_element_by_type<military_units_construction_text>(state, id);

		} else if(name == "cut_down_to_size") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			cdts_icon = ptr.get();
			return ptr;

		} else if(name == "sort_name") {
			return make_element_by_type<military_units_sortby_name>(state, id);

		} else if(name == "sort_strength") {
			return make_element_by_type<military_units_sortby_strength>(state, id);

		} else if(name == "build_new") {
			auto ptr = make_element_by_type<build_unit_button<T>>(state, id);
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				ptr->set_button_text(state, text::produce_simple_string(state, "military_build_army_label"));
			} else {
				ptr->set_button_text(state, text::produce_simple_string(state, "military_build_navy_label"));
			}
			ptr->set_visible(state, true);
			return nullptr;

		} else if(name == "unit_listbox") {
			return make_element_by_type<military_units_listbox<T>>(state, id);

		} else {
			return nullptr;
		}
	}
};

} // namespace ui
