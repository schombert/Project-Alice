#pragma once

#include "gui_element_types.hpp"
#include "gui_province_window.hpp"
#include "military.hpp"

namespace ui {


class lc_attacker_leader_img : public image_element_base {
	dcon::gfx_object_id def;

	void on_update(sys::state& state) noexcept override {
		if(!def)
			def = base_data.data.image.gfx_object;

		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto lid = state.world.land_battle_get_general_from_attacking_general(b);

		if(!lid) {
			base_data.data.image.gfx_object = def;
			return;
		}

		auto owner = state.world.leader_get_nation_from_leader_loyalty(lid);
		auto pculture = state.world.nation_get_primary_culture(owner);
		auto ltype = pculture.get_group_from_culture_group_membership().get_leader();

		if(ltype) {
			auto grange = ltype.get_generals();
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
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto lid = state.world.land_battle_get_general_from_attacking_general(b);

		if(lid)
			display_leader_attributes(state, lid, contents, 0);
		else
			text::add_line(state, contents, "no_leader");
	}
};
class lc_defending_leader_img : public image_element_base {
	dcon::gfx_object_id def;

	void on_update(sys::state& state) noexcept override {
		if(!def)
			def = base_data.data.image.gfx_object;

		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto lid = state.world.land_battle_get_general_from_defending_general(b);

		if(!lid) {
			base_data.data.image.gfx_object = def;
			return;
		}

		auto owner = state.world.leader_get_nation_from_leader_loyalty(lid);
		auto pculture = state.world.nation_get_primary_culture(owner);
		auto ltype = pculture.get_group_from_culture_group_membership().get_leader();

		if(ltype) {
			auto grange = ltype.get_generals();
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
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto lid = state.world.land_battle_get_general_from_defending_general(b);

		if(lid)
			display_leader_attributes(state, lid, contents, 0);
		else
			text::add_line(state, contents, "no_leader");
	}
};

class lc_attacking_leader_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto lid = state.world.land_battle_get_general_from_attacking_general(b);

		if(lid) {
			auto name = state.to_string_view(state.world.leader_get_name(lid));
			set_text(state, std::string(name));
		} else {
			set_text(state, "");
		}
	}
};
class lc_defending_leader_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto lid = state.world.land_battle_get_general_from_defending_general(b);

		if(lid) {
			auto name = state.to_string_view(state.world.leader_get_name(lid));
			set_text(state, std::string(name));
		} else {
			set_text(state, "");
		}
	}
};

class lc_attacker_flag : public flag_button {
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
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto n = military::get_land_battle_lead_attacker(state, b);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
};
class lc_defender_flag : public flag_button {
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
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto n = military::get_land_battle_lead_defender(state, b);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
};

class lc_defender_org : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner)  == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == false) {
				for(auto r : a.get_army().get_army_membership()) {
					++count;
					total += r.get_regiment().get_org();
				}
			}
		}
		progress = count > 0 ? total / count : 0.0f;
	}
};
class lc_defender_str : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == false) {
				for(auto r : a.get_army().get_army_membership()) {
					++count;
					total += r.get_regiment().get_strength();
				}
			}
		}
		progress = count > 0 ? total / count : 0.0f;
	}
};
class lc_attacker_org : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == true) {
				for(auto r : a.get_army().get_army_membership()) {
					++count;
					total += r.get_regiment().get_org();
				}
			}
		}
		progress = count > 0 ? total / count : 0.0f;
	}
};
class lc_attacker_str : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == true) {
				for(auto r : a.get_army().get_army_membership()) {
					++count;
					total += r.get_regiment().get_strength();
				}
			}
		}
		progress = count > 0 ? total / count : 0.0f;
	}
};

