#pragma once

#include "dcon_generated_ids.hpp"
#include "gui_element_types.hpp"

namespace ui {

class province_window_header;
class province_view_foreign_details;
class province_view_statistics;
class province_view_buildings;
class province_window_colony;

class province_terrain_image : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		dcon::province_id province_id = retrieve<dcon::province_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, province_id);
		auto terrain_id = fat_id.get_terrain().id;
		auto terrain_image = state.province_definitions.terrain_to_gfx_map[terrain_id];
		if(base_data.get_element_type() == element_type::image && terrain_image) {
			base_data.data.image.gfx_object = terrain_image;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		dcon::province_id province_id = retrieve<dcon::province_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, province_id);
		//terrain
		if(auto name = fat_id.get_terrain().get_name(); name) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, name), text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
		if(auto mod_id = fat_id.get_terrain().id; mod_id) {
			modifier_description(state, contents, mod_id);
		}
		//climate
		if(auto name = fat_id.get_climate().get_name(); name) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, name), text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
		if(auto mod_id = fat_id.get_climate().id; mod_id) {
			modifier_description(state, contents, mod_id);
		}
		//continent
		if(auto name = fat_id.get_continent().get_name(); name) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, name), text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
		if(auto mod_id = fat_id.get_continent().id; mod_id) {
			modifier_description(state, contents, mod_id);
		}
	}
};

class province_flashpoint_indicator : public image_element_base {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		if(state.world.state_instance_get_flashpoint_tag(state.world.province_get_state_membership(prov))) {
			image_element_base::render(state, x, y);
		} else {
			// no flashpoint
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		dcon::province_id prov = retrieve<dcon::province_id>(state, parent);
		if(!state.world.state_instance_get_flashpoint_tag(state.world.province_get_state_membership(prov)))
			return;

		text::substitution_map sub_map{};
		text::add_to_substitution_map(sub_map, text::variable_type::value, text::fp_two_places{ state.world.state_instance_get_flashpoint_tension(state.world.province_get_state_membership(prov)) });
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("flashpoint_tension"), sub_map);
		text::close_layout_box(contents, box);
	}
};

class province_controller_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(parent) {
			dcon::province_id province_id = retrieve<dcon::province_id>(state, parent);
			auto fat_id = dcon::fatten(state.world, province_id);
			return state.world.nation_get_identity_from_identity_holder(fat_id.get_province_control_as_province().get_nation().id);
		}
		return dcon::national_identity_id{};
	}

	void on_update(sys::state& state) noexcept override {
		flag_button::set_current_nation(state, province_controller_flag::get_current_nation(state));
		if(parent) {
			dcon::province_id province_id = retrieve<dcon::province_id>(state, parent);
			auto prov_fat = dcon::fatten(state.world, province_id);
			auto controller = prov_fat.get_province_control_as_province().get_nation();
			auto rebel_faction = prov_fat.get_province_rebel_control_as_province().get_rebel_faction();
			if(rebel_faction) {
				flag_texture_handle = ogl::get_rebel_flag_handle(state, rebel_faction);
			}
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		dcon::province_id province_id = retrieve<dcon::province_id>(state, parent);
		auto prov_fat = dcon::fatten(state.world, province_id);
		auto controller = prov_fat.get_province_control_as_province().get_nation();
		auto rebel_faction = prov_fat.get_province_rebel_control_as_province().get_rebel_faction();
		if(!controller && !rebel_faction)
			return;
		flag_button::render(state, x, y);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		dcon::province_id province_id = retrieve<dcon::province_id>(state, parent);
		auto prov_fat = dcon::fatten(state.world, province_id);
		auto controller = prov_fat.get_province_control_as_province().get_nation();
		auto rebel_faction = prov_fat.get_province_rebel_control_as_province().get_rebel_faction();
		if(!controller && !rebel_faction)
			return;
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("pv_controller"));
		text::add_space_to_layout_box(state, contents, box);
		if(controller) {
			text::add_to_layout_box(state, contents, box, text::get_name(state, controller));
		} else {
			text::add_to_layout_box(state, contents, box, rebel::rebel_name(state, rebel_faction));
		}
		text::close_layout_box(contents, box);
	}
};

class province_national_focus_button : public right_click_button_element_base {
public:
	int32_t get_icon_frame(sys::state& state) noexcept {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		if(state.world.state_instance_get_nation_from_flashpoint_focus(content) == state.local_player_nation)
			return state.world.national_focus_get_icon(state.national_definitions.flashpoint_focus) - 1;
		return bool(state.world.state_instance_get_owner_focus(content).id)
			? state.world.state_instance_get_owner_focus(content).get_icon() - 1
			: 0;
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		disabled = true;
		for(auto nfid : state.world.in_national_focus) {
			disabled = command::can_set_national_focus(state, state.local_player_nation, content, nfid) ? false : disabled;
		}
		if(state.world.state_instance_get_nation_from_flashpoint_focus(content) == state.local_player_nation)
			disabled = false;
		frame = get_icon_frame(state);
	}

