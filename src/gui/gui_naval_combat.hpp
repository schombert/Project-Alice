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
		if(!lid) {
			text::add_line(state, contents, "no_leader");
		}
		display_leader_attributes(state, lid, contents, 0);
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
		if(!lid) {
			text::add_line(state, contents, "no_leader");
		}
		display_leader_attributes(state, lid, contents, 0);
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
			set_text(state, text::produce_simple_string(state, "no_leader"));
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
			set_text(state, text::produce_simple_string(state, "no_leader"));
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
				gfx_def.is_vertically_flipped(),
				false);
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
				gfx_def.is_vertically_flipped(),
				false);
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

		auto defender_per = military::get_leader_personality_wrapper(state, state.world.naval_battle_get_admiral_from_defending_admiral(b));
		auto defender_bg = military::get_leader_background_wrapper(state, state.world.naval_battle_get_admiral_from_defending_admiral(b));

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

		auto attacker_per = military::get_leader_personality_wrapper(state, state.world.naval_battle_get_admiral_from_attacking_admiral(b));
		auto attacker_bg = military::get_leader_background_wrapper(state, state.world.naval_battle_get_admiral_from_attacking_admiral(b));

		auto attack_bonus =
			int32_t(state.world.leader_trait_get_attack(attacker_per) + state.world.leader_trait_get_attack(attacker_bg));

		row_contents.push_back(lc_modifier_data{ lc_mod_type::dice, attacker_dice });
		if(attack_bonus != 0)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::leader, attack_bonus });

		update(state);
	}
};

template<bool attacker>
class nc_stacking_penalty_icon : public image_element_base {
	dcon::gfx_object_id def;

	void on_update(sys::state& state) noexcept override {

	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(b);

		int32_t attacker_ships = 0;
		int32_t defender_ships = 0;

		for(uint32_t j = slots.size(); j-- > 0;) {
			switch(slots[j].flags & military::ship_in_battle::mode_mask) {
			case military::ship_in_battle::mode_seeking:
			case military::ship_in_battle::mode_approaching:
			case military::ship_in_battle::mode_retreating:
			case military::ship_in_battle::mode_engaged:
				if((slots[j].flags & military::ship_in_battle::is_attacking) != 0)
					++attacker_ships;
				else
					++defender_ships;
				break;
			default:
				break;
			}
		}
		int32_t friendly_ships;
		int32_t enemy_ships;
		std::string atk_or_def_string;
		if(attacker) {
			atk_or_def_string = "attackers";
			friendly_ships = attacker_ships;
			enemy_ships = defender_ships;
		}
		else {
			atk_or_def_string = "defenders";
			friendly_ships = defender_ships;
			enemy_ships = attacker_ships;
		}
		float coordination_penalty = military::naval_battle_get_coordination_penalty(state, friendly_ships, enemy_ships);
		float coordination_bonus = military::naval_battle_get_coordination_bonus(state, friendly_ships, enemy_ships);
		float stacking_penalty = military::get_damage_reduction_stacking_penalty(state, friendly_ships, enemy_ships);
		if(coordination_penalty > 0.0f) {
			text::add_line(state, contents, "alice_naval_coordination_penalty", text::variable_type::x, atk_or_def_string, text::variable_type::y, text::format_percentage(coordination_penalty), text::variable_type::val, text::format_percentage(coordination_penalty * state.defines.naval_combat_stacking_target_select));
		}
		else {
			text::add_line(state, contents, "alice_naval_coordination_bonus", text::variable_type::x, atk_or_def_string, text::variable_type::y, text::format_percentage(coordination_bonus), text::variable_type::val, text::format_percentage(coordination_bonus * state.defines.alice_naval_combat_enemy_stacking_target_select_bonus));
		}
		if(stacking_penalty < 1.0f) {
			text::add_line(state, contents, "alice_naval_stacking_penalty", text::variable_type::x, atk_or_def_string, text::variable_type::y, text::format_percentage(stacking_penalty));
		}
		
	}
};

enum class ship_stance {
	normal,
	engaged,
	sunk,
	retreating,
	disengaged,

};
inline float navy_battle_distance_to_pixels(uint16_t distance) {
	float distance_percent = float(distance) / float(military::naval_battle_distance_to_center);
	return (1.0f - distance_percent) * 61.0f;
}


template<ship_stance Stance, bool Attacker>
class naval_combat_target_ship_stance : public image_element_base {
public:
	/*tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);

		military::ship_in_battle ship = retrieve<military::ship_in_battle>(state, parent);
		if(ship.target_slot > -1 && ship.target_slot < int16_t(slots.size())) {
			auto target_ship = slots[ship.target_slot];
			text::add_line(state, contents, "ORG_NAME", text::variable_type::val, text::fp_percentage_two_places{ state.world.ship_get_org(target_ship.ship) });
			text::add_line(state, contents, "alice_str_name", text::variable_type::val, text::fp_percentage_two_places{ state.world.ship_get_strength(target_ship.ship) });
#ifndef NDEBUG
			text::add_line(state, contents, "Ship ID :" + std::to_string(target_ship.ship.value));
			if(target_ship.target_slot > -1 && target_ship.target_slot < int16_t(slots.size())) {
				text::add_line(state, contents, "Target ship ID: " + std::to_string(slots[target_ship.target_slot].ship.value));
			}
			else {
				text::add_line(state, contents, "No or invalid target");
			}
			text::add_line(state, contents, "Target Slot ID :" + std::to_string(target_ship.target_slot));
			
#endif
		}
		
	}*/
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		military::ship_in_battle ship = retrieve<military::ship_in_battle>(state, parent);
		military::ship_in_battle target_ship{ };
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		bool visible = false;