class lc_defender_org_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == false) {
				for(auto r : a.get_army().get_army_membership()) {
					++count;
					total += r.get_regiment().get_org();
				}
			}
		}
		set_text(state, text::format_percentage(count > 0 ? total / count : 0.0f, 1));
	}
};
class lc_defender_str_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == false) {
				for(auto r : a.get_army().get_army_membership()) {
					++count;
					total += r.get_regiment().get_strength();
				}
			}
		}
		set_text(state, text::format_percentage(count > 0 ? total / count : 0.0f, 1));
	}
};
class lc_attacker_org_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == true) {
				for(auto r : a.get_army().get_army_membership()) {
					++count;
					total += r.get_regiment().get_org();
				}
			}
		}
		set_text(state, text::format_percentage(count > 0 ? total / count : 0.0f, 1));
	}
};
class lc_attacker_str_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float count = 0.0f;
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == true) {
				for(auto r : a.get_army().get_army_membership()) {
					++count;
					total += r.get_regiment().get_strength();
				}
			}
		}
		set_text(state, text::format_percentage(count > 0 ? total / count : 0.0f, 1));
	}
};

template<int32_t index>
class lc_static_icon : public image_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		image_element_base::on_create(state);
		frame = index;
	}
};

class lc_defender_inf_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == false) {
				for(auto r : a.get_army().get_army_membership()) {
					auto t = r.get_regiment().get_type();
					if(t && state.military_definitions.unit_base_definitions[t].type == military::unit_type::infantry) {
						total += r.get_regiment().get_strength();
					}
				}
			}
		}
		set_text(state, text::prettify(uint64_t(total * state.defines.pop_size_per_regiment)));
	}
};
class lc_attacker_inf_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == true) {
				for(auto r : a.get_army().get_army_membership()) {
					auto t = r.get_regiment().get_type();
					if(t && state.military_definitions.unit_base_definitions[t].type == military::unit_type::infantry) {
						total += r.get_regiment().get_strength();
					}
				}
			}
		}
		set_text(state, text::prettify(uint64_t(total * state.defines.pop_size_per_regiment)));
	}
};
class lc_defender_cav_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == false) {
				for(auto r : a.get_army().get_army_membership()) {
					auto t = r.get_regiment().get_type();
					if(t && state.military_definitions.unit_base_definitions[t].type == military::unit_type::cavalry) {
						total += r.get_regiment().get_strength();
					}
				}
			}
		}
		set_text(state, text::prettify(uint64_t(total * state.defines.pop_size_per_regiment)));
	}
};
class lc_attacker_cav_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == true) {
				for(auto r : a.get_army().get_army_membership()) {
					auto t = r.get_regiment().get_type();
					if(t && state.military_definitions.unit_base_definitions[t].type == military::unit_type::cavalry) {
						total += r.get_regiment().get_strength();
					}
				}
			}
		}
		set_text(state, text::prettify(uint64_t(total * state.defines.pop_size_per_regiment)));
	}
};
class lc_defender_art_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == false) {
				for(auto r : a.get_army().get_army_membership()) {
					auto t = r.get_regiment().get_type();
					if(t && (state.military_definitions.unit_base_definitions[t].type == military::unit_type::support || state.military_definitions.unit_base_definitions[t].type == military::unit_type::special)) {
						total += r.get_regiment().get_strength();
					}
				}
			}
		}
		set_text(state, text::prettify(uint64_t(total * state.defines.pop_size_per_regiment)));
	}
};
class lc_attacker_art_txt : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto w = state.world.land_battle_get_war_from_land_battle_in_war(b);
		float total = 0.0f;
		for(auto a : state.world.land_battle_get_army_battle_participation(b)) {
			auto owner = a.get_army().get_controller_from_army_control();
			bool battle_attacker = false;
			if(w) {
				battle_attacker = (military::get_role(state, w, owner) == military::war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
			} else {
				battle_attacker = !bool(owner) == state.world.land_battle_get_war_attacker_is_attacker(b);
			}
			if(battle_attacker == true) {
				for(auto r : a.get_army().get_army_membership()) {
					auto t = r.get_regiment().get_type();
					if(t && (state.military_definitions.unit_base_definitions[t].type == military::unit_type::support || state.military_definitions.unit_base_definitions[t].type == military::unit_type::special)) {
						total += r.get_regiment().get_strength();
					}
				}
			}
		}
		set_text(state, text::prettify(uint64_t(total * state.defines.pop_size_per_regiment)));
	}
};

