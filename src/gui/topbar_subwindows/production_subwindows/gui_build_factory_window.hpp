#pragma once

#include "gui_element_types.hpp"
#include "gui_production_enum.hpp"
#include "ai_economy.hpp"
#include "triggers.hpp"
#include "construction.hpp"
#include "economy_government.hpp"
#include "economy_production.hpp"
#include "economy_factory_view.hpp"
#include "economy.hpp"

namespace ui {

struct factory_build_evaluation {
	float construction_cost = 0.0f;
	float input_cost = 0.0f;
	float output_value = 0.0f;
	float wage_cost = 0.0f;
	float net_profit = 0.0f;
	float profit_margin = 0.0f;
	float payback_days = 0.0f;
	float available_workers = 0.0f;
	float input_availability = 1.0f;
	bool constructable = false;
	bool recommended = false;
};

inline factory_build_evaluation evaluate_factory_build(
	sys::state& state,
	dcon::province_id pid,
	dcon::factory_type_id type,
	std::vector<dcon::factory_type_id> const& recommended_types
) {
	factory_build_evaluation result;
	auto const sid = state.world.province_get_state_membership(pid);
	auto const market = state.world.state_instance_get_market_from_local_market(sid);
	auto const owner = state.world.province_get_nation_from_province_ownership(pid);
	auto const factory_type = dcon::fatten(state.world, type);

	result.constructable = command::can_begin_factory_building_construction(
		state, state.local_player_nation, pid, type, false);
	result.recommended = std::find(recommended_types.begin(), recommended_types.end(), type) != recommended_types.end();

	auto const tax_efficiency = economy::tax_collection_rate(state, owner, pid);
	auto const retained_revenue = 1.0f - tax_efficiency * float(state.world.nation_get_rich_tax(owner)) / 100.0f;
	result.construction_cost = economy::factory_type_build_cost(state, owner, pid, type, false);
	result.input_cost = economy::factory_type_input_cost(state, owner, market, type);
	result.output_value = economy::factory_type_output_cost(state, owner, market, type) * retained_revenue;
	result.wage_cost = state.world.province_get_labor_price(pid, economy::labor::basic_education)
		* 2.0f * factory_type.get_base_workforce();
	result.net_profit = result.output_value - result.input_cost - result.wage_cost;
	auto const operating_cost = std::max(0.00001f, result.input_cost + result.wage_cost);
	result.profit_margin = result.net_profit / operating_cost;
	result.payback_days = result.net_profit > 0.00001f
		? result.construction_cost / result.net_profit
		: 0.0f;

	auto const uneducated_supply = state.world.province_get_labor_supply(pid, economy::labor::no_education);
	auto const educated_supply = state.world.province_get_labor_supply(pid, economy::labor::basic_education);
	result.available_workers =
		uneducated_supply * (1.0f - state.world.province_get_labor_supply_sold(pid, economy::labor::no_education))
		+ educated_supply * (1.0f - state.world.province_get_labor_supply_sold(pid, economy::labor::basic_education));

	auto const& inputs = state.world.factory_type_get_inputs(type);
	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i] && inputs.commodity_amounts[i] > 0.0f) {
			result.input_availability = std::min(result.input_availability,
				state.world.market_get_actual_probability_to_buy(market, inputs.commodity_type[i]));
		}
	}

	return result;
}

inline std::vector<dcon::factory_type_id> ranked_factory_builds(sys::state& state, dcon::province_id pid) {
	std::vector<dcon::factory_type_id> recommended_types;
	auto const sid = state.world.province_get_state_membership(pid);
	auto const market = state.world.state_instance_get_market_from_local_market(sid);
	if(economy::can_build_factory_in_colony(state, pid)) {
		ai::get_desired_factory_types(
			state, state.local_player_nation, market, pid, recommended_types, false);
	}

	std::vector<dcon::factory_type_id> types;
	for(auto type : state.world.in_factory_type) {
		types.push_back(type);
	}

	std::sort(types.begin(), types.end(), [&](auto a, auto b) {
		auto const ae = evaluate_factory_build(state, pid, a, recommended_types);
		auto const be = evaluate_factory_build(state, pid, b, recommended_types);
		if(ae.constructable != be.constructable)
			return ae.constructable > be.constructable;
		if(ae.recommended != be.recommended)
			return ae.recommended > be.recommended;
		if((ae.net_profit > 0.0f) != (be.net_profit > 0.0f))
			return ae.net_profit > 0.0f;
		if(ae.net_profit != be.net_profit)
			return ae.net_profit > be.net_profit;
		return a.index() < b.index();
	});

	return types;
}

