#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "text.hpp"
#include "parsers.hpp"
#include <algorithm>
#include <functional>

#include "gui_diplomacy_actions_window.hpp"
#include "gui_pick_wargoal_window.hpp"
#include "gui_peace_window.hpp"
#include "gui_crisis_window.hpp"

namespace military {
void populate_war_text_subsitutions(sys::state&, dcon::war_id, text::substitution_map&);
}

namespace ui {


class cb_wargoal_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::cb_type_id cbt = retrieve<dcon::cb_type_id>(state, parent);
		frame = state.world.cb_type_get_sprite_index(cbt) - 1;
	}
};

class cb_wargoal_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::cb_type_id content = retrieve<dcon::cb_type_id>(state, parent);
		set_button_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_name()));
		if(parent) {
			auto selected = retrieve<dcon::cb_type_id>(state, parent->parent);
			disabled = selected == content;
		}
	}

	void button_action(sys::state& state) noexcept override {
		const dcon::cb_type_id content = retrieve<dcon::cb_type_id>(state, parent);
		Cyto::Any newpayload = element_selection_wrapper<dcon::cb_type_id>{ content };
		parent->impl_get(state, newpayload);
	}
};

class diplomacy_make_cb_type : public listbox_row_element_base<dcon::cb_type_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<cb_wargoal_icon>(state, id);
		} else if(name == "select_cb") {
			return make_element_by_type<cb_wargoal_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_make_cb_listbox : public listbox_element_base<diplomacy_make_cb_type, dcon::cb_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "cb_type_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		dcon::nation_id content = retrieve<dcon::nation_id>(state, parent);
		state.world.for_each_cb_type([&](dcon::cb_type_id cb) {
			if(command::can_fabricate_cb(state, state.local_player_nation, content, cb))
				row_contents.push_back(cb);
		});
		update(state);
	}
};

class diplomacy_make_cb_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::cb_type_id>(state, parent);
		auto target_nation = retrieve<dcon::nation_id>(state, parent);
		command::fabricate_cb(state, state.local_player_nation, target_nation, content);
		parent->set_visible(state, false);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::cb_type_id>(state, parent);
		auto target_nation = retrieve<dcon::nation_id>(state, parent);
		disabled = !command::can_fabricate_cb(state, state.local_player_nation, target_nation, content);
	}
};

class diplomacy_make_cb_desc : public simple_multiline_body_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {

		auto fat_cb = dcon::fatten(state.world, retrieve<dcon::cb_type_id>(state, parent));

		auto box = text::open_layout_box(contents);

		auto fab_time = std::ceil(100.0f / (state.defines.cb_generation_base_speed * fat_cb.get_construction_speed() * (state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::cb_generation_speed_modifier) + 1.0f)));

		if(fat_cb.is_valid()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::type, fat_cb.get_name());
			text::add_to_substitution_map(sub, text::variable_type::days, int64_t(fab_time));
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_one_place{military::cb_infamy(state, fat_cb)});
			text::localised_format_box(state, contents, box, std::string_view("cb_creation_detail"), sub);
		}

		text::close_layout_box(contents, box);
	}
};

class make_cb_title : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, "make_cbtitle"));
	}
};

class diplomacy_make_cb_window : public window_element_base { // eu3dialogtype
private:
	dcon::cb_type_id root_cb{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge size for proper sizing
			return ptr;
		} else if(name == "diplo_makecb_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "title") {
			return make_element_by_type<make_cb_title>(state, id);
		} else if(name == "description") {
			return make_element_by_type<diplomacy_make_cb_desc>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<diplomacy_make_cb_button>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "cb_list") {
			return make_element_by_type<diplomacy_make_cb_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::cb_type_id>>()) {
			root_cb = any_cast<element_selection_wrapper<dcon::cb_type_id>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::cb_type_id>()) {
			payload.emplace<dcon::cb_type_id>(root_cb);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};


enum class diplomacy_window_tab : uint8_t { great_powers = 0x0, wars = 0x1, casus_belli = 0x2, crisis = 0x3 };

class diplomacy_nation_navies_text : public nation_num_ships {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		text::add_line(state, contents, "diplomacy_ships", text::variable_type::value, military::total_ships(state, n));
		text::add_line_break_to_layout(state, contents);
		int32_t total = 0;
		int32_t discovered = 0;
		state.world.for_each_technology([&](dcon::technology_id id) {
			auto fat_id = dcon::fatten(state.world, id);
			if(state.culture_definitions.tech_folders[fat_id.get_folder_index()].category != culture::tech_category::navy)
				return;
			++total;
			if(state.world.nation_get_active_technologies(n, id))
				++discovered;
		});
		text::add_line(state, contents, "navy_technology_levels", text::variable_type::val, discovered, text::variable_type::max, total);
	}
};

class diplomacy_nation_armies_text : public nation_num_regiments {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		text::add_line(state, contents, "diplomacy_brigades", text::variable_type::value, military::total_regiments(state, n));
		text::add_line_break_to_layout(state, contents);
		int32_t total = 0;
		int32_t discovered = 0;
		state.world.for_each_technology([&](dcon::technology_id id) {
			auto fat_id = dcon::fatten(state.world, id);
			if(state.culture_definitions.tech_folders[fat_id.get_folder_index()].category != culture::tech_category::army)
				return;
			++total;
			if(state.world.nation_get_active_technologies(n, id))
				++discovered;
		});
		text::add_line(state, contents, "army_technology_levels", text::variable_type::val, discovered, text::variable_type::max, total);
	}
};

class diplomacy_war_exhaustion : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::format_percentage(fat_id.get_war_exhaustion() / 100.0f, 1);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {	
		auto n = retrieve<dcon::nation_id>(state, parent);
		
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "diplomacy_wx_1");
		auto mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::war_exhaustion);
		if(mod > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::red);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{mod}, text::text_color::red);
		} else if(mod == 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+0"));
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{mod}, text::text_color::green);
		}
		text::close_layout_box(contents, box);

		active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::war_exhaustion, false);
	}
};

class diplomacy_country_select : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(!parent)
			return;
		auto content = retrieve<dcon::nation_id>(state, parent);
		auto window_content = retrieve<dcon::nation_id>(state, parent->parent);
		if(content == window_content)
			frame = 1;
		else
			frame = 0;
	}
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::nation_id>(state, parent);
		send(state, parent, element_selection_wrapper<dcon::nation_id>{content});
	}
};

void explain_influence(sys::state& state, dcon::nation_id target, text::columnar_layout& contents) {
	int32_t total_influence_shares = 0;
	auto n = fatten(state.world, state.local_player_nation);

	for(auto rel : state.world.nation_get_gp_relationship_as_great_power(state.local_player_nation)) {
		if(nations::can_accumulate_influence_with(state, state.local_player_nation, rel.get_influence_target(), rel)) {
			switch(rel.get_status() & nations::influence::priority_mask) {
			case nations::influence::priority_one:
				total_influence_shares += 1;
				break;
			case nations::influence::priority_two:
				total_influence_shares += 2;
				break;
			case nations::influence::priority_three:
				total_influence_shares += 3;
				break;
			default:
			case nations::influence::priority_zero:
				break;
			}
		}
	}

	auto rel = fatten(state.world, state.world.get_gp_relationship_by_gp_influence_pair(target, state.local_player_nation));

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0) {
		text::add_line(state, contents, "influence_explain_1");
		return;
	}
	if(military::has_truce_with(state, state.local_player_nation, target)) {
		text::add_line(state, contents, "influence_explain_2");
		return;
	}
	if(military::are_at_war(state, state.local_player_nation, target)) {
		text::add_line(state, contents, "influence_explain_3");
		return;
	}

	float total_gain = state.defines.base_greatpower_daily_influence * (1.0f + n.get_modifier_values(sys::national_mod_offsets::influence_modifier)) * (1.0f + n.get_modifier_values(sys::national_mod_offsets::influence));

	float gp_score = n.get_industrial_score() + n.get_military_score() + nations::prestige_score(state, n);
	
	float base_shares = [&]() {
		switch(rel.get_status() & nations::influence::priority_mask) {
			case nations::influence::priority_one:
				return total_gain / float(total_influence_shares);
			case nations::influence::priority_two:
				return 2.0f * total_gain / float(total_influence_shares);
			case nations::influence::priority_three:
				return 3.0f * total_gain / float(total_influence_shares);
			default:
			case nations::influence::priority_zero:
				return 0.0f;
			}
	}();

	bool has_sphere_neighbor = [&]() {
		for(auto g : state.world.nation_get_nation_adjacency(target)) {
			if(g.get_connected_nations(0) != target && g.get_connected_nations(0).get_in_sphere_of() == n)
				return true;
			if(g.get_connected_nations(1) != target && g.get_connected_nations(1).get_in_sphere_of() == n)
				return true;
			}
		return false;
	}();

	float total_fi = 0.0f;
	for(auto i : state.world.nation_get_unilateral_relationship_as_target(target)) {
		total_fi += i.get_foreign_investment();
	}
	auto gp_invest = state.world.unilateral_relationship_get_foreign_investment(
			state.world.get_unilateral_relationship_by_unilateral_pair(target, n));

	float discredit_factor =
		(rel.get_status() & nations::influence::is_discredited) != 0
		? state.defines.discredit_influence_gain_factor
		: 0.0f;
	float neighbor_factor =
			bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(n, target))
		? state.defines.neighbour_bonus_influence_percent
		: 0.0f;
	float sphere_neighbor_factor =
		has_sphere_neighbor
		? state.defines.sphere_neighbour_bonus_influence_percent
		: 0.0f;
	float continent_factor =
		n.get_capital().get_continent() != state.world.nation_get_capital(target).get_continent()
		? state.defines.other_continent_bonus_influence_percent
		: 0.0f;
	float puppet_factor =
		fatten(state.world, state.world.nation_get_overlord_as_subject(target)).get_ruler() == n
		? state.defines.puppet_bonus_influence_percent
		: 0.0f;
	float relationship_factor =
		state.world.diplomatic_relation_get_value(state.world.get_diplomatic_relation_by_diplomatic_pair(n, target)) / state.defines.relation_influence_modifier;

	float investment_factor =
		total_fi > 0.0f
		? state.defines.investment_influence_defense * gp_invest / total_fi
		: 0.0f;
	float pop_factor =
			state.world.nation_get_demographics(target, demographics::total) > state.defines.large_population_limit
		? state.defines.large_population_influence_penalty * state.world.nation_get_demographics(target, demographics::total) / state.defines.large_population_influence_penalty_chunk
		: 0.0f;
	float score_factor =
		gp_score > 0.0f
		? std::max(1.0f - (state.world.nation_get_industrial_score(target) + state.world.nation_get_military_score(target) + nations::prestige_score(state, target)) / gp_score, 0.0f)
		: 0.0f;

	float total_multiplier = 1.0f + discredit_factor + neighbor_factor + sphere_neighbor_factor + continent_factor + puppet_factor + relationship_factor + investment_factor + pop_factor + score_factor;

	auto gain_amount = std::max(0.0f, base_shares * total_multiplier);

	text::add_line(state, contents, "remove_diplomacy_dailyinflulence_gain", text::variable_type::num, text::fp_two_places{gain_amount}, text::variable_type::country, target);
	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "influence_explain_4", text::variable_type::x, text::fp_two_places{total_gain});
	text::add_line(state, contents, "influence_explain_5", text::variable_type::x, text::fp_two_places{state.defines.base_greatpower_daily_influence}, text::variable_type::y, text::fp_percentage{1.0f + n.get_modifier_values(sys::national_mod_offsets::influence_modifier)}, text::variable_type::val, text::fp_percentage{1.0f + n.get_modifier_values(sys::national_mod_offsets::influence)});

	text::add_line(state, contents, "influence_explain_6", text::variable_type::x, text::fp_percentage{1.0f + n.get_modifier_values(sys::national_mod_offsets::influence_modifier)});
	active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::influence_modifier, false);

	text::add_line(state, contents, "influence_explain_7", text::variable_type::x, text::fp_percentage{1.0f + n.get_modifier_values(sys::national_mod_offsets::influence)});
	active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::influence, false);

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "influence_explain_8", text::variable_type::x, text::fp_two_places{base_shares});

	if(discredit_factor != 0 || neighbor_factor != 0 || sphere_neighbor_factor != 0 || continent_factor != 0 || puppet_factor != 0 || relationship_factor != 0 || investment_factor != 0 ||  pop_factor != 0 || score_factor != 0) {

		text::add_line(state, contents, "influence_explain_9");

		if(discredit_factor != 0) {
			text::add_line(state, contents, "influence_explain_10", text::variable_type::x, text::fp_two_places{discredit_factor}, 15);
		}
		if(neighbor_factor != 0) {
			text::add_line(state, contents, "influence_explain_11", text::variable_type::x, text::fp_two_places{neighbor_factor}, 15);
		}
		if(sphere_neighbor_factor != 0) {
			text::add_line(state, contents, "influence_explain_12", text::variable_type::x, text::fp_two_places{sphere_neighbor_factor}, 15);
		}
		if(continent_factor != 0) {
			text::add_line(state, contents, "influence_explain_13", text::variable_type::x, text::fp_two_places{continent_factor}, 15);
		}
		if(puppet_factor != 0) {
			text::add_line(state, contents, "influence_explain_14", text::variable_type::x, text::fp_two_places{puppet_factor}, 15);
		}
		if(relationship_factor != 0) {
			text::add_line(state, contents, "influence_explain_15", text::variable_type::x, text::fp_two_places{relationship_factor}, 15);
		}
		if(investment_factor != 0) {
			text::add_line(state, contents, "influence_explain_16", text::variable_type::x, text::fp_two_places{investment_factor}, 15);
		}
		if(pop_factor != 0) {
			text::add_line(state, contents, "influence_explain_17", text::variable_type::x, text::fp_two_places{pop_factor}, 15);
		}
		if(score_factor != 0) {
			text::add_line(state, contents, "influence_explain_18", text::variable_type::x, text::fp_two_places{score_factor}, 15);
		}
	}
}

