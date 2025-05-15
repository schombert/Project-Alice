#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "economy.hpp"
#include "economy_production.hpp"
#include "economy_government.hpp"
#include "economy_stats.hpp"
#include "gui_graphics.hpp"
#include "gui_element_types.hpp"
#include "military.hpp"
#include "nations.hpp"
#include "politics.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "triggers.hpp"
#include <unordered_map>
#include <vector>

namespace ui {

typedef std::variant< std::monostate, dcon::nation_id, dcon::state_instance_id, dcon::province_id> pop_list_filter;

enum class country_list_sort : uint8_t {
	country,
	boss,
	economic_rank,
	military_rank,
	prestige_rank,
	total_rank,
	relation,
	opinion,
	priority,
	player_investment,
	player_influence,
	factories,
	gp_influence = 0x40,
	gp_investment = 0x80
};

// Filters used on both production and diplomacy tabs for the country lists
enum class country_list_filter : uint8_t {
	all,
	neighbors,
	sphere,
	enemies,
	allies,
	find_allies, // Used only by diplo window
	neighbors_no_vassals,
	influenced,
	deselect_all, // Used only by message filter window
	best_guess, // Used only by message filter window
	continent
};

bool country_category_filter_check(sys::state& state, country_list_filter filt, dcon::nation_id a, dcon::nation_id b);
void sort_countries(sys::state& state, std::vector<dcon::nation_id>& list, country_list_sort sort, bool sort_ascend);

void open_build_foreign_factory(sys::state& state, dcon::province_id st);
void open_foreign_investment(sys::state& state, dcon::nation_id n);

std::string get_status_text(sys::state& state, dcon::nation_id nation_id);

template<country_list_sort Sort>
class country_sort_button : public button_element_base {
public:
	uint8_t offset = 0;
	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<country_list_sort>{country_list_sort(uint8_t(Sort) | offset)});
	}
};

class country_sort_by_player_investment : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<country_list_sort>{country_list_sort::player_investment});
	}
};

class button_press_notification { };

template<class T, class K>
class generic_settable_element : public T {
public:
	K content{};

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<K>()) {
			content = any_cast<K>(payload);
			T::impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

template<class T>
class generic_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		T content = retrieve<T>(state, parent);
		auto fat_id = dcon::fatten(state.world, content);
		simple_text_element_base::set_text(state, text::get_name_as_string(state, fat_id));
	}
};

template<class T>
class generic_multiline_name_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		T content = retrieve<T>(state, parent);
		auto color = multiline_text_element_base::black_text ? text::text_color::black : text::text_color::white;
		auto container = text::create_endless_layout(state, multiline_text_element_base::internal_layout,
				text::layout_parameters{0, 0, multiline_text_element_base::base_data.size.x,
						multiline_text_element_base::base_data.size.y, multiline_text_element_base::base_data.data.text.font_handle, 0, text::alignment::left, color, false});
		auto fat_id = dcon::fatten(state.world, content);
		auto box = text::open_layout_box(container);
		text::add_to_layout_box(state, container, box, fat_id.get_name(), text::substitution_map{});
		text::close_layout_box(container, box);
	}
};

class simple_multiline_text : public multiline_text_element_base {
public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents) noexcept { }

	void on_update(sys::state& state) noexcept override {
		text::alignment align = text::alignment::left;
		switch(base_data.data.text.get_alignment()) {
			case ui::alignment::right:
				align = text::alignment::right;
				break;
			case ui::alignment::centered:
				align = text::alignment::center;
				break;
			default:
				break;
		}
		auto border = base_data.data.text.border_size;

		auto color = black_text ? text::text_color::black : text::text_color::white;
		auto container = text::create_endless_layout(state,
			internal_layout,
			text::layout_parameters{
				border.x,
				border.y,
				int16_t(base_data.size.x - border.x * 2),
				int16_t(base_data.size.y - border.y * 2),
				base_data.data.text.font_handle,
				0,
				align,
				color,
				false});
		populate_layout(state, container);
	}
};

class simple_multiline_body_text : public multiline_text_element_base {
public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents) noexcept { }

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto old_handle = base_data.data.text_common.font_handle;
		if(!state.user_settings.use_classic_fonts) {
			base_data.data.text_common.font_handle &= ~(0x01 << 7);
			auto old_value = base_data.data.text_common.font_handle & 0x3F;
			base_data.data.text_common.font_handle &= ~(0x003F);
			base_data.data.text_common.font_handle |= (old_value - 2);
		}
		multiline_text_element_base::render(state, x, y);
		base_data.data.text_common.font_handle = old_handle;
	}
	void on_update(sys::state& state) noexcept override {
		text::alignment align = text::alignment::left;
		switch(base_data.data.text.get_alignment()) {
			case ui::alignment::right:
				align = text::alignment::right;
				break;
			case ui::alignment::centered:
				align = text::alignment::center;
				break;
			default:
				break;
		}
		auto border = base_data.data.text.border_size;

		auto color = black_text ? text::text_color::black : text::text_color::white;

		auto old_handle = base_data.data.text_common.font_handle;
		if(!state.user_settings.use_classic_fonts) {
			base_data.data.text_common.font_handle &= ~(0x01 << 7);
			auto old_value = base_data.data.text_common.font_handle & 0x3F;
			base_data.data.text_common.font_handle &= ~(0x003F);
			base_data.data.text_common.font_handle |= (old_value - 2);
		}
		auto container = text::create_endless_layout(state,
			internal_layout,
			text::layout_parameters{
			border.x,
				border.y,
				int16_t(base_data.size.x - border.x * 2),
				int16_t(base_data.size.y - border.y * 2),
				base_data.data.text.font_handle,
				0,
				align,
				color,
				false});
		populate_layout(state, container);
		base_data.data.text_common.font_handle = old_handle;
	}
};

template<typename T>
class generic_multiline_text : public multiline_text_element_base {
public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents, T id) noexcept { }

	void on_update(sys::state& state) noexcept override {
		text::alignment align = text::alignment::left;
		switch(base_data.data.text.get_alignment()) {
		case ui::alignment::right:
			align = text::alignment::right;
			break;
		case ui::alignment::centered:
			align = text::alignment::center;
			break;
		default:
			break;
		}
		auto border = base_data.data.text.border_size;

		auto content = retrieve<T>(state, parent);
		auto color = black_text ? text::text_color::black : text::text_color::white;
		auto container = text::create_endless_layout(state, 
			internal_layout,
			text::layout_parameters{
				border.x,
				border.y,
				int16_t(base_data.size.x - border.x * 2),
				int16_t(base_data.size.y - border.y * 2),
				base_data.data.text.font_handle,
				0,
				align,
				color,
				false});
		populate_layout(state, container, content);
	}
};

class expanded_hitbox_text : public simple_text_element_base {
public:
	ui::xy_pair bottom_right_extension{ 0,0 };
	ui::xy_pair top_left_extension{ 0,0 };

	void on_reset_text(sys::state& state) noexcept override {
		auto actual_size = base_data.size.x;
		base_data.size.x -= int16_t(top_left_extension.x + bottom_right_extension.x);
		simple_text_element_base::on_reset_text(state);
		base_data.size.x = actual_size;
	}
	void on_create(sys::state& state) noexcept override {
		if(base_data.get_element_type() == element_type::button) {
			black_text = text::is_black_from_font_id(base_data.data.button.font_handle);
		} else if(base_data.get_element_type() == element_type::text) {
			black_text = text::is_black_from_font_id(base_data.data.text.font_handle);
		}
		base_data.size.x += int16_t(top_left_extension.x + bottom_right_extension.x);
		base_data.size.y += int16_t(top_left_extension.y + bottom_right_extension.y);
		base_data.position.x -= int16_t(top_left_extension.x);
		base_data.position.y -= int16_t(top_left_extension.y);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		simple_text_element_base::render(state, x + top_left_extension.x, y + top_left_extension.y);
	}
};

class state_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		set_text(state, text::get_dynamic_state_name(state, content));
	}
};

class province_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::produce_simple_string(state, state.world.province_get_name(content)));
	}
};

class province_factory_count_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		int32_t count = economy::province_factory_count(state, content);
		auto txt = std::to_string(count) + "/" + std::to_string(int32_t(state.defines.factories_per_state));
		set_text(state, txt);
	}
};