inline void add_factory_build_recommendations(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::province_id pid,
	uint32_t limit = 3
) {
	if(!economy::can_build_factory_in_colony(state, pid))
		return;

	auto const ranked = ranked_factory_builds(state, pid);
	std::vector<dcon::factory_type_id> recommended_types;
	auto const sid = state.world.province_get_state_membership(pid);
	auto const market = state.world.state_instance_get_market_from_local_market(sid);
	ai::get_desired_factory_types(state, state.local_player_nation, market, pid, recommended_types, false);

	uint32_t shown = 0;
	for(auto type : ranked) {
		auto const evaluation = evaluate_factory_build(state, pid, type, recommended_types);
		if(!evaluation.constructable)
			continue;
		if(shown == 0) {
			text::add_line_break_to_layout(state, contents);
			text::add_line(state, contents, "alice_factory_best_options");
		}
		text::add_line(state, contents, "alice_factory_best_option",
			text::variable_type::name, state.world.factory_type_get_name(type),
			text::variable_type::value, text::fp_currency{ evaluation.net_profit },
			text::variable_type::x, text::fp_percentage_one_place{ evaluation.profit_margin });
		if(++shown >= limit)
			break;
	}
}

class factory_build_cancel_button : public generic_close_button {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "production_close_tooltip");
	}
};

class factory_build_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		disabled = !command::can_begin_factory_building_construction(state, state.local_player_nation, pid, content, false);
	}
	void button_action(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		command::begin_factory_building_construction(state, state.local_player_nation, pid, content, false);
		if(parent) parent->set_visible(state, false);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "shift_to_hold_open");

		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto type = retrieve<dcon::factory_type_id>(state, parent);

		/* If mod uses Factory Province limits */
		auto output = state.world.factory_type_get_output(type);
		if(state.world.commodity_get_uses_potentials(output)) {
			auto limit = economy::calculate_state_factory_limit(state, sid, output);
			text::add_line_with_condition(state, contents, "factory_build_condition_11", limit >= 1);
		}
	}
	void button_shift_action(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		command::begin_factory_building_construction(state, state.local_player_nation, pid, content, false);
	}
	void button_shift_right_action(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		auto n = retrieve<dcon::nation_id>(state, parent); //n may be another nation, i.e foreign investment
		for(const auto s : state.world.nation_get_state_ownership_as_nation(n)) {
			auto sid = s.get_state();
			if(command::can_begin_factory_building_construction(state, state.local_player_nation, pid, content, false)) {
				command::begin_factory_building_construction(state, state.local_player_nation, pid, content, false);
			}
		}
	}
};

class factory_build_output_name_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto name = fat.get_name();
		return text::produce_simple_string(state, name);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class factory_build_cost_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto& name = fat.get_construction_costs();

		auto s = retrieve<dcon::state_instance_id>(state, parent);
		auto pid = retrieve<dcon::province_id>(state, parent);
		float factor = economy::factory_build_cost_multiplier(state, state.local_player_nation, pid, false);

		auto total = 0.0f;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
			auto cid = name.commodity_type[i];
			if(bool(cid)) {
				total += economy::price(state, s, cid) * name.commodity_amounts[i] * factor;
			}
		} // Credit to leaf for this code :3
		return text::format_money(total);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		set_text(state, get_text(state, content));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto type = retrieve<dcon::factory_type_id>(state, parent);
		// Construction cost goods breakdown

		auto s = retrieve<dcon::state_instance_id>(state, parent);
		auto pid = retrieve<dcon::province_id>(state, parent);
		float factor = economy::factory_build_cost_multiplier(state, state.local_player_nation, pid, false);

		auto constr_cost = state.world.factory_type_get_construction_costs(type);

		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			auto box = text::open_layout_box(contents, 0);
			auto cid = constr_cost.commodity_type[i];

			if(!cid) {
				break;
			}
			std::string padding = cid.index() < 10 ? "0" : "";
			std::string description = "@$" + padding + std::to_string(cid.index());
			text::add_unparsed_text_to_layout_box(state, contents, box, description);
			text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(constr_cost.commodity_type[i]));
			text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
			text::add_to_layout_box(state, contents, box, text::fp_one_place{ constr_cost.commodity_amounts[i] * factor });
			text::close_layout_box(contents, box);
		}
	}
};