enum class lc_mod_type {
	terrain, river, dice, digin, leader, gas
};
struct lc_modifier_data {
	lc_mod_type type = lc_mod_type::terrain;
	int32_t value = 0;
};

class lc_modifier_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		lc_modifier_data dat = retrieve< lc_modifier_data>(state, parent);
		switch(dat.type) {
			case lc_mod_type::terrain:
				frame = 3;
				break;
			case lc_mod_type::river:
				frame = 2;
				break;
			case lc_mod_type::dice:
				frame = 0;
				break;
			case lc_mod_type::digin:
				frame = 1;
				break;
			case lc_mod_type::leader:
				frame = 4;
				break;
			case lc_mod_type::gas:
				frame = 5;
				break;
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		lc_modifier_data dat = retrieve< lc_modifier_data>(state, parent);
		switch(dat.type) {
			case lc_mod_type::terrain:
				text::add_line(state, contents, "combat_terrain");
				break;
			case lc_mod_type::river:
				text::add_line(state, contents, "combat_crossing");
				break;
			case lc_mod_type::dice:
				text::add_line(state, contents, "combat_dice");
				break;
			case lc_mod_type::digin:
				text::add_line(state, contents, "combat_digin");
				break;
			case lc_mod_type::leader:
				text::add_line(state, contents, "combat_leader_mod");
				break;
			case lc_mod_type::gas:
				text::add_line(state, contents, "combat_gas");
				break;
		}
	}
};

class lc_modifier_value : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		lc_modifier_data dat = retrieve< lc_modifier_data>(state, parent);
		set_text(state, std::to_string(dat.value));
	}
};

class lc_modifier : public window_element_base {
public:
	lc_modifier_data data;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "modifier_icon") {
			return make_element_by_type<lc_modifier_icon>(state, id);
		} else if(name == "modifier_value") {
			return make_element_by_type<lc_modifier_value>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<lc_modifier_data>()) {
			payload.emplace<lc_modifier_data>(data);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class defender_combat_modifiers : public overlapping_listbox_element_base<lc_modifier, lc_modifier_data> {
	std::string_view get_row_element_name() override {
		return "alice_combat_modifier";
	}
	void update_subwindow(sys::state& state, lc_modifier& subwindow, lc_modifier_data content) override {
		subwindow.data = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto both_dice = state.world.land_battle_get_dice_rolls(b);
		auto defender_mods = state.world.land_battle_get_defender_bonus(b);
		auto dig_in_value = defender_mods & military::defender_bonus_dig_in_mask;

		auto attacking_nation = military::get_land_battle_lead_attacker(state, b);
		auto defending_nation = military::get_land_battle_lead_defender(state, b);

		bool defender_gas =
			state.world.nation_get_has_gas_attack(defending_nation) && !state.world.nation_get_has_gas_defense(attacking_nation);

		auto defender_dice = (both_dice >> 4) & 0x0F;

		auto location = state.world.land_battle_get_location_from_land_battle_location(b);
		auto terrain_bonus = state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::defense);

		auto defender_per = state.world.leader_get_personality(state.world.land_battle_get_general_from_attacking_general(b));
		auto defender_bg = state.world.leader_get_background(state.world.land_battle_get_general_from_attacking_general(b));

		auto defence_bonus =
			int32_t(state.world.leader_trait_get_defense(defender_per) + state.world.leader_trait_get_defense(defender_bg));

		row_contents.push_back(lc_modifier_data{ lc_mod_type::dice, defender_dice });
		if(dig_in_value != 0)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::digin, dig_in_value });
		if(terrain_bonus != 0)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::digin, int32_t(terrain_bonus) });
		if(defence_bonus != 0)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::leader, defence_bonus });
		if(defender_gas)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::gas, int32_t(state.defines.gas_attack_modifier) });

		update(state);
	}
};
class attacker_combat_modifiers : public overlapping_listbox_element_base<lc_modifier, lc_modifier_data> {
	std::string_view get_row_element_name() override {
		return "alice_combat_modifier";
	}
	void update_subwindow(sys::state& state, lc_modifier& subwindow, lc_modifier_data content) override {
		subwindow.data = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto both_dice = state.world.land_battle_get_dice_rolls(b);
		auto defender_mods = state.world.land_battle_get_defender_bonus(b);
		auto crossing_value = defender_mods & military::defender_bonus_crossing_mask;

		auto attacking_nation = military::get_land_battle_lead_attacker(state, b);
		auto defending_nation = military::get_land_battle_lead_defender(state, b);

		bool attacker_gas =
			state.world.nation_get_has_gas_attack(attacking_nation) && !state.world.nation_get_has_gas_defense(defending_nation);
		bool defender_gas =
			state.world.nation_get_has_gas_attack(defending_nation) && !state.world.nation_get_has_gas_defense(attacking_nation);

		int32_t crossing_adjustment =
			(crossing_value == military::defender_bonus_crossing_none ? 0 : (crossing_value == military::defender_bonus_crossing_river ? -1 : -2));

		auto attacker_dice = both_dice & 0x0F;

		auto attacker_per = state.world.leader_get_personality(state.world.land_battle_get_general_from_attacking_general(b));
		auto attacker_bg = state.world.leader_get_background(state.world.land_battle_get_general_from_attacking_general(b));

		auto attack_bonus =
			int32_t(state.world.leader_trait_get_attack(attacker_per) + state.world.leader_trait_get_attack(attacker_bg));

		row_contents.push_back(lc_modifier_data{lc_mod_type::dice, attacker_dice });
		if(crossing_adjustment != 0)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::river, crossing_adjustment });
		if(attack_bonus != 0)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::leader, attack_bonus });
		if(attacker_gas)
			row_contents.push_back(lc_modifier_data{ lc_mod_type::gas, int32_t(state.defines.gas_attack_modifier) });

		update(state);
	}
};

