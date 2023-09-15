#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "gui_element_types.hpp"
#include "gui_common_elements.hpp"
#include "gui_graphics.hpp"
#include "gui_population_window.hpp"
#include "nations.hpp"
#include "province.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "gui_production_window.hpp"

namespace ui {

class province_liferating : public progress_bar {
public:

	void on_update(sys::state& state) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		progress = state.world.province_get_life_rating(prov_id) / 100.f;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id prov_id = Cyto::any_cast<dcon::province_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box, std::string_view("provinceview_liferating"),
					text::variable_type::value, text::fp_one_place{float(state.world.province_get_life_rating(prov_id))});
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("col_liferate_techs"));
			text::close_layout_box(contents, box);
		}
	}
};

class province_population : public province_population_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("provinceview_totalpop"));
		text::close_layout_box(contents, box);
	}
};


class province_rgoworkers : public province_rgo_workers_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_single_sub_box(state, contents, box, std::string_view("provinceview_employment"), text::variable_type::value,
				std::string_view(""));
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("production_factory_employeecount_tooltip"));
		// TODO - list the workers that are used to calculate the value of the above thing here
		text::localised_format_box(state, contents, box, std::string_view("base_rgo_size"));
		text::add_to_layout_box(state, contents, box, std::string_view("UwU"));
		text::close_layout_box(contents, box);
	}
};

class province_rgo : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, retrieve<dcon::province_id>(state, parent));
		frame = fat_id.get_rgo().get_icon();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		auto rgo = state.world.province_get_rgo(prov);
		text::add_line(state, contents, state.world.commodity_get_name(rgo));
	}
};

class province_close_button : public generic_close_button {
public:
	void button_action(sys::state& state) noexcept override {
		state.map_state.set_selected_province(dcon::province_id{});
		generic_close_button::button_action(state);
	}
};

class province_pop_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id province_id = Cyto::any_cast<dcon::province_id>(payload);

			if(state.ui_state.population_subwindow != nullptr) {
				Cyto::Any fl_payload = pop_list_filter(province_id);
				state.ui_state.population_subwindow->impl_set(state, fl_payload);
				if(state.ui_state.topbar_subwindow != nullptr)
					state.ui_state.topbar_subwindow->set_visible(state, false);
				state.ui_state.topbar_subwindow = state.ui_state.population_subwindow;
				state.ui_state.population_subwindow->set_visible(state, true);
				state.ui_state.root->move_child_to_front(state.ui_state.population_subwindow);
				// ui_state.population_subwindow->impl_get(*this, fl_payload);
			}
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id province_id = Cyto::any_cast<dcon::province_id>(payload);

			text::substitution_map sub_map;
			text::add_to_substitution_map(sub_map, text::variable_type::loc, state.world.province_get_name(province_id));

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("pw_open_popscreen"), sub_map);
			text::close_layout_box(contents, box);
		}
	}
};

class province_terrain_image : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id province_id = Cyto::any_cast<dcon::province_id>(payload);

			auto fat_id = dcon::fatten(state.world, province_id);
			auto terrain_id = fat_id.get_terrain().id;
			auto terrain_image = state.province_definitions.terrain_to_gfx_map[terrain_id];
			if(base_data.get_element_type() == element_type::image) {
				base_data.data.image.gfx_object = terrain_image;
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id province_id = Cyto::any_cast<dcon::province_id>(payload);

			auto fat_id = dcon::fatten(state.world, province_id);
			auto name = fat_id.get_terrain().get_name();
			if(bool(name)) {
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, name), text::text_color::yellow);
				text::close_layout_box(contents, box);
			}
			auto mod_id = fat_id.get_terrain().id;
			if(bool(mod_id))
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
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id p = Cyto::any_cast<dcon::province_id>(payload);

			text::substitution_map sub_map{};
			text::add_to_substitution_map(sub_map, text::variable_type::value, text::fp_two_places{ state.world.state_instance_get_flashpoint_tension(state.world.province_get_state_membership(p)) });

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("flashpoint_tension"), sub_map);
			text::close_layout_box(contents, box);
		}
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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("pv_controller"));
		text::add_space_to_layout_box(state, contents, box);

		dcon::province_id province_id = retrieve<dcon::province_id>(state, parent);
		auto prov_fat = dcon::fatten(state.world, province_id);
		dcon::nation_id controller_id = prov_fat.get_province_control_as_province().get_nation().id;
		if(bool(controller_id)) {
			auto controller_name = state.world.nation_get_name(controller_id);
			text::add_to_layout_box(state, contents, box, controller_name);
		} else {
			auto rebel_faction_id = prov_fat.get_province_rebel_control_as_province().get_rebel_faction();
			text::add_to_layout_box(state, contents, box, rebel::rebel_name(state, rebel_faction_id));
		}
		text::close_layout_box(contents, box);
	}
};