class factory_build_time_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto name = fat.get_construction_time();
		return (text::prettify(name) + "d");
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class factory_build_item_button : public tinted_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		auto pid = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_begin_factory_building_construction(state, state.local_player_nation, pid, content, false);
		if(retrieve<bool>(state, parent)) {
			color = sys::pack_color(196, 255, 196);
		} else {
			color = sys::pack_color(255, 255, 255);
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		send(state, parent, element_selection_wrapper<dcon::factory_type_id>{content});
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(retrieve<bool>(state, parent)) {
			text::add_line(state, contents, "alice_recommended_build");
		}

		auto content = dcon::fatten(state.world, retrieve<dcon::factory_type_id>(state, parent));

		text::add_line(state, contents, "factory_tier", text::variable_type::x, text::int_wholenum{ state.world.factory_type_get_factory_tier(content) });

		//
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto n = state.world.state_ownership_get_nation(state.world.state_instance_get_state_ownership(sid));
		//
		text::add_line(state, contents, "alice_factory_base_workforce", text::variable_type::x, state.world.factory_type_get_base_workforce(content));

		// List factory type inputs
		text::add_line(state, contents, "alice_factory_inputs");

		auto s = retrieve<dcon::state_instance_id>(state, parent);

		auto const& iset = state.world.factory_type_get_inputs(content);
		for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
			if(iset.commodity_type[i] && iset.commodity_amounts[i] > 0.0f) {
				auto amount = iset.commodity_amounts[i];
				auto cid = iset.commodity_type[i];
				auto price = economy::price(state, s, cid);

				auto box = text::open_layout_box(contents, 0);

				// Commodity icon
				std::string padding = cid.index() < 10 ? "0" : "";
				std::string description = "@$" + padding + std::to_string(cid.index());
				text::add_unparsed_text_to_layout_box(state, contents, box, description);
				// Text
				text::substitution_map m;
				text::add_to_substitution_map(m, text::variable_type::name, state.world.commodity_get_name(cid));
				text::add_to_substitution_map(m, text::variable_type::val, text::fp_currency{ price });
				text::add_to_substitution_map(m, text::variable_type::need, text::fp_four_places{ amount });
				text::add_to_substitution_map(m, text::variable_type::cost, text::fp_currency{ price * amount });
				text::localised_format_box(state, contents, box, "alice_factory_input_item", m);
				text::close_layout_box(contents, box);
			}
		}

		text::add_line_break_to_layout(state, contents);

		// List factory type construction costs
		text::add_line(state, contents, "alice_construction_cost");
		auto const& cset = state.world.factory_type_get_construction_costs(content);
		for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
			if(cset.commodity_type[i] && cset.commodity_amounts[i] > 0.0f) {
				auto amount = cset.commodity_amounts[i];
				auto cid = cset.commodity_type[i];
				auto price = economy::price(state, s, cid);

				// Commodity icon
				auto box = text::open_layout_box(contents, 0);
				std::string padding = cid.index() < 10 ? "0" : "";
				std::string description = "@$" + padding + std::to_string(cid.index());
				text::add_unparsed_text_to_layout_box(state, contents, box, description);

				text::substitution_map m;
				text::add_to_substitution_map(m, text::variable_type::name, state.world.commodity_get_name(cid));
				text::add_to_substitution_map(m, text::variable_type::val, text::fp_currency{ price });
				text::add_to_substitution_map(m, text::variable_type::need, text::fp_four_places{ amount });
				text::add_to_substitution_map(m, text::variable_type::cost, text::fp_currency{ price * amount });
				text::localised_format_box(state, contents, box, "alice_factory_input_item", m);
				text::close_layout_box(contents, box);
			}
		}
		/*text::add_line_break_to_layout(state, contents);
		// These bonuses are not applied in PA
		float sum = 0.f;
		if(auto b1 = state.world.factory_type_get_bonus_1_trigger(content); b1) {
			text::add_line(state, contents, "alice_factory_bonus", text::variable_type::x, text::fp_four_places{ state.world.factory_type_get_bonus_1_amount(content) });
			if(trigger::evaluate(state, b1, trigger::to_generic(sid), trigger::to_generic(n), 0)) {
				sum -= state.world.factory_type_get_bonus_1_amount(content);
			}
			ui::trigger_description(state, contents, b1, trigger::to_generic(sid), trigger::to_generic(n), 0);
		}
		if(auto b2 = state.world.factory_type_get_bonus_2_trigger(content); b2) {
			text::add_line(state, contents, "alice_factory_bonus", text::variable_type::x, text::fp_four_places{ state.world.factory_type_get_bonus_2_amount(content) });
			if(trigger::evaluate(state, b2, trigger::to_generic(sid), trigger::to_generic(n), 0)) {
				sum -= state.world.factory_type_get_bonus_2_amount(content);
			}
			ui::trigger_description(state, contents, b2, trigger::to_generic(sid), trigger::to_generic(n), 0);
		}
		if(auto b3 = state.world.factory_type_get_bonus_3_trigger(content); b3) {
			text::add_line(state, contents, "alice_factory_bonus", text::variable_type::x, text::fp_four_places{ state.world.factory_type_get_bonus_3_amount(content) });
			if(trigger::evaluate(state, b3, trigger::to_generic(sid), trigger::to_generic(n), 0)) {
				sum -= state.world.factory_type_get_bonus_3_amount(content);
			}
			ui::trigger_description(state, contents, b3, trigger::to_generic(sid), trigger::to_generic(n), 0);
		}
		text::add_line(state, contents, "alice_factory_total_bonus", text::variable_type::x, text::fp_four_places{ sum });*/

		/* If mod uses Factory Province limits */
		auto output = state.world.factory_type_get_output(content);
		if(state.world.commodity_get_uses_potentials(output)) {
			auto limit = economy::calculate_state_factory_limit(state, sid, output);

			text::add_line_with_condition(state, contents, "factory_build_condition_11", 1 <= limit);
		}

		text::add_line(state, contents, "alice_building_conditions");

		text::add_line_with_condition(state, contents, "nation_is_factory_type_active", state.world.nation_get_active_building(n, content) || state.world.factory_type_get_is_available_from_start(content), 15);

		auto p = state.world.state_instance_get_capital(sid);

		if(state.world.province_get_is_colonial(p)) {
			text::add_line_with_condition(state, contents, "nation_is_factory_type_colonies", state.world.factory_type_get_can_be_built_in_colonies(content), 15);
		}

		auto const tax_eff = economy::tax_collection_rate(state, n, p);

		auto mid = state.world.state_instance_get_market_from_local_market(sid);
		auto market_demand_satisfaction = state.world.market_get_expected_probability_to_sell(mid, output);

		auto wage = state.world.province_get_labor_price(p, economy::labor::basic_education) * 2.f;
		auto const rich_effect = (1.0f - tax_eff * float(state.world.nation_get_rich_tax(n)) / 100.0f);

		float cost = economy::factory_type_build_cost(state, n, p, content, false) + 0.1f;
		float output_value = economy::factory_type_output_cost(state, n, mid, content) * rich_effect;
		float input = economy::factory_type_input_cost(state, n, mid, content) + 0.1f;
		float profitability = (output_value - input - wage * content.get_base_workforce()) / input;
		float payback_time = cost / std::max(0.00001f, (output_value - input - wage * content.get_base_workforce()));

		text::add_line(state, contents, "construction_cost", text::variable_type::x, text::fp_currency{ cost });
		text::add_line(state, contents, "input_value", text::variable_type::x, text::fp_currency{ input });
		text::add_line(state, contents, "output_value", text::variable_type::x, text::fp_currency{ output_value });
		text::add_line(state, contents, "profitability", text::variable_type::x, text::fp_percentage_one_place{ profitability });
		text::add_line(state, contents, "payback_time", text::variable_type::x, text::fp_two_places{ payback_time });

		// Some extra outputs for AI debugging

		text::add_line(state, contents, "alice_pop_show_details");
		
		text::add_line_with_condition(state, contents, "province_has_workers", ai::province_has_workers(state, p));

		if(state.cheat_data.ui_debug_mode) {
			text::add_line(state, contents, "alice_building_id", text::variable_type::val, content.id.value);
			text::add_line(state, contents, "alice_province_id", text::variable_type::val, p.id.value);
		}
	}
};

