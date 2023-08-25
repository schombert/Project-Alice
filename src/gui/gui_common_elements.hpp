#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "economy.hpp"
#include "gui_graphics.hpp"
#include "gui_element_types.hpp"
#include "military.hpp"
#include "nations.hpp"
#include "politics.hpp"
#include "province.hpp"
#include "rebels.hpp"
#include "system_state.hpp"
#include "text.hpp"
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

void sort_countries(sys::state& state, std::vector<dcon::nation_id>& list, country_list_sort sort, bool sort_ascend);

void open_build_foreign_factory(sys::state& state, dcon::state_instance_id st);
void open_foreign_investment(sys::state& state, dcon::nation_id n);

template<country_list_sort Sort>
class country_sort_button : public button_element_base {
public:
	uint8_t offset = 0;
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<country_list_sort>{country_list_sort(uint8_t(Sort) | offset)};
			parent->impl_get(state, payload);
		}
	}
};

class country_sort_by_player_investment : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<country_list_sort>{country_list_sort::player_investment};
			parent->impl_get(state, payload);
		}
	}
};

// Filters used on both production and diplomacy tabs for the country lists
enum class country_list_filter : uint8_t {
	all,
	neighbors,
	sphere,
	enemies,
	allies,
	deselect_all, // Used only by message filter window
	best_guess,		// Used only by message filter window
	continent
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
		auto container = text::create_endless_layout(multiline_text_element_base::internal_layout,
				text::layout_parameters{0, 0, multiline_text_element_base::base_data.size.x,
						multiline_text_element_base::base_data.size.y, multiline_text_element_base::base_data.data.text.font_handle, 0, text::alignment::left, color, false});
		auto fat_id = dcon::fatten(state.world, content);
		auto box = text::open_layout_box(container);
		text::add_to_layout_box(state, container, box, fat_id.get_name(), text::substitution_map{});
		text::close_layout_box(container, box);
	}
};

template<typename T>
class generic_simple_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, T content) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		T content = retrieve<T>(state, parent);
		simple_text_element_base::set_text(state, get_text(state, content));
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
		auto container = text::create_endless_layout(
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
		if(!state.user_settings.use_classic_fonts) {
			auto old_handle = base_data.data.text_common.font_handle;
			base_data.data.text_common.font_handle &= ~(0x01 << 7);
			auto old_value = base_data.data.text_common.font_handle & 0x3F;
			base_data.data.text_common.font_handle &= ~(0x003F);
			base_data.data.text_common.font_handle |= (old_value - 2);

			multiline_text_element_base::render(state, x, y);

			base_data.data.text_common.font_handle = old_handle;
		} else {
			multiline_text_element_base::render(state, x, y);
		}
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

		if(!state.user_settings.use_classic_fonts) {
			auto old_handle = base_data.data.text_common.font_handle;
			base_data.data.text_common.font_handle &= ~(0x01 << 7);
			auto old_value = base_data.data.text_common.font_handle & 0x3F;
			base_data.data.text_common.font_handle &= ~(0x003F);
			base_data.data.text_common.font_handle |= (old_value - 2);

			auto container = text::create_endless_layout(
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
		} else {
			auto container = text::create_endless_layout(
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
		auto container = text::create_endless_layout(
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

class state_name_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		return text::get_dynamic_state_name(state, content);
	}
};

class state_factory_count_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		uint8_t count = 0;
		province::for_each_province_in_state_instance(state, content, [&](dcon::province_id pid) {
			auto fat_id = dcon::fatten(state.world, pid);
			fat_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) { ++count; });
		});
		return std::to_string(count) + "/" + std::to_string(int32_t(state.defines.factories_per_state));
	}
};

class state_admin_efficiency_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		return text::format_percentage(province::state_admin_efficiency(state, content), 1);
	}
};

class state_aristocrat_presence_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		auto total_pop = state.world.state_instance_get_demographics(content, demographics::total);
		auto aristocrat_key = demographics::to_key(state, state.culture_definitions.aristocrat);
		auto aristocrat_amount = state.world.state_instance_get_demographics(content, aristocrat_key);
		return text::format_percentage(total_pop > 0 ? aristocrat_amount / total_pop : 0.0f, 1);
	}
};

class state_population_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		auto total_pop = state.world.state_instance_get_demographics(content, demographics::total);
		return text::prettify(int32_t(total_pop));
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

class movement_issue_name_text : public standard_movement_text {
public:
	std::string get_text(sys::state& state, dcon::movement_id movement_id) noexcept override {
		auto issue = state.world.movement_get_associated_issue_option(movement_id);
		return text::produce_simple_string(state, issue.get_movement_name());
	}
};

