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
#include <string_view>

namespace ui {

class province_liferating : public province_liferating_progress_bar {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id prov_id = Cyto::any_cast<dcon::province_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box, std::string_view("provinceview_liferating"), text::variable_type::value, text::fp_one_place{float(state.world.province_get_life_rating(prov_id))});
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("col_liferate_techs"));
			text::close_layout_box(contents, box);
		}
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
				text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, name), text::text_color::yellow);
				text::close_layout_box(contents, box);
			}
			auto mod_id = fat_id.get_terrain().id;
			if(bool(mod_id))
				modifier_description(state, contents, mod_id);
		}
	}
};

class province_flashpoint_indicator : public standard_province_icon {
public:
	void on_update(sys::state& state) noexcept override {
		standard_province_icon::on_update(state);
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id province_id = Cyto::any_cast<dcon::province_id>(payload);
			auto fat_id = dcon::fatten(state.world, province_id);
			auto tension = fat_id.get_state_membership().get_flashpoint_tension();
			set_visible(state, tension > 0.f);
		}
	}
};

class province_controller_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id province_id = Cyto::any_cast<dcon::province_id>(payload);

			auto fat_id = dcon::fatten(state.world, province_id);
			auto controller_id = fat_id.get_province_control_as_province().get_nation();
			return controller_id.get_identity_from_identity_holder().id;
		}
		return dcon::national_identity_id{};
	}

	void on_update(sys::state& state) noexcept override {
		flag_button::on_update(state);

		auto nation = get_current_nation(state);
		if(bool(nation)) {
			flag_button::set_current_nation(state, get_current_nation(state));
		}
		set_visible(state, bool(nation));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto province_id = any_cast<dcon::province_id>(payload);

			auto prov_fat = dcon::fatten(state.world, province_id);
			auto controller_name = prov_fat.get_province_control_as_province().get_nation().get_name();
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("pv_controller"));
			text::add_space_to_layout_box(contents, state, box);
			text::add_to_layout_box(contents, state, box, controller_name);
			text::close_layout_box(contents, box);
		}
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
			text::add_to_substitution_map(sub1, text::variable_type::num, text::fp_one_place{state.defines.state_creation_admin_limit * 100.f});
			float total_pop = state.world.state_instance_get_demographics(state_instance_id, demographics::total);
			float b_size = province::state_accepted_bureaucrat_size(state, state_instance_id);
			text::add_to_substitution_map(sub1, text::variable_type::curr, text::fp_one_place{(b_size / total_pop) * 100.f});
			text::localised_format_box(state, contents, box, std::string_view("pw_colony_no_state"), sub1);

			text::add_line_break_to_layout_box(contents, state, box);
			text::substitution_map sub2{};
			text::add_to_substitution_map(sub2, text::variable_type::value, int32_t(province::colony_integration_cost(state, state_instance_id)));
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

			return bool(state.world.state_instance_get_owner_focus(content).id) ? state.world.state_instance_get_owner_focus(content).get_icon() - 1 : 0;
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
			text::add_to_layout_box(contents, state, box, focus.get_name());
			text::close_layout_box(contents, box);
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
			return make_element_by_type<province_state_name_text_SCH>(state, id);
		} else if(name == "province_name") {
			return make_element_by_type<generic_name_text<dcon::province_id>>(state, id);
		} else if(name == "prov_terrain") {
			return make_element_by_type<province_terrain_image>(state, id);
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
class province_building_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::province_id prov_id) noexcept override {
		switch(Value) {
		case economy::province_building_type::railroad: {
			auto fat_id = dcon::fatten(state.world, prov_id);
			return fat_id.get_railroad_level();
		}
		case economy::province_building_type::fort: {
			auto fat_id = dcon::fatten(state.world, prov_id);
			return fat_id.get_fort_level();
		}
		case economy::province_building_type::naval_base: {
			auto fat_id = dcon::fatten(state.world, prov_id);
			return fat_id.get_naval_base_level();
		}
		}
		return 0;
	}
};
template<economy::province_building_type Value>
class province_building_expand_button : public button_element_base {
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
};
template<economy::province_building_type Value>
class province_building_window : public window_element_base {
	button_element_base* expand_button = nullptr;
	image_element_base* under_construction_icon = nullptr;
	element_base* building_progress = nullptr;
	element_base* expanding_text = nullptr;

