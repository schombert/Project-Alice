#pragma once

#include <vector>

#include "gui_element_types.hpp"
#include "triggers.hpp"
#include "text.hpp"

namespace ui {

inline void produce_decision_substitutions(sys::state& state, text::substitution_map& m, dcon::nation_id n) {
	text::add_to_substitution_map(m, text::variable_type::country_adj, text::get_adjective(state, n));
	text::add_to_substitution_map(m, text::variable_type::country, n);
	text::add_to_substitution_map(m, text::variable_type::countryname, n);
	text::add_to_substitution_map(m, text::variable_type::thiscountry, n);
	text::add_to_substitution_map(m, text::variable_type::capital, state.world.nation_get_capital(n));
	text::add_to_substitution_map(m, text::variable_type::monarchtitle, text::get_ruler_title(state, n));
	text::add_to_substitution_map(m, text::variable_type::continentname, state.world.nation_get_capital(n).get_continent().get_name());
	// Date
	text::add_to_substitution_map(m, text::variable_type::year, int32_t(state.current_date.to_ymd(state.start_date).year));
	//text::add_to_substitution_map(m, text::variable_type::month, text::localize_month(state, state.current_date.to_ymd(state.start_date).month));
	text::add_to_substitution_map(m, text::variable_type::day, int32_t(state.current_date.to_ymd(state.start_date).day));
	auto sm = state.world.nation_get_in_sphere_of(n);
	text::add_to_substitution_map(m, text::variable_type::spheremaster, sm);
	text::add_to_substitution_map(m, text::variable_type::spheremaster_adj, text::get_adjective(state, sm));
	auto smpc = state.world.nation_get_primary_culture(sm);
	text::add_to_substitution_map(m, text::variable_type::spheremaster_union_adj, smpc.get_group_from_culture_group_membership().get_identity_from_cultural_union_of().get_adjective());

	// Non-vanilla
	text::add_to_substitution_map(m, text::variable_type::government, state.world.nation_get_government_type(n).get_name());
	text::add_to_substitution_map(m, text::variable_type::ideology, state.world.nation_get_ruling_party(n).get_ideology().get_name());
	text::add_to_substitution_map(m, text::variable_type::party, state.world.nation_get_ruling_party(n).get_name());
	text::add_to_substitution_map(m, text::variable_type::religion, state.world.religion_get_name(state.world.nation_get_religion(n)));
	text::add_to_substitution_map(m, text::variable_type::infamy, text::fp_two_places{ state.world.nation_get_infamy(n) });
	text::add_to_substitution_map(m, text::variable_type::badboy, text::fp_two_places{ state.world.nation_get_infamy(n) });
	text::add_to_substitution_map(m, text::variable_type::spheremaster, state.world.nation_get_in_sphere_of(n));
	text::add_to_substitution_map(m, text::variable_type::overlord, state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(n)));
	text::add_to_substitution_map(m, text::variable_type::nationalvalue, state.world.nation_get_national_value(n).get_name());
	// Crisis stuff
	text::add_to_substitution_map(m, text::variable_type::crisistaker, state.crisis_liberation_tag);
	text::add_to_substitution_map(m, text::variable_type::crisistaker_adj, state.world.national_identity_get_adjective(state.crisis_liberation_tag));
	text::add_to_substitution_map(m, text::variable_type::crisistaker_capital, state.world.national_identity_get_capital(state.crisis_liberation_tag));
	text::add_to_substitution_map(m, text::variable_type::crisistaker_continent, state.world.national_identity_get_capital(state.crisis_liberation_tag).get_continent().get_name());
	text::add_to_substitution_map(m, text::variable_type::crisisattacker, state.primary_crisis_attacker);
	text::add_to_substitution_map(m, text::variable_type::crisisattacker_capital, state.world.nation_get_capital(state.primary_crisis_attacker));
	text::add_to_substitution_map(m, text::variable_type::crisisattacker_continent, state.world.nation_get_capital(state.primary_crisis_attacker).get_continent().get_name());
	text::add_to_substitution_map(m, text::variable_type::crisisdefender, state.primary_crisis_defender);
	text::add_to_substitution_map(m, text::variable_type::crisisdefender_capital, state.world.nation_get_capital(state.primary_crisis_defender));
	text::add_to_substitution_map(m, text::variable_type::crisisdefender_continent, state.world.nation_get_capital(state.primary_crisis_defender).get_continent().get_name());
	text::add_to_substitution_map(m, text::variable_type::crisistarget, state.primary_crisis_defender);
	text::add_to_substitution_map(m, text::variable_type::crisistarget_adj, text::get_adjective(state, state.primary_crisis_defender));
	text::add_to_substitution_map(m, text::variable_type::crisisarea, state.crisis_state);
	text::add_to_substitution_map(m, text::variable_type::temperature, text::fp_two_places{ state.crisis_temperature });
	// TODO: Is this correct? I remember in vanilla it could vary
	auto pc = state.world.nation_get_primary_culture(n);
	text::add_to_substitution_map(m, text::variable_type::culture, state.world.culture_get_name(pc));
	text::add_to_substitution_map(m, text::variable_type::culture_group_union, pc.get_group_from_culture_group_membership().get_identity_from_cultural_union_of().get_nation_from_identity_holder());
	text::add_to_substitution_map(m, text::variable_type::union_adj, pc.get_group_from_culture_group_membership().get_identity_from_cultural_union_of().get_adjective());
	text::add_to_substitution_map(m, text::variable_type::countryculture, state.world.culture_get_name(pc));
	auto names_pair = rng::get_random_pair(state, uint32_t(n.index()) << 6, uint32_t(pc.id.index()));
	auto first_names = state.world.culture_get_first_names(state.world.nation_get_primary_culture(n));
	if(first_names.size() > 0) {
		auto first_name = first_names.at(rng::reduce(uint32_t(names_pair.high), first_names.size()));
		auto last_names = state.world.culture_get_last_names(state.world.nation_get_primary_culture(n));
		if(last_names.size() > 0) {
			auto last_name = last_names.at(rng::reduce(uint32_t(names_pair.high), last_names.size()));
			text::add_to_substitution_map(m, text::variable_type::culture_first_name, state.to_string_view(first_name));
			text::add_to_substitution_map(m, text::variable_type::culture_last_name, state.to_string_view(last_name));
		}
	}
	text::add_to_substitution_map(m, text::variable_type::tech, state.world.nation_get_current_research(n).get_name());
	text::add_to_substitution_map(m, text::variable_type::now, state.current_date);
	if(auto plist = state.world.nation_get_province_ownership(n); plist.begin() != plist.end()) {
		auto plist_size = uint32_t(plist.end() - plist.begin());
		uint32_t index = rng::reduce(uint32_t(names_pair.high), plist_size);
		text::add_to_substitution_map(m, text::variable_type::anyprovince, (*(plist.begin() + index)).get_province());
	}
}

