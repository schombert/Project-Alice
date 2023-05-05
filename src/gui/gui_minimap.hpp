#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_ledger_window.hpp"
#include "gui_search_window.hpp"
#include "opengl_wrapper.hpp"
#include "map.hpp"
#include "map_modes.hpp"
#include <glm/glm.hpp>

namespace ui {

class minimap_mapmode_button : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override {
		map_mode::set_map_mode(state, target);
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
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_8")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::civilization_level:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_19")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::colonial:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_7")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::crisis:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_21")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::diplomatic:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_4")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::infrastructure:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_6")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::migration:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_18")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::naval:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_22")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::nationality:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_13")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::national_focus:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_10")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::party_loyalty:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_16")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::political:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_2")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::population:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_12")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::rank:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_17")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::recruitment:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_9")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::region:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_5")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::relation:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_20")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::revolt:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_3")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::rgo_output:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_11")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::sphere:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_14")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::supply:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_15")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
				break;
			case map_mode::mode::terrain:
				if(auto k = state.key_to_text_sequence.find(std::string_view("mapmode_1")); k != state.key_to_text_sequence.end())
					text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{});
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
};

class minimap_container_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "messagelog_window") {
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
			//chat_window
		} else if(name == "chat_window") {
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "button_goto") {
			return make_element_by_type<minimap_goto_button>(state, id);
		} else if(name == "ledger_button") {
			return make_element_by_type<minimap_ledger_button>(state, id);
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

private:
	const std::string_view mapmode_btn_prefix{ "mapmode_" };
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

}