class diplomacy_priority_button : public right_click_button_element_base {
	static std::string_view get_prio_key(uint8_t f) {
		switch(f & nations::influence::priority_mask) {
		case nations::influence::priority_zero:
			return "diplomacy_prio_none";
		case nations::influence::priority_one:
			return "diplomacy_prio_low";
		case nations::influence::priority_two:
			return "diplomacy_prio_middle";
		case nations::influence::priority_three:
			return "diplomacy_prio_high";
		}
		return "diplomacy_prio_none";
	}

public:
	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto rel = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, state.local_player_nation);
		uint8_t rel_flags = bool(rel) ? state.world.gp_relationship_get_status(rel) : 0;
		switch(rel_flags & nations::influence::priority_mask) {
		case nations::influence::priority_zero:
			frame = 0;
			disabled = !command::can_change_influence_priority(state, state.local_player_nation, nation_id, 1);
			break;
		case nations::influence::priority_one:
			frame = 1;
			disabled = !command::can_change_influence_priority(state, state.local_player_nation, nation_id, 2);
			break;
		case nations::influence::priority_two:
			frame = 2;
			disabled = !command::can_change_influence_priority(state, state.local_player_nation, nation_id, 3);
			break;
		case nations::influence::priority_three:
			frame = 3;
			disabled = !command::can_change_influence_priority(state, state.local_player_nation, nation_id, 0);
			break;
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto rel = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, state.local_player_nation);
		uint8_t rel_flags = bool(rel) ? state.world.gp_relationship_get_status(rel) : 0;
		switch(rel_flags & nations::influence::priority_mask) {
		case nations::influence::priority_zero:
			command::change_influence_priority(state, state.local_player_nation, nation_id, 1);
			break;
		case nations::influence::priority_one:
			command::change_influence_priority(state, state.local_player_nation, nation_id, 2);
			break;
		case nations::influence::priority_two:
			command::change_influence_priority(state, state.local_player_nation, nation_id, 3);
			break;
		case nations::influence::priority_three:
			command::change_influence_priority(state, state.local_player_nation, nation_id, 0);
			break;
		}
	}

	void button_right_action(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto rel = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, state.local_player_nation);
		uint8_t rel_flags = bool(rel) ? state.world.gp_relationship_get_status(rel) : 0;
		switch(rel_flags & nations::influence::priority_mask) {
		case nations::influence::priority_zero:
			command::change_influence_priority(state, state.local_player_nation, nation_id, 3);
			break;
		case nations::influence::priority_one:
			command::change_influence_priority(state, state.local_player_nation, nation_id, 0);
			break;
		case nations::influence::priority_two:
			command::change_influence_priority(state, state.local_player_nation, nation_id, 1);
			break;
		case nations::influence::priority_three:
			command::change_influence_priority(state, state.local_player_nation, nation_id, 2);
			break;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		
		if(!nations::is_great_power(state, state.local_player_nation)) {
			text::add_line(state, contents, "diplomacy_cannot_set_prio");
		} else if(nations::is_great_power(state, nation_id)) {
			text::add_line(state, contents, "diplomacy_cannot_set_prio_gp");
		} else {
			explain_influence(state, nation_id, contents);
		}
			
		auto box = text::open_layout_box(contents, 0);
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("diplomacy_set_prio_desc"));
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::influence, false);
	}
};

class diplomacy_country_info : public listbox_row_element_base<dcon::nation_id> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<dcon::nation_id>::on_create(state);
		base_data.position.x -= 14;
		base_data.position.y -= 524;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_select") {
			return make_element_by_type<diplomacy_country_select>(state, id);
		} else if(name == "country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			ptr->base_data.position.y -= 2; // Nudge
			return ptr;
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_prio") {
			return make_element_by_type<diplomacy_priority_button>(state, id);
		} else if(name == "country_boss_flag") {
			return make_element_by_type<nation_overlord_flag>(state, id);
		} else if(name == "country_prestige") {
			return make_element_by_type<nation_prestige_rank_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<nation_industry_rank_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<nation_military_rank_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "country_opinion") {
			return make_element_by_type<nation_player_opinion_text>(state, id);
		} else if(name == "country_relation") {
			return make_element_by_type<nation_player_relations_text>(state, id);
		} else if(name.substr(0, 10) == "country_gp") {
			auto ptr = make_element_by_type<nation_gp_opinion_text>(state, id);
			ptr->rank = uint16_t(std::stoi(std::string{name.substr(10)}));
			return ptr;
		} else {
			return nullptr;
		}
	}
};

struct country_filter_setting {
	country_list_filter general_category = country_list_filter::all;
	dcon::modifier_id continent;
};
struct country_sort_setting {
	country_list_sort sort = country_list_sort::country;
	bool sort_ascend = true;
};

struct dip_make_nation_visible {
	dcon::nation_id data;
};

class diplomacy_country_listbox : public listbox_element_base<diplomacy_country_info, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "diplomacy_country_info";
	}
	void on_update(sys::state& state) noexcept override {
		auto current_filter = retrieve< country_filter_setting>(state, parent);
		auto current_sort = retrieve<country_sort_setting>(state, parent);

		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0) {
				bool passes_filter = [&]() {
					switch(current_filter.general_category) {
						case country_list_filter::all:
							return true;
						case country_list_filter::allies:
							return nations::are_allied(state, id, state.local_player_nation);
						case country_list_filter::enemies:
							return military::are_at_war(state, state.local_player_nation, id);
						case country_list_filter::sphere:
							return state.world.nation_get_in_sphere_of(id) == state.local_player_nation;
						case country_list_filter::neighbors:
							return bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(state.local_player_nation, id));
						default:
							return true;
					}
					//return true;
				}();
				bool right_continent = !current_filter.continent || state.world.nation_get_capital(id).get_continent() == current_filter.continent;

				if(passes_filter && right_continent)
					row_contents.push_back(id);
			}
				
		});
		sort_countries(state, row_contents, current_sort.sort, current_sort.sort_ascend);
		update(state);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dip_make_nation_visible>()) {
			on_update(state);

			auto n = any_cast<dip_make_nation_visible>(payload).data;
			int32_t i = int32_t(row_contents.size());
			for(; i-- > 0;) {
				if(row_contents[i] == n) {
					break;
				}
			}
			if(i != -1) {
				auto current_vis = list_scrollbar->raw_value();
				auto vis_count = int32_t(row_windows.size());

				if(i < current_vis || i >= current_vis + vis_count) {
					list_scrollbar->update_raw_value(state, i);
					update(state);
				}
			}
			return message_result::consumed;
		} else {
			return listbox_element_base<diplomacy_country_info, dcon::nation_id>::get(state, payload);
		}
	}
};

