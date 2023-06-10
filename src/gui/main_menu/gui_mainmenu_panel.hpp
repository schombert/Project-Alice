#pragma once

#include "gui_element_types.hpp"
#include "gui_mainmenu_settings.hpp"

namespace ui {

enum class mainmenu_action : uint8_t { settings, credits, sp, mp, tutorial, homepage };

class mainmenu_options_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = element_selection_wrapper<mainmenu_action>{mainmenu_action{mainmenu_action::settings}};
		parent->impl_get(state, payload);
	}
};

class mainmenu_multiplayer_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = element_selection_wrapper<mainmenu_action>{mainmenu_action{mainmenu_action::mp}};
		parent->impl_get(state, payload);
	}
};

class mainmenu_singleplayer_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = element_selection_wrapper<mainmenu_action>{mainmenu_action{mainmenu_action::sp}};
		parent->impl_get(state, payload);
	}
};

class mainmenu_panel : public window_element_base {
private:
	mainmenu_settings_window* mainmenu_settings_win = nullptr;
	multiplayer_menu_window* mainmenu_multiplayer_win = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		{
			auto new_win = make_element_by_type<mainmenu_settings_window>(state,
					state.ui_state.defs_by_name.find("menu_settings")->second.definition);
			new_win->set_visible(state, false);
			mainmenu_settings_win = new_win.get();
			add_child_to_front(std::move(new_win));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "single_player_button") {
			return make_element_by_type<mainmenu_singleplayer_button>(state,
					id); // TODO - replace with proper button, this is just temp

		} else if(name == "multi_player_button") {
			return make_element_by_type<mainmenu_multiplayer_button>(state, id);

		} else if(name == "tutorial_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->disabled = true;
			return ptr;

		} else if(name == "settings_button") {
			return make_element_by_type<mainmenu_options_button>(state, id);

		} else if(name == "credits_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "exit_button") {
			return make_element_by_type<close_application_button>(state, id);

		} else if(name == "homepage_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "forum_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->disabled = true;
			return ptr;

		} else if(name == "facebook_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->disabled = true;
			return ptr;

		} else if(name == "twitter_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->disabled = true;
			return ptr;

		} else if(name == "nudge_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<mainmenu_action>>()) {
			auto content = any_cast<element_selection_wrapper<mainmenu_action>>(payload).data;
			switch(content) {
			case mainmenu_action::settings:
				mainmenu_settings_win->is_visible() ? mainmenu_settings_win->set_visible(state, false)
																						: mainmenu_settings_win->set_visible(state, true);
				mainmenu_settings_win->impl_on_update(state);
				break;
			case mainmenu_action::mp:
				return message_result::unseen;
			case mainmenu_action::sp:
				return message_result::unseen;
			default:
				break;
			};
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
