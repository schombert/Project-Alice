#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_ledger_window.hpp"
#include "gui_search_window.hpp"
#include "gui_main_menu.hpp"
#include "gui_message_filters_window.hpp"
#include "opengl_wrapper.hpp"
#include "map.hpp"
#include "map_modes.hpp"
#include <glm/glm.hpp>

namespace ui {

class minimap_mapmode_button : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override {
		map_mode::set_map_mode(state, target);
		// Update elements...
		state.ui_state.root->impl_on_update(state);
	}

	bool is_active(sys::state& state) noexcept override {
		return state.map_state.active_map_mode == target;
	}

	map_mode::mode target = map_mode::mode::terrain;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		switch(target) {
		case map_mode::mode::admin:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_8"));
			break;
		case map_mode::mode::civilization_level:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_19"));
			break;
		case map_mode::mode::colonial:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_7"));
			break;
		case map_mode::mode::crisis:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_21"));
			break;
		case map_mode::mode::diplomatic:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_4"));
			break;
		case map_mode::mode::infrastructure:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_6"));
			break;
		case map_mode::mode::migration:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_18"));
			break;
		case map_mode::mode::naval:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_22"));
			break;
		case map_mode::mode::nationality:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_13"));
			break;
		case map_mode::mode::national_focus:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_10"));
			break;
		case map_mode::mode::party_loyalty:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_16"));
			break;
		case map_mode::mode::political:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_2"));
			break;
		case map_mode::mode::population:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_12"));
			break;
		case map_mode::mode::rank:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_17"));
			break;
		case map_mode::mode::recruitment:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_9"));
			break;
		case map_mode::mode::region:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_5"));
			break;
		case map_mode::mode::relation:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_20"));
			break;
		case map_mode::mode::revolt:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_3"));
			break;
		case map_mode::mode::rgo_output:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_11"));
			break;
		case map_mode::mode::sphere:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_14"));
			break;
		case map_mode::mode::supply:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_15"));
			break;
		case map_mode::mode::terrain:
			text::localised_format_box(state, contents, box, std::string_view("mapmode_1"));
			break;
		}
		text::close_layout_box(contents, box);
	}
};

class minimap_goto_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.search_window) {
			auto window = make_element_by_type<province_search_window>(state, "goto_box");
			state.ui_state.search_window = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.search_window->is_visible()) {
			state.ui_state.search_window->set_visible(state, false);
		} else {
			state.ui_state.search_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.search_window);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("goto_goto"));
		text::close_layout_box(contents, box);
	}
};

class minimap_ledger_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.ledger_window) {
			auto window = make_element_by_type<ledger_window>(state, "ledger");
			state.ui_state.ledger_window = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.ledger_window->is_visible()) {
			state.ui_state.ledger_window->set_visible(state, false);
		} else {
			state.ui_state.ledger_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.ledger_window);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("m_ledger_button"));
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_settings_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.msg_filters_window) {
			auto window = make_element_by_type<message_filters_window>(state, "message_filters");
			state.ui_state.msg_filters_window = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.msg_filters_window->is_visible()) {
			state.ui_state.msg_filters_window->set_visible(state, false);
		} else {
			state.ui_state.msg_filters_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.msg_filters_window);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_settings"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_combat_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_combat"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_diplo_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_diplo"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_unit_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_unit"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_province_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_province"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_other_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_other"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_event_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_event"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_menu_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.main_menu) {
			auto window = make_element_by_type<main_menu_window>(state, "alice_main_menu");
			state.ui_state.main_menu = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.main_menu->is_visible()) {
			state.ui_state.main_menu->set_visible(state, false);
		} else {
			state.ui_state.main_menu->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.main_menu);
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("m_menu_button"));
		text::close_layout_box(contents, box);
	}
};

struct open_msg_log_data {
	int dummy;
};
class open_msg_log_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		frame = state.ui_state.msg_log_window->is_visible() ? 1 : 0;
	}

	void button_action(sys::state& state) noexcept override {
		send(state, parent, open_msg_log_data{});
	}
};

class minimap_zoom_in_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.map_state.zoom = std::clamp(state.map_state.zoom * 2.0f,map::min_zoom, map::max_zoom);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "zoom_in");
	}
};
class minimap_zoom_out_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.map_state.zoom = std::clamp(state.map_state.zoom * 0.5f, map::min_zoom, map::max_zoom);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "zoom_out");
	}
};

class minimap_container_window : public window_element_base {
	const std::string_view mapmode_btn_prefix{"mapmode_"};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "messagelog_window") {
			auto ptr = make_element_by_type<message_log_window>(state, id);
			state.ui_state.msg_log_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "openbutton") {
			return make_element_by_type<open_msg_log_button>(state, id);
		} else if(name == "chat_window") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "menu_button") {
			return make_element_by_type<minimap_menu_button>(state, id);
		} else if(name == "button_goto") {
			return make_element_by_type<minimap_goto_button>(state, id);
		} else if(name == "ledger_button") {
			return make_element_by_type<minimap_ledger_button>(state, id);
		} else if(name == "map_zoom_in") {
			return make_element_by_type<minimap_zoom_in_button>(state, id);
		} else if(name == "map_zoom_out") {
			return make_element_by_type<minimap_zoom_out_button>(state, id);
		} else if(name == "menubar_msg_settings") {
			return make_element_by_type<minimap_msg_settings_button>(state, id);
		} else if(name == "menubar_msg_combat") {
			return make_element_by_type<minimap_msg_combat_button>(state, id);
		} else if(name == "menubar_msg_diplo") {
			return make_element_by_type<minimap_msg_diplo_button>(state, id);
		} else if(name == "menubar_msg_unit") {
			return make_element_by_type<minimap_msg_unit_button>(state, id);
		} else if(name == "menubar_msg_province") {
			return make_element_by_type<minimap_msg_province_button>(state, id);
		} else if(name == "menubar_msg_event") {
			return make_element_by_type<minimap_msg_event_button>(state, id);
		} else if(name == "menubar_msg_other") {
			return make_element_by_type<minimap_msg_other_button>(state, id);
		} else if(name == "menubar_plans_toggle") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "menubar_plans_open") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name.starts_with(mapmode_btn_prefix)) {
			auto ptr = make_element_by_type<minimap_mapmode_button>(state, id);
			size_t num_index = name.rfind("_") + 1;
			uint8_t num = 0;
			for(size_t i = num_index; i < name.size(); i++) {
				num *= 10;
				num += name[i] - '0';
			}
			ptr->target = static_cast<map_mode::mode>(num);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		base_data.size.x = int16_t(ui_width(state));
		base_data.size.y = int16_t(ui_height(state));
		window_element_base::render(state, x, y);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<open_msg_log_data>()) {
			state.ui_state.msg_log_window->set_visible(state, !state.ui_state.msg_log_window->is_visible());
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class minimap_picture_window : public opaque_element_base {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		opaque_element_base::render(state, x, y);
		// TODO draw white box to represent window borders
	}

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto minimap_size = glm::vec2(base_data.size.x, base_data.size.y);
		state.map_state.set_pos(glm::vec2(x, y) / minimap_size);
		return message_result::consumed;
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return message_result::unseen;
	}
};

} // namespace ui