class factory_build_item : public listbox_row_element_base<dcon::factory_type_id> {
	std::vector<dcon::factory_type_id> desired_types;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			auto ptr = make_element_by_type<factory_build_item_button>(state, id);
			ptr->base_data.size.x *= 2;	 // Nudge
			ptr->base_data.size.x += 42; // Nudge
			ptr->base_data.size.y += 5;	 // Nudge
			return ptr;

		} else if(name == "output") {
			return make_element_by_type<commodity_image>(state, id);

		} else if(name == "name") {
			return make_element_by_type<factory_build_output_name_text>(state, id);

		} else if(name == "total_build_cost") {
			return make_element_by_type<factory_build_cost_text>(state, id);

		} else if(name == "buildtime") {
			return make_element_by_type<factory_build_time_text>(state, id);

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		desired_types.clear();
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto m = state.world.state_instance_get_market_from_local_market(sid);
		ai::get_desired_factory_types(state, state.local_player_nation, m, pid, desired_types, false);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(dcon::fatten(state.world, content).get_output().id);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			auto sid = retrieve<dcon::state_instance_id>(state, parent);
			auto pid = retrieve<dcon::province_id>(state, parent);
			bool is_hl = std::find(desired_types.begin(), desired_types.end(), content) != desired_types.end();
			is_hl = is_hl && command::can_begin_factory_building_construction(state, state.local_player_nation, pid, content, false);
			payload.emplace<bool>(is_hl);
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::factory_type_id>::get(state, payload);
	}
};