	void button_action(sys::state& state) noexcept override;
	void button_right_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		command::set_national_focus(state, state.local_player_nation, content, dcon::national_focus_id{});
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);

		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto fat_si = dcon::fatten(state.world, sid);
		text::add_to_layout_box(state, contents, box, sid);
		text::add_line_break_to_layout_box(state, contents, box);
		auto content = state.world.state_instance_get_owner_focus(sid);
		if(bool(content)) {
			auto fat_nf = dcon::fatten(state.world, content);
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
		}
		text::close_layout_box(contents, box);
		if(auto mid = state.world.national_focus_get_modifier(content);  mid) {
			modifier_description(state, contents, mid, 15);
		}
		text::add_line(state, contents, "alice_nf_controls");
	}
};


class province_modifier_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		sys::dated_modifier mod = retrieve< sys::dated_modifier>(state, parent);
		if(mod.mod_id) {
			frame = state.world.modifier_get_icon(mod.mod_id) - 1;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		sys::dated_modifier mod = retrieve< sys::dated_modifier>(state, parent);
		if(mod.mod_id) {
			auto p = retrieve<dcon::province_id>(state, parent);
			auto n = state.world.province_get_nation_from_province_ownership(p);
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, state.world.modifier_get_name(mod.mod_id), text::text_color::yellow);
			text::add_line_break_to_layout_box(state, contents, box);
			if(auto desc = state.world.modifier_get_desc(mod.mod_id); state.key_is_localized(desc)) {
				text::substitution_map sub{};
				text::add_to_substitution_map(sub, text::variable_type::country, n);
				text::add_to_substitution_map(sub, text::variable_type::country_adj, text::get_adjective(state, n));
				text::add_to_substitution_map(sub, text::variable_type::capital, state.world.nation_get_capital(n));
				text::add_to_substitution_map(sub, text::variable_type::continentname, state.world.modifier_get_name(state.world.province_get_continent(state.world.nation_get_capital(n))));
				text::add_to_substitution_map(sub, text::variable_type::provincename, p);
				text::add_to_layout_box(state, contents, box, desc, sub);
			}
			text::close_layout_box(contents, box);
			modifier_description(state, contents, mod.mod_id, 15);
		}
		if(mod.expiration) {
			text::add_line(state, contents, "expires_on", text::variable_type::date, mod.expiration);
		}
	}
};

class province_modifier_win : public window_element_base {
public:
	sys::dated_modifier mod;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "modifier") {
			return make_element_by_type<province_modifier_icon>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<sys::dated_modifier>()) {
			payload.emplace<sys::dated_modifier>(mod);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

};

class province_modifiers : public overlapping_listbox_element_base<province_modifier_win, sys::dated_modifier> {
public:
	std::string_view get_row_element_name() override {
		return "prov_state_modifier";
	}
	void update_subwindow(sys::state& state, province_modifier_win& subwindow, sys::dated_modifier content) override {
		subwindow.mod = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto prov = retrieve<dcon::province_id>(state, parent);
		if(prov) {
			for(auto mods : state.world.province_get_current_modifiers(prov)) {
				row_contents.push_back(mods);
			}
		}
		update(state);
	}
};

class province_move_capital_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_move_capital(state, state.local_player_nation, p);
	}

	void button_action(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		command::move_capital(state, state.local_player_nation, p);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto source = state.local_player_nation;
		auto p = retrieve<dcon::province_id>(state, parent);
		text::add_line(state, contents, "alice_mvcap_1");
		text::add_line_with_condition(state, contents, "alice_mvcap_2", state.current_crisis_state == sys::crisis_state::inactive);
		text::add_line_with_condition(state, contents, "alice_mvcap_3", !(state.world.nation_get_is_at_war(source)));
		text::add_line_with_condition(state, contents, "alice_mvcap_4", !(state.world.nation_get_capital(source) == p));
		text::add_line_with_condition(state, contents, "alice_mvcap_5", !(state.world.province_get_is_colonial(p)));
		text::add_line_with_condition(state, contents, "alice_mvcap_6", !(state.world.province_get_continent(state.world.nation_get_capital(source)) != state.world.province_get_continent(p)));
		text::add_line_with_condition(state, contents, "alice_mvcap_7", !(nations::nation_accepts_culture(state, source, state.world.province_get_dominant_culture(p)) == false));
		text::add_line_with_condition(state, contents, "alice_mvcap_8", !(state.world.province_get_siege_progress(p) > 0.f));
		text::add_line_with_condition(state, contents, "alice_mvcap_9", !(state.world.province_get_siege_progress(state.world.nation_get_capital(source)) > 0.f));
		text::add_line_with_condition(state, contents, "alice_mvcap_10", !(state.world.province_get_nation_from_province_ownership(p) != source));
		text::add_line_with_condition(state, contents, "alice_mvcap_11", !(state.world.province_get_nation_from_province_control(p) != source));
		text::add_line_with_condition(state, contents, "alice_mvcap_12", !(state.world.province_get_is_owner_core(p) == false));
	}
};

