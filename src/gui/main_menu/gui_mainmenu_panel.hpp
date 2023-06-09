#pragma once

#include "gui_element_types.hpp"

namespace ui {

class mainmenu_panel : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "single_player_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "multi_player_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "tutorial_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "settings_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "credits_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "exit_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "homepage_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "forum_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "facebook_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "twitter_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "nudge_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class tutorial_panel : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "frontend_tutorial_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "tutorial_tab_basic") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "tutorial_tab_medium") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "tutorial_tab_advanced") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "clergy_icon1") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "clergy_icon2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "clergy_icon3") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "basic_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "medium_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "advanced_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "basic_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "medium_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "advanced_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

} // namespace ui
