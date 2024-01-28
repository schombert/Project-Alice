#include "gui_population_window.hpp"
#include "demographics.hpp"
#include "gui_modifier_tooltips.hpp"
#include "gui_trigger_tooltips.hpp"
#include "triggers.hpp"

namespace ui {

void describe_conversion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto location = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto conversion_chances = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.conversion_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);

	float base_amount =
			state.defines.conversion_scale *
			(state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::conversion_rate) + 1.0f) *
			(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_conversion_rate) + 1.0f) *
			conversion_chances;

	auto pr = state.world.pop_get_religion(ids);
	auto state_religion = state.world.nation_get_religion(owner);

	// pops of the state religion do not convert
	if(state_religion == pr)
		base_amount = 0.0f;

	// need at least 1 pop following the religion in the province
	if(state.world.province_get_demographics(location, demographics::to_key(state, state_religion.id)) < 1.f)
		base_amount = 0.0f;

	text::add_line(state, contents, "pop_conver_1", text::variable_type::x, int64_t(std::max(0.0f, state.world.pop_get_size(ids) * base_amount)));
	text::add_line_break_to_layout(state, contents);

	if(state_religion == pr) {
		text::add_line(state, contents, "pop_conver_2");
		return;
	}
	// need at least 1 pop following the religion in the province
	if(state.world.province_get_demographics(location, demographics::to_key(state, state_religion.id)) < 1.f) {
		text::add_line(state, contents, "pop_conver_3");
		return; // early exit
	}
	text::add_line(state, contents, "pop_conver_4");
	text::add_line(state, contents, "pop_conver_5", text::variable_type::x, text::fp_three_places{state.defines.conversion_scale});
	text::add_line(state, contents, "pop_conver_6", text::variable_type::x, text::fp_two_places{ std::max(0.0f,state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_conversion_rate) + 1.0f)});
	active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_conversion_rate, true);
	text::add_line(state, contents, "pop_conver_7", text::variable_type::x, text::fp_two_places{ std::max(0.0f, state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::conversion_rate) + 1.0f)});
	active_modifiers_description(state, contents, location, 15, sys::provincial_mod_offsets::conversion_rate, true);

	text::add_line(state, contents, "pop_conver_8", text::variable_type::x, text::fp_four_places{conversion_chances});
	additive_value_modifier_description(state, contents, state.culture_definitions.conversion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}

void describe_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);


	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_mig_1");
		return;
	}
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_mig_2");
		return;
	}

	auto owners = state.world.province_get_nation_from_province_ownership(loc);
	auto migration_chance = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.migration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto prov_mod = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto scale = state.defines.immigration_scale;

	text::add_line(state, contents, "pop_mig_3", text::variable_type::x, text::fp_three_places{scale}, text::variable_type::y,
			text::fp_percentage{prov_mod}, text::variable_type::val, text::fp_two_places{migration_chance});

	active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);

	text::add_line(state, contents, "pop_mig_4");
	additive_value_modifier_description(state, contents, state.culture_definitions.migration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0);
}

void describe_colonial_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);


	if(state.world.nation_get_is_colonial_nation(owner) == false) {
		text::add_line(state, contents, "pop_cmig_1");
		return;
	}
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_cmig_2");
		return;
	}
	auto pt = state.world.pop_get_poptype(ids);
	if(pt == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_cmig_3");
		return;
	}
	if(state.world.pop_type_get_strata(pt) == uint8_t(culture::pop_strata::rich)) {
		text::add_line(state, contents, "pop_cmig_4");
		return;
	}
	if(pt == state.culture_definitions.primary_factory_worker || pt == state.culture_definitions.secondary_factory_worker) {
		text::add_line(state, contents, "pop_cmig_5");
		return;
	}

	auto mig_chance = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.colonialmigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0),  0.0f);
	auto im_push = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto cmig = (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::colonial_migration) + 1.0f);
	auto scale = state.defines.immigration_scale;

	text::add_line(state, contents, "pop_cmig_6", text::variable_type::x, text::fp_three_places{scale}, text::variable_type::y,
			text::fp_percentage{im_push}, text::variable_type::num, text::fp_percentage{cmig}, text::variable_type::val,
			text::fp_two_places{mig_chance});

	active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);
	active_modifiers_description(state, contents, owner, 0, sys::national_mod_offsets::colonial_migration, true);

	text::add_line(state, contents, "pop_cmig_7");
	additive_value_modifier_description(state, contents, state.culture_definitions.colonialmigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0);
}