class standard_movement_multiline_text : public multiline_text_element_base {
public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents, dcon::movement_id movement_id) noexcept { }

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::movement_id>(state, parent);
		auto color = black_text ? text::text_color::black : text::text_color::white;
		auto container =
				text::create_endless_layout(internal_layout, text::layout_parameters{0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color, false});
		populate_layout(state, container, content);
	}
};

class movement_nationalist_name_text : public standard_movement_multiline_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::movement_id movement_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, movement_id);
		auto independence_target = fat_id.get_associated_independence();
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

class national_identity_vassal_type_text : public generic_simple_text<dcon::national_identity_id> {
public:
	std::string get_text(sys::state& state, dcon::national_identity_id content) noexcept override {
		// TODO: figure out if it should be a substate
		return text::produce_simple_string(state, "satellite");
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
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id sphereling_id = Cyto::any_cast<dcon::nation_id>(payload);
			auto ovr_id = state.world.nation_get_in_sphere_of(sphereling_id);
			return ovr_id.get_identity_from_identity_holder();
		}
		return dcon::national_identity_id{};
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

class nation_actual_stockpile_spending_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		float amount = state.world.nation_get_effective_construction_spending(nation_id);
		amount += state.world.nation_get_effective_land_spending(nation_id);
		amount += state.world.nation_get_effective_naval_spending(nation_id);
		return text::format_money(amount);
	}
};

class nation_tariff_percentage_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_percentage(state.world.nation_get_tariffs(nation_id));
	}
};

class nation_gold_income_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_money(economy::estimate_gold_income(state, nation_id));
	}
};

class nation_loan_spending_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_money(economy::estimate_loan_payments(state, nation_id));
	}
};

class nation_diplomatic_balance_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_money(economy::estimate_diplomatic_balance(state, nation_id));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);

		float w_subsidies_amount =
				economy::estimate_war_subsidies_income(state, n) - economy::estimate_war_subsidies_spending(state, n);
		float reparations_amount = economy::estimate_reparations_income(state, n) - economy::estimate_reparations_spending(state, n);

		if(w_subsidies_amount > 0.0f) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{w_subsidies_amount});
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "warsubsidies_income", m);
			text::close_layout_box(contents, box);
		} else if(w_subsidies_amount < 0.0f) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{w_subsidies_amount});
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "warsubsidies_expense", m);
			text::close_layout_box(contents, box);
		}

		if(reparations_amount > 0.0f) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{w_subsidies_amount});
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "warindemnities_income", m);
			text::close_layout_box(contents, box);
		} else if(reparations_amount < 0.0f) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{w_subsidies_amount});
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "warindemnities_expense", m);
			text::close_layout_box(contents, box);
		}
	}
};

class nation_subsidy_spending_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_money(economy::estimate_subsidy_spending(state, nation_id));
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
};

class nation_military_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(fat_id.get_military_score()));
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
		switch(nations::get_status(state, nation_id)) {
		case nations::status::great_power:
			return text::produce_simple_string(state, "diplomacy_greatnation_status");
		case nations::status::secondary_power:
			return text::produce_simple_string(state, "diplomacy_colonialnation_status");
		case nations::status::civilized:
			return text::produce_simple_string(state, "diplomacy_civilizednation_status");
		case nations::status::westernizing:
			return text::produce_simple_string(state, "diplomacy_almost_western_nation_status");
		case nations::status::uncivilized:
			return text::produce_simple_string(state, "diplomacy_uncivilizednation_status");
		case nations::status::primitive:
			return text::produce_simple_string(state, "diplomacy_primitivenation_status");
		default:
			return text::produce_simple_string(state, "diplomacy_greatnation_status");
		}
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
};

class nation_government_type_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		auto gov_type_id = fat_id.get_government_type();
		if(gov_type_id) {
			auto gov_name_seq = state.culture_definitions.governments[gov_type_id].name;
			return text::produce_simple_string(state, gov_name_seq);
		} else {
			return "";
		}
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
		} else {
			return text::produce_simple_string(state, "tb_tech_no_current");
		}
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

class national_tech_school : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto mod_id = state.world.nation_get_tech_school(nation_id);
		if(bool(mod_id)) {
			return text::produce_simple_string(state, state.world.modifier_get_name(mod_id));
		} else {
			return text::produce_simple_string(state, "traditional_academic");
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto mod_id = state.world.nation_get_tech_school(retrieve<dcon::nation_id>(state, parent));
		if(bool(mod_id)) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, state.world.modifier_get_name(mod_id), text::text_color::yellow);
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

class standard_nation_progress_bar : public progress_bar {
public:
	virtual float get_progress(sys::state& state, dcon::nation_id nation_id) noexcept {
		return 0.f;
	}

	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		progress = get_progress(state, nation_id);
	}
};

