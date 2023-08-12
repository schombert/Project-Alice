#pragma once

#include "gui_element_types.hpp"
#include "military.hpp"
#include "gui_land_combat.hpp"

namespace ui {


class nc_attacker_leader_img : public image_element_base {
	dcon::gfx_object_id def;

	void on_update(sys::state& state) noexcept override {
		if(!def)
			def = base_data.data.image.gfx_object;

		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto lid = state.world.naval_battle_get_admiral_from_attacking_admiral(b);

		if(!lid) {
			base_data.data.image.gfx_object = def;
			return;
		}

		auto owner = state.world.leader_get_nation_from_leader_loyalty(lid);
		auto pculture = state.world.nation_get_primary_culture(owner);
		auto ltype = pculture.get_group_from_culture_group_membership().get_leader();

		if(ltype) {
			auto grange = ltype.get_admirals();
			if(grange.size() > 0) {
				auto rval = rng::get_random(state, uint32_t(state.world.leader_get_since(lid).value), uint32_t(lid.value));
				auto in_range = rng::reduce(uint32_t(rval), grange.size());
				base_data.data.image.gfx_object = grange[in_range];
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto lid = state.world.naval_battle_get_admiral_from_attacking_admiral(b);

		if(lid)
			display_leader_attributes(state, lid, contents, 0);
		else
			text::add_line(state, contents, "no_leader");
	}
};
class nc_defending_leader_img : public image_element_base {
	dcon::gfx_object_id def;

	void on_update(sys::state& state) noexcept override {
		if(!def)
			def = base_data.data.image.gfx_object;

		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto lid = state.world.naval_battle_get_admiral_from_defending_admiral(b);

		if(!lid) {
			base_data.data.image.gfx_object = def;
			return;
		}

		auto owner = state.world.leader_get_nation_from_leader_loyalty(lid);
		auto pculture = state.world.nation_get_primary_culture(owner);
		auto ltype = pculture.get_group_from_culture_group_membership().get_leader();

		if(ltype) {
			auto grange = ltype.get_admirals();
			if(grange.size() > 0) {
				auto rval = rng::get_random(state, uint32_t(state.world.leader_get_since(lid).value), uint32_t(lid.value));
				auto in_range = rng::reduce(uint32_t(rval), grange.size());
				base_data.data.image.gfx_object = grange[in_range];
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto lid = state.world.naval_battle_get_admiral_from_defending_admiral(b);

		if(lid)
			display_leader_attributes(state, lid, contents, 0);
		else
			text::add_line(state, contents, "no_leader");
	}
};

class nc_attacking_leader_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto lid = state.world.naval_battle_get_admiral_from_attacking_admiral(b);

		if(lid) {
			auto name = state.to_string_view(state.world.leader_get_name(lid));
			set_text(state, std::string(name));
		} else {
			set_text(state, "");
		}
	}
};
class nc_defending_leader_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto lid = state.world.naval_battle_get_admiral_from_defending_admiral(b);

		if(lid) {
			auto name = state.to_string_view(state.world.leader_get_name(lid));
			set_text(state, std::string(name));
		} else {
			set_text(state, "");
		}
	}
};


class nc_attacker_flag : public flag_button {
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
		base_data.position.x += int16_t(4);
		flag_button::on_create(state);
	}
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto n = military::get_naval_battle_lead_attacker(state, b);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
};
class nc_defender_flag : public flag_button {
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
		base_data.position.x += int16_t(4);
		flag_button::on_create(state);
	}
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto n = military::get_naval_battle_lead_defender(state, b);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
};


class nc_defender_org : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == false) {
				for(auto r : a.get_navy().get_navy_membership()) {
					++count;
					total += r.get_ship().get_org();
				}
			}
		}
		progress = count > 0 ? total / count : 0.0f;
	}
};
class nc_defender_str : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == false) {
				for(auto r : a.get_navy().get_navy_membership()) {
					++count;
					total += r.get_ship().get_strength();
				}
			}
		}
		progress = count > 0 ? total / count : 0.0f;
	}
};
class nc_attacker_org : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == true) {
				for(auto r : a.get_navy().get_navy_membership()) {
					++count;
					total += r.get_ship().get_org();
				}
			}
		}
		progress = count > 0 ? total / count : 0.0f;
	}
};
class nc_attacker_str : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == true) {
				for(auto r : a.get_navy().get_navy_membership()) {
					++count;
					total += r.get_ship().get_strength();
				}
			}
		}
		progress = count > 0 ? total / count : 0.0f;
	}
};


