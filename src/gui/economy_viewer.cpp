#include "economy_viewer.hpp"

namespace economy_viewer {

enum class static_elements : int32_t {
	markets = 0,
	factory_types = 1000,
	factory_type_page = 1500,
	factory_type_page_number = 1600,
	factory_type_priority_button = 1700,
	factory_type_selector = 2000,
	commodities = 3000,
	commodities_inputs = 4000,
	commodities_inputs_amount = 5000,
	commodities_inputs_cost = 6000,
	commodities_e_inputs = 7000,
	commodities_e_inputs_amount = 8000,
	commodities_e_inputs_cost = 9000,


	commodities_output = 10000,
	commodities_output_amount = 11000,
	commodities_output_cost = 12000,

	commodities_button = 10000000,
	factory_types_button = 10000001,
	close_factory_type_description = 10000002,
	province_name = 10000003,
	state_def_name = 10000004,
	factory_type_name = 10000005,
	factory_type_total = 10000006,

	priority_text = 10000007,
	priority_text_national = 10000008,
	priority_text_private = 10000009,
	priority_text_bonus = 10000010,
	priority_value_national = 10000011,
	priority_value_private = 10000012,
	priority_value_bonus = 10000013,
};
void update(sys::state& state) {
	if(state.iui_state.per_market_data.size() != state.world.market_size()) {
		state.iui_state.per_market_data.resize(state.world.market_size());
	}

	if(state.selected_factory_type) {
		auto& inputs = state.world.factory_type_get_inputs(state.selected_factory_type);
		auto& e_inputs = state.world.factory_type_get_efficiency_inputs(state.selected_factory_type);

		state.world.for_each_market([&](auto market) {
			auto ftid = state.selected_factory_type;
			auto sid = state.world.market_get_zone_from_local_market(market);
			auto sdif = state.world.state_instance_get_definition(sid);
			auto nid = state.world.state_instance_get_nation_from_state_ownership(sid);
			auto mid = market;

			auto e_input_mult = state.world.nation_get_modifier_values(nid, sys::national_mod_offsets::factory_maintenance) + 1.0f;
			auto input_mult = economy::nation_factory_input_multiplier(state, ftid, nid);
			auto output_mult = economy::nation_factory_output_multiplier(state, ftid, nid);

			float total = 0.f;
			for(auto i = 0; i < inputs.set_size; i++) {
				if(inputs.commodity_type[i]) {
					auto cid = inputs.commodity_type[i];
					total -= inputs.commodity_amounts[i] * input_mult * state.world.market_get_price(mid, cid);
				}
			}
			for(auto i = 0; i < e_inputs.set_size; i++) {
				if(e_inputs.commodity_type[i]) {
					auto cid = e_inputs.commodity_type[i];
					total -= e_inputs.commodity_amounts[i] * input_mult * e_input_mult * state.world.market_get_price(mid, cid);
				}
			}

			auto cid = state.world.factory_type_get_output(ftid).id;
			total += state.world.factory_type_get_output_amount(ftid) * output_mult * state.world.market_get_price(mid, cid);
			state.iui_state.per_market_data[market.index()] = total;
		});
	}
}

void commodity_panel(sys::state& state, int32_t identifier_amount, int32_t identifier_cost, dcon::commodity_id cid, float amount, float price, iui::rect& r) {
	state.iui_state.panel(state, r);
	auto icon_size = 20.f;
	ogl::render_commodity_icon(
		state, cid,
		r.x + 5.f, r.y + r.h / 2.f - icon_size / 2.f, icon_size, icon_size
	);
	iui::rect data{ r.x + 15.f, r.y + 4.f, r.w - 18.f, r.h / 2.f };
	state.iui_state.float_2(
		state, identifier_amount,
		data, amount
	);
	data.y += r.h / 2.f;
	state.iui_state.price(
		state, identifier_cost,
		data, amount * price
	);
}

void render(sys::state& state) {
	if(state.iui_state.per_market_data.size() != state.world.market_size()) {
		state.iui_state.per_market_data.resize(state.world.market_size());
	}

	state.iui_state.frame_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(state.open_gl.ui_shader_program);
	glUniform1i(state.open_gl.ui_shader_texture_sampler_uniform, 0);
	glUniform1i(state.open_gl.ui_shader_secondary_texture_sampler_uniform, 1);
	glUniform1f(state.open_gl.ui_shader_screen_width_uniform, float(state.x_size) / state.user_settings.ui_scale);
	glUniform1f(state.open_gl.ui_shader_screen_height_uniform, float(state.y_size) / state.user_settings.ui_scale);
	glUniform1f(state.open_gl.ui_shader_gamma_uniform, state.user_settings.gamma);
	glViewport(0, 0, state.x_size, state.y_size);
	glDepthRange(-1.0f, 1.0f);

	auto zoom = state.map_state.zoom;
	auto screen_size = glm::vec2(state.x_size, state.y_size);

	// render market data
	if(zoom > map::zoom_close) {
		

		if(state.iui_state.current_font == 0) {
			state.iui_state.current_font = text::name_into_font_id(state, "garamond_16");
		}

		iui::rect market_label_rect{ 0.f, 0.f, 80.f, 30.f };
		iui::rect market_label_rect_text{ 0.f, 0.f, 70.f, 20.f };

		state.world.for_each_market([&](dcon::market_id mid) {
			auto sid = state.world.market_get_zone_from_local_market(mid);
			auto capital = state.world.state_instance_get_capital(sid);
			auto& midpoint = state.world.province_get_mid_point(capital);
			auto map_pos = state.map_state.normalize_map_coord(midpoint);
			glm::vec2 screen_pos{};
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				return;
			}

			if(screen_pos.x < -100.f) {
				return;
			}
			if(screen_pos.y < -100.f) {
				return;
			}
			if(screen_pos.x > screen_size.x + 100.f) {
				return;
			}
			if(screen_pos.y > screen_size.y + 100.f) {
				return;
			}

			iui::move_to(market_label_rect, screen_pos.x - 40.f, screen_pos.y - 15.f);
			iui::move_to(market_label_rect_text, screen_pos.x - 40.f + 5.f, screen_pos.y - 15.f + 5.f);

			state.iui_state.panel(state, market_label_rect);

			if(state.selected_factory_type) {
				if(mid.index() < (int32_t)(state.iui_state.per_market_data.size())) {
					state.iui_state.price(
						state, mid.index(),
						market_label_rect_text,
						state.iui_state.per_market_data[mid.index()]
					);
				}
			} else {
				state.iui_state.float_2(
					state, mid.index(),
					market_label_rect_text,
					state.world.market_get_max_throughput(mid)
				);
			}
		});
	}

