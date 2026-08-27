#pragma once
#include "gui_listbox_templates.hpp"
#include "gui_diplomacy_request.hpp"

namespace ui {

class diplomatic_message_topbar_entry_window : public listbox_row_element_base<diplomatic_message::message> {
public:
	diplomatic_message_topbar_button* btn = nullptr;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

class diplomatic_message_topbar_listbox : public overlapping_listbox_element_base<diplomatic_message_topbar_entry_window, diplomatic_message::message> {
	std::string_view get_row_element_name() override {
		return "alice_diplomessageicon_entry_window";
	}
public:
	std::vector< diplomatic_message::message> messages;
	void on_update(sys::state& state) noexcept override {
		auto it = std::remove_if(messages.begin(), messages.end(), [&](auto& m) {
			return m.when + (int32_t)state.defines.alice_message_expiration_days <= state.current_date
				|| !diplomatic_message::can_accept(state, m);
		});
		messages.erase(it, messages.end());

		row_contents.clear();
		for(auto m : messages) {
			row_contents.push_back(m);
		}
		update(state);
	}
};
}