void describe_emigration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owners = state.world.province_get_nation_from_province_ownership(loc);

	if(state.world.nation_get_is_civilized(owners) == false) {
		text::add_line(state, contents, "pop_emg_1");
		return;
	}
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_emg_2");
		return;
	}
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_emg_3");
		return;
	}
	if(state.world.culture_group_get_is_overseas(
				 state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(ids))) == false) {
		text::add_line(state, contents, "pop_emg_4");
		return;
	}

	auto impush = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto emig = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.emigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto scale =  state.defines.immigration_scale;

	text::add_line(state, contents, "pop_emg_5", text::variable_type::x, text::fp_three_places{scale}, text::variable_type::y,
			text::fp_percentage{impush}, text::variable_type::val, text::fp_two_places{emig});

	active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);

	text::add_line(state, contents, "pop_emg_6");
	additive_value_modifier_description(state, contents, state.culture_definitions.emigration_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}

void describe_promotion_demotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {

	auto owner = nations::owner_of_pop(state, ids);
	auto promotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.promotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);
	auto demotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.demotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(ptype);

	text::add_line(state, contents, "pop_prom_1");
	if(promoted_type) {
		if(promoted_type == ptype) {
			text::add_line(state, contents, "pop_prom_3");
		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {
			text::add_line(state, contents, "pop_prom_2", text::variable_type::val, text::fp_two_places{promotion_bonus});
			promotion_chance += promotion_bonus;
		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			demotion_chance += promotion_bonus;
		}
	}
	additive_value_modifier_description(state, contents, state.culture_definitions.promotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "pop_prom_4");
	if(promoted_type) {
		if(promoted_type == ptype) {

		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {

		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			text::add_line(state, contents, "pop_prom_2", text::variable_type::val, text::fp_two_places{promotion_bonus});
		}
	}
	additive_value_modifier_description(state, contents, state.culture_definitions.demotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	if(promotion_chance <= 0.0f && demotion_chance <= 0.0f) {
		text::add_line(state, contents, "pop_prom_7");
		return;
	}

	bool promoting = promotion_chance >= demotion_chance;
	if(promoting) {
		text::add_line(state, contents, "pop_prom_5");
		text::add_line(state, contents, "pop_prom_8", text::variable_type::x, text::fp_three_places{state.defines.promotion_scale},
				text::variable_type::val, text::fp_two_places{promotion_chance}, text::variable_type::y,
				text::fp_percentage{state.world.nation_get_administrative_efficiency(owner)});
	} else {
		text::add_line(state, contents, "pop_prom_6");
		text::add_line(state, contents, "pop_prom_9", text::variable_type::x, text::fp_three_places{state.defines.promotion_scale},
				text::variable_type::val, text::fp_two_places{demotion_chance});
	}
}

void describe_con(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	float cfrac =
			state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);
	auto types = state.world.pop_get_poptype(ids);

	float lx_mod = state.world.pop_get_luxury_needs_satisfaction(ids) * state.defines.con_luxury_goods;
	float cl_mod = cfrac * (state.world.pop_type_get_strata(types) == int32_t(culture::pop_strata::poor) ?
														state.defines.con_poor_clergy : state.defines.con_midrich_clergy);
	float lit_mod = ((state.world.nation_get_plurality(owner) / 10.0f) *
								 (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::literacy_con_impact) + 1.0f) *
								 state.defines.con_literacy * state.world.pop_get_literacy(ids) *
								 (state.world.province_get_is_colonial(loc) ? state.defines.con_colonial_factor : 1.0f)) / 10.f;

	float pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_consciousness_modifier);
	float omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_consciousness_modifier);
	float cmod = (state.world.province_get_is_colonial(loc) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_consciousness_modifier));

	float local_mod = (pmod + omod) + cmod;

	float sep_mod = (state.world.pop_get_is_primary_or_accepted_culture(ids) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier));
	auto old_con = state.world.pop_get_consciousness(ids) * 0.01f;
	auto total = (lx_mod + (cl_mod + lit_mod - old_con)) + (local_mod + sep_mod);

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{total}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{total}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "pop_con_2");
	text::add_line(state, contents, "pop_con_3", text::variable_type::x, text::fp_two_places{lx_mod});
	text::add_line(state, contents, "pop_con_4", text::variable_type::val, text::fp_percentage{cfrac}, text::variable_type::x,
			text::fp_two_places{cl_mod});
	text::add_line(state, contents, "pop_con_5", text::variable_type::x, text::fp_two_places{lit_mod});

	text::add_line(state, contents, "pop_con_6", text::variable_type::x,
			text::fp_two_places{state.world.nation_get_plurality(owner)}, 15);
	text::add_line(state, contents, "pop_con_7", text::variable_type::x,
			text::fp_percentage{state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::literacy_con_impact) + 1.0f},
			15);
	active_modifiers_description(state, contents, owner, 30, sys::national_mod_offsets::literacy_con_impact, false);
	text::add_line(state, contents, "pop_con_8", text::variable_type::x, text::fp_two_places{state.defines.con_literacy}, 15);
	text::add_line(state, contents, "pop_con_9", text::variable_type::x, text::fp_percentage{state.world.pop_get_literacy(ids)},
			15);
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_con_10", text::variable_type::x, text::fp_two_places{state.defines.con_colonial_factor},
				15);
	}
	text::add_line(state, contents, "pop_con_11", 15);
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_12");
		if(pmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{pmod}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{pmod}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, loc, 15, sys::provincial_mod_offsets::pop_consciousness_modifier, false);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_13");
		if(omod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{omod}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{omod}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_pop_consciousness_modifier, false);
	}
	if(!state.world.province_get_is_colonial(loc)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_14");
		if(cmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{cmod}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{cmod}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::core_pop_consciousness_modifier, false);
	}
	if(!state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_15");
		if(sep_mod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{sep_mod}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{sep_mod}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier,
				false);
	}
	text::add_line(state, contents, "alice_con_decay_description", text::variable_type::x, text::fp_three_places{ state.world.pop_get_consciousness(ids) * 0.01f });
}