class province_colony_button : public standard_state_instance_button {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		frame = 1;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);
			disabled = !command::can_upgrade_colony_to_state(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);
			command::upgrade_colony_to_state(state, state.local_player_nation, content);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto state_instance_id = any_cast<dcon::state_instance_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("pw_colony"));
			text::add_divider_to_layout_box(state, contents, box);

			text::substitution_map sub1{};
			text::add_to_substitution_map(sub1, text::variable_type::num,
					text::fp_one_place{state.defines.state_creation_admin_limit * 100.f});
			float total_pop = state.world.state_instance_get_demographics(state_instance_id, demographics::total);
			float b_size = province::state_accepted_bureaucrat_size(state, state_instance_id);
			text::add_to_substitution_map(sub1, text::variable_type::curr, text::fp_one_place{(b_size / total_pop) * 100.f});
			text::localised_format_box(state, contents, box, std::string_view("pw_colony_no_state"), sub1);

			text::add_line_break_to_layout_box(state, contents, box);
			text::substitution_map sub2{};
			text::add_to_substitution_map(sub2, text::variable_type::value,
					int32_t(province::colony_integration_cost(state, state_instance_id)));
			text::localised_format_box(state, contents, box, std::string_view("pw_cant_upgrade_to_state"), sub2);

			text::close_layout_box(contents, box);
		}
	}
};

class province_state_name_text_SCH : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id result = Cyto::any_cast<dcon::province_id>(payload);
			set_text(state, text::get_province_state_name(state, result));
		}
	}
};

class province_national_focus_button : public button_element_base {
public:
	int32_t get_icon_frame(sys::state& state) noexcept {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);

			return bool(state.world.state_instance_get_owner_focus(content).id)
								 ? state.world.state_instance_get_owner_focus(content).get_icon() - 1
								 : 0;
		}
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);
			disabled = true;
			state.world.for_each_national_focus([&](dcon::national_focus_id nfid) {
				disabled = command::can_set_national_focus(state, state.local_player_nation, content, nfid) ? false : disabled;
			});
			frame = get_icon_frame(state);
		}
	}

	void button_action(sys::state& state) noexcept override;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);

			dcon::national_focus_fat_id focus = state.world.state_instance_get_owner_focus(content);
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, focus.get_name());
			text::close_layout_box(contents, box);
		}
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
			text::add_line(state, contents, state.world.modifier_get_name(mod.mod_id));
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

class province_window_header : public window_element_base {
private:
	fixed_pop_type_icon* slave_icon = nullptr;
	province_colony_button* colony_button = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_name") {
			return make_element_by_type<province_state_name_text_SCH>(state, id);
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
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "occupation_icon") {
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "occupation_flag") {
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "colony_button") {
			auto ptr = make_element_by_type<province_colony_button>(state, id);
			colony_button = ptr.get();
			return ptr;
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
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id prov_id = any_cast<dcon::province_id>(payload);
			dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
			colony_button->set_visible(state, fat_id.get_is_colonial());
			slave_icon->set_visible(state, fat_id.get_is_slave());
		}
	}
};

class province_send_diplomat_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			state.open_diplomacy(any_cast<dcon::nation_id>(payload));
		}
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
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			populate(state, any_cast<dcon::province_id>(payload));
		}
	}
};