class province_toggle_administration_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_toggle_local_administration(state, state.local_player_nation, p);
	}

	void button_action(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		command::toggle_local_administration(state, state.local_player_nation, p);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto source = state.local_player_nation;
		auto p = retrieve<dcon::province_id>(state, parent);
		text::add_line(state, contents, "alice_toggle_administration");
	}
};

class province_take_province_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(state.defines.alice_allow_revoke_subject_states == 0.0f) {
			set_visible(state, false);
			return;
		}
		auto p = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_take_province(state, state.local_player_nation, p);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		auto fid = dcon::fatten(state.world, p);
		auto owner = fid.get_nation_from_province_ownership();

		if(owner != state.local_player_nation) {
			button_element_base::render(state, x, y);
		} else {
			// not rendered
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		command::take_province(state, state.local_player_nation, p);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto source = state.local_player_nation;
		auto p = retrieve<dcon::province_id>(state, parent);

		auto fid = dcon::fatten(state.world, p);
		auto owner = fid.get_nation_from_province_ownership();
		auto rel = state.world.nation_get_overlord_as_subject(owner);
		auto overlord = state.world.overlord_get_ruler(rel);

		text::add_line(state, contents, "alice_take_province_1");
		{
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{ state.defines.years_of_nationalism });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "alice_take_province_2", m);
			text::close_layout_box(contents, box);
		}
		{
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{ state.defines.alice_take_province_militancy_subject });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "alice_take_province_3", m);
			text::close_layout_box(contents, box);
		}
		{
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{ state.defines.alice_take_province_militancy_all_subjects });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "alice_take_province_4", m);
			text::close_layout_box(contents, box);
		}

		text::add_line_with_condition(state, contents, "alice_take_province_5", !(overlord != source));
		text::add_line_with_condition(state, contents, "alice_mvcap_2", state.current_crisis_state == sys::crisis_state::inactive);
		text::add_line_with_condition(state, contents, "alice_mvcap_3", !(state.world.nation_get_is_at_war(source)));
		text::add_line_with_condition(state, contents, "alice_take_province_6", !(state.world.nation_get_is_at_war(owner)));
		text::add_line_with_condition(state, contents, "alice_mvcap_8", !(state.world.province_get_siege_progress(p) > 0.f));
		text::add_line_with_condition(state, contents, "alice_mvcap_9", !(state.world.province_get_siege_progress(state.world.nation_get_capital(owner)) > 0.f));
		text::add_line_with_condition(state, contents, "alice_take_province_7", !(state.world.province_get_nation_from_province_control(p) != owner));
	}
};

class province_grant_province_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		disabled = true;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		auto fid = dcon::fatten(state.world, p);
		auto owner = fid.get_nation_from_province_ownership();

		if(owner == state.local_player_nation) {
			button_element_base::render(state, x, y);
		} else {
			// not rendered
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		command::move_capital(state, state.local_player_nation, p);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto source = state.local_player_nation;
		auto p = retrieve<dcon::province_id>(state, parent);
		
	}
};

class province_victory_points_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		auto n = state.world.province_get_nation_from_province_ownership(p);

		auto vp = military::province_point_cost(state, p, n);
		set_text(state, text::format_wholenum(vp));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "province_victory_points");
		auto p = retrieve<dcon::province_id>(state, parent);
		auto n = state.world.province_get_nation_from_province_ownership(p);

		text::add_line(state, contents, "province_victory_points_base");

		if(!state.world.province_get_is_colonial(p)) {
			auto nbsize = state.world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base));

			if (nbsize > 0)
				text::add_line(state, contents, "province_victory_points_nb", text::variable_type::x, nbsize);
		}

		auto fac_range = state.world.province_get_factory_location(p);
		auto fcount = int32_t(fac_range.end() - fac_range.begin());

		if (fcount > 0)
			text::add_line(state, contents, "province_victory_points_fcount", text::variable_type::x, fcount);

		auto fortsize = state.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort));

		if (fortsize > 0)
			text::add_line(state, contents, "province_victory_points_fortsize", text::variable_type::x, fortsize);

		auto owner_cap = state.world.nation_get_capital(n);
		auto overseas = (state.world.province_get_continent(p) != state.world.province_get_continent(owner_cap)) &&
			(state.world.province_get_connected_region_id(p) != state.world.province_get_connected_region_id(owner_cap));

		if(state.world.province_get_is_owner_core(p) && !overseas) {
			text::add_line(state, contents, "province_victory_points_mainlandcore");
		}
		if(state.world.nation_get_capital(n) == p) {
			text::add_line(state, contents, "province_victory_points_capital");
		}
	}
};

