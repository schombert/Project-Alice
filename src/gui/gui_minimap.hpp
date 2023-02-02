#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "opengl_wrapper.hpp"
#include <glm/glm.hpp>

namespace ui {

class minimap_container_window : public image_element_base {
public:
    void render(sys::state& state, int32_t x, int32_t y) noexcept override {
        image_element_base::render(state, x, y);
        // TODO draw white box to represent window borders
    }
    
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
        auto minimap_size = glm::vec2(base_data.size.x, base_data.size.y);
        state.map_display.set_pos(glm::vec2(x, y) / minimap_size);
        return message_result::consumed;
    }
};

}