template<economy::province_building_type Value>
class province_building_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, prov_id);
		frame = fat_id.get_building_level(Value);
	}
};
template<economy::province_building_type Value>
class province_building_expand_button : public shift_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			disabled = !command::can_begin_province_building_construction(state, state.local_player_nation, content, Value);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			command::begin_province_building_construction(state, state.local_player_nation, content, Value);
		}
	}
	virtual void button_shift_action(sys::state& state) noexcept override {
		if constexpr(Value == economy::province_building_type::naval_base) {
			button_action(state);
		} else {
			auto pid = retrieve<dcon::province_id>(state, parent);
			auto si = state.world.province_get_state_membership(pid);
			if(si) {
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
					command::begin_province_building_construction(state, state.local_player_nation, p, Value);
				});
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto id = retrieve<dcon::province_id>(state, parent);

		if constexpr(Value == economy::province_building_type::fort) {
			text::add_line_with_condition(state, contents, "fort_build_tt_1", state.world.province_get_nation_from_province_control(id) == state.local_player_nation);
			text::add_line_with_condition(state, contents, "fort_build_tt_2", !military::province_is_under_siege(state, id));

			int32_t current_lvl = state.world.province_get_building_level(id, economy::province_building_type::fort);
			int32_t max_local_lvl = state.world.nation_get_max_building_level(state.local_player_nation, economy::province_building_type::fort);
			int32_t min_build = int32_t(state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_fort));

			text::add_line_with_condition(state, contents, "fort_build_tt_3", (max_local_lvl - current_lvl - min_build > 0), text::variable_type::x, int64_t(current_lvl), text::variable_type::n, int64_t(min_build), text::variable_type::y, int64_t(max_local_lvl));

		} else if constexpr(Value == economy::province_building_type::naval_base) {
			text::add_line_with_condition(state, contents, "fort_build_tt_1", state.world.province_get_nation_from_province_control(id) == state.local_player_nation);
			text::add_line_with_condition(state, contents, "fort_build_tt_2", !military::province_is_under_siege(state, id));
			text::add_line_with_condition(state, contents, "nb_build_tt_1", state.world.province_get_is_coast(id));


			int32_t current_lvl = state.world.province_get_building_level(id, economy::province_building_type::naval_base);
			int32_t max_local_lvl = state.world.nation_get_max_building_level(state.local_player_nation, economy::province_building_type::naval_base);
			int32_t min_build = int32_t(state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_naval_base));

			auto si = state.world.province_get_state_membership(id);
			text::add_line_with_condition(state, contents, "nb_build_tt_2", current_lvl > 0 || !si.get_naval_base_is_taken());

			text::add_line_with_condition(state, contents, "fort_build_tt_3", (max_local_lvl - current_lvl - min_build > 0), text::variable_type::x, int64_t(current_lvl), text::variable_type::n, int64_t(min_build), text::variable_type::y, int64_t(max_local_lvl));

		} if constexpr(Value == economy::province_building_type::railroad) {
			text::add_line_with_condition(state, contents, "fort_build_tt_1", state.world.province_get_nation_from_province_control(id) == state.local_player_nation);
			text::add_line_with_condition(state, contents, "fort_build_tt_2", !military::province_is_under_siege(state, id));

			auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
			text::add_line_with_condition(state, contents, "rr_build_tt_1", (rules & issue_rule::build_railway) != 0);

			int32_t current_rails_lvl = state.world.province_get_building_level(id, economy::province_building_type::railroad);
			int32_t max_local_rails_lvl = state.world.nation_get_max_building_level(state.local_player_nation, economy::province_building_type::railroad);
			int32_t min_build_railroad =int32_t(state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_railroad));

			text::add_line_with_condition(state, contents, "fort_build_tt_3", (max_local_rails_lvl - current_rails_lvl - min_build_railroad > 0), text::variable_type::x, int64_t(current_rails_lvl), text::variable_type::n, int64_t(min_build_railroad), text::variable_type::y, int64_t(max_local_rails_lvl));
		}
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
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			progress = economy::province_building_construction(state, content, Value).progress;
		}
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

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(goods.commodity_type[i]) {
						auto box = text::open_layout_box(contents, 0);
						text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(goods.commodity_type[i]));
						text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
						text::add_to_layout_box(state, contents, box, text::fp_one_place{ cgoods.commodity_amounts[i] });
						text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
						text::add_to_layout_box(state, contents, box, text::fp_one_place{ goods.commodity_amounts[i] });
						text::close_layout_box(contents, box);
					}
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
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);

			expand_button->set_visible(state, !is_being_built(state, content));
			under_construction_icon->set_visible(state, is_being_built(state, content));
			building_progress->set_visible(state, is_being_built(state, content));
			expanding_text->set_visible(state, is_being_built(state, content));
		}
	}
};

