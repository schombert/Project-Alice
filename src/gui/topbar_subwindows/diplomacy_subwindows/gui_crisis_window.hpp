#pragma once

#include "gui_element_types.hpp"

namespace ui {

class diplomacy_crisis_attacker_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		return state.world.nation_get_identity_from_identity_holder(state.primary_crisis_attacker);
	}
};

class diplomacy_crisis_sponsored_attacker_flag : public flag_button {
public:
	bool show = false;

	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(state.current_crisis == sys::crisis_type::colonial) {
			auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
			if(colonizers.begin() != colonizers.end()) {
				auto attacking_colonizer = (*colonizers.begin()).get_colonizer();
				return state.world.nation_get_identity_from_identity_holder(attacking_colonizer);
			}
		} else if(state.current_crisis == sys::crisis_type::liberation) {
			return state.crisis_liberation_tag;
		}
		return dcon::national_identity_id{};
	}
	void on_update(sys::state& state) noexcept override {
		flag_button::on_update(state);

		if(state.current_crisis == sys::crisis_type::colonial) {
			auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
			if(colonizers.begin() != colonizers.end()) {
				auto attacking_colonizer = (*colonizers.begin()).get_colonizer();
				show = attacking_colonizer != state.primary_crisis_attacker;
			} else {
				show = false;
			}
		} else if(state.current_crisis == sys::crisis_type::liberation) {
			show = state.crisis_liberation_tag != state.world.nation_get_identity_from_identity_holder(state.primary_crisis_attacker);
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			flag_button::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return flag_button::impl_probe_mouse(state, x, y, type);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;
		flag_button::update_tooltip(state, x, y, contents);
	}
};

class support_defender_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = !nations::is_involved_in_crisis(state, state.local_player_nation);
		disabled = !command::can_take_sides_in_crisis(state, state.local_player_nation, false);
	}
	void button_action(sys::state& state) noexcept override {
		if(show)
			command::take_sides_in_crisis(state, state.local_player_nation, false);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;

		text::add_line_with_condition(state, contents, "crisis_back_explain_2", state.current_crisis_mode == sys::crisis_mode::heating_up);

		for(auto& i : state.crisis_participants) {
			if(i.id == state.local_player_nation) {
				text::add_line_with_condition(state, contents, "crisis_back_explain_1", i.merely_interested == true);
				return;
			}
			if(!i.id) {
				text::add_line_with_condition(state, contents, "crisis_back_explain_1", false);
				return;
			}
		}
	}
};

class support_attacker_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = !nations::is_involved_in_crisis(state, state.local_player_nation);
		disabled = !command::can_take_sides_in_crisis(state, state.local_player_nation, true);
	}
	void button_action(sys::state& state) noexcept override {
		if(show)
			command::take_sides_in_crisis(state, state.local_player_nation, true);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;

		text::add_line_with_condition(state, contents, "crisis_back_explain_2", state.current_crisis_mode == sys::crisis_mode::heating_up);

		for(auto& i : state.crisis_participants) {
			if(i.id == state.local_player_nation) {
				text::add_line_with_condition(state, contents, "crisis_back_explain_1", i.merely_interested == true);
				return;
			}
			if(!i.id) {
				text::add_line_with_condition(state, contents, "crisis_back_explain_1", false);
				return;
			}
		}
	}
};


class propose_attacker_solution_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = state.local_player_nation == state.primary_crisis_attacker;
	}
	void button_action(sys::state& state) noexcept override {
		//if(show)
		//TODO
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
};

class propose_defender_solution_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = state.local_player_nation == state.primary_crisis_defender;
	}
	void button_action(sys::state& state) noexcept override {
		//if(show)
		//TODO
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
};

class diplomacy_crisis_attacker_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, state.world.nation_get_name(state.primary_crisis_attacker)));
	}
};