	std::string get_icon_name() noexcept {
		switch(Value) {
		case economy::province_building_type::railroad:
			return "build_icon0";
		case economy::province_building_type::fort:
			return "build_icon1";
		case economy::province_building_type::naval_base:
			return "build_icon2";
		}
		return "???";
	}

	std::string get_description(sys::state& state) noexcept {
		switch(Value) {
		case economy::province_building_type::railroad:
			return text::produce_simple_string(state, "railroad");
		case economy::province_building_type::fort:
			return text::produce_simple_string(state, "fort");
		case economy::province_building_type::naval_base:
			return text::produce_simple_string(state, "naval_base");
		}
		return "???";
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
			ptr->set_text(state, get_description(state));
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

class province_invest_railroad_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			command::begin_province_building_construction(state, state.local_player_nation, content, economy::province_building_type::railroad);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			disabled = !command::can_begin_province_building_construction(state, state.local_player_nation, content, economy::province_building_type::railroad);
		}
	}
};

class province_view_foreign_details : public window_element_base {
private:
	flag_button* country_flag_button = nullptr;
	culture_piechart<dcon::province_id>* culture_chart = nullptr;
	ideology_piechart<dcon::province_id>* ideology_chart = nullptr;
	workforce_piechart<dcon::province_id>* workforce_chart = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
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
			return make_element_by_type<province_population_text>(state, id);
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
			return make_element_by_type<province_rgo_icon>(state, id);
		} else if(name == "build_icon_fort") {
			return make_element_by_type<province_building_icon<economy::province_building_type::fort>>(state, id);
		} else if(name == "build_icon_navalbase") {
			return make_element_by_type<province_building_icon<economy::province_building_type::naval_base>>(state, id);
		} else if(name == "build_icon_infra") {
			return make_element_by_type<province_building_icon<economy::province_building_type::railroad>>(state, id);
		} else if(name == "infra_progress_win") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "invest_build_infra") {
			return make_element_by_type<province_invest_railroad_button>(state, id);
		} else if(name == "invest_factory_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->disabled = true;
			return ptr;
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
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto prov_id = any_cast<dcon::province_id>(payload);

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
			return make_element_by_type<province_rgo_icon>(state, id);
		} else if(name == "open_popscreen") {
			return make_element_by_type<province_pop_button>(state, id);
		} else if(name == "total_population") {
			return make_element_by_type<province_population_text>(state, id);
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
			return make_element_by_type<province_rgo_workers_text>(state, id);
		} else if(name == "rgo_percent") {
			return make_element_by_type<province_rgo_employment_percent_text>(state, id);
		} else if(name == "produced") {
			return make_element_by_type<province_goods_produced_text>(state, id);
		} else if(name == "income") {
			return make_element_by_type<province_income_text>(state, id);
		} else if(name == "growth") {
			return make_element_by_type<province_pop_growth_text>(state, id);
		} else if(name == "migration") {
			return make_element_by_type<province_migration_text>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto prov_id = any_cast<dcon::province_id>(payload);
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
		if(name == "army_progress") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "army_progress_overlay") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "navy_progress") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "navy_progress_overlay") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "army_text") { // this seems to be unused
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "navy_text") { // this seems to be unused
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "army_size") {
			return make_element_by_type<province_army_size_text>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto prov_id = any_cast<dcon::province_id>(payload);
			dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
			auto nation_id = fat_id.get_nation_from_province_ownership();
			if(bool(nation_id) && nation_id.id == state.local_player_nation) {
				set_visible(state, true);
			} else {
				set_visible(state, false);
			}
		}
	}
};

class province_colony_rgo_icon : public image_element_base {
public: // goto hell;
	    // Seriously hate this code, just no, this is awful and shouldnt be needed
	    // but i refuse to loose my sanity to something to assining
	dcon::text_sequence_id rgo_name;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, rgo_name);
		text::close_layout_box(contents, box);
	}
};

class province_protectorate_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);

			command::finish_colonization(state, state.local_player_nation, content);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			disabled = !command::can_finish_colonization(state, state.local_player_nation, content);
		}
	}
};

class province_withdraw_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);

			command::abandon_colony(state, state.local_player_nation, content);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			disabled = !command::can_abandon_colony(state, state.local_player_nation, content);
		}
	}
};