void describe_mil(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto const conservatism_key = pop_demographics::to_key(state, state.culture_definitions.conservative);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	auto ruling_party = state.world.nation_get_ruling_party(owner);
	auto ruling_ideology = state.world.political_party_get_ideology(ruling_party);

	float lx_mod = std::max(state.world.pop_get_luxury_needs_satisfaction(ids) - 0.5f, 0.0f) * state.defines.mil_has_luxury_need;
	float con_sup = (state.world.pop_get_demographics(ids, conservatism_key) * state.defines.mil_ideology);
	float ruling_sup = ruling_ideology ? state.world.pop_get_demographics(ids, pop_demographics::to_key(state, ruling_ideology)) *
																					 state.defines.mil_ruling_party
																		 : 0.0f;
	float ref_mod = state.world.province_get_is_colonial(loc)
											? 0.0f
											: (state.world.pop_get_social_reform_desire(ids) + state.world.pop_get_political_reform_desire(ids)) *
														(state.defines.mil_require_reform * 0.25f);

	float sub_t = (lx_mod + ruling_sup) + (con_sup + ref_mod);

	float pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_militancy_modifier);
	float omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_militancy_modifier);
	float cmod = (state.world.province_get_is_colonial(loc) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_militancy_modifier));

	float local_mod = (pmod + omod) + cmod;

	float sep_mod = (state.world.pop_get_is_primary_or_accepted_culture(ids) ? 0.0f :
			(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::seperatism) + 1.0f) *
					state.defines.mil_non_accepted);
	float ln_mod = std::min((state.world.pop_get_life_needs_satisfaction(ids) - 0.5f), 0.0f) * state.defines.mil_no_life_need;
	float en_mod_a =
			std::min(0.0f, (state.world.pop_get_everyday_needs_satisfaction(ids) - 0.5f)) * state.defines.mil_lack_everyday_need;
	float en_mod_b =
			std::max(0.0f, (state.world.pop_get_everyday_needs_satisfaction(ids) - 0.5f)) * state.defines.mil_has_everyday_need;
	//Ranges from +0.00 - +0.50 militancy monthly, 0 - 100 war exhaustion
	float war_exhaustion =
		state.world.nation_get_war_exhaustion(owner) * 0.005f;
	auto old_mil = state.world.pop_get_militancy(ids) * 0.01f;
	float total = (sub_t + local_mod) + ((sep_mod - ln_mod - old_mil) + (en_mod_b - en_mod_a) + war_exhaustion);

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::red);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{total}, text::text_color::red);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{total}, text::text_color::green);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "pop_mil_2");
	text::add_line(state, contents, "pop_mil_3", text::variable_type::x, text::fp_two_places{ln_mod < 0.0f ? -ln_mod : 0.0f});
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_4");
		if(en_mod_b - en_mod_a >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::red);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{en_mod_b - en_mod_a}, text::text_color::red);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{en_mod_b - en_mod_a}, text::text_color::green);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line(state, contents, "pop_mil_5", text::variable_type::x, text::fp_two_places{lx_mod});
	text::add_line(state, contents, "pop_mil_6", text::variable_type::x, text::fp_two_places{con_sup});
	text::add_line(state, contents, "pop_mil_7", text::variable_type::x, text::fp_two_places{ruling_sup});
	text::add_line(state, contents, "pop_mil_8", text::variable_type::x, text::fp_two_places{ref_mod});
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_9");
		if(pmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{pmod}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{pmod}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, loc, 15, sys::provincial_mod_offsets::pop_militancy_modifier, false);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_10");
		if(omod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{omod}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{omod}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_pop_militancy_modifier, false);
	}
	if(!state.world.province_get_is_colonial(loc)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_11");
		if(cmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{cmod}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{cmod}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::core_pop_militancy_modifier, false);
	}
	if(!state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		text::add_line(state, contents, "pop_mil_12",
			text::variable_type::val, text::fp_two_places{sep_mod},
			text::variable_type::x, text::fp_two_places{state.defines.mil_non_accepted},
			text::variable_type::y, text::fp_percentage{state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::seperatism) + 1.0f});
		active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::seperatism, false);
	}
	if(war_exhaustion > 0) {
		text::add_line(state, contents, "pop_mil_13", text::variable_type::val, text::fp_three_places{war_exhaustion});
	}
	text::add_line(state, contents, "alice_mil_decay_description", text::variable_type::x, text::fp_three_places{ state.world.pop_get_militancy(ids) * 0.01f });
}

