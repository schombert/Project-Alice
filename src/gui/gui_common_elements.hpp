#pragma once

// BIG SCARY HEADER
// TODO: MOVE EVERYTHING POSSIBLE INTO CPP FILE

#include "dcon_generated_ids.hpp"
#include "demographics.hpp"
#include "gui_graphics.hpp"
#include "gui_element_types.hpp"
#include "military.hpp"
#include "nations.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "triggers.hpp"
#include <vector>
#include "province.hpp"

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

struct production_selection_wrapper {
	dcon::province_id data{};
	bool is_build = false;
	xy_pair focus_pos{ 0, 0 };
};

// Open Build new Factory window
void open_build_factory(sys::state& state, dcon::province_id pid);
// Open Build new Factory abroad (investment) window
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
	void on_update(sys::state& state) noexcept override;
};

class province_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};

class province_factory_count_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};

class standard_nation_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept;
	void on_update(sys::state& state) noexcept override;
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
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
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
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override;
};

class nation_ppp_gdp_per_capita_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override;
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

class province_population_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		set_text(state, text::prettify(int32_t(total_pop)));
	}
};





class province_state_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::get_province_state_name(state, province_id));
	}
};

class province_goods_produced_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class province_income_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class province_rgo_workers_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};


class factory_level_text : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};

class factory_profit_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};
class factory_income_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};
class factory_priority_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};


inline void commodity_tooltip(sys::state& state, text::columnar_layout& contents, dcon::commodity_id c) {
	auto box = text::open_layout_box(contents, 0);
	text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_name(c)), text::text_color::yellow);
	text::close_layout_box(contents, box);

	// Commodity in pop needs
	text::add_line_break_to_layout(state, contents);
	box = text::open_layout_box(contents, 0);
	text::localised_format_box(state, contents, box, std::string_view{ "commodity_consumption_header" });
	text::close_layout_box(contents, box);

	state.world.for_each_pop_type([&](dcon::pop_type_id pop_type) {
		auto life_base = state.world.pop_type_get_life_needs(pop_type, c);
		auto everyday_base = state.world.pop_type_get_everyday_needs(pop_type, c);
		auto luxury_base = state.world.pop_type_get_luxury_needs(pop_type, c);

		if(life_base > 0 || everyday_base > 0 || luxury_base > 0) {
			box = text::open_layout_box(contents, 15);
			text::localised_format_box(state, contents, box, std::string_view{ "commodity_consumed_by" });
			text::add_to_layout_box(state, contents, box, std::string_view{ " " });
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.pop_type_get_name(pop_type)), text::text_color::yellow);
			text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ life_base }, text::text_color::yellow);
			text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ everyday_base }, text::text_color::yellow);
			text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ luxury_base }, text::text_color::yellow);
			text::close_layout_box(contents, box);

		}
	});

	// Commodity in production chains
	text::add_line_break_to_layout(state, contents);
	box = text::open_layout_box(contents, 0);
	text::localised_format_box(state, contents, box, std::string_view{ "commodity_production_header" });
	text::close_layout_box(contents, box);

	for (auto fid : state.world.in_factory_type) {
		auto output = fid.get_output();
		if(output != c)
			continue;

		auto outputamount = 0.f;
		if (output == c)
			outputamount = fid.get_output_amount();

		auto inputamount = 0.f;
		auto& inputs = fid.get_inputs();

		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(inputs.commodity_type[i] == c) {
				inputamount = inputs.commodity_amounts[i];
				break;
			}
		}

		if(inputamount > 0 || outputamount > 0) {
			box = text::open_layout_box(contents, 15);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, fid.get_name()), text::text_color::yellow);
			text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ inputamount }, text::text_color::red);
			text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ outputamount }, text::text_color::green);
			text::close_layout_box(contents, box);
		}
	}
};

class commodity_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
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

class national_focus_category_list;

class national_focus_category : public window_element_base {
private:
	simple_text_element_base* category_label = nullptr;
	national_focus_category_list* focus_list = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override;
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

class show_all_saves_checkbox : public checkbox_button {
	void button_action(sys::state& state) noexcept override {
		state.user_settings.show_all_saves = !state.user_settings.show_all_saves;
		state.save_list_updated.store(true, std::memory_order::release); // update save list
		state.game_state_updated.store(true, std::memory_order::release); //update ui
		state.save_user_settings();
	}
	bool is_active(sys::state& state) noexcept override {
		return state.user_settings.show_all_saves;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, "alice_show_all_saves_tooltip");
		text::close_layout_box(contents, box);

	}

};


} // namespace ui
