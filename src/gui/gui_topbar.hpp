#pragma once

#include "gui_element_types.hpp"
#include "gui_production_window.hpp"
#include "gui_military_window.hpp"

namespace ui {

class background_image : public opaque_element_base {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		base_data.size.x = int16_t(ui_width(state));
		base_data.size.y = int16_t(ui_height(state));
		opaque_element_base::render(state, x, y);
	}

	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(is_visible()) {
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class topbar_production_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.production_window) {
			auto new_pw = make_element_by_type<production_window>(state, "country_production");
			state.ui_state.production_window = new_pw.get();
			state.ui_state.root->add_child_to_front(std::move(new_pw));
		} else if(state.ui_state.production_window->is_visible()) {
			state.ui_state.production_window->set_visible(state, false);
			state.ui_state.root->move_child_to_back(state.ui_state.production_window);
		} else {
			state.ui_state.production_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.production_window);
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		frame = int32_t(state.ui_state.production_window && state.ui_state.production_window->is_visible());
		button_element_base::render(state, x, y);
	}
};

class topbar_military_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.military_window) {
			auto new_pw = make_element_by_type<military_window>(state, "country_production");
			state.ui_state.military_window = new_pw.get();
			state.ui_state.root->add_child_to_front(std::move(new_pw));
		} else if(state.ui_state.military_window->is_visible()) {
			state.ui_state.military_window->set_visible(state, false);
			state.ui_state.root->move_child_to_back(state.ui_state.military_window);
		} else {
			state.ui_state.military_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.military_window);
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		frame = int32_t(state.ui_state.military_window && state.ui_state.military_window->is_visible());
		button_element_base::render(state, x, y);
	}
};

class topbar_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto tc_prod = make_element_by_type<background_image>(state, "topcolor_production");
		production_topcolor = tc_prod.get();
		add_child_to_back(std::move(tc_prod));
		auto bg_pic = make_element_by_type<background_image>(state, "bg_main_menus");
		background_pic = bg_pic.get();
		add_child_to_back(std::move(bg_pic));
	}
	
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "topbarbutton_production") {
			return make_element_by_type<topbar_production_button>(state, id);
		}
		else if(name == "topbarbutton_military") {
			return make_element_by_type<topbar_military_button>(state, id);
		}
		else {
			return nullptr;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.ui_state.production_window && state.ui_state.production_window->is_visible()) {
			production_topcolor->set_visible(state, true);
			background_pic->set_visible(state, true);
		} else {
			production_topcolor->set_visible(state, false);
			background_pic->set_visible(state, false);
		}
		window_element_base::render(state, x, y);
	}

private:
	element_base* production_topcolor;
	element_base* background_pic;
};

}