class land_combat_defender_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<lc_defender_flag>(state, id);
		} else if(name == "leader_icon") {
			return make_element_by_type<lc_defending_leader_img>(state, id);
		} else if(name == "leader_name") {
			return make_element_by_type<lc_defending_leader_name>(state, id);
		} else if(name == "morale") {
			return make_element_by_type<lc_defender_org>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<lc_defender_str>(state, id);
		} else if(name == "morale_text") {
			return make_element_by_type<lc_defender_org_txt>(state, id);
		} else if(name == "strength_text") {
			return make_element_by_type<lc_defender_str_txt>(state, id);
		} else if(name == "unit_type_1") {
			return make_element_by_type<lc_static_icon<0>>(state, id);
		} else if(name == "unit_type_2") {
			return make_element_by_type<lc_static_icon<1>>(state, id);
		} else if(name == "unit_type_3") {
			return make_element_by_type<lc_static_icon<2>>(state, id);
		} else if(name == "unit_type_1_value") {
			return make_element_by_type<lc_defender_inf_txt>(state, id);
		} else if(name == "unit_type_2_value") {
			return make_element_by_type<lc_defender_cav_txt>(state, id);
		} else if(name == "unit_type_3_value") {
			return make_element_by_type<lc_defender_art_txt>(state, id);
		} else if(name == "modifiers") {
			return make_element_by_type<defender_combat_modifiers>(state, id);
		} else {
			return nullptr;
		}
	}
};