class nation_westernization_progress_bar : public standard_nation_progress_bar {
public:
	float get_progress(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return state.world.nation_get_modifier_values(nation_id, sys::national_mod_offsets::civilization_progress_modifier);
	}
};

class nation_technology_research_progress : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto tech_id = nations::current_research(state, nation_id);
		if(bool(tech_id)) {
			progress = state.world.nation_get_research_points(nation_id) / state.world.technology_get_cost(tech_id);
		} else {
			progress = 0.f;
		}
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::unseen;
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
		return int32_t(politics::get_military_reform_multiplier(state, nation_id) <= 0.f);
	}
};

class nation_economic_reform_multiplier_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(politics::get_economic_reform_multiplier(state, nation_id) <= 0.f);
	}
};

class nation_ruling_party_ideology_plupp : public tinted_image_element_base {
public:
	uint32_t get_tint_color(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto ruling_party = state.world.nation_get_ruling_party(nation_id);
		auto ideology = state.world.political_party_get_ideology(ruling_party);
		return state.world.ideology_get_color(ideology);
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
	uint32_t get_tint_color(sys::state& state) noexcept override {
		auto ideology_id = retrieve<dcon::ideology_id>(state, parent);
		return state.world.ideology_get_color(ideology_id);
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
		if(bool(name)) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, name);
			text::close_layout_box(contents, box);
		}
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
		if(bool(name)) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, name);
			text::close_layout_box(contents, box);
		}
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
	std::unordered_map<dcon::ideology_id::value_base_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<dcon::ideology_id::value_base_t, float> distrib = {};
		Cyto::Any nat_id_payload = dcon::nation_id{};
		if(parent) {
			parent->impl_get(state, nat_id_payload);
			if(nat_id_payload.holds_type<dcon::nation_id>()) {
				auto nat_id = any_cast<dcon::nation_id>(nat_id_payload);
				state.world.for_each_ideology([&](dcon::ideology_id ideo_id) {
					auto weight = .01f * state.world.nation_get_upper_house(nat_id, ideo_id);
					distrib[dcon::ideology_id::value_base_t(ideo_id.index())] = weight;
				});
			}
		}
		return distrib;
	}
};

class voter_ideology_piechart : public piechart<dcon::ideology_id> {
protected:
	std::unordered_map<dcon::ideology_id::value_base_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<dcon::ideology_id::value_base_t, float> distrib = {};
		Cyto::Any nat_id_payload = dcon::nation_id{};
		if(parent) {
			parent->impl_get(state, nat_id_payload);
			if(nat_id_payload.holds_type<dcon::nation_id>()) {
				auto nat_id = any_cast<dcon::nation_id>(nat_id_payload);
				auto total = politics::vote_total(state, nat_id);
				if(total <= 0.f) {
					enabled = false;
					return distrib;
				} else {
					enabled = true;
				}
				auto ideo_pool = std::vector<float>(state.world.ideology_size());
				state.world.for_each_province([&](dcon::province_id province) {
					if(nat_id == state.world.province_get_nation_from_province_ownership(province)) {
						for(auto pop_loc : state.world.province_get_pop_location(province)) {
							auto pop_id = pop_loc.get_pop();
							float vote_size = politics::pop_vote_weight(state, pop_id, nat_id);
							state.world.for_each_ideology([&](dcon::ideology_id iid) {
								auto dkey = pop_demographics::to_key(state, iid);
								ideo_pool[iid.index()] += state.world.pop_get_demographics(pop_id.id, dkey) * vote_size;
							});
						}
					}
				});
				for(size_t i = 0; i < ideo_pool.size(); i++) {
					distrib[dcon::ideology_id::value_base_t(i)] = ideo_pool[i] / total;
				}
			}
		}
		return distrib;
	}
};


class standard_province_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::province_id province_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class province_population_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::prettify(int32_t(total_pop));
	}
};

class province_militancy_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto militancy = state.world.province_get_demographics(province_id, demographics::militancy);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::format_float(militancy / total_pop);
	}
};

class province_consciousness_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto consciousness = state.world.province_get_demographics(province_id, demographics::consciousness);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::format_float(consciousness / total_pop);
	}
};

class province_literacy_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto literacy = state.world.province_get_demographics(province_id, demographics::literacy);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::format_percentage(literacy / total_pop, 1);
	}
};