class province_window_header : public window_element_base {
private:
	fixed_pop_type_icon* slave_icon = nullptr;
	province_colony_button* colony_button = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_name") {
			return make_element_by_type<province_state_name_text>(state, id);
		} else if(name == "province_name") {
			return make_element_by_type<generic_name_text<dcon::province_id>>(state, id);
		} else if(name == "prov_terrain") {
			return make_element_by_type<province_terrain_image>(state, id);
		} else if(name == "province_modifiers") {
			return make_element_by_type<province_modifiers>(state, id);
		} else if(name == "slave_state_icon") {
			auto ptr = make_element_by_type<fixed_pop_type_icon>(state, id);
			slave_icon = ptr.get();
			ptr->set_type(state, state.culture_definitions.slaves);
			return ptr;
		} else if(name == "admin_icon") {
			auto ptr = make_element_by_type<fixed_pop_type_icon>(state, id);
			ptr->set_type(state, state.culture_definitions.bureaucrat);
			return ptr;
		} else if(name == "owner_icon") {
			auto ptr = make_element_by_type<fixed_pop_type_icon>(state, id);
			ptr->set_type(state, state.culture_definitions.aristocrat);
			return ptr;
		} else if(name == "controller_flag") {
			return make_element_by_type<province_controller_flag>(state, id);
		} else if(name == "flashpoint_indicator") {
			return make_element_by_type<province_flashpoint_indicator>(state, id);
		} else if(name == "occupation_progress") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "occupation_icon") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "occupation_flag") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "colony_button") {
			auto btn = make_element_by_type<province_colony_button>(state, id);
			colony_button = btn.get();
			return btn;
		} else if(name == "alice_move_capital") {
			return make_element_by_type<province_move_capital_button>(state, id);
		} else if(name == "alice_toggle_administration") {
			return make_element_by_type<province_toggle_administration_button>(state, id);
		} else if(name == "province_victory_points_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "province_victory_points") {
		return make_element_by_type<province_victory_points_text>(state, id);
		} else if(name == "alice_take_province") {
			return make_element_by_type<province_take_province_button>(state, id);
		} else if(name == "alice_grant_province") {
			return make_element_by_type<province_grant_province_button>(state, id);
		} else if(name == "national_focus") {
			return make_element_by_type<province_national_focus_button>(state, id);
		} else if(name == "admin_efficiency") {
			return make_element_by_type<state_admin_efficiency_text>(state, id);
		} else if(name == "owner_presence") {
			return make_element_by_type<state_aristocrat_presence_text>(state, id);
		} else if(name == "liferating") {
			return make_element_by_type<province_liferating>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		dcon::province_id prov_id = retrieve<dcon::province_id>(state, parent);
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		colony_button->set_visible(state, fat_id.get_is_colonial());
		slave_icon->set_visible(state, fat_id.get_is_slave());
	}
};

class province_send_diplomat_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.open_diplomacy(retrieve<dcon::nation_id>(state, parent));
	}
};

class province_core_flags : public overlapping_flags_box {
private:
	void populate(sys::state& state, dcon::province_id prov_id) {
		row_contents.clear();
		auto fat_id = dcon::fatten(state.world, prov_id);
		fat_id.for_each_core_as_province([&](dcon::core_id core_id) {
			auto core_fat_id = dcon::fatten(state.world, core_id);
			auto identity = core_fat_id.get_identity();
			row_contents.push_back(identity.id);
		});
		update(state);
	}

public:
	void on_update(sys::state& state) noexcept override {
		populate(state, retrieve<dcon::province_id>(state, parent));
	}
};

template<economy::province_building_type Value>
class province_building_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, prov_id);
		frame = fat_id.get_building_level(uint8_t(Value));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto id = retrieve<dcon::province_id>(state, parent);
		province_building_tooltip(state, contents, id, Value);
	}
};
template<economy::province_building_type Value>
class province_building_expand_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_begin_province_building_construction(state, state.local_player_nation, content, Value);
	}

	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		command::begin_province_building_construction(state, state.local_player_nation, content, Value);
	}
	virtual void button_shift_action(sys::state& state) noexcept override {
		if constexpr(Value == economy::province_building_type::naval_base) {
			button_action(state);
		} else {
			auto pid = retrieve<dcon::province_id>(state, parent);
			auto si = state.world.province_get_state_membership(pid);
			if(si) {
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
					if(command::can_begin_province_building_construction(state, state.local_player_nation, p, Value))
						command::begin_province_building_construction(state, state.local_player_nation, p, Value);
				});
			}
		}
	}
	virtual void button_shift_right_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		auto within_nation = state.world.province_get_nation_from_province_ownership(content);
		for(auto p : state.world.nation_get_province_ownership(within_nation)) {
			if(command::can_begin_province_building_construction(state, state.local_player_nation, p.get_province(), Value))
				command::begin_province_building_construction(state, state.local_player_nation, p.get_province(), Value);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto id = retrieve<dcon::province_id>(state, parent);
		province_building_construction_tooltip(state, contents, id, Value);
	}
};

