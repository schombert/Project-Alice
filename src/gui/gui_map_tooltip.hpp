#pragma once 

#include "gui_element_types.hpp"

namespace ui {

class map_tooltip : public opaque_element_base {
protected:
    void debug_tooltip_box(sys::state& state, text::columnar_layout& contents) {
        auto box = text::open_layout_box(contents);
        text::add_to_layout_box(state, contents, box, std::string_view("UwU"));
        text::close_layout_box(contents, box);
    }
public:
    void on_create(sys::state& state) noexcept override {
        state.ui_state.map_tooltip_window = this;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
        return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
        debug_tooltip_box(state, contents);
    }
};

}