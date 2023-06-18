#pragma once

#include "gui_production_window.hpp"
#include "gui_production_enum.hpp"
#include "gui_factory_buttons_window.hpp"

namespace ui {

class player_investement_text : public multiline_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto to_nation = retrieve<dcon::nation_id>(state, parent);

		auto ul = state.world.get_unilateral_relationship_by_unilateral_pair(to_nation, state.local_player_nation);
		auto player_investment = state.world.unilateral_relationship_get_foreign_investment(ul);

		float total_investment = 0.0f;
		for(auto oul : state.world.nation_get_unilateral_relationship_as_target(to_nation)) {
			total_investment += oul.get_foreign_investment();
		}

		auto container = text::create_endless_layout(multiline_text_element_base::internal_layout,
				text::layout_parameters{0, 0, multiline_text_element_base::base_data.size.x,
						multiline_text_element_base::base_data.size.y, multiline_text_element_base::base_data.data.text.font_handle, 0,
						text::alignment::left, text::text_color::black});

		text::add_line(state, container, "production_our_invest", text::variable_type::amount, text::pretty_integer{int64_t(player_investment)}, text::variable_type::percentage, text::fp_one_place{total_investment > 0 ? 100.0f * player_investment / total_investment : 0.0f});
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {

		auto to_nation = retrieve<dcon::nation_id>(state, parent);

		if(auto sl = state.world.nation_get_in_sphere_of(to_nation); sl) {

			auto ul = state.world.get_unilateral_relationship_by_unilateral_pair(to_nation, sl);
			auto sl_investment = state.world.unilateral_relationship_get_foreign_investment(ul);

			float total_investment = 0.0f;
			for(auto oul : state.world.nation_get_unilateral_relationship_as_target(to_nation)) {
				total_investment += oul.get_foreign_investment();
			}

			float base = state.world.nation_get_rank(to_nation) <= state.defines.colonial_rank
											 ? state.defines.second_rank_base_share_factor
											 : state.defines.civ_base_share_factor;
			float investment_fraction = total_investment > 0.0001f ? sl_investment / total_investment : 0.0f;
			auto share = base + (1.0f - base) * investment_fraction;

			if(!state.world.nation_get_is_civilized(to_nation)) {
				text::add_line(state, contents, "sphere_share_unciv", text::variable_type::percentage,
						int64_t(100.0f * state.defines.unciv_base_share_factor));
			} else if(state.world.nation_get_rank(to_nation) <= state.defines.colonial_rank) {
				text::add_line(state, contents, "sphere_share_2ndrank", text::variable_type::percentage, int64_t(100.0f * share),
						text::variable_type::base_percentage, int64_t(100.0f * state.defines.second_rank_base_share_factor));
			} else {
				text::add_line(state, contents, "sphere_share_civ", text::variable_type::percentage, int64_t(100.0f * share),
						text::variable_type::base_percentage, int64_t(100.0f * state.defines.civ_base_share_factor));
			}
		} else {
			text::add_line(state, contents, "sphere_share_not_in_sphere");
		}
	}
};

class close_investments : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		send(state, parent, production_window_tab::investments);
	}
};

class gp_investment_text : public simple_text_element_base {
public:
	uint16_t rank = 0;
	void on_update(sys::state& state) noexcept override {
		auto nat_id = nations::get_nth_great_power(state, rank);
		auto to_nation = retrieve<dcon::nation_id>(state, parent);

		auto ul = state.world.get_unilateral_relationship_by_unilateral_pair(to_nation, nat_id);
		auto investment = state.world.unilateral_relationship_get_foreign_investment(ul);

		set_text(state, text::format_money(investment));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		simple_text_element_base::render(state, x, y);
	}
};


