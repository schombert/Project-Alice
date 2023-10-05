#pragma once
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_nation_picker.hpp"

namespace ui {


class end_nation_window : public window_element_base {
public:
	dcon::nation_id n;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag") {
			return make_element_by_type<picker_flag>(state, id);
		} else if(name == "country_flag_overlay") {
			return make_element_by_type<nation_flag_frame>(state, id);
		} else if(name == "score") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} 
		return nullptr;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept  override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(n);
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

class national_treasury_text : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto amount = state.world.nation_get_stockpiles(n, economy::money);
		set_text(state, text::format_money(amount));
	}
};

class national_province_count : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		int32_t total = state.world.nation_get_owned_province_count(n);
		set_text(state, std::to_string(total));
	}
};

class national_factory_count : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		int32_t total = 0;
		for(auto p : state.world.nation_get_province_ownership(n)) {
			auto frange = p.get_province().get_factory_location();
			total += int32_t(frange.end() - frange.begin());
		}
		set_text(state, std::to_string(total));
	}
};

class national_regiment_count : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		int32_t total = 0;
		for(auto p : state.world.nation_get_army_control(n)) {
			auto frange = p.get_army().get_army_membership();
			total += int32_t(frange.end() - frange.begin());
		}
		set_text(state, std::to_string(total));
	}
};

class national_ship_count : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		int32_t total = 0;
		for(auto p : state.world.nation_get_navy_control(n)) {
			auto frange = p.get_navy().get_navy_membership();
			total += int32_t(frange.end() - frange.begin());
		}
		set_text(state, std::to_string(total));
	}
};

class national_tech_count : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		int32_t total = 0;
		for(auto t : state.world.in_technology) {
			if(state.world.nation_get_active_technologies(n, t))
				++total;
		}
		set_text(state, std::to_string(total));
	}
};

class national_invention_count : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		int32_t total = 0;
		for(auto t : state.world.in_invention) {
			if(state.world.nation_get_active_inventions(n, t))
				++total;
		}
		set_text(state, std::to_string(total));
	}
};

class end_window_ideologies : public piechart<dcon::ideology_id> {
protected:
	void on_update(sys::state& state) noexcept override {
		distribution.clear();

		auto n = retrieve<dcon::nation_id>(state, parent);
		
		for(auto pt : state.world.in_ideology) {
			auto amount = state.world.nation_get_demographics(n, demographics::to_key(state, pt));
			distribution.emplace_back(pt.id, amount);
		}
		
		update_chart(state);
	}
};

class end_right_flag : public flag_button {
public:

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid && flag_texture_handle > 0) {
			auto& gfx_def = state.ui_defs.gfx[gid];
			auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependent - 1), true);
			auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
			ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), float(x),
					float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, mask_handle, base_data.get_rotation(),
					gfx_def.is_vertically_flipped());
		}
		image_element_base::render(state, x, y);
	}

	void on_create(sys::state& state) noexcept override {
		std::swap(base_data.size.x, base_data.size.y);
		base_data.position.x += int16_t(24);
		base_data.position.y -= int16_t(25);
		flag_button::on_create(state);
	}
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
	void button_action(sys::state& state) noexcept override {

	}
};

class end_left_flag : public flag_button {
public:

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid && flag_texture_handle > 0) {
			auto& gfx_def = state.ui_defs.gfx[gid];
			auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependent - 1), true);
			auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
			ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), float(x),
				float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, mask_handle,
				ui::rotation::r90_right, false);
	
			ogl::render_textured_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
				ui::rotation::r90_right, false);
		}

	}

	void on_create(sys::state& state) noexcept override {
		std::swap(base_data.size.x, base_data.size.y);
		base_data.position.x += int16_t(21);
		base_data.position.y -= int16_t(25);
		flag_button::on_create(state);
	}
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
	void button_action(sys::state& state) noexcept override {

	}
};

