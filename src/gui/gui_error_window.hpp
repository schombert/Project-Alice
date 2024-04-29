#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include <string_view>

namespace ui {
class error_body_text : public scrollable_text {
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept;
public:
	std::string msg = "";
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class error_dialog_window : public window_element_base {
public:
	simple_text_element_base* title = nullptr;
	error_body_text* body = nullptr;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};
void popup_error_window(sys::state& state, std::string_view title, std::string_view body);
} // namespace ui