class diplomacy_crisis_attacker_backers : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) noexcept override {
		if(bool(current_nation)) {
			row_contents.clear();
			for(uint32_t i = 0; i < state.crisis_participants.size(); i++) {
				if(!state.crisis_participants[i].id)
					break;

				if(state.crisis_participants[i].supports_attacker && !state.crisis_participants[i].merely_interested && state.crisis_participants[i].id != state.primary_crisis_attacker) {
					auto content = dcon::fatten(state.world, state.crisis_participants[i].id);
					row_contents.push_back(content.get_identity_from_identity_holder().id);
				}
			}
			update(state);
		}
	}
};

class diplomacy_crisis_attacker_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag") {
			return make_element_by_type<diplomacy_crisis_attacker_flag>(state, id);

		} else if(name == "sponsored_flag") { 
			return make_element_by_type<diplomacy_crisis_sponsored_attacker_flag>(state, id);

		} else if(name == "country_name") {
			return make_element_by_type<diplomacy_crisis_attacker_name>(state, id);

		} else if(name == "wargoals") {
			// TODO - overlapping stuff
			return nullptr;

		} else if(name == "backers_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "backers") {
			auto ptr = make_element_by_type<diplomacy_crisis_attacker_backers>(state, id);
			ptr->base_data.position.y -= 6;
			return ptr;
		} else if(name == "support_side") {
			return make_element_by_type<support_attacker_button>(state, id);
		} else if(name == "back_down") {
			return make_element_by_type<propose_defender_solution_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_crisis_defender_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		return state.world.nation_get_identity_from_identity_holder(state.primary_crisis_defender);
	}
};

class diplomacy_crisis_sponsored_defender_flag : public flag_button {
public:
	bool show = false;

	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(state.current_crisis == sys::crisis_type::colonial) {
			auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
			if(colonizers.end() - colonizers.begin() >= 2) {
				auto def_colonizer = (*(colonizers.begin() + 1)).get_colonizer();
				return state.world.nation_get_identity_from_identity_holder(def_colonizer);
			}
		} else if(state.current_crisis == sys::crisis_type::liberation) {
			return state.world.nation_get_identity_from_identity_holder(state.world.state_instance_get_nation_from_state_ownership(state.crisis_state));
		}
		return dcon::national_identity_id{};
	}
	void on_update(sys::state& state) noexcept override {
		flag_button::on_update(state);

		if(state.current_crisis == sys::crisis_type::colonial) {
			auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
			if(colonizers.end() - colonizers.begin() >= 2) {
				auto def_colonizer = (*(colonizers.begin() + 1)).get_colonizer();
				show = def_colonizer != state.primary_crisis_defender;
			} else {
				show = false;
			}
		} else if(state.current_crisis == sys::crisis_type::liberation) {
			show = state.world.state_instance_get_nation_from_state_ownership(state.crisis_state) != state.primary_crisis_defender;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			flag_button::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return flag_button::impl_probe_mouse(state, x, y, type);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;
		flag_button::update_tooltip(state, x, y, contents);
	}
};

class diplomacy_crisis_defender_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, state.world.nation_get_name(state.primary_crisis_defender)));
	}
};

class diplomacy_crisis_defender_backers : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) noexcept override {
		if(bool(current_nation)) {
			row_contents.clear();
			for(uint32_t i = 0; i < state.crisis_participants.size(); i++) {
				if(!state.crisis_participants[i].id)
					break;

				if(!state.crisis_participants[i].supports_attacker && !state.crisis_participants[i].merely_interested && state.crisis_participants[i].id != state.primary_crisis_defender) {
					auto content = dcon::fatten(state.world, state.crisis_participants[i].id);
					row_contents.push_back(content.get_identity_from_identity_holder().id);
				}
			}
			update(state);
		}
	}
};