class land_combat_attacker_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<lc_attacker_flag>(state, id);
		} else if(name == "leader_icon") {
			return make_element_by_type<lc_attacker_leader_img>(state, id);
		} else if(name == "leader_name") {
			return make_element_by_type<lc_attacking_leader_name>(state, id);
		} else if(name == "morale") {
			return make_element_by_type<lc_attacker_org>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<lc_attacker_str>(state, id);
		} else if(name == "morale_text") {
			return make_element_by_type<lc_attacker_org_txt>(state, id);
		} else if(name == "strength_text") {
			return make_element_by_type<lc_attacker_str_txt>(state, id);
		} else if(name == "unit_type_1") {
			return make_element_by_type<lc_static_icon<0>>(state, id);
		} else if(name == "unit_type_2") {
			return make_element_by_type<lc_static_icon<1>>(state, id);
		} else if(name == "unit_type_3") {
			return make_element_by_type<lc_static_icon<2>>(state, id);
		} else if(name == "unit_type_1_value") {
			return make_element_by_type<lc_attacker_inf_txt>(state, id);
		} else if(name == "unit_type_2_value") {
			return make_element_by_type<lc_attacker_cav_txt>(state, id);
		} else if(name == "unit_type_3_value") {
			return make_element_by_type<lc_attacker_art_txt>(state, id);
		} else if(name == "modifiers") {
			return make_element_by_type<attacker_combat_modifiers>(state, id);
		} else {
			return nullptr;
		}
	}
};

class lc_retreat_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		disabled = !command::can_retreat_from_land_battle(state, state.local_player_nation, retrieve<dcon::land_battle_id>(state, parent));
	}
	void button_action(sys::state& state) noexcept override {
		command::retreat_from_land_battle(state, state.local_player_nation, retrieve<dcon::land_battle_id>(state, parent));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		text::add_line(state, contents, "alice_retreat");
		text::add_line_with_condition(state, contents, "alice_retreat_1",
			state.world.land_battle_get_start_date(b) + military::days_before_retreat < state.current_date,
			text::variable_type::x, military::days_before_retreat);
		text::add_line_with_condition(state, contents, "alice_retreat_2",
			state.local_player_nation == military::get_land_battle_lead_attacker(state, b)
			|| state.local_player_nation == military::get_land_battle_lead_defender(state, b));
	}
};

class lbattle_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		auto loc = state.world.land_battle_get_location_from_land_battle_location(b);
		auto name = state.world.province_get_name(loc);
		auto txt = text::produce_simple_string(state, "battle_of") + " " + text::produce_simple_string(state, name);
		set_text(state, txt);
	}
};

enum class regiment_rank {
	attacker_front, attacker_back, defender_front, defender_back
};