class colony_nation_id_pair {
public:
	dcon::colonization_id col_id{};
	dcon::nation_id player_id{};
};

class colony_invest_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);

			command::invest_in_colony(state, state.local_player_nation, content);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);

			disabled = !command::can_invest_in_colony(state, state.local_player_nation, content);
		}
	}
};

class level_entry : public listbox_row_element_base<uint8_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "progress_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = content;
			return ptr;
		} else if(name == "progress_button") {
			return make_element_by_type<colony_invest_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class colonisation_listbox : public overlapping_listbox_element_base<level_entry, uint8_t> {
protected:
	std::string_view get_row_element_name() override {
		return "level_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = dcon::colonization_id{};
		parent->impl_get(state, payload);
		auto content = any_cast<dcon::colonization_id>(payload);
		auto fat_colony = dcon::fatten(state.world, content);
		for(uint8_t i = 0; i < fat_colony.get_level(); ++i) {
			row_contents.push_back(i);
		}

		if(row_contents.size() == 0) {
			row_contents.push_back(1);
		}

		update(state);
	}
};

class colonist_entry : public listbox_row_element_base<colony_nation_id_pair> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "levels") {
			// return make_element_by_type<level_entry>(state, id);
			return make_element_by_type<colonisation_listbox>(state, id);
		} else if(name == "progress_counter") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::national_identity_id>()) {
			if(!bool(content.col_id)) {
				payload.emplace<dcon::national_identity_id>(dcon::fatten(state.world, content.player_id).get_identity_from_identity_holder().id);
			} else if(bool(content.col_id)) {
				payload.emplace<dcon::national_identity_id>(dcon::fatten(state.world, content.col_id).get_colonizer().get_identity_from_identity_holder().id);
			}
			return message_result::consumed;
		} else if(payload.holds_type<dcon::colonization_id>()) {
			payload.emplace<dcon::colonization_id>(content.col_id);
		}
		return message_result::unseen;
	}
};

class colonist_listbox : public listbox_element_base<colonist_entry, colony_nation_id_pair> {
protected:
	std::string_view get_row_element_name() override {
		return "colonist_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);
			auto fat_def = dcon::fatten(state.world, content).get_definition();

			row_contents.clear();

			for(auto colony : fat_def.get_colonization()) {
				colony_nation_id_pair test;
				test.col_id = colony;
				test.player_id = dcon::nation_id{0};
				row_contents.push_back(test);
			}

			colony_nation_id_pair aa;
			aa.col_id = dcon::colonization_id{0};
			aa.player_id = state.local_player_nation;
			row_contents.push_back(aa);

			update(state);
		}
	}
};

class province_colonisation_temperature : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = dcon::state_instance_id{};
		parent->impl_get(state, payload);
		auto content = any_cast<dcon::state_instance_id>(payload);
		progress = dcon::fatten(state.world, content).get_definition().get_colonization_temperature();
	}
};

class province_window_colony : public window_element_base {
private:
	province_colony_rgo_icon* rgo_icon = nullptr;
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
			auto ptr = make_element_by_type<province_colony_rgo_icon>(state, id);
			rgo_icon = ptr.get();
			return ptr;
		} else if(name == "colonize_button") {
			return make_element_by_type<province_protectorate_button>(state, id);
		} else if(name == "withdraw_button") {
			return make_element_by_type<province_withdraw_button>(state, id);
		} else if(name == "colonist_list") {
			// TODO - Listbox
			return make_element_by_type<colonist_listbox>(state, id);
		} else if(name == "crisis_temperature") {
			return make_element_by_type<province_colonisation_temperature>(state, id);
		} else if(name == "crisis_temperature_frame") {
			return make_element_by_type<image_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto prov_id = any_cast<dcon::province_id>(payload);
			dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
			auto nation_id = fat_id.get_nation_from_province_ownership();
			if(bool(nation_id)) {
				set_visible(state, false);
			} else {
				rgo_icon->frame = fat_id.get_rgo().get_icon();
				rgo_icon->rgo_name = fat_id.get_rgo().get_name();
				auto total_pop = state.world.province_get_demographics(prov_id, demographics::total);
				population_box->set_text(state, text::prettify(int32_t(total_pop)));
				culture_chart->on_update(state);
				set_visible(state, true);
			}
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