class state_admin_efficiency_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		auto owner = state.world.province_get_nation_from_province_ownership(content);
		set_text(state, text::format_percentage(state.world.province_get_control_ratio(content), 1));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		auto owner = state.world.province_get_nation_from_province_ownership(content);
		auto sid = state.world.province_get_state_membership(content);

		// text::add_line(state, contents, "admin_explain_8", text::variable_type::val, text::fp_percentage{ economy::local_admin_ratio(state, owner, content) });

		// Vanilla numbers. Show only in colonial states
		if(state.world.province_get_is_colonial(content)) {
			text::add_line(state, contents, "admin_integrating_explain_1", text::variable_type::val, text::fp_percentage{ province::state_admin_efficiency(state, sid) });
		}

		// Population per admin
		// text::add_line(state, contents, "admin_explain_11", text::variable_type::val, text::fp_one_place{ economy::population_per_admin(state, owner) });

		auto admin_mod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::administrative_efficiency_modifier);
		text::add_line(state, contents, "admin_explain_1", text::variable_type::val, text::fp_percentage{ 1.0f + admin_mod }, 15);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::administrative_efficiency_modifier, false);

		float issue_sum = 0.0f;
		for(auto i : state.culture_definitions.social_issues) {
			issue_sum += state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(owner, i));
		}
		auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment + state.defines.max_bureaucracy_percentage;
		float non_core_effect = 0.0f;
		float separatism_effect = 0.0f;
		float bsum = 0.0f;
		float rebelb = 0.0f;

		if(!state.world.province_get_is_owner_core(content)) {
			non_core_effect += state.defines.noncore_tax_penalty;
		}
		if(state.world.province_get_nationalism(content) > 0.f) {
			separatism_effect += state.defines.separatism_tax_penalty;
		}
		for(auto po : state.world.province_get_pop_location(content)) {
			if(po.get_pop().get_is_primary_or_accepted_culture() &&
					po.get_pop().get_poptype() == state.culture_definitions.bureaucrat) {
				if(po.get_pop().get_rebel_faction_from_pop_rebellion_membership()) {
					rebelb += po.get_pop().get_size();
				}
				bsum += po.get_pop().get_size();
			}
		}

		text::add_line(state, contents, "admin_explain_7", text::variable_type::x, text::fp_one_place{ rebelb }, text::variable_type::y, text::fp_one_place{ bsum }, 15);
		text::add_line(state, contents, "admin_explain_4", text::variable_type::val, text::fp_percentage{ 0.01f / from_issues }, 15);
		text::add_line(state, contents, "admin_explain_5", text::variable_type::val, text::fp_one_place{ non_core_effect }, 15);
		text::add_line(state, contents, "admin_explain_6", text::variable_type::val, text::fp_one_place{ separatism_effect }, 15);

		// text::add_line(state, contents, "admin_explain_9", text::variable_type::val, text::fp_currency{ economy::tax_collection_capacity(state, owner, content) });

		auto localtax = economy::explain_tax_income_local(state, state.local_player_nation, content);
		// Collected taxes
		{
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ localtax.poor });
			text::add_to_substitution_map(sub, text::variable_type::y, text::fp_currency{ localtax.mid });
			text::add_to_substitution_map(sub, text::variable_type::m, text::fp_currency{ localtax.rich });
			text::add_to_substitution_map(sub, text::variable_type::total, text::fp_currency{ localtax.poor + localtax.mid + localtax.rich });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "admin_explain_12", sub);
			text::close_layout_box(contents, box);
		};
		// Potential taxes
		{
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ localtax.poor_potential });
			text::add_to_substitution_map(sub, text::variable_type::y, text::fp_currency{ localtax.mid_potential });
			text::add_to_substitution_map(sub, text::variable_type::m, text::fp_currency{ localtax.rich_potential });
			text::add_to_substitution_map(sub, text::variable_type::total, text::fp_currency{ localtax.poor_potential + localtax.mid_potential + localtax.rich_potential });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "admin_explain_13", sub);
			text::close_layout_box(contents, box);
		};

		// Spendings on salaries
		// auto fraction = float(state.world.nation_get_administrative_spending(state.local_player_nation)) / 100.0f;
		// text::add_line(state, contents, "admin_explain_10", text::variable_type::val, text::fp_currency{ economy::estimate_spendings_administration_local(state, owner, content, fraction) });
	}
};

class state_aristocrat_presence_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto total_pop = state.world.state_instance_get_demographics(content, demographics::total);
		auto aristocrat_key = demographics::to_key(state, state.culture_definitions.aristocrat);
		auto aristocrat_amount = state.world.state_instance_get_demographics(content, aristocrat_key);
		auto txt = text::format_percentage(total_pop > 0 ? aristocrat_amount / total_pop : 0.0f, 1);
		set_text(state, txt);
	}
};

class state_population_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto total_pop = state.world.state_instance_get_demographics(content, demographics::total);
		set_text(state, text::prettify(int32_t(total_pop)));
	}
};

class standard_movement_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::movement_id movement_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::movement_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class movement_size_text : public standard_movement_text {
public:
	std::string get_text(sys::state& state, dcon::movement_id movement_id) noexcept override {
		auto size = state.world.movement_get_pop_support(movement_id);
		return text::prettify(int64_t(size));
	}
};

class movement_radicalism_text : public standard_movement_text {
public:
	std::string get_text(sys::state& state, dcon::movement_id movement_id) noexcept override {
		auto radicalism = state.world.movement_get_radicalism(movement_id);
		return text::format_float(radicalism, 1);
	}
};

class movement_issue_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto mid = retrieve<dcon::movement_id>(state, parent);
		auto issue = state.world.movement_get_associated_issue_option(mid);
		if(issue)
			set_text(state, text::produce_simple_string(state, issue.get_movement_name()));
		else
			set_text(state, "");
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto mid = retrieve<dcon::movement_id>(state, parent);
		auto issue = state.world.movement_get_associated_issue_option(mid);
		text::add_line(state, contents, "reform_movement_desc", text::variable_type::reform, state.world.issue_option_get_name(issue));
	}
};

class standard_movement_multiline_text : public multiline_text_element_base {
public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents, dcon::movement_id movement_id) noexcept { }

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::movement_id>(state, parent);
		auto color = black_text ? text::text_color::black : text::text_color::white;
		auto container =
				text::create_endless_layout(state, internal_layout, text::layout_parameters{0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color, false});
		populate_layout(state, container, content);
	}
};

class movement_nationalist_name_text : public standard_movement_multiline_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::movement_id movement_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, movement_id);
		auto independence_target = fat_id.get_associated_independence();
		if(!independence_target)
			return;

		auto box = text::open_layout_box(contents);
		text::substitution_map sub{};
		if(independence_target.get_cultural_union_of().id) {
			std::string movement_adj = text::get_adjective_as_string(state, independence_target);
			text::add_to_substitution_map(sub, text::variable_type::country_adj, std::string_view(movement_adj));
			text::localised_format_box(state, contents, box, std::string_view("nationalist_union_movement"), sub);
		} else {
			std::string movement_adj = text::get_adjective_as_string(state, independence_target);
			text::add_to_substitution_map(sub, text::variable_type::country, std::string_view(movement_adj));
			text::localised_format_box(state, contents, box, std::string_view("nationalist_liberation_movement"), sub);
		}
		text::close_layout_box(contents, box);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto mid = retrieve<dcon::movement_id>(state, parent);
		auto independence_target = state.world.movement_get_associated_independence(mid);
		if(!independence_target)
			return;
		if(state.world.national_identity_get_cultural_union_of(independence_target)) {
			text::add_line(state, contents, "NATIONALIST_UNION_MOVEMENT_DESC", text::variable_type::reform, state.world.national_identity_get_name(independence_target));
		} else {
			text::add_line(state, contents, "NATIONALIST_LIBERATION_MOVEMENT_DESC", text::variable_type::reform, state.world.national_identity_get_name(independence_target));
		}
	}
};

class standard_rebel_faction_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::rebel_faction_id rebel_faction_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::rebel_faction_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class national_identity_vassal_type_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::nation_id>(state, parent);
		if(state.world.nation_get_is_substate(content))
			set_text(state, text::produce_simple_string(state, "substate"));
		else
			set_text(state, text::produce_simple_string(state, "satellite"));
	}
};

class standard_nation_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::nation_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class nation_gp_opinion_text : public standard_nation_text {
public:
	uint16_t rank = 0;
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto const great_power_id = nations::get_nth_great_power(state, rank);
		if(!bool(great_power_id))
			return "0";
		auto great_power_rel = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, great_power_id);
		auto fat_id = dcon::fatten(state.world, great_power_rel);
		auto influence = fat_id.get_influence();
		return std::to_string(int32_t(influence));
	}
};

class nation_player_investment_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(nation_id, state.local_player_nation);
		auto fat_id = dcon::fatten(state.world, uni_rel);
		return text::format_money(fat_id.get_foreign_investment());
	}
};

class nation_gp_investment_text : public standard_nation_text {
public:
	uint16_t rank = 0;
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto const great_power_id = nations::get_nth_great_power(state, rank);
		if(!bool(great_power_id))
			return text::format_money(0.f);
		auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(nation_id, great_power_id);
		auto fat_id = dcon::fatten(state.world, uni_rel);
		return text::format_money(fat_id.get_foreign_investment());
	}
};

class nation_overlord_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		dcon::nation_id sphereling_id = retrieve<dcon::nation_id>(state, parent);
		auto ovr_id = state.world.nation_get_in_sphere_of(sphereling_id);
		return ovr_id.get_identity_from_identity_holder();
	}

	void on_update(sys::state& state) noexcept override {
		set_current_nation(state, get_current_nation(state));
	}
	void button_action(sys::state& state) noexcept override {
		if(get_current_nation(state))
			flag_button::button_action(state);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(get_current_nation(state))
			flag_button::render(state, x, y);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(get_current_nation(state))
			flag_button::update_tooltip(state, x, y, contents);
	}
};

class nation_prestige_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return std::to_string(int32_t(nations::prestige_score(state, nation_id)));
	}
};

class nation_industry_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(fat_id.get_industrial_score()));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		if(state.world.nation_get_owned_province_count(n) == 0)
			return;

		auto iweight = state.defines.investment_score_factor;
		for(auto si : state.world.nation_get_state_ownership(n)) {
			float score = 0.f;
			float workers = 0.f;
			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto f : state.world.province_get_factory_location(p)) {
					score += economy::factory_total_employment(state, f.get_factory())
						/ f.get_factory().get_building_type().get_base_workforce();
					workers += economy::factory_total_employment(state, f.get_factory());
				}
			});
			float per_state = 4.0f * score;
			if(per_state > 0.f) {
				auto box = text::open_layout_box(contents);
				text::layout_box name_entry = box;
				text::layout_box level_entry = box;
				text::layout_box workers_entry = box;
				text::layout_box max_workers_entry = box;
				text::layout_box score_box = box;

				name_entry.x_size /= 10;
				text::add_to_layout_box(state, contents, name_entry, text::get_short_state_name(state, si.get_state()).substr(0, 20), text::text_color::yellow);
				
				workers_entry.x_position += 150;
				text::add_to_layout_box(state, contents, workers_entry, text::int_wholenum{ int32_t(workers) });

				score_box.x_position += 250;
				text::add_to_layout_box(state, contents, score_box, text::fp_two_places{ per_state });

				text::add_to_layout_box(state, contents, box, std::string(" "));
				text::close_layout_box(contents, box);
			}
		}
		float total_invest = nations::get_foreign_investment(state, n);
		if(total_invest > 0.f) {
			text::add_line(state, contents, "industry_score_explain_2", text::variable_type::x, text::fp_four_places{ iweight });
			for(auto ur : state.world.nation_get_unilateral_relationship_as_source(n)) {
				if(ur.get_foreign_investment() > 0.f) {
					text::substitution_map sub{};
					text::add_to_substitution_map(sub, text::variable_type::x, ur.get_target());
					text::add_to_substitution_map(sub, text::variable_type::y, text::fp_currency{ ur.get_foreign_investment() });
					auto box = text::open_layout_box(contents);
					text::localised_format_box(state, contents, box, std::string_view("industry_score_explain_3"), sub);
					text::close_layout_box(contents, box);
				}
			}
		}
	}
};

