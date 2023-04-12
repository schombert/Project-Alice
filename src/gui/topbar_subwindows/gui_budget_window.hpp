#pragma once

#include "gui_element_types.hpp"

namespace ui {

class budget_tariffs_percent : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto tariffs = state.world.nation_get_tariffs(state.local_player_nation);
		set_text(state, std::to_string(tariffs) + "%");
	}
};

class budget_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "tariffs_percent") {
			return make_element_by_type<budget_tariffs_percent>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