		if(target_slot > -1 && target_slot < int16_t(slots.size())) {
			target_ship = slots[target_slot];
			if constexpr(Attacker) {
				x = x - int32_t(navy_battle_distance_to_pixels(target_ship.get_distance()));
			}
			else {
				x = x + int32_t(navy_battle_distance_to_pixels(target_ship.get_distance()));
			}
			
		} else {
			visible = false;
			return;
		}
		

		uint16_t mode = target_ship.flags & military::ship_in_battle::mode_mask;
		switch(Stance) {
		case ship_stance::normal:
			if(mode == military::ship_in_battle::mode_engaged) {
				if(target_ship.target_slot > -1 && target_ship.target_slot < int16_t(slots.size())) {
					auto target_of_target = slots[target_ship.target_slot];
					if(target_of_target.ship == ship.ship) {
						visible = false;

					} else {
						visible = true;
					}
				} else {
					visible = true;
				}
				break;
			} else {
				visible = mode == military::ship_in_battle::mode_seeking || mode == military::ship_in_battle::mode_approaching;
			}
			break;
		case ship_stance::engaged:
			if(mode == military::ship_in_battle::mode_engaged && target_ship.target_slot > -1 && target_ship.target_slot < int16_t(slots.size())) {
				auto target_of_target = slots[target_ship.target_slot];
				if(target_of_target.ship == ship.ship) {
					visible = true;
					break;
				}
			}
			visible = false;
			break;
		case ship_stance::sunk:
			visible = mode == military::ship_in_battle::mode_sunk;
			break;
		case ship_stance::retreating:
			visible = mode == military::ship_in_battle::mode_retreating;
			break;
		case ship_stance::disengaged:
			visible = mode == military::ship_in_battle::mode_retreated;
			break;
		default:
			assert(false);
		}
		
		if(visible) {
			image_element_base::render(state, x, y);
		}
	}

	void on_update(sys::state& state) noexcept override {
		military::ship_in_battle target_ship{}; 
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = retrieve<military::ship_in_battle>(state, parent).target_slot;
		if(target_slot > -1 && target_slot < int16_t(slots.size())) {
			target_ship = slots[target_slot];
		} else {
			return;
		}
		
		auto type = state.world.ship_get_type(target_ship.ship);
		if(type) {
			frame = state.military_definitions.unit_base_definitions[type].naval_icon - 1;
		}
	}
};

template<ship_stance Stance, bool Attacker>
class naval_combat_main_ship_stance : public image_element_base {
public:
	/*tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);

		military::ship_in_battle ship = retrieve<military::ship_in_battle>(state, parent);
		text::add_line(state, contents, "ORG_NAME", text::variable_type::val, text::fp_percentage_two_places{ state.world.ship_get_org(ship.ship) });
		text::add_line(state, contents, "alice_str_name", text::variable_type::val, text::fp_percentage_two_places{ state.world.ship_get_strength(ship.ship) });
#ifndef NDEBUG
		text::add_line(state, contents, "Ship ID: " + std::to_string(ship.ship.value));
		if(ship.target_slot > -1 && ship.target_slot < int16_t(slots.size())) {
			text::add_line(state, contents, "Target ship ID: " + std::to_string(slots[ship.target_slot].ship.value));
		} else {
			text::add_line(state, contents, "No or invalid target");
		}
		text::add_line(state, contents, "Target Slot ID: " + std::to_string(ship.target_slot));

#endif
		

	}*/


	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		military::ship_in_battle ship = retrieve<military::ship_in_battle>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		bool visible = false;


		ship = retrieve<military::ship_in_battle>(state, parent);
		if constexpr(Attacker) {
			x = x + int32_t(navy_battle_distance_to_pixels(ship.get_distance()));
		}
		else {
			x = x - int32_t(navy_battle_distance_to_pixels(ship.get_distance()));
		}
		
		
		uint16_t mode = ship.flags & military::ship_in_battle::mode_mask;
		switch(Stance) {
		case ship_stance::normal:
			visible = mode == military::ship_in_battle::mode_seeking || mode == military::ship_in_battle::mode_approaching;
			break;
		case ship_stance::engaged:
			visible = mode == military::ship_in_battle::mode_engaged;
			break;
		case ship_stance::sunk:
			visible = mode == military::ship_in_battle::mode_sunk;
			break;
		case ship_stance::retreating:
			visible = mode == military::ship_in_battle::mode_retreating;
			break;
		case ship_stance::disengaged:
			visible = mode == military::ship_in_battle::mode_retreated;
			break;
		default:
			assert(false);
		}
		
		
		
		if(visible) {
			image_element_base::render(state, x, y);
		}
	}

	void on_update(sys::state& state) noexcept override {
		military::ship_in_battle ship{};

		ship = retrieve<military::ship_in_battle>(state, parent);


		auto type = state.world.ship_get_type(ship.ship);
		if(type) {
			frame = state.military_definitions.unit_base_definitions[type].naval_icon - 1;
		}
	}
};

class naval_combat_main_ship_org_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		if(bool(ship.ship)) {
			progress = state.world.ship_get_org(ship.ship);
		}
	}
};

class naval_combat_target_ship_org_bar : public vertical_progress_bar {
public:

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		if(target_slot > -1 && target_slot < uint16_t(slots.size())) {
			auto target_ship = slots[target_slot];
			if(bool(target_ship.ship)) {
				vertical_progress_bar::render(state, x, y);
			}

		}
	}
	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		if(target_slot > -1 && target_slot < uint16_t(slots.size())) {
			auto target_ship = slots[target_slot];
			if(bool(target_ship.ship)) {
				progress = state.world.ship_get_org(target_ship.ship);
			}

		}
	}
};