void describe_lit(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	auto cfrac =
			state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);

	auto tmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency) + 1.0f;
	auto nmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency_modifier) + 1.0f;
	auto espending =
			(float(state.world.nation_get_education_spending(owner)) / 100.0f) * state.world.nation_get_spending_level(owner);
	auto cmod = std::max(0.0f, std::min(1.0f, (cfrac - state.defines.base_clergy_for_literacy) /
																 (state.defines.max_clergy_for_literacy - state.defines.base_clergy_for_literacy)));

	float total = (0.01f * state.defines.literacy_change_speed) * ((espending * cmod) * (tmod * nmod));

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_lit_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_three_places{total}, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_three_places{total}, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "pop_lit_2");
	text::add_line(state, contents, "pop_lit_3", text::variable_type::val, text::fp_percentage{cfrac}, text::variable_type::x,
			text::fp_two_places{cmod});
	text::add_line(state, contents, "pop_lit_4", text::variable_type::x, text::fp_two_places{espending});
	text::add_line(state, contents, "pop_lit_5", text::variable_type::x, text::fp_percentage{tmod});
	active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::education_efficiency, false);
	text::add_line(state, contents, "pop_lit_6", text::variable_type::x, text::fp_percentage{nmod});
	active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::education_efficiency_modifier, false);
	text::add_line(state, contents, "pop_lit_7", text::variable_type::x, text::fp_two_places{state.defines.literacy_change_speed});
	text::add_line(state, contents, "pop_lit_8");
}