class nc_defender_org_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == false) {
				for(auto r : a.get_navy().get_navy_membership()) {
					++count;
					total += r.get_ship().get_org();
				}
			}
		}
		set_text(state, text::format_percentage(count > 0 ? total / count : 0.0f, 1));
	}
};
class nc_defender_str_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == false) {
				for(auto r : a.get_navy().get_navy_membership()) {
					++count;
					total += r.get_ship().get_strength();
				}
			}
		}
		set_text(state, text::format_percentage(count > 0 ? total / count : 0.0f, 1));
	}
};
class nc_attacker_org_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == true) {
				for(auto r : a.get_navy().get_navy_membership()) {
					++count;
					total += r.get_ship().get_org();
				}
			}
		}
		set_text(state, text::format_percentage(count > 0 ? total / count : 0.0f, 1));
	}
};
class nc_attacker_str_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == true) {
				for(auto r : a.get_navy().get_navy_membership()) {
					++count;
					total += r.get_ship().get_strength();
				}
			}
		}
		set_text(state, text::format_percentage(count > 0 ? total / count : 0.0f, 1));
	}
};

class nc_defender_bs_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		int32_t count = 0;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == false) {
				for(auto r : a.get_navy().get_navy_membership()) {
					auto type = r.get_ship().get_type();
					if(type && state.military_definitions.unit_base_definitions[type].type == military::unit_type::big_ship)
						++count;
				}
			}
		}
		set_text(state, text::prettify(count));
	}
};
class nc_defender_ss_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		int32_t count = 0;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == false) {
				for(auto r : a.get_navy().get_navy_membership()) {
					auto type = r.get_ship().get_type();
					if(type && state.military_definitions.unit_base_definitions[type].type == military::unit_type::light_ship)
						++count;
				}
			}
		}
		set_text(state, text::prettify(count));
	}
};
class nc_defender_ts_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		int32_t count = 0;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == false) {
				for(auto r : a.get_navy().get_navy_membership()) {
					auto type = r.get_ship().get_type();
					if(type && state.military_definitions.unit_base_definitions[type].type == military::unit_type::transport)
						++count;
				}
			}
		}
		set_text(state, text::prettify(count));
	}
};
class nc_attacker_bs_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		int32_t count = 0;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == true) {
				for(auto r : a.get_navy().get_navy_membership()) {
					auto type = r.get_ship().get_type();
					if(type && state.military_definitions.unit_base_definitions[type].type == military::unit_type::big_ship)
						++count;
				}
			}
		}
		set_text(state, text::prettify(count));
	}
};
class nc_attacker_ss_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		int32_t count = 0;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == true) {
				for(auto r : a.get_navy().get_navy_membership()) {
					auto type = r.get_ship().get_type();
					if(type && state.military_definitions.unit_base_definitions[type].type == military::unit_type::light_ship)
						++count;
				}
			}
		}
		set_text(state, text::prettify(count));
	}
};
class nc_attacker_ts_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
		int32_t count = 0;
		for(auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto owner = a.get_navy().get_controller_from_navy_control();
			bool battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
			if(battle_attacker == true) {
				for(auto r : a.get_navy().get_navy_membership()) {
					auto type = r.get_ship().get_type();
					if(type && state.military_definitions.unit_base_definitions[type].type == military::unit_type::transport)
						++count;
				}
			}
		}
		set_text(state, text::prettify(count));
	}
};

