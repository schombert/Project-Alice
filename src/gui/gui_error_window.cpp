#include "gui_error_window.hpp"

namespace ui {

void error_body_text::populate_layout(sys::state& state, text::endless_layout& contents) noexcept {
	auto box = text::open_layout_box(contents);
	text::add_to_layout_box(state, contents, box, msg);
	text::close_layout_box(contents, box);
}
void error_body_text::on_create(sys::state& state) noexcept {
	base_data.size.y = int16_t(150);
	scrollable_text::on_create(state);
}
void error_body_text::on_update(sys::state& state) noexcept {
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
	auto container = text::create_endless_layout(state, 
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

std::unique_ptr<element_base> error_dialog_window::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
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
void popup_error_window(sys::state& state, std::string_view title, std::string_view body) {
	if(state.ui_state.error_win == nullptr) {
		auto new_elm = ui::make_element_by_type<ui::error_dialog_window>(state, "defaultinfodialog");
		state.ui_state.error_win = new_elm.get();
		state.get_root_element()->add_child_to_front(std::move(new_elm));
	}

	auto win = static_cast<ui::error_dialog_window*>(state.ui_state.error_win);
	win->title->set_text(state, std::string(title));
	win->body->msg = std::string(body);
	win->set_visible(state, true);
	win->impl_on_update(state);
}
}