class cb_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto cb = retrieve<military::available_cb>(state, parent);
		dcon::cb_type_id content = cb.cb_type;
		frame = state.world.cb_type_get_sprite_index(content) - 1;

		auto conditions = state.world.cb_type_get_can_use(cb.cb_type);
		if(conditions) {
			disabled = !trigger::evaluate(state, conditions, trigger::to_generic(retrieve<dcon::nation_id>(state, parent)), trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation));
		} else {
			disabled = false;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto cb = retrieve<military::available_cb>(state, parent);
		text::add_line(state, contents, state.world.cb_type_get_name(cb.cb_type));
		if(cb.expiration) {
			text::add_line(state, contents, "until_date", text::variable_type::x, cb.expiration);
		}
		auto conditions = state.world.cb_type_get_can_use(cb.cb_type);
		if(conditions) {
			text::add_line_break_to_layout(state, contents);
			text::add_line(state, contents, "cb_conditions_header");
			ui::trigger_description(state, contents, conditions, trigger::to_generic(retrieve<dcon::nation_id>(state, parent)), trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation));
		}
	}
};

class overlapping_cb_icon : public listbox_row_element_base<military::available_cb> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<cb_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

class overlapping_wargoals : public overlapping_listbox_element_base<overlapping_cb_icon, military::available_cb> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto content = retrieve<dcon::nation_id>(state, parent);
		if(!content || content == state.local_player_nation)
			return;

		auto one_cbs = state.world.nation_get_available_cbs(state.local_player_nation);
		for(auto& cb : one_cbs)
			if(cb.target == content)
				row_contents.push_back(cb);

		for(auto cb : state.world.in_cb_type) {
			if((cb.get_type_bits() & military::cb_flag::always) != 0) {
				if(military::cb_conditions_satisfied(state, state.local_player_nation, content, cb))
					row_contents.push_back(military::available_cb{content, sys::date{}, cb});
			}
		}
		update(state);
		
	}
};

class overlapping_active_wargoals : public overlapping_listbox_element_base<overlapping_wg_icon, military::wg_summary> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto content = retrieve<dcon::nation_id>(state, parent);
		
		auto war = military::find_war_between(state, content, state.local_player_nation);
	
		if(content && content != state.local_player_nation && war) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				if(wg.get_wargoal().get_added_by() == state.local_player_nation && wg.get_wargoal().get_target_nation() == content) {
					row_contents.push_back(military::wg_summary{
						wg.get_wargoal().get_secondary_nation(),
						wg.get_wargoal().get_associated_tag(),
						wg.get_wargoal().get_associated_state(),
						wg.get_wargoal().get_type()
					});
				}
			}
		}


		update(state);

	}
};

class diplomacy_action_add_wargoal_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::nation_id>(state, parent);
		if(!content)
			return;

		disabled = true;

		if(content == state.local_player_nation) {
			return;
		}

		if(state.world.nation_get_diplomatic_points(state.local_player_nation) < state.defines.addwargoal_diplomatic_cost) {
			return;
		}

		auto w = military::find_war_between(state, state.local_player_nation, content);
		if(!w) {
			return;
		}

		bool is_attacker = military::is_attacker(state, w, state.local_player_nation);
		if(!is_attacker && military::defenders_have_status_quo_wargoal(state, w))
			return;

		for(auto cb_type : state.world.in_cb_type) {
			if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::always) == 0 && military::cb_conditions_satisfied(state, state.local_player_nation, content, cb_type)) {
				bool cb_fabbed = false;
				for(auto& fab_cb : state.world.nation_get_available_cbs(state.local_player_nation)) {
					if(fab_cb.cb_type == cb_type && fab_cb.target == content) {
						cb_fabbed = true;
						break;
					}
				}
				if(!cb_fabbed) {
					if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::is_not_constructing_cb) != 0)
						continue; // can only add a constructable cb this way

					auto totalpop = state.world.nation_get_demographics(state.local_player_nation, demographics::total);
					auto jingoism_perc = totalpop > 0 ? state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, state.culture_definitions.jingoism)) / totalpop : 0.0f;

					if(state.world.war_get_is_great(w)) {
						if(jingoism_perc >= state.defines.wargoal_jingoism_requirement * state.defines.gw_wargoal_jingoism_requirement_mod) {
							disabled = false;
							return;
						}
					} else {
						if(jingoism_perc >= state.defines.wargoal_jingoism_requirement) {
							disabled = false;
							return;
						}
					}
				} else {
					disabled = false;
					return;
				}
			} else { // this is an always CB
				// prevent duplicate war goals
				if(military::cb_conditions_satisfied(state, state.local_player_nation, content, cb_type) && military::can_add_always_cb_to_war(state, state.local_player_nation, content, cb_type, w)) {
					disabled = false;
					return;
				}
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::add_wargoal;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::nation_id>(state, parent);
		if(content == state.local_player_nation) {
			text::add_line_with_condition(state, contents, "add_wg_1", false);
			return;
		}

		if(state.defines.addwargoal_diplomatic_cost > 0) {
			text::add_line_with_condition(state, contents, "add_wg_3", state.world.nation_get_diplomatic_points(state.local_player_nation) >= state.defines.addwargoal_diplomatic_cost, text::variable_type::x, int64_t(state.defines.addwargoal_diplomatic_cost));
		}

		auto w = military::find_war_between(state, state.local_player_nation, content);
		text::add_line_with_condition(state, contents, "add_wg_2", bool(w));
		if(!w) {
			return;
		}

		bool is_attacker = military::is_attacker(state, w, state.local_player_nation);
		text::add_line_with_condition(state, contents, "add_wg_5", is_attacker || !military::defenders_have_status_quo_wargoal(state, w));

		for(auto cb_type : state.world.in_cb_type) {
			if(military::cb_conditions_satisfied(state, state.local_player_nation, content, cb_type)) {
				if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::always) == 0) {
					for(auto& fab_cb : state.world.nation_get_available_cbs(state.local_player_nation)) {
						if(fab_cb.cb_type == cb_type && fab_cb.target == content) {
							return;
						}
					}
				} else { // this is an always CB
					// prevent duplicate war goals
					if(military::can_add_always_cb_to_war(state, state.local_player_nation, content, cb_type, w)) {
						return;
					}
				}
			}
		}

		// if we hit this, it means no existing cb is ready to be applied
		auto totalpop = state.world.nation_get_demographics(state.local_player_nation, demographics::total);
		auto jingoism_perc = totalpop > 0 ? state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, state.culture_definitions.jingoism)) / totalpop : 0.0f;
		if(state.world.war_get_is_great(w)) {
			text::add_line_with_condition(state, contents, "add_wg_4", jingoism_perc >= state.defines.wargoal_jingoism_requirement * state.defines.gw_wargoal_jingoism_requirement_mod, text::variable_type::x, text::fp_percentage_one_place{jingoism_perc}, text::variable_type::y, text::fp_percentage_one_place{state.defines.wargoal_jingoism_requirement * state.defines.gw_wargoal_jingoism_requirement_mod});
		} else {
			text::add_line_with_condition(state, contents, "add_wg_4", jingoism_perc >= state.defines.wargoal_jingoism_requirement, text::variable_type::x, text::fp_percentage_one_place{jingoism_perc}, text::variable_type::y, text::fp_percentage_one_place{state.defines.wargoal_jingoism_requirement});
		}
	}
};

class primary_culture : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto pc = state.world.nation_get_primary_culture(retrieve<dcon::nation_id>(state, parent));
		set_text(state, text::produce_simple_string(state, pc.get_name()));
	}
};

class accepted_cultures : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto ac = state.world.nation_get_accepted_cultures(retrieve<dcon::nation_id>(state, parent));

		std::string t;
		if(ac.size() > 0) {
			t += text::produce_simple_string(state, state.world.culture_get_name(ac[0]));
		}
		for(uint32_t i = 1; i < ac.size(); ++i) {
			t += ", " ;
			t += text::produce_simple_string(state, state.world.culture_get_name(ac[i]));
		}

		set_text(state, text::produce_simple_string(state, t));
	}
};

class gp_info_background : public image_element_base {
public:
	bool show = false;

	void on_update(sys::state& state) noexcept override {
		show = nations::is_great_power(state, retrieve<dcon::nation_id>(state, parent));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			image_element_base::render(state, x, y);
	}
};

struct gp_detail_num {
	int32_t value = 0;
};

class gp_detail_banned : public image_element_base {
public:
	bool show = false;
	void on_update(sys::state& state) noexcept override {
		auto gp = nations::get_nth_great_power(state, uint16_t(retrieve<gp_detail_num>(state, parent).value));
		auto target = retrieve<dcon::nation_id>(state, parent);
		show = (state.world.gp_relationship_get_status(state.world.get_gp_relationship_by_gp_influence_pair(target, gp)) & nations::influence::is_banned) != 0;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto gp = nations::get_nth_great_power(state, uint16_t(retrieve<gp_detail_num>(state, parent).value));
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto rel = state.world.get_gp_relationship_by_gp_influence_pair(target, gp);

		text::add_line(state, contents, "dp_inf_tooltip_ban", text::variable_type::x, state.world.gp_relationship_get_penalty_expires_date(rel));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			image_element_base::render(state, x, y);
	}
};

class gp_detail_discredited : public image_element_base {
public:
	bool show = false;
	void on_update(sys::state& state) noexcept override {
		auto gp = nations::get_nth_great_power(state, uint16_t(retrieve<gp_detail_num>(state, parent).value));
		auto target = retrieve<dcon::nation_id>(state, parent);
		show = (state.world.gp_relationship_get_status(state.world.get_gp_relationship_by_gp_influence_pair(target, gp)) & nations::influence::is_discredited) != 0;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto gp = nations::get_nth_great_power(state, uint16_t(retrieve<gp_detail_num>(state, parent).value));
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto rel = state.world.get_gp_relationship_by_gp_influence_pair(target, gp);

		text::add_line(state, contents, "dp_inf_tooltip_discredit", text::variable_type::x, state.world.gp_relationship_get_penalty_expires_date(rel));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			image_element_base::render(state, x, y);
	}
};

