#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {
class error_body_text : public simple_multiline_body_text {
public:
	std::string msg = "";
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, msg, text::text_color::white);
		text::close_layout_box(contents, box);
	}
};
class error_dialog_window : public window_element_base {
public:
	simple_text_element_base* title = nullptr;
	error_body_text* body = nullptr;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "title") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			title = ptr.get();
			return ptr;
		} else if(name == "description") {
			auto ptr = make_element_by_type<error_body_text>(state, id);
			body = ptr.get();
			return ptr;
		} else if(name == "agreebutton") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size;
			return ptr;
		} else {
			return nullptr;
		}
	}
};
void popup_error_window(sys::state& state, std::string_view title, std::string_view body);
} // namespace ui
