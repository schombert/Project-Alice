#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "constants.hpp"

namespace ui {

static std::string get_setting_text_key(sys::message_setting_type type) {
	switch(type) {
#define MSG_SETTING_ITEM(name, locale_name)                                                                                      \
	case sys::message_setting_type::name:                                                                                          \
		return locale_name "_setup";
		MSG_SETTING_LIST
#undef MSG_SETTING_ITEM
	default:
		return "???";
	}
}

class message_settings_item : public listbox_row_element_base<sys::message_setting_type> {
	simple_text_element_base* text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		text->set_text(state, text::produce_simple_string(state, get_setting_text_key(content)));
	}
};

class message_settings_listbox : public listbox_element_base<message_settings_item, sys::message_setting_type> {
protected:
	std::string_view get_row_element_name() override {
		return "message_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto i = 0; i != uint8_t(sys::message_setting_type::count); ++i) {
			row_contents.push_back(sys::message_setting_type(i));
		}
		update(state);
	}
};

enum class message_settings_category : uint8_t { all, combat, diplomacy, units, provinces, events, others, count };

class message_settings_window : public generic_tabbed_window<message_settings_category> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window<message_settings_category>::on_create(state);
		base_data.position.y -= 21; // Nudge
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "settings") {
			// Nudge required for listbox before it is created...
			state.ui_defs.gui[id].size.x -= 21; // Nudge
			auto ptr = make_element_by_type<message_settings_listbox>(state, id);
			ptr->base_data.position.x += 21; // Nudge
			ptr->base_data.position.y += 21; // Nudge
			return ptr;
		} else if(name == "messagecat_all") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::all;
			return ptr;
		} else if(name == "messagecat_combat") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::combat;
			return ptr;
		} else if(name == "messagecat_diplomacy") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::diplomacy;
			return ptr;
		} else if(name == "messagecat_units") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::units;
			return ptr;
		} else if(name == "messagecat_provinces") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::provinces;
			return ptr;
		} else if(name == "messagecat_events") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::events;
			return ptr;
		} else if(name == "messagecat_others") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::others;
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<message_settings_category>()) {
			auto content = any_cast<message_settings_category>(payload);
			active_tab = content;
			switch(content) {
				case message_settings_category::all:
					break;
				case message_settings_category::combat:
					break;
				case message_settings_category::diplomacy:
					break;
				case message_settings_category::units:
					break;
				case message_settings_category::provinces:
					break;
				case message_settings_category::events:
					break;
				case message_settings_category::others:
					break;
				default:
					break;
			};
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class message_log_text : public multiline_button_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};

class message_log_entry : public listbox_row_element_base<int32_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "messagelogbutton") {
			return make_element_by_type<message_log_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class message_log_listbox : public listbox_element_base<message_log_entry, int32_t> {
protected:
	std::string_view get_row_element_name() override {
		return "logtext";
	}
};

template<message_settings_category Filter>
class message_log_filter_checkbox : public checkbox_button {
	static std::string_view get_filter_text_key(message_settings_category f) noexcept {
		switch(f) {
		case message_settings_category::all:
			return "messagesettings_all";
		case message_settings_category::combat:
			return "messagesettings_combat";
		case message_settings_category::diplomacy:
			return "messagesettings_diplomacy";
		case message_settings_category::units:
			return "messagesettings_units";
		case message_settings_category::provinces:
			return "messagesettings_provinces";
		case message_settings_category::events:
			return "messagesettings_events";
		case message_settings_category::others:
			return "messagesettings_other";
		default:
			return "???";
		}
	}

public:
	bool is_active(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = Filter;
			parent->impl_get(state, payload);
			return any_cast<bool>(payload);
		}
		return false;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<message_settings_category>{Filter};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, get_filter_text_key(Filter));
		text::close_layout_box(contents, box);
	}
};

class message_log_window : public window_element_base {
	std::vector<bool> cat_filters;
	message_log_listbox* log_list = nullptr;

public:
	std::vector<notification::message> messages;

	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

#undef MSG_SETTING_LIST
} // namespace ui