void describe_growth(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto type = state.world.pop_get_poptype(ids);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);

	auto base_life_rating = float(state.world.province_get_life_rating(loc));
	auto mod_life_rating = std::min(
			base_life_rating * (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::life_rating) + 1.0f), 40.0f);
	auto lr_factor =
			std::max((mod_life_rating - state.defines.min_life_rating_for_growth) * state.defines.life_rating_growth_bonus, 0.0f);
	auto province_factor = lr_factor + state.defines.base_popgrowth;

	auto ln_factor = state.world.pop_get_life_needs_satisfaction(ids) - state.defines.life_need_starvation_limit;
	auto mod_sum = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::population_growth) +
								 state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pop_growth);

	auto total_factor = ln_factor * province_factor * 4.0f + mod_sum * 0.1f;

	if(type == state.culture_definitions.slaves)
		total_factor = 0.0f;

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_growth_1");
		if(total_factor >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
			text::add_to_layout_box(state, contents, box, int64_t(total_factor * state.world.pop_get_size(ids)), text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, int64_t(total_factor * state.world.pop_get_size(ids)),
					text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);

	if(type == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_growth_2");
		return;
	}
	text::add_line(state, contents, "pop_growth_3");
	text::add_line(state, contents, "pop_growth_4", text::variable_type::x, text::fp_three_places{ln_factor * province_factor * 4.0f});
	text::add_line(state, contents, "pop_growth_5", text::variable_type::x, text::fp_four_places{province_factor}, 15);
	text::add_line(state, contents, "pop_growth_6", text::variable_type::x, text::fp_one_place{mod_life_rating},
			text::variable_type::y, text::fp_one_place{state.defines.min_life_rating_for_growth}, text::variable_type::val,
			text::fp_four_places{state.defines.life_rating_growth_bonus},
			30);
	text::add_line(state, contents, "pop_growth_7", text::variable_type::x, text::fp_three_places{state.defines.base_popgrowth},
			30);
	text::add_line(state, contents, "pop_growth_8", text::variable_type::x, text::fp_two_places{ln_factor},
			text::variable_type::y, text::fp_two_places{state.world.pop_get_life_needs_satisfaction(ids)}, text::variable_type::val,
			text::fp_two_places{state.defines.life_need_starvation_limit},
			15);
	text::add_line(state, contents, "pop_growth_9", 15);
	text::add_line(state, contents, "pop_growth_10", text::variable_type::x,
			text::fp_three_places{state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::population_growth)});
	active_modifiers_description(state, contents, loc, 15, sys::provincial_mod_offsets::population_growth, false);
	text::add_line(state, contents, "pop_growth_11", text::variable_type::x,
			text::fp_three_places{state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pop_growth)});
	active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::pop_growth, false);
}

