#pragma once

#include "gui_element_types.hpp"
#include "gui_production_window.hpp"
#include "gui_diplomacy_window.hpp"
#include "gui_technology_window.hpp"
#include "gui_politics_window.hpp"
#include "gui_budget_window.hpp"
#include "gui_trade_window.hpp"
#include "gui_population_window.hpp"
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

class topbar_tab_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		const auto override_and_show_tab = [&]() {
			//state.ui_state.topbar_window->background_pic->set_visible(state, true);

			tab_background_image->set_visible(state, true);
			state.ui_state.topbar_window->move_child_to_front(tab_background_image);
			state.ui_state.tab_background_image = tab_background_image;

			tab_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(tab_window);
			state.ui_state.tab_window = tab_window;
		};

		if(state.ui_state.tab_window->is_visible() || state.ui_state.tab_background_image->is_visible()) {
			//state.ui_state.topbar_window->background_pic->set_visible(state, false);

			state.ui_state.tab_window->set_visible(state, false);
			state.ui_state.tab_background_image->set_visible(state, false);
			if(state.ui_state.tab_window != tab_window || state.ui_state.tab_background_image != tab_background_image) {
				override_and_show_tab();
			}
		} else {
			override_and_show_tab();
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		frame = int32_t(state.ui_state.tab_window == tab_window && state.ui_state.tab_window->is_visible());
		button_element_base::render(state, x, y);
	}

	element_base* tab_window = nullptr;
	element_base* tab_background_image = nullptr;
};

class topbar_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto bg_pic = make_element_by_type<background_image>(state, "bg_main_menus");
		background_pic = bg_pic.get();
		add_child_to_back(std::move(bg_pic));
		state.ui_state.topbar_window = this;
	}
	
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "topbarbutton_production") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			// Default to this tab... but keep it hidden!
			auto topcolor = make_element_by_type<background_image>(state, "topcolor_production");
			topcolor->set_visible(state, false);
			state.ui_state.tab_background_image = btn->tab_background_image = topcolor.get();
			add_child_to_back(std::move(topcolor));

			auto tab = make_element_by_type<production_window>(state, "country_production");
			state.ui_state.tab_window = btn->tab_window = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_budget") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto topcolor = make_element_by_type<background_image>(state, "topcolor_budget");
			topcolor->set_visible(state, false);
			btn->tab_background_image = topcolor.get();
			add_child_to_back(std::move(topcolor));

			auto tab = make_element_by_type<budget_window>(state, "country_budget");
			btn->tab_window = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_tech") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto topcolor = make_element_by_type<background_image>(state, "topcolor_technology");
			topcolor->set_visible(state, false);
			btn->tab_background_image = topcolor.get();
			add_child_to_back(std::move(topcolor));

			auto tab = make_element_by_type<technology_window>(state, "country_technology");
			btn->tab_window = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_politics") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto topcolor = make_element_by_type<background_image>(state, "topcolor_politics");
			topcolor->set_visible(state, false);
			btn->tab_background_image = topcolor.get();
			add_child_to_back(std::move(topcolor));

			auto tab = make_element_by_type<politics_window>(state, "country_politics");
			btn->tab_window = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_pops") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto topcolor = make_element_by_type<background_image>(state, "topcolor_pop");
			topcolor->set_visible(state, false);
			btn->tab_background_image = topcolor.get();
			add_child_to_back(std::move(topcolor));

			auto tab = make_element_by_type<population_window>(state, "country_pop");
			btn->tab_window = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_trade") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto topcolor = make_element_by_type<background_image>(state, "topcolor_trade");
			topcolor->set_visible(state, false);
			btn->tab_background_image = topcolor.get();
			add_child_to_back(std::move(topcolor));

			auto tab = make_element_by_type<trade_window>(state, "country_trade");
			btn->tab_window = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_diplomacy") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto topcolor = make_element_by_type<background_image>(state, "topcolor_diplomacy");
			topcolor->set_visible(state, false);
			btn->tab_background_image = topcolor.get();
			add_child_to_back(std::move(topcolor));

			auto tab = make_element_by_type<diplomacy_window>(state, "country_diplomacy");
			btn->tab_window = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_military") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto topcolor = make_element_by_type<background_image>(state, "topcolor_military");
			topcolor->set_visible(state, false);
			btn->tab_background_image = topcolor.get();
			add_child_to_back(std::move(topcolor));

			auto tab = make_element_by_type<military_window>(state, "country_military");
			btn->tab_window = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else {
			return nullptr;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.ui_state.tab_window && state.ui_state.tab_window->is_visible()) {
			background_pic->set_visible(state, true);
		} else {
			background_pic->set_visible(state, false);
		}
		window_element_base::render(state, x, y);
	}

private:
	element_base* background_pic = nullptr;

	friend class topbar_tab_button;
};

}