template<economy::province_building_type Value>
class province_building_progress : public progress_bar {
public:
	void on_create(sys::state& state) noexcept override {
		progress_bar::on_create(state);
		base_data.position.y -= 2;
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		progress = economy::province_building_construction(state, content, Value).progress;
	}


	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);

		for(auto pb_con : state.world.province_get_province_building_construction(prov)) {
			if(pb_con.get_type() == uint8_t(Value)) {
				auto& goods = state.economy_definitions.building_definitions[int32_t(Value)].cost;
				auto& cgoods = pb_con.get_purchased_goods();

				float factor = economy::build_cost_multiplier(state, prov, pb_con.get_is_pop_project());

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					auto cid = goods.commodity_type[i];
					if(!cid) break;

					auto box = text::open_layout_box(contents, 0);

					auto required = goods.commodity_amounts[i];
					auto current = cgoods.commodity_amounts[i];

					std::string padding = cid.index() < 10 ? "0" : "";
					std::string description = "@$" + padding + std::to_string(cid.index());
					text::add_unparsed_text_to_layout_box(state, contents, box, description);

					text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(cid));
					text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
					text::add_to_layout_box(state, contents, box, text::fp_one_place{ current });
					text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
					text::add_to_layout_box(state, contents, box, text::fp_one_place{ required * factor });
					text::close_layout_box(contents, box);
				}

				return;
			}
		}
	}
};

template<economy::province_building_type Value>
class province_building_window : public window_element_base {
	button_element_base* expand_button = nullptr;
	image_element_base* under_construction_icon = nullptr;
	element_base* building_progress = nullptr;
	element_base* expanding_text = nullptr;

	std::string get_icon_name() noexcept {
		switch(Value) {
		case economy::province_building_type::fort:
			return "build_icon0";
		case economy::province_building_type::naval_base:
			return "build_icon1";
		case economy::province_building_type::railroad:
			return "build_icon2";
		case economy::province_building_type::bank:
			return "build_icon3";
		case economy::province_building_type::university:
			return "build_icon4";
		default:
			return "build_icon0";
		}
	}

	bool is_being_built(sys::state& state, dcon::province_id id) noexcept {
		for(auto pb : state.world.province_get_province_building_construction(id))
			if(economy::province_building_type(pb.get_type()) == Value)
				return true;
		return false;
	}

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == get_icon_name()) {
			return make_element_by_type<province_building_icon<Value>>(state, id);
		} else if(name == "underconstruction_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			under_construction_icon = ptr.get();
			return ptr;
		} else if(name == "building_progress") {
			auto ptr = make_element_by_type<province_building_progress<Value>>(state, id);
			building_progress = ptr.get();
			return ptr;
		} else if(name == "expand_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			expanding_text = ptr.get();
			return ptr;
		} else if(name == "expand") {
			auto ptr = make_element_by_type<province_building_expand_button<Value>>(state, id);
			expand_button = ptr.get();
			return ptr;
		} else if(name == "description") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, economy::province_building_type_get_name(Value)));
			return ptr;
		} else if(name.substr(0, 10) == "build_icon") {
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		expand_button->set_visible(state, !is_being_built(state, content));
		under_construction_icon->set_visible(state, is_being_built(state, content));
		building_progress->set_visible(state, is_being_built(state, content));
		expanding_text->set_visible(state, is_being_built(state, content));
	}
};

//
// Selector
//
class province_selector_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_toggle_select_province(state, state.local_player_nation, p);

		bool found = false;
		for(auto m : state.world.province_get_current_modifiers(p)) {
			if(m.mod_id == state.economy_definitions.selector_modifier) {
				found = true;
				break;
			}
		}

		if(found) {
			set_button_text(state, text::produce_simple_string(state, "on"));
		} else {
			set_button_text(state, text::produce_simple_string(state, "off"));
		}
	}
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		command::toggle_select_province(state, state.local_player_nation, content);
	}
	virtual void button_shift_action(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto si = state.world.province_get_state_membership(pid);
		if(si) {
			province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
				command::toggle_select_province(state, state.local_player_nation, p);
			});
		}
	}
};
class province_selector_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		frame = 0;
		for(auto m : state.world.province_get_current_modifiers(p)) {
			if(m.mod_id == state.economy_definitions.selector_modifier) {
				frame = 1;
				break;
			}
		}
	}
};
class province_selector_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "underconstruction_icon"
		|| name == "building_progress"
		|| name == "expand_text") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "expand") {
			return make_element_by_type<province_selector_button>(state, id);
		} else if(name == "description") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "province_selector"));
			return ptr;
		} else if(name.substr(0, 10) == "build_icon") {
			int32_t value = name[10] - '0';
			int32_t num_buildings = 0;
			for(auto& def : state.economy_definitions.building_definitions) {
				if(def.defined)
					num_buildings++;
			}
			if(value == num_buildings) {
				return make_element_by_type<province_selector_image>(state, id);
			}
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}
};