class factory_build_list : public listbox_element_base<factory_build_item, dcon::factory_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "new_factory_option";
	}

public:
	void on_update(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		row_contents = ranked_factory_builds(state, pid);
		update(state);
	}
};

class factory_title : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id fid) noexcept {
		auto fat = dcon::fatten(state.world, fid);
		return text::produce_simple_string(state, fat.get_name());
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class needed_workers_count_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id fid) noexcept {
		auto fat = dcon::fatten(state.world, fid);
		return text::prettify(fat.get_base_workforce());
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class output_amount_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id fid) noexcept {
		auto fat = dcon::fatten(state.world, fid);
		return text::prettify(int64_t(fat.get_output_amount()));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class factory_current_funds_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state) noexcept {
		return text::format_money(nations::get_treasury(state, state.local_player_nation));
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}
};

class factory_build_description : public multiline_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		multiline_text_element_base::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_type_id>(state, parent);
		if(!content)
			return;
		auto fat = dcon::fatten(state.world, content);
		auto layout = text::create_endless_layout(state, internal_layout,
			text::layout_parameters{0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, false});

		auto const pid = retrieve<dcon::province_id>(state, parent);
		auto const sid = state.world.province_get_state_membership(pid);
		auto const market = state.world.state_instance_get_market_from_local_market(sid);
		std::vector<dcon::factory_type_id> recommended_types;
		if(economy::can_build_factory_in_colony(state, pid)) {
			ai::get_desired_factory_types(
				state, state.local_player_nation, market, pid, recommended_types, false);
		}
		auto const evaluation = evaluate_factory_build(state, pid, content, recommended_types);

		text::add_line(state, layout,
			evaluation.recommended ? "alice_factory_recommendation_positive" : "alice_factory_recommendation_neutral");
		text::add_line(state, layout, "alice_factory_estimated_profit",
			text::variable_type::value, text::fp_currency{ evaluation.net_profit },
			text::variable_type::x, text::fp_percentage_one_place{ evaluation.profit_margin });
		if(evaluation.payback_days > 0.0f) {
			text::add_line(state, layout, "alice_factory_estimated_payback",
				text::variable_type::x, text::fp_one_place{ evaluation.payback_days });
		}
		text::add_line(state, layout, "alice_factory_workers_available",
			text::variable_type::x, text::pretty_integer{ int64_t(evaluation.available_workers) },
			text::variable_type::value, text::pretty_integer{ int64_t(fat.get_base_workforce()) });
		text::add_line(state, layout, "alice_factory_input_availability",
			text::variable_type::x, text::fp_percentage_one_place{ evaluation.input_availability });
		text::add_line_break_to_layout(state, layout);

		auto box = text::open_layout_box(layout, 0);
		text::add_to_layout_box(state, layout, box, fat.get_description());
		text::close_layout_box(layout, box);
	}
};