class decision_requirements : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);
		auto condition = state.world.decision_get_allow(id);
		if(condition)
			trigger_description(state, contents, condition, trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), -1);
	}
};

class decision_ai_will_do : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);
		text::add_line(state, contents, "alice_ai_decision");
		auto mkey = state.world.decision_get_ai_will_do(id);
		if(mkey)
			multiplicative_value_modifier_description(state, contents, mkey, trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), -1);
	}
};

class decision_potential : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);
		auto potential = state.world.decision_get_potential(id);
		if(potential)
			trigger_description(state, contents, potential, trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), -1);
	}
};

class make_decision : public button_element_base {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_decision_sound(state);
	}

	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::decision_id>(state, parent);
		command::take_decision(state, state.local_player_nation, content);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::decision_id>(state, parent);
		disabled = !command::can_take_decision(state, state.local_player_nation, content);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, id);
		auto box = text::open_layout_box(contents);
		text::substitution_map m;
		produce_decision_substitutions(state, m, state.local_player_nation);
		text::add_to_layout_box(state, contents, box, fat_id.get_name(), m);
		text::close_layout_box(contents, box);

		auto ef = fat_id.get_effect();
		if(bool(ef))
			effect_description(state, contents, ef, trigger::to_generic(state.local_player_nation),
				trigger::to_generic(state.local_player_nation), -1, uint32_t(state.current_date.value),
				uint32_t(state.local_player_nation.index() << 4 ^ id.index()));
	}

};

// -------------
// Decision Name
// -------------

class decision_name : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);
		auto contents = text::create_endless_layout(internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::white, true });
		auto box = text::open_layout_box(contents);
		text::substitution_map m;
		produce_decision_substitutions(state, m, state.local_player_nation);
		text::add_to_layout_box(state, contents, box, state.world.decision_get_name(id), m);
		text::close_layout_box(contents, box);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto const id = retrieve<dcon::decision_id>(state, parent);
		auto const description = state.world.decision_get_description(id);
		if(state.cheat_data.show_province_id_tooltip) {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, std::string_view("Decision ID:"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::to_string(id.value));
			text::close_layout_box(contents, box);
		}
		auto box = text::open_layout_box(contents);
		text::substitution_map m;
		produce_decision_substitutions(state, m, state.local_player_nation);
		text::add_to_layout_box(state, contents, box, description, m);
		text::close_layout_box(contents, box);
	}
};

// --------------
// Decision Image
// --------------

class decision_image : public image_element_base {
public:
	bool get_horizontal_flip(sys::state& state) noexcept override {
		return false; //never flip
	}
	void on_update(sys::state& state) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, id);
		base_data.data.image.gfx_object = fat_id.get_image();
	}
};