class naval_combat_main_ship_str_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		if(bool(ship.ship)) {
			progress = state.world.ship_get_strength(ship.ship);
		}
	}
};

class naval_combat_target_ship_str_bar : public vertical_progress_bar {
public:

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		if(target_slot > -1 && target_slot < uint16_t(slots.size())) {
			auto target_ship = slots[target_slot];
			if(bool(target_ship.ship)) {
				vertical_progress_bar::render(state, x, y);
			}

		}
	}

	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		if(target_slot > -1 && target_slot < uint16_t(slots.size())) {
			auto target_ship = slots[target_slot];
			if(bool(target_ship.ship)) {
				progress = state.world.ship_get_strength(target_ship.ship);
			}

		}
	}
};


class naval_combat_target_ship_org_str_frame : public image_element_base {
public:

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		if(target_slot > -1 && target_slot < uint16_t(slots.size())) {
			auto target_ship = slots[target_slot];
			if(bool(target_ship.ship)) {
				image_element_base::render(state, x, y);
			}

		}
	}
};


template<bool Attacker>
class naval_main_ship_combat_slot :public  window_element_base {
public:


	/*tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);

		military::ship_in_battle ship = retrieve<military::ship_in_battle>(state, parent);
		text::add_line(state, contents, "ORG_NAME", text::variable_type::val, text::fp_percentage_two_places{ state.world.ship_get_org(ship.ship) });
		text::add_line(state, contents, "alice_str_name", text::variable_type::val, text::fp_percentage_two_places{ state.world.ship_get_strength(ship.ship) });
#ifndef NDEBUG
		text::add_line(state, contents, "Ship ID: " + std::to_string(ship.ship.value));
		if(ship.target_slot > -1 && ship.target_slot < int16_t(slots.size())) {
			text::add_line(state, contents, "Target ship ID: " + std::to_string(slots[ship.target_slot].ship.value));
		} else {
			text::add_line(state, contents, "No or invalid target");
		}
		text::add_line(state, contents, "Target Slot ID: " + std::to_string(ship.target_slot));

#endif


	}*/

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "attacker_icon_normal") {
			return make_element_by_type<naval_combat_main_ship_stance<ship_stance::normal, Attacker>>(state, id);
		} else if(name == "attacker_icon_attacking") {
			return make_element_by_type<naval_combat_main_ship_stance<ship_stance::engaged, Attacker>>(state, id);
		}
		else if(name == "attacker_icon_sunk") {
			return make_element_by_type<naval_combat_main_ship_stance<ship_stance::sunk, Attacker>>(state, id);
		}
		else if(name == "attacker_icon_retreat") {
			return make_element_by_type<naval_combat_main_ship_stance<ship_stance::retreating, Attacker>>(state, id);
		}
		else if(name == "attacker_icon_disengaged") {
			return make_element_by_type<naval_combat_main_ship_stance<ship_stance::disengaged, Attacker>>(state, id);
		}
		else if(name == "attacker_icon_attacking") {
			return make_element_by_type<naval_combat_main_ship_stance<ship_stance::engaged, Attacker>>(state, id);
		}
		/*else if(name == "seeking_target") {
			return make_element_by_type<naval_combat_attacker_ship_stance<ship_stance::seeking, Type>>(state, id);
		}*/
		else if(name == "str") {
			return make_element_by_type<naval_combat_main_ship_str_bar>(state, id);
		}
		else if(name == "org") {
			return make_element_by_type<naval_combat_main_ship_org_bar>(state, id);
		}
		else if(name == "mini_frame_str") {
			return make_element_by_type<image_element_base>(state, id);
		}
		else if(name == "mini_frame_org") {
			return make_element_by_type<image_element_base>(state, id);
		}
		else {
			return nullptr;
		}
	}
};


template<bool Attacker>
class naval_target_ship_combat_slot :public  window_element_base {
public:


	/*tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);

		military::ship_in_battle ship = retrieve<military::ship_in_battle>(state, parent);
		if(ship.target_slot > -1 && ship.target_slot < int16_t(slots.size())) {
			auto target_ship = slots[ship.target_slot];
			text::add_line(state, contents, "ORG_NAME", text::variable_type::val, text::fp_percentage_two_places{ state.world.ship_get_org(target_ship.ship) });
			text::add_line(state, contents, "alice_str_name", text::variable_type::val, text::fp_percentage_two_places{ state.world.ship_get_strength(target_ship.ship) });
#ifndef NDEBUG
			text::add_line(state, contents, "Ship ID :" + std::to_string(target_ship.ship.value));
			if(target_ship.target_slot > -1 && target_ship.target_slot < int16_t(slots.size())) {
				text::add_line(state, contents, "Target ship ID: " + std::to_string(slots[target_ship.target_slot].ship.value));
			} else {
				text::add_line(state, contents, "No or invalid target");
			}
			text::add_line(state, contents, "Target Slot ID :" + std::to_string(target_ship.target_slot));

#endif
		}

	}*/

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "attacker_icon_normal") {
			return make_element_by_type<naval_combat_target_ship_stance<ship_stance::normal, Attacker>>(state, id);
		} else if(name == "attacker_icon_attacking") {
			return make_element_by_type<naval_combat_target_ship_stance<ship_stance::engaged, Attacker>>(state, id);
		} else if(name == "attacker_icon_sunk") {
			return make_element_by_type<naval_combat_target_ship_stance<ship_stance::sunk, Attacker>>(state, id);
		} else if(name == "attacker_icon_retreat") {
			return make_element_by_type<naval_combat_target_ship_stance<ship_stance::retreating, Attacker>>(state, id);
		} else if(name == "attacker_icon_disengaged") {
			return make_element_by_type<naval_combat_target_ship_stance<ship_stance::disengaged, Attacker>>(state, id);
		} else if(name == "attacker_icon_attacking") {
			return make_element_by_type<naval_combat_target_ship_stance<ship_stance::engaged, Attacker>>(state, id);
		}
		/*else if(name == "seeking_target") {
			return make_element_by_type<naval_combat_attacker_ship_stance<ship_stance::seeking, Type>>(state, id);
		}*/
		else if(name == "str") {
			return make_element_by_type<naval_combat_target_ship_str_bar>(state, id);
		} else if(name == "org") {
			return make_element_by_type<naval_combat_target_ship_org_bar>(state, id);
		}
		else if(name == "mini_frame_str") {
			return make_element_by_type<naval_combat_target_ship_org_str_frame>(state, id);
		}
		else if(name == "mini_frame_org") {
			return make_element_by_type<naval_combat_target_ship_org_str_frame>(state, id);
		}
		else {
			return nullptr;
		}
	}
};