class great_power_opinion_detail : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto gp = nations::get_nth_great_power(state, uint16_t(retrieve<gp_detail_num>(state, parent).value));
		auto target = retrieve<dcon::nation_id>(state, parent);

		set_text(state, text::get_influence_level_name(state, state.world.gp_relationship_get_status(state.world.get_gp_relationship_by_gp_influence_pair(target, gp))));
	}
};
class great_power_influence_detail : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto gp = nations::get_nth_great_power(state, uint16_t(retrieve<gp_detail_num>(state, parent).value));
		auto target = retrieve<dcon::nation_id>(state, parent);

		set_text(state, text::format_float(state.world.gp_relationship_get_influence(state.world.get_gp_relationship_by_gp_influence_pair(target, gp)), 1));
	}
};
class great_power_investment_detail : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto gp = nations::get_nth_great_power(state, uint16_t(retrieve<gp_detail_num>(state, parent).value));
		auto target = retrieve<dcon::nation_id>(state, parent);

		set_text(state, text::format_money(state.world.unilateral_relationship_get_foreign_investment(state.world.get_unilateral_relationship_by_unilateral_pair(target, gp))));
	}
};

class great_power_detail_flag : public flag_button {
public:
	virtual dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto gp = nations::get_nth_great_power(state, uint16_t(retrieve<gp_detail_num>(state, parent).value));
		return state.world.nation_get_identity_from_identity_holder(gp);
	}
};

struct dip_tab_request {
	ui::diplomacy_window_tab tab = ui::diplomacy_window_tab::great_powers;
};

class great_power_inf_detail : public window_element_base {
public:
	int32_t gp_num = 0;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag") {
			return make_element_by_type<great_power_detail_flag>(state, id);
		} else if(name == "nongp_country_opinion") {
			return make_element_by_type<great_power_opinion_detail>(state, id);
		} else if(name == "nongp_country_influence") {
			return make_element_by_type<great_power_influence_detail>(state, id);
		} else if(name == "nongp_country_invest") {
			return make_element_by_type<great_power_investment_detail>(state, id);
		} else if(name == "country_discredited") {
			return make_element_by_type<gp_detail_discredited>(state, id);
		} else if(name == "country_banned_embassy") {
			return make_element_by_type<gp_detail_banned>(state, id);
		}

		return nullptr;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<gp_detail_num>()) {
			payload.emplace<gp_detail_num>(gp_detail_num{ gp_num });
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

template<class T>
class nation_status_diplomacy_window : public simple_text_element_base {
public:

	void on_update(sys::state& state) noexcept override {
		T content = retrieve<T>(state, parent);
		dcon::overlord_id overlord = state.world.nation_get_overlord_as_subject(content);
		dcon::nation_id overlord_nation = state.world.overlord_get_ruler(overlord);
		auto fat_id = dcon::fatten(state.world, overlord_nation);
		if(state.world.nation_get_is_substate(content)) {
			simple_text_element_base::set_text(state, ui::get_status_text(state, content) + ", " + text::produce_simple_string(state, "substate_of_nocolor") + " " + text::get_name_as_string(state, fat_id));
		} else if(overlord_nation) {
			simple_text_element_base::set_text(state, ui::get_status_text(state, content) + ", " + text::produce_simple_string(state, "satellite_of_nocolor") + " " + text::get_name_as_string(state, fat_id));
		} else {
			simple_text_element_base::set_text(state, ui::get_status_text(state, content));
		}
	}
};

class diplomacy_country_facts : public window_element_base {
private:
	flag_button* country_flag = nullptr;
	nation_player_relations_text* country_relation = nullptr;
	image_element_base* country_relation_icon = nullptr;

	ui::xy_pair sub_window_top_left = {};

	std::array<element_base*, 7> gp_elements;
	std::array<element_base*, 9> non_gp_elements;
	std::array<element_base*, 21> war_elements;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		{
			auto win = make_element_by_type<great_power_inf_detail>(state, state.ui_state.defs_by_name.find("diplomacy_non_gp_extra_info")->second.definition);
			win->gp_num = 0;
			win->base_data.position = sub_window_top_left;
			win->base_data.position.y += 35 * 0;
			non_gp_elements[1] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<great_power_inf_detail>(state, state.ui_state.defs_by_name.find("diplomacy_non_gp_extra_info")->second.definition);
			win->gp_num = 1;
			win->base_data.position = sub_window_top_left;
			win->base_data.position.y += 35 * 1;
			non_gp_elements[2] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<great_power_inf_detail>(state, state.ui_state.defs_by_name.find("diplomacy_non_gp_extra_info")->second.definition);
			win->gp_num = 2;
			win->base_data.position = sub_window_top_left;
			win->base_data.position.y += 35 * 2;
			non_gp_elements[3] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<great_power_inf_detail>(state, state.ui_state.defs_by_name.find("diplomacy_non_gp_extra_info")->second.definition);
			win->gp_num = 3;
			win->base_data.position = sub_window_top_left;
			win->base_data.position.y += 35 * 3;
			non_gp_elements[4] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<great_power_inf_detail>(state, state.ui_state.defs_by_name.find("diplomacy_non_gp_extra_info")->second.definition);
			win->gp_num = 4;
			win->base_data.position = sub_window_top_left;
			win->base_data.position.y += 35 * 0;
			win->base_data.position.x += 157;
			non_gp_elements[5] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<great_power_inf_detail>(state, state.ui_state.defs_by_name.find("diplomacy_non_gp_extra_info")->second.definition);
			win->gp_num = 5;
			win->base_data.position = sub_window_top_left;
			win->base_data.position.y += 35 * 1;
			win->base_data.position.x += 157;
			non_gp_elements[6] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<great_power_inf_detail>(state, state.ui_state.defs_by_name.find("diplomacy_non_gp_extra_info")->second.definition);
			win->gp_num = 6;
			win->base_data.position = sub_window_top_left;
			win->base_data.position.y += 35 * 2;
			win->base_data.position.x += 157;
			non_gp_elements[7] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<great_power_inf_detail>(state, state.ui_state.defs_by_name.find("diplomacy_non_gp_extra_info")->second.definition);
			win->gp_num = 7;
			win->base_data.position = sub_window_top_left;
			win->base_data.position.y += 35 * 3;
			win->base_data.position.x += 157;
			non_gp_elements[8] = win.get();
			add_child_to_front(std::move(win));
		}

	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "gp_extra_info_bg") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			gp_elements[0] = ptr.get();
			return ptr;
		} else if(name == "country_gp_friendly_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			gp_elements[1] = ptr.get();
			return ptr;
		} else if(name == "country_gp_cordial_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			gp_elements[2] = ptr.get();
			return ptr;
		} else if(name == "country_gp_sphere_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			gp_elements[3] = ptr.get();
			return ptr;
		} else if(name == "country_gp_friendly") {
			auto ptr = make_element_by_type<overlapping_friendly_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			gp_elements[4] = ptr.get();
			return ptr;
		} else if(name == "country_gp_cordial") {
			auto ptr = make_element_by_type<overlapping_cordial_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			gp_elements[5] = ptr.get();
			return ptr;
		} else if(name == "country_gp_sphere") {
			auto ptr = make_element_by_type<overlapping_sphere_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			gp_elements[6] = ptr.get();
			return ptr;
		} else if(name == "nongp_extra_info_bg") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			non_gp_elements[0] = ptr.get();
			sub_window_top_left = ptr->base_data.position;
			sub_window_top_left.x += 8;
			sub_window_top_left.y += 12;
			return ptr;
		} else if(name == "country_flag") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "country_flag_overlay") {
			return make_element_by_type<nation_flag_frame>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_status") {
			auto ptr = make_element_by_type<nation_status_diplomacy_window<dcon::nation_id>>(state, id);
			return ptr;
		} else if(name == "selected_nation_totalrank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "ideology_icon") {
			return make_element_by_type<nation_ruling_party_ideology_plupp>(state, id);
		} else if(name == "country_party") {
			return make_element_by_type<nation_ruling_party_text>(state, id);
		} else if(name == "country_gov") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "country_tech") {
			return make_element_by_type<national_tech_school>(state, id);
		} else if(name == "our_relation_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			country_relation_icon = ptr.get();
			return ptr;
		} else if(name == "our_relation") {
			auto ptr = make_element_by_type<nation_player_relations_text>(state, id);
			country_relation = ptr.get();
			return ptr;
		} else if(name == "country_prestige") {
			return make_element_by_type<nation_prestige_text>(state, id);
		} else if(name == "selected_prestige_rank") {
			return make_element_by_type<nation_prestige_rank_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<nation_industry_score_text>(state, id);
		} else if(name == "selected_industry_rank") {
			return make_element_by_type<nation_industry_rank_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<nation_military_score_text>(state, id);
		} else if(name == "selected_military_rank") {
			return make_element_by_type<nation_military_rank_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else if(name == "selected_total_rank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "country_population") {
			return make_element_by_type<nation_population_text>(state, id);
		} else if(name == "country_primary_cultures") {
			return make_element_by_type<primary_culture>(state, id);
		} else if(name == "country_accepted_cultures") {
			return make_element_by_type<accepted_cultures>(state, id);
		} else if(name == "war_extra_info_bg") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			war_elements[0] = ptr.get();
			return ptr;
		} else if(name == "country_wars") {
			auto ptr = make_element_by_type<overlapping_enemy_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			war_elements[1] = ptr.get();
			return ptr;
		} else if(name == "country_wars_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			war_elements[2] = ptr.get();
			return ptr;
		} else if(name == "country_allies_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			war_elements[3] = ptr.get();
			return ptr;
		} else if(name == "country_protected_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			war_elements[4] = ptr.get();
			return ptr;
		} else if(name == "country_truce_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			war_elements[5] = ptr.get();
			return ptr;
		} else if(name == "country_cb_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			war_elements[6] = ptr.get();
			return ptr;
		} else if(name == "infamy_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			war_elements[7] = ptr.get();
			return ptr;
		} else if(name == "warexhastion_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			war_elements[8] = ptr.get();
			return ptr;
		} else if(name == "brigade_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			war_elements[9] = ptr.get();
			return ptr;
		} else if(name == "ships_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			war_elements[10] = ptr.get();
			return ptr;
		} else if(name == "country_allies") {
			auto ptr = make_element_by_type<overlapping_ally_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			war_elements[11] = ptr.get();
			return ptr;
		} else if(name == "country_protected") {
			auto ptr = make_element_by_type<overlapping_sphere_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			war_elements[12] = ptr.get();
			return ptr;
		} else if(name == "country_truce") {
			auto ptr = make_element_by_type<overlapping_truce_flags>(state, id);
			ptr->base_data.position.y -= 8 - 1; // Nudge
			war_elements[13] = ptr.get();
			return ptr;
		} else if(name == "current_wargoals") { 
			auto ptr = make_element_by_type<overlapping_active_wargoals>(state, id);
			war_elements[14] = ptr.get();
			return ptr;
		} else if(name == "country_cb") { 
			auto ptr = make_element_by_type<overlapping_wargoals>(state, id);
			war_elements[15] = ptr.get();
			return ptr;
		} else if(name == "infamy_text") {
			auto ptr = make_element_by_type<nation_infamy_text>(state, id);
			war_elements[16] = ptr.get();
			return ptr;
		} else if(name == "warexhastion_text") {
			auto ptr = make_element_by_type<diplomacy_war_exhaustion>(state, id);
			war_elements[17] = ptr.get();
			return ptr;
		} else if(name == "brigade_text") {
			auto ptr = make_element_by_type<diplomacy_nation_armies_text>(state, id);
			war_elements[18] = ptr.get();
			return ptr;
		} else if(name == "ships_text") {
			auto ptr = make_element_by_type<diplomacy_nation_navies_text>(state, id);
			war_elements[19] = ptr.get();
			return ptr;
		} else if(name == "add_wargoal") {
			auto ptr = make_element_by_type<diplomacy_action_add_wargoal_button>(state, id);
			war_elements[20] = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::nation_id>(state, parent);
		country_relation->set_visible(state, content != state.local_player_nation);
		country_relation_icon->set_visible(state, content != state.local_player_nation);
		auto active_tab = retrieve<dip_tab_request>(state, parent).tab;

		if(active_tab != diplomacy_window_tab::great_powers) {
			for(auto p : gp_elements) {
				p->set_visible(state, false);
			}
			for(auto p : non_gp_elements) {
				p->set_visible(state, false);
			}
			for(auto p : war_elements) {
				p->set_visible(state, true);
			}
		} else if(nations::is_great_power(state, content)) {
			for(auto p : gp_elements) {
				p->set_visible(state, true);
			}
			for(auto p : non_gp_elements) {
				p->set_visible(state, false);
			}
			for(auto p : war_elements) {
				p->set_visible(state, false);
			}
		} else {
			for(auto p : gp_elements) {
				p->set_visible(state, false);
			}
			for(auto p : non_gp_elements) {
				p->set_visible(state, true);
			}
			for(auto p : war_elements) {
				p->set_visible(state, false);
			}
		}
		
	}
};