// --------------------
// Decision Description
// --------------------

class decision_desc : public scrollable_text {
private:
	dcon::text_key description;
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept {
		auto box = text::open_layout_box(contents);
		text::substitution_map m;
		produce_decision_substitutions(state, m, state.local_player_nation);
		text::add_to_layout_box(state, contents, box, description, m);
		text::close_layout_box(contents, box);
	}

public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.y = 76;
		scrollable_text::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, id);
		description = fat_id.get_description();
		auto container = text::create_endless_layout(delegate->internal_layout,
			text::layout_parameters{0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
				base_data.data.text.font_handle, 0, text::alignment::left,
				text::is_black_from_font_id(base_data.data.text.font_handle) ? text::text_color::black : text::text_color::white,
				false});
		populate_layout(state, container);
		calibrate_scrollbar(state);
	}

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		// Ignore mouse wheel scrolls so people DO NOT get confused!
		if(type == mouse_probe_type::scroll)
			return message_result::unseen;
		return scrollable_text::test_mouse(state, x, y, type);
	}
};

// ---------------
// Ignore Checkbox
// ---------------

class ignore_checkbox : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);
		if(bool(id)) {
			state.world.decision_set_hide_notification(id, !state.world.decision_get_hide_notification(id));
			state.game_state_updated.store(true, std::memory_order_release);
		}
	}

	bool is_active(sys::state& state) noexcept override {
		auto id = retrieve<dcon::decision_id>(state, parent);
		return state.world.decision_get_hide_notification(id);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "hide_decision");
	}
};

// -------------
// Decision Item
// -------------

class decision_item : public listbox_row_element_base<dcon::decision_id> {
public:
	std::unique_ptr<ui::element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "decision_name") {
			return make_element_by_type<decision_name>(state, id);
		} else if(name == "decision_image") {
			return make_element_by_type<decision_image>(state, id);
		} else if(name == "decision_desc") {
			return make_element_by_type<decision_desc>(state, id);
		} else if(name == "requirements") {
			// Extra button to tell if AI will do
			auto btn1 = make_element_by_type<decision_ai_will_do>(state, id);
			btn1->base_data.position.x -= btn1->base_data.size.x * 2;
			add_child_to_front(std::move(btn1));
			auto btn2 = make_element_by_type<decision_potential>(state, id);
			btn2->base_data.position.x -= btn2->base_data.size.x;
			add_child_to_front(std::move(btn2));
			return make_element_by_type<decision_requirements>(state, id);
		} else if(name == "ignore_checkbox") {
			return make_element_by_type<ignore_checkbox>(state, id);
		} else if(name == "make_decision") {
			return make_element_by_type<make_decision>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::decision_id>()) {
			payload.emplace<dcon::decision_id>(content);
			return message_result::consumed;
		} else if(payload.holds_type<wrapped_listbox_row_content<dcon::decision_id>>()) {
			return listbox_row_element_base<dcon::decision_id>::get(state, payload);
		} else {
			return message_result::unseen;
		}
	}
};

// ----------------
// Decision Listbox
// ----------------

class decision_listbox : public listbox_element_base<decision_item, dcon::decision_id> {
protected:
	std::string_view get_row_element_name() override {
		return "decision_entry";
	}
private:
	std::vector<dcon::decision_id> get_decisions(sys::state& state) {
		std::vector<dcon::decision_id> list;
		auto n = state.local_player_nation;
		for(uint32_t i = state.world.decision_size(); i-- > 0;) {
			dcon::decision_id did{ dcon::decision_id::value_base_t(i) };
			if(!state.cheat_data.always_potential_decisions) {
				auto lim = state.world.decision_get_potential(did);
				if(!lim || trigger::evaluate(state, lim, trigger::to_generic(n), trigger::to_generic(n), 0)) {
					list.push_back(did);
				}
			} else {
				list.push_back(did);
			}
		}

		std::sort(list.begin(), list.end(), [&](dcon::decision_id a, dcon::decision_id b) {
			auto allow_a = state.world.decision_get_allow(a);
			auto allow_b = state.world.decision_get_allow(b);
			auto a_res = !allow_a || trigger::evaluate(state, allow_a, trigger::to_generic(n), trigger::to_generic(n), 0);
			auto b_res = !allow_b || trigger::evaluate(state, allow_b, trigger::to_generic(n), trigger::to_generic(n), 0);
			if(a_res != b_res)
				return a_res;
			else
				return a.index() < b.index();
		});

		return list;
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents = get_decisions(state);
		update(state);
	}
};

// ----------------
// Decision Window
// ----------------

class decision_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "decision_listbox") {
			return make_element_by_type<decision_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

} // namespace ui