class production_foreign_investment_window : public window_element_base {
public:
	dcon::nation_id curr_nation{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "prod_investment_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "country_flag_overlay") {
			return make_element_by_type<nation_flag_frame>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_investment") {
			return make_element_by_type<player_investement_text>(state, id);
		} else if(name == "invest_country_browse") {
			return make_element_by_type<close_investments>(state, id);
		} else if(name == "select_all") {
			return make_element_by_type<factory_select_all_button>(state, id);
		} else if(name == "deselect_all") {
			return make_element_by_type<factory_deselect_all_button>(state, id);
		} else if(name == "show_empty_states") {
			return make_element_by_type<factory_show_empty_states_button>(state, id);
		} else if(name == "sort_by_name") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "sort_by_factories") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "sort_by_infra") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "filter_bounds") {
			return make_element_by_type<commodity_filters_window>(state, id);
		} else {
			return nullptr;
		}
	}

	// 134 x 50
	// 450 x x-off
	// nation_gp_flag
	/*
	
			if(subwindow_width <= 0) {
				subwindow_width = ptr->base_data.size.x;
			}
			windows.push_back(static_cast<ItemWinT*>(ptr.get()));
			add_child_to_front(std::move(ptr));
	*/
	//"flag_list_flag";
	void on_create(sys::state& state) noexcept override {
		for(int32_t i = 0; i < 4; ++i) {
			{
				auto ptr = make_element_by_type<nation_gp_flag>(state, "flag_list_flag");
				static_cast<nation_gp_flag*>(ptr.get())->rank = uint16_t(i);
				ptr->base_data.position.x = int16_t(134 * i + 460);
				ptr->base_data.position.y = int16_t(15 + 55);
				add_child_to_back(std::move(ptr));
			}
			{
				auto ptr = make_element_by_type<gp_investment_text>(state, "alice_investment_text");
				static_cast<gp_investment_text*>(ptr.get())->rank = uint16_t(i);
				ptr->base_data.position.x = int16_t(134 * i + 460 + 25);
				ptr->base_data.position.y = int16_t(15 + 55);
				add_child_to_back(std::move(ptr));
			}
		}
		for(int32_t i = 4; i < 8; ++i) {
			{
				auto ptr = make_element_by_type<nation_gp_flag>(state, "flag_list_flag");
				static_cast<nation_gp_flag*>(ptr.get())->rank = uint16_t(i);
				ptr->base_data.position.x = int16_t(134 * (i - 4) + 460);
				ptr->base_data.position.y = int16_t(15 + 50 + 55);
				add_child_to_back(std::move(ptr));
			}
			{
				auto ptr = make_element_by_type<gp_investment_text>(state, "alice_investment_text");
				static_cast<gp_investment_text*>(ptr.get())->rank = uint16_t(i);
				ptr->base_data.position.x = int16_t(134 * (i - 4) + 460 + 25);
				ptr->base_data.position.y = int16_t(15 + 50 + 55);
				add_child_to_back(std::move(ptr));
			}
		}


		xy_pair base_sort_template_offset =
				state.ui_defs.gui[state.ui_state.defs_by_name.find("sort_by_pop_template_offset")->second.definition].position;
		xy_pair sort_template_offset = base_sort_template_offset;
		sort_template_offset.y += 233;

		auto ptr = make_element_by_type<button_element_base>(state,
				state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		ptr->set_button_text(state,
				text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.primary_factory_worker)));
		sort_template_offset.x = 478 + base_sort_template_offset.x * 0;
		ptr->base_data.position = sort_template_offset;
		add_child_to_back(std::move(ptr));

		auto ptr2 = make_element_by_type<button_element_base>(state,
				state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		ptr2->set_button_text(state,
				text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.secondary_factory_worker)));
		sort_template_offset.x = 478 + base_sort_template_offset.x * 1;
		ptr2->base_data.position = sort_template_offset;
		add_child_to_back(std::move(ptr2));

		auto ptr3 = make_element_by_type<button_element_base>(state,
				state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		ptr3->set_button_text(state,
				text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.capitalists)));
		sort_template_offset.x = 478 + base_sort_template_offset.x * 2;
		ptr3->base_data.position = sort_template_offset;
		add_child_to_back(std::move(ptr3));

		window_element_base::on_create(state);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::nation_id>>()) {
			curr_nation = any_cast<element_selection_wrapper<dcon::nation_id>>(payload).data;
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			payload = curr_nation;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