class province_invest_railroad_button : public shift_button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			command::begin_province_building_construction(state, state.local_player_nation, content, economy::province_building_type::railroad);
		}
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
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			disabled = !command::can_begin_province_building_construction(state, state.local_player_nation, content,
					economy::province_building_type::railroad);
		}
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
			open_build_foreign_factory(state, state.world.province_get_state_membership(content));
		}
			
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		disabled = true;

		if(!content)
			return;

		for(auto ft : state.world.in_factory_type) {
			if(command::can_begin_factory_building_construction(state, state.local_player_nation,
				state.world.province_get_state_membership(content), ft, false)) {

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
		auto supply = int32_t(military::peacetime_attrition_limit(state, state.local_player_nation, province_id));
		set_text(state, std::to_string(supply));
	}

	// TODO: needs an explanation of where the value comes from
};

class rr_investment_progress : public progress_bar {
public:
	void on_update(sys::state& state) noexcept {
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
		} else if(name == "rally_land_icon"
			|| name == "rallypoint_merge_icon"
			|| name == "rally_naval_icon"
			|| name == "rallypoint_merge_icon_naval"
			|| name == "rallypoint_checkbox"
			|| name == "rallypoint_merge_checkbox"
			|| name == "rallypoint_checkbox_naval"
			|| name == "rallypoint_merge_checkbox_naval") {

			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
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
		auto employment_ratio = state.world.province_get_rgo_employment(province);
		frame = int32_t(10.f * employment_ratio);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		auto owner = state.world.province_get_nation_from_province_ownership(prov_id);
		auto max_emp = economy::rgo_max_employment(state, owner, prov_id);
		auto employment_ratio = state.world.province_get_rgo_employment(prov_id);

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
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			auto box = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box, std::string_view("provinceview_crimefight"),
					text::variable_type::value, text::fp_one_place{province::crime_fighting_efficiency(state, content) * 100});
			text::close_layout_box(contents, box);
		}
	}
	*/
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
		// Not sure if this is the right key, but looking through the CSV files, this is the only one with a value you can
		// substitute.
		auto box = text::open_layout_box(contents, 0);
		text::localised_single_sub_box(state, contents, box, std::string_view("avg_mil_on_map"), text::variable_type::value,
				text::fp_one_place{province::revolt_risk(state, content) * 100});
		text::close_layout_box(contents, box);
	}
};

class province_rgo_employment_percent_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::format_percentage(state.world.province_get_rgo_employment(province_id), 1));
	}
};

class province_migration_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto internal = province::monthly_net_pop_internal_migration(state, province_id);
		auto external = province::monthly_net_pop_external_migration(state, province_id);
		set_text(state, text::prettify(int32_t(internal + external)));
	}
};

class province_pop_growth_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		set_text(state, text::prettify(int32_t(province::monthly_net_pop_growth(state, province_id))));
	}
};

class province_army_size_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto built = military::regiments_created_from_province(state, province_id);
		auto max_possible = military::regiments_max_possible_from_province(state, province_id);
		set_text(state, text::format_ratio(built, max_possible));
	}
};



class province_view_statistics : public window_element_base {
private:
	culture_piechart<dcon::province_id>* culture_chart = nullptr;
	ideology_piechart<dcon::province_id>* ideology_chart = nullptr;
	workforce_piechart<dcon::province_id>* workforce_chart = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<province_owner_rgo>(state, id);
		} else if(name == "open_popscreen") {
			return make_element_by_type<province_pop_button>(state, id);
		} else if(name == "total_population") {
			return make_element_by_type<province_population>(state, id);
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
		} else if(name == "core_icons") {
			return make_element_by_type<province_core_flags>(state, id);
		} else if(name == "supply_limit") {
			return make_element_by_type<province_supply_limit_text>(state, id);
		} else if(name == "crime_icon") {
			return make_element_by_type<province_crime_icon>(state, id);
		} else if(name == "crime_name") {
			return make_element_by_type<province_crime_name_text>(state, id);
		} else if(name == "crimefight_percent") {
			return make_element_by_type<province_crime_fighting_text>(state, id);
		} else if(name == "rebel_percent") {
			return make_element_by_type<province_rebel_percent_text>(state, id);
		} else if(name == "employment_ratio") {
			return make_element_by_type<province_rgo_employment_progress_icon>(state, id);
		} else if(name == "rgo_population") {
			return make_element_by_type<province_rgoworkers>(state, id);
		} else if(name == "rgo_percent") {
			return make_element_by_type<province_rgo_employment_percent_text>(state, id);
		} else if(name == "produced") {
			return make_element_by_type<province_owner_goods_produced_text>(state, id);
		} else if(name == "income") {
			return make_element_by_type<province_owner_income_text>(state, id);
		} else if(name == "growth") {
			return make_element_by_type<province_pop_growth_text>(state, id);
		} else if(name == "migration") {
			return make_element_by_type<province_migration_text>(state, id);
		} else if(name == "build_factory_button") {
			return make_element_by_type<province_build_new_factory>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		if(bool(nation_id) && nation_id.id == state.local_player_nation) {
			culture_chart->impl_on_update(state);
			workforce_chart->impl_on_update(state);
			ideology_chart->impl_on_update(state);
			set_visible(state, true);
		} else {
			set_visible(state, false);
		}
	}
};