class nation_military_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(fat_id.get_military_score()));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto recruitable = state.world.nation_get_recruitable_regiments(n);
		auto active_regs = state.world.nation_get_active_regiments(n);
		auto is_disarmed = state.world.nation_get_disarmed_until(n) > state.current_date;
		auto disarm_factor = is_disarmed ? state.defines.disarmament_army_hit : 1.f;
		auto supply_mod = std::max(state.world.nation_get_modifier_values(n, sys::national_mod_offsets::supply_consumption) + 1.0f, 0.1f);
		auto avg_land_score = state.world.nation_get_averge_land_unit_score(n);
		auto gen_range = state.world.nation_get_leader_loyalty(n);
		auto num_capital_ships = state.world.nation_get_capital_ship_score(n);
		text::add_line(state, contents, "military_score_explain_1", text::variable_type::x, text::fp_two_places{ num_capital_ships });
		text::add_line(state, contents, "military_score_explain_2", text::variable_type::x, text::int_wholenum{ recruitable });
		text::add_line(state, contents, "military_score_explain_3", text::variable_type::x, text::int_wholenum{ active_regs });
		text::add_line_with_condition(state, contents, "military_score_explain_4", is_disarmed, text::variable_type::x, text::fp_two_places{ state.defines.disarmament_army_hit });
		text::add_line(state, contents, "military_score_explain_5", text::variable_type::x, text::fp_two_places{ supply_mod });
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::supply_consumption, true);
		text::add_line(state, contents, "military_score_explain_6", text::variable_type::x, text::fp_two_places{ avg_land_score });
		//text::add_line(state, contents, "military_score_explain_7", text::variable_type::x, text::fp_two_places{ num_leaders });
	}
};

class nation_total_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(
				int32_t(nations::prestige_score(state, nation_id) + fat_id.get_industrial_score() + fat_id.get_military_score()));
	}
};

class nation_ppp_gdp_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {		
		return text::format_float(economy::gdp(state, nation_id));
	}
};

class nation_ppp_gdp_per_capita_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		float population = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_float(economy::gdp(state, nation_id) / population * 1000000.f);
	}
};

class nation_sol_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_float(demographics::calculate_nation_sol(state, nation_id));
	}
};

class nation_prestige_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_prestige_rank());
	}
};

class nation_industry_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_industrial_rank());
	}
};

class nation_military_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_military_rank());
	}
};

class nation_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_rank());
	}
};

class nation_status_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return get_status_text(state, nation_id);
	}
};

class nation_ruling_party_ideology_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto ruling_party = state.world.nation_get_ruling_party(nation_id);
		auto ideology = state.world.political_party_get_ideology(ruling_party);
		return text::get_name_as_string(state, ideology);
	}
};

class nation_ruling_party_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::get_name_as_string(state, fat_id.get_ruling_party());
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, nation_id);
		std::string ruling_party = text::get_name_as_string(state, fat_id.get_ruling_party());
		ruling_party = ruling_party + " (" + text::get_name_as_string(state, state.world.political_party_get_ideology(state.world.nation_get_ruling_party(nation_id))) + ")";
		{
			auto box = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box, std::string_view("topbar_ruling_party"), text::variable_type::curr, std::string_view(ruling_party));
			text::add_divider_to_layout_box(state, contents, box);
			text::close_layout_box(contents, box);
		}
		for(auto pi : state.culture_definitions.party_issues) {
			reform_description(state, contents, state.world.political_party_get_party_issues(fat_id.get_ruling_party(), pi));
			text::add_line_break_to_layout(state, contents);
		}
	}
};

class nation_government_type_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		auto gov_type_id = fat_id.get_government_type();
		auto gov_name_seq = state.world.government_type_get_name(gov_type_id);
		return text::produce_simple_string(state, gov_name_seq);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents);
		text::substitution_map sub{};
		text::add_to_substitution_map(sub, text::variable_type::country, n);
		text::add_to_substitution_map(sub, text::variable_type::country_adj, text::get_adjective(state, n));
		text::add_to_substitution_map(sub, text::variable_type::capital, state.world.nation_get_capital(n));
		text::add_to_substitution_map(sub, text::variable_type::continentname, state.world.modifier_get_name(state.world.province_get_continent(state.world.nation_get_capital(n))));
		text::add_to_layout_box(state, contents, box, state.world.government_type_get_desc(state.world.nation_get_government_type(n)), sub);
		text::close_layout_box(contents, box);
	}
};

class nation_sphere_list_label : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		if(bool(fat_id.get_in_sphere_of().id)) {
			return text::produce_simple_string(state, "rel_sphere_of_influence") + ":";
		} else {
			return text::produce_simple_string(state, "diplomacy_sphere_label");
		}
	}
};

class nation_puppet_list_label : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		auto puppets_iter = state.world.nation_get_overlord_as_ruler(nation_id);
		if(puppets_iter.begin() < puppets_iter.end()) {
			return text::produce_simple_string(state, "country_puppets_list");
		} else if(fat_id.get_is_substate()) {
			return text::produce_simple_string(state, "substate_of_nocolor") + ":";
		} else {
			return text::produce_simple_string(state, "satellite_of_nocolor") + ":";
		}
	}
};

class nation_player_opinion_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto gp_rel_id = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, state.local_player_nation);
		if(bool(gp_rel_id)) {
			auto const status = state.world.gp_relationship_get_status(gp_rel_id);
			return text::get_influence_level_name(state, status);
		}
		return text::produce_simple_string(state, "rel_neutral");
	}
};

class nation_industries_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		uint32_t num_factories = 0;
		for(auto si : state.world.nation_get_state_ownership(nation_id)) {
			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				num_factories +=
						uint32_t(state.world.province_get_factory_location(p).end() - state.world.province_get_factory_location(p).begin());
			});
		}
		return std::to_string(num_factories);
	}
};

class nation_provinces_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		size_t num_provinces = 0;
		for(auto si : state.world.nation_get_state_ownership(nation_id))
			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) { ++num_provinces; });
		return std::to_string(num_provinces);
	}
};

class nation_player_relations_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(nation_id, state.local_player_nation);
		auto fat_rel = dcon::fatten(state.world, rel);
		return std::to_string(int32_t(fat_rel.get_value()));
	}
};

class nation_militancy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto militancy = state.world.nation_get_demographics(nation_id, demographics::militancy);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_float(militancy / total_pop);
	}
};

class nation_consciousness_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto consciousness = state.world.nation_get_demographics(nation_id, demographics::consciousness);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_float(consciousness / total_pop);
	}
};

class nation_colonial_power_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fcp = nations::free_colonial_points(state, nation_id);
		auto mcp = nations::max_colonial_points(state, nation_id);
		return text::format_ratio(fcp, mcp);
	}
};

class nation_budget_funds_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto budget = nations::get_treasury(state, nation_id);
		return text::format_money(budget);
	}
};

class nation_budget_bank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto budget = nations::get_bank_funds(state, nation_id);
		return text::format_money(budget);
	}
};

class nation_budget_debt_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto budget = nations::get_debt(state, nation_id);
		return text::format_money(budget);
	}
};

class nation_budget_interest_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto budget = economy::interest_payment(state, nation_id);
		return text::format_money(budget);
	}
};

class nation_literacy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto literacy = state.world.nation_get_demographics(nation_id, demographics::literacy);
		auto total_pop = std::max(1.0f, state.world.nation_get_demographics(nation_id, demographics::total));
		return text::format_percentage(literacy / total_pop, 1);
	}
};

class nation_infamy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::format_float(fat_id.get_infamy(), 2);
	}
};

class nation_population_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::prettify(int64_t(total_pop));
	}
};

class nation_current_research_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto tech_id = nations::current_research(state, nation_id);
		if(tech_id) {
			return text::get_name_as_string(state, dcon::fatten(state.world, tech_id));
		}
		return "?R" + text::produce_simple_string(state, "tb_tech_no_current");
	}
};

class nation_daily_research_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = nations::daily_research_points(state, nation_id);
		return text::format_float(points, 2);
	}
};

class nation_research_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = state.world.nation_get_research_points(nation_id);
		return text::format_float(points, 1);
	}
};

class nation_suppression_points_text : public expanded_hitbox_text {
public:

	void on_create(sys::state& state) noexcept override {
		top_left_extension = ui::xy_pair{ 25, 3 };
		bottom_right_extension = ui::xy_pair{ -10, 2 };
		expanded_hitbox_text::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto points = nations::suppression_points(state, retrieve<dcon::nation_id>(state, parent));
		set_text(state, text::format_float(points, 1));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);

		auto base = state.defines.suppression_points_gain_base;
		auto nmod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::suppression_points_modifier) + 1.0f;
		auto bmod = state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.bureaucrat)) /
						std::max(state.world.nation_get_demographics(n, demographics::total), 1.0f) * state.defines.suppress_bureaucrat_factor;

		text::add_line(state, contents, "sup_point_gain", text::variable_type::val, text::fp_two_places{base * nmod * bmod});
		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "sup_point_explain", text::variable_type::x, text::fp_two_places{base},
				text::variable_type::y, text::fp_three_places{bmod}, text::variable_type::val, text::fp_two_places{nmod});

		text::add_line_break_to_layout(state, contents);
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::suppression_points_modifier, true);
	}
};

class nation_focus_allocation_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto available = nations::max_national_focuses(state, nation_id);
		auto in_use = nations::national_focuses_in_use(state, nation_id);
		return text::format_ratio(in_use, available);
	}
};

class nation_diplomatic_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = nations::diplomatic_points(state, nation_id);
		return text::format_float(points, 0);
	}
};

class nation_mobilization_size_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		int32_t sum = 0;
		auto fat_id = dcon::fatten(state.world, nation_id);
		for(auto prov_own : fat_id.get_province_ownership_as_nation()) {
			auto prov = prov_own.get_province();
			sum += military::mobilized_regiments_possible_from_province(state, prov.id);
		}
		return std::to_string(sum);
	}
};

