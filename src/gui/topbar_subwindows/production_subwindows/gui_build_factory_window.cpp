#include "gui_build_factory_window.hpp"
#include "economy_government.hpp"
#include "ai_economy.hpp"

namespace ui {

	void factory_build_item_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "alice_building_conditions");

		// US53AC10 Tooltip for a factory type displays if the factory type has been activated with a technology

		text::add_line_with_condition(state, contents, "nation_is_factory_type_active", state.world.nation_get_active_building(n, content) || state.world.factory_type_get_is_available_from_start(content), 15);

		auto p = state.world.state_instance_get_capital(sid);

		// US53AC11 Tooltip for a factory type displays if the target state is a colony and target factory type can be built in colonies

		if(state.world.province_get_is_colonial(p)) {
			text::add_line_with_condition(state, contents, "nation_is_factory_type_colonies", state.world.factory_type_get_can_be_built_in_colonies(content), 15);
		}

		// US53AC12 Tooltip for a factory type displays if the factory type requires potentials and target state doesn't have required potentials

		/* If mod uses Factory Province limits */
		auto output = state.world.factory_type_get_output(content);
		if(state.world.commodity_get_uses_potentials(output)) {
			auto limit = economy::calculate_province_factory_limit(state, p, output);

			text::add_line_with_condition(state, contents, "factory_build_condition_11", 1 <= limit, 15);
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

		// US53AC13 Tooltip for a factory type displays key economic metrics of the potential construction: construction cost, input costs, output prices, profitability, payback type

		text::add_line(state, contents, "construction_cost", text::variable_type::value, text::fp_currency{ cost });
		text::add_line(state, contents, "input_value", text::variable_type::value, text::fp_currency{ input });
		text::add_line(state, contents, "output_value", text::variable_type::value, text::fp_currency{ output_value });
		text::add_line(state, contents, "profitability", text::variable_type::value, text::fp_percentage_one_place{ profitability });
		text::add_line(state, contents, "payback_time", text::variable_type::value, text::fp_two_places{ payback_time });

		// Some extra outputs for AI debugging

		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "alice_factory_type_score");

		// US53AC14 Tooltip for a factory type displays the score AI places to the factory type

		auto factory_type_score = ai::evaluate_factory_type(state, n, mid, p, content, false, 0.3f, 0.5f, 200.f, rich_effect);

		for(auto line : factory_type_score.getSteps()) {
			if(line.operation == sys::Operation::ADD || line.operation == sys::Operation::SUBTRACT) {
				text::add_line(state, contents, line.explanation, text::variable_type::value, text::fp_one_place{ line.value }, 15);
			} else if(line.operation == sys::Operation::DIVIDE) {
				text::add_line(state, contents, line.explanation, text::variable_type::value, text::fp_one_place{ 1 / line.value }, 15);
			} else {
				text::add_line(state, contents, line.explanation, text::variable_type::value, text::fp_one_place{ line.value }, 15);
			}
		}

		text::add_line(state, contents, "total_score", text::variable_type::value, text::fp_one_place{ factory_type_score.getResult() });

		text::add_line_with_condition(state, contents, "province_has_workers", ai::province_has_workers(state, p));
		text::add_line_with_condition(state, contents, "province_has_available_workers", ai::province_has_available_workers(state, p));
	}

}
