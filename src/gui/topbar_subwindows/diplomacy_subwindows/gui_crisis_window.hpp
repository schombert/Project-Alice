#pragma once

#include "gui_element_types.hpp"

namespace ui {

class diplomacy_crisis_backdown_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::crisis_backdown;
			parent->impl_get(state, payload);
		}
	}
};

class diplomacy_crisis_support_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::crisis_support;
			parent->impl_get(state, payload);
		}
	}
};

class diplomacy_crisis_attacker_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(state.current_crisis != sys::crisis_type::colonial) { // Liberation
			return state.crisis_liberation_tag;
		} else if(state.current_crisis != sys::crisis_type::liberation) { // Colonial
			return dcon::fatten(state.world, state.primary_crisis_attacker).get_identity_from_identity_holder();
		}

		return dcon::national_identity_id{0};
	}
};

class diplomacy_crisis_sponsor_attacker_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(state.current_crisis != sys::crisis_type::colonial) { // Liberation
			auto fat_id = dcon::fatten(state.world, state.primary_crisis_attacker);
			return fat_id.get_identity_from_identity_holder();
		} else if(state.current_crisis != sys::crisis_type::liberation) { // Colonial
			return dcon::national_identity_id{
				0}; // TODO - this should only appear for things that would need a GP, and a GP cant have a sponsor i think?
		}
		return dcon::national_identity_id{0};
	}
};

class diplomacy_crisis_attacker_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(state.current_crisis != sys::crisis_type::colonial) { // Liberation
			set_text(state,
				text::produce_simple_string(state, dcon::fatten(state.world, state.crisis_liberation_tag).get_name()));
			return;
		} else if(state.current_crisis != sys::crisis_type::liberation) { // Colonial
			set_text(state,
				text::produce_simple_string(state, dcon::fatten(state.world, state.primary_crisis_attacker).get_name()));
			return;
		}
	}
};

class diplomacy_crisis_attacker_backers : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) noexcept override {
		if(bool(current_nation)) {
			row_contents.clear();
			for(uint32_t i = 0; i < state.crisis_participants.size(); i++) {
				if(state.crisis_participants[i].supports_attacker) {
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

		} else if(name == "sponsored_flag") { // Brought to you BY no one, fuck companies >w<
			return make_element_by_type<diplomacy_crisis_sponsor_attacker_flag>(state, id);

		} else if(name == "country_name") {
			return make_element_by_type<diplomacy_crisis_attacker_name>(state, id);

		} else if(name == "wargoals") {
			// TODO - overlapping stuff
			return nullptr;

		} else if(name == "backers_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "backers") {
			return make_element_by_type<diplomacy_crisis_attacker_backers>(state, id);

		} else if(name == "support_side") {
			auto ptr = make_element_by_type<diplomacy_crisis_support_button>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "back_down") {
			return make_element_by_type<diplomacy_crisis_backdown_button>(state, id);

		} else {
			return nullptr;
		}
	}
};

class diplomacy_crisis_defender_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(state.current_crisis != sys::crisis_type::colonial) { // Liberation
			if(nations::is_great_power(state, state.primary_crisis_defender)) {
				return state.crisis_liberation_tag;
			} else {
				return dcon::fatten(state.world, state.crisis_state)
					.get_nation_from_state_ownership()
					.get_identity_from_identity_holder()
					.id;
			}
		} else if(state.current_crisis != sys::crisis_type::liberation) { // Colonial
			return dcon::fatten(state.world, state.primary_crisis_defender).get_identity_from_identity_holder();
		}

		return dcon::national_identity_id{0};
	}
};

class diplomacy_crisis_sponsor_defender_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(state.current_crisis != sys::crisis_type::colonial) { // Liberation
			auto fat_id = dcon::fatten(state.world, state.primary_crisis_defender);
			return fat_id.get_identity_from_identity_holder();
		} else if(state.current_crisis != sys::crisis_type::liberation) { // Colonial
			return dcon::national_identity_id{
				0}; // TODO - this should only appear for things that would need a GP, and a GP cant have a sponsor i think?
		}
		return dcon::national_identity_id{0};
	}
};

class diplomacy_crisis_defender_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, state.primary_crisis_defender);
		set_text(state, text::produce_simple_string(state, fat_id.get_name()));
	}
};

class diplomacy_crisis_defender_backers : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) noexcept override {
		if(bool(current_nation)) {
			row_contents.clear();
			for(uint32_t i = 0; i < state.crisis_participants.size(); i++) {
				if(!state.crisis_participants[i].supports_attacker) {
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

		} else if(name == "sponsored_flag") { // Brought to you BY no one, fuck companies >w<
			return make_element_by_type<diplomacy_crisis_sponsor_defender_flag>(state, id);

		} else if(name == "country_name") {
			return make_element_by_type<diplomacy_crisis_defender_name>(state, id);

		} else if(name == "wargoals") {
			// TODO - overlapping stuff
			return nullptr;

		} else if(name == "backers_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "backers") {
			return make_element_by_type<diplomacy_crisis_defender_backers>(state, id);

		} else if(name == "support_side") {
			return make_element_by_type<diplomacy_crisis_support_button>(state, id);

		} else if(name == "back_down") {
			auto ptr = make_element_by_type<diplomacy_crisis_backdown_button>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else {
			return nullptr;
		}
	}
};

class diplomacy_crisis_title_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(state.current_crisis == sys::crisis_type::colonial || state.current_crisis == sys::crisis_type::claim) {
			set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, state.crisis_colony).get_name()));
		} else if(state.current_crisis == sys::crisis_type::liberation || state.current_crisis == sys::crisis_type::influence) {
			set_text(state,
				text::produce_simple_string(state, dcon::fatten(state.world, state.crisis_liberation_tag).get_name()));
		} else {
			set_text(state, "Pwease gib me Crisis UwU");
		}
	}
};

class diplomacy_crisis_subtitle_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		switch(state.current_crisis) {
		case sys::crisis_type::none:
			set_text(state, "Im having a mental crisis >w<");
			break;
		case sys::crisis_type::claim:
			set_text(state, text::produce_simple_string(state, "crisis_description_reclaim"));
			break;
		case sys::crisis_type::liberation:
			set_text(state, text::produce_simple_string(state, "crisis_description_liberation"));
			break;
		case sys::crisis_type::colonial:
			set_text(state, text::produce_simple_string(state, "crisis_description_colonize"));
			break;
		case sys::crisis_type::influence:
			set_text(state, text::produce_simple_string(state, "crisis_description_influence"));
			break;
		};
	}
};

class diplomacy_crisis_temperature_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override { progress = state.crisis_temperature; }
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
			set_text(state, text::produce_simple_string(state, "has_crisis")); // TODO - find the correct CSV Key for this...
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