class nation_brigade_allocation_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto available = state.world.nation_get_recruitable_regiments(nation_id);
		auto in_use = state.world.nation_get_active_regiments(nation_id);
		return text::format_ratio(in_use, available);
	}
};

class nation_navy_allocation_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto available = military::naval_supply_points(state, nation_id);
		auto in_use = military::naval_supply_points_used(state, nation_id);
		return text::format_ratio(in_use, available);
	}
};

class nation_num_regiments : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::prettify(military::total_regiments(state, nation_id));
	}
};

class nation_num_ships : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::prettify(military::total_ships(state, nation_id));
	}
};

class nation_leadership_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = nations::leadership_points(state, nation_id);
		return text::format_float(points, 1);
	}
};

class nation_can_do_social_reform_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		if(nations::has_social_reform_available(state, nation_id)) {
			// yes, the original localisation says "refroms"
			return text::produce_simple_string(state, "politics_can_do_social_refroms");
		} else {
			return text::produce_simple_string(state, "politics_can_not_do_social_refroms");
		}
	}
};

class nation_can_do_political_reform_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		if(nations::has_political_reform_available(state, nation_id)) {
			// yes, the original localisation says "refroms"
			return text::produce_simple_string(state, "politics_can_do_political_refroms");
		} else {
			return text::produce_simple_string(state, "politics_can_not_do_political_refroms");
		}
	}
};

class nation_national_value_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::produce_simple_string(state, fat_id.get_national_value().get_name());
	}
};

class national_tech_school : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto mod_id = state.world.nation_get_tech_school(n);
		if(bool(mod_id)) {
			set_text(state, text::produce_simple_string(state, state.world.modifier_get_name(mod_id)));
		} else {
			set_text(state, text::produce_simple_string(state, "traditional_academic"));
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = state.local_player_nation;
		auto mod_id = state.world.nation_get_tech_school(retrieve<dcon::nation_id>(state, parent));
		if(bool(mod_id)) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, state.world.modifier_get_name(mod_id), text::text_color::yellow);
			if(auto desc = state.world.modifier_get_desc(mod_id); state.key_is_localized(desc)) {
				text::substitution_map sub{};
				text::add_to_substitution_map(sub, text::variable_type::country, n);
				text::add_to_substitution_map(sub, text::variable_type::country_adj, text::get_adjective(state, n));
				text::add_to_substitution_map(sub, text::variable_type::capital, state.world.nation_get_capital(n));
				text::add_to_substitution_map(sub, text::variable_type::continentname, state.world.modifier_get_name(state.world.province_get_continent(state.world.nation_get_capital(n))));
				text::add_to_layout_box(state, contents, box, state.world.modifier_get_desc(mod_id), sub);
			}
			text::close_layout_box(contents, box);
			modifier_description(state, contents, mod_id);
		}
	}
};

class standard_nation_icon : public image_element_base {
public:
	virtual int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		frame = get_icon_frame(state, n);
	}
};


class nation_westernization_progress_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		progress = state.world.nation_get_modifier_values(nation_id, sys::national_mod_offsets::civilization_progress_modifier);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "modifier_civilization_progress");
			text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ state.world.nation_get_modifier_values(nation_id, sys::national_mod_offsets::civilization_progress_modifier) }, text::text_color::green);
			text::close_layout_box(contents, box);
		}
		active_modifiers_description(state, contents, nation_id, 15, sys::national_mod_offsets::civilization_progress_modifier, false);
	}
};

class nation_technology_research_progress : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto tech_id = nations::current_research(state, nation_id);
		if(tech_id && state.world.technology_get_cost(tech_id) > 0) {
			progress = state.world.nation_get_research_points(nation_id) / culture::effective_technology_rp_cost(state, state.current_date.to_ymd(state.start_date).year, state.local_player_nation, tech_id);
		} else if (tech_id && state.world.technology_get_leadership_cost(tech_id) > 0) {
			progress = state.world.nation_get_leadership_points(nation_id) / culture::effective_technology_lp_cost(state, state.current_date.to_ymd(state.start_date).year, state.local_player_nation, tech_id);
		} else {
			progress = 0.f;
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto tech_id = nations::current_research(state, nation_id);

		if(tech_id && state.world.technology_get_cost(tech_id) > 0) {
			text::add_line(state, contents, "technologyview_research_tooltip",
				text::variable_type::tech, state.world.technology_get_name(tech_id),
				text::variable_type::date, nations::get_research_end_date(state, tech_id, nation_id));
			text::add_line(state, contents, "technologyview_research_invested_tooltip",
				text::variable_type::invested, int64_t(state.world.nation_get_research_points(nation_id)),
				text::variable_type::cost, int64_t(culture::effective_technology_rp_cost(state, state.ui_date.to_ymd(state.start_date).year, nation_id, tech_id)));
		} else if(tech_id && state.world.technology_get_leadership_cost(tech_id) > 0) {
			text::add_line(state, contents, "technologyview_research_tooltip",
				text::variable_type::tech, state.world.technology_get_name(tech_id),
				text::variable_type::date, nations::get_research_end_date(state, tech_id, nation_id));
			text::add_line(state, contents, "technologyview_research_invested_tooltip",
				text::variable_type::invested, int64_t(state.world.nation_get_leadership_points(nation_id)),
				text::variable_type::cost, int64_t(culture::effective_technology_lp_cost(state, state.ui_date.to_ymd(state.start_date).year, nation_id, tech_id)));
		} else {
			text::add_line(state, contents, "technologyview_no_research_tooltip");
		}
	}
};

class standard_state_instance_button : public button_element_base {
public:
	virtual int32_t get_icon_frame(sys::state& state, dcon::state_instance_id state_instance_id) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		auto state_instance_id = retrieve<dcon::state_instance_id>(state, parent);
		frame = get_icon_frame(state, state_instance_id);
	}
};

class standard_nation_button : public button_element_base {
public:
	virtual int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		frame = get_icon_frame(state, nation_id);
	}
};

class nation_flag_frame : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto status = nations::get_status(state, nation_id);
		return std::min(3, int32_t(status));
	}
};

class nation_can_do_social_reform_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!nations::has_social_reform_available(state, nation_id));
	}
};

class nation_can_do_political_reform_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!nations::has_political_reform_available(state, nation_id));
	}
};

class nation_military_reform_multiplier_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(politics::get_military_reform_multiplier(state, nation_id) >= 0.f);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);

		auto val = politics::get_military_reform_multiplier(state, nation_id) - 1.0f;
		text::add_line(state, contents, "alice_unciv_reform_cost", text::variable_type::x, text::fp_percentage_one_place{ val });
	}
};

class nation_economic_reform_multiplier_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(politics::get_economic_reform_multiplier(state, nation_id) >= 0.f);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);

		auto val = politics::get_economic_reform_multiplier(state, nation_id) - 1.0f;
		text::add_line(state, contents, "alice_unciv_reform_cost", text::variable_type::x, text::fp_percentage_one_place{ val });
	}
};

class nation_ruling_party_ideology_plupp : public tinted_image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto ruling_party = state.world.nation_get_ruling_party(nation_id);
		auto ideology = state.world.political_party_get_ideology(ruling_party);
		color = state.world.ideology_get_color(ideology);
	}
};

class nation_player_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, state.local_player_nation);
		return fat_id.get_identity_from_identity_holder();
	}
};

class nation_gp_flag : public flag_button {
public:
	uint16_t rank = 0;
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto const nat_id = nations::get_nth_great_power(state, rank);
		if(!bool(nat_id))
			return dcon::national_identity_id{};
		auto fat_id = dcon::fatten(state.world, nat_id);
		return fat_id.get_identity_from_identity_holder();
	}
};

class ideology_plupp : public tinted_image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto ideology_id = retrieve<dcon::ideology_id>(state, parent);
		color = state.world.ideology_get_color(ideology_id);
	}
};

class military_score_icon : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province = retrieve<dcon::province_id>(state, parent);
		auto nation = retrieve<dcon::nation_id>(state, parent);
		
		if(province) {
			auto fat_id = dcon::fatten(state.world, province);
			if(fat_id.get_province_ownership().get_nation().get_is_mobilized()) {
				frame = 1;
			} else {
				frame = 0;
			}
		} else if (nation) {
			auto fat_id = dcon::fatten(state.world, nation);
			if(fat_id.get_is_mobilized()) {
				frame = 1;
			} else {
				frame = 0;
			}
		}
		
	}
};

class fixed_pop_type_icon : public opaque_element_base {
public:
	dcon::pop_type_id type{};

	void set_type(sys::state& state, dcon::pop_type_id t) {
		type = t;
		frame = int32_t(state.world.pop_type_get_sprite(t) - 1);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto name = state.world.pop_type_get_name(type);
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, name);
		text::close_layout_box(contents, box);
	}
};

class pop_type_icon : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::pop_type_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, content);
		frame = int32_t(fat_id.get_sprite() - 1);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::pop_type_id>(state, parent);
		auto name = state.world.pop_type_get_name(content);
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, name);
		text::close_layout_box(contents, box);
	}
};
class religion_type_icon : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::religion_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, content);
		frame = int32_t(fat_id.get_icon() - 1);
	}
};

class nation_ideology_percentage_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto ideology_id = retrieve<dcon::ideology_id>(state, parent);
		if(nation_id && ideology_id) {
			auto percentage = .01f * state.world.nation_get_upper_house(nation_id, ideology_id);
			set_text(state, text::format_percentage(percentage, 1));
		}
	}
};

class upper_house_piechart : public piechart<dcon::ideology_id> {
protected:
	void on_update(sys::state& state) noexcept override {
		auto nat_id = retrieve<dcon::nation_id>(state, parent);
		distribution.clear();
		for(auto id : state.world.in_ideology) {
			distribution.emplace_back(id.id, float(state.world.nation_get_upper_house(nat_id, id)));
		}
		update_chart(state);
	}
};