class overlapping_attacker_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override {
		row_contents.clear();
		dcon::war_id w = retrieve<dcon::war_id>(state, parent);
		auto war = dcon::fatten(state.world, w);
		row_contents.push_back(war.get_primary_attacker().get_identity_from_identity_holder().id);
		for(auto o : war.get_war_participant())
			if(o.get_is_attacker() == true && o.get_nation() != war.get_primary_attacker())
				row_contents.push_back(o.get_nation().get_identity_from_identity_holder().id);
		update(state);
	}
};
class overlapping_defender_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override {
		row_contents.clear();
		dcon::war_id w = retrieve<dcon::war_id>(state, parent);
		auto war = dcon::fatten(state.world, w);
		row_contents.push_back(war.get_primary_defender().get_identity_from_identity_holder().id);
		for(auto o : war.get_war_participant())
			if(o.get_is_attacker() == false && o.get_nation() != war.get_primary_defender())
				row_contents.push_back(o.get_nation().get_identity_from_identity_holder().id);
		update(state);
	}
};

template<bool IsAttacker>
class war_side_strength_text : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		dcon::war_id content = retrieve<dcon::war_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, content);
		int32_t strength = 0;
		for(auto o : fat_id.get_war_participant())
			if(o.get_is_attacker() == IsAttacker)
				strength += int32_t(o.get_nation().get_military_score());
		set_button_text(state, std::to_string(strength));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		dcon::war_id content = retrieve<dcon::war_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, content);
		for(auto o : fat_id.get_war_participant()) {
			if(o.get_is_attacker() == IsAttacker) {
				auto name = o.get_nation().get_name();
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, name), text::text_color::yellow);
				text::add_to_layout_box(state, contents, box, std::string{":"}, text::text_color::yellow);
				text::add_space_to_layout_box(state, contents, box);
				auto strength = int32_t(o.get_nation().get_military_score());
				text::add_to_layout_box(state, contents, box, std::to_string(strength), text::text_color::white);
				text::close_layout_box(contents, box);
			}
		}
	}
};

template<bool B>
class diplomacy_join_war_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, "");
	}

	void on_update(sys::state& state) noexcept override {
		dcon::war_id war_id = retrieve<dcon::war_id>(state, parent);
		disabled = !command::can_intervene_in_war(state, state.local_player_nation, war_id, B);
	}

	void button_action(sys::state& state) noexcept override {
		dcon::war_id war_id = retrieve<dcon::war_id>(state, parent);
		command::intervene_in_war(state, state.local_player_nation, war_id, B);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		
			dcon::nation_id nation_id = retrieve<dcon::nation_id>(state, parent);
			dcon::war_id w = retrieve<dcon::war_id>(state, parent);

			if(!state.world.war_get_is_great(w)) {
				text::add_line(state, contents, "intervene_1");
			} else {
				text::add_line(state, contents, "intervene_2");
			}
			text::add_line_break_to_layout(state, contents);

			text::add_line_with_condition(state, contents, "intervene_3", nations::is_great_power(state, state.local_player_nation));
			text::add_line_with_condition(state, contents, "intervene_4", !nations::is_involved_in_crisis(state, state.local_player_nation));
			if(state.defines.min_months_to_intervene > 0) {
				text::add_line_with_condition(state, contents, "intervene_5", state.current_date >= state.world.war_get_start_date(w) + int32_t(30.0f * state.defines.min_months_to_intervene), text::variable_type::x, int64_t(state.defines.min_months_to_intervene));
			}
			text::add_line_with_condition(state, contents, "intervene_6", military::joining_war_does_not_violate_constraints(state, state.local_player_nation, w, B));

			if constexpr(!B) {
				text::add_line_with_condition(state, contents, "intervene_8", !military::has_truce_with(state, state.local_player_nation, state.world.war_get_primary_attacker(w)));
			}

			if(!state.world.war_get_is_great(w)) {
				auto defender = state.world.war_get_primary_defender(w);
				auto rel_w_defender = state.world.get_gp_relationship_by_gp_influence_pair(defender, state.local_player_nation);
				auto inf = state.world.gp_relationship_get_status(rel_w_defender) & nations::influence::level_mask;

				text::add_line_with_condition(state, contents, "intervene_17", inf == nations::influence::level_friendly);

				text::add_line_with_condition(state, contents, "intervene_7", !state.world.war_get_is_crisis_war(w));
				text::add_line_with_condition(state, contents, "intervene_9", !B);
				text::add_line_with_condition(state, contents, "intervene_10", !military::defenders_have_non_status_quo_wargoal(state, w));
				text::add_line_with_condition(state, contents, "intervene_11", military::primary_warscore(state, w) >= -state.defines.min_warscore_to_intervene, text::variable_type::x, int64_t(-state.defines.min_warscore_to_intervene));

			} else {
				if constexpr(B) {
					text::add_line_with_condition(state, contents, "intervene_8", !military::joining_as_attacker_would_break_truce(state, state.local_player_nation, w));
				}

				text::add_line_with_condition(state, contents, "intervene_12", state.world.nation_get_war_exhaustion(state.local_player_nation) < state.defines.gw_intervene_max_exhaustion, text::variable_type::x, int64_t(state.defines.gw_intervene_max_exhaustion));

				auto primary_on_side = B ? state.world.war_get_primary_attacker(w) : state.world.war_get_primary_defender(w);
				auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(primary_on_side, state.local_player_nation);

				text::add_line_with_condition(state, contents, "intervene_13", state.world.diplomatic_relation_get_value(rel) >= state.defines.gw_intervene_min_relations, text::variable_type::x, int64_t(state.defines.gw_intervene_min_relations));

				bool any_in_sphere = false;
				bool any_allied = false;
				bool any_armies = false;

				for(auto p : state.world.war_get_war_participant(w)) {
					if(p.get_is_attacker() != B) { // scan nations on other side
						if(p.get_nation().get_in_sphere_of() == state.local_player_nation)
							any_in_sphere = true;

						auto irel = state.world.get_diplomatic_relation_by_diplomatic_pair(p.get_nation(), state.local_player_nation);
						if(state.world.diplomatic_relation_get_are_allied(irel))
							any_allied = true;

						for(auto prov : p.get_nation().get_province_ownership()) {
							for(auto arm : prov.get_province().get_army_location()) {
								if(arm.get_army().get_controller_from_army_control() == state.local_player_nation)
									any_armies = true;
							}
						}
					}
				}
				text::add_line_with_condition(state, contents, "intervene_14", !any_in_sphere);
				text::add_line_with_condition(state, contents, "intervene_15", !any_allied);
				text::add_line_with_condition(state, contents, "intervene_16", !any_armies);
			}

		
	}
};

class wargoal_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg = retrieve<dcon::wargoal_id>(state, parent);
		frame = state.world.cb_type_get_sprite_index(state.world.wargoal_get_type(wg)) - 1;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto wg = retrieve<dcon::wargoal_id>(state, parent);
		auto cb = state.world.wargoal_get_type(wg);
		text::add_line(state, contents, state.world.cb_type_get_name(cb));

		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "war_goal_1", text::variable_type::x, state.world.wargoal_get_added_by(wg));
		text::add_line(state, contents, "war_goal_2", text::variable_type::x, state.world.wargoal_get_target_nation(wg));
		if(state.world.wargoal_get_associated_state(wg)) {
			text::add_line(state, contents, "war_goal_3", text::variable_type::x, state.world.wargoal_get_associated_state(wg));
		}
		if(state.world.wargoal_get_associated_tag(wg)) {
			text::add_line(state, contents, "war_goal_4", text::variable_type::x, state.world.wargoal_get_associated_tag(wg));
		} else if(state.world.wargoal_get_secondary_nation(wg)) {
			text::add_line(state, contents, "war_goal_4", text::variable_type::x, state.world.wargoal_get_secondary_nation(wg));
		}
		if(state.world.wargoal_get_ticking_war_score(wg) != 0) {
			text::add_line(state, contents, "war_goal_5", text::variable_type::x, text::fp_one_place{state.world.wargoal_get_ticking_war_score(wg)});
		}
	}
};

