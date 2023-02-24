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
#include "text.hpp"

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

class topbar_tab_button : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override {
		const auto override_and_show_tab = [&]() {
			topbar_subwindow->set_visible(state, true);
			state.ui_state.root->move_child_to_front(topbar_subwindow);
			state.ui_state.topbar_subwindow = topbar_subwindow;
		};

		if(state.ui_state.topbar_subwindow->is_visible()) {
			state.ui_state.topbar_subwindow->set_visible(state, false);
			if(state.ui_state.topbar_subwindow != topbar_subwindow) {
				override_and_show_tab();
			}
		} else {
			override_and_show_tab();
		}
	}

	bool is_active(sys::state& state) noexcept override {
		return state.ui_state.topbar_subwindow == topbar_subwindow && state.ui_state.topbar_subwindow->is_visible();
	}

	element_base* topbar_subwindow = nullptr;
};

class topbar_date_text : public simple_text_element_base {
private:
	std::string localize_month(sys::state& state, uint16_t month) {
		switch (month) {
		case 1:
			return text::produce_simple_string(state, "January");
		case 2:
			return text::produce_simple_string(state, "February");
		case 3:
			return text::produce_simple_string(state, "March");
		case 4:
			return text::produce_simple_string(state, "April");
		case 5:
			return text::produce_simple_string(state, "May");
		case 6:
			return text::produce_simple_string(state, "June");
		case 7:
			return text::produce_simple_string(state, "July");
		case 8:
			return text::produce_simple_string(state, "August");
		case 9:
			return text::produce_simple_string(state, "September");
		case 10:
			return text::produce_simple_string(state, "October");
		case 11:
			return text::produce_simple_string(state, "November");
		case 12:
			return text::produce_simple_string(state, "December");
		default:
			return text::produce_simple_string(state, "January");
		}
	}

	std::string date_to_string(sys::state& state, sys::date date) {
		sys::year_month_day ymd = date.to_ymd(state.start_date);
		return localize_month(state, ymd.month) + " " + std::to_string(ymd.day) + ", " + std::to_string(ymd.year);
	}

public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, date_to_string(state, state.current_date));
	}

	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		on_update(state);
	}
};

class topbar_player_flag : public flag_button {
public:
	dcon::nation_id get_current_nation(sys::state& state) noexcept override {
		return state.local_player_nation;
	}
};

class topbar_pause_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(state.actual_game_speed <= 0) {
			state.actual_game_speed = state.ui_state.held_game_speed;
		} else {
			state.ui_state.held_game_speed = state.actual_game_speed.load();
			state.actual_game_speed = 0;
		}
	}

	void on_update(sys::state& state) noexcept override {
		disabled = state.internally_paused;
	}
};

class topbar_speedup_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::ADD;
	}

	void button_action(sys::state& state) noexcept override {
		if(state.actual_game_speed > 0) {
			state.actual_game_speed = std::min(5, state.actual_game_speed.load() + 1);
		} else {
			state.ui_state.held_game_speed = std::min(5, state.ui_state.held_game_speed + 1);
		}
	}
};

class topbar_speeddown_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::MINUS;
	}

	void button_action(sys::state& state) noexcept override {
		if(state.actual_game_speed > 0) {
			state.actual_game_speed = std::max(1, state.actual_game_speed.load() - 1);
		} else {
			state.ui_state.held_game_speed = std::max(1, state.ui_state.held_game_speed - 1);
		}
	}
};

class topbar_speed_indicator : public topbar_pause_button {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::SPACE;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.internally_paused) {
			frame = 0;
		} else {
			frame = state.actual_game_speed;
		}
		topbar_pause_button::render(state, x, y);
	}
};

class topbar_country_name : public simple_text_element_base {
private:
	dcon::nation_id current_nation{};

public:
	void on_update(sys::state& state) noexcept override {
		if(current_nation != state.local_player_nation) {
			if(bool(state.local_player_nation)) {
				dcon::nation_fat_id fat_id = dcon::fatten(state.world, state.local_player_nation);
				set_text(state, text::produce_simple_string(state, fat_id.get_name()));
			} else {
				set_text(state, "");
			}
			current_nation = state.local_player_nation;
		}
	}

	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		on_update(state);
	}
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

			auto tab = make_element_by_type<production_window>(state, "country_production");
			state.ui_state.topbar_subwindow = btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_budget") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<budget_window>(state, "country_budget");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_tech") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<technology_window>(state, "country_technology");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_politics") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<politics_window>(state, "country_politics");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_pops") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<population_window>(state, "country_pop");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_trade") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<trade_window>(state, "country_trade");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_diplomacy") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<diplomacy_window>(state, "country_diplomacy");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_military") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<military_window>(state, "country_military");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "button_speedup") {
			return make_element_by_type<topbar_speedup_button>(state, id);
		} else if(name == "button_speeddown") {
			return make_element_by_type<topbar_speeddown_button>(state, id);
		} else if(name == "pause_bg") {
			return make_element_by_type<topbar_pause_button>(state, id);
		} else if(name == "speed_indicator") {
			return make_element_by_type<topbar_speed_indicator>(state, id);
		} else if(name == "datetext") {
			return make_element_by_type<topbar_date_text>(state, id);
		} else if(name == "countryname") {
			return make_element_by_type<topbar_country_name>(state, id);
		} else if(name == "player_flag") {
			return make_element_by_type<topbar_player_flag>(state, id);
		} else {
			return nullptr;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.ui_state.topbar_subwindow->is_visible()) {
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