class naval_combat_ship_status : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		switch(ship.flags & military::ship_in_battle::mode_mask) {
		case military::ship_in_battle::mode_approaching:
			set_text(state, "Approaching");
			break;
		case military::ship_in_battle::mode_engaged:
			set_text(state, "Engaged");
			break;
		case military::ship_in_battle::mode_retreated:
			set_text(state, "Disengaged");
			break;
		case military::ship_in_battle::mode_retreating:
			set_text(state, "Retreating");
			break;
		case military::ship_in_battle::mode_seeking:
			set_text(state, "Seeking target");
			break;
		case military::ship_in_battle::mode_sunk:
			set_text(state, "Sunk");
			break;
		default:
			assert(false);
		}
		
		
	}
	
};
template<bool Attacker>
class naval_combat_ship_range_icon : public image_element_base {
public:

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		uint16_t mode = ship.flags & military::ship_in_battle::mode_mask;
		if(bool(ship.ship) && (mode == military::ship_in_battle::mode_approaching || mode == military::ship_in_battle::mode_engaged)) {
			auto controller = state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(ship.ship));
			auto type = state.world.ship_get_type(ship.ship);
			const auto& stats = state.world.nation_get_unit_stats(controller, type);
			auto ship_pixel_pos = navy_battle_distance_to_pixels(ship.get_distance());
			auto max_range_pos = navy_battle_distance_to_pixels(military::naval_battle_distance_to_center - uint16_t(stats.reconnaissance_or_fire_range * military::naval_battle_distance_to_center));
			if constexpr(Attacker) {
				x = x + uint32_t(ship_pixel_pos + max_range_pos);
			}
			else {
				x = x - uint32_t(ship_pixel_pos + max_range_pos);
			}
			image_element_base::render(state, x, y);
		}
	}
};

class naval_combat_target_ship_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		if(target_slot > -1 && target_slot < uint16_t(slots.size())) {
			auto target_ship = slots[target_slot];
			if(bool(target_ship.ship)) {
				auto name = state.world.ship_get_name(target_ship.ship);		
				set_text(state, text::produce_simple_string(state, state.to_string_view(name)));
			}
			else {
				set_text(state, "");
			}

		}
		else {
			set_text(state, "");
		}
	}
};

class naval_combat_main_ship_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		if(bool(ship.ship)) {
			auto name = state.world.ship_get_name(ship.ship);
			set_text(state, text::produce_simple_string(state, state.to_string_view(name)));
		}
	}
};


class naval_combat_main_ship_flag : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		if(bool(ship.ship)) {
			auto controller = state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(ship.ship));
			auto ident = state.world.nation_get_identity_from_identity_holder(controller);
			auto ii = state.world.national_identity_get_identifying_int(ident);
			set_text(state, "@" + nations::int_to_tag(ii));
		}
		else {
			set_text(state, "");
		}
	}
};

class naval_combat_target_ship_flag : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto ship = retrieve<military::ship_in_battle>(state, parent);
		auto battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(battle);
		auto target_slot = ship.target_slot;
		if(target_slot > -1 && target_slot < uint16_t(slots.size())) {
			auto target_ship = slots[target_slot];
			if(bool(target_ship.ship)) {
				auto controller = state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(target_ship.ship));
				auto ident = state.world.nation_get_identity_from_identity_holder(controller);
				auto ii = state.world.national_identity_get_identifying_int(ident);
				set_text(state, "@" + nations::int_to_tag(ii));
			}
			else {
				set_text(state, "");
			}

		}
		else {
			set_text(state, "");
		}
	}
};


class naval_combat_defender_ship_slot_element : public listbox_row_element_base<military::ship_in_battle> {

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "highlight") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "right_slot") {
			return make_element_by_type<naval_main_ship_combat_slot<false>>(state, id);
		} else if(name == "left_slot") {
			return make_element_by_type<naval_target_ship_combat_slot<false>>(state, id);
		} else if(name == "attacker_name") {
			return make_element_by_type<naval_combat_target_ship_name>(state, id);
		} else if(name == "defender_name") {
			return make_element_by_type<naval_combat_main_ship_name>(state, id);
		} else if(name == "attacker_flag") {
			return make_element_by_type<naval_combat_target_ship_flag>(state, id);
		} else if(name == "defender_flag") {
			return make_element_by_type<naval_combat_main_ship_flag>(state, id);
		} else if(name == "status") {
			return make_element_by_type<naval_combat_ship_status>(state, id);
		} else if(name == "range_r") {
			return make_element_by_type<naval_combat_ship_range_icon<false>>(state, id);
		} else if(name == "range_l") {
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}

	}
};