class overlapping_wargoal_icon : public listbox_row_element_base<dcon::wargoal_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<wargoal_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

template<bool B>
class diplomacy_war_overlapping_wargoals : public overlapping_listbox_element_base<overlapping_wargoal_icon, dcon::wargoal_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		dcon::war_id content = retrieve<dcon::war_id>(state, parent);
		for(auto wg : state.world.war_get_wargoals_attached(content)) {
			if(military::is_attacker(state, content, wg.get_wargoal().get_added_by()) == B)
				row_contents.push_back(wg.get_wargoal().id);
		}

		update(state);
	}
};

class war_name_text : public generic_multiline_text<dcon::war_id> {
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::war_id id) noexcept override {
		contents.fixed_parameters.suppress_hyperlinks = true;

		auto war = dcon::fatten(state.world, id);

		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		military::populate_war_text_subsitutions(state, war, sub);
		text::add_to_layout_box(state, contents, box, state.world.war_get_name(war), sub);
		text::close_layout_box(contents, box);
	}
};

class war_score_progress_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto war = retrieve<dcon::war_id>(state, parent);
		if(war) {
			auto ws = military::primary_warscore(state, war);
			progress = ws / 200.0f + 0.5f;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto war = retrieve<dcon::war_id>(state, parent);
		text::add_line(state, contents, "war_score_1", text::variable_type::x, text::fp_one_place{military::primary_warscore_from_occupation(state, war)});
		text::add_line(state, contents, "war_score_2", text::variable_type::x, text::fp_one_place{military::primary_warscore_from_battles(state, war)});
		text::add_line(state, contents, "war_score_3", text::variable_type::x, text::fp_one_place{military::primary_warscore_from_war_goals(state, war)});
		text::add_line(state, contents, "war_score_4", text::variable_type::x, text::fp_one_place{ military::primary_warscore_from_blockades(state, war) });
	}
};

struct war_bar_position {
	ui::xy_pair bottom_left = ui::xy_pair{0,0};
	int16_t width = 0;
};

class attacker_peace_goal : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto bar_pos = retrieve<war_bar_position>(state, parent);
		auto war = retrieve<dcon::war_id>(state, parent);

		auto attacker_cost = std::min(military::attacker_peace_cost(state, war), 100);
		auto x_pos = int16_t((float(attacker_cost) / 200.0f + 0.5f) * float(bar_pos.width));

		base_data.position.x = int16_t(x_pos + bar_pos.bottom_left.x - base_data.size.x / 2);
		base_data.position.y = bar_pos.bottom_left.y;
	}
};

class defender_peace_goal : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto bar_pos = retrieve<war_bar_position>(state, parent);
		auto war = retrieve<dcon::war_id>(state, parent);

		auto defender_cost = std::min(military::defender_peace_cost(state, war), 100);
		auto x_pos = int16_t((float(-defender_cost) / 200.0f + 0.5f) * float(bar_pos.width));

		base_data.position.x = int16_t(x_pos + bar_pos.bottom_left.x - base_data.size.x / 2);
		base_data.position.y = bar_pos.bottom_left.y;
	}
};

class war_bg : public image_element_base {
	void on_update(sys::state& state) noexcept override {
		auto war = retrieve<dcon::war_id>(state, parent);
		if(state.world.war_get_is_great(war)) {
			frame = 2;
		} else if(state.world.war_get_is_crisis_war(war)) {
			frame = 1;
		} else {
			frame = 0;
		}
	}
};

class war_score_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto war = retrieve<dcon::war_id>(state, parent);
		if(war) {
			auto ws = military::primary_warscore(state, war) / 100.0f;
			set_text(state, text::format_percentage(ws, 0));
		}
	}
};

class diplomacy_war_info : public listbox_row_element_base<dcon::war_id> {
public:
	war_bar_position bar_position;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		base_data.position.x = base_data.position.y = 0;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "diplo_war_entrybg") {
			return make_element_by_type<war_bg>(state, id);
		} else if(name == "war_name") {
			auto ptr = make_element_by_type<war_name_text>(state, id);
			//ptr->base_data.position.x += 90; // Nudge
			return ptr;
		} else if(name == "attackers_mil_strength") {
			auto ptr = make_element_by_type<war_side_strength_text<true>>(state, id);
			ptr->base_data.position.y -= 4; // Nudge
			return ptr;
		} else if(name == "defenders_mil_strength") {
			auto ptr = make_element_by_type<war_side_strength_text<false>>(state, id);
			ptr->base_data.position.y -= 4; // Nudge
			return ptr;
		} else if(name == "warscore") {
			auto ptr = make_element_by_type<war_score_progress_bar>(state, id);
			bar_position.width = ptr->base_data.size.x;
			bar_position.bottom_left = ui::xy_pair{ptr->base_data.position.x, int16_t(ptr->base_data.position.y + ptr->base_data.size.y)};
			return ptr;
		} if(name == "diplo_warscore_marker1") {
			return make_element_by_type<attacker_peace_goal>(state, id);
		} else if(name == "diplo_warscore_marker2") {
			return make_element_by_type<defender_peace_goal>(state, id);
		} else if(name == "warscore_text") {
			auto ptr = make_element_by_type<war_score_text>(state, id);
			ptr->base_data.position.y -= 2;
			return ptr;
		} else if(name == "attackers") {
			auto ptr = make_element_by_type<overlapping_attacker_flags>(state, id);
			ptr->base_data.position.y -= 8 - 2;
			return ptr;
		} else if(name == "defenders") {
			auto ptr = make_element_by_type<overlapping_defender_flags>(state, id);
			ptr->base_data.position.y -= 8 - 2;
			return ptr;
		} else if(name == "attackers_wargoals") {
			return make_element_by_type<diplomacy_war_overlapping_wargoals<true>>(state, id);
		} else if(name == "defenders_wargoals") {
			return make_element_by_type<diplomacy_war_overlapping_wargoals<false>>(state, id);
		} else if(name == "join_attackers") {
			return make_element_by_type<diplomacy_join_war_button<true>>(state, id);
		} else if(name == "join_defenders") {
			return make_element_by_type<diplomacy_join_war_button<false>>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<war_bar_position>()) {
			payload.emplace<war_bar_position>(bar_position);
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::war_id>::get(state, payload);
	}
};

class justifying_cb_type_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::nation_id content = retrieve<dcon::nation_id>(state, parent);
		auto fat = dcon::fatten(state.world, content);
		frame = fat.get_constructing_cb_type().get_sprite_index() - 1;
	}
};

class justifying_cb_progress : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::nation_id content = retrieve<dcon::nation_id>(state, parent);
		auto fat = dcon::fatten(state.world, content);
		progress = (fat.get_constructing_cb_progress() / 100.0f);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto fab_by = retrieve<dcon::nation_id>(state, parent);
		if(fab_by == state.local_player_nation) {
			auto target = state.world.nation_get_constructing_cb_target(state.local_player_nation);
			
			if(nations::is_involved_in_crisis(state, state.local_player_nation)) {
				text::add_line(state, contents, "fab_is_paused");
			} else {
				auto rem_progress = 100.0f - state.world.nation_get_constructing_cb_progress(state.local_player_nation);
				auto daily_progress = state.defines.cb_generation_base_speed * state.world.nation_get_constructing_cb_type(state.local_player_nation).get_construction_speed()* (state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::cb_generation_speed_modifier) + 1.0f);
				auto days = int32_t(std::ceil(rem_progress / daily_progress));
				text::add_line(state, contents, "fab_finish_date", text::variable_type::date, state.current_date + days);
			}

			text::add_line_break_to_layout(state, contents);
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::cb_generation_speed_modifier, true);
		}
	}
};

class justifying_attacker_flag : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) noexcept override {
		const dcon::nation_id content = retrieve<dcon::nation_id>(state, parent);
		auto fat = dcon::fatten(state.world, content);
		row_contents.clear();
		row_contents.push_back(fat.get_identity_from_identity_holder().id);
		update(state);
	}
};

class justifying_defender_flag : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) noexcept override {
		const dcon::nation_id content = retrieve<dcon::nation_id>(state, parent);
		auto fat = dcon::fatten(state.world, content);
		row_contents.clear();
		row_contents.push_back(fat.get_constructing_cb_target().get_identity_from_identity_holder().id);
		update(state);
	}
};

class diplomacy_casus_belli_cancel_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::nation_id content = retrieve<dcon::nation_id>(state, parent);
		if(content != state.local_player_nation) {
			disabled = true;
		} else {
			disabled = !command::can_cancel_cb_fabrication(state, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		const dcon::nation_id content = retrieve<dcon::nation_id>(state, parent);
		command::cancel_cb_fabrication(state, content);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(retrieve<dcon::nation_id>(state, parent) == state.local_player_nation)
			button_element_base::render(state, x, y);
	}
};

class cb_progress_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto just_progress = state.world.nation_get_constructing_cb_progress(retrieve<dcon::nation_id>(state, parent));
		set_text(state, text::format_percentage(just_progress / 100.0f, 0));
	}
};

class diplomacy_casus_belli_entry : public listbox_row_element_base<dcon::nation_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "diplo_cb_entrybg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "cb_type_icon") {
			return make_element_by_type<justifying_cb_type_icon>(state, id);
		} else if(name == "cb_progress") {
			return make_element_by_type<justifying_cb_progress>(state, id);
		} else if(name == "cb_progress_text") {
			return make_element_by_type<cb_progress_text>(state, id);
		} else if(name == "attackers") {
			auto ptr = make_element_by_type<justifying_attacker_flag>(state, id);
			ptr->base_data.position.y -= 7; // Nudge
			return ptr;
		} else if(name == "defenders") {
			auto ptr = make_element_by_type<justifying_defender_flag>(state, id);
			ptr->base_data.position.y -= 7; // Nudge
			return ptr;
		} else if(name == "cancel") {
			return make_element_by_type<diplomacy_casus_belli_cancel_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_casus_belli_listbox : public listbox_element_base<diplomacy_casus_belli_entry, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "diplomacy_cb_info_player";
	}

