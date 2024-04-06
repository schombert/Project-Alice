#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {
class error_body_text : public scrollable_text {
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept {
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, msg);
		text::close_layout_box(contents, box);
	}
public:
	std::string msg = "";
	void on_create(sys::state& state) noexcept override {
		base_data.size.y = int16_t(150);
		scrollable_text::on_create(state);
	}
	void on_update(sys::state& state) noexcept override {
		text::alignment align = text::alignment::left;
		switch(base_data.data.text.get_alignment()) {
		case ui::alignment::right:
			align = text::alignment::right;
			break;
		case ui::alignment::centered:
			align = text::alignment::center;
			break;
		default:
			break;
		}
		auto border = base_data.data.text.border_size;
		auto color = delegate->black_text ? text::text_color::black : text::text_color::white;
		auto container = text::create_endless_layout(
			delegate->internal_layout,
			text::layout_parameters{
				border.x,
				border.y,
				int16_t(base_data.size.x - border.x * 2),
				int16_t(base_data.size.y - border.y * 2),
				base_data.data.text.font_handle,
				0,
				align,
				color,
				false });
		populate_layout(state, container);
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
			return make_element_by_type<generic_close_button>(state, id);
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