class nc_defender_combat_modifiers : public overlapping_listbox_element_base<lc_modifier, lc_modifier_data> {
	std::string_view get_row_element_name() override {
		return "alice_combat_modifier";
	}
	void update_subwindow(sys::state& state, lc_modifier& subwindow, lc_modifier_data content) override {
		subwindow.data = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto both_dice = state.world.naval_battle_get_dice_rolls(b);

		auto defender_dice = (both_dice >> 4) & 0x0F;

		auto defender_per = state.world.leader_get_personality(state.world.naval_battle_get_admiral_from_attacking_admiral(b));
		auto defender_bg = state.world.leader_get_background(state.world.naval_battle_get_admiral_from_attacking_admiral(b));

		auto defence_bonus =
			int32_t(state.world.leader_trait_get_defense(defender_per) + state.world.leader_trait_get_defense(defender_bg));

		row_contents.push_back(lc_modifier_data{ lc_mod_type::dice, defender_dice });
		if(defence_bonus != 0)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::leader, defence_bonus });

		update(state);
	}
};
class nc_attacker_combat_modifiers : public overlapping_listbox_element_base<lc_modifier, lc_modifier_data> {
	std::string_view get_row_element_name() override {
		return "alice_combat_modifier";
	}
	void update_subwindow(sys::state& state, lc_modifier& subwindow, lc_modifier_data content) override {
		subwindow.data = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto both_dice = state.world.naval_battle_get_dice_rolls(b);
		auto attacker_dice = both_dice & 0x0F;

		auto attacker_per = state.world.leader_get_personality(state.world.naval_battle_get_admiral_from_attacking_admiral(b));
		auto attacker_bg = state.world.leader_get_background(state.world.naval_battle_get_admiral_from_attacking_admiral(b));

		auto attack_bonus =
			int32_t(state.world.leader_trait_get_attack(attacker_per) + state.world.leader_trait_get_attack(attacker_bg));

		row_contents.push_back(lc_modifier_data{ lc_mod_type::dice, attacker_dice });
		if(attack_bonus != 0)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::leader, attack_bonus });

		update(state);
	}
};


class naval_combat_attacker_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<nc_attacker_flag>(state, id);
		} else if(name == "leader_icon") {
			return make_element_by_type<nc_attacker_leader_img>(state, id);
		} else if(name == "leader_name") {
			return make_element_by_type<nc_attacking_leader_name>(state, id);
		} else if(name == "morale") {
			return make_element_by_type<nc_attacker_org>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<nc_attacker_str>(state, id);
		} else if(name == "morale_text") {
			return make_element_by_type<nc_attacker_org_txt>(state, id);
		} else if(name == "strength_text") {
			return make_element_by_type<nc_attacker_str_txt>(state, id);
		} else if(name == "big_ship_icon") {
			return make_element_by_type<lc_static_icon<0>>(state, id);
		} else if(name == "small_ship_icon") {
			return make_element_by_type<lc_static_icon<1>>(state, id);
		} else if(name == "trade_ship_icon") {
			return make_element_by_type<lc_static_icon<2>>(state, id);
		} else if(name == "big_ship_value") {
			return make_element_by_type<nc_attacker_bs_txt>(state, id);
		} else if(name == "small_ship_value") {
			return make_element_by_type<nc_attacker_ss_txt>(state, id);
		} else if(name == "trade_ship_value") {
			return make_element_by_type<nc_attacker_ts_txt>(state, id);
		} else if(name == "modifiers") {
			return make_element_by_type<nc_attacker_combat_modifiers>(state, id);
		} else {
			return nullptr;
		}
	}
};

class naval_combat_defender_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<nc_defender_flag>(state, id);
		} else if(name == "leader_icon") {
			return make_element_by_type<nc_defending_leader_img>(state, id);
		} else if(name == "leader_name") {
			return make_element_by_type<nc_defending_leader_name>(state, id);
		} else if(name == "morale") {
			return make_element_by_type<nc_defender_org>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<nc_defender_str>(state, id);
		} else if(name == "morale_text") {
			return make_element_by_type<nc_defender_org_txt>(state, id);
		} else if(name == "strength_text") {
			return make_element_by_type<nc_defender_str_txt>(state, id);
		} else if(name == "big_ship_icon") {
			return make_element_by_type<lc_static_icon<0>>(state, id);
		} else if(name == "small_ship_icon") {
			return make_element_by_type<lc_static_icon<1>>(state, id);
		} else if(name == "trade_ship_icon") {
			return make_element_by_type<lc_static_icon<2>>(state, id);
		} else if(name == "big_ship_value") {
			return make_element_by_type<nc_defender_bs_txt>(state, id);
		} else if(name == "small_ship_value") {
			return make_element_by_type<nc_defender_ss_txt>(state, id);
		} else if(name == "trade_ship_value") {
			return make_element_by_type<nc_defender_ts_txt>(state, id);
		} else if(name == "modifiers") {
			return make_element_by_type<nc_defender_combat_modifiers>(state, id);
		} else {
			return nullptr;
		}
	}
};