class end_window_player : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag1") {
			return make_element_by_type<end_left_flag>(state, id);
		} else if(name == "country_flag2") {
			return make_element_by_type<end_right_flag>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_class") {
			return make_element_by_type<nation_status_text>(state, id);
		} else if(name == "total_score") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else if(name == "country_rank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "country_prestige") {
			return make_element_by_type<nation_prestige_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<nation_industry_score_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<nation_military_score_text>(state, id);
		} else if(name == "country_money") {
			return make_element_by_type<national_treasury_text>(state, id);
		} else if(name == "population_value") {
			return make_element_by_type<nation_population_text>(state, id);
		} else if(name == "provinces_value") {
			return make_element_by_type<national_province_count>(state, id);
		} else if(name == "factories_value") {
			return make_element_by_type<national_factory_count>(state, id);
		} else if(name == "literacy_value") {
			return make_element_by_type<nation_literacy_text>(state, id);
		} else if(name == "army_value") {
			return make_element_by_type<national_regiment_count>(state, id);
		} else if(name == "navy_value") {
			return make_element_by_type<national_ship_count>(state, id);
		} else if(name == "gov_type_value") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "politics_party_icon") {
			return make_element_by_type<nation_ruling_party_ideology_plupp>(state, id);
		} else if(name == "party_value") {
			return make_element_by_type<nation_ruling_party_text>(state, id);
		} else if(name == "technologies_value") {
			return make_element_by_type<national_tech_count>(state, id);
		} else if(name == "inventions_value") {
			return make_element_by_type<national_invention_count>(state, id);
		} else if(name == "ideology_chart") {
			return make_element_by_type<end_window_ideologies>(state, id);
		} else if(name == "workforce_chart") {
			return make_element_by_type<nation_picker_poptypes_chart>(state, id);
		} else if(name == "ledger_button") {
			return make_element_by_type<nation_picker_hidden>(state, id);
		}
		return nullptr;
	}
};

class end_top_four_gp : public overlapping_listbox_element_base<end_nation_window, dcon::nation_id> {
public:
	std::string_view get_row_element_name() override {
		return "great_power_entry";
	}
	void update_subwindow(sys::state& state, end_nation_window& subwindow, dcon::nation_id content) override {
		subwindow.n = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		for(uint32_t i = 0; i < 4; ++i) {
			if(i < state.great_nations.size())
				row_contents.push_back(state.great_nations[i].nation);
		}
		update(state);
	}
};

class end_bottom_four_gp : public overlapping_listbox_element_base<end_nation_window, dcon::nation_id> {
public:
	std::string_view get_row_element_name() override {
		return "great_power_entry";
	}
	void update_subwindow(sys::state& state, end_nation_window& subwindow, dcon::nation_id content) override {
		subwindow.n = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		for(uint32_t i = 4; i < 8; ++i) {
			if(i < state.great_nations.size())
				row_contents.push_back(state.great_nations[i].nation);
		}
		update(state);
	}
};

class end_secondary_powers : public overlapping_listbox_element_base<end_nation_window, dcon::nation_id> {
public:
	std::string_view get_row_element_name() override {
		return "second_rank_entry";
	}
	void update_subwindow(sys::state& state, end_nation_window& subwindow, dcon::nation_id content) override {
		subwindow.n = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto total_num = int32_t(state.defines.colonial_rank - state.defines.great_nations_count);

		for(int32_t i = 0; i < int32_t(state.defines.colonial_rank); ++i) {
			auto n = state.nations_by_rank[i];
			if(!nations::is_great_power(state, n) && total_num > 0) {
				row_contents.push_back(n);
				--total_num;
			}
		}
		update(state);
	}
};

class end_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "year_text") {
			return make_element_by_type<date_label>(state, id);
		} else if(name == "great_powers_list") {
			return make_element_by_type<end_top_four_gp>(state, id);
		} else if(name == "great_powers_list2") {
			return make_element_by_type<end_bottom_four_gp>(state, id);
		} else if(name == "second_rank_list") {
			return make_element_by_type<end_secondary_powers>(state, id);
		} else if(name == "exit") {
			return make_element_by_type<quit_game_button>(state, id);
		} else if(name == "player_window") {
			return make_element_by_type<end_window_player>(state, id);
		}
		return nullptr;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept  override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

}