class land_rally_point : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		frame = (state.world.province_get_land_rally_point(retrieve<dcon::province_id>(state, parent))) ? 1 : 0;
	}
	void button_action(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		command::set_rally_point(state, state.local_player_nation, p, false,  !state.world.province_get_land_rally_point(p));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "rally_point_enable_info");
	}
};

class naval_rally_point : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		frame = state.world.province_get_naval_rally_point(p) ? 1 : 0;
		disabled = !(state.world.province_get_is_coast(p));
	}
	void button_action(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		command::set_rally_point(state, state.local_player_nation, p, true, !state.world.province_get_naval_rally_point(p));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "rally_point_enable_info");
	}
};


class province_view_buildings : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		{
			auto ptr = make_element_by_type<province_building_window<economy::province_building_type::fort>>(state, "building");
			ptr->base_data.position.y = 1;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<province_building_window<economy::province_building_type::naval_base>>(state, "building");
			ptr->base_data.position.y = 36;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<province_building_window<economy::province_building_type::railroad>>(state, "building");
			ptr->base_data.position.y = 71;
			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "army_size") {
			return make_element_by_type<province_army_size_text>(state, id);
		} else if(name == "rallypoint_checkbox") {
			return make_element_by_type< land_rally_point>(state, id);
		} else if(name == "rallypoint_checkbox_naval") {
			return make_element_by_type< naval_rally_point>(state, id);
		} else if(name == "rallypoint_merge_icon"
			|| name == "rallypoint_merge_icon_naval"
			|| name == "rallypoint_merge_checkbox"
			|| name == "rallypoint_merge_checkbox_naval"
			|| name == "army_progress"
			|| name == "army_progress_overlay"
			|| name == "navy_progress"
			|| name == "navy_progress_overlay"
			|| name == "army_text"
			|| name == "navy_text"
			|| name == "build_army"
			|| name == "build_navy"
			|| name == "navy_icon") {

			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		if(bool(nation_id) && nation_id.id == state.local_player_nation) {
			set_visible(state, true);
		} else {
			set_visible(state, false);
		}
	}
};

class province_protectorate_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		command::finish_colonization(state, state.local_player_nation, content);
		state.ui_state.province_window->set_visible(state, false);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_finish_colonization(state, state.local_player_nation, content);
	}
};

class province_withdraw_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		command::abandon_colony(state, state.local_player_nation, content);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_abandon_colony(state, state.local_player_nation, content);
	}
};

