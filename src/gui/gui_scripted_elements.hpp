#pragma once

namespace ui {

int32_t frame_from_datamodel(sys::state& state, ui::datamodel datamodel) {
	// US9AC3
	if(datamodel == ui::datamodel::state_religion) {
		auto country = state.local_player_nation;
		auto fat_id = dcon::fatten(state.world, state.world.nation_get_religion(country));
		return int32_t(fat_id.get_icon() - 1);
	}

	return 0;
}


// US9AC3 When an icon has `datamodel="state_religion"`, it always displays the state religion of the player
class icon_w_datamodel : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		frame = frame_from_datamodel(state, base_data.datamodel);
	}
};

class province_script_button : public button_element_base {
public:
	dcon::gui_def_id base_definition;
	bool visible = true;

	province_script_button(dcon::gui_def_id base_definition) : base_definition(base_definition) { }
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		flags |= ui::element_base::wants_update_when_hidden_mask;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible) {
			button_element_base::render(state, x, y);
		}
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};
class nation_script_button : public button_element_base {
public:
	dcon::gui_def_id base_definition;
	bool visible = true;

	nation_script_button(dcon::gui_def_id base_definition) : base_definition(base_definition) { }
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		flags |= ui::element_base::wants_update_when_hidden_mask;
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible) {
			button_element_base::render(state, x, y);
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

// US8AC2 When clicking button with toggle_ui_key the associated UI variable is toggled True/False
// We treat UI variable toggle buttons as nation buttons but with toggle_ui_key instead of the effect
class ui_variable_toggle_button : public button_element_base {
public:
	dcon::gui_def_id base_definition;
	bool visible = true;

	ui_variable_toggle_button(dcon::gui_def_id base_definition) : base_definition(base_definition) { }

	void button_action(sys::state& state) noexcept override {
		auto var = base_data.data.button.toggle_ui_key;
		state.world.ui_variable_set_value(var, !state.world.ui_variable_get_value(var));
		state.ui_state.root->impl_on_update(state);
	};

	void on_update(sys::state& state) noexcept;

	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		flags |= ui::element_base::wants_update_when_hidden_mask;
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible) {
			button_element_base::render(state, x, y);
		}
	}
};

// US8AC4 Window is shown only when UI variable in `visible_ui_key` is set to True
class ui_variable_toggleable_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		flags |= ui::element_base::wants_update_when_hidden_mask;
	}
	void on_update(sys::state& state) noexcept override {
		window_element_base::on_update(state);

		auto var = base_data.data.window.visible_ui_key;

		if(is_visible() != state.world.ui_variable_get_value(var)) {
			set_visible(state, state.world.ui_variable_get_value(var));
		}
	};
};

} // namespace ui