class diplomacy_crisis_defender_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag") {
			return make_element_by_type<diplomacy_crisis_defender_flag>(state, id);

		} else if(name == "sponsored_flag") {
			return make_element_by_type<diplomacy_crisis_sponsored_defender_flag>(state, id);

		} else if(name == "country_name") {
			return make_element_by_type<diplomacy_crisis_defender_name>(state, id);

		} else if(name == "wargoals") {
			// TODO - overlapping stuff
			return nullptr;

		} else if(name == "backers_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "backers") {
			auto ptr = make_element_by_type<diplomacy_crisis_defender_backers>(state, id);
			ptr->base_data.position.y -= 6;
			return ptr;
		} else if(name == "support_side") {
			return make_element_by_type<support_defender_button>(state, id);
		} else if(name == "back_down") {
			return make_element_by_type<propose_attacker_solution_button>(state, id);

		} else {
			return nullptr;
		}
	}
};

class diplomacy_crisis_title_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(state.current_crisis == sys::crisis_type::colonial) {
			set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, state.crisis_colony).get_name()));
		} else if(state.current_crisis == sys::crisis_type::liberation) {
			set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, state.crisis_state).get_definition().get_name()));
		} else {
			
		}
	}
};

class diplomacy_crisis_subtitle_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		switch(state.current_crisis) {
		case sys::crisis_type::none:
			break;
		case sys::crisis_type::claim:
			//set_text(state, text::produce_simple_string(state, "crisis_description_reclaim"));
			break;
		case sys::crisis_type::liberation:
		{
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::country, state.crisis_liberation_tag);
			if(state.world.nation_get_owned_province_count(state.world.national_identity_get_nation_from_identity_holder(state.crisis_liberation_tag)) > 0) {
				set_text(state, text::resolve_string_substitution(state, "crisis_description_reclaim", m));
			} else {
				set_text(state, text::resolve_string_substitution(state, "crisis_description_liberation", m));
			}
		}
			break;
		case sys::crisis_type::colonial:
			set_text(state, text::produce_simple_string(state, "crisis_description_colonize"));
			break;
		case sys::crisis_type::influence:
			//set_text(state, text::produce_simple_string(state, "crisis_description_influence"));
			break;
		};
	}
};

class diplomacy_crisis_temperature_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		progress = state.current_crisis_mode == sys::crisis_mode::heating_up ? state.crisis_temperature / 100.0f : 0.0f;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return state.current_crisis_mode == sys::crisis_mode::heating_up ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.current_crisis_mode != sys::crisis_mode::heating_up)
			return;
		text::add_line(state, contents, "crisis_temperature", text::variable_type::value, int64_t(state.crisis_temperature));
	}
};

class diplomacy_crisis_status_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		switch(state.current_crisis_mode) {
		case sys::crisis_mode::inactive:
			set_text(state, text::produce_simple_string(state, "has_no_crisis"));
			break;
		case sys::crisis_mode::finding_attacker:
			set_text(state, text::produce_simple_string(state, "crisis_waiting_on_backer"));
			break;
		case sys::crisis_mode::finding_defender:
			set_text(state, text::produce_simple_string(state, "crisis_waiting_on_backer"));
			break;
		case sys::crisis_mode::heating_up:
			set_text(state, text::produce_simple_string(state, "")); // TODO - find the correct CSV Key for this...
			break;
		}
	}
};

class diplomacy_crisis_info_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "crisis_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "crisis_title") {
			return make_element_by_type<diplomacy_crisis_title_text>(state, id);

		} else if(name == "crisis_sub_title") {
			return make_element_by_type<diplomacy_crisis_subtitle_text>(state, id);

		} else if(name == "crisis_temperature") {
			return make_element_by_type<diplomacy_crisis_temperature_bar>(state, id);

		} else if(name == "crisis_temperature_overlay") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "crisis_status_label") {
			return make_element_by_type<diplomacy_crisis_status_text>(state, id);

		} else if(name == "attacker_win") {
			return make_element_by_type<diplomacy_crisis_attacker_window>(state, id);

		} else if(name == "defender_win") {
			return make_element_by_type<diplomacy_crisis_defender_window>(state, id);

		} else if(name == "fence_sitters_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "fence_sitters_win") {
			return nullptr;

		} else {
			return nullptr;
		}
	}
};

} // namespace ui