class colony_invest_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		command::invest_in_colony(state, state.local_player_nation, content);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		disabled = !command::can_invest_in_colony(state, state.local_player_nation, content);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto province = retrieve<dcon::province_id>(state, parent);
		auto sdef = state.world.province_get_state_from_abstract_state_membership(province);
		auto col = retrieve<dcon::colonization_id>(state, parent);

		if(!province::is_colonizing(state, state.local_player_nation, sdef)) {
			text::add_line(state, contents, "col_start_title");
			text::add_line_break_to_layout(state, contents);

			text::add_line_with_condition(state, contents, "col_start_1", state.world.state_definition_get_colonization_stage(sdef) <= uint8_t(1));
			text::add_line_with_condition(state, contents, "col_start_2", state.world.nation_get_rank(state.local_player_nation) <= uint16_t(state.defines.colonial_rank), text::variable_type::x, uint16_t(state.defines.colonial_rank));
			text::add_line_with_condition(state, contents, "col_start_3", state.crisis_colony != sdef);
		
			bool war_participant = false;
			for(auto par : state.world.war_get_war_participant(state.crisis_war)) {
				if(par.get_nation() == state.local_player_nation)
					war_participant = true;
			}
			text::add_line_with_condition(state, contents, "col_start_4", !war_participant);

			float max_life_rating = -1.0f;
			for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
				if(!p.get_province().get_nation_from_province_ownership()) {
					max_life_rating = std::max(max_life_rating, float(p.get_province().get_life_rating()));
				}
			}

			text::add_line_with_condition(state, contents, "col_start_5",
				state.defines.colonial_liferating + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::colonial_life_rating) <= max_life_rating,
				text::variable_type::x, int64_t(state.defines.colonial_liferating + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::colonial_life_rating)),
				text::variable_type::y, int64_t(max_life_rating));
			text::add_line(state, contents, "col_start_6", text::variable_type::x, int64_t(state.defines.colonial_liferating), 15);
			active_modifiers_description(state, contents, state.local_player_nation, 15, sys::national_mod_offsets::colonial_life_rating, false);

			auto colonizers = state.world.state_definition_get_colonization(sdef);
			auto num_colonizers = colonizers.end() - colonizers.begin();

			text::add_line_with_condition(state, contents, "col_start_7", num_colonizers < 4);
		
			bool adjacent = [&]() {
				for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
					if(!p.get_province().get_nation_from_province_ownership()) {
						for(auto adj : p.get_province().get_province_adjacency()) {
							auto indx = adj.get_connected_provinces(0) != p.get_province() ? 0 : 1;
							auto o = adj.get_connected_provinces(indx).get_nation_from_province_ownership();
							if(o == state.local_player_nation)
								return true;
							if(o.get_overlord_as_subject().get_ruler() == state.local_player_nation)
								return true;
						}
					}
				}
				return false;
			}();

			bool reachable_by_sea = false;

			dcon::province_id coastal_target = [&]() {
				for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
					if(!p.get_province().get_nation_from_province_ownership()) {
						if(p.get_province().get_is_coast())
							return p.get_province().id;
					}
				}
				return dcon::province_id{};
			}();

			if(!adjacent && coastal_target && state.world.nation_get_central_ports(state.local_player_nation) != 0) {
				for(auto p : state.world.nation_get_province_ownership(state.local_player_nation)) {
					if(auto nb_level = p.get_province().get_building_level(economy::province_building_type::naval_base); nb_level > 0 && p.get_province().get_nation_from_province_control() == state.local_player_nation) {
						if(province::direct_distance(state, p.get_province(), coastal_target) <= province::world_circumference * 0.075f * nb_level) {
							reachable_by_sea = true;
							break;
						}
					}
				}
			}

			text::add_line_with_condition(state, contents, "col_start_8", adjacent || reachable_by_sea);

			auto free_points = nations::free_colonial_points(state, state.local_player_nation);
			auto required_points = int32_t(state.defines.colonization_interest_cost_initial + (adjacent ? state.defines.colonization_interest_cost_neighbor_modifier : 0.0f));
			
			text::add_line_with_condition(state, contents, "col_start_9", free_points > required_points, text::variable_type::x, required_points);
		} else {
			text::add_line(state, contents, "col_invest_title");
			text::add_line_break_to_layout(state, contents);

			text::add_line_with_condition(state, contents, "col_invest_1", state.world.nation_get_rank(state.local_player_nation) <= uint16_t(state.defines.colonial_rank), text::variable_type::x, uint16_t(state.defines.colonial_rank));

			text::add_line_with_condition(state, contents, "col_invest_2", state.crisis_colony != sdef);

			bool war_participant = false;
			for(auto par : state.world.war_get_war_participant(state.crisis_war)) {
				if(par.get_nation() == state.local_player_nation)
					war_participant = true;
			}
			text::add_line_with_condition(state, contents, "col_invest_3", !war_participant);

			auto crange = state.world.state_definition_get_colonization(sdef);
			auto last_investment = state.world.colonization_get_last_investment(col);

			if(crange.end() - crange.begin() <= 1) { // no competition
				text::add_line_break_to_layout(state, contents);
				text::add_line(state, contents, "col_invest_4", text::variable_type::x, last_investment + int32_t(state.defines.colonization_days_for_initial_investment));
			} else {
				text::add_line_with_condition(state, contents, "col_invest_5", last_investment + int32_t(state.defines.colonization_days_between_investment) <= state.current_date,
					text::variable_type::x, int32_t(state.defines.colonization_days_between_investment),
					text::variable_type::y, last_investment + int32_t(state.defines.colonization_days_between_investment));

				auto free_points = nations::free_colonial_points(state, state.local_player_nation);
				int32_t point_cost = 0;
				if(state.world.state_definition_get_colonization_stage(sdef) == 1) {
					point_cost = int32_t(state.defines.colonization_interest_cost);
				} else if(state.world.colonization_get_level(col) <= 4) {
					point_cost = int32_t(state.defines.colonization_influence_cost);
				} else {
					point_cost = int32_t(state.defines.colonization_extra_guard_cost * (state.world.colonization_get_level(col) - 4) + state.defines.colonization_influence_cost);
				}

				text::add_line_with_condition(state, contents, "col_invest_6", free_points >= point_cost, text::variable_type::x, point_cost);
			}
		}
	}
};