class voter_ideology_piechart : public piechart<dcon::ideology_id> {
protected:
	void on_update(sys::state& state) noexcept override {
		auto nat_id = retrieve<dcon::nation_id>(state, parent);
		distribution.clear();
		for(auto id : state.world.in_ideology) {
			distribution.emplace_back(id.id, 0.0f);
		}
		for(auto p : state.world.nation_get_province_ownership(nat_id)) {
			for(auto pop_loc : state.world.province_get_pop_location(p.get_province())) {
				auto pop_id = pop_loc.get_pop();
				float vote_size = politics::pop_vote_weight(state, pop_id, nat_id);
				if(vote_size > 0) {
					state.world.for_each_ideology([&](dcon::ideology_id iid) {
						auto dkey = pop_demographics::to_key(state, iid);
						distribution[iid.index()].value += pop_demographics::get_demo(state, pop_id.id, dkey) * vote_size;
					});
				}
			}
		}

		update_chart(state);
	}
};

class province_population_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		set_text(state, text::prettify(int32_t(total_pop)));
	}
};

class province_militancy_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto militancy = state.world.province_get_demographics(province_id, demographics::militancy);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		set_text(state, text::format_float(militancy / total_pop));
	}
};

class province_consciousness_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto consciousness = state.world.province_get_demographics(province_id, demographics::consciousness);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		set_text(state, text::format_float(consciousness / total_pop));
	}
};

class province_literacy_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto literacy = state.world.province_get_demographics(province_id, demographics::literacy);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		set_text(state, text::format_percentage(literacy / total_pop, 1));
	}
};


class province_dominant_culture_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::get_name_as_string(state, state.world.province_get_dominant_culture(province_id)));
	}
};
class province_dominant_religion_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::get_name_as_string(state, state.world.province_get_dominant_religion(province_id)));
	}
};
class province_dominant_issue_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::get_name_as_string(state, state.world.province_get_dominant_issue_option(province_id)));
	}
};
class province_dominant_ideology_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::get_name_as_string(state, state.world.province_get_dominant_ideology(province_id)));
	}
};

class province_state_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::get_province_state_name(state, province_id));
	}
};

class province_rgo_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::get_name_as_string(state, state.world.province_get_rgo(province_id)));
	}
};

class province_goods_produced_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::format_float(economy::rgo_output(state, state.world.province_get_rgo(province_id), province_id), 3));
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);

		auto n = state.world.province_get_nation_from_province_ownership(p);

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto production = economy::rgo_output(state, c, p);

			if(production < 0.0001f) {
				return;
			}

			auto base_box = text::open_layout_box(contents);
			auto name_box = base_box;
			name_box.x_size = 75;
			auto production_box = base_box;
			production_box.x_position += 120.f;

			text::add_to_layout_box(state, contents, name_box, text::get_name_as_string(state, dcon::fatten(state.world, c)));
			text::add_to_layout_box(state, contents, production_box, text::format_money(production));
			text::add_to_layout_box(state, contents, base_box, std::string(" "));
			text::close_layout_box(contents, base_box);
		});
	}
};

class province_income_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::format_money(economy::rgo_income(state, province_id)));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);

		auto n = state.world.province_get_nation_from_province_ownership(p);

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto profit = economy::rgo_income(state, c, p);

			if(profit < 0.0001f) {
				return;
			}

			auto base_box = text::open_layout_box(contents);
			auto name_box = base_box;
			name_box.x_size = 75;
			auto profit_box = base_box;
			profit_box.x_position += 120.f;

			text::add_to_layout_box(state, contents, name_box, text::get_name_as_string(state, dcon::fatten(state.world, c)));
			text::add_to_layout_box(state, contents, profit_box, text::format_money(profit));
			text::add_to_layout_box(state, contents, base_box, std::string(" "));
			text::close_layout_box(contents, base_box);
		});
	}
};

class province_rgo_workers_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::prettify(int32_t(economy::rgo_employment(state, province_id))));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);

		auto n = state.world.province_get_nation_from_province_ownership(p);
		auto s = state.world.province_get_state_membership(p);
		auto m = state.world.state_instance_get_market_from_local_market(s);

		auto row_1 = text::open_layout_box(contents);
		auto col_1 = row_1;
		col_1.x_size = 75;
		auto col_2 = row_1;
		col_2.x_position += 90.f;
		auto col_3 = row_1;
		col_3.x_position += 180.f;
		auto col_4 = row_1;
		col_4.x_position += 250.f;
		text::add_to_layout_box(state, contents, col_1, std::string_view("Good"));
		text::add_to_layout_box(state, contents, col_2, std::string_view("Employed"));
		text::add_to_layout_box(state, contents, col_3, std::string_view("Max"));
		text::add_to_layout_box(state, contents, col_4, std::string_view("Profit"));
		text::add_to_layout_box(state, contents, row_1, std::string_view(" "));
		text::close_layout_box(contents, row_1);

		state.world.for_each_commodity([&](dcon::commodity_id c) {
			auto current_employment = int64_t(economy::rgo_employment(state, c, p));
			auto max_employment = int64_t(economy::rgo_max_employment(state, c, p));

			if(max_employment < 1.f) {
				return;
			}
			auto base_box = text::open_layout_box(contents);
			auto name_box = base_box;
			name_box.x_size = 75;
			auto employment_box = base_box;
			employment_box.x_position += 120.f;
			auto max_employment_box = base_box;
			max_employment_box.x_position += 180.f;

			text::add_to_layout_box(state, contents, name_box, text::get_name_as_string(state, dcon::fatten(state.world, c)));

			
			text::add_to_layout_box(state, contents, employment_box, current_employment);
			text::add_to_layout_box(state, contents, max_employment_box, max_employment);

			text::add_to_layout_box(state, contents, base_box, std::string(" "));
			text::close_layout_box(contents, base_box);
		});

		auto rgo_employment = state.world.province_get_subsistence_employment(p);
		auto current_employment = int64_t(rgo_employment);
		auto max_employment = int64_t(economy::subsistence_max_pseudoemployment(state, n, p));
		auto expected_profit = 0.f;

		auto base_box = text::open_layout_box(contents);
		auto name_box = base_box;
		name_box.x_size = 75;
		auto employment_box = base_box;
		employment_box.x_position += 120.f;
		auto max_employment_box = base_box;
		max_employment_box.x_position += 180.f;
		auto expected_profit_box = base_box;
		expected_profit_box.x_position += 250.f;

		text::add_to_layout_box(state, contents, name_box, std::string_view("Subsistence"));

		text::add_to_layout_box(state, contents, employment_box, current_employment);
		text::add_to_layout_box(state, contents, max_employment_box, max_employment);
		text::add_to_layout_box(state, contents, expected_profit_box, text::format_money(expected_profit));

		text::add_to_layout_box(state, contents, base_box, std::string(" "));
		text::close_layout_box(contents, base_box);

		active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::mine_rgo_size, false);
		if(auto owner = state.world.province_get_nation_from_province_ownership(p); owner)
			active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::mine_rgo_size, false);
		active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::farm_rgo_size, false);
		if(auto owner = state.world.province_get_nation_from_province_ownership(p); owner)
			active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::farm_rgo_size, false);
	}
};

class province_rgo_size_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::format_float(economy::rgo_max_employment(state, province_id), 2));
	}
};

class factory_state_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto factory_id = retrieve<dcon::factory_id>(state, parent);
		auto flid = state.world.factory_get_factory_location_as_factory(factory_id);
		auto pid = state.world.factory_location_get_province(flid);
		auto sdef = state.world.province_get_state_from_abstract_state_membership(pid);
		dcon::state_instance_id sid{};
		state.world.for_each_state_instance([&](dcon::state_instance_id id) {
			if(state.world.state_instance_get_definition(id) == sdef)
				sid = id;
		});
		set_text(state, text::get_dynamic_state_name(state, sid));
	}
};
class factory_output_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto factory_id = retrieve<dcon::factory_id>(state, parent);
		auto cid = state.world.factory_get_building_type(factory_id).get_output();
		set_text(state, text::get_name_as_string(state, cid));
	}
};
class factory_produced_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto factory_id = retrieve<dcon::factory_id>(state, parent);
		set_text(state, text::format_float(state.world.factory_get_output(factory_id), 2));
	}
};
class factory_income_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto factory_id = retrieve<dcon::factory_id>(state, parent);
		set_text(state, text::format_float(
			economy::explain_last_factory_profit(state, factory_id).profit
		, 2));
	}
};
class factory_workers_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto factory_id = retrieve<dcon::factory_id>(state, parent);
		set_text(state, text::format_float(economy::factory_total_employment(state, factory_id), 2));
	}
};
class factory_level_text : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.x += int16_t(20);
	}
	void on_update(sys::state& state) noexcept override {
		auto factory_id = retrieve<dcon::factory_id>(state, parent);		
		set_text(state, text::format_float(economy::get_factory_level(state, factory_id)));
	}
};
class factory_profit_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_id>(state, parent);

		auto profit = economy::explain_last_factory_profit(state, content).profit;
		bool is_positive = profit >= 0.f;
		auto text = (is_positive ? "+" : "") + text::format_float(profit, 2);
		// Create colour
		auto contents = text::create_endless_layout(state, internal_layout,
				text::layout_parameters{0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
						base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, true});
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text,
				is_positive ? text::text_color::dark_green : text::text_color::dark_red);
		text::close_layout_box(contents, box);
	}
};
class factory_income_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_id>(state, parent);
		float profit = economy::explain_last_factory_profit(state, content).profit;

		if(profit > 0.f) {
			frame = 0;
		} else if (profit < 0.f) {
			frame = 1;
		} else {
			frame = 2; //empty frame
		}
	}
};
class factory_priority_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_id>(state, parent);
		frame = economy::factory_priority(state, content);
	}
};