//
// Immigrator
//
class province_immigrator_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_toggle_immigrator_province(state, state.local_player_nation, p);

		bool found = false;
		for(auto m : state.world.province_get_current_modifiers(p)) {
			if(m.mod_id == state.economy_definitions.immigrator_modifier) {
				found = true;
				break;
			}
		}

		if(found) {
			set_button_text(state, text::produce_simple_string(state, "on"));
		} else {
			set_button_text(state, text::produce_simple_string(state, "off"));
		}
	}
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		command::toggle_immigrator_province(state, state.local_player_nation, content);
	}
	virtual void button_shift_action(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto si = state.world.province_get_state_membership(pid);
		if(si) {
			province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
				command::toggle_immigrator_province(state, state.local_player_nation, p);
			});
		}
	}
};
class province_immigrator_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		frame = 0;
		for(auto m : state.world.province_get_current_modifiers(p)) {
			if(m.mod_id == state.economy_definitions.immigrator_modifier) {
				frame = 1;
				break;
			}
		}
	}
};
class province_immigrator_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "underconstruction_icon"
		|| name == "building_progress"
		|| name == "expand_text") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "expand") {
			return make_element_by_type<province_immigrator_button>(state, id);
		} else if(name == "description") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "province_immigrator"));
			return ptr;
		} else if(name.substr(0, 10) == "build_icon") {
			int32_t value = name[10] - '0';
			int32_t num_buildings = 0;
			for(auto& def : state.economy_definitions.building_definitions) {
				if(def.defined)
					num_buildings++;
			}
			if(value == num_buildings + 1) {
				return make_element_by_type<province_immigrator_image>(state, id);
			}
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}
};

class province_invest_railroad_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		command::begin_province_building_construction(state, state.local_player_nation, content, economy::province_building_type::railroad);
	}
	virtual void button_shift_action(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto si = state.world.province_get_state_membership(pid);
		if(si) {
			province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
				command::begin_province_building_construction(state, state.local_player_nation, p, economy::province_building_type::railroad);
			});
		}
	}
	virtual void button_shift_right_action(sys::state& state) noexcept override {
		auto pid = retrieve<dcon::province_id>(state, parent);
		auto n = state.world.province_get_nation_from_province_ownership(pid);
		for(auto p : state.world.nation_get_province_ownership(n)) {
			command::begin_province_building_construction(state, state.local_player_nation, p.get_province(), economy::province_building_type::railroad);
		}
	}
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_begin_province_building_construction(state, state.local_player_nation, content,
				economy::province_building_type::railroad);
	}
};

class province_country_flag_button : public flag_button {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
};

template<economy::province_building_type Value>
class province_view_foreign_building_icon : public province_building_icon<Value> {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		switch(Value) {
		case economy::province_building_type::railroad:
			text::localised_format_box(state, contents, box, std::string_view("pv_railroad"));
			break;
		case economy::province_building_type::fort:
			text::localised_format_box(state, contents, box, std::string_view("pv_fort"));
			break;
		case economy::province_building_type::naval_base:
			text::localised_format_box(state, contents, box, std::string_view("pv_navalbase"));
			break;
		case economy::province_building_type::bank:
			text::localised_format_box(state, contents, box, std::string_view("pv_bank"));
			break;
		case economy::province_building_type::university:
			text::localised_format_box(state, contents, box, std::string_view("pv_university"));
			break;
		default:
			break;
		}
		text::close_layout_box(contents, box);
	}
};

class province_invest_factory_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		if(content) {
			open_build_foreign_factory(state, content);
		}

	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		disabled = true;

		if(!content)
			return;

		for(auto ft : state.world.in_factory_type) {
			if(command::can_begin_factory_building_construction(state, state.local_player_nation,
				content, ft, false)) {

				disabled = false;
				return;
			}
		}
	}
};

class province_supply_limit_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto supply = int32_t(military::supply_limit_in_province(state, state.local_player_nation, province_id));
		set_text(state, std::to_string(supply));
	}

	// TODO: needs an explanation of where the value comes from
};

class rr_investment_progress : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		progress = economy::province_building_construction(state, prov, economy::province_building_type::railroad).progress;
	}
};

class rr_invest_inwdow : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "building_progress") {
			return make_element_by_type<rr_investment_progress>(state, id);
		} else {
			return nullptr;
		}
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		if(province::has_railroads_being_built(state, prov)) {
			window_element_base::impl_render(state, x, y);
		}
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		if(province::has_railroads_being_built(state, prov))
			return window_element_base::test_mouse(state, x, y, type);
		return message_result::unseen;
	}
};