public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.x += int16_t(400);
		listbox_element_base<diplomacy_casus_belli_entry, dcon::nation_id>::on_create(state);
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		// Put player wargoals first
		for(const auto id : state.world.in_nation) {
			if(id == state.local_player_nation && dcon::fatten(state.world, state.local_player_nation).get_constructing_cb_type().is_valid()) {
				row_contents.push_back(id);
			}
		}
		// And after - everyone elses
		for(const auto id : state.world.in_nation) {
			if(id != state.local_player_nation && dcon::fatten(state.world, id).get_constructing_cb_is_discovered()) {
				row_contents.push_back(id);
			}
		}
		update(state);
	}
};

class diplomacy_casus_belli_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "cb_listbox") {
			auto ptr = make_element_by_type<diplomacy_casus_belli_listbox>(state, id);
			ptr->base_data.position.x -= 400; // Nudge
			return ptr;
		} else {
			return nullptr;
		}
	}
};

class diplomacy_war_listbox : public listbox_element_base<diplomacy_war_info, dcon::war_id> {
protected:
	std::string_view get_row_element_name() override {
		return "diplomacy_war_info";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_war([&](dcon::war_id id) { row_contents.push_back(id); });
		std::sort(row_contents.begin(), row_contents.end(), [&](dcon::war_id a, dcon::war_id b) {
			auto in_a = military::get_role(state, a, state.local_player_nation) != military::war_role::none;
			auto in_b = military::get_role(state, b, state.local_player_nation) != military::war_role::none;
			if(in_a != in_b) {
				return in_a;
			} else {
				return state.world.war_get_start_date(a) < state.world.war_get_start_date(b);
			}
		});
		update(state);
	}
};

class diplomacy_greatpower_info : public window_element_base {
public:
	uint8_t rank = 0;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "country_puppets") {
			auto ptr = make_element_by_type<overlapping_sphere_flags>(state, id);
			ptr->base_data.position.y -= 8; // Nudge
			return ptr;
		} else if(name == "gp_prestige") {
			return make_element_by_type<nation_prestige_text>(state, id);
		} else if(name == "gp_economic") {
			return make_element_by_type<nation_industry_score_text>(state, id);
		} else if(name == "gp_military") {
			return make_element_by_type<nation_military_score_text>(state, id);
		} else if(name == "gp_total") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(nations::get_nth_great_power(state, rank));
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}

	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = nations::get_nth_great_power(state, rank);
		impl_set(state, payload);
	}
};

class diplomacy_sort_nation_gp_flag : public nation_gp_flag {
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == tooltip_behavior::no_tooltip)
			return message_result::unseen;
		return type == mouse_probe_type::tooltip ? message_result::consumed : message_result::unseen;
	}
	void button_action(sys::state& state) noexcept override { }
};

class crisis_tab_button : public checkbox_button {
public:
	bool is_active(sys::state& state) noexcept final {
		return parent && static_cast<generic_tabbed_window<diplomacy_window_tab>*>(parent)->active_tab == diplomacy_window_tab::crisis;
	}

	void button_action(sys::state& state) noexcept final {
		diplomacy_window_tab target = diplomacy_window_tab::crisis;
		send(state, parent, target);
	}

	void on_update(sys::state& state) noexcept override {
		checkbox_button::on_update(state);
		disabled = state.current_crisis == sys::crisis_type::none;
	}
};

class gp_tab_text : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		set_text(state, text::produce_simple_string(state, "tut_8c_3"));
	}
};

template<country_list_filter category>
class category_filter_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept final {
		send(state, parent, category);
		if constexpr(category == country_list_filter::all) {
			send(state, parent, dcon::modifier_id{});
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto filter_settings = retrieve<country_filter_setting>(state, parent);
		disabled = filter_settings.general_category != category;
		button_element_base::render(state, x, y);
		disabled = false;
	}
};

class continent_filter_button : public button_element_base {
public:
	dcon::modifier_id continent;

	void button_action(sys::state& state) noexcept final {
		send(state, parent, continent);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto filter_settings = retrieve<country_filter_setting>(state, parent);
		disabled = filter_settings.continent != continent;
		button_element_base::render(state, x, y);
		disabled = false;
	}
};

class diplomacy_window : public generic_tabbed_window<diplomacy_window_tab> {
private:
	diplomacy_country_listbox* country_listbox = nullptr;
	diplomacy_war_listbox* war_listbox = nullptr;
	diplomacy_country_facts* country_facts = nullptr;
	diplomacy_action_dialog_window* action_dialog_win = nullptr;
	diplomacy_gp_action_dialog_window* gp_action_dialog_win = nullptr;
	diplomacy_declare_war_dialog* declare_war_win = nullptr;
	offer_war_goal_dialog* offer_goal_win = nullptr;
	diplomacy_setup_peace_dialog* setup_peace_win = nullptr;
	diplomacy_make_cb_window* make_cb_win = nullptr;
	crisis_resolution_dialog* crisis_backdown_win = nullptr;
	diplomacy_casus_belli_window* casus_belli_window = nullptr;
	// element_base* casus_belli_window = nullptr;
	diplomacy_crisis_info_window* crisis_window = nullptr;

	std::vector<diplomacy_greatpower_info*> gp_infos{};
	std::vector<element_base*> action_buttons{};

	dcon::nation_id facts_nation_id{};

	country_filter_setting filter = country_filter_setting{};
	country_sort_setting sort = country_sort_setting{};

	template<typename T>
	void add_action_button(sys::state& state, xy_pair offset) noexcept {
		auto ptr = make_element_by_type<T>(state, state.ui_state.defs_by_name.find("diplomacy_option")->second.definition);
		ptr->base_data.position = offset;
		action_buttons.push_back(ptr.get());
		add_child_to_front(std::move(ptr));
	}

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		set_visible(state, false);
		state.ui_state.diplomacy_subwindow = this;

		xy_pair base_gp_info_offset =
				state.ui_defs.gui[state.ui_state.defs_by_name.find("diplomacy_greatpower_pos")->second.definition].position;
		xy_pair gp_info_offset = base_gp_info_offset;
		for(uint8_t i = 0; i < uint8_t(state.defines.great_nations_count); i++) {
			auto ptr = make_element_by_type<diplomacy_greatpower_info>(state,
					state.ui_state.defs_by_name.find("diplomacy_greatpower_info")->second.definition);
			ptr->base_data.position = gp_info_offset;
			ptr->rank = i;
			// Increment gp offset
			gp_info_offset.y += ptr->base_data.size.y;
			if(i + 1 == uint8_t(state.defines.great_nations_count) / 2) {
				gp_info_offset.y = base_gp_info_offset.y;
				gp_info_offset.x += (ptr->base_data.size.x / 2) + 62; // Nudge
			}
			gp_infos.push_back(ptr.get());
			add_child_to_front(std::move(ptr));
		}

		// Fill out all the options for the diplomacy window
		xy_pair options_base_offset =
				state.ui_defs.gui[state.ui_state.defs_by_name.find("diplomacy_actions_pos")->second.definition].position;
		xy_pair options_size = state.ui_defs.gui[state.ui_state.defs_by_name.find("diplomacy_option")->second.definition].size;
		xy_pair options_offset = options_base_offset;
		add_action_button<diplomacy_action_window<diplomacy_action_ally_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_call_ally_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_military_access_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_give_military_access_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_increase_relations_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_decrease_relations_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_war_subisides_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_declare_war_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_release_subject_button>>(state, options_offset);
		// Next row of actions...
		options_offset.x += options_size.x;
		options_offset.y = options_base_offset.y;
		add_action_button<diplomacy_action_window<diplomacy_action_discredit_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_expel_advisors_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_ban_embassy_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_increase_opinion_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_decrease_opinion_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_add_to_sphere_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_remove_from_sphere_button>>(state, options_offset);
		options_offset.y += options_size.y;
		add_action_button<diplomacy_action_window<diplomacy_action_justify_war_button>>(state, options_offset);

		auto new_win1 = make_element_by_type<diplomacy_action_dialog_window>(state,
				state.ui_state.defs_by_name.find("defaultdiplomacydialog")->second.definition);
		new_win1->set_visible(state, false);
		action_dialog_win = new_win1.get();
		add_child_to_front(std::move(new_win1));

		auto new_win2 = make_element_by_type<diplomacy_gp_action_dialog_window>(state,
				state.ui_state.defs_by_name.find("gpselectdiplomacydialog")->second.definition);
		new_win2->set_visible(state, false);
		gp_action_dialog_win = new_win2.get();
		add_child_to_front(std::move(new_win2));

		auto new_win3 = make_element_by_type<diplomacy_declare_war_dialog>(state,
				state.ui_state.defs_by_name.find("declarewardialog")->second.definition);
		new_win3->set_visible(state, false);
		declare_war_win = new_win3.get();
		add_child_to_front(std::move(new_win3));

		{
			auto new_winc = make_element_by_type<offer_war_goal_dialog>(state,
				state.ui_state.defs_by_name.find("declarewardialog")->second.definition);
			new_winc->set_visible(state, false);
			offer_goal_win = new_winc.get();
			add_child_to_front(std::move(new_winc));
		}

		auto new_win4 = make_element_by_type<diplomacy_setup_peace_dialog>(state,
				state.ui_state.defs_by_name.find("setuppeacedialog")->second.definition);
		new_win4->set_visible(state, false);
		setup_peace_win = new_win4.get();
		add_child_to_front(std::move(new_win4));

		auto new_win5 = make_element_by_type<diplomacy_make_cb_window>(state,
				state.ui_state.defs_by_name.find("makecbdialog")->second.definition);
		new_win5->set_visible(state, false);
		make_cb_win = new_win5.get();
		add_child_to_front(std::move(new_win5));