class level_entry : public listbox_row_element_base<int8_t> {
private:
	image_element_base* progressicon = nullptr;
	button_element_base* investbutton = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "progress_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			progressicon = ptr.get();
			return ptr;
		} else if(name == "progress_button") {
			auto ptr = make_element_by_type<colony_invest_button>(state, id);
			investbutton = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(content >= 0) {
			progressicon->frame = content;
			investbutton->set_visible(state, false);
			progressicon->set_visible(state, true);
		} else {
			investbutton->frame = -(content + 1);
			investbutton->set_visible(state, true);
			progressicon->set_visible(state, false);
		}
	}
};

class colonisation_listbox : public overlapping_listbox_element_base<level_entry, int8_t> {
protected:
	std::string_view get_row_element_name() override {
		return "level_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::colonization_id>(state, parent);
		
		row_contents.clear();

		if(!content) {
			row_contents.push_back(int8_t(-1));
		} else {
			auto province = retrieve<dcon::province_id>(state, parent);
			auto stage = state.world.state_definition_get_colonization_stage(state.world.province_get_state_from_abstract_state_membership(province));

			auto fat_colony = dcon::fatten(state.world, content);

			if(stage == 3) {
				row_contents.push_back(int8_t(0));
				row_contents.push_back(int8_t(1));
				row_contents.push_back(int8_t(2));
				row_contents.push_back(int8_t(3));
				row_contents.push_back(int8_t(4));
			} else if(fat_colony.get_colonizer() == state.local_player_nation) {
				int8_t i = 0;
				for(; i < 4 && i < int32_t(fat_colony.get_level()); ++i) {
					row_contents.push_back(i);
				}
				row_contents.push_back(int8_t(-(i + 1)));
			} else {
				for(int8_t i = 0; i < 5 && i < int32_t(fat_colony.get_level()); ++i) {
					row_contents.push_back(i);
				}
			}
		}
		update(state);
	}
};

class colonization_level_number : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto col = retrieve<dcon::colonization_id>(state, parent);
		auto level = state.world.colonization_get_level(col);
		if(level < 5)
			set_text(state, "");
		else
			set_text(state, std::to_string(level - 4));
	}
};

class colonist_entry : public listbox_row_element_base<dcon::colonization_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "levels") {
			return make_element_by_type<colonisation_listbox>(state, id);
		} else if(name == "progress_counter") {
			return make_element_by_type<colonization_level_number>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::national_identity_id>()) {
			if(content)
				payload.emplace<dcon::national_identity_id>(dcon::fatten(state.world, content).get_colonizer().get_identity_from_identity_holder().id);
			else
				payload.emplace<dcon::national_identity_id>(state.world.nation_get_identity_from_identity_holder(state.local_player_nation));
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::colonization_id>::get(state, payload);
	}
};

class colonist_listbox : public listbox_element_base<colonist_entry, dcon::colonization_id> {
protected:
	std::string_view get_row_element_name() override {
		return "colonist_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		
		auto prov = retrieve<dcon::province_id>(state, parent);
		auto fat_def = dcon::fatten(state.world, state.world.province_get_state_from_abstract_state_membership(prov));

		row_contents.clear();

		bool found_player = false;
		int32_t existing_colonizers = 0;
		for(auto colony : fat_def.get_colonization()) {
			if(colony.get_colonizer().id == state.local_player_nation) {
				found_player = true;
			}
			row_contents.push_back(colony.id);
			++existing_colonizers;
		}

		if(!found_player && (existing_colonizers == 0 || (fat_def.get_colonization_stage() == 1 && existing_colonizers < 4))) {
			row_contents.push_back(dcon::colonization_id{});
		}

		update(state);
	}
};