class nc_retreat_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		disabled = !command::can_retreat_from_naval_battle(state, state.local_player_nation, retrieve<dcon::naval_battle_id>(state, parent));
	}
	void button_action(sys::state& state) noexcept override {
		command::retreat_from_naval_battle(state, state.local_player_nation, retrieve<dcon::naval_battle_id>(state, parent));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		text::add_line(state, contents, "alice_retreat");
		text::add_line_with_condition(state, contents, "alice_retreat_1",
			state.world.naval_battle_get_start_date(b) + military::days_before_retreat < state.current_date,
			text::variable_type::x, military::days_before_retreat);
		text::add_line_with_condition(state, contents, "alice_retreat_2",
			state.local_player_nation == military::get_naval_battle_lead_attacker(state, b)
			|| state.local_player_nation == military::get_naval_battle_lead_defender(state, b));
	}
};

class nbattle_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto loc = state.world.naval_battle_get_location_from_naval_battle_location(b);
		auto name = state.world.province_get_name(loc);
		auto txt = text::produce_simple_string(state, "battle_of") + " " + text::produce_simple_string(state, name);
		set_text(state, txt);
	}
};

class naval_combat_window : public window_element_base {
public:
	dcon::naval_battle_id battle;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		state.ui_state.naval_combat_window = this;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "combat_bg") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "label_battlename") {
			return make_element_by_type<nbattle_name>(state, id);
		} else if(name == "combat_retreat") {
			return make_element_by_type<nc_retreat_button>(state, id);
		} else if(name == "closebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "attacker") {
			return make_element_by_type<naval_combat_attacker_window>(state, id);
		} else if(name == "defender") {
			return make_element_by_type<naval_combat_defender_window>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(!state.world.naval_battle_is_valid(battle)) {
			set_visible(state, false);
			battle = dcon::naval_battle_id{};
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::naval_battle_id>()) {
			payload.emplace<dcon::naval_battle_id>(battle);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(state.world.naval_battle_get_location_from_naval_battle_location(battle));
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

//===============================================================================================================================

class naval_combat_end_popup_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{bool{true}};
			parent->impl_get(state, payload);
		}
	}
};

class naval_combat_end_popup : public window_element_base {
private:
	image_element_base* loss_image = nullptr;
	image_element_base* win_image = nullptr;
	simple_text_element_base* battlename_text = nullptr;
	simple_text_element_base* winorlose_text = nullptr;
	simple_text_element_base* warscore_text = nullptr;
	simple_text_element_base* ourleader_text = nullptr;
	simple_text_element_base* enemyleader_text = nullptr;

	//===============================================================

	simple_text_element_base* left_bigship_initial_text = nullptr;
	simple_text_element_base* left_bigship_casualties_text = nullptr;
	simple_text_element_base* left_bigship_survivors_text = nullptr;

	simple_text_element_base* left_smallship_initial_text = nullptr;
	simple_text_element_base* left_smallship_casualties_text = nullptr;
	simple_text_element_base* left_smallship_survivors_text = nullptr;

	simple_text_element_base* left_transship_initial_text = nullptr;
	simple_text_element_base* left_transship_casualties_text = nullptr;
	simple_text_element_base* left_transship_survivors_text = nullptr;

	//===============================================================

	simple_text_element_base* right_bigship_initial_text = nullptr;
	simple_text_element_base* right_bigship_casualties_text = nullptr;
	simple_text_element_base* right_bigship_survivors_text = nullptr;

	simple_text_element_base* right_smallship_initial_text = nullptr;
	simple_text_element_base* right_smallship_casualties_text = nullptr;
	simple_text_element_base* right_smallship_survivors_text = nullptr;

	simple_text_element_base* right_transship_initial_text = nullptr;
	simple_text_element_base* right_transship_casualties_text = nullptr;
	simple_text_element_base* right_transship_survivors_text = nullptr;

	//===============================================================

	simple_text_element_base* left_total_initial_text = nullptr;
	simple_text_element_base* left_total_lost_text = nullptr;
	simple_text_element_base* left_total_left_text = nullptr;

