#pragma once 

#include "gui_element_types.hpp"

namespace ui {

class tooltip_box : public multiline_text_element_base {
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            auto layout = text::create_endless_layout(internal_layout,
                text::layout_parameters{0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0,
                    text::alignment::left, text::text_color::white});

            auto box = text::open_layout_box(layout);
            text::add_to_layout_box(state, layout, box, std::string_view("Fuck"));
            text::close_layout_box(layout, box);
        }
    }
};

class map_tooltip : public window_element_base {
    multiline_text_element_base* t_text = nullptr;
    image_element_base* background = nullptr;
public:
    void on_create(sys::state& state) noexcept override {
        {
            auto win1 = make_element_by_type<tooltip_box>(state, state.ui_state.defs_by_name.find("tooltip")->second.definition);
            base_data.size = win1->base_data.size;
            t_text = win1.get();
			add_child_to_front(std::move(win1));
        }
        {
            auto win2 = make_element_by_type<image_element_base>(state, state.ui_state.defs_by_name.find("tooltip")->second.definition);
            background = win2.get();
			add_child_to_front(std::move(win2));
        }
    }

    void on_update(sys::state& state) noexcept override {
        background->base_data.position.x = base_data.position.x;
        background->base_data.position.y = base_data.position.y;
        t_text->base_data.position.x = base_data.position.x;
        t_text->base_data.position.y = base_data.position.y;
    }
};

}