class naval_combat_attacker_ship_slot_element : public listbox_row_element_base<military::ship_in_battle> {

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<opaque_element_base>(state, id);
		}
		else if(name == "highlight") {
			return make_element_by_type<image_element_base>(state, id);
		}
		else if(name == "right_slot") {
			return make_element_by_type<naval_target_ship_combat_slot<true>>(state, id);
		}
		else if(name == "left_slot") {
			return make_element_by_type<naval_main_ship_combat_slot<true>>(state, id);
		}
		else if(name == "attacker_name") {
			return make_element_by_type<naval_combat_main_ship_name>(state, id);
		}
		else if(name == "defender_name") {
			return make_element_by_type<naval_combat_target_ship_name>(state, id);
		}
		else if(name == "attacker_flag") {
			return make_element_by_type<naval_combat_main_ship_flag>(state, id);
		}
		else if(name == "defender_flag") {
			return make_element_by_type<naval_combat_target_ship_flag>(state, id);
		}
		else if(name == "status") {
			return make_element_by_type<naval_combat_ship_status>(state, id);
		}
		else if(name == "range_r") {
			return make_element_by_type<invisible_element>(state, id);
		}
		else if(name == "range_l") {
			return make_element_by_type<naval_combat_ship_range_icon<true>>(state, id);
		}
		else {
			return nullptr;
		}
		
		
	}
};



class naval_combat_attacker_ship_slots : public listbox_element_base<naval_combat_attacker_ship_slot_element, military::ship_in_battle> {
protected:
	std::string_view get_row_element_name() override {
		return "ship_unit";
	}

public:

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		auto naval_battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(naval_battle);
		for(auto ship : slots) {
			if(!ship.ship) {
				continue;
			}
			bool is_attacker = (ship.flags & military::ship_in_battle::is_attacking) != 0;
			if(is_attacker) {
				row_contents.push_back(ship);
			}
			
		}
		update(state);
		
	}

};


class naval_combat_defender_ship_slots : public listbox_element_base<naval_combat_defender_ship_slot_element, military::ship_in_battle> {
protected:
	std::string_view get_row_element_name() override {
		return "ship_unit";
	}

public:

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		auto naval_battle = retrieve<dcon::naval_battle_id>(state, parent);
		auto slots = state.world.naval_battle_get_slots(naval_battle);
		for(auto ship : slots) {
			if(!ship.ship) {
				continue;
			}
			bool is_attacker = (ship.flags & military::ship_in_battle::is_attacking) != 0;
			if(!is_attacker) {
				row_contents.push_back(ship);
			}

		}
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
		} else if(name == "stacking_penalty_icon") {
			return make_element_by_type<nc_stacking_penalty_icon<true>>(state, id);
		} else if(name == "slots_list") {
			return make_element_by_type<naval_combat_attacker_ship_slots>(state, id);
		}

		else {
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
		} else if(name == "stacking_penalty_icon") {
			return make_element_by_type<nc_stacking_penalty_icon<false>>(state, id);
		} else if(name == "slots_list") {
			return make_element_by_type<naval_combat_defender_ship_slots>(state, id);
		}
		else {
			return nullptr;
		}
	}
};

class nc_retreat_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		for(auto n : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto navy = n.get_navy();
			if(!command::can_retreat_from_naval_battle(state, state.local_player_nation, navy, true).empty()) {
				command::retreat_from_naval_battle(state, state.local_player_nation, navy, true);
			}
			
			
			
		}
	}
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		if(!military::can_retreat_from_battle(state, b) || province::make_naval_retreat_path(state, state.local_player_nation, state.world.naval_battle_get_location_from_naval_battle_location(b)).empty()) {
			disabled = true;
			return;
		}
		for(auto n : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto navy = n.get_navy();
			if(state.world.navy_get_controller_from_navy_control(navy) == state.local_player_nation && !navy.get_is_retreating()) {
				disabled = false;
				return;
			}
		}
		disabled = true;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto b = retrieve<dcon::naval_battle_id>(state, parent);
		float current_dist_to_center = state.world.naval_battle_get_avg_distance_from_center_line(b);
		float required_dist_to_center = military::required_avg_dist_to_center_for_retreat(state);
		bool close_enough = current_dist_to_center <= required_dist_to_center;
		bool already_retreating = true;
		for(auto n : state.world.naval_battle_get_navy_battle_participation(b)) {
			auto navy = n.get_navy();
			if(state.world.navy_get_controller_from_navy_control(navy) == state.local_player_nation && !navy.get_is_retreating()) {
				already_retreating = false;
				break;
			}
		}
		bool accessible_port = !province::make_naval_retreat_path(state, state.local_player_nation, state.world.naval_battle_get_location_from_naval_battle_location(b)).empty();
		text::add_line_with_condition(state, contents, "alice_naval_retreat_condition_1", close_enough);
		text::add_line_with_condition(state, contents, "alice_naval_retreat_condition_2", !already_retreating);
		text::add_line_with_condition(state, contents, "alice_naval_retreat_condition_3", accessible_port);
		if(!close_enough) {
			text::add_line(state, contents, "alice_naval_noretreat", text::variable_type::x, text::fp_two_places{ current_dist_to_center - required_dist_to_center });
		}
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
		}
		else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(!state.world.naval_battle_is_valid(battle)) {
			set_visible(state, false);
			battle = dcon::naval_battle_id{};
		}
		sound::play_effect(state, sound::get_random_naval_battle_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
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

class nc_loss_image : public image_element_base {
	bool visible = true;
	void on_update(sys::state& state) noexcept override {
		visible = !(retrieve< military::naval_battle_report*>(state, parent)->player_on_winning_side);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};
class nc_win_image : public image_element_base {
	bool visible = true;
	void on_update(sys::state& state) noexcept override {
		visible = (retrieve< military::naval_battle_report*>(state, parent)->player_on_winning_side);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};
class nc_result_battle_name : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto loc = retrieve< military::naval_battle_report*>(state, parent)->location;
		auto name = state.world.province_get_name(loc);
		auto txt = text::produce_simple_string(state, "battle_of") + " " + text::produce_simple_string(state, name);
		set_text(state, txt);
	}
};

class nc_our_leader_img : public image_element_base {
	dcon::gfx_object_id def;

	void on_update(sys::state& state) noexcept override {
		if(!def)
			def = base_data.data.image.gfx_object;

		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		dcon::leader_id lid = we_are_attacker ? report->attacking_admiral : report->defending_admiral;

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
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		dcon::leader_id lid = we_are_attacker ? report->attacking_admiral : report->defending_admiral;
		if(!lid) {
			text::add_line(state, contents, "no_leader");
		}
		display_leader_attributes(state, lid, contents, 0);
	}
};
class nc_our_leader_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		dcon::leader_id lid = we_are_attacker ? report->attacking_admiral : report->defending_admiral;

		if(lid) {
			auto name = state.to_string_view(state.world.leader_get_name(lid));
			set_text(state, std::string(name));
		} else {
			set_text(state, text::produce_simple_string(state, "no_leader"));
		}
	}
};
class nc_their_leader_img : public image_element_base {
	dcon::gfx_object_id def;

	void on_update(sys::state& state) noexcept override {
		if(!def)
			def = base_data.data.image.gfx_object;

		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		dcon::leader_id lid = !we_are_attacker ? report->attacking_admiral : report->defending_admiral;

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
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		dcon::leader_id lid = !we_are_attacker ? report->attacking_admiral : report->defending_admiral;
		if(!lid) {
			text::add_line(state, contents, "no_leader");
		}
		display_leader_attributes(state, lid, contents, 0);
	}
};
class nc_their_leader_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		dcon::leader_id lid = !we_are_attacker ? report->attacking_admiral : report->defending_admiral;

		if(lid) {
			auto name = state.to_string_view(state.world.leader_get_name(lid));
			set_text(state, std::string(name));
		} else {
			set_text(state, text::produce_simple_string(state, "no_leader"));
		}
	}
};
class nc_our_prestige : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		auto prestige_value = report->prestige_effect;
		set_text(state, text::format_float(prestige_value, 2));
		if(prestige_value > 0) {
			color = text::text_color::green;
		} else if(prestige_value < 0) {
			color = text::text_color::red;
		} else {
			color = text::text_color::white;
		}
	}
};
class nc_their_prestige : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		auto prestige_value = -report->prestige_effect;
		set_text(state, text::format_float(prestige_value, 2));
		if(prestige_value > 0) {
			color = text::text_color::green;
		} else if(prestige_value < 0) {
			color = text::text_color::red;
		} else {
			color = text::text_color::white;
		}
	}
};
class nc_o_initial_bs : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? report->attacker_big_ships : report->defender_big_ships;
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_o_initial_ss : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? report->attacker_small_ships : report->defender_small_ships;
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_o_initial_ts : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? report->attacker_transport_ships : report->defender_transport_ships;
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_t_initial_bs : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? report->attacker_big_ships : report->defender_big_ships;
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_t_initial_ss : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? report->attacker_small_ships : report->defender_small_ships;
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_t_initial_ts : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? report->attacker_transport_ships : report->defender_transport_ships;
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_o_loss_bs : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? std::min(report->attacker_big_ships, report->attacker_big_losses) : std::min(report->defender_big_ships, report->defender_big_losses);
		set_text(state, text::prettify(-int64_t(value)));
	}
};
class nc_o_loss_ss : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? std::min(report->attacker_small_ships, report->attacker_small_losses) : std::min(report->defender_small_ships, report->defender_small_losses);
		set_text(state, text::prettify(-int64_t(value)));
	}
};
class nc_o_loss_ts : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? std::min(report->attacker_transport_ships, report->attacker_transport_losses) : std::min(report->defender_transport_ships, report->defender_transport_losses);
		set_text(state, text::prettify(-int64_t(value)));
	}
};
class nc_t_loss_bs : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? std::min(report->attacker_big_ships, report->attacker_big_losses) : std::min(report->defender_big_ships, report->defender_big_losses);
		set_text(state, text::prettify(-int64_t(value)));
	}
};
class nc_t_loss_ss : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? std::min(report->attacker_small_ships, report->attacker_small_losses) : std::min(report->defender_small_ships, report->defender_small_losses);
		set_text(state, text::prettify(-int64_t(value)));
	}
};
class nc_t_loss_ts : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? std::min(report->attacker_transport_ships, report->attacker_transport_losses) : std::min(report->defender_transport_ships, report->defender_transport_losses);
		set_text(state, text::prettify(-int64_t(value)));
	}
};
class nc_o_rem_bs : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? std::max(report->attacker_big_ships - report->attacker_big_losses, 0) : std::max(report->defender_big_ships - report->defender_big_losses, 0);
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_o_rem_ss : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? std::max(report->attacker_small_ships - report->attacker_small_losses, 0) : std::max(report->defender_small_ships - report->defender_small_losses, 0);
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_o_rem_ts : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? std::max(report->attacker_transport_ships - report->attacker_transport_losses, 0) : std::max(report->defender_transport_ships - report->defender_transport_losses, 0);
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_t_rem_bs : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? std::max(report->attacker_big_ships - report->attacker_big_losses, 0) : std::max(report->defender_big_ships - report->defender_big_losses, 0);
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_t_rem_ss : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? std::max(report->attacker_small_ships - report->attacker_small_losses, 0) : std::max(report->defender_small_ships - report->defender_small_losses, 0);
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_t_rem_ts : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? std::max(report->attacker_transport_ships - report->attacker_transport_losses, 0) : std::max(report->defender_transport_ships - report->defender_transport_losses, 0);
		set_text(state, text::prettify(int64_t(value)));
	}
};

class nc_o_initial_total : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? report->attacker_big_ships + report->attacker_small_ships + report->attacker_transport_ships : report->defender_big_ships + report->defender_small_ships + report->defender_transport_ships;
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_o_loss_total : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? std::min(report->attacker_big_ships + report->attacker_small_ships + report->attacker_transport_ships, report->attacker_big_losses + report->attacker_small_losses + report->attacker_transport_losses) : std::min(report->defender_big_ships + report->defender_small_ships + report->defender_transport_ships, report->defender_big_losses + report->defender_small_losses + report->defender_transport_losses);
		set_text(state, text::prettify(-int64_t(value)));
	}
};
class nc_o_rem_total : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = we_are_attacker ? std::max(report->attacker_big_ships + report->attacker_small_ships + report->attacker_transport_ships - (report->attacker_big_losses + report->attacker_small_losses + report->attacker_transport_losses), 0) : std::max(report->defender_big_ships + report->defender_small_ships + report->defender_transport_ships - (report->defender_big_losses + report->defender_small_losses + report->defender_transport_losses), 0);
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_t_initial_total : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? report->attacker_big_ships + report->attacker_small_ships + report->attacker_transport_ships : report->defender_big_ships + report->defender_small_ships + report->defender_transport_ships;
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_t_loss_total : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? std::min(report->attacker_big_ships + report->attacker_small_ships + report->attacker_transport_ships, report->attacker_big_losses + report->attacker_small_losses + report->attacker_transport_losses) : std::min(report->defender_big_ships + report->defender_small_ships + report->defender_transport_ships, report->defender_big_losses + report->defender_small_losses + report->defender_transport_losses);
		set_text(state, text::prettify(-int64_t(value)));
	}
};
class nc_t_rem_total : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		bool we_are_attacker = (report->attacker_won == report->player_on_winning_side);
		auto value = !we_are_attacker ? std::max(report->attacker_big_ships + report->attacker_small_ships + report->attacker_transport_ships - (report->attacker_big_losses + report->attacker_small_losses + report->attacker_transport_losses), 0) : std::max(report->defender_big_ships + report->defender_small_ships + report->defender_transport_ships - (report->defender_big_losses + report->defender_small_losses + report->defender_transport_losses), 0);
		set_text(state, text::prettify(int64_t(value)));
	}
};
class nc_goto_location_button : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		auto prov = report->location;
		if(prov && prov.value < state.province_definitions.first_sea_province.value) {
			state.map_state.set_selected_province(prov);
			static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, prov);
			if(state.map_state.get_zoom() < map::zoom_very_close)
				state.map_state.zoom = map::zoom_very_close;
			state.map_state.center_map_on_province(state, prov);
		}
	}
};

class nc_close_button : public button_element_base {
	void button_action(sys::state& state) noexcept override;
};
class nc_win_lose : public color_text_element {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		if(report->player_on_winning_side) {
			set_text(state, text::produce_simple_string(state, "you_won"));
			color = text::text_color::dark_green;
		} else {
			set_text(state, text::produce_simple_string(state, "you_lost"));
			color = text::text_color::dark_red;
		}
	}
};
class nc_warscore : public color_text_element {
	void on_update(sys::state& state) noexcept override {
		military::naval_battle_report* report = retrieve< military::naval_battle_report*>(state, parent);
		if(report->player_on_winning_side) {
			set_text(state, std::string("+") + text::format_float(report->warscore_effect, 1));
			color = text::text_color::green;
		} else {
			set_text(state, text::format_float(report->warscore_effect, 1));
			color = text::text_color::red;
		}
	}
};

class naval_combat_end_popup : public window_element_base {
public:
	military::naval_battle_report report;
	static std::vector<std::unique_ptr<ui::naval_combat_end_popup>> naval_reports_pool;


	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	static void make_new_report(sys::state& state, military::naval_battle_report const& r);
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};

inline std::vector<std::unique_ptr<ui::naval_combat_end_popup>> naval_combat_end_popup::naval_reports_pool;

inline message_result naval_combat_end_popup::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<military::naval_battle_report*>()) {
		payload.emplace<military::naval_battle_report*>(&report);
		return message_result::consumed;
	}
	return window_element_base::get(state, payload);
}

inline void naval_combat_end_popup::make_new_report(sys::state& state, military::naval_battle_report const& r) {
	if(naval_reports_pool.empty()) {
		auto new_elm = ui::make_element_by_type<ui::naval_combat_end_popup>(state, "endofnavalcombatpopup");
		auto ptr = new_elm.get();
		naval_combat_end_popup* actual = static_cast<naval_combat_end_popup*>(ptr);
		actual->report = r;
		actual->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		std::unique_ptr<naval_combat_end_popup> ptr = std::move(naval_reports_pool.back());
		naval_reports_pool.pop_back();
		ptr->report = r;
		ptr->set_visible(state, true);
		state.ui_state.root->add_child_to_front(std::move(ptr));
	}
}

