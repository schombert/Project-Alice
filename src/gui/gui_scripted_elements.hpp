#pragma once

#include "dcon_generated.hpp"
#include "gui_graphics.hpp"
#include "gui_element_base.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "color.hpp"
#include "gui_element_types.hpp"

namespace ui {

// US29AC4 US29AC5 Extracted into a separate function because both icon classes (below) and scripted button classes reference it
int32_t frame_from_datamodel(sys::state& state, ui::datamodel datamodel);

// US29AC3
class datamodel_general_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		// US29AC3 US29AC3 When an icon has `datamodel="state_religion"`, it always displays the state religion of the player
		frame = frame_from_datamodel(state, base_data.datamodel);
	}
};

// US29AC6 US29AC7
class datamodel_icon_country_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto elname = text::produce_simple_string(state, base_data.name);

		for(auto identity : state.world.in_national_identity) {
			auto tag = nations::int_to_tag(identity.get_identifying_int());
			if(elname.ends_with(tag)) {
				return identity;
			}
		}
		
		return state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
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

// US28AC2 When clicking button with toggle_ui_key the associated UI variable is toggled True/False
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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;

};

// US28AC4 Window is shown only when UI variable in `visible_ui_key` is set to True
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