	{
		float tabs_layout_start = 200.f;
		float tabs_layout_margin = 50.f;
		float tab_height = 60.f;
		float tab_width = 120.f;

		iui::rect tab_rect{ tabs_layout_start, tabs_layout_margin, tab_height, tab_width };

		if(state.iui_state.button(
			state, (int32_t)(static_elements::commodities_button),
			tab_rect,
			state.iui_state.tab == iui::iui_tab::commodities_markets
		)) {
			state.iui_state.tab = iui::iui_tab::commodities_markets;
		}

		tab_rect.x += tab_width + tabs_layout_margin;

		if(state.iui_state.button(
			state, (int32_t)(static_elements::factory_types_button),
			tab_rect,
			state.iui_state.tab == iui::iui_tab::factory_types
		)) {
			state.iui_state.tab = iui::iui_tab::factory_types;
		}
	}

	if(state.iui_state.tab == iui::iui_tab::commodities_markets) {
	} else if(state.iui_state.tab == iui::iui_tab::factory_types) {

		// absolute
		float size_panel_w = 400.f;
		float size_panel_h = 500.f;
		float margin = 5.f;
		float size_selector_w = 150.f;
		float size_page_selector_h = 40.f;
		float page_selector_item_size_h = 25.f;
		float page_selector_item_size_w = 25.f;
		float top = screen_size.y - size_panel_h;
		float priority_settings_height = 120.f;

		float priority_button_height = 15.f;
		float priority_button_width = 25.f;

		// calculated
		float size_view_w = size_panel_w - size_selector_w;
		iui::rect selector_panel = { margin, top, size_selector_w, size_panel_h - size_page_selector_h };
		iui::rect page_selector_rect = { margin, top + size_panel_h - size_page_selector_h, size_selector_w, size_page_selector_h };
		iui::rect view_panel = { margin + size_selector_w, top, size_view_w, size_panel_h };

		iui::shrink(selector_panel, margin);
		iui::shrink(page_selector_rect, margin);
		iui::shrink(view_panel, margin);

		uint32_t factories_on_a_page = 8;
		iui::rect selector_button = selector_panel;
		selector_button.h = selector_panel.h / float(factories_on_a_page);
		auto page = state.iui_state.page_production_methods;

		{
			state.iui_state.panel(state, selector_panel);
			float y = 10.f;
			for(
				uint32_t i = 0;
				i < factories_on_a_page;
				i++
			) {
				auto factory_index = i + factories_on_a_page * page;
				if(factory_index >= state.world.factory_type_size()) {
					break;
				}
				dcon::factory_type_id ftid{ (dcon::factory_type_id::value_base_t)factory_index };
				selector_button.y = selector_panel.y + i * selector_button.h;
				if(state.iui_state.button(
					state, (int32_t)static_elements::factory_type_selector + ftid.index(),
					selector_button, state.selected_factory_type == ftid
				)) {
					if(state.selected_factory_type == ftid) {
						state.selected_factory_type = { };
					} else {
						state.selected_factory_type = ftid;
					}
				}
				state.iui_state.localized_string_r(
					state, (int32_t)static_elements::factory_types + ftid.index(),
					{ selector_button.x + 30.f, selector_button.y, selector_button.w - 30.f, selector_button.h },
					text::produce_simple_string(state, state.world.factory_type_get_name(ftid))
				);
				ogl::render_commodity_icon(
					state, state.world.factory_type_get_output(ftid),
					selector_button.x + 5.f, selector_button.y + 5.f, 20.f, 20.f
				);
			}

			state.iui_state.panel(state, page_selector_rect);
			int pages = (state.world.factory_type_size() - 1) / factories_on_a_page + 1;

			iui::rect page_button{
				0.f,
				page_selector_rect.y - page_selector_item_size_h / 2.f + page_selector_rect.h / 2.f,
				page_selector_item_size_w,
				page_selector_item_size_h
			};

			if(pages * page_selector_item_size_w <= page_selector_rect.w) {
				float margin_pages = (page_selector_rect.w - pages * page_selector_item_size_w) / (pages + 1);

				for(int i = 0; i < pages; i++) {
					page_button.x = page_selector_rect.x + (page_button.w + margin_pages) * i + margin_pages;

					if(state.iui_state.button(
						state, (int32_t)static_elements::factory_type_page + i,
						page_button, state.iui_state.page_production_methods == i
					)) {
						state.iui_state.page_production_methods = i;
					}

					state.iui_state.int_whole(
						state,
						(int32_t)static_elements::factory_type_page_number + i,
						page_button, i
					);
				}
			} else {
				page_button.x = page_selector_rect.x + margin;
				if(state.iui_state.button(
					state, (int32_t)static_elements::factory_type_page,
					page_button, false
				)) {
					state.iui_state.page_production_methods = std::max(0, state.iui_state.page_production_methods - 1);
				}

				page_button.x = page_selector_rect.x + page_selector_rect.w - page_button.w - margin;
				if(state.iui_state.button(
					state, (int32_t)static_elements::factory_type_page + 2,
					page_button, false
				)) {
					state.iui_state.page_production_methods = std::min(
						pages - 1, state.iui_state.page_production_methods + 1
					);
				}

				iui::rect page_selector_page_current = page_selector_rect;
				page_selector_page_current.x = page_selector_rect.x + page_selector_rect.w / 4.f;
				page_selector_page_current.w = page_selector_rect.w / 2.f;
				page_selector_page_current.y = page_selector_rect.y + margin;
				page_selector_page_current.h = page_selector_rect.h - margin * 2.f;

				page_selector_page_current.w /= 2.f;
				state.iui_state.int_whole(
					state,
					(int32_t)static_elements::factory_type_page_number + 1,
					page_selector_page_current, state.iui_state.page_production_methods
				);

				page_selector_page_current.x += page_selector_page_current.w;
				state.iui_state.int_whole(
					state,
					(int32_t)static_elements::factory_type_page_number + 1,
					page_selector_page_current, pages
				);
			}
		}

		if(state.selected_factory_type && state.map_state.selected_province) {
			auto ftid = state.selected_factory_type;
			auto sid = state.world.province_get_state_membership(state.map_state.selected_province);
			auto sdif = state.world.state_instance_get_definition(sid);
			auto nid = state.world.province_get_nation_from_province_ownership(state.map_state.selected_province);
			auto mid = state.world.state_instance_get_market_from_local_market(sid);

			auto e_input_mult = state.world.nation_get_modifier_values(nid, sys::national_mod_offsets::factory_maintenance) + 1.0f;
			auto input_mult = economy::nation_factory_input_multiplier(state, ftid, nid);
			auto output_mult = economy::nation_factory_output_multiplier(state, ftid, nid);

			state.iui_state.panel(state, view_panel);

			state.iui_state.localized_string_r(
				state,
				(int32_t)static_elements::state_def_name,
				{ view_panel.x + 5.f, view_panel.y + 5.f, view_panel.w, view_panel.h },
				text::produce_simple_string(state, state.world.state_definition_get_name(sdif))
			);

			state.iui_state.localized_string_r(
				state,
				(int32_t)static_elements::factory_type_name,
				{ view_panel.x + 5.f, view_panel.y + 25.f, view_panel.w, view_panel.h },
				text::produce_simple_string(state, state.world.factory_type_get_name(ftid))
			);

			iui::rect close{ view_panel.x + view_panel.w - 20.f, view_panel.y, 20.f, 20.f };
			if(state.iui_state.button(state, (int32_t)static_elements::close_factory_type_description, close, false)) {
				state.selected_factory_type = { };
			}

			//retrieve data
			auto& inputs = state.world.factory_type_get_inputs(ftid);
			int inputs_size = 0;
			for(auto i = 0; i < inputs.set_size; i++) {
				if(inputs.commodity_type[i]) {
					inputs_size++;
				}
			}
			auto& e_inputs = state.world.factory_type_get_efficiency_inputs(ftid);
			int e_inputs_size = 0;
			for(auto i = 0; i < e_inputs.set_size; i++) {
				if(e_inputs.commodity_type[i]) {
					e_inputs_size++;
				}
			}

			float grid_margin = 5.f;
			int items_per_row = 3;

			iui::rect commodity_rect{ 0.f, 0.f, (view_panel.w - grid_margin) / float(items_per_row) - grid_margin, 40.f };

			int rows_input = ((inputs_size - 1) / items_per_row) + 1;
			int rows_e_input = ((e_inputs_size - 1) / items_per_row) + 1;

			// data about production method
			{
				// inputs
				float total = 0.f;

				float current_y = view_panel.y + 40.f;

				for(auto i = 0; i < inputs.set_size; i++) {
					if(inputs.commodity_type[i]) {
						auto cid = inputs.commodity_type[i];

						int row = i / items_per_row;
						int column = i - row * items_per_row;
						commodity_rect.x = view_panel.x + grid_margin + column * (grid_margin + commodity_rect.w);
						commodity_rect.y = current_y + grid_margin + row * (grid_margin + commodity_rect.h);

						commodity_panel(
							state,
							(int32_t)static_elements::commodities_inputs_amount + cid.index(),
							(int32_t)static_elements::commodities_inputs_cost + cid.index(),
							cid,
							inputs.commodity_amounts[i] * input_mult,
							state.world.market_get_price(mid, cid),
							commodity_rect
						);

						total -= inputs.commodity_amounts[i] * input_mult * state.world.market_get_price(mid, cid);
					}
				}

				current_y += grid_margin + rows_input * (grid_margin + commodity_rect.h) + 10.f;

				// maintenance

				for(auto i = 0; i < e_inputs.set_size; i++) {
					if(e_inputs.commodity_type[i]) {
						auto cid = e_inputs.commodity_type[i];

						int row = i / items_per_row;
						int column = i - row * items_per_row;
						commodity_rect.x = view_panel.x + grid_margin + column * (grid_margin + commodity_rect.w);
						commodity_rect.y = current_y + grid_margin + row * (grid_margin + commodity_rect.h);

						commodity_panel(
							state,
							(int32_t)static_elements::commodities_e_inputs_amount + cid.index(),
							(int32_t)static_elements::commodities_e_inputs_cost + cid.index(),
							cid,
							e_inputs.commodity_amounts[i] * input_mult * e_input_mult,
							state.world.market_get_price(mid, cid),
							commodity_rect
						);

						total -= e_inputs.commodity_amounts[i] * input_mult * e_input_mult * state.world.market_get_price(mid, cid);
					}
				}

				current_y += grid_margin + rows_e_input * (grid_margin + commodity_rect.h) + 10.f;

				commodity_rect.x = view_panel.x + grid_margin;
				commodity_rect.y = current_y;

				// output
				auto cid = state.world.factory_type_get_output(ftid).id;

				commodity_panel(
					state,
					(int32_t)static_elements::commodities_e_inputs_amount + cid.index(),
					(int32_t)static_elements::commodities_e_inputs_cost + cid.index(),
					cid,
					state.world.factory_type_get_output_amount(ftid) * output_mult,
					state.world.market_get_price(mid, cid),
					commodity_rect
				);

				commodity_rect.x += commodity_rect.w + grid_margin;
				total += state.world.factory_type_get_output_amount(ftid) * output_mult * state.world.market_get_price(mid, cid);

				state.iui_state.price(
					state, (int32_t)static_elements::factory_type_total,
					commodity_rect, total
				);


				iui::rect priority_settings{ view_panel.x, view_panel.y + view_panel.h - priority_settings_height, view_panel.w, priority_settings_height };
				iui::shrink(priority_settings, margin);
				state.iui_state.panel(state, priority_settings);
				iui::shrink(priority_settings, margin);

				iui::rect priority_description{ priority_settings.x, priority_settings.y, priority_settings.w, 20.f };

				state.iui_state.localized_string(
					state,
					(int32_t)static_elements::priority_text,
					priority_description,
					text::produce_simple_string(state, "alice_factory_type_viewer_priority")
				);

				priority_description.y += 20.f;
				state.iui_state.localized_string(
					state,
					(int32_t)static_elements::priority_text_national,
					priority_description,
					text::produce_simple_string(state, "alice_factory_type_viewer_priority_national")
				);
				state.iui_state.float_2(
					state,
					(int32_t)static_elements::priority_value_national,
					priority_description, nations::priority_national(state, nid, ftid)
				);

				priority_description.y += 20.f;
				state.iui_state.localized_string(
					state,
					(int32_t)static_elements::priority_text_private,
					priority_description,
					text::produce_simple_string(state, "alice_factory_type_viewer_priority_private")
				);
				state.iui_state.float_2(
					state,
					(int32_t)static_elements::priority_value_private,
					priority_description, nations::priority_private(state, nid, ftid)
				);

				priority_description.y += 20.f;
				state.iui_state.localized_string(
					state,
					(int32_t)static_elements::priority_text_bonus,
					priority_description,
					text::produce_simple_string(state, "alice_factory_type_viewer_priority_bonus")
				);
				state.iui_state.float_2(
					state,
					(int32_t)static_elements::priority_value_bonus,
					priority_description,
					economy::priority_multiplier(state, ftid, nid)
				);

				priority_description.y += 20.f;

				iui::rect button_priority{
					priority_description.x, priority_description.y,
					priority_button_width, priority_button_height
				};

				auto current_priority = state.world.nation_get_factory_type_experience_priority_national(
					state.local_player_nation, ftid
				);

				for(int i = 0; i < 5; i++) {
					if(state.iui_state.button(
						state, (int32_t)static_elements::factory_type_priority_button + i,
						button_priority, current_priority >= (float)(i) * 10.f
					)) {						
						command::set_factory_type_priority(state, state.local_player_nation, ftid, (float)(i) * 10.f);
					}
					button_priority.x += button_priority.w;
				}
			}
		}
	}

	state.iui_state.frame_end();
}
}
