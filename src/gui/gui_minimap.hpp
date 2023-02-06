#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "opengl_wrapper.hpp"
#include "map.hpp"
#include "map_modes.hpp"
#include <glm/glm.hpp>

namespace ui {

class minimap_mapmode_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		map_mode::set_map_mode(state, target);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		frame = int32_t(state.map_display.active_map_mode == target);
		button_element_base::render(state, x, y);
	}

	map_mode::mode target = map_mode::mode::terrain;
};

class minimap_container_window: public window_element_base {
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
        state.map_display.set_pos(glm::vec2(x, y) / minimap_size);
        return message_result::consumed;
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return message_result::unseen;
	}
};

}