class commodity_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		frame = int32_t(state.world.commodity_get_icon(retrieve<dcon::commodity_id>(state, parent)));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		if(!com)
			return;

		auto n = retrieve<dcon::nation_id>(state, parent);
		auto p = retrieve<dcon::province_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_name(com)), text::text_color::yellow);
		text::close_layout_box(contents, box);

		// Nation modifiers
		if(bool(n)) {
			auto commodity_mod_description = [&](float value, std::string_view locale_base_name, std::string_view locale_farm_base_name) {
				if(value == 0.f)
					return;
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_name(com)), text::text_color::white);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_is_mine(com) ? locale_base_name : locale_farm_base_name), text::text_color::white);
				text::add_to_layout_box(state, contents, box, std::string{ ":" }, text::text_color::white);
				text::add_space_to_layout_box(state, contents, box);
				auto color = value > 0.f ? text::text_color::green : text::text_color::red;
				text::add_to_layout_box(state, contents, box, (value > 0.f ? "+" : "") + text::format_percentage(value, 1), color);
				text::close_layout_box(contents, box);
			};
			commodity_mod_description(state.world.nation_get_factory_goods_output(n, com), "tech_output", "tech_output");
			commodity_mod_description(state.world.nation_get_rgo_goods_output(n, com), "tech_mine_output", "tech_farm_output");
			commodity_mod_description(state.world.nation_get_rgo_size(n, com), "tech_mine_size", "tech_farm_size");
		}
		if(economy::commodity_get_factory_types_as_output(state, com).size() > 0) {
			if(bool(n)) {
				active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::factory_output, true);
			}
			if(bool(p)) {
				active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::local_factory_output, true);
			}
			if(bool(n)) {
				active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::factory_throughput, true);
			}
			if(bool(p)) {
				active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::local_factory_throughput, true);
			}
		} else {
			if(state.world.commodity_get_is_mine(com)) {
				if(bool(n)) {
					active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mine_rgo_eff, true);
				}
				if(bool(p)) {
					active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::mine_rgo_eff, true);
				}
				if(bool(n)) {
					active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mine_rgo_size, true);
				}
				if(bool(p)) {
					active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::mine_rgo_size, true);
				}
			} else {
				if(bool(n)) {
					active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::farm_rgo_eff, true);
				}
				if(bool(p)) {
					active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::farm_rgo_eff, true);
				}
				if(bool(n)) {
					active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::farm_rgo_size, true);
				}
				if(bool(p)) {
					active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::farm_rgo_size, true);
				}
			}
		}

		for(const auto tid : state.world.in_technology) {
			if(state.world.nation_get_active_technologies(n, tid)) {
				bool have_header = false;
				auto commodity_technology_mod_description = [&](auto const& list, std::string_view locale_base_name, std::string_view locale_farm_base_name) {
					for(const auto mod : list) {
						if(mod.type != com || mod.amount == 0.f)
							return;
						auto box = text::open_layout_box(contents, 0);
						if(!have_header) {
							text::add_to_layout_box(state, contents, box, state.world.technology_get_name(tid), text::text_color::yellow);
							text::add_line_break_to_layout_box(state, contents, box);
							have_header = true;
						}
						auto name = state.world.commodity_get_name(mod.type);
						text::add_to_layout_box(state, contents, box, name);
						text::add_space_to_layout_box(state, contents, box);
						text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_is_mine(mod.type) ? locale_base_name : locale_farm_base_name), text::text_color::white);
						text::add_to_layout_box(state, contents, box, std::string{ ":" }, text::text_color::white);
						text::add_space_to_layout_box(state, contents, box);
						auto color = mod.amount > 0.f ? text::text_color::green : text::text_color::red;
						text::add_to_layout_box(state, contents, box, (mod.amount > 0.f ? "+" : "") + text::format_percentage(mod.amount, 1), color);
						text::close_layout_box(contents, box);
					}
				};
				commodity_technology_mod_description(state.world.technology_get_factory_goods_output(tid), "tech_output", "tech_output");
				commodity_technology_mod_description(state.world.technology_get_rgo_goods_output(tid), "tech_mine_output", "tech_farm_output");
				commodity_technology_mod_description(state.world.technology_get_rgo_size(tid), "tech_mine_size", "tech_farm_size");
			}
		}
		for(const auto iid : state.world.in_invention) {
			if(state.world.nation_get_active_inventions(n, iid)) {
				bool have_header = false;
				auto commodity_invention_mod_description = [&](auto const& list, std::string_view locale_base_name, std::string_view locale_farm_base_name) {
					for(const auto mod : list) {
						if(mod.type != com || mod.amount == 0.f)
							return;
						auto box = text::open_layout_box(contents, 0);
						if(!have_header) {
							text::add_to_layout_box(state, contents, box, state.world.invention_get_name(iid), text::text_color::yellow);
							text::add_line_break_to_layout_box(state, contents, box);
							have_header = true;
						}
						auto name = state.world.commodity_get_name(mod.type);
						text::add_to_layout_box(state, contents, box, name);
						text::add_space_to_layout_box(state, contents, box);
						text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_is_mine(mod.type) ? locale_base_name : locale_farm_base_name), text::text_color::white);
						text::add_to_layout_box(state, contents, box, std::string{ ":" }, text::text_color::white);
						text::add_space_to_layout_box(state, contents, box);
						auto color = mod.amount > 0.f ? text::text_color::green : text::text_color::red;
						text::add_to_layout_box(state, contents, box, (mod.amount > 0.f ? "+" : "") + text::format_percentage(mod.amount, 1), color);
						text::close_layout_box(contents, box);
					}
				};
				commodity_invention_mod_description(state.world.invention_get_factory_goods_output(iid), "tech_output", "tech_output");
				commodity_invention_mod_description(state.world.invention_get_rgo_goods_output(iid), "tech_mine_output", "tech_farm_output");
				commodity_invention_mod_description(state.world.invention_get_rgo_size(iid), "tech_mine_size", "tech_farm_size");
			}
		}
	}
};

// -----------------------------------------------------------------------------
// National focuses

struct close_focus_window_notification { };

class national_focus_icon : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto nfid = retrieve<dcon::national_focus_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		disabled = !command::can_set_national_focus(state, state.local_player_nation, sid, nfid);
		frame = state.world.national_focus_get_icon(nfid) - 1;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::national_focus_id>(state, parent);
		if(bool(content)) {
			auto sid = retrieve<dcon::state_instance_id>(state, parent);
			auto fat_si = dcon::fatten(state.world, sid);
			auto fat_nf = dcon::fatten(state.world, content);

			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, sid);
			text::add_line_break_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, state.world.national_focus_get_name(content), text::substitution_map{});
			text::add_line_break_to_layout_box(state, contents, box);
			auto color = text::text_color::white;
			if(fat_nf.get_promotion_type()) {
				//Is the NF not optimal? Recolor it
				if(fat_nf.get_promotion_type() == state.culture_definitions.clergy) {
					if((fat_si.get_demographics(demographics::to_key(state, fat_nf.get_promotion_type())) / fat_si.get_demographics(demographics::total)) > state.defines.max_clergy_for_literacy) {
						color = text::text_color::red;
					}
				}
				auto full_str = text::format_percentage(fat_si.get_demographics(demographics::to_key(state, fat_nf.get_promotion_type())) / fat_si.get_demographics(demographics::total));
				text::add_to_layout_box(state, contents, box, std::string_view(full_str), color);
			}
			text::close_layout_box(contents, box);
			if(auto mid = state.world.national_focus_get_modifier(content);  mid) {
				modifier_description(state, contents, mid, 15);
			}
			auto state_cap = state.world.state_instance_get_capital(sid);
			ui::trigger_description(state, contents, state.world.national_focus_get_limit(content), trigger::to_generic(state_cap), trigger::to_generic(state.local_player_nation), -1);
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto nat_focus = retrieve<dcon::national_focus_id>(state, parent);
		command::set_national_focus(state, state.local_player_nation, content, nat_focus);
		send(state, parent, close_focus_window_notification{});
	}
};

class national_focus_item : public listbox_row_element_base<dcon::national_focus_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "focus_icon") {
			return make_element_by_type<national_focus_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

class national_focus_category_list : public overlapping_listbox_element_base<national_focus_item, dcon::national_focus_id> {
public:
	std::string_view get_row_element_name() override {
		return "focus_item";
	}
};

class national_focus_category : public window_element_base {
private:
	simple_text_element_base* category_label = nullptr;
	national_focus_category_list* focus_list = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			category_label = ptr.get();
			return ptr;
		} else if(name == "focus_icons") {
			auto ptr = make_element_by_type<national_focus_category_list>(state, id);
			focus_list = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<nations::focus_type>()) {
			auto category = any_cast<nations::focus_type>(payload);
			if(category_label)
				category_label->set_text(state, text::get_focus_category_name(state, category));

			if(focus_list) {
				focus_list->row_contents.clear();
				state.world.for_each_national_focus([&](dcon::national_focus_id focus_id) {
					auto fat_id = dcon::fatten(state.world, focus_id);
					if(fat_id.get_type() == uint8_t(category))
						focus_list->row_contents.push_back(focus_id);
				});
				focus_list->update(state);
			}
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class national_focus_remove_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		command::set_national_focus(state, state.local_player_nation, content, dcon::national_focus_id{});
		send(state, parent, close_focus_window_notification{});
	}
};

class national_focus_window : public window_element_base {
public:
	dcon::state_instance_id provided;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto start = make_element_by_type<window_element_base>(state, "focuscategory_start");
		auto current_pos = start->base_data.position;
		auto step = make_element_by_type<window_element_base>(state, "focuscategory_step");
		auto step_y = step->base_data.position.y;

		for(uint8_t i = 1; i <= uint8_t(nations::focus_type::party_loyalty_focus); i++) {
			auto ptr = make_element_by_type<national_focus_category>(state, "focus_category");
			ptr->base_data.position = current_pos;
			current_pos = xy_pair{current_pos.x, int16_t(current_pos.y + step_y)};

			Cyto::Any foc_type_payload = nations::focus_type(i);
			ptr->impl_set(state, foc_type_payload);

			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "cancel_button") {
			return make_element_by_type<national_focus_remove_button>(state, id);
		} else {
			return nullptr;
		}
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept  override {
		if(payload.holds_type<close_focus_window_notification>()) {
			set_visible(state, false);
			return message_result::consumed;
		}
		if(provided) {
			if(payload.holds_type<dcon::state_instance_id>()) {
				payload = provided;
				return message_result::consumed;
			} else if(payload.holds_type<dcon::nation_id>()) {
				payload = state.world.state_instance_get_nation_from_state_ownership(provided);
				return message_result::consumed;
			}
		}
		return window_element_base::get(state, payload);
	}
};