inline std::unique_ptr<element_base> naval_combat_end_popup::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "background") {
		return make_element_by_type<draggable_target>(state, id);
	} else if(name == "combat_end_naval_lost") {
		return make_element_by_type<nc_loss_image>(state, id);
	} else if(name == "combat_end_naval_won") {
		return make_element_by_type<nc_win_image>(state, id);
	} else if(name == "line1") {
		return make_element_by_type<nc_result_battle_name>(state, id);
	} else if(name == "ourleader_photo") {
		return make_element_by_type<nc_our_leader_img>(state, id);
	} else if(name == "ourleader") {
		return make_element_by_type<nc_our_leader_name>(state, id);
	} else if(name == "prestige_number") {
		return make_element_by_type<nc_our_prestige>(state, id);
	} else if(name == "we_number") {
		return make_element_by_type<lc_empty_text>(state, id);
	} else if(name == "enemyleader_photo") {
		return make_element_by_type<nc_their_leader_img>(state, id);
	} else if(name == "enemyleader") {
		return make_element_by_type<nc_their_leader_name>(state, id);
	} else if(name == "prestige_number2") {
		return make_element_by_type<nc_their_prestige>(state, id);
	} else if(name == "we_number2") {
		return make_element_by_type<lc_empty_text>(state, id);
	} else if(name == "our_unit_type_1") {
		return make_element_by_type<lc_static_icon<3>>(state, id);
	} else if(name == "our_unit_type_1_1_text") {
		return make_element_by_type<nc_o_initial_bs>(state, id);
	} else if(name == "our_unit_type_1_2_text") {
		return make_element_by_type<nc_o_loss_bs>(state, id);
	} else if(name == "our_unit_type_1_3_text") {
		return make_element_by_type<nc_o_rem_bs>(state, id);
	} else if(name == "our_unit_type_2") {
		return make_element_by_type<lc_static_icon<4>>(state, id);
	} else if(name == "our_unit_type_2_1_text") {
		return make_element_by_type<nc_o_initial_ss>(state, id);
	} else if(name == "our_unit_type_2_2_text") {
		return make_element_by_type<nc_o_loss_ss>(state, id);
	} else if(name == "our_unit_type_2_3_text") {
		return make_element_by_type<nc_o_rem_ss>(state, id);
	} else if(name == "our_unit_type_3") {
		return make_element_by_type<lc_static_icon<5>>(state, id);
	} else if(name == "our_unit_type_3_1_text") {
		return make_element_by_type<nc_o_initial_ts>(state, id);
	} else if(name == "our_unit_type_3_2_text") {
		return make_element_by_type<nc_o_loss_ts>(state, id);
	} else if(name == "our_unit_type_3_3_text") {
		return make_element_by_type<nc_o_rem_ts>(state, id);
	} else if(name == "enemy_unit_type_1") {
		return make_element_by_type<lc_static_icon<3>>(state, id);
	} else if(name == "enemy_unit_type_1_1_text") {
		return make_element_by_type<nc_t_initial_bs>(state, id);
	} else if(name == "enemy_unit_type_1_2_text") {
		return make_element_by_type<nc_t_loss_bs>(state, id);
	} else if(name == "enemy_unit_type_1_3_text") {
		return make_element_by_type<nc_t_rem_bs>(state, id);
	} else if(name == "enemy_unit_type_2") {
		return make_element_by_type<lc_static_icon<4>>(state, id);
	} else if(name == "enemy_unit_type_2_1_text") {
		return make_element_by_type<nc_t_initial_ss>(state, id);
	} else if(name == "enemy_unit_type_2_2_text") {
		return make_element_by_type<nc_t_loss_ss>(state, id);
	} else if(name == "enemy_unit_type_2_3_text") {
		return make_element_by_type<nc_t_rem_ss>(state, id);
	} else if(name == "enemy_unit_type_3") {
		return make_element_by_type<lc_static_icon<5>>(state, id);
	} else if(name == "enemy_unit_type_3_1_text") {
		return make_element_by_type<nc_t_initial_ts>(state, id);
	} else if(name == "enemy_unit_type_3_2_text") {
		return make_element_by_type<nc_t_loss_ts>(state, id);
	} else if(name == "enemy_unit_type_3_3_text") {
		return make_element_by_type<nc_t_rem_ts>(state, id);
	} else if(name == "our_total_armies") {
		return make_element_by_type<nc_o_initial_total>(state, id);
	} else if(name == "our_total_loss") {
		return make_element_by_type<nc_o_loss_total>(state, id);
	} else if(name == "our_total_left") {
		return make_element_by_type<nc_o_rem_total>(state, id);
	} else if(name == "enemy_total_armies") {
		return make_element_by_type<nc_t_initial_total>(state, id);
	} else if(name == "enemy_total_loss") {
		return make_element_by_type<nc_t_loss_total>(state, id);
	} else if(name == "enemy_total_left") {
		return make_element_by_type<nc_t_rem_total>(state, id);
	} else if(name == "agreebutton") {
		auto ptr = make_element_by_type<nc_close_button>(state, id);
		ptr->base_data.position.y += 146; // Nudge
		return ptr;
	} else if(name == "declinebutton") {
		auto ptr = make_element_by_type<nc_goto_location_button>(state, id);
		ptr->base_data.position.y += 146; // Nudge
		ptr->set_button_text(state, text::produce_simple_string(state, "landbattleover_btn3"));
		return ptr;
	} else if(name == "centerok") {
		return make_element_by_type<invisible_element>(state, id);
	} else if(name == "warscore") {
		return make_element_by_type<nc_warscore>(state, id);
	} else if(name == "winorlose") {
		return make_element_by_type<nc_win_lose>(state, id);
	} else {
		return nullptr;
	}
}

inline void nc_close_button::button_action(sys::state& state) noexcept {
	parent->set_visible(state, false);
	auto uptr = state.ui_state.root->remove_child(parent);
	assert(uptr);
	std::unique_ptr<naval_combat_end_popup> ptr(static_cast<naval_combat_end_popup*>(uptr.release()));
	naval_combat_end_popup::naval_reports_pool.push_back(std::move(ptr));
}

} // namespace ui