class counter_ico : public image_element_base {
public:
	regiment_rank rank;
	int32_t slot = 0;
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		auto b = retrieve<dcon::land_battle_id>(state, parent);
		switch(rank) {
			case regiment_rank::attacker_front:
			{
				auto& row = state.world.land_battle_get_attacker_front_line(b);
				auto reg = row.at(slot);
				if(!reg) {
					visible = false;
				} else {
					visible = true;
					auto type = state.world.regiment_get_type(reg);
					if(type) {
						if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::infantry) {
							frame = 0;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::cavalry) {
							frame = 1;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::special) {
							frame = 2;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::support) {
							frame = 3;
						}
					}
				}
			}
				break;
			case regiment_rank::attacker_back:
			{
				auto& row = state.world.land_battle_get_attacker_back_line(b);
				auto reg = row.at(slot);
				if(!reg) {
					visible = false;
				} else {
					visible = true;
					auto type = state.world.regiment_get_type(reg);
					if(type) {
						if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::infantry) {
							frame = 0;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::cavalry) {
							frame = 1;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::special) {
							frame = 2;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::support) {
							frame = 3;
						}
					}
				}
			}
				break;
			case regiment_rank::defender_front:
			{
				auto& row = state.world.land_battle_get_defender_front_line(b);
				auto reg = row.at(slot);
				if(!reg) {
					visible = false;
				} else {
					visible = true;
					auto type = state.world.regiment_get_type(reg);
					if(type) {
						if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::infantry) {
							frame = 0;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::cavalry) {
							frame = 1;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::special) {
							frame = 2;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::support) {
							frame = 3;
						}
					}
				}
			}
				break;
			case regiment_rank::defender_back:
			{
				auto& row = state.world.land_battle_get_defender_back_line(b);
				auto reg = row.at(slot);
				if(!reg) {
					visible = false;
				} else {
					visible = true;
					auto type = state.world.regiment_get_type(reg);
					if(type) {
						if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::infantry) {
							frame = 0;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::cavalry) {
							frame = 1;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::special) {
							frame = 2;
						} else if(state.military_definitions.unit_base_definitions[type].type == military::unit_type::support) {
							frame = 3;
						}
					}
				}
			}
				break;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class land_combat_window : public window_element_base {
public:
	dcon::land_battle_id battle;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		state.ui_state.army_combat_window = this;

		auto def = state.ui_state.defs_by_name.find("counter")->second.definition;
		for(int32_t i = 0; i < 30; ++i) {
			int32_t s = (i < 15) ? 28 - i * 2 : (i - 15) * 2 + 1;
			{
				auto win = make_element_by_type<counter_ico>(state, def);
				win->slot = s;
				win->rank = regiment_rank::attacker_back;
				win->base_data.position.x = int16_t(21 + i * 12);
				win->base_data.position.y = int16_t(231);
				add_child_to_front(std::move(win));
			}
			{
				auto win = make_element_by_type<counter_ico>(state, def);
				win->slot = s;
				win->rank = regiment_rank::attacker_front;
				win->base_data.position.x = int16_t(21 + i * 12);
				win->base_data.position.y = int16_t(231 + 12);
				add_child_to_front(std::move(win));
			}
			{
				auto win = make_element_by_type<counter_ico>(state, def);
				win->slot = s;
				win->rank = regiment_rank::defender_front;
				win->base_data.position.x = int16_t(21 + i * 12);
				win->base_data.position.y = int16_t(321);
				add_child_to_front(std::move(win));
			}
			{
				auto win = make_element_by_type<counter_ico>(state, def);
				win->slot = s;
				win->rank = regiment_rank::defender_back;
				win->base_data.position.x = int16_t(21 + i * 12);
				win->base_data.position.y = int16_t(321 + 12);
				add_child_to_front(std::move(win));
			}
		}

	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "combat_bg") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "combat_terrain1") {
			return make_element_by_type<province_terrain_image>(state, id);
		} else if(name == "label_battlename") {
			return make_element_by_type<lbattle_name>(state, id);
		} else if(name == "combat_retreat") {
			return make_element_by_type<lc_retreat_button>(state, id);
		} else if(name == "closebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "attacker") {
			return make_element_by_type<land_combat_attacker_window>(state, id);
		} else if(name == "defender") {
			return make_element_by_type<land_combat_defender_window>(state, id);

		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::land_battle_id>()) {
			payload.emplace<dcon::land_battle_id>(battle);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(state.world.land_battle_get_location_from_land_battle_location(battle));
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

//===============================================================================================================================

class land_combat_end_popup : public window_element_base {
private:
	simple_text_element_base* winorlose_text = nullptr;
	simple_text_element_base* warscore_text = nullptr;

public:
	void on_update(sys::state& state) noexcept override {
		winorlose_text->set_text(state, "UwU");
		warscore_text->set_text(state, "69");
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			// ptr->base_data.size = base_data.size; // Nudge
			return ptr;
		} else if(name == "combat_end_naval_lost") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "combat_end_naval_won") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "line1") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "ourleader_photo") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "ourleader") { // Our great leader, the leader of the universe, Puffy, Puffy the cat, tremble in her might
			return make_element_by_type<simple_text_element_base>(state, id);
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
			return make_element_by_type<simple_text_element_base>(state, id);
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
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "our_unit_type_1_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_1_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_1_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "our_unit_type_2_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_2_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_2_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_3") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "our_unit_type_3_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_3_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_3_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_1") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "enemy_unit_type_1_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_1_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_1_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "enemy_unit_type_2_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_2_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_2_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_3") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "enemy_unit_type_3_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_3_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_3_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_total_armies") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_total_loss") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_total_left") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_total_armies") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_total_loss") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_total_left") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "agreebutton") {
			auto ptr = make_element_by_type<generic_close_button>(state, id);
			ptr->base_data.position.y += 146; // Nudge
			return ptr;
		} else if(name == "declinebutton") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.y += 146; // Nudge
			ptr->set_button_text(state, text::produce_simple_string(state, "landbattleover_btn3"));
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
};

} // namespace ui