class province_view_foreign_details : public window_element_base {
private:
	province_country_flag_button* country_flag_button = nullptr;
	culture_piechart<dcon::province_id>* culture_chart = nullptr;
	ideology_piechart<dcon::province_id>* ideology_chart = nullptr;
	workforce_piechart<dcon::province_id>* workforce_chart = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_flag") {
			auto ptr = make_element_by_type<province_country_flag_button>(state, id);
			country_flag_button = ptr.get();
			return ptr;
		} else if(name == "country_gov") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "country_party") {
			return make_element_by_type<nation_ruling_party_text>(state, id);
		} else if(name == "country_prestige") {
			return make_element_by_type<nation_prestige_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<nation_industry_score_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<nation_military_score_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else if(name == "selected_prestige_rank") {
			return make_element_by_type<nation_prestige_rank_text>(state, id);
		} else if(name == "selected_industry_rank") {
			return make_element_by_type<nation_industry_rank_text>(state, id);
		} else if(name == "selected_military_rank") {
			return make_element_by_type<nation_military_rank_text>(state, id);
		} else if(name == "selected_total_rank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "country_status") {
			return make_element_by_type<nation_status_text>(state, id);
		} else if(name == "country_flag_overlay") {
			return make_element_by_type<nation_flag_frame>(state, id);
		} else if(name == "total_population") {
			return make_element_by_type<province_population>(state, id);
		} else if(name == "sphere_label") {
			return make_element_by_type<nation_sphere_list_label>(state, id);
		} else if(name == "puppet_label") {
			return make_element_by_type<nation_puppet_list_label>(state, id);
		} else if(name == "our_relation") {
			return make_element_by_type<nation_player_relations_text>(state, id);
		} else if(name == "workforce_chart") {
			auto ptr = make_element_by_type<workforce_piechart<dcon::province_id>>(state, id);
			workforce_chart = ptr.get();
			return ptr;
		} else if(name == "ideology_chart") {
			auto ptr = make_element_by_type<ideology_piechart<dcon::province_id>>(state, id);
			ideology_chart = ptr.get();
			return ptr;
		} else if(name == "culture_chart") {
			auto ptr = make_element_by_type<culture_piechart<dcon::province_id>>(state, id);
			culture_chart = ptr.get();
			return ptr;
		} else if(name == "goods_type") {
			return make_element_by_type<province_rgo>(state, id);
		} else if(name == "build_icon_fort") {
			return make_element_by_type<province_view_foreign_building_icon<economy::province_building_type::fort>>(state, id);
		} else if(name == "build_icon_navalbase") {
			return make_element_by_type<province_view_foreign_building_icon<economy::province_building_type::naval_base>>(state, id);
		} else if(name == "build_icon_infra") {
			return make_element_by_type<province_view_foreign_building_icon<economy::province_building_type::railroad>>(state, id);
		} else if(name == "build_icon_bank") {
			return make_element_by_type<province_view_foreign_building_icon<economy::province_building_type::bank>>(state, id);
		} else if(name == "build_icon_university") {
			return make_element_by_type<province_view_foreign_building_icon<economy::province_building_type::university>>(state, id);
		} else if(name == "infra_progress_win") {
			return make_element_by_type<rr_invest_inwdow>(state, id);
		} else if(name == "invest_build_infra") {
			return make_element_by_type<province_invest_railroad_button>(state, id);
		} else if(name == "invest_factory_button") {
			return make_element_by_type<province_invest_factory_button>(state, id);
		} else if(name == "sphere_targets") {
			return make_element_by_type<overlapping_sphere_flags>(state, id);
		} else if(name == "puppet_targets") {
			return make_element_by_type<overlapping_puppet_flags>(state, id);
		} else if(name == "allied_targets") {
			return make_element_by_type<overlapping_ally_flags>(state, id);
		} else if(name == "war_targets") {
			return make_element_by_type<overlapping_enemy_flags>(state, id);
		} else if(name == "send_diplomat") {
			return make_element_by_type<province_send_diplomat_button>(state, id);
		} else if(name == "core_icons") {
			return make_element_by_type<province_core_flags>(state, id);
		} else if(name == "supply_limit") {
			return make_element_by_type<province_supply_limit_text>(state, id);
		} else if (name == "selected_military_icon") {
			auto ptr = make_element_by_type<military_score_icon>(state, id);
			return ptr;
		} else if(name == "rallypoint_checkbox") {
			return make_element_by_type<land_rally_point>(state, id);
		} else if(name == "rallypoint_checkbox_naval") {
			return make_element_by_type<naval_rally_point>(state, id);
		} else if(name == "rallypoint_merge_checkbox" || name == "rallypoint_merge_checkbox_naval") {
			return make_element_by_type<merge_rally_point>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		if(!bool(nation_id) || nation_id.id == state.local_player_nation) {
			set_visible(state, false);
		} else {
			country_flag_button->impl_on_update(state);
			culture_chart->impl_on_update(state);
			workforce_chart->impl_on_update(state);
			ideology_chart->impl_on_update(state);
			set_visible(state, true);
		}
	}
};

void province_owner_rgo_draw_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov_id) noexcept;

class province_owner_rgo : public province_rgo {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		province_owner_rgo_draw_tooltip(state, contents, prov_id);
	}
};

class province_owner_income_text : public province_income_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		province_owner_rgo_draw_tooltip(state, contents, prov_id);
	}
};