class wg_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<military::wg_summary>(state, parent).cb;
		frame = state.world.cb_type_get_sprite_index(content) - 1;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto wg = retrieve<military::wg_summary>(state, parent);
		text::add_line(state, contents, state.world.cb_type_get_name(wg.cb));

		text::add_line_break_to_layout(state, contents);
		if(wg.state) {
			text::add_line(state, contents, "war_goal_3", text::variable_type::x, wg.state);
		}
		if(wg.wg_tag) {
			text::add_line(state, contents, "war_goal_4", text::variable_type::x, wg.wg_tag);
		} else if(wg.secondary_nation) {
			text::add_line(state, contents, "war_goal_4", text::variable_type::x, wg.secondary_nation);
		}
	}
};

class overlapping_wg_icon : public listbox_row_element_base<military::wg_summary> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<wg_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

class full_wg_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<sys::full_wg>(state, parent).cb;
		frame = state.world.cb_type_get_sprite_index(content) - 1;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto wg = retrieve<sys::full_wg>(state, parent);
		text::add_line(state, contents, state.world.cb_type_get_name(wg.cb));

		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "war_goal_1", text::variable_type::x, wg.added_by);
		text::add_line(state, contents, "war_goal_2", text::variable_type::x, wg.target_nation);
		if(wg.state) {
			text::add_line(state, contents, "war_goal_3", text::variable_type::x, wg.state);
		}
		if(wg.wg_tag) {
			text::add_line(state, contents, "war_goal_4", text::variable_type::x, wg.wg_tag);
		} else if(wg.secondary_nation) {
			text::add_line(state, contents, "war_goal_4", text::variable_type::x, wg.secondary_nation);
		}
	}
};

class overlapping_full_wg_icon : public listbox_row_element_base<sys::full_wg> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<full_wg_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

class province_colony_button : public standard_state_instance_button {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		frame = 1;
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		disabled = !command::can_upgrade_colony_to_state(state, state.local_player_nation, content);
	}

	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		command::upgrade_colony_to_state(state, state.local_player_nation, content);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto state_instance_id = retrieve<dcon::state_instance_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("pw_colony"));
		text::add_divider_to_layout_box(state, contents, box);

		text::substitution_map sub1{};
		text::add_to_substitution_map(sub1, text::variable_type::num, text::fp_one_place{ state.defines.state_creation_admin_limit * 100.f });
		float total_pop = state.world.state_instance_get_demographics(state_instance_id, demographics::total);
		float b_size = province::state_accepted_bureaucrat_size(state, state_instance_id);
		text::add_to_substitution_map(sub1, text::variable_type::curr, text::fp_one_place{ (b_size / total_pop) * 100.f });
		text::localised_format_box(state, contents, box, std::string_view("pw_colony_no_state"), sub1);
		text::add_line_break_to_layout_box(state, contents, box);
		text::substitution_map sub2{};
		text::add_to_substitution_map(sub2, text::variable_type::value, int32_t(province::colony_integration_cost(state, state_instance_id)));
		text::localised_format_box(state, contents, box, std::string_view("pw_cant_upgrade_to_state"), sub2);

		text::close_layout_box(contents, box);
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

template<country_list_filter category>
class category_filter_button : public right_click_button_element_base {
public:
	void button_action(sys::state& state) noexcept final {
		send(state, parent, category);
		if constexpr(category == country_list_filter::all) {
			send(state, parent, dcon::modifier_id{});
		}
	}

	void button_right_action(sys::state& state) noexcept final {
		if constexpr(category == country_list_filter::allies) {
			send(state, parent, country_list_filter::find_allies);
		} else if constexpr(category == country_list_filter::sphere) {
			send(state, parent, country_list_filter::influenced);
		} else if constexpr(category == country_list_filter::neighbors) {
			send(state, parent, country_list_filter::neighbors_no_vassals);
		} else {
			send(state, parent, category);
			if constexpr(category == country_list_filter::all) {
				send(state, parent, dcon::modifier_id{});
			}
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto filter_settings = retrieve<country_filter_setting>(state, parent);
		auto t_category = filter_settings.general_category;
		switch(t_category) {
		case country_list_filter::influenced:
			t_category = country_list_filter::sphere;
			break;
		case country_list_filter::find_allies:
			t_category = country_list_filter::allies;
			break;
		case country_list_filter::neighbors_no_vassals:
			t_category = country_list_filter::neighbors;
			break;
		default:
			break;
		}
		disabled = t_category != category;
		button_element_base::render(state, x, y);
		disabled = false;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		switch(category) {
		case country_list_filter::all:
			text::add_line(state, contents, "filter_all");
			break;
		case country_list_filter::neighbors:
		case country_list_filter::neighbors_no_vassals:
			text::add_line(state, contents, "filter_neighbors");
			break;
		case country_list_filter::sphere:
		case country_list_filter::influenced:
			text::add_line(state, contents, "filter_sphere");
			break;
		case country_list_filter::enemies:
			text::add_line(state, contents, "filter_enemies");
			break;
		case country_list_filter::find_allies:
		case country_list_filter::allies:
			text::add_line(state, contents, "filter_allies");
			break;
		case country_list_filter::best_guess:
			text::add_line(state, contents, "filter_best_guess");
			break;
		default:
			break;
		}
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

class go_to_battleplanner_button : public button_element_base {
	void button_action(sys::state& state) noexcept final {
		game_scene::switch_scene(state, game_scene::scene_id::in_game_military);
	}
};

class go_to_battleplanner_selection_button : public button_element_base {
	void button_action(sys::state& state) noexcept final {
		game_scene::switch_scene(state, game_scene::scene_id::in_game_military_selector);
	}

	void on_update(sys::state& state) noexcept override {
		if(state.selected_army_group) {
			disabled = false;
		} else {
			disabled = true;
		}
	}
};

class go_to_base_game_button : public button_element_base {
	void button_action(sys::state& state) noexcept final {
		game_scene::switch_scene(state, game_scene::scene_id::in_game_basic);
	}
};

inline void province_owner_rgo_commodity_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov_id, dcon::commodity_id c) {
	auto rgo_good = dcon::fatten(state.world, c);
	auto nat_id = state.world.province_get_nation_from_province_ownership(prov_id);
	auto sid = state.world.province_get_state_membership(prov_id);
	auto market = state.world.state_instance_get_market_from_local_market(sid);
	auto mobilization_impact = (state.world.nation_get_is_mobilized(nat_id) ? military::mobilization_impact(state, nat_id) : 1.f);

	text::add_line(state, contents, "provinceview_goodsincome", text::variable_type::goods, rgo_good.get_name(), text::variable_type::value,
					text::fp_currency{ economy::rgo_income(state, prov_id) });

	text::add_line(state, contents, "PROVINCEVIEW_EMPLOYMENT", text::variable_type::value, text::fp_two_places{ economy::rgo_employment(state, rgo_good, prov_id) });
	text::add_line(state, contents, "provinceview_max_employment", text::variable_type::value, text::fp_two_places{ economy::rgo_max_employment(state, rgo_good, prov_id) });
	{
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub_map;
		auto const production = economy::rgo_output(state, rgo_good, prov_id);
		text::add_to_substitution_map(sub_map, text::variable_type::curr, text::fp_two_places{ production });
		text::localised_format_box(state, contents, box, std::string_view("production_output_goods_tooltip2"), sub_map);
		text::localised_format_box(state, contents, box, std::string_view("production_output_explanation"));
		text::close_layout_box(contents, box);
	}

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, std::string_view("production_base_output_goods_tooltip"), text::variable_type::base, text::fp_two_places{ economy::rgo_potential_output(state, rgo_good, prov_id) });

	{
		auto box = text::open_layout_box(contents, 0);
		bool const is_mine = state.world.commodity_get_is_mine(rgo_good);
		auto const efficiency = 1.0f +
			state.world.province_get_modifier_values(prov_id,
					is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff) +
			state.world.nation_get_modifier_values(nat_id,
					is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff);
		text::localised_format_box(state, contents, box, std::string_view("production_output_efficiency_tooltip"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ efficiency }, efficiency >= 0.0f ? text::text_color::green : text::text_color::red);
		text::close_layout_box(contents, box);
	}

	text::add_line_break_to_layout(state, contents);

	{
		auto box = text::open_layout_box(contents, 0);
		auto const throughput = 1.0f + state.world.province_get_modifier_values(prov_id, sys::provincial_mod_offsets::local_rgo_throughput) +
			state.world.nation_get_modifier_values(nat_id, sys::national_mod_offsets::rgo_throughput);
		text::localised_format_box(state, contents, box, std::string_view("production_throughput_efficiency_tooltip"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ throughput }, throughput >= 0.0f ? text::text_color::green : text::text_color::red);

		text::close_layout_box(contents, box);
	}

	auto inputs = economy::rgo_inputs_actual(state, nat_id, market, prov_id, c, mobilization_impact);
	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			if(i == 0) {
				text::add_line(state, contents, "province_rgo_efficiency_inputs");
			}
			auto input_c = dcon::fatten(state.world, inputs.commodity_type[i]);
			text::add_line(state, contents, "province_rgo_efficiency_input", text::variable_type::goods, input_c.get_name(), text::variable_type::value, text::fp_two_places{ inputs.commodity_amounts[i] });
		} else {
			break;
		}
	}
};