		auto new_win6 = make_element_by_type<crisis_resolution_dialog>(state,
				state.ui_state.defs_by_name.find("setuppeacedialog")->second.definition);
		new_win6->set_visible(state, false);
		crisis_backdown_win = new_win6.get();
		add_child_to_front(std::move(new_win6));

		Cyto::Any payload = element_selection_wrapper<dcon::nation_id>{ state.local_player_nation };
		impl_get(state, payload);
	}

	void on_update(sys::state& state) noexcept override {
		if(active_tab == diplomacy_window_tab::crisis && state.current_crisis == sys::crisis_type::none) {
			send<diplomacy_window_tab>(state, this, diplomacy_window_tab::great_powers);
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "gp_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::great_powers;
			return ptr;
		} else if(name == "gp_info_text") {
			return make_element_by_type<gp_tab_text>(state, id);
		} else if(name == "war_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::wars;
			return ptr;
		} else if(name == "cb_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::casus_belli;
			return ptr;
		} else if(name == "crisis_info") {
			return make_element_by_type<crisis_tab_button>(state, id);
		} else if(name == "filter_all") {
			auto ptr = make_element_by_type<category_filter_button<country_list_filter::all>>(state, id);
			return ptr;
		} else if(name == "filter_enemies") {
			auto ptr = make_element_by_type<category_filter_button<country_list_filter::enemies>>(state, id);
			return ptr;
		} else if(name == "filter_allies") {
			auto ptr = make_element_by_type<category_filter_button<country_list_filter::allies>>(state, id);
			return ptr;
		} else if(name == "filter_neighbours") {
			auto ptr = make_element_by_type<category_filter_button<country_list_filter::neighbors>>(state, id);
			return ptr;
		} else if(name == "filter_sphere") {
			auto ptr = make_element_by_type<category_filter_button<country_list_filter::sphere>>(state, id);
			return ptr;
		} else if(name == "cb_info_win") {
			auto ptr = make_element_by_type<diplomacy_casus_belli_window>(state, id);
			// auto ptr = make_element_immediate(state, id);
			casus_belli_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "crisis_info_win") {
			auto ptr = make_element_by_type<diplomacy_crisis_info_window>(state, id);
			crisis_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "country_listbox") {
			auto ptr = make_element_by_type<diplomacy_country_listbox>(state, id);
			country_listbox = ptr.get();
			return ptr;
		} else if(name == "war_listbox") {
			auto ptr = make_element_by_type<diplomacy_war_listbox>(state, id);
			war_listbox = ptr.get();
			war_listbox->set_visible(state, false);
			return ptr;
		} else if(name == "diplomacy_country_facts") {
			auto ptr = make_element_by_type<diplomacy_country_facts>(state, id);
			country_facts = ptr.get();
			return ptr;
		} else if(name == "sort_by_boss") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::boss>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_prestige") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::prestige_rank>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_economic") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::economic_rank>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_military") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::military_rank>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_total") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::total_rank>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_relation") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::relation>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_opinion") {
			return make_element_by_type<country_sort_button<country_list_sort::opinion>>(state, id);
		} else if(name == "sort_by_prio") {
			return make_element_by_type<country_sort_button<country_list_sort::priority>>(state, id);
		} else if(name.substr(0, 14) == "sort_by_gpflag") {
			auto ptr = make_element_by_type<diplomacy_sort_nation_gp_flag>(state, id);
			ptr->rank = uint16_t(std::stoi(std::string{name.substr(14)}));
			ptr->base_data.position.y -= 2; // Nudge
			return ptr;
		} else if(name.substr(0, 10) == "sort_by_gp") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::gp_influence>>(state, id);
			ptr->offset = uint8_t(std::stoi(std::string{name.substr(10)}));
			ptr->base_data.position.y -= 2; // Nudge
			return ptr;
		} else if(name.length() >= 7 && name.substr(0, 7) == "filter_") {
			auto const filter_name = name.substr(7);
			auto ptr = make_element_by_type<continent_filter_button>(state, id);
			if(auto it = state.key_to_text_sequence.find(filter_name); it != state.key_to_text_sequence.end()) {
				for(auto m : state.world.in_modifier) {
					if(m.get_name() == it->second) {
						ptr->continent = m;
						break;
					}
				}
			}
			return ptr;
		} else {
			return nullptr;
		}
	}

	void hide_tabs(sys::state& state) {
		war_listbox->set_visible(state, false);
		casus_belli_window->set_visible(state, false);
		crisis_window->set_visible(state, false);
		for(auto e : gp_infos)
			e->set_visible(state, false);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<diplomacy_window_tab>()) {
			auto enum_val = any_cast<diplomacy_window_tab>(payload);
			hide_tabs(state);
			switch(enum_val) {
				case diplomacy_window_tab::great_powers:
					for(auto e : gp_infos)
						e->set_visible(state, true);
					break;
				case diplomacy_window_tab::wars:
					war_listbox->set_visible(state, true);
					break;
				case diplomacy_window_tab::casus_belli:
					casus_belli_window->set_visible(state, true);
					break;
				case diplomacy_window_tab::crisis:
					crisis_window->set_visible(state, true);
					break;
			}
			active_tab = enum_val;
			country_facts->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dip_tab_request>()) {
			payload.emplace<dip_tab_request>(dip_tab_request{ active_tab });
			return message_result::consumed;
		} else if(payload.holds_type<country_sort_setting>()) {
			payload.emplace<country_sort_setting>(sort);
			return message_result::consumed;
		} else if(payload.holds_type< country_filter_setting>()) {
			payload.emplace<country_filter_setting>(filter);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<country_list_sort>>()) {
			auto new_sort = any_cast<element_selection_wrapper<country_list_sort>>(payload).data;
			sort.sort_ascend = (new_sort == sort.sort) ? !sort.sort_ascend : true;
			sort.sort = new_sort;
			country_listbox->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<country_list_filter>()) {
			auto temp = any_cast<country_list_filter>(payload);
			filter.general_category = filter.general_category != temp ? temp : country_list_filter::all;
			country_listbox->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::modifier_id>()) {
			auto temp_c = any_cast<dcon::modifier_id>(payload);
			filter.continent = filter.continent == temp_c ? dcon::modifier_id{} : temp_c;
			country_listbox->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(facts_nation_id);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::nation_id>>()) {
			facts_nation_id = any_cast<element_selection_wrapper<dcon::nation_id>>(payload).data;
			send(state, country_listbox, dip_make_nation_visible{ facts_nation_id });
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<open_offer_window>()) {
			auto offer_to = any_cast<open_offer_window>(payload).to;

			offer_goal_win->set_visible(state, false);
			offer_goal_win->reset_window(state, offer_to);
			offer_goal_win->set_visible(state, true);

		} else if(payload.holds_type<trigger_gp_choice>()) {
			auto action = any_cast<trigger_gp_choice>(payload).action;

			action_dialog_win->set_visible(state, false);
			declare_war_win->set_visible(state, false);
			setup_peace_win->set_visible(state, false);
			make_cb_win->set_visible(state, false);
			crisis_backdown_win->set_visible(state, false);

			gp_action_dialog_win->set_visible(state, false);
			gp_action_dialog_win->action_target = facts_nation_id;
			gp_action_dialog_win->current_action = action;
			gp_action_dialog_win->set_visible(state, true); // this will also force an update

		} else if(payload.holds_type<diplomacy_action>()) {
			auto v = any_cast<diplomacy_action>(payload);
			gp_action_dialog_win->set_visible(state, false);
			action_dialog_win->set_visible(state, false);
			declare_war_win->set_visible(state, false);
			setup_peace_win->set_visible(state, false);
			make_cb_win->set_visible(state, false);
			crisis_backdown_win->set_visible(state, false);
			Cyto::Any new_payload = facts_nation_id;
			auto fat = dcon::fatten(state.world, facts_nation_id);
			switch(v) {
			case diplomacy_action::add_to_sphere:
				command::add_to_sphere(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::military_access:
				command::ask_for_military_access(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::cancel_military_access:
				command::cancel_military_access(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::give_military_access:
				// TODO: Give military access
				break;
			case diplomacy_action::cancel_give_military_access:
				command::cancel_given_military_access(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::increase_relations:
				command::increase_relations(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::decrease_relations:
				command::decrease_relations(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::war_subsidies:
				command::give_war_subsidies(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::cancel_war_subsidies:
				command::cancel_war_subsidies(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::ally:
				command::ask_for_alliance(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::cancel_ally:
				command::cancel_alliance(state, state.local_player_nation, facts_nation_id);
				break;
			case diplomacy_action::call_ally:
				for(auto war_par : fat.get_war_participant()) {
					command::call_to_arms(state, state.local_player_nation, facts_nation_id,
							dcon::fatten(state.world, war_par).get_war().id);
				}
				break;
			case diplomacy_action::remove_from_sphere:
				gp_action_dialog_win->set_visible(state, true);
				gp_action_dialog_win->impl_set(state, new_payload);
				gp_action_dialog_win->impl_set(state, payload);
				gp_action_dialog_win->impl_on_update(state);
				break;
			case diplomacy_action::declare_war:
			case diplomacy_action::add_wargoal:
				declare_war_win->set_visible(state, false);
				declare_war_win->reset_window(state);
				declare_war_win->set_visible(state, true);
				
				break;
			case diplomacy_action::make_peace:
				setup_peace_win->open_window(state);
				setup_peace_win->set_visible(state, true);
				setup_peace_win->impl_set(state, new_payload);
				setup_peace_win->impl_set(state, payload);
				setup_peace_win->impl_on_update(state);
				break;
			case diplomacy_action::justify_war:
				make_cb_win->impl_set(state, new_payload);
				make_cb_win->impl_set(state, payload);
				make_cb_win->set_visible(state, true);
				break;
			case diplomacy_action::crisis_backdown:
				crisis_backdown_win->open_window(state);
				crisis_backdown_win->set_visible(state, true);
				crisis_backdown_win->impl_on_update(state);
				break;
			case diplomacy_action::crisis_support:
				break;
			default:
				action_dialog_win->set_visible(state, true);
				action_dialog_win->impl_set(state, new_payload);
				action_dialog_win->impl_set(state, payload);
				action_dialog_win->impl_on_update(state);
				break;
			}
			return message_result::consumed;
		}
		return generic_tabbed_window<diplomacy_window_tab>::get(state, payload);
	}
};

} // namespace ui