	simple_text_element_base* right_total_initial_text = nullptr;
	simple_text_element_base* right_total_lost_text = nullptr;
	simple_text_element_base* right_total_left_text = nullptr;

	//===============================================================
public:
	military::naval_battle_report report;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			// ptr->base_data.size = base_data.size; // Nudge
			return ptr;
		} else if(name == "combat_end_naval_lost") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			loss_image = ptr.get();
			return ptr;

		} else if(name == "combat_end_naval_won") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			win_image = ptr.get();
			return ptr;

		} else if(name == "line1") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			battlename_text = ptr.get();
			return ptr;

		} else if(name == "ourleader_photo") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "ourleader") { // Our great leader, the leader of the universe, Puffy, Puffy the cat, tremble in her might
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ourleader_text = ptr.get();
			return ptr;

		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige_number") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "we_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "we_number") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "initial_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "casualties_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "survivors_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemyleader_photo") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "enemyleader") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			enemyleader_text = ptr.get();
			return ptr;

		} else if(name == "prestige_icon2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige_number2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "we_icon2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "we_number2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "initial_label2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "casualties_label2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "survivors_label2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_1") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "our_unit_type_1_1_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_bigship_initial_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_1_2_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_bigship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_1_3_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_bigship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_2") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "our_unit_type_2_1_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_smallship_initial_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_2_2_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_smallship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_2_3_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_smallship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_3") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "our_unit_type_3_1_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_transship_initial_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_3_2_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_transship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_3_3_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_transship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_1") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "enemy_unit_type_1_1_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_bigship_initial_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_1_2_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_bigship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_1_3_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_bigship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_2") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "enemy_unit_type_2_1_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_smallship_initial_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_2_2_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_smallship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_2_3_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_smallship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_3") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "enemy_unit_type_3_1_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_transship_initial_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_3_2_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_transship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_3_3_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_transship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "our_total_armies") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_total_initial_text = ptr.get();
			return ptr;

		} else if(name == "our_total_loss") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_total_lost_text = ptr.get();
			return ptr;

		} else if(name == "our_total_left") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_total_left_text = ptr.get();
			return ptr;

		} else if(name == "enemy_total_armies") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			right_total_initial_text = ptr.get();
			return ptr;

		} else if(name == "enemy_total_loss") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			right_total_lost_text = ptr.get();
			return ptr;

		} else if(name == "enemy_total_left") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			right_total_left_text = ptr.get();
			return ptr;

		} else if(name == "agreebutton") {
			auto ptr = make_element_by_type<naval_combat_end_popup_close_button>(state, id);
			ptr->base_data.position.y += 146; // Nudge
			return ptr;
		} else if(name == "declinebutton") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.y += 146; // Nudge
			ptr->set_button_text(state, text::produce_simple_string(state, "navalbattleover_btn3"));
			return ptr;
		} else if(name == "centerok") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false); // Disacrd
			return ptr;
		} else if(name == "warscore_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "warscore") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			warscore_text = ptr.get();
			return ptr;
		} else if(name == "winorlose") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			winorlose_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		battlename_text->set_text(state, (text::produce_simple_string(state, "battle_of") + " " + text::produce_simple_string(state, dcon::fatten(state.world, report.location).get_name())));
		if(report.player_on_winning_side) {
			win_image->set_visible(state, true);
			loss_image->set_visible(state, false);
		} else {
			win_image->set_visible(state, false);
			loss_image->set_visible(state, true);
		}

		warscore_text->set_text(state, text::format_float(report.warscore_effect, 1));
		winorlose_text->set_text(state, report.player_on_winning_side ? text::produce_simple_string(state, "you_won") :
										text::produce_simple_string(state, "you_lost"));
		auto attack_leader = dcon::fatten(state.world, report.attacking_admiral);
		auto defend_leader = dcon::fatten(state.world, report.defending_admiral);

		attack_leader.is_valid() ? ourleader_text->set_text(state, text::produce_simple_string(state, state.to_string_view(attack_leader.get_name())))
					: ourleader_text->set_text(state, text::produce_simple_string(state, "no_leader"));
		defend_leader.is_valid() ? enemyleader_text->set_text(state, text::produce_simple_string(state, state.to_string_view(defend_leader.get_name())))
					: enemyleader_text->set_text(state, text::produce_simple_string(state, "no_leader"));

		//============================================================================================

		left_bigship_initial_text->set_text(state, text::format_float(report.attacker_big_ships, 0));
		left_smallship_initial_text->set_text(state, text::format_float(report.attacker_small_ships, 0));
		left_transship_initial_text->set_text(state, text::format_float(report.attacker_transport_ships, 0));

		right_bigship_initial_text->set_text(state, text::format_float(report.defender_big_ships, 0));
		right_smallship_initial_text->set_text(state, text::format_float(report.defender_small_ships, 0));
		right_transship_initial_text->set_text(state, text::format_float(report.defender_transport_ships, 0));

		//============================================================================================

		left_bigship_casualties_text->set_text(state, text::format_float(report.attacker_big_losses, 0));
		left_smallship_casualties_text->set_text(state, text::format_float(report.attacker_small_losses, 0));
		left_transship_casualties_text->set_text(state, text::format_float(report.attacker_transport_losses, 0));	// We stand for the Thin Pink Line

		right_bigship_casualties_text->set_text(state, text::format_float(report.defender_big_losses, 0));
		right_smallship_casualties_text->set_text(state, text::format_float(report.defender_small_losses, 0));
		right_transship_casualties_text->set_text(state, text::format_float(report.defender_transport_losses, 0));

		//============================================================================================

		left_bigship_survivors_text->set_text(state, text::format_float(float(report.attacker_big_ships - report.attacker_big_losses), 0));
		left_smallship_survivors_text->set_text(state, text::format_float(float(report.attacker_small_ships - report.attacker_small_losses), 0));
		left_transship_survivors_text->set_text(state, text::format_float(float(report.attacker_transport_ships - report.attacker_transport_losses), 0));

		right_bigship_survivors_text->set_text(state, text::format_float(float(report.defender_big_ships - report.defender_big_losses), 0));
		right_smallship_survivors_text->set_text(state, text::format_float(float(report.defender_small_ships - report.defender_small_losses), 0));
		right_transship_survivors_text->set_text(state, text::format_float(float(report.defender_transport_ships - report.defender_transport_losses), 0));

		//============================================================================================

		left_total_initial_text->set_text(state, text::format_float(
					float(report.attacker_big_ships + report.attacker_small_ships + report.attacker_transport_ships), 0));
		left_total_lost_text->set_text(state, text::format_float(
					float(report.attacker_big_losses + report.attacker_small_losses + report.attacker_transport_losses), 0));
		left_total_left_text->set_text(state, text::format_float(float(
					(report.attacker_big_ships - report.attacker_big_losses) +
					(report.attacker_small_ships - report.attacker_small_losses) +
					(report.attacker_transport_ships - report.attacker_transport_losses)
					), 0));
		right_total_initial_text->set_text(state, text::format_float(
					float(report.defender_big_ships + report.defender_small_ships + report.defender_transport_ships), 0));
		right_total_lost_text->set_text(state, text::format_float(
					float(report.defender_big_losses + report.defender_small_losses + report.defender_transport_losses), 0));
		right_total_left_text->set_text(state, text::format_float(float(
					(report.defender_big_ships - report.defender_big_losses) +
					(report.defender_small_ships - report.defender_small_losses) +
					(report.defender_transport_ships - report.defender_transport_losses)
					), 0));

		//============================================================================================
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<bool>>()) {
			auto content = any_cast<element_selection_wrapper<bool>>(payload).data;
			if(content) {
					auto root = static_cast<ui::container_base*>(state.ui_state.root.get());
					if(auto result = std::find_if(root->children.begin(), root->children.end(), [this](std::unique_ptr<element_base>& p){return p.get() == this;});
							result != root->children.end()) {
						if(result + 1 != children.end()) { std::rotate(result, result+1, root->children.end());}

						auto temp = std::move(root->children.back());
						root->children.pop_back();
						set_visible(state, false);
						state.ui_state.endof_navalcombat_windows.push_back(std::move(temp));
					} else {
						std::abort();	// Failed to move the naavl combat window back out
					}
			} else {
				// TODO - We dont handle this, yet
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
