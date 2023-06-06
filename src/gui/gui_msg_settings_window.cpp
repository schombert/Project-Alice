#include "gui_msg_settings_window.hpp"

namespace ui {
void msg_log_text::on_update(sys::state& state) noexcept  {
	if(parent) {
		Cyto::Any payload = int32_t{};
		parent->impl_get(state, payload);
		int32_t index = any_cast<int32_t>(payload);

		auto& messages = static_cast<ui::msg_log_window*>(state.ui_state.message_log_window)->messages;
		if(index < int32_t(messages.size())) {
			auto& m = messages[index];
			m.body(state, internal_layout);
		}
	}
}

void msg_log_window::on_create(sys::state& state) noexcept {
	window_element_base::on_create(state);
	cat_filters.resize(size_t(msg_settings_category::count));
}

std::unique_ptr<element_base> msg_log_window::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "close") {
		return make_element_by_type<generic_close_button>(state, id);
	} else if(name == "messagelog") {
		auto ptr = make_element_by_type<msg_log_listbox>(state, id);
		log_list = ptr.get();
		return ptr;
	} else if(name == "messagecat_combat") {
		return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::combat>>(state, id);
	} else if(name == "messagecat_diplomacy") {
		return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::diplomacy>>(state, id);
	} else if(name == "messagecat_units") {
		return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::units>>(state, id);
	} else if(name == "messagecat_provinces") {
		return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::provinces>>(state, id);
	} else if(name == "messagecat_events") {
		return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::events>>(state, id);
	} else if(name == "messagecat_others") {
		return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::others>>(state, id);
	} else {
		return nullptr;
	}
}

void msg_log_window::on_update(sys::state& state) noexcept {
	while(messages.size() >= 100)
		messages.pop_back();
	
	log_list->row_contents.clear();
	for(int32_t i = 0; i < int32_t(messages.size()); ++i)
		log_list->row_contents.push_back(i);
	log_list->update(state);
}

message_result msg_log_window::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<msg_settings_category>()) {
		auto cat = any_cast<msg_settings_category>(payload);
		payload.emplace<bool>(cat_filters[uint8_t(cat)]);
		return message_result::consumed;
	} else if(payload.holds_type<element_selection_wrapper<msg_settings_category>>()) {
		auto cat = any_cast<element_selection_wrapper<msg_settings_category>>(payload).data;
		cat_filters[uint8_t(cat)] = !cat_filters[uint8_t(cat)];
		impl_on_update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}
}
