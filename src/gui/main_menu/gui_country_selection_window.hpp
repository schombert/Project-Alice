#pragma once

#include "gui_element_types.hpp"
#include "gui_multiplayer_suchen.hpp"
#include "gui_mainmenu_panel.hpp"

namespace ui {

class country_selection_window : public window_element_base {
private:
	image_element_base* black_background = nullptr;
	image_element_base* art_background = nullptr;
	window_element_base* multiplayer_menu = nullptr;
	window_element_base* mainmenu_panel_win = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		state.ui_state.main_menu_win = this;
		base_data.position.x -= 100;
		base_data.position.y += 61;
	}

	void on_update(sys::state& state) noexcept override {
		int width = 0;
		int height = 0;
		window::get_window_size(state, width, height);
		black_background->base_data.size.x = width;
		black_background->base_data.size.y = height;

		art_background->base_data.position.x = (width - art_background->base_data.size.x) - 60;

		mainmenu_panel_win->base_data.position.x = ((width / 2) - (mainmenu_panel_win->base_data.size.x / 2)) - 126;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<image_element_base>(state, id);	// Black Background for larger screens
			ptr->base_data.position.x = 100;
			ptr->base_data.position.y = -61;
			black_background = ptr.get();
			return ptr;

		} else if(name == "frontend_backdrop") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->base_data.position.x += 890;
			art_background = ptr.get();
			return ptr;

		} else if(name == "frontend_toptile1") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile2") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile3") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile4") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile5") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile1") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile2") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile3") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile4") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile5") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "version_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			// TODO - we want to display the current version of PA here
			ptr->base_data.position.x = 0;
			ptr->base_data.position.y = 0;
			ptr->set_text(state, "Project Alice Pre-1.0");
			return ptr;

		} else if(name == "multiplayer_menu") {
			auto ptr = make_element_by_type<multiplayer_menu_window>(state, id);
			ptr->base_data.position.x += 550;
			ptr->set_visible(state, false);
			multiplayer_menu = ptr.get();
			return ptr;

		} else if(name == "multiplayer_connect_menu") {
			auto ptr = make_element_by_type<multiplayer_metaserver_connect_window>(state, id);
			ptr->base_data.position.x += 350;
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "mainmenu_panel") {
			auto ptr = make_element_by_type<mainmenu_panel>(state, id);
			ptr->base_data.position.x += 900; // Nudge
			mainmenu_panel_win = ptr.get();
			return ptr;

		} else if(name == "tutorial_panel") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "back_button") {	// Handle Junk Button
			auto ptr = make_element_by_type<opaque_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "start_button") {	// Handle Junk Button
			auto ptr = make_element_by_type<opaque_element_base>(state, id);
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
				case mainmenu_action::mp:
					multiplayer_menu->is_visible() ? multiplayer_menu->set_visible(state, false) : multiplayer_menu->set_visible(state, true);
					multiplayer_menu->impl_on_update(state);
					break;
				case mainmenu_action::sp:
					set_visible(state, false);
				default:
					break;
			};
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