void describe_assimilation(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto location = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto assimilation_chances = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.assimilation_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);

	float base_amount =
			state.defines.assimilation_scale *
			(state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::assimilation_rate) + 1.0f) *
			(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_assimilation_rate) + 1.0f) *
			assimilation_chances;

	/*
	In a colonial province, assimilation numbers for pops with an *non* "overseas"-type culture group are reduced by a
	factor of 100. In a non-colonial province, assimilation numbers for pops with an *non* "overseas"-type culture
	group are reduced by a factor of 10.
	*/

	auto pc = state.world.pop_get_culture(ids);
	if(!state.world.culture_group_get_is_overseas(state.world.culture_get_group_from_culture_group_membership(pc))) {
		base_amount /= 10.0f;
	}


	/*
	All pops have their assimilation numbers reduced by a factor of 100 per core in the province sharing their primary
	culture.
	*/
	for(auto core : state.world.province_get_core(location)) {
		if(core.get_identity().get_primary_culture() == pc) {
			base_amount /= 100.0f;
		}
	}

	// slaves do not assimilate
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves)
		base_amount = 0.0f;

	// pops of an accepted culture do not assimilate
	if(state.world.pop_get_is_primary_or_accepted_culture(ids))
		base_amount = 0.0f;

	// pops in an overseas and colonial province do not assimilate
	if(state.world.province_get_is_colonial(location) && province::is_overseas(state, location))
		base_amount = 0.0f;

	text::add_line(state, contents, "pop_assim_1", text::variable_type::x, int64_t(std::max(0.0f, state.world.pop_get_size(ids) * base_amount)));
	text::add_line_break_to_layout(state, contents);

	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_assim_2");
		return;
	}
	if(state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		text::add_line(state, contents, "pop_assim_3");
		return;
	}
	if(state.world.province_get_is_colonial(location)) {
		text::add_line(state, contents, "pop_assim_4");
		return;
	}
	if(province::is_overseas(state, location)) {
		text::add_line(state, contents, "pop_assim_5");
		return;
	}
	text::add_line(state, contents, "pop_assim_6");
	text::add_line(state, contents, "pop_assim_7", text::variable_type::x, text::fp_three_places{state.defines.assimilation_scale});
	text::add_line(state, contents, "pop_assim_8", text::variable_type::x, text::fp_two_places{ std::max(0.0f,state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_assimilation_rate) + 1.0f)});
	active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_assimilation_rate, true);
	text::add_line(state, contents, "pop_assim_9", text::variable_type::x, text::fp_two_places{ std::max(0.0f, state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::assimilation_rate) + 1.0f)});
	active_modifiers_description(state, contents, location, 15, sys::provincial_mod_offsets::assimilation_rate, true);

	if(!state.world.culture_group_get_is_overseas(state.world.culture_get_group_from_culture_group_membership(pc))) {
		text::add_line(state, contents, "pop_assim_10");
	}
	float core_factor = 1.0f;
	for(auto core : state.world.province_get_core(location)) {
		if(core.get_identity().get_primary_culture() == pc) {
			core_factor /= 100.0f;
		}
	}
	if(core_factor < 1.0f) {
		text::add_line(state, contents, "pop_assim_11", text::variable_type::x, text::fp_four_places{core_factor});
	}
	text::add_line(state, contents, "pop_assim_12", text::variable_type::x, text::fp_four_places{assimilation_chances});
	additive_value_modifier_description(state, contents, state.culture_definitions.assimilation_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}

std::vector<dcon::pop_id> const& get_pop_window_list(sys::state& state) {
	static const std::vector<dcon::pop_id> empty{};
	if(state.ui_state.population_subwindow)
		return static_cast<population_window*>(state.ui_state.population_subwindow)->country_pop_listbox->row_contents;
	return empty;
}

dcon::pop_id get_pop_details_pop(sys::state& state) {
	dcon::pop_id id{};
	if(state.ui_state.population_subwindow) {
		auto win = static_cast<population_window*>(state.ui_state.population_subwindow)->details_win;
		if(win) {
			Cyto::Any payload = dcon::pop_id{};
			win->impl_get(state, payload);
			id = any_cast<dcon::pop_id>(payload);
		}
	}
	return id;
}

void pop_national_focus_button::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = dcon::state_instance_id{};
		parent->impl_get(state, payload);

		auto pop_window = static_cast<population_window*>(state.ui_state.population_subwindow);
		pop_window->focus_state = any_cast<dcon::state_instance_id>(payload);
		pop_window->nf_win->set_visible(state, !pop_window->nf_win->is_visible());
		pop_window->nf_win->base_data.position = base_data.position;
		pop_window->move_child_to_front(pop_window->nf_win);
		pop_window->impl_on_update(state);
	}
}

} // namespace ui