class province_colonisation_temperature : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		progress = dcon::fatten(state.world, content).get_definition().get_colonization_temperature();
	}
};

class province_window_colony : public window_element_base {
private:
	simple_text_element_base* population_box = nullptr;
	culture_piechart<dcon::province_id>* culture_chart = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "total_population") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			population_box = ptr.get();
			return ptr;
		} else if(name == "culture_chart") {
			auto ptr = make_element_by_type<culture_piechart<dcon::province_id>>(state, id);
			culture_chart = ptr.get();
			return ptr;
		} else if(name == "goods_type") {
			return make_element_by_type<province_rgo>(state, id);
		} else if(name == "colonize_button") {
			return make_element_by_type<province_protectorate_button>(state, id);
		} else if(name == "withdraw_button") {
			return make_element_by_type<province_withdraw_button>(state, id);
		} else if(name == "colonist_list") {
			return make_element_by_type<colonist_listbox>(state, id);
		} else if(name == "crisis_temperature") {
			return make_element_by_type<province_colonisation_temperature>(state, id);
		} else if(name == "rally_land_icon"
			|| name == "rallypoint_merge_icon"
			|| name == "rally_naval_icon"
			|| name == "rallypoint_merge_icon_naval"
			|| name == "rallypoint_checkbox"
			|| name == "rallypoint_merge_checkbox"
			|| name == "rallypoint_checkbox_naval"
			|| name == "rallypoint_merge_checkbox_naval") {

			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto prov_id = retrieve<dcon::province_id>(state, parent);
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		if(bool(nation_id)) {
			set_visible(state, false);
		} else {
			auto total_pop = state.world.province_get_demographics(prov_id, demographics::total);
			population_box->set_text(state, text::prettify(int32_t(total_pop)));
			culture_chart->on_update(state);
			set_visible(state, true);
		}
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
	element_base* nf_win = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		state.ui_state.province_window = this;
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<province_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "province_view_header") {
			auto ptr = make_element_by_type<province_window_header>(state, id);
			header_window = ptr.get();
			return ptr;
		} else if(name == "province_other") {
			auto ptr = make_element_by_type<province_view_foreign_details>(state, id);
			ptr->set_visible(state, false);
			foreign_details_window = ptr.get();
			return ptr;
		} else if(name == "province_colony") {
			auto ptr = make_element_by_type<province_window_colony>(state, id);
			ptr->set_visible(state, false);
			colony_window = ptr.get();
			return ptr;
		} else if(name == "province_statistics") {
			auto ptr = make_element_by_type<province_view_statistics>(state, id);
			local_details_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "province_buildings") {
			auto ptr = make_element_by_type<province_view_buildings>(state, id);
			local_buildings_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "national_focus_window") {
			auto ptr = make_element_by_type<national_focus_window>(state, id);
			ptr->set_visible(state, false);
			nf_win = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(active_province);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			dcon::nation_id nid = dcon::fatten(state.world, active_province).get_nation_from_province_ownership();
			payload.emplace<dcon::nation_id>(nid);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::state_instance_id>()) {
			dcon::state_instance_id sid = dcon::fatten(state.world, active_province).get_state_membership();
			payload.emplace<dcon::state_instance_id>(sid);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void set_active_province(sys::state& state, dcon::province_id map_province) {
		if(bool(map_province)) {
			active_province = map_province;

			header_window->impl_on_update(state);
			foreign_details_window->impl_on_update(state);
			local_details_window->impl_on_update(state);
			local_buildings_window->impl_on_update(state);
			colony_window->impl_on_update(state);

			if(!is_visible())
				set_visible(state, true);
			else
				impl_on_update(state);
		} else {
			set_visible(state, false);
		}
	}

	friend class province_national_focus_button;
};

void province_national_focus_button::button_action(sys::state& state) noexcept {
	auto province_window = static_cast<province_view_window*>(state.ui_state.province_window);
	province_window->nf_win->set_visible(state, !province_window->nf_win->is_visible());
}

} // namespace ui