class input_n_amout : public simple_text_element_base {
public:
	int32_t n = 0;

	void on_update(sys::state& state) noexcept override {
		auto type = retrieve<dcon::factory_type_id>(state, parent);
		if(!type) {
			set_text(state, "");
			return;
		}
		auto& inputs = state.world.factory_type_get_inputs(type);
		if(n < int32_t(economy::commodity_set::set_size)) {
			auto amount = inputs.commodity_amounts[n];
			if(amount > 0) {
				set_text(state, text::format_float(amount, 2));
			} else {
				set_text(state, "");
			}
		}
	}
};

class input_n_image : public image_element_base {
public:
	int32_t n = 0;
	dcon::commodity_id c;

	void on_update(sys::state& state) noexcept override {
		auto type = retrieve<dcon::factory_type_id>(state, parent);
		if(!type) {
			c = dcon::commodity_id{};
			return;
		}
		auto& inputs = state.world.factory_type_get_inputs(type);
		if(n < int32_t(economy::commodity_set::set_size)) {
			c = inputs.commodity_type[n];
			frame = int32_t(state.world.commodity_get_icon(c));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(c)
			text::add_line(state, contents, state.world.commodity_get_name(c));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(c)
			image_element_base::render(state, x, y);
	}
};

class factory_build_window : public window_element_base {
private:
	dcon::factory_type_id factory_to_build{};
	dcon::province_id focus_province{};

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "state_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "factory_type") {
			return make_element_by_type<factory_build_list>(state, id);
		} else if(name == "output") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "building_name") {
			return make_element_by_type<factory_title>(state, id);
		} else if(name == "output_amount") {
			return make_element_by_type<output_amount_text>(state, id);
		} else if(name == "description_text") {
			return make_element_by_type<factory_build_description>(state, id);
		} else if(name == "needed_workers") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "needed_workers_count") {
			return make_element_by_type<needed_workers_count_text>(state, id);
		} else if(name == "available_workers") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "current_funds_label") {
			return make_element_by_type<factory_current_funds_text>(state, id);
		} else if(name == "base_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "input_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "total_price") {
			return make_element_by_type<factory_build_cost_text>(state, id);
		} else if(name == "you_have") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "prod_cost") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "cancel") {
			return make_element_by_type<factory_build_cancel_button>(state, id);
		} else if(name == "build") {
			return make_element_by_type<factory_build_button>(state, id);
		} else if(name.substr(0, 6) == "input_" && parsers::is_integer(name.data() + 6, name.data() + name.length())) {
			auto ptr = make_element_by_type<input_n_image>(state, id);
			std::from_chars(name.data() + 6, name.data() + name.length(), ptr->n);
			return ptr;
			//input_0_amount
		} else if(name.substr(0, 6) == "input_" && name.substr(name.length() - 7) == "_amount" && parsers::is_integer(name.data() + 6, name.data() + name.length() - 7)) {
			auto ptr = make_element_by_type<input_n_amout>(state, id);
			std::from_chars(name.data() + 6, name.data() + name.length() - 7, ptr->n);
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::factory_type_id>>()) {
			factory_to_build = any_cast<element_selection_wrapper<dcon::factory_type_id>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::factory_type_id>()) {
			payload.emplace<dcon::factory_type_id>(factory_to_build);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(dcon::fatten(state.world, factory_to_build).get_output());
			return message_result::consumed;
		} else if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(focus_province);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::state_instance_id>()) {
			payload.emplace<dcon::state_instance_id>(state.world.province_get_state_membership(focus_province));
			return message_result::consumed;
		} else if(payload.holds_type<production_selection_wrapper>()) {
			auto data = any_cast<production_selection_wrapper>(payload);
			focus_province = data.data;
			factory_to_build = dcon::factory_type_id{};
			for(auto type : ranked_factory_builds(state, focus_province)) {
				if(command::can_begin_factory_building_construction(
					state, state.local_player_nation, focus_province, type, false)) {
					factory_to_build = type;
					break;
				}
			}
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