inline void factory_stats_tooltip(sys::state& state, text::columnar_layout& contents, dcon::factory_id fid) {
	auto p = state.world.factory_get_province_from_factory_location(fid);
	auto p_fat = fatten(state.world, p);
	auto n = p_fat.get_nation_from_province_ownership();
	auto sdef = state.world.abstract_state_membership_get_state(state.world.province_get_abstract_state_membership(p));
	dcon::state_instance_id s{};
	state.world.for_each_state_instance([&](dcon::state_instance_id id) {
		if(state.world.state_instance_get_definition(id) == sdef)
			s = id;
	});
	auto market = state.world.state_instance_get_market_from_local_market(s);

	// nation data

	float mobilization_impact = state.world.nation_get_is_mobilized(n) ? military::mobilization_impact(state, n) : 1.0f;
	auto cap_prov = state.world.nation_get_capital(n);
	auto cap_continent = state.world.province_get_continent(cap_prov);
	auto cap_region = state.world.province_get_connected_region_id(cap_prov);


	auto fac = fatten(state.world, fid);
	auto type = state.world.factory_get_building_type(fid);

	auto& inputs = type.get_inputs();
	auto& einputs = type.get_efficiency_inputs();

	//inputs
	auto inputs_data = economy::get_inputs_data(state, market, type.get_inputs());
	auto e_inputs_data = economy::get_inputs_data(state, market, type.get_efficiency_inputs());

	//modifiers

	float input_multiplier = economy::factory_input_multiplier(state, fac, n, p, s);
	float e_input_multiplier = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	float throughput_multiplier = economy::factory_throughput_multiplier(state, type, n, p, s, fac.get_size());
	float output_multiplier = economy::factory_output_multiplier_no_secondary_workers(state, fac, n, p);

	float bonus_profit_thanks_to_max_e_input = fac.get_building_type().get_output_amount()
		* 0.25f
		* throughput_multiplier
		* output_multiplier
		* economy::price(state, market, fac.get_building_type().get_output());

	// if efficiency inputs are not worth it, then do not buy them
	if(bonus_profit_thanks_to_max_e_input < e_inputs_data.total_cost * e_input_multiplier * input_multiplier)
		e_inputs_data.min_available = 0.f;

	float base_throughput =
		(
			state.world.factory_get_unqualified_employment(fac)
			* state.world.province_get_labor_demand_satisfaction(fac.get_province_from_factory_location(), economy::labor::no_education)
			* economy::unqualified_throughput_multiplier
			+
			state.world.factory_get_primary_employment(fac)
			* state.world.province_get_labor_demand_satisfaction(fac.get_province_from_factory_location(), economy::labor::basic_education)
		)
		* economy::factory_throughput_additional_multiplier(
			state,
			fac,
			mobilization_impact,
			false
		);

	float effective_production_scale = economy::factory_total_employment_score(state, fid);

	auto amount = (0.75f + 0.25f * e_inputs_data.min_available) * inputs_data.min_available * effective_production_scale;

	text::add_line(state, contents, state.world.factory_type_get_name(type));

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "factory_stats_1", text::variable_type::val, text::fp_percentage{ amount });

	text::add_line(state, contents, "factory_stats_2", text::variable_type::val,
			text::fp_percentage{ economy::factory_total_employment_score(state, fid) });

	text::add_line(state, contents, "factory_stats_3", text::variable_type::val,
			text::fp_one_place{ state.world.factory_get_output(fid) }, text::variable_type::x, type.get_output().get_name());

	auto profit_explantion = economy::explain_last_factory_profit(state, fid);

	text::add_line(state, contents, "factory_stats_4", text::variable_type::val,
		text::fp_currency{
			profit_explantion.profit
		}
	);

	{
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ -profit_explantion.inputs }, text::text_color::red);
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ -profit_explantion.maintenance }, text::text_color::red);
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ -profit_explantion.expansion }, text::text_color::red);
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ -profit_explantion.wages }, text::text_color::red);
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ profit_explantion.output }, text::text_color::green);
		text::close_layout_box(contents, box);
	}


	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "factory_stats_5");

	float total_expenses = 0.f;

	int position_demand_sat = 100;
	int position_amount = 180;
	int position_cost = 250;

	auto input_cost_line = [&](
		dcon::commodity_id cid,
		float base_amount
	) {
	auto box = text::open_layout_box(contents);
	text::layout_box name_entry = box;
	text::layout_box demand_satisfaction = box;
	text::layout_box amount_box = box;
	text::layout_box cost_box = box;

	demand_satisfaction.x_position += position_demand_sat;
	amount_box.x_position += position_amount;
	cost_box.x_position += position_cost;

	name_entry.x_size /= 10;

	std::string padding = cid.index() < 10 ? "0" : "";
	std::string description = "@$" + padding + std::to_string(cid.index());
	text::add_unparsed_text_to_layout_box(state, contents, name_entry, description);

	text::add_to_layout_box(state, contents, name_entry, state.world.commodity_get_name(cid));

	auto sat = state.world.market_get_demand_satisfaction(market, cid);
	text::add_to_layout_box(state, contents,
		demand_satisfaction,
		text::fp_percentage{ sat },
		sat >= 0.9f ? text::text_color::green : text::text_color::red
	);

	float amount =
		base_amount
		* input_multiplier
		* throughput_multiplier
		* inputs_data.min_available
		* effective_production_scale;

	float cost =
		economy::price(state, market, cid)
		* amount;

	total_expenses += cost;

	text::add_to_layout_box(state, contents, amount_box, text::fp_two_places{ amount });
	text::add_to_layout_box(state, contents, cost_box, text::fp_currency{ -cost }, text::text_color::red);

	text::add_to_layout_box(state, contents, box, std::string(" "));
	text::close_layout_box(contents, box);
		};

	auto e_input_cost_line = [&](
		dcon::commodity_id cid,
		float base_amount
	) {
	auto box = text::open_layout_box(contents);
	text::layout_box name_entry = box;
	text::layout_box demand_satisfaction = box;
	text::layout_box amount_box = box;
	text::layout_box cost_box = box;

	demand_satisfaction.x_position += position_demand_sat;
	amount_box.x_position += position_amount;
	cost_box.x_position += position_cost;

	name_entry.x_size /= 10;
	std::string padding = cid.index() < 10 ? "0" : "";
	std::string description = "@$" + padding + std::to_string(cid.index());
	text::add_unparsed_text_to_layout_box(state, contents, name_entry, description);
	text::add_to_layout_box(state, contents, name_entry, state.world.commodity_get_name(cid));

	auto sat = state.world.market_get_demand_satisfaction(market, cid);
	text::add_to_layout_box(state, contents,
		demand_satisfaction,
		text::fp_percentage{ sat },
		sat >= 0.9f ? text::text_color::green : text::text_color::red
	);

	float amount =
		base_amount
		* input_multiplier * e_input_multiplier
		* e_inputs_data.min_available
		* effective_production_scale;

	float cost =
		economy::price(state, market, cid)
		* amount;

	total_expenses += cost;

	text::add_to_layout_box(state, contents, amount_box, text::fp_two_places{ amount });
	text::add_to_layout_box(state, contents, cost_box, text::fp_currency{ -cost }, text::text_color::red);

	text::add_to_layout_box(state, contents, box, std::string(" "));
	text::close_layout_box(contents, box);
		};

	auto named_money_line = [&](
		std::string_view loc,
		float value
	) {
	auto box = text::open_layout_box(contents);
	text::layout_box name_entry = box;
	text::layout_box cost = box;

	cost.x_position += position_cost;
	name_entry.x_size /= 10;

	text::localised_format_box(state, contents, name_entry, loc);
	text::add_to_layout_box(state, contents, cost, text::fp_currency{ value }, value >= 0.f ? text::text_color::green : text::text_color::red);

	text::add_to_layout_box(state, contents, box, std::string(" "));
	text::close_layout_box(contents, box);
		};

	auto output_cost_line = [&](
		dcon::commodity_id cid,
		float base_amount
	) {
	auto box = text::open_layout_box(contents);
	text::layout_box name_entry = box;
	text::layout_box amount = box;
	text::layout_box cost = box;

	amount.x_position += position_amount;
	cost.x_position += position_cost;

	name_entry.x_size /= 10;

	std::string padding = cid.index() < 10 ? "0" : "";
	std::string description = "@$" + padding + std::to_string(cid.index());
	text::add_unparsed_text_to_layout_box(state, contents, name_entry, description);
	text::add_to_layout_box(state, contents, name_entry, state.world.commodity_get_name(cid));

	float output_amount =
		base_amount
		* (0.75f + 0.25f * e_inputs_data.min_available)
		* throughput_multiplier
		* output_multiplier
		* inputs_data.min_available
		* effective_production_scale;

	float output_cost =
		economy::price(state, market, cid)
		* output_amount;

	text::add_to_layout_box(state, contents, amount, text::fp_two_places{ output_amount });
	text::add_to_layout_box(state, contents, cost, text::fp_currency{ output_cost }, text::text_color::green);

	text::add_to_layout_box(state, contents, box, std::string(" "));
	text::close_layout_box(contents, box);
		};

	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_cost_line(inputs.commodity_type[i], inputs.commodity_amounts[i]);
		} else {
			break;
		}
	}

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "factory_stats_6");

	for(uint32_t i = 0; i < economy::small_commodity_set::set_size; ++i) {
		if(einputs.commodity_type[i]) {
			e_input_cost_line(einputs.commodity_type[i], einputs.commodity_amounts[i]);
		} else {
			break;
		}
	}

	text::add_line_break_to_layout(state, contents);

	auto const min_wage_factor = economy::pop_min_wage_factor(state, n);

	auto prov = fac.get_province_from_factory_location();

	auto wage_unqualified = state.world.province_get_labor_price(prov, economy::labor::no_education);
	auto wage_primary = state.world.province_get_labor_price(prov, economy::labor::basic_education);
	auto wage_secondary = state.world.province_get_labor_price(prov, economy::labor::high_education);

	float wage_estimation = 0.f;

	wage_estimation +=
		wage_unqualified
		* state.world.factory_get_unqualified_employment(fid)
		* state.world.province_get_labor_demand_satisfaction(prov, economy::labor::no_education);

	wage_estimation +=
		wage_primary
		* state.world.factory_get_primary_employment(fid)
		* state.world.province_get_labor_demand_satisfaction(prov, economy::labor::basic_education);

	wage_estimation +=
		wage_secondary
		* state.world.factory_get_secondary_employment(fid)
		* state.world.province_get_labor_demand_satisfaction(prov, economy::labor::high_education);

	total_expenses += wage_estimation;

	named_money_line("factory_stats_wage",
		-wage_estimation
	);

	text::add_line_break_to_layout(state, contents);

	named_money_line("factory_stats_expenses",
		-total_expenses
	);

	output_cost_line(type.get_output(), type.get_output_amount());

	float desired_income = total_expenses;

	named_money_line("factory_stats_desired_income",
		desired_income
	);

	text::add_line(state, contents, "factory_stats_7", text::variable_type::val, text::fp_percentage{ fac.get_size() / fac.get_building_type().get_base_workforce() / 100.f });
};


} // namespace ui