class province_owner_goods_produced_text : public province_goods_produced_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		province_owner_rgo_draw_tooltip(state, contents, prov_id);
	}
};

class province_rgo_employment_progress_icon : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province = retrieve<dcon::province_id>(state, parent);
		auto max_emp = economy::rgo_max_employment(state, province);
		auto employment_ratio = economy::rgo_employment(state, province) / (max_emp + 1.f);
		frame = int32_t(10.f * employment_ratio);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		auto owner = state.world.province_get_nation_from_province_ownership(prov_id);
		auto max_emp = economy::rgo_max_employment(state, prov_id);
		auto employment_ratio = economy::rgo_employment(state, prov_id) / (max_emp + 1.f);

		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, int64_t(std::ceil(employment_ratio * max_emp)));
		text::add_to_layout_box(state, contents, box, std::string_view{" / "});
		text::add_to_layout_box(state, contents, box, int64_t(std::ceil(max_emp)));

		text::close_layout_box(contents, box);
	}
};

class province_crime_icon : public image_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void on_update(sys::state& state) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, prov_id);
		frame = fat_id.get_crime().index() + 1;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto province = retrieve<dcon::province_id>(state, parent);
		auto crime = state.world.province_get_crime(province);
		if(crime) {
			modifier_description(state, contents, state.culture_definitions.crimes[crime].modifier);
		}
	}
};

class province_crime_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, province_id);
		auto crime_id = fat_id.get_crime();
		if(crime_id) {
			set_text(state, text::produce_simple_string(state, state.culture_definitions.crimes[crime_id].name));
		} else {
			set_text(state, "");
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto province = retrieve<dcon::province_id>(state, parent);
		auto crime = state.world.province_get_crime(province);
		if(crime) {
			modifier_description(state, contents, state.culture_definitions.crimes[crime].modifier);
		}
	}
};

class province_crime_fighting_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::format_percentage(province::crime_fighting_efficiency(state, province_id), 1));
	}

	/*
	// TODO: explain where the value comes from
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		text::localised_single_sub_box(state, contents, box, std::string_view("provinceview_crimefight"), text::variable_type::value, text::fp_one_place{province::crime_fighting_efficiency(state, content) * 100});
		text::close_layout_box(contents, box);
	}
	*/

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::administrative_efficiency_modifier, true);
	}
};

class province_rebel_percent_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::format_float(province::revolt_risk(state, province_id), 2));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		text::add_line(state, contents, "avg_mil_on_map", text::variable_type::value, text::fp_one_place{ province::revolt_risk(state, content) });
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::core_pop_militancy_modifier, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::global_pop_militancy_modifier, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::non_accepted_pop_militancy_modifier, true);
	}
};

class province_rgo_employment_percent_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto max_emp = economy::rgo_max_employment(state, province_id);
		auto emp = economy::rgo_employment(state, province_id);
		auto ratio = 0.f;
		if(max_emp > 0) {
			ratio = emp / max_emp;
		}
		set_text(state, text::format_percentage(ratio, 1));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::farm_rgo_eff, true);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::farm_rgo_size, true);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::mine_rgo_eff, true);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::mine_rgo_size, true);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::local_rgo_input, true);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::local_rgo_output, true);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::local_rgo_throughput, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::rgo_input, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::rgo_output, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::rgo_throughput, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::farm_rgo_eff, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::farm_rgo_size, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::mine_rgo_eff, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::mine_rgo_size, true);
	}
};

class province_migration_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto migration = state.world.province_get_daily_net_migration(province_id);
		auto immigration = state.world.province_get_daily_net_immigration(province_id);
		set_text(state, text::prettify(int32_t(migration - immigration)));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::immigrant_attract, true);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::immigrant_push, true);
		if(state.world.province_get_is_colonial(content)) {
			ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::colonial_migration, true);
		}
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::global_immigrant_attract, true);
	}
};

class province_pop_growth_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::prettify(int32_t(demographics::get_monthly_pop_increase(state, province_id))));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		ui::active_modifiers_description(state, contents, content, 0, sys::provincial_mod_offsets::population_growth, true);
		ui::active_modifiers_description(state, contents, state.world.province_control_get_nation(state.world.province_get_province_control_as_province(content)), 0, sys::national_mod_offsets::pop_growth, true);
	}
};

class province_view_window : public window_element_base {
private:
	dcon::province_id active_province{};
	province_window_header* header_window = nullptr;
	province_view_foreign_details* foreign_details_window = nullptr;
	province_view_statistics* local_details_window = nullptr;
	province_view_buildings* local_buildings_window = nullptr;
	province_window_colony* colony_window = nullptr;
	element_base* economy_window = nullptr;
	element_base* nf_win = nullptr;
	element_base* tiles_window = nullptr;
	element_base* market_window = nullptr;

public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;	
	void set_active_province(sys::state& state, dcon::province_id map_province);
	void on_update(sys::state& state) noexcept override;
	friend class province_national_focus_button;
};

} // namespace ui
