#pragma once

#include "gui_element_base.hpp"
#include "gui_element_types.hpp"
#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"
#include "notifications.hpp"

namespace ui {

class message_body_text;

class message_window : public window_element_base {
	simple_text_element_base* count_text = nullptr;
	int32_t index = 0;
	int32_t prev_size = 0;

	simple_text_element_base* title_text = nullptr;
	message_body_text* desc_text = nullptr;

public:
	std::vector<notification::message> messages;
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};
}