class province_dominant_culture_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		dcon::culture_id last_culture{};
		float last_culture_amount = 0.f;
		state.world.for_each_culture([&](dcon::culture_id id) {
			auto amount = state.world.province_get_demographics(province_id, demographics::to_key(state, id));
			if(amount > last_culture_amount) {
				last_culture_amount = amount;
				last_culture = id;
			}
		});
		return text::get_name_as_string(state, dcon::fatten(state.world, last_culture));
	}
};
class province_dominant_religion_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		dcon::religion_id last_religion{};
		float last_religion_amount = 0.f;
		state.world.for_each_religion([&](dcon::religion_id id) {
			auto amount = state.world.province_get_demographics(province_id, demographics::to_key(state, id));
			if(amount > last_religion_amount) {
				last_religion_amount = amount;
				last_religion = id;
			}
		});
		return text::get_name_as_string(state, dcon::fatten(state.world, last_religion));
	}
};
class province_dominant_issue_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		dcon::issue_option_id last_issue{};
		float last_issue_amount = 0.f;
		state.world.for_each_issue_option([&](dcon::issue_option_id id) {
			auto amount = state.world.province_get_demographics(province_id, demographics::to_key(state, id));
			if(amount > last_issue_amount) {
				last_issue_amount = amount;
				last_issue = id;
			}
		});
		return text::get_name_as_string(state, dcon::fatten(state.world, last_issue));
	}
};
class province_dominant_ideology_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		dcon::ideology_id last_ideology{};
		float last_ideology_amount = 0.f;
		state.world.for_each_ideology([&](dcon::ideology_id id) {
			auto amount = state.world.province_get_demographics(province_id, demographics::to_key(state, id));
			if(amount > last_ideology_amount) {
				last_ideology_amount = amount;
				last_ideology = id;
			}
		});
		return text::get_name_as_string(state, dcon::fatten(state.world, last_ideology));
	}
};

class province_state_name_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::get_province_state_name(state, province_id);
	}
};

class province_rgo_name_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::get_name_as_string(state, state.world.province_get_rgo(province_id));
	}
};

class province_goods_produced_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_float(province::rgo_production_quantity(state, province_id), 3);
	}
};

class province_income_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_money(province::rgo_income(state, province_id));
	}
};

class province_rgo_workers_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::prettify(int32_t(province::rgo_employment(state, province_id)));
	}
};

class province_rgo_size_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_float(economy::rgo_effective_size(state, state.world.province_get_nation_from_province_ownership(province_id), province_id), 2);
	}
};

class standard_factory_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};
class factory_state_name_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		auto flid = state.world.factory_get_factory_location_as_factory(factory_id);
		auto pid = state.world.factory_location_get_province(flid);
		auto sdef = state.world.province_get_state_from_abstract_state_membership(pid);
		dcon::state_instance_id sid{};
		state.world.for_each_state_instance([&](dcon::state_instance_id id) {
			if(state.world.state_instance_get_definition(id) == sdef)
				sid = id;
		});
		return text::get_dynamic_state_name(state, sid);
	}
};
class factory_output_name_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		auto cid = state.world.factory_get_building_type(factory_id).get_output();
		return text::get_name_as_string(state, cid);
	}
};
class factory_produced_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		return text::format_float(state.world.factory_get_actual_production(factory_id), 2);
	}
};
class factory_income_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		return text::format_float(state.world.factory_get_full_profit(factory_id), 2);
	}
};
class factory_workers_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		return text::format_float(economy::factory_total_employment(state, factory_id), 2);
	}
};
class factory_level_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		return std::to_string(uint32_t(state.world.factory_get_level(factory_id)));
	}
};
class factory_profit_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_id>(state, parent);

		auto profit = state.world.factory_get_full_profit(content);
		bool is_positive = profit >= 0.f;
		auto text = (is_positive ? "+" : "") + text::format_float(profit, 2);
		// Create colour
		auto contents = text::create_endless_layout(internal_layout,
				text::layout_parameters{0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
						base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, true});
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text,
				is_positive ? text::text_color::dark_green : text::text_color::dark_red);
		text::close_layout_box(contents, box);
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
		text::add_line(state, contents, state.world.commodity_get_name(com));
	}
};

// -----------------------------------------------------------------------------
// National focuses
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
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, state.world.national_focus_get_name(content), text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto nat_focus = retrieve<dcon::national_focus_id>(state, parent);
		command::set_national_focus(state, state.local_player_nation, content, nat_focus);
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
			category_label->set_text(state, text::get_focus_category_name(state, category));

			focus_list->row_contents.clear();
			state.world.for_each_national_focus([&](dcon::national_focus_id focus_id) {
				auto fat_id = dcon::fatten(state.world, focus_id);
				if(fat_id.get_type() == uint8_t(category))
					focus_list->row_contents.push_back(focus_id);
			});
			focus_list->update(state);
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
	}
};

class national_focus_window : public window_element_base {
public:
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
		auto content = retrieve<military::full_wg>(state, parent).cb;
		frame = state.world.cb_type_get_sprite_index(content) - 1;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto wg = retrieve<military::full_wg>(state, parent);
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

class overlapping_full_wg_icon : public listbox_row_element_base<military::full_wg> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<full_wg_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

} // namespace ui